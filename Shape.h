#ifndef SHAPE
#define SHAPE

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <vector>
using namespace glm;
using namespace std;

struct Shape;
struct Face;
struct Sphere;

vector<Face> getQuad(float x, float z);

struct Shape
{
    Shape() {}
    Shape (vec4 _diffuse, vec4 _specular, vec4 _emission, float _shininess, float _alpha, float _refractionFactor): diffuse(_diffuse), specular(_specular), emission(_emission), shininess(_shininess), alpha(_alpha), refractionFactor(_refractionFactor)  {}
    virtual ~Shape() {} 

    // Members
    bool isSphere = false;

    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
    float alpha;
    float refractionFactor;

    mat4x4 transform;

    // UV coordinate data
    bool hasTexture = false;
    vector<vec3> *texture;
    int textureW;
    int textureH;
};

struct Face: public Shape
{
    Face(): Shape() {}
    Face(vec3 _v1, vec3 _v2, vec3 _v3, vec3 _n1, vec3 _n2, vec3 _n3, vec4 _diffuse, vec4 _specular, vec4 _emission, float _shininess, float _alpha, float _refractionFactor)
    : v1(_v1), v2(_v2), v3(_v3), 
    n1(_n1), n2(_n2), n3(_n3),
    Shape(_diffuse, _specular, _emission, _shininess, _alpha, _refractionFactor)
    {}
    void setTexture(vector<vec3> *_texture, int _textureW, int _textureH, vec2 _uv1, vec2 _uv2, vec2 _uv3);
    void setNormalMap(vector<vec3> *_normalMap, int _normalMapW, int _normalMapH, vec2 _nuv1, vec2 _nuv2, vec2 _nuv3);
    
    // Vertices
    vec3 v1; 
    vec3 v2;
    vec3 v3;

    // Normals
    vec3 n1; 
    vec3 n2;
    vec3 n3;

    // UV coordinate data
    vec2 uv1;
    vec2 uv2;
    vec2 uv3;

    bool hasNormalMap = false;
    vector<vec3> *normalMap;
    int normalMapW;
    int normalMapH;
    vec2 nuv1;
    vec2 nuv2;
    vec2 nuv3;
    mat3x3 TBN;
};

struct Sphere: public Shape
{
    Sphere(): Shape() 
    {
        isSphere = true;
    }
    Sphere(vec3 _center, float _radius, vec3 _dir): center(_center), radius(_radius), dir(_dir), Shape()
    {
        isSphere = true;
    }
    Sphere(vec3 _center, float _radius, vec4 _diffuse, vec4 _specular, vec4 _emission, float _shininess, float _alpha, float _refractionFactor, vec3 _dir)
    : center(_center), radius(_radius), dir(_dir),
    Shape(_diffuse, _specular, _emission, _shininess, _alpha, _refractionFactor)
    {
        isSphere = true;
    }
    void setTexture(vector<vec3> *_texture, int _textureW, int _textureH);
    void setNormalMap(vector<vec3> *_normalMap, int _normalMapW, int _normalMapH);

    float radius;
    vec3 center;
    vec3 dir;
};

#endif