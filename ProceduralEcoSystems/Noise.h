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
#include <glad/glad.h>
#include <algorithm>
#include <glm/glm.hpp>

class Noise {
public:

	int seed;
	float scale;
	int octaves;
	float persistance;
	float lacunarity;
	float predictedNoiseMax;

	int erosionMapSize = 256;
	int erosionMapOffset = 128;
	std::vector<std::vector<float>> erosionMap;

	const int texSize = 256;
	int numDroplets = 5;
	int dropletLifetime = 30;
	float sedimentCapacityFactor = 4; // Multiplier for how much sediment a droplet can carry
	float minSedimentCapacity = 0.01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
	float inertia = 0.05f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
	float depositSpeed = 0.3f;
	float erodeSpeed = 0.3f;
	float evaporateSpeed = 0.01f;


	std::vector<float> offsets;

	time_t time_created;

	FastNoiseLite fastNoiseLite;

	Noise(float input_scale, int input_octaves, float input_persistance, float input_lacunarity, int input_seed);

	void updateNoiseValues(float input_scale, int input_octaves, float input_persistance, float input_lacunarity);

	void updateSeed(int input_seed);

	float** generateNoiseMap(int width, int height, float scale);

	float get(float x, float y, bool useErosion);

	void updateErosionValues(
		int input_numIterations,
		int input_maxLifetime,
		float input_inertia,
		float input_depositSpeed,
		float input_erodeSpeed,
		float input_evaporateSpeed
	);

	void generateErosionMap();

	glm::vec3 ErosionGetHeightAndGradients(float x, float y);

private:
};

#endif