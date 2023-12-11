#ifndef NOISE_CLASS_H
#define NOISE_CLASS_H
#include<vector>

class Noise {
public:
	float** noiseMap;

	int seed;

	float scale;
	int octaves;
	float persistance;
	float lacunarity;

	float predictedNoiseMax;

	std::vector<float> offsets;

	time_t time_created;

	Noise(float input_scale, int input_octaves, float input_persistance, float input_lacunarity, int input_seed);

	void updateNoiseValues(float input_scale, int input_octaves, float input_persistance, float input_lacunarity);

	void updateSeed(int input_seed);

	float** generateNoiseMap(int width, int height, float scale);

	float get(int x, int y);


private:
};

#endif