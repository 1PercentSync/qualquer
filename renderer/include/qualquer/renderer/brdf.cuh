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

// ---- Constants (pi constants are in math_utils.cuh) -------------------------

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
 * Uses the identity n²·a² + (1 − n²) instead of n²·(a² − 1) + 1 to avoid
 * catastrophic cancellation when a² ≪ 1 (the latter subtracts a² − 1 ≈ −1,
 * losing the roughness term entirely in single precision).
 *
 * @param NdotH Must be >= 0.
 * @param alpha roughness²; may be very small but must be > 0.
 */
__forceinline__ __device__ float D_GGX(const float NdotH, const float alpha) {
    const float a2 = alpha * alpha;
    const float n2 = NdotH * NdotH;
    const float d  = fmaf(n2, a2, 1.0f - n2);
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
 * @param alpha roughness²; may be very small but must be > 0.
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
    const float x  = 1.0f - VdotH;
    const float x2 = x * x;
    const float f  = x2 * x2 * x;
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
    float sin_phi, cos_phi;
    __sincosf(phi, &sin_phi, &cos_phi);
    const float t1   = r * cos_phi;
    float       t2   = r * sin_phi;
    const float s    = 0.5f * (1.0f + Vh.z);
    t2 = lerp(sqrtf(fmaxf(0.0f, 1.0f - t1 * t1)), t2, s);

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
 * @param alpha roughness²; may be very small but must be > 0.
 */
__forceinline__ __device__ float pdf_ggx_vndf(const float NdotH,
                                              const float NdotV,
                                              const float alpha) {
    const float D  = D_GGX(NdotH, alpha);
    const float a2 = alpha * alpha;
    const float G1 = 2.0f * NdotV / (NdotV + sqrtf(a2 + (1.0f - a2) * NdotV * NdotV));
    return (D * G1) / (4.0f * NdotV);
}

// ---- Specular: VNDF fused weight + PDF --------------------------------------
//
// For VNDF importance sampling, the analytic throughput weight simplifies:
//   BRDF * cos / PDF = D·V·F·NdotL / (D·G1/(4·NdotV)) = F · G2/G1
// The NDF (D) cancels between numerator and denominator. This fused function
// computes the G2/G1 ratio and the VNDF PDF together, sharing sqrt_v and a2.
// Avoids: 1× redundant D_GGX, 1× redundant sqrtf (sqrt_v shared with G1).
// Bonus: G2/G1 ∈ [0,1] is naturally bounded; the old D·V·NdotL/PDF path has
// unbounded intermediates when D → ∞ at low alpha.

/**
 * @brief Fused VNDF importance sampling weight (G2/G1) and solid-angle PDF.
 *
 * The throughput update for specular VNDF is F * turquin_comp * weight / p_spec.
 */
struct VndfWeightAndPdf {
    float weight; ///< G2/G1 (D cancels analytically in the importance sampling weight).
    float pdf;    ///< Solid-angle VNDF PDF: D * G1 / (4 * NdotV).
};

/**
 * @brief Computes both the G2/G1 weight and VNDF PDF, sharing intermediates.
 *
 * @param NdotH dot(N, H), must be >= 0 (needed for PDF only).
 * @param NdotV dot(N, V), must be > 0.
 * @param NdotL dot(N, L), must be > 0.
 * @param alpha roughness²; may be very small but must be > 0.
 */
__forceinline__ __device__ VndfWeightAndPdf vndf_weight_and_pdf(
        const float NdotH, const float NdotV, const float NdotL, const float alpha) {
    const float a2 = alpha * alpha;
    const float one_minus_a2 = 1.0f - a2;

    // Smith G1 masking sqrt — shared between weight (G2/G1) and PDF (D*G1/(4*NdotV)).
    const float sqrt_v = sqrtf(NdotV * NdotV * one_minus_a2 + a2);
    const float sqrt_l = sqrtf(NdotL * NdotL * one_minus_a2 + a2);

    // G2/G1 for height-correlated Smith GGX (Heitz 2014).
    const float weight = NdotL * (NdotV + sqrt_v)
        / fmaxf(NdotL * sqrt_v + NdotV * sqrt_l, kEpsilon);

    // VNDF PDF = D * G1 / (4 * NdotV), reusing sqrt_v for G1.
    const float D  = D_GGX(NdotH, alpha);
    const float G1 = 2.0f * NdotV / (NdotV + sqrt_v);
    const float pdf = (D * G1) / (4.0f * NdotV);

    return {weight, pdf};
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

// ---- BRDF parameter bundle --------------------------------------------------

/**
 * @brief Per-shading-point BRDF parameters, computed once and shared by
 *        brdf_eval_and_pdf and brdf_sample.
 *
 * Packs geometry (V_ts/N/T/B), material (F0, diffuse_rho, alpha, r),
 * and precomputed energy compensation (turquin_comp, diffuse_weight, p_spec).
 * CLTC invariants (LTC basis/coefficients, EON mix weights) are deliberately
 * excluded: they are recomputed on-the-fly in brdf_eval_and_pdf / brdf_sample
 * from V_ts and r, so they do not cross shadow-ray optixTrace continuation
 * points and thus do not inflate the per-thread continuation stack.
 */
struct BrdfParams {
    float3 V_ts;           ///< View direction (tangent space, z = NdotV).
    float3 N;              ///< Shading normal (world, normalized).
    float3 T;              ///< Tangent axis (world, from build_orthonormal_basis).
    float3 B;              ///< Bitangent axis (world, from build_orthonormal_basis).
    float3 F0;             ///< lerp(0.04, base_color, metallic); RGB.
    float3 base_color;     ///< Linear RGB base color (EON single-scatter albedo input).
    float3 turquin_comp;   ///< Per-channel specular compensation multiplier (RGB).
    float3 E_glossy_rgb;   ///< Per-channel specular directional reflectance (DLSS-RR specular albedo).
    float3 diffuse_weight; ///< (1-metallic) * (1 - E_glossy_per_channel); RGB.
    float  alpha;          ///< roughness² (specular primitives); must be > 0.
    float  r;              ///< Linear roughness in [0,1] (EON / E_ss / E_glossy).
    float  NdotV;          ///< Clamped dot(N, V), must be > 0.
    float  p_spec;         ///< Specular lobe selection probability.
};

/**
 * @brief Result of brdf_sample: sampled direction, throughput update, and
 *        combined multi-lobe PDF.
 *
 * A pdf_combined of 0 signals an invalid sample (specular lobe reflected
 * below the surface); the caller terminates the path in that case.
 */
struct BrdfSample {
    float3 next_dir;          ///< Sampled direction (world, normalized).
    float3 throughput_update; ///< BRDF * cos / (pdf * lobe_prob).
    float  pdf_combined;      ///< Combined multi-lobe PDF (0 = invalid sample).
};

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
 * @brief CLTC invariants computed on-the-fly from V_ts and roughness.
 *
 * Deliberately not stored in BrdfParams: recomputing these (~30 FMA) at each
 * brdf_eval_and_pdf / brdf_sample call avoids carrying 48 extra bytes across
 * shadow-ray optixTrace continuation points.
 */
struct CltcInvariants {
    float3 X;   ///< LTC basis X axis (tangent space).
    float3 Y;   ///< LTC basis Y axis (tangent space).
    float  a;   ///< LTC matrix coefficient a.
    float  b;   ///< LTC matrix coefficient b.
    float  c;   ///< LTC matrix coefficient c.
    float  d;   ///< LTC matrix coefficient d.
    float  P_u; ///< EON uniform-hemisphere mix weight.
    float  P_c; ///< EON CLTC mix weight (1 - P_u).
};

/**
 * @brief Computes CLTC invariants from the view direction and roughness.
 *
 * @param V_ts View direction in tangent space (z = NdotV).
 * @param r    Linear roughness in [0,1].
 * @return CltcInvariants ready for cltc_sample / cltc_pdf / sample_EON / pdf_EON.
 */
__forceinline__ __device__ CltcInvariants compute_cltc(const float3 V_ts,
                                                       const float r) {
    CltcInvariants inv{};
    ltc_basis(V_ts, inv.X, inv.Y);
    ltc_coeffs(V_ts.z, r, inv.a, inv.b, inv.c, inv.d);
    const float mu = V_ts.z;
    inv.P_u = __powf(r, 0.1f)
        * (0.162925f + (-0.372058f + (0.538233f - 0.290822f * mu) * mu) * mu);
    inv.P_c = 1.0f - inv.P_u;
    return inv;
}

/**
 * @brief CLTC sample: direction + PDF.
 *
 * @param cltc CLTC invariants (from compute_cltc).
 * @param u1   Uniform random number in [0,1).
 * @param u2   Uniform random number in [0,1).
 * @return float4: xyz = sampled wi (tangent space, normalized), w = PDF.
 */
__forceinline__ __device__ float4 cltc_sample(const CltcInvariants &cltc,
                                              const float u1, const float u2) {
    const float a = cltc.a, b = cltc.b, c = cltc.c, d = cltc.d;

    const float R     = sqrtf(u1);
    const float phi   = kTwoPi * u2;
    float sin_phi, cos_phi;
    __sincosf(phi, &sin_phi, &cos_phi);
    const float x_disk = R * cos_phi;
    const float y     = R * sin_phi;
    const float vz    = 1.0f / sqrtf(d * d + 1.0f);
    const float s     = 0.5f * (1.0f + vz);
    const float x     = -lerp(sqrtf(fmaxf(0.0f, 1.0f - y * y)), x_disk, s);

    const float3 wh = make_float3(x, y,
        sqrtf(fmaxf(0.0f, 1.0f - (x * x + y * y))));
    const float pdf_wh = wh.z / (kPi * s);

    float3 wi = make_float3(a * wh.x + b * wh.z, c * wh.y, d * wh.x + wh.z);
    const float len   = sqrtf(dot(wi, wi));
    const float det_m = c * (a - b * d);
    const float pdf_wi = pdf_wh * len * len * len / det_m;

    wi = normalize(ltc_from(cltc.X, cltc.Y, wi));

    return make_float4(wi.x, wi.y, wi.z, pdf_wi);
}

/**
 * @brief CLTC PDF for a given direction pair.
 *
 * @param cltc     CLTC invariants (from compute_cltc).
 * @param wi_local Incident direction (tangent space, z = normal).
 */
__forceinline__ __device__ float cltc_pdf(const CltcInvariants &cltc,
                                         const float3 wi_local) {
    const float3 wi = ltc_to(cltc.X, cltc.Y, wi_local);

    const float a = cltc.a, b = cltc.b, c = cltc.c, d = cltc.d;
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
    float sin_phi, cos_phi;
    __sincosf(phi, &sin_phi, &cos_phi);
    return make_float3(sin_theta * cos_phi, sin_theta * sin_phi, u1);
}

/**
 * @brief EON importance sampling (uniform hemisphere + CLTC mix).
 *
 * @param cltc CLTC invariants (from compute_cltc).
 * @param u1   Uniform random number in [0,1).
 * @param u2   Uniform random number in [0,1).
 * @return float4: xyz = sampled wi (tangent space, normalized), w = PDF.
 */
__forceinline__ __device__ float4 sample_EON(const CltcInvariants &cltc,
                                             const float u1, const float u2) {
    const float P_u = cltc.P_u;
    const float P_c = cltc.P_c;

    if (u1 <= P_u) {
        const float3 wi = uniform_lobe_sample(u1 / P_u, u2);
        const float pdf_c = cltc_pdf(cltc, wi);
        constexpr float pdf_u = kInvTwoPi;
        return make_float4(wi.x, wi.y, wi.z, P_u * pdf_u + P_c * pdf_c);
    }
    const float4 wi_c = cltc_sample(cltc, (u1 - P_u) / P_c, u2);
    constexpr float pdf_u = kInvTwoPi;
    return make_float4(wi_c.x, wi_c.y, wi_c.z, P_u * pdf_u + P_c * wi_c.w);
}

/**
 * @brief EON sampling PDF for a given direction pair.
 *
 * @param cltc     CLTC invariants (from compute_cltc).
 * @param wi_local Incident direction (tangent space, z = normal).
 */
__forceinline__ __device__ float pdf_EON(const CltcInvariants &cltc,
                                        const float3 wi_local) {
    const float pdf_c = cltc_pdf(cltc, wi_local);
    constexpr float pdf_u = kInvTwoPi;
    return cltc.P_u * pdf_u + cltc.P_c * pdf_c;
}

// ---- Multi-scatter energy compensation (Turquin 2019) -----------------------
//
// Coefficients: Sforza & Pellacini 2022 rational polynomial fits.
// Source: github.com/dsforza96/energy-preservation (MIT).

/**
 * @brief GGX single-scatter directional albedo (F=1, geometric only).
 *
 * @param r  Linear roughness in [0,1] (sqrt(alpha)).
 * @param mu cos(theta) in [0,1] (= NdotV).
 * @return E_ss in [0,1]; caller must clamp to [eps,1] before dividing.
 */
__forceinline__ __device__ float E_ss(const float r, const float mu) {
    // PolynomialFeatures(3) on [r, mu] -> [1, r, mu, r^2, r*mu, mu^2, r^3, r^2*mu, r*mu^2, mu^3]
    const float r2 = r * r;
    const float mu2 = mu * mu;
    const float r_mu = r * mu;
    const float r3 = r2 * r;
    const float r2_mu = r2 * mu;
    const float r_mu2 = r * mu2;
    const float mu3 = mu2 * mu;

    const float num =  // c0..c9
        1.0247217f        + (-10.984229f) * r   + 10.918318f * mu +
        46.93353f * r2    + (-54.779343f) * r_mu + 21.742077f * mu2 +
        (-30.368898f) * r3 + 31.919222f * r2_mu   + (-8.013965f) * r_mu2 +
        (-6.2407165f) * mu3;
    const float denom =  // 1 + c10..c18
        1.0f +
        (-10.218104f) * r   + 10.955399f * mu +
        44.08196f * r2      + (-55.33452f) * r_mu + 21.437538f * mu2 +
        (-23.744568f) * r3  + 33.265057f * r2_mu + (-7.9268975f) * r_mu2 +
        (-5.930959f) * mu3;
    return num / denom;
}

/**
 * @brief Specular multi-scatter compensation: 1 + F0*(1 - E_ss)/E_ss.
 *
 * @param F0      Normal-incidence reflectance (scalar; call per-channel for metals).
 * @param E_ss_val Clamped E_ss in [eps, 1].
 * @return Compensation multiplier (>= 1).
 */
__forceinline__ __device__ float turquin_compensation(const float F0,
                                                     const float E_ss_val) {
    return 1.0f + F0 * (1.0f - E_ss_val) / E_ss_val;
}

/**
 * @brief Specular directional albedo with Fresnel (for diffuse-specular coupling).
 *
 * Diffuse weight = 1 - E_glossy. Separate fit needed because Fresnel
 * depends on VdotH, not NdotV, so E_glossy cannot be derived from E_ss.
 * Dielectric-only; metals have no diffuse and skip this.
 *
 * @param F0 Scalar normal-incidence reflectance.
 * @param r  Linear roughness in [0,1].
 * @param mu cos(theta) in [0,1] (= NdotV).
 * @return E_glossy in [0,1].
 */
__forceinline__ __device__ float E_glossy(const float F0, const float r,
                                         const float mu) {
    const float F02 = F0 * F0;
    const float r2 = r * r;
    const float mu2 = mu * mu;
    const float F0_r = F0 * r;
    const float F0_mu = F0 * mu;
    const float r_mu = r * mu;
    const float F03 = F02 * F0;
    const float F02_r = F02 * r;
    const float F02_mu = F02 * mu;
    const float F0_r2 = F0 * r2;
    const float F0_r_mu = F0 * r_mu;
    const float F0_mu2 = F0 * mu2;
    const float r3 = r2 * r;
    const float r2_mu = r2 * mu;
    const float r_mu2 = r * mu2;
    const float mu3 = mu2 * mu;

    // PolynomialFeatures(3) on [F0, r, mu]:
    // [1, F0, r, mu, F0^2, F0*r, F0*mu, r^2, r*mu, mu^2,
    //  F0^3, F0^2*r, F0^2*mu, F0*r^2, F0*r*mu, F0*mu^2,
    //  r^3, r^2*mu, r*mu^2, mu^3]
    const float num =  // c0..c19
        0.04301317f     + 132.98329f * F0     + (-0.9273584f) * r   + (-0.61434704f) * mu +
        (-262.23462f) * F02 + (-137.75214f) * F0_r + (-234.72151f) * F0_mu +
        5.125822f * r2      + (-0.37465897f) * r_mu + 9.284745f * mu2 +
        129.71187f * F03    + 171.82188f * F02_r   + 400.04813f * F02_mu +
        206.99231f * F0_r2  + 1.0847985f * F0_r_mu + 428.02484f * F0_mu2 +
        (-2.2108653f) * r3  + (-6.056363f) * r2_mu + 0.95864034f * r_mu2 +
        (-11.775469f) * mu3;
    const float denom =  // 1 + c20..c38
        1.0f +
        139.43494f * F0     + (-24.177433f) * r   + (-3.7300687f) * mu +
        (-253.77824f) * F02 + 6.717145f * F0_r   + 98.03935f * F0_mu +
        153.19194f * r2     + (-184.53282f) * r_mu + 230.02286f * mu2 +
        113.9376f * F03     + 66.64211f * F02_r  + 108.315094f * F02_mu +
        23.577564f * F0_r2  + 120.04127f * F0_r_mu + 102.90899f * F0_mu2 +
        17.030241f * r3     + 25.947954f * r2_mu + 75.77901f * r_mu2 +
        49.348934f * mu3;
    return num / denom;
}

/**
 * @brief Factored RGB E_glossy: evaluates three F0 channels sharing r/mu work.
 *
 * Reorganizes the 39-coefficient rational polynomial by F0 powers:
 *   P(F0) = k0(r,mu) + F0·k1(r,mu) + F0²·k2(r,mu) + F0³·k3
 * Computing k0..k3 once and evaluating the Horner form per channel saves ~40%
 * FMA vs three independent E_glossy calls when F0 channels differ. Each channel
 * is clamped to [0,1] (fitting error at domain boundaries).
 *
 * @param F0 Per-channel normal-incidence reflectance.
 * @param r  Linear roughness in [0,1].
 * @param mu cos(theta) in [0,1] (= NdotV).
 * @return E_glossy per channel, clamped to [0,1].
 */
__forceinline__ __device__ float3 E_glossy_rgb_clamped(const float3 F0,
                                                       const float r,
                                                       const float mu) {
    const float r2 = r * r, mu2 = mu * mu, r_mu = r * mu;
    const float r3 = r2 * r, r2_mu = r2 * mu, r_mu2 = r * mu2, mu3 = mu2 * mu;

    // Numerator coefficients grouped by F0 power.
    const float nk0 =  0.04301317f + (-0.9273584f) * r + (-0.61434704f) * mu
                     +  5.125822f * r2 + (-0.37465897f) * r_mu + 9.284745f * mu2
                     + (-2.2108653f) * r3 + (-6.056363f) * r2_mu
                     +  0.95864034f * r_mu2 + (-11.775469f) * mu3;
    const float nk1 =  132.98329f + (-137.75214f) * r + (-234.72151f) * mu
                     +  206.99231f * r2 + 1.0847985f * r_mu + 428.02484f * mu2;
    const float nk2 = -262.23462f + 171.82188f * r + 400.04813f * mu;
    const float nk3 =  129.71187f;

    // Denominator coefficients grouped by F0 power.
    const float dk0 =  1.0f + (-24.177433f) * r + (-3.7300687f) * mu
                     +  153.19194f * r2 + (-184.53282f) * r_mu + 230.02286f * mu2
                     +  17.030241f * r3 + 25.947954f * r2_mu
                     +  75.77901f * r_mu2 + 49.348934f * mu3;
    const float dk1 =  139.43494f + 6.717145f * r + 98.03935f * mu
                     +  23.577564f * r2 + 120.04127f * r_mu + 102.90899f * mu2;
    const float dk2 = -253.77824f + 66.64211f * r + 108.315094f * mu;
    const float dk3 =  113.9376f;

    // Per-channel Horner evaluation + clamp.
    const float nx = nk0 + F0.x * (nk1 + F0.x * (nk2 + F0.x * nk3));
    const float dx = dk0 + F0.x * (dk1 + F0.x * (dk2 + F0.x * dk3));
    const float ny = nk0 + F0.y * (nk1 + F0.y * (nk2 + F0.y * nk3));
    const float dy = dk0 + F0.y * (dk1 + F0.y * (dk2 + F0.y * dk3));
    const float nz = nk0 + F0.z * (nk1 + F0.z * (nk2 + F0.z * nk3));
    const float dz = dk0 + F0.z * (dk1 + F0.z * (dk2 + F0.z * dk3));

    return make_float3(
        fminf(fmaxf(nx / dx, 0.0f), 1.0f),
        fminf(fmaxf(ny / dy, 0.0f), 1.0f),
        fminf(fmaxf(nz / dz, 0.0f), 1.0f));
}

/**
 * @brief Clamped E_glossy per channel with scalar fast path.
 *
 * Pure dielectrics (metallic=0) have F0={0.04,0.04,0.04}: all channels
 * identical. Detects this and evaluates the polynomial once, avoiding
 * 3x redundant computation for the most common material class. Falls
 * back to the factored RGB variant for mixed-metallic F0.
 */
__forceinline__ __device__ float3 compute_E_glossy_clamped(
        const float3 F0, const float r, const float mu) {
    if (F0.x == F0.y && F0.y == F0.z) {
        const float eg = fminf(fmaxf(E_glossy(F0.x, r, mu), 0.0f), 1.0f);
        return make_float3(eg, eg, eg);
    }
    return E_glossy_rgb_clamped(F0, r, mu);
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

// ---- Tangent-space transforms (TBN basis) ----------------------------------

/**
 * @brief Transforms a world-space vector into tangent space (z = N).
 *
 * @param T Tangent axis (world, unit, from build_orthonormal_basis).
 * @param B Bitangent axis (world, unit).
 * @param N Normal axis (world, unit).
 * @param v World-space vector.
 * @return Tangent-space vector with z = dot(N, v).
 */
__forceinline__ __device__ float3 world_to_tangent(const float3 T, const float3 B,
                                                   const float3 N, const float3 v) {
    return make_float3(dot(T, v), dot(B, v), dot(N, v));
}

/**
 * @brief Transforms a tangent-space vector back to world space.
 *
 * @param T Tangent axis (world, unit).
 * @param B Bitangent axis (world, unit).
 * @param N Normal axis (world, unit).
 * @param v Tangent-space vector (z = normal component).
 * @return World-space vector.
 */
__forceinline__ __device__ float3 tangent_to_world(const float3 T, const float3 B,
                                                   const float3 N, const float3 v) {
    return T * v.x + B * v.y + N * v.z;
}

// ---- Orthonormal basis ------------------------------------------------------

/**
 * @brief Builds an orthonormal basis (T, B) from a unit normal.
 *
 * Crosses N with +Z, or +X when N is nearly parallel to +Z, to avoid
 * degenerate cross products. Ported from Himalaya pt_common.glsl.
 *
 * @param N Unit normal (world space, must be normalized).
 * @param T [out] Tangent axis perpendicular to N.
 * @param B [out] Bitangent axis perpendicular to N and T.
 */
__forceinline__ __device__ void build_orthonormal_basis(const float3 N,
                                                        float3 &T, float3 &B) {
    if (fabsf(N.z) < 0.999f) {
        T = normalize(cross(make_float3(0.0f, 0.0f, 1.0f), N));
    } else {
        T = normalize(cross(make_float3(1.0f, 0.0f, 0.0f), N));
    }
    B = cross(N, T);
}

// ---- Multi-lobe BRDF selection ----------------------------------------------

/**
 * @brief Probability of selecting the specular lobe over diffuse.
 *
 * Fresnel reflectance luminance at the view angle, clamped to [0.01, 0.99]
 * to avoid division by a zero selection probability. Ported from Himalaya
 * pt_common.glsl.
 *
 * @param NdotV Clamped dot(N, V), must be > 0.
 * @param F0    Normal-incidence reflectance (RGB).
 * @return Specular lobe selection probability in [0.01, 0.99].
 */
__forceinline__ __device__ float specular_probability(const float NdotV,
                                                      const float3 F0) {
    const float3 F = F_Schlick(NdotV, F0);
    const float spec_weight = luminance(F);
    return fmaxf(fminf(spec_weight, 0.99f), 0.01f);
}

// ---- BRDF parameter bundle --------------------------------------------------

/**
 * @brief Constructs a BrdfParams from raw shading-point inputs.
 *
 * Centralizes the per-channel energy compensation so closesthit stays free
 * of compensation bookkeeping:
 *   - turquin_comp: 3 channels of turquin_compensation sharing one scalar
 *     E_ss (E_ss is F0-independent, computed once).
 *   - diffuse_weight: (1-metallic) * (1 - E_glossy_per_channel), where
 *     E_glossy uses the mixed specular F0 = lerp(0.04, base_color, metallic)
 *     per channel. Pure metals (metallic=1) have no diffuse, so E_glossy is
 *     always skipped and diffuse_weight = 0 (callers that need E_glossy for
 *     aux G-buffer must compute it themselves).
 *
 * @param V              View direction (world, normalized, facing surface);
 *                       converted to tangent space and stored as V_ts.
 * @param N              Shading normal (world, normalized).
 * @param T_basis        Tangent axis (world, from build_orthonormal_basis).
 * @param B_basis        Bitangent axis (world).
 * @param base_color     Linear RGB base color.
 * @param metallic       Metallic factor in [0,1].
 * @param roughness      Linear roughness in [0,1] (glTF value, unclamped).
 * @param alpha          roughness²; caller applies a small floor (e.g. 1e-7)
 *                       to avoid GGX delta singularity at zero.
 */
__forceinline__ __device__ BrdfParams init_brdf_params(
        const float3 V, const float3 N,
        const float3 T_basis, const float3 B_basis,
        const float3 base_color, const float metallic,
        const float roughness, const float alpha) {
    BrdfParams p{};
    p.N = N;
    p.T = T_basis;
    p.B = B_basis;
    p.V_ts = world_to_tangent(T_basis, B_basis, N, V);
    p.F0 = compute_F0(base_color, metallic);
    p.base_color = base_color;
    p.alpha = alpha;
    p.r = roughness;
    p.NdotV = fmaxf(dot(N, V), 1e-4f);

    const float E_ss_val = fmaxf(fminf(E_ss(roughness, p.NdotV), 1.0f), kEpsilon);
    // Scalar fast path when F0 channels are equal (pure dielectric, ~90% of
    // glTF materials). turquin_compensation is lightweight (1 div + 2 FMA)
    // but the pattern matches the E_glossy optimization below.
    if (p.F0.x == p.F0.y && p.F0.y == p.F0.z) {
        const float tc = turquin_compensation(p.F0.x, E_ss_val);
        p.turquin_comp = make_float3(tc, tc, tc);
    } else {
        p.turquin_comp = make_float3(
            turquin_compensation(p.F0.x, E_ss_val),
            turquin_compensation(p.F0.y, E_ss_val),
            turquin_compensation(p.F0.z, E_ss_val));
    }

    // Pure metal: no diffuse lobe. Always skip E_glossy here — BRDF shading
    // does not need it, and pipeline aux consumers compute it at the write site.
    if (metallic >= 1.0f) {
        p.diffuse_weight = make_float3(0.0f, 0.0f, 0.0f);
        p.p_spec = 1.0f;
        return p;
    }

    // E_glossy per channel with scalar fast path for equal F0 (pure
    // dielectric) and factored RGB variant for mixed metallic.
    p.E_glossy_rgb = compute_E_glossy_clamped(p.F0, roughness, p.NdotV);

    const float3 one = make_float3(1.0f, 1.0f, 1.0f);
    p.diffuse_weight = (1.0f - metallic) * (one - p.E_glossy_rgb);
    p.p_spec = specular_probability(p.NdotV, p.F0);

    return p;
}

// ---- BRDF eval + PDF (NEE) --------------------------------------------------

/**
 * @brief Fused BRDF value and combined PDF at a fixed light direction.
 *
 * Shares L_ts, H_ts, and NdotH between eval and PDF, eliminating redundant
 * world_to_tangent + normalize + dot that the separate functions repeated.
 */
struct BrdfEvalResult {
    float3 value; ///< BRDF value (specular + diffuse), RGB.
    float  pdf;   ///< Combined multi-lobe solid-angle PDF.
};

/**
 * @brief Evaluates BRDF value and combined PDF for a light direction (NEE).
 *
 * Specular = D * V * F * turquin_comp (Turquin compensation included).
 * Diffuse = diffuse_weight * f_EON (EON already includes 1/pi and its own
 * multi-scatter compensation; diffuse_weight carries (1-metallic) and
 * (1 - E_glossy) coupling).
 *
 * @param params BRDF parameter bundle (from init_brdf_params).
 * @param L      Light direction (world, normalized, facing surface).
 * @param NdotL  dot(N, L), must be > 0 (caller culls back-facing).
 * @return BrdfEvalResult with BRDF value and combined PDF.
 */
__forceinline__ __device__ BrdfEvalResult brdf_eval_and_pdf(
        const BrdfParams &params, const float3 L, const float NdotL) {
    const float3 &V_ts = params.V_ts;
    const float3 L_ts = world_to_tangent(params.T, params.B, params.N, L);

    const float3 H_ts = normalize(V_ts + L_ts);
    const float NdotH = fmaxf(H_ts.z, 0.0f);
    const float VdotH = fmaxf(dot(V_ts, H_ts), 0.0f);

    // Eval: specular (always needed)
    const float D   = D_GGX(NdotH, params.alpha);
    const float Vis = V_SmithGGXCorrelated(params.NdotV, NdotL, params.alpha);
    const float3 F  = F_Schlick(VdotH, params.F0);
    const float3 spec = F * params.turquin_comp * (D * Vis);

    const float pdf_spec = pdf_ggx_vndf(NdotH, params.NdotV, params.alpha);

    // Pure specular (metal): skip EON diffuse eval and CLTC computation.
    // SER groups threads by material, so this branch is warp-coherent.
    if (params.p_spec >= 1.0f) {
        return {spec, pdf_spec};
    }

    // Eval: diffuse (dielectric / mixed metallic only)
    const float3 diff = params.diffuse_weight
        * f_EON(params.base_color, params.r, L_ts, V_ts);

    // PDF: combined multi-lobe (CLTC invariants recomputed locally so they
    // do not cross shadow-ray optixTrace boundaries in the caller).
    const CltcInvariants cltc = compute_cltc(params.V_ts, params.r);
    const float pdf_diff = pdf_EON(cltc, L_ts);

    return {spec + diff,
            combined_lobe_pdf(pdf_spec, pdf_diff, params.p_spec)};
}

// ---- BRDF sample (bounce) ---------------------------------------------------

/**
 * @brief Samples the combined BRDF for the next bounce direction.
 *
 * Selects the specular lobe (GGX VNDF) with probability p_spec, otherwise
 * the diffuse lobe (EON CLTC). Returns the throughput update
 * (BRDF * cos / (pdf * lobe_prob)) and the combined multi-lobe PDF for MIS.
 *
 * A pdf_combined of 0 signals an invalid sample (specular lobe reflected
 * below the surface); the caller terminates the path.
 *
 * @param params BRDF parameter bundle (from init_brdf_params).
 * @param u_lobe Uniform random in [0,1) for lobe selection.
 * @param u0     Uniform random in [0,1) for direction sampling.
 * @param u1     Uniform random in [0,1) for direction sampling.
 * @return BrdfSample with next_dir, throughput_update, pdf_combined.
 */
__forceinline__ __device__ BrdfSample brdf_sample(const BrdfParams &params,
                                                  const float u_lobe,
                                                  const float u0, const float u1) {
    BrdfSample result{};
    const float3 &V_ts = params.V_ts;

    if (u_lobe < params.p_spec) {
        // Specular lobe: GGX VNDF
        const float3 H_ts = sample_ggx_vndf(V_ts, params.alpha, make_float2(u0, u1));
        const float3 L_ts = reflect(-V_ts, H_ts);

        if (L_ts.z <= 0.0f) {
            result.next_dir = params.N;
            result.throughput_update = make_float3(0.0f, 0.0f, 0.0f);
            result.pdf_combined = 0.0f;
            return result;
        }

        const float NdotL = L_ts.z;
        const float NdotH = fmaxf(H_ts.z, 0.0f);
        const float VdotH = fmaxf(dot(V_ts, H_ts), 0.0f);

        // D cancels in BRDF*cos/PDF → throughput = F * turquin * G2/G1 / p_spec.
        const float3 F = F_Schlick(VdotH, params.F0);
        const VndfWeightAndPdf wp = vndf_weight_and_pdf(
            NdotH, params.NdotV, NdotL, params.alpha);
        result.throughput_update = F * params.turquin_comp
            * (wp.weight / params.p_spec);

        // Pure specular (metal): combined PDF = specular PDF, skip CLTC entirely.
        if (params.p_spec >= 1.0f) {
            result.pdf_combined = wp.pdf;
        } else {
            const CltcInvariants cltc = compute_cltc(params.V_ts, params.r);
            const float pdf_diff = pdf_EON(cltc, L_ts);
            result.pdf_combined = combined_lobe_pdf(wp.pdf, pdf_diff, params.p_spec);
        }
        result.next_dir = tangent_to_world(params.T, params.B, params.N, L_ts);
    } else {
        // Diffuse lobe: EON CLTC (only reached when p_spec < 1.0).
        const CltcInvariants cltc = compute_cltc(params.V_ts, params.r);
        const float4 wi_c = sample_EON(cltc, u0, u1);
        const float3 L_ts = make_float3(wi_c.x, wi_c.y, wi_c.z);
        const float pdf_diff = wi_c.w;

        // Reject samples below the shading hemisphere. CLTC is designed to
        // avoid this, but numerical edge cases (r≈1, mu≈1) can produce z<=0.
        // Do not clamp — that alters the sample without adjusting the PDF.
        if (L_ts.z <= 0.0f || pdf_diff <= 0.0f) {
            result.next_dir = params.N;
            result.throughput_update = make_float3(0.0f, 0.0f, 0.0f);
            result.pdf_combined = 0.0f;
            return result;
        }

        const float NdotL = L_ts.z;

        const float3 diff_brdf = params.diffuse_weight
            * f_EON(params.base_color, params.r, L_ts, V_ts);
        result.throughput_update = diff_brdf * NdotL
            / fmaxf(pdf_diff * (1.0f - params.p_spec), kEpsilon);

        const float3 H_ts = normalize(V_ts + L_ts);
        const float NdotH = fmaxf(H_ts.z, 0.0f);
        const float pdf_spec = pdf_ggx_vndf(NdotH, params.NdotV, params.alpha);
        result.pdf_combined = combined_lobe_pdf(pdf_spec, pdf_diff, params.p_spec);
        result.next_dir = tangent_to_world(params.T, params.B, params.N, L_ts);
    }

    return result;
}

} // namespace qualquer::renderer
