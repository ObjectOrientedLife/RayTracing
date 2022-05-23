#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
using namespace std;
using namespace glm;
typedef unsigned int uint;

void readBMP(const string &path, vector<vec3> *data, int *width, int *height);
vec3 getPixel(const vector<vec3> &data, int width, int height, int x, int y);
void writeBMP(const string &path, const uint width, const uint height, const vector<ivec3> &data);
void writeBMP(const string &path, const uint width, const uint height, const vector<vec3> &data); 