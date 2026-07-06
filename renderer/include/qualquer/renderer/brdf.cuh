#pragma once

/**
 * @file brdf.cuh
 * @brief BRDF building blocks for the renderer layer.
 */

// Specular primitives receive alpha (= roughness^2).
// EON diffuse / E_FON / CLTC receive linear roughness r in [0,1].
//
// All directions are tangent-space, +Z = normal, oriented away from surface.
//
// Specular: ported from Himalaya brdf.glsl + pt_common.glsl (GLSL -> CUDA).
// EON + CLTC: ported from EON-diffuse main.glsl (GLSL -> CUDA).

#include <cuda_runtime.h>

#include <qualquer/renderer/math_utils.cuh>

namespace qualquer::renderer {

// ---- Constants --------------------------------------------------------------

constexpr float kPi       = 3.14159265358979323846f;
constexpr float kTwoPi    = 6.28318530717958647692f;
constexpr float kInvPi    = 0.31830988618379067154f;
constexpr float kInvTwoPi = 0.15915494309189533577f;
constexpr float kEpsilon  = 1.0e-7f;

// ---- Specular: F0 -----------------------------------------------------------

/**
 * @brief Normal-incidence reflectance from base color and metallic (glTF convention).
 *
 * Dielectric F0 = 0.04 (IoR 1.5); metal F0 = base_color.
 * Returns RGB; scalar-F0 consumers (E_ss, E_glossy) must reduce per-channel.
 *
 * @param base_color Linear RGB base color.
 * @param metallic   Metallic factor in [0,1].
 */
__forceinline__ __device__ float3 compute_F0(const float3 base_color,
                                             const float metallic) {
    const float3 kDielectricF0 = make_float3(0.04f, 0.04f, 0.04f);
    return kDielectricF0 + (base_color - kDielectricF0) * metallic;
}

// ---- Specular D: GGX Normal Distribution -----------------------------------

/**
 * @brief GGX / Trowbridge-Reitz NDF.
 *
 * @param NdotH Must be >= 0.
 * @param alpha roughness^2; caller clamps >= 1e-4 (Dirac-delta at 0).
 */
__forceinline__ __device__ float D_GGX(const float NdotH, const float alpha) {
    const float a2 = alpha * alpha;
    const float d  = NdotH * NdotH * (a2 - 1.0f) + 1.0f;
    return a2 / (kPi * d * d);
}

// ---- Specular V: Height-Correlated Smith GGX --------------------------------

/**
 * @brief Smith height-correlated visibility for GGX (Heitz 2014).
 *
 * Includes the 1/(4·NdotV·NdotL) denominator, so specular BRDF = D·V·F.
 *
 * @param NdotV Must be > 0.
 * @param NdotL Must be > 0.
 * @param alpha roughness^2; caller clamps >= 1e-4.
 */
__forceinline__ __device__ float V_SmithGGXCorrelated(const float NdotV,
                                                      const float NdotL,
                                                      const float alpha) {
    const float a2 = alpha * alpha;
    const float ggx_v = NdotL * sqrtf(NdotV * NdotV * (1.0f - a2) + a2);
    const float ggx_l = NdotV * sqrtf(NdotL * NdotL * (1.0f - a2) + a2);
    return 0.5f / fmaxf(ggx_v + ggx_l, kEpsilon);
}

// ---- Specular F: Schlick Fresnel -------------------------------------------

/**
 * @brief Schlick Fresnel (F90 = 1 only; no F90-parameter overload).
 *
 * @param VdotH In [0,1].
 * @param F0    Normal-incidence reflectance (RGB).
 */
__forceinline__ __device__ float3 F_Schlick(const float VdotH,
                                            const float3 F0) {
    const float f = __powf(1.0f - VdotH, 5.0f);
    return F0 + (make_float3(1.0f, 1.0f, 1.0f) - F0) * f;
}

// ---- Specular: GGX VNDF Sampling (Heitz 2018) -------------------------------

/**
 * @brief Samples the GGX visible normal distribution (isotropic, Heitz 2018).
 *
 * @param Ve    View direction in tangent space, normalized.
 * @param alpha roughness^2 (isotropic: ax = ay = alpha).
 * @param xi    Two uniform random numbers in [0,1).
 * @return Half vector in tangent space (normalized).
 */
__forceinline__ __device__ float3 sample_ggx_vndf(const float3 Ve,
                                                   const float alpha,
                                                   const float2 xi) {
    const float3 Vh = normalize(make_float3(alpha * Ve.x, alpha * Ve.y, Ve.z));

    const float len2 = Vh.x * Vh.x + Vh.y * Vh.y;
    const float3 T1 = len2 > 0.0f
        ? make_float3(-Vh.y, Vh.x, 0.0f) * rsqrtf(len2)
        : make_float3(1.0f, 0.0f, 0.0f);
    const float3 T2 = cross(Vh, T1);

    const float r    = sqrtf(xi.x);
    const float phi  = kTwoPi * xi.y;
    const float t1   = r * cosf(phi);
    float       t2   = r * sinf(phi);
    const float s    = 0.5f * (1.0f + Vh.z);
    t2 = (1.0f - s) * sqrtf(fmaxf(0.0f, 1.0f - t1 * t1)) + s * t2;

    const float3 Nh = t1 * T1 + t2 * T2
        + sqrtf(fmaxf(0.0f, 1.0f - t1 * t1 - t2 * t2)) * Vh;
    return normalize(make_float3(alpha * Nh.x, alpha * Nh.y,
                                 fmaxf(0.0f, Nh.z)));
}

/**
 * @brief Solid-angle PDF of GGX VNDF sampling.
 *
 * D(H)·G1(V)·VdotH/NdotV → ÷ 4·VdotH → D·G1/(4·NdotV).
 *
 * @param NdotH dot(N, H), must be >= 0.
 * @param NdotV dot(N, V), must be > 0.
 * @param alpha roughness^2; caller clamps >= 1e-4.
 */
__forceinline__ __device__ float pdf_ggx_vndf(const float NdotH,
                                              const float NdotV,
                                              const float alpha) {
    const float D  = D_GGX(NdotH, alpha);
    const float a2 = alpha * alpha;
    const float G1 = 2.0f * NdotV / (NdotV + sqrtf(a2 + (1.0f - a2) * NdotV * NdotV));
    return (D * G1) / (4.0f * NdotV);
}

// ---- Diffuse: EON (Energy-Preserving Oren-Nayar) ----------------------------
//
// FON single-scatter + Kulla-Conty-style multi-scatter compensation.
// Ported from EON-diffuse main.glsl (Portsmouth et al., JCGT 2025).
// EON's f_ms is independent of the specular Turquin compensation.

constexpr float kConstant1Fon = 0.5f - 2.0f / (3.0f * kPi);
constexpr float kConstant2Fon = 2.0f / 3.0f - 28.0f / (15.0f * kPi);

/**
 * @brief FON directional albedo, 4-coefficient polynomial (error < 0.1%).
 *
 * @param mu cos(theta) in [0,1].
 * @param r  Linear roughness in [0,1].
 */
__forceinline__ __device__ float E_FON(const float mu, const float r) {
    const float mucomp = 1.0f - mu;
    constexpr float g1 = 0.0571085289f;
    constexpr float g2 = 0.491881867f;
    constexpr float g3 = -0.332181442f;
    constexpr float g4 = 0.0714429953f;
    const float g_over_pi = mucomp * (g1 + mucomp * (g2 + mucomp * (g3 + mucomp * g4)));
    return (1.0f + r * g_over_pi) / (1.0f + kConstant1Fon * r);
}

/**
 * @brief EON diffuse BRDF value (single-scatter + multi-scatter).
 *
 * @param rho      Single-scattering albedo (RGB). Caller applies (1-metallic)
 *                 weighting externally; this function evaluates the diffuse lobe only.
 * @param r        Linear roughness in [0,1].
 * @param wi_local Incident direction (tangent space, z = normal).
 * @param wo_local Outgoing direction (tangent space, z = normal).
 */
__forceinline__ __device__ float3 f_EON(const float3 rho, const float r,
                                        const float3 wi_local,
                                        const float3 wo_local) {
    const float mu_i = wi_local.z;
    const float mu_o = wo_local.z;
    const float s = dot(wi_local, wo_local) - mu_i * mu_o;
    const float s_over_f = s > 0.0f ? s / fmaxf(mu_i, mu_o) : s;
    const float AF = 1.0f / (1.0f + kConstant1Fon * r);
    const float3 f_ss = (rho * kInvPi) * AF * (1.0f + r * s_over_f);

    const float EFo  = E_FON(mu_o, r);
    const float EFi  = E_FON(mu_i, r);
    const float avgEF = AF * (1.0f + kConstant2Fon * r);
    const float3 rho_ms = (rho * rho) * avgEF
        / (make_float3(1.0f, 1.0f, 1.0f) - rho * (1.0f - avgEF));
    const float3 f_ms = (rho_ms * kInvPi)
        * fmaxf(kEpsilon, 1.0f - EFo)
        * fmaxf(kEpsilon, 1.0f - EFi)
        / fmaxf(kEpsilon, 1.0f - avgEF);
    return f_ss + f_ms;
}

// ---- EON importance sampling: CLTC + uniform hemisphere mix -----------------
//
// Mixes uniform hemisphere (P_u) and CLTC (P_c = 1 - P_u). P_u is a
// polynomial of (mu, r). CLTC importance-samples the FON single-scatter
// shape via an LTC matrix; the uniform lobe covers the multi-scatter tail.

/**
 * @brief LTC-frame basis vectors X, Y from view direction V.
 *
 * @param V View direction (tangent space, z = normal).
 * @param X [out] LTC basis X axis in tangent space.
 * @param Y [out] LTC basis Y axis in tangent space.
 */
__forceinline__ __device__ void ltc_basis(const float3 V, float3 &X, float3 &Y) {
    const float len_sqr = V.x * V.x + V.y * V.y;
    X = len_sqr > 0.0f
        ? make_float3(V.x, V.y, 0.0f) * rsqrtf(len_sqr)
        : make_float3(1.0f, 0.0f, 0.0f);
    Y = make_float3(-X.y, X.x, 0.0f);
}

/**
 * @brief LTC space -> tangent space.
 *
 * @param X LTC basis X axis.
 * @param Y LTC basis Y axis.
 * @param v Direction in LTC space.
 */
__forceinline__ __device__ float3 ltc_from(const float3 X, const float3 Y,
                                           const float3 v) {
    return X * v.x + Y * v.y + make_float3(0.0f, 0.0f, v.z);
}

/**
 * @brief Tangent space -> LTC space (transpose).
 *
 * @param X LTC basis X axis.
 * @param Y LTC basis Y axis.
 * @param v Direction in tangent space.
 */
__forceinline__ __device__ float3 ltc_to(const float3 X, const float3 Y,
                                         const float3 v) {
    return make_float3(dot(X, v), dot(Y, v), v.z);
}

/**
 * @brief LTC matrix coefficients a/b/c/d as rational fits of (mu, r).
 *
 * @param mu    cos(theta) of the outgoing direction, in [0,1].
 * @param r     Linear roughness in [0,1].
 * @param a     [out] LTC matrix coefficient a.
 * @param b     [out] LTC matrix coefficient b.
 * @param c     [out] LTC matrix coefficient c.
 * @param d     [out] LTC matrix coefficient d.
 */
__forceinline__ __device__ void ltc_coeffs(const float mu, const float r,
                                           float &a, float &b, float &c, float &d) {
    a = 1.0f + r * (0.303392f + (-0.518982f + 0.111709f * mu) * mu
                       + (-0.276266f + 0.335918f * mu) * r);
    b = r * (-1.16407f + 1.15859f * mu + (0.150815f - 0.150105f * mu) * r)
        / (mu * mu * mu - 1.43545f);
    c = 1.0f + r * (0.20013f + (-0.506373f + 0.261777f * mu) * mu);
    d = r * (0.540852f + (-1.01625f + 0.475392f * mu) * mu)
        / (-1.0743f + (0.0725628f + mu) * mu);
}

/**
 * @brief CLTC sample: direction + PDF.
 *
 * @param wo_local Outgoing direction (tangent space, z = normal).
 * @param r        Linear roughness in [0,1].
 * @param u1       Uniform random number in [0,1).
 * @param u2       Uniform random number in [0,1).
 * @return float4: xyz = sampled wi (tangent space, normalized), w = PDF.
 */
__forceinline__ __device__ float4 cltc_sample(const float3 wo_local,
                                              const float r,
                                              const float u1, const float u2) {
    float a, b, c, d;
    ltc_coeffs(wo_local.z, r, a, b, c, d);

    const float R     = sqrtf(u1);
    const float phi   = kTwoPi * u2;
    const float x_disk = R * cosf(phi);
    const float y     = R * sinf(phi);
    const float vz    = 1.0f / sqrtf(d * d + 1.0f);
    const float s     = 0.5f * (1.0f + vz);
    // -mix(sqrt(1-y*y), x_disk, s)
    const float x     = -((1.0f - s) * sqrtf(fmaxf(0.0f, 1.0f - y * y)) + s * x_disk);

    const float3 wh = make_float3(x, y,
        sqrtf(fmaxf(0.0f, 1.0f - (x * x + y * y))));
    const float pdf_wh = wh.z / (kPi * s);

    float3 wi = make_float3(a * wh.x + b * wh.z, c * wh.y, d * wh.x + wh.z);
    const float len   = sqrtf(dot(wi, wi));
    const float det_m = c * (a - b * d);
    const float pdf_wi = pdf_wh * len * len * len / det_m;

    float3 X, Y;
    ltc_basis(wo_local, X, Y);
    wi = normalize(ltc_from(X, Y, wi));

    return make_float4(wi.x, wi.y, wi.z, pdf_wi);
}

/**
 * @brief CLTC PDF for a given direction pair.
 *
 * @param wo_local Outgoing direction (tangent space, z = normal).
 * @param wi_local Incident direction (tangent space, z = normal).
 * @param r        Linear roughness in [0,1].
 */
__forceinline__ __device__ float cltc_pdf(const float3 wo_local,
                                         const float3 wi_local,
                                         const float r) {
    float3 X, Y;
    ltc_basis(wo_local, X, Y);
    const float3 wi = ltc_to(X, Y, wi_local);

    float a, b, c, d;
    ltc_coeffs(wo_local.z, r, a, b, c, d);
    const float det_m = c * (a - b * d);
    const float3 wh = make_float3(c * (wi.x - b * wi.z),
                                  (a - b * d) * wi.y,
                                  -c * (d * wi.x - a * wi.z));
    const float lensq = dot(wh, wh);
    const float vz = 1.0f / sqrtf(d * d + 1.0f);
    const float s  = 0.5f * (1.0f + vz);
    const float ratio = det_m / fmaxf(lensq, kEpsilon);
    return (ratio * ratio) * fmaxf(wh.z, 0.0f) / (kPi * s);
}

/**
 * @brief Uniform hemisphere sample (z up), PDF = 1/(2pi).
 *
 * @param u1 Uniform random number in [0,1).
 * @param u2 Uniform random number in [0,1).
 */
__forceinline__ __device__ float3 uniform_lobe_sample(const float u1, const float u2) {
    const float sin_theta = sqrtf(fmaxf(0.0f, 1.0f - u1 * u1));
    const float phi = kTwoPi * u2;
    return make_float3(sin_theta * cosf(phi), sin_theta * sinf(phi), u1);
}

/**
 * @brief EON importance sampling (uniform hemisphere + CLTC mix).
 *
 * @param wo_local Outgoing direction (tangent space, z = normal).
 * @param r        Linear roughness in [0,1].
 * @param u1       Uniform random number in [0,1).
 * @param u2       Uniform random number in [0,1).
 * @return float4: xyz = sampled wi (tangent space, normalized), w = PDF.
 */
__forceinline__ __device__ float4 sample_EON(const float3 wo_local, const float r,
                                             const float u1, const float u2) {
    const float mu  = wo_local.z;
    const float P_u = __powf(r, 0.1f)
        * (0.162925f + (-0.372058f + (0.538233f - 0.290822f * mu) * mu) * mu);
    const float P_c = 1.0f - P_u;

    if (u1 <= P_u) {
        const float3 wi = uniform_lobe_sample(u1 / P_u, u2);
        const float pdf_c = cltc_pdf(wo_local, wi, r);
        constexpr float pdf_u = kInvTwoPi;
        return make_float4(wi.x, wi.y, wi.z, P_u * pdf_u + P_c * pdf_c);
    }
    const float4 wi_c = cltc_sample(wo_local, r, (u1 - P_u) / P_c, u2);
    constexpr float pdf_u = kInvTwoPi;
    return make_float4(wi_c.x, wi_c.y, wi_c.z, P_u * pdf_u + P_c * wi_c.w);
}

/**
 * @brief EON sampling PDF for a given direction pair.
 *
 * @param wo_local Outgoing direction (tangent space, z = normal).
 * @param wi_local Incident direction (tangent space, z = normal).
 * @param r        Linear roughness in [0,1].
 */
__forceinline__ __device__ float pdf_EON(const float3 wo_local,
                                        const float3 wi_local, const float r) {
    const float mu  = wo_local.z;
    const float P_u = __powf(r, 0.1f)
        * (0.162925f + (-0.372058f + (0.538233f - 0.290822f * mu) * mu) * mu);
    const float P_c = 1.0f - P_u;
    const float pdf_c = cltc_pdf(wo_local, wi_local, r);
    constexpr float pdf_u = kInvTwoPi;
    return P_u * pdf_u + P_c * pdf_c;
}

// ---- Combined multi-lobe PDF ------------------------------------------------

/**
 * @brief Combined multi-lobe PDF: p_spec·pdf_spec + (1-p_spec)·pdf_diff.
 *
 * Single function ensures consistency across all call sites, preventing
 * MIS bias from mismatched weights.
 *
 * @param pdf_spec Specular lobe PDF.
 * @param pdf_diff Diffuse lobe PDF.
 * @param p_spec   Specular lobe selection probability.
 */
__forceinline__ __device__ float combined_lobe_pdf(const float pdf_spec,
                                                   const float pdf_diff,
                                                   const float p_spec) {
    return p_spec * pdf_spec + (1.0f - p_spec) * pdf_diff;
}

} // namespace qualquer::renderer
