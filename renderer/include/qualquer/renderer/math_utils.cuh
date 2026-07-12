#pragma once

/**
 * @file math_utils.cuh
 * @brief CUDA device math helpers (renderer layer).
 */

#include <qualquer/renderer/launch_params.h>

using qualquer::renderer::float4x4;

// ---- Mathematical constants --------------------------------------------------

constexpr float kPi       = 3.14159265358979323846f;
constexpr float kTwoPi    = 6.28318530717958647692f;
constexpr float kInvPi    = 0.31830988618379067154f;
constexpr float kInvTwoPi = 0.15915494309189533577f;

// ---- float3 / float2 / float4 arithmetic helpers ----------------------------

__forceinline__ __device__ float3 operator+(const float3 a, const float3 b) {
    return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}
__forceinline__ __device__ float3 operator-(const float3 a, const float3 b) {
    return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}
__forceinline__ __device__ float3 operator-(const float3 a) {
    return make_float3(-a.x, -a.y, -a.z);
}
__forceinline__ __device__ float3 operator-(const float3 a, const float s) {
    return make_float3(a.x - s, a.y - s, a.z - s);
}
__forceinline__ __device__ float3 operator*(const float3 a, const float s) {
    return make_float3(a.x * s, a.y * s, a.z * s);
}
__forceinline__ __device__ float3 operator*(const float s, const float3 a) { return a * s; }

__forceinline__ __device__ float3 operator*(const float3 a, const float3 b) {
    return make_float3(a.x * b.x, a.y * b.y, a.z * b.z);
}
__forceinline__ __device__ float3 operator/(const float3 a, const float s) {
    return make_float3(a.x / s, a.y / s, a.z / s);
}
__forceinline__ __device__ float3 operator/(const float3 a, const float3 b) {
    return make_float3(a.x / b.x, a.y / b.y, a.z / b.z);
}

__forceinline__ __device__ float2 operator+(const float2 a, const float2 b) {
    return make_float2(a.x + b.x, a.y + b.y);
}
__forceinline__ __device__ float2 operator*(const float2 a, const float s) {
    return make_float2(a.x * s, a.y * s);
}
__forceinline__ __device__ float2 operator*(const float s, const float2 a) { return a * s; }

__forceinline__ __device__ float4 operator+(const float4 a, const float4 b) {
    return make_float4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
__forceinline__ __device__ float4 operator*(const float4 a, const float s) {
    return make_float4(a.x * s, a.y * s, a.z * s, a.w * s);
}
__forceinline__ __device__ float4 operator*(const float s, const float4 a) { return a * s; }

// ---- Scalar float3 math -----------------------------------------------------

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace {

__forceinline__ __device__ float dot(const float3 a, const float3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
__forceinline__ __device__ float3 cross(const float3 a, const float3 b) {
    return make_float3(a.y * b.z - a.z * b.y,
                       a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x);
}
__forceinline__ __device__ float3 normalize(const float3 v) {
    return v * rsqrtf(dot(v, v));
}

/// Linear interpolation: (1-t)*a + t*b (GLSL mix equivalent).
__forceinline__ __device__ float lerp(const float a, const float b, const float t) {
    return a + t * (b - a);
}

/// Reflects I about unit normal N: I - 2*dot(I,N)*N (GLSL reflect equivalent).
__forceinline__ __device__ float3 reflect(const float3 I, const float3 N) {
    return I - N * (2.0f * dot(I, N));
}

/// BT.709 luminance of a linear RGB color.
__forceinline__ __device__ float luminance(const float3 c) {
    return 0.2126f * c.x + 0.7152f * c.y + 0.0722f * c.z;
}

/// Rotates a direction around the Y axis by (sin_r, cos_r).
__forceinline__ __device__ float3 rotate_y_dir(const float3 d,
                                               const float sin_r, const float cos_r) {
    return make_float3(cos_r * d.x + sin_r * d.z,
                       d.y,
                       -sin_r * d.x + cos_r * d.z);
}

// ---- Matrix helpers ---------------------------------------------------------

/// Multiply a float4x4 (row-major) by a float4 column vector.
__forceinline__ __device__ float4 mul(const float4x4 &m, const float4 &v) {
    return make_float4(
        m.rows[0].x * v.x + m.rows[0].y * v.y + m.rows[0].z * v.z + m.rows[0].w * v.w,
        m.rows[1].x * v.x + m.rows[1].y * v.y + m.rows[1].z * v.z + m.rows[1].w * v.w,
        m.rows[2].x * v.x + m.rows[2].y * v.y + m.rows[2].z * v.z + m.rows[2].w * v.w,
        m.rows[3].x * v.x + m.rows[3].y * v.y + m.rows[3].z * v.z + m.rows[3].w * v.w);
}

// ---- Device vertex layout ---------------------------------------------------

/// Matches host Vertex (vertex.h): position/normal/uv0/color/tangent, 64 bytes.
struct DeviceVertex {
    float3 position;
    float3 normal;
    float2 uv0;
    float4 color;
    float4 tangent;
};
static_assert(sizeof(DeviceVertex) == 64);

// ---- Normal mapping (ported from Himalaya normal.glsl) ----------------------

/// Construct TBN, decode BC5 normal map RG, and return world-space shading normal.
__forceinline__ __device__ float3 get_shading_normal(
        const float3 N, const float3 T_world, const float tangent_w,
        const float2 normal_rg, const float normal_scale) {
    // Decode tangent-space XY from [0,1] → [-1,1], reconstruct Z
    const float tx = (normal_rg.x * 2.0f - 1.0f) * normal_scale;
    const float ty = (normal_rg.y * 2.0f - 1.0f) * normal_scale;
    const float tz = sqrtf(fmaxf(0.0f, 1.0f - tx * tx - ty * ty));

    // Degenerate tangent guard
    const float t_len_sq = dot(T_world, T_world);
    if (t_len_sq < 1e-6f) {
        return N;
    }

    // TBN: T, B = cross(N, T) * handedness, N
    const float3 T = T_world * rsqrtf(t_len_sq);
    const float3 B = cross(N, T) * tangent_w;

    return normalize(T * tx + B * ty + N * tz);
}

} // namespace
