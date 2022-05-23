#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <omp.h>
#include "objImporter.h"
#include "Scene.h"
#include "Shape.h"
#include "Light.h"
#include "bmpLib.h"
#include "utility.h"
using namespace std;
using namespace glm;

// ==================== Function declarations ====================
int main(int argc, char** argv);
vec3 traceRay(vec3 from, vec3 dir, vec3 color, int depth, float prevRefractionFactor);
void initObjects();
vector<vec3> getAverageImage(vector<vector<vec3>> images);

// ==================== Scene setup constants ====================
int SAMPLE_COUNT;
const int SCENE0_SAMPLE_COUNT = 20;
const int SCENE1_SAMPLE_COUNT = 120;
const float F = 13.0f; // Focal length
const int W = 600; // Image width
const int H = 450; // Image height
const float COEF = 7.0f / H; // Pixel size in the world coordinate
const int MAX_DEPTH = 4; // MAX_DEPTH == 1 -> Raycast
const float AIR_FACTOR = 1.0f;

// ==================== Object variables ====================
Sphere led;
vector<Face> thinkPad;
vector<Face> panel;
vector<Face> plane;
vector<Face> rock;
vector<Face> key;
vector<Face> trackPoint;
vector<Face> cube;
vector<Face> checker;
Sphere goldSphere;
Sphere sphere;
Sphere largeSphere;
Sphere checkerSphere;
Sphere earth;

int carbonW;
int carbonH;
vector<vec3> carbonTexture;

int carbonNormalW;
int carbonNormalH;
vector<vec3> carbonNormal;

int earthW;
int earthH;
vector<vec3> earthTexture;

int rockNormalW;
int rockNormalH;
vector<vec3> rockNormal;

int checkerW;
int checkerH;
vector<vec3> checkerTexture;

int goldW;
int goldH;
vector<vec3> goldTexture;

// ==================== Movement variables ====================
vector<vec3> redMovement; // Movement from the previous value, length == SAMPLE_COUNT

// ==================== Scene variables ====================
Scene scene;

int main(int argc, char** argv) 
{
    double start = omp_get_wtime();

    initObjects();
    int sceneNumber = stoi(argv[1]);

    if (sceneNumber == 0)
    {
        SAMPLE_COUNT = SCENE0_SAMPLE_COUNT;

        // ==================== Place objects onto the scene ====================
        mat4x4 identity = mat4x4(1.0f);
        mat4x4 transformScene = rotate(identity, 15.0f * (float) M_PI / 180.0f, vec3(1, 0, 0)); // Transforms the whole scene
        transformScene = rotate(transformScene, 15.0f * (float) M_PI / 180.0f, vec3(0, 1, 0));

        mat4x4 transformPlane = translate(transformScene, vec3(0, 0, 0));
        scene.insertFaces(plane, transformPlane, vec3(0.1, 0.1, 0.1), vec3(0.01, 0.01, 0.01), vec3(0, 0, 0), 1, 1, 1);

        mat4x4 transformBackground = translate(transformScene, vec3(0, 0, -4));
        transformBackground = rotate(transformBackground, 90.0f * (float) M_PI / 180.0f, vec3(1, 0, 0));
        scene.insertFaces(plane, transformBackground, vec3(0.1, 0.1, 0.1), vec3(0.01, 0.01, 0.01), vec3(0, 0, 0), 1, 1, 1);

        mat4x4 transformGoldenSphere = translate(transformScene, vec3(1.2, 0.5, 1.5));
        scene.insertSphere(goldSphere, transformGoldenSphere, vec3(0.88, 0.75, 0.3), vec3(1, 0.84, 0), vec3(0, 0, 0), 7, 1, 1);

        mat4x4 transformSilverSphere = translate(transformScene, vec3(2.4, 0.5, 0));
        scene.insertSphere(sphere, transformSilverSphere, vec3(0.4, 0.4, 0.4), vec3(1, 1, 1), vec3(0, 0, 0), 100, 1, 1);

        mat4x4 transformSapphireSphere = translate(transformScene, vec3(2.4, 0.5, 1.5));
        scene.insertSphere(sphere, transformSapphireSphere, vec3(0.5, 0.5, 1), vec3(0.87, 0.86, 1), vec3(0, 0, 0), 100, 0.1, 1.2);

        mat4x4 transformTrackPoint = translate(transformScene, vec3(1.8, 0.7, 0.75));
        scene.insertFaces(trackPoint, transformTrackPoint, vec3(1, 0.09, 0.11), vec3(0.1, 0.059, 0.06), vec3(0, 0, 0), 2, 1, 1);

        mat4x4 transformPanel = translate(transformScene, vec3(0, 1.8, -0.5));
        transformPanel = rotate(transformPanel, 45.0f * (float) M_PI / 180.0f, vec3(0, 0, 1));
        transformPanel = rotate(transformPanel, 90.0f * (float) M_PI / 180.0f, vec3(1, 0, 0));
        scene.insertFaces(panel, transformPanel, vec3(0.5, 0.5, 1), vec3(1, 1, 1), vec3(0, 0, 0), 3, 0.02, 1.2);

        mat4x4 transformThinkPad = translate(transformScene, vec3(0, 1.8, 0));
        transformThinkPad = rotate(transformThinkPad, 45.0f * (float) M_PI / 180.0f, vec3(0, 0, 1));
        transformThinkPad = rotate(transformThinkPad, 90.0f * (float) M_PI / 180.0f, vec3(1, 0, 0));
        scene.insertFaces(thinkPad, transformThinkPad, vec3(1.5, 1.5, 1.5), vec3(1, 1, 1), vec3(0, 0, 0), 3, 1, 0);

        mat4x4 transformLED = translate(transformScene, vec3(0, 1.8, 0));
        transformLED = rotate(transformLED, 45.0f * (float) M_PI / 180.0f, vec3(0, 0, 1));
        transformLED = translate(transformLED, vec3(-0.6, 0.4, 0.2));
        scene.insertSphere(led, transformLED, vec3(1, 0, 0), vec3(0.79,0.33,0.33), vec3(1, 0, 0), 3, 0.7, 1.01);

        // ==================== Place lights onto the scene ====================
        Light light1(vec3(0, 10, 10), vec3(0, 0, 0), vec3(1, 1, 1), 0, true, 0.3);
        scene.insertLight(light1);

        vec3 light2Pos(0, 0.1, 1);
        light2Pos = transformPoint(transformTrackPoint, light2Pos);
        Light light2(light2Pos, vec3(0, 0, 0), vec3(1, 1, 2), 0.2, false, 0.1);
        scene.insertLight(light2);

        vec3 light3Pos(0, 0, 0);
        transformLED = translate(transformLED, vec3(0, 0, 0.2));
        light3Pos = transformPoint(transformLED, light3Pos);
        Light light3(light3Pos, vec3(0, 0, 0), vec3(1, 0, 0), 0.2, false, 0.1);
        scene.insertLight(light3);
    }
    else if (sceneNumber == 1)
    {
        SAMPLE_COUNT = SCENE1_SAMPLE_COUNT;

        // ==================== Place objects onto the scene ====================
        mat4x4 transformScene = mat4x4(1.0f); // Transforms the whole scene
        transformScene = translate(transformScene, vec3(0, -0.5, -3));
        transformScene = rotate(transformScene, 15.0f * (float) M_PI / 180.0f, vec3(1, 0, 0)); // Transforms the whole scene

        mat4x4 transformEarth = translate(transformScene, vec3(0, 0, 0));
        scene.insertSphere(earth, transformEarth, vec3(1, 0.09, 0.11), vec3(0.1, 0.059, 0.06), vec3(0, 0, 0), 2, 1, 1);

        mat4x4 transformSphere1 = translate(transformScene, vec3(3, 1, -0.5));
        scene.insertSphere(checkerSphere, transformSphere1, vec3(0.4, 0.4, 0.4), vec3(1, 1, 1), vec3(0, 0, 0), 100, 1, 1);

        mat4x4 transformSphere2 = translate(transformScene, vec3(-2, 7, 0.5));
        scene.insertSphere(sphere, transformSphere2, vec3(0.8, 0, 0), vec3(1, 1, 1), vec3(0, 0, 0), 100, 1, 1);
        scene.insertMovementSphere(redMovement);

        mat4x4 transformSphere3 = translate(transformScene, vec3(1.4, 1.5, 2));
        scene.insertSphere(largeSphere, transformSphere3, vec3(0.5, 0.5, 1), vec3(1, 1, 1), vec3(0, 0, 0), 100, 0.1, 1.05);

        mat4x4 transformRock = translate(transformScene, vec3(0, -1, 0));
        scene.insertFaces(rock, transformRock, vec3(0.8, 0.8, 0.8), vec3(0.8, 0.8, 0.8), vec3(0, 0, 0), 90, 1, 1);

        mat4x4 transformMirror = translate(transformScene, vec3(0, 0, -2.5));
        scene.insertFaces(panel, transformMirror, vec3(0, 1, 0), vec3(0.8, 1, 0.8), vec3(0, 0, 0), 100, 1, 1);

        mat4x4 transformChecker = translate(transformScene, vec3(0, 0, -4));
        transformChecker = rotate(transformChecker, 90.0f * (float) M_PI / 180.0f, vec3(1, 0, 0));
        scene.insertFaces(checker, transformChecker, vec3(0.8, 0.8, 0.8), vec3(0.8, 0.8, 0.8), vec3(0, 0, 0), 90, 1, 1);

        // ==================== Place lights onto the scene ====================
        Light light1(vec3(5, 10, 10), vec3(0, 0, 0), vec3(1, 1, 1), 0, true, 0.12);
        scene.insertLight(light1);
    }
    
    // ==================== Ray tracing ====================
    vector<vec3> sampleResults[SAMPLE_COUNT];
    #pragma omp parallel for // Execute in parallel
    for (int s = 0; s < SAMPLE_COUNT; ++s) // s: Sample
    {
        vector<vec3> imgVector;
        vec3 origin(0, 0, F);
        scene.move(s); // Move the scene
        for (int y = 0 ; y < H; ++y)
        {
            cout << "Sample: " << s << ", tracing y: " << y << "/" << H - 1 << endl;
            for (int x = 0; x < W; ++x)
            {
                float offsetX = getRandomFloat(0, 1);
                float offsetY = getRandomFloat(0, 1);

                float xPos = (-W / 2 + x + offsetX) * COEF; // Center pixel at the origin
                float yPos = (H / 2 - y + offsetY) * COEF;
                vec3 pixelPos(xPos, yPos, 0);

                vec3 primaryRay = normalize(pixelPos - origin);

                vec3 traced = traceRay(origin, primaryRay, vec3(0, 0, 0), 1, AIR_FACTOR);
                imgVector.push_back(traced);
            }
        }
        scene.restore(); // Restore the scene
        sampleResults[s] = imgVector;
    }

    vector<vector<vec3>> samples;
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        samples.push_back(sampleResults[i]);
    }
    vector<vec3> averaged = getAverageImage(samples); // Take the average

    writeBMP("./Results/Result.bmp", W, H, averaged);
    
    double end = omp_get_wtime();
    cout << "Time elapsed: " << end - start << " seconds" << endl;

    return 0;
}

vec3 traceRay(vec3 from, vec3 dir, vec3 color, int depth, float prevRefractionFactor)
{
    Ray ray(from, dir);
    Hit hit = scene.getClosest(ray);

    if (hit.dist != INFINITY) // If the ray hits something
    {
        // ==================== Calculate Phong illumination parameters: N(normal), V(view direction, ray direction) ====================
        vec3 hitPos = hit.pos; // Where did the ray hit in the world coordinate?
        vec3 diffuse = hit.shape->diffuse;
        vec3 specular = hit.shape->specular;
        float shininess = hit.shape->shininess;
        float alpha = hit.shape->alpha;

        vec3 V = normalize(-dir); // View direction, ray direction
        vec3 N; // Normal
        if (hit.shape->isSphere)
        {
            shared_ptr<Sphere> sp = dynamic_pointer_cast<Sphere>(hit.shape);
            N = normalize(hitPos - sp->center);

            if (sp->hasTexture)
            {
                float uCoord = atan2(N.x, N.z) / (2 * M_PI) + 0.5f;
                float vCoord = N.y * 0.5f + 0.5f;
                int u = sp->textureW * uCoord - 1;
                int v = sp->textureH * vCoord - 1;
                diffuse = getPixel(*(sp->texture), sp->textureW, sp->textureH, u, v);
            }
        }
        else
        {
            shared_ptr<Face> fp = dynamic_pointer_cast<Face>(hit.shape);
            vec3 v1 = fp->v1;
            vec3 v2 = fp->v2;
            vec3 v3 = fp->v3;

            vec3 n1 = fp->n1;
            vec3 n2 = fp->n2;
            vec3 n3 = fp->n3;
            vec3 bc = worldToBarycentric(hitPos, v1, v2, v3);
            N = normalize(bc.x * n1 + bc.y * n2 + bc.z * n3);

            if (fp->hasTexture)
            {
                vec2 uvCoord = barycentricToUV(bc, fp->uv1, fp->uv2, fp->uv3);
                diffuse = getPixel(*(fp->texture), fp->textureW, fp->textureH, uvCoord.x, uvCoord.y);
            }

            if (fp->hasNormalMap)
            {
                vec2 uvCoord = barycentricToUV(bc, fp->nuv1, fp->nuv2, fp->nuv3);
                vec3 normalFromMap = normalize(getPixel(*(fp->normalMap), fp->normalMapW, fp->normalMapH, uvCoord.x, uvCoord.y));
                normalFromMap = 2.0f * normalFromMap - vec3(1, 1, 1);

                N = normalize(fp->TBN * normalFromMap);
            }
        }

        // ==================== Get the illumination ====================
        color += alpha * scene.Phong(hitPos, N, V, diffuse, specular, shininess);

        // ==================== Recursive ====================
        if (depth < MAX_DEPTH)
        {
            // ==================== Refraction: Snell's law ====================
            float nextRefractionFactor = prevRefractionFactor == AIR_FACTOR ? hit.shape->refractionFactor : AIR_FACTOR;
            N = prevRefractionFactor == AIR_FACTOR ? N : -N; // From inside to out
            float refractionRatio = prevRefractionFactor / nextRefractionFactor;
            float cti = dot(V, N);
            float k = 1 - pow(refractionRatio, 2) * (1 - pow(cti, 2));

            if (k > 0)
            {
                float ctr = sqrt(k); // cos(theta_i)
                vec3 T = normalize((refractionRatio * cti - ctr) * N - refractionRatio * V);
                color += (1.0f - alpha) * traceRay(hitPos, T, color, depth + 1, nextRefractionFactor);
            }
            else // Total internal reflection
            {
                alpha = 1.0f - alpha;
            }

            // ==================== Reflection ====================
            // Reflection vector of the traced ray, not the Phong illumination's light reflection vector
            // Modify to the stochastic ray tracing - shoot many stochastic rays
            vec3 R = 2 * dot(V, N) * N - V;
            // Perturbation
            vec3 dummy = normalize(R + vec3(0, 1, 0));
            vec3 axis1 = cross(R, dummy);
            vec3 axis2 = cross(R, axis1);
            float rand1 = getRandomFloat(-1, 1) / shininess;
            float rand2 = getRandomFloat(-1, 1) / shininess;
            vec3 randDir = normalize(R + rand1 * axis1 + rand2 * axis2);

            // Trace
            color += (1 - LOCAL_FACTOR) * alpha * specular * traceRay(hitPos, randDir, color, depth + 1, prevRefractionFactor); // alpha * specular...?
        }
    }
    else
    {
        color += BACKGROUND_COLOR;
    }

    color = saturate(color);
    return color;
}

void initObjects() 
{
    // ==================== Initalize objects ====================
    led = Sphere(vec3(0, 0, 0), 0.1, vec3(0, 0, 0));
    thinkPad = parseData("./Models/ThinkPad.obj");
    panel = parseData("./Models/Panel.obj");
    key = parseData("./Models/Key.obj");
    trackPoint = parseData("./Models/TrackPoint.obj");
    cube = parseData("./Models/Cube.obj");
    sphere = Sphere(vec3(0, 0, 0), 0.5, vec3(0, 0, 0));
    goldSphere = Sphere(vec3(0, 0, 0), 0.5, vec3(0, 0, 0));
    largeSphere = Sphere(vec3(0, 0, 0), 1.5, vec3(0, 0, 0));
    checkerSphere = Sphere(vec3(0, 0, 0), 1.3, vec3(0, 0, 0));
    earth = Sphere(vec3(0, 0, 0), 1, vec3(0, 0, 0));
	plane = parseData("./Models/Plane.obj");
    checker = parseData("./Models/Plane.obj");
    rock = parseData("./Models/Plane.obj");

    // ==================== Initalize textures ====================
    readBMP("./Textures/Carbon.bmp", &carbonTexture, &carbonW, &carbonH);
    plane[0].setTexture(&carbonTexture, carbonW, carbonH, vec2(carbonW - 1, carbonH - 1), vec2(0, 0), vec2(0, carbonH - 1));
    plane[1].setTexture(&carbonTexture, carbonW, carbonH, vec2(carbonW - 1, carbonH - 1), vec2(carbonH - 1, 0), vec2(0, 0));

    readBMP("./Textures/CarbonNormal.bmp", &carbonNormal, &carbonNormalW, &carbonNormalH);
    plane[0].setNormalMap(&carbonNormal, carbonNormalW, carbonNormalH, vec2(carbonNormalW - 1, carbonNormalH - 1), vec2(0, 0), vec2(0, carbonNormalH - 1));
    plane[1].setNormalMap(&carbonNormal, carbonNormalW, carbonNormalH, vec2(carbonNormalW - 1, carbonNormalH - 1), vec2(carbonNormalH - 1, 0), vec2(0, 0));

    readBMP("./Textures/Earth.bmp", &earthTexture, &earthW, &earthH);
    earth.setTexture(&earthTexture, earthW, earthH);

    readBMP("./Textures/RockNormal.bmp", &rockNormal, &rockNormalW, &rockNormalH);
    rock[0].setNormalMap(&rockNormal, rockNormalW, rockNormalH, vec2(rockNormalW - 1, rockNormalH - 1), vec2(0, 0), vec2(0, rockNormalH - 1));
    rock[1].setNormalMap(&rockNormal, rockNormalW, rockNormalH, vec2(rockNormalW - 1, rockNormalH - 1), vec2(rockNormalH - 1, 0), vec2(0, 0));

    readBMP("./Textures/Checker.bmp", &checkerTexture, &checkerW, &checkerH);
    checker[0].setTexture(&checkerTexture, checkerW, checkerH, vec2(checkerW - 1, checkerH - 1), vec2(0, 0), vec2(0, checkerH - 1));
    checker[1].setTexture(&checkerTexture, checkerW, checkerH, vec2(checkerW - 1, checkerH - 1), vec2(checkerH - 1, 0), vec2(0, 0));
    checkerSphere.setTexture(&checkerTexture, checkerW, checkerH);

    readBMP("./Textures/Gold.bmp", &goldTexture, &goldW, &goldH);
    goldSphere.setTexture(&goldTexture, goldW, goldH);

    // ==================== Initalize movements ====================
    vec3 redMove = vec3(0, -0.007f, 0);
    for (int i = 0; i < SCENE1_SAMPLE_COUNT * 3 / 4; i += 20)
    {
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
        redMovement.push_back(redMove * (float) i);
    }
    for (int i = SCENE1_SAMPLE_COUNT * 3 / 4; i < SCENE1_SAMPLE_COUNT; ++i)
    {
        redMovement.push_back(redMove * (float) (SCENE1_SAMPLE_COUNT / 2));
    }
}

vector<vec3> getAverageImage(vector<vector<vec3>> images)
{
    int imgCount = images.size();
    int imgLength = images[0].size();

    vector<vec3> avgs;
    for (int i = 0; i < imgLength; ++i)
    {
        vec3 avg(0, 0, 0);
        for (int j = 0; j < imgCount; ++j)
        {
            avg += images[j][i];
        }
        avg /= imgCount;

        avgs.push_back(avg);
    }

    return avgs;
}