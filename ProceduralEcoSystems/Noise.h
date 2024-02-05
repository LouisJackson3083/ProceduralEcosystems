#ifndef NOISE_CLASS_H
#define NOISE_CLASS_H
#include<vector>
#include "SimplexNoise.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdint>
#include "FastNoiseLite.h"

class Noise {
public:

	int seed;

	float scale;
	int octaves;
	float persistance;
	float lacunarity;

	float predictedNoiseMax;

	std::vector<float> offsets;

	time_t time_created;

	FastNoiseLite fastNoiseLite;

	Noise(float input_scale, int input_octaves, float input_persistance, float input_lacunarity, int input_seed);

	void updateNoiseValues(float input_scale, int input_octaves, float input_persistance, float input_lacunarity);

	void updateSeed(int input_seed);

	float** generateNoiseMap(int width, int height, float scale);

	float get(float x, float y);


private:
};

#endif