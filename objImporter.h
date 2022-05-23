#ifndef OBJ_IMPORTER
#define OBJ_IMPORTER

#include <vector>
#include <string>
#include <cmath>
#include "Shape.h"
using namespace std;

vector<Face> parseData(string path);
vector<string> split(string input, char delimiter);
void debug();

#endif