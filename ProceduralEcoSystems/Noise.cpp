#include "Noise.h"


Noise::Noise(float input_scale, int input_octaves, float input_persistance, float input_lacunarity, int input_seed) {
    scale = input_scale;
    octaves = input_octaves;
    persistance = input_persistance;
    lacunarity = input_lacunarity;
    seed = input_seed;
    srand(seed);
    time_created = time(0);

    fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_Ridged);

    float amplitude = 1.0f;
    float frequency = 1;
    predictedNoiseMax = 0.0f;
    for (int i = 0; i < octaves; i++) {
        predictedNoiseMax += 1.0f * amplitude;
        amplitude *= persistance;
        frequency *= lacunarity;

        offsets.push_back(rand());
    }



}

void Noise::updateNoiseValues(float input_scale, int input_octaves, float input_persistance, float input_lacunarity) {
    scale = input_scale;
    octaves = input_octaves;
    persistance = input_persistance;
    lacunarity = input_lacunarity;

    float amplitude = 1.0f;
    float frequency = 1;
    predictedNoiseMax = 0.0f;
    for (int i = 0; i < octaves; i++) {
        predictedNoiseMax += 1.0f * amplitude;
        amplitude *= persistance;
        frequency *= lacunarity;
    }

    if (offsets.size() != octaves) {
        updateSeed(seed);
    }
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

            //float simplexNoiseValue = SimplexNoise::noise(sampleX, sampleY);
            float simplexNoiseValue = fastNoiseLite.GetNoise((float)sampleX, (float)sampleY);


            noiseMap[y][x] = simplexNoiseValue * 255;
        }
    }

	return noiseMap;

}

float Noise::get(float x, float y) {
    float amplitude = 1.0f;
    float frequency = 1;
    float noiseHeight = 0;
    float maxNoiseEstimate = 0.0f;
    float minNoiseEstimate = 0.0f;

    for (int i = 0; i < octaves; i++) {

        float sampleX = x / scale * frequency + offsets[i];
        float sampleY = y / scale * frequency + offsets[i];

        //float simplexNoiseValue = SimplexNoise::noise(sampleX, sampleY);
        float simplexNoiseValue = fastNoiseLite.GetNoise((float)sampleX, (float)sampleY);

        noiseHeight += simplexNoiseValue * amplitude;

        maxNoiseEstimate += 1.0f * amplitude;
        minNoiseEstimate -= 1.0f * amplitude;

        amplitude *= persistance;
        frequency *= lacunarity;
    }

    return (noiseHeight + predictedNoiseMax) / (predictedNoiseMax * 2);


}

void Noise::updateSeed(int input_seed) {
    seed = input_seed;
    srand(seed);

    offsets.clear();

    for (int i = 0; i < octaves; i++) {
        offsets.push_back(rand());
    }
}
