#ifndef LIGHT
#define LIGHT

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
using namespace glm;
using namespace std;

struct Light;

struct Light
{
    Light(vec3 _pos, vec3 _aimAt, vec3 _diffuse, float _quadAtten, bool _isFar, float _areaFactor): pos(_pos), aimAt(_aimAt), diffuse(_diffuse), quadAtten(_quadAtten), isFar(_isFar), areaFactor(_areaFactor) {}

    vec3 pos;
    vec3 aimAt;
    vec3 diffuse;
    float quadAtten;
    bool isFar;
    float areaFactor;
};

#endif