#ifndef UTILITY
#define UTILITY

#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <random>
#include <chrono>
using namespace std;
using namespace glm;

vec3 getNormal(vec3 v1, vec3 v2, vec3 v3);
float distFromPlane(vec3 N, float D, vec3 p);
vec3 transformPoint(const mat4x4 &transformation, vec3 v);
vec3 transformVec(const mat4x4 &transformation, vec3 v);
void printVec(vec3 v);
vec3 saturate(vec3 col);
vec4 saturate(vec4 col);
float saturate(float f);
vec3 worldToBarycentric(vec3 p, vec3 a, vec3 b, vec3 c);
vec2 barycentricToUV(vec3 p, vec2 uv1, vec2 uv2, vec2 uv3);
float getRandomFloat(float lower, float upper);

#endif