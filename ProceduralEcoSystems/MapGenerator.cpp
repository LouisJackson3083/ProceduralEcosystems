#include"MapGenerator.h"
#include"Noise.h"

MapGenerator::MapGenerator() {

}

void MapGenerator::GenerateMap(int width, int height, float scale) {
	Noise noise;
	float** noiseMap = noise.generateNoiseMap(width, height, scale);

}