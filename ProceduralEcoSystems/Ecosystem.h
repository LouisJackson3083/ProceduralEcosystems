#ifndef ECOSYSTEM_CLASS_H
#define ECOSYSTEM_CLASS_H

#include<vector>
#include<iostream>
#include<glm/glm.hpp>
#include"FastPoissonDiskSampling.h"	
#include"Plant.h"
#include"Tree.h"
#include"Noise.h"
#include"Grass.h"
#include"Terrain.h"

class Ecosystem
{
public:
	Ecosystem(Grass* input_grass, std::vector<Plant>* input_plants, std::vector<Tree>* input_trees, Noise* input_noise, Terrain* input_terrain);

	void GeneratePoissonPositions(float terrainSize, float grassSize1, float grassSize2);

	void RecalculateLayers();
	
	void DistributePositions();

	Grass* grass;
	std::vector<Plant>* plants;
	std::vector<Tree>* trees;
	std::vector<std::vector<int>> layerIndices;
	std::vector<float> layerRadii;
	std::vector<std::vector<glm::vec2>> poissonPositions;
	Noise* noise;
	Terrain* terrain;
	float lowPolyGrassLimit;

};
#endif

