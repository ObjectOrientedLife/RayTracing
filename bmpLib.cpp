#include "bmpLib.h"

void readBMP(const string &path, vector<vec3> *data, int *width, int *height)
{
    ifstream file(path, ios::in | ios::binary);
    if (file.fail()) 
    {
        cout << "Error: File " + path + " does not exist!" << endl;
    }

    uint w = 0;
    uint h = 0;
    char header[54];
    file.read(header, 54);
    for(uint i = 0; i < 4; i++) 
    {
        w |= (header[18 + i] & 255) << (8 * i);
        h |= (header[22 + i] & 255) << (8 * i);
    }
    int pad = (4 - (3 * w) % 4) % 4;
    int imgSize = (3 * w + pad) * h;
    *width = w;
    *height = h;

    char *img = new char[imgSize];
    file.read(img, imgSize);
    file.close();
    data->resize(w * h);
    for(uint y = 0; y < h; y++) 
    {
        for(uint x = 0; x < w; x++) 
        {
            int i = 3 * x + y * (3 * w + pad);
            (*data)[x + (h - 1 - y) * w].b = (img[i] & 255) / 255.0f;
            (*data)[x + (h - 1 - y) * w].g = (img[i + 1] & 255) / 255.0f;
            (*data)[x + (h - 1 - y) * w].r = (img[i + 2] & 255) / 255.0f;
        }
    }
    
    delete[] img;
}

vec3 getPixel(const vector<vec3> &data, int width, int height, int x, int y)
{
    int pad = (4 - (3 * width) % 4) % 4;
    return data[x + (height - 1 - y) * width];
}

void writeBMP(const string &path, const uint width, const uint height, const vector<ivec3> &data) 
{
    const uint pad = (4 - (3 * width) % 4) % 4, filesize = 54 + (3 * width + pad) * height; // horizontal line must be a multiple of 4 bytes long, header is 54 bytes
    char header[54] = 
    { 
        'B','M', 0, 0, 
        0, 0, 0, 0, 0, 
        0, 54, 0, 0, 0, 
        40, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        1, 0, 24, 0 
    };

    for(uint i = 0; i < 4; i++) 
    {
        header[2 + i] = (char)((filesize >> (8 * i)) & 255);
        header[18 + i] = (char)((width >> (8 * i)) & 255);
        header[22 + i] = (char)((height >> (8 * i)) & 255);
    }

    char* img = new char[filesize];
    for(uint i = 0; i< 54; i++) 
    {
        img[i] = header[i];
    }

    for(uint y = 0; y < height; y++) 
    {
        for(uint x = 0; x < width; x++) 
        {
            ivec3 color = data[x + (height - 1 - y) * width];
            const int i = 54 + 3 * x + y * (3 * width + pad);
            img[i] = (char) (color.b & 255);
            img[i + 1] = (char) (color.g & 255);
            img[i + 2] = (char)(color.r & 255);
        }

        for(uint p = 0; p < pad; p++)
        {
            img[54 + (3 * width + p) + y * (3 * width + pad)] = 0;
        }
    }

    ofstream file(path, ios::out|ios::binary);
    file.write(img, filesize);
    file.close();
    delete[] img;
}

void writeBMP(const string &path, const uint width, const uint height, const vector<vec3> &data)
{
    vector<ivec3> intData;
    for (auto color : data)
    {
        vec3 intColor;
        intColor.r = (int) (255 * color.r);
        intColor.g = (int) (255 * color.g);
        intColor.b = (int) (255 * color.b);

        intData.push_back(intColor);
    }
    writeBMP(path, width, height, intData);
}