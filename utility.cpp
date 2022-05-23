#include "utility.h"

vec3 getNormal(vec3 v1, vec3 v2, vec3 v3)
{
    return normalize(cross(v2 - v1, v3 - v1));
}

float distFromPlane(vec3 N, float D, vec3 p)
{
    return dot(N, p) + D;
}

vec3 transformPoint(const mat4x4 &transformation, vec3 v) // Column major vs Row major...?
{
    vec4 homogeneous(v.x, v.y, v.z, 1);
    vec4 transformed = transformation * homogeneous;
    return vec3(transformed.x, transformed.y, transformed.z);
}

vec3 transformVec(const mat4x4 &transformation, vec3 v) // Column major vs Row major...?
{
    vec4 homogeneous(v.x, v.y, v.z, 0);
    vec4 transformed = transformation * homogeneous;
    return vec3(transformed.x, transformed.y, transformed.z);
}

void printVec(vec3 v)
{
    cout << v.x << " " << v.y << " " << v.z << endl;
}

vec3 saturate(vec3 col)
{
    return vec3(saturate(col.r), saturate(col.g), saturate(col.b));
}

vec4 saturate(vec4 col)
{
    return vec4(saturate(col.r), saturate(col.g), saturate(col.b), saturate(col.a));
}

float saturate(float f)
{
    if (f < 0.0f)
    {
        f = 0.0f;
    }
    else if (f > 1.0f)
    {
        f = 1.0f;
    }
    return f;
}

vec3 worldToBarycentric(vec3 p, vec3 a, vec3 b, vec3 c) // What is the barycentric coordinate of world coordinate p? 
{
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = p - a;

    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return vec3(u, v, w);
}

vec2 barycentricToUV(vec3 p, vec2 uv1, vec2 uv2, vec2 uv3) // What is the uv coordinate of barycentric coordinate p? 
{
    vec2 coord = p.x * uv1 + p.y * uv2 + p.z * uv3;
    return coord;
}

float getRandomFloat(float lower, float upper)
{
    mt19937_64 rng;
    uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
    seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    std::uniform_real_distribution<double> unif(lower, upper);
    return unif(rng);
}