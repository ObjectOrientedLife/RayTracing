#ifndef SCENE
#define SCENE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <map>
#include "Shape.h"
#include "Light.h"
#include "utility.h"
using namespace glm;
using namespace std;

struct Ray;
struct Hit;

const float EPS = 0.001f;
const float LOCAL_FACTOR = 0.5f; // How much specular component will be calculated only with the local illumination?
const vec3 BACKGROUND_COLOR(0.901, 0.980, 0.976);

Hit rayCast(const Ray &r, const Face &f);
Hit rayCast(const Ray &r, const Sphere &s);
vec3 getTriangleNormal(vec3 v1, vec3 v2, vec3 v3);
bool isInsideTriangle(Face triangle, vec3 p);

class Scene
{
public:
    void insertFaces(vector<Face> object, mat4x4 transformation, vec3 diffuse, vec3 specular, vec3 emission, float shininess, float alpha, float refractionFactor);
    void insertSphere(Sphere sphere, mat4x4 transformation, vec3 diffuse, vec3 specular, vec3 emission, float shininess, float alpha, float refractionFactor);
    void insertLight(Light light);
    void insertMovementSphere(vector<vec3> movement);
    Hit getClosest(Ray ray);
    vec3 Phong(vec3 hitPos, vec3 N, vec3 V, vec3 diffuse, vec3 specular, float shininess); // pixel position, normal of that pixel, where the ray comes from, material properties
    void move(int sampleIdx);
    void restore();

private:
    vector<Face> faces;
    vector<Sphere> spheres;
    vector<Light> lights;
    map<int, vector<vec3>> sphereMovements;
    vector<Sphere> spheresBackup;
};

struct Ray
{
    Ray(vec3 _start, vec3 _dir): start(_start), dir(_dir) {} 
    vec3 start;
    vec3 dir; // Unit vector that represents the direction
};

struct Hit
{
    shared_ptr<Shape> shape;
    vec3 pos;
    float dist = INFINITY; // Distance that the ray travelled 
};

#endif