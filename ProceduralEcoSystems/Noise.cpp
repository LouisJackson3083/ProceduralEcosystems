#include "Noise.h"
#include "SimplexNoise.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdint>

Noise::Noise() {

}

float** Noise::generateNoiseMap(int width, int height, float scale) {
	float** noiseMap = new float* [height];

    if (scale <= 0) {
        scale = 0.000000001f;
    }

    for (int y = 0; y < height; y++) {

        noiseMap[y] = new float[width];

        for (int x = 0; x < width; x++) {
            float sampleX = x / scale;
            float sampleY = y / scale;

            float simplexNoiseValue = SimplexNoise::noise(sampleX, sampleY);

            noiseMap[y][x] = simplexNoiseValue * 255;
        }
    }

	return noiseMap;

}

void Noise::SaveImage(const char* filename, float img_vals[][512])
{   
    
}
