#include "Scene.h"

void Scene::insertFaces(vector<Face> object, mat4x4 transformation, vec3 diffuse, vec3 specular, vec3 emission, float shininess, float alpha, float refractionFactor)
{
    for (Face face : object)
    {
        Face transformedFace;

        transformedFace.v1 = transformPoint(transformation, face.v1);
        transformedFace.v2 = transformPoint(transformation, face.v2);
        transformedFace.v3 = transformPoint(transformation, face.v3);

        transformedFace.n1 = transformVec(transformation, face.n1);
        transformedFace.n2 = transformVec(transformation, face.n2);
        transformedFace.n3 = transformVec(transformation, face.n3);

        transformedFace.diffuse = diffuse;
        transformedFace.specular = specular;
        transformedFace.emission = emission;
        transformedFace.shininess = shininess;
        transformedFace.alpha = alpha;
        transformedFace.refractionFactor = refractionFactor;

        transformedFace.hasTexture = face.hasTexture;
        transformedFace.texture = face.texture;
        transformedFace.uv1 = face.uv1;
        transformedFace.uv2 = face.uv2;
        transformedFace.uv3 = face.uv3;
        transformedFace.textureW = face.textureW;
        transformedFace.textureH = face.textureH;

        transformedFace.hasNormalMap = face.hasNormalMap;
        transformedFace.normalMap = face.normalMap;
        transformedFace.nuv1 = face.nuv1;
        transformedFace.nuv2 = face.nuv2;
        transformedFace.nuv3 = face.nuv3;
        transformedFace.normalMapW = face.normalMapW;
        transformedFace.normalMapH = face.normalMapH;

        transformedFace.transform = transformation;

        if (transformedFace.hasNormalMap)
        {
            // positions
            vec3 v1 = face.v1;
            vec3 v2 = face.v2;
            vec3 v3 = face.v3;

            // texture coordinates
            vec2 nuv1 = face.nuv1;
            vec2 nuv2 = face.nuv2;
            vec2 nuv3 = face.nuv3;

            vec3 normal = face.n1; // Arbitrary

            vec3 edge1 = v2 - v1;
            vec3 edge2 = v3 - v1;
            vec2 deltaUV1 = nuv2 - nuv1;
            vec2 deltaUV2 = nuv3 - nuv1;  

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            vec3 T = normalize(transformVec(transformation, tangent));
            vec3 B = normalize(transformVec(transformation, bitangent));
            vec3 N = normalize(transformVec(transformation, normal));

            transformedFace.TBN = mat3x3(T, B, N);
        }

        faces.push_back(transformedFace);
    }
}

void Scene::insertSphere(Sphere sphere, mat4x4 transformation, vec3 diffuse, vec3 specular, vec3 emission, float shininess, float alpha, float refractionFactor)
{
    Sphere transformedSphere;

    transformedSphere.center = transformPoint(transformation, sphere.center);
    transformedSphere.radius = sphere.radius;
    transformedSphere.dir = transformVec(transformation, sphere.dir);
    
    transformedSphere.diffuse = diffuse;
    transformedSphere.specular = specular;
    transformedSphere.emission = emission;
    transformedSphere.shininess = shininess;
    transformedSphere.alpha = alpha;
    transformedSphere.refractionFactor = refractionFactor;

    transformedSphere.transform = transformation;

    transformedSphere.hasTexture = sphere.hasTexture;
    transformedSphere.texture = sphere.texture;
    transformedSphere.textureW = sphere.textureW;
    transformedSphere.textureH = sphere.textureH;

    spheres.push_back(transformedSphere);
}

void Scene::insertMovementSphere(vector<vec3> movement)
{
    int idx = spheres.size() - 1;
    sphereMovements[idx] = movement;
}

void Scene::move(int sampleIdx)
{
    // For each sphere movements
    for (auto p : sphereMovements)
    {
        int idx = p.first; // Sphere index
        vec3 movement = p.second[sampleIdx];

        spheresBackup = spheres;
        spheres[idx].center += movement;
    }
}

void Scene::restore()
{
    spheres = spheresBackup;
}

vec3 Scene::Phong(vec3 hitPos, vec3 N, vec3 V, vec3 diffuse, vec3 specular, float shininess) // pixel position, normal of that pixel, where the ray comes from, material properties
{
    // Calculate the illumination
    vec3 I(0, 0, 0);
    for (Light l : lights)
    {
        vec3 L = l.isFar ? normalize(l.pos - l.aimAt) : normalize(l.pos - hitPos);
        // Perturbation
        vec3 dummy = normalize(L + vec3(1, 0, 0));
        vec3 axis1 = cross(L, dummy);
        vec3 axis2 = cross(L, axis1);
        float rand1 = getRandomFloat(-1, 1) * l.areaFactor;
        float rand2 = getRandomFloat(-1, 1) * l.areaFactor;
        vec3 randDir = normalize(L + rand1 * axis1 + rand2 * axis2);

        Ray shadowRay(hitPos, randDir); // Shoot a shadow ray
        Hit shadowHit = getClosest(shadowRay);
        if (shadowHit.dist == INFINITY) // Contribution only if the light is visible
        {
            float d = distance(l.pos, hitPos);
            float quadAtten = !l.isFar && l.quadAtten > 0 ? 1 / (l.quadAtten * d * d) : 1; 

            vec3 R = 2 * dot(L, N) * N - L;
            float viewDiff = saturate(dot(R, V));
            vec3 diffuseIntensity = saturate((diffuse * l.diffuse * quadAtten) * dot(N, L)); // Overflow occurs without saturation
            //printVec(N);
            //cout << dot(N, L) << endl;
            //cout << "==========" << endl;
            vec3 specularIntensity = LOCAL_FACTOR * saturate((specular * l.diffuse) * pow(viewDiff, (float) shininess));

            I += saturate(diffuseIntensity + specularIntensity);
        }
    }

    return I;
}

void Scene::insertLight(Light light)
{
    lights.push_back(light);
}

Hit Scene::getClosest(Ray ray) // Add sphere!
{
    float minDist = INFINITY;
    Hit closestHit;
    
    for (Face f : faces)
    {
        Hit hit = rayCast(ray, f);
        if (hit.dist > EPS && hit.dist < minDist)
        {
            closestHit = hit;
            minDist = hit.dist;
        }
    }

    for (Sphere s : spheres)
    {
        Hit hit = rayCast(ray, s);
        if (hit.dist > EPS && hit.dist < minDist)
        {
            closestHit = hit;
            minDist = hit.dist;
        }
    }

    return closestHit;
}

Hit rayCast(const Ray &r, const Face &f)
{
    vec3 N = getNormal(f.v1, f.v2, f.v3);
    float D = -(N.x * f.v1.x + N.y * f.v1.y + N.z * f.v1.z); // Define the plane equation

    float s = - (D + dot(N, r.start)) / dot(N, r.dir);
    vec3 hitPos = r.start + s * r.dir;

    Hit hit;
    bool isInside = isInsideTriangle(f, hitPos);
    if (isInside)
    {
        hit.pos = hitPos;
        hit.shape = make_shared<Face>(f);

        hit.dist = s;
    }
    return hit;
}

Hit rayCast(const Ray &r, const Sphere &s)
{
    vec3 dp = s.center - r.start;
    float b = -dot(r.dir, dp);
    float c = length(dp) * length(dp) - s.radius * s.radius;
    float det = b * b - c;

    Hit hit;
    if (det >= 0.0f)
    {
        hit.dist = -b - sqrt(det);
        hit.shape = make_shared<Sphere>(s);
        hit.pos = r.start + hit.dist * r.dir;
    }
    return hit;    
}  

bool isInsideTriangle(Face triangle, vec3 p)
{
    vec3 a = triangle.v1;
    vec3 b = triangle.v2;
    vec3 c = triangle.v3;

    a -= p;
    b -= p;
    c -=p;

    vec3 u = cross(b, c);
    vec3 v = cross(c, a);
    vec3 w = cross(a, b);

    if (dot(u, v) <= 0.0f) 
    {
        return false;
    }

    if (dot(u, w) <= 0.0f) 
    {
        return false;
    }

    // All normals facing the same way, return true
    return true;
}