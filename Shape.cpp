#include "Shape.h"

void Face::setTexture(vector<vec3> *_texture, int _textureW, int _textureH, vec2 _uv1, vec2 _uv2, vec2 _uv3)
{
	hasTexture = true;
	texture = _texture;
	textureW = _textureW;
    textureH = _textureH;

	uv1 = _uv1;
	uv2 = _uv2;
	uv3 = _uv3;
}

void Face::setNormalMap(vector<vec3> *_normalMap, int _normalMapW, int _normalMapH, vec2 _nuv1, vec2 _nuv2, vec2 _nuv3)
{
	hasNormalMap = true;
	normalMap = _normalMap;
	normalMapW = _normalMapW;
    normalMapH = _normalMapH;

	nuv1 = _nuv1;
	nuv2 = _nuv2;
	nuv3 = _nuv3;
}

void Sphere::setTexture(vector<vec3> *_texture, int _textureW, int _textureH)
{
	hasTexture = true;
	texture = _texture;
	textureW = _textureW;
    textureH = _textureH;
}

vector<Face> getQuad(float x, float z)
{
	float halfX = x / 2.0f;
	float halfZ = z / 2.0f;
	vector<Face> faces;

	// Make f1
	Face f1;
	f1.v1 = vec3(-halfX, 0, halfZ);
	f1.n1 = vec3(0, 1, 0);

	f1.v2 = vec3(halfX, 0, -halfZ);
	f1.n2 = vec3(0, 1, 0);

	f1.v3 = vec3(-halfX, 0, -halfZ);
	f1.n3 = vec3(0, 1, 0);

	faces.push_back(f1);

	// Make f2
	Face f2;
	f2.v1 = vec3(halfX, 0, halfZ);
	f2.n1 = vec3(0, 1, 0);

	f2.v2 = vec3(halfX, 0, -halfZ);
	f2.n2 = vec3(0, 1, 0);

	f2.v3 = vec3(-halfX, 0, halfZ);
	f2.n3 = vec3(0, 1, 0);

	faces.push_back(f2);

	return faces;
}