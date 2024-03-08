#ifndef ECOSYSTEM_CLASS_H
#define ECOSYSTEM_CLASS_H

#include<vector>
#include<iostream>
#include<glm/glm.hpp>
#include"FastPoissonDiskSampling.h"	
#include"Plant.h"
#include"Noise.h"
#include"Terrain.h"

class Ecosystem
{
public:
	Ecosystem(std::vector<Plant>* input_plants, Noise* input_noise, Terrain* input_terrain);

	void GeneratePoissonPositions(float terrainSize);

	void RecalculateLayers();
	
	void DistributePositions();

	std::vector<Plant>* plants;
	std::vector<std::vector<int>> layerIndices;
	std::vector<float> layerRadii;
	std::vector<std::vector<glm::vec2>> poissonPositions;
	Noise* noise;
	Terrain* terrain;

};
#endif

