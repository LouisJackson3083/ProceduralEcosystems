#ifndef ECOSYSTEM_CLASS_H
#define ECOSYSTEM_CLASS_H

#include<vector>
#include<glm/glm.hpp>
#include"FastPoissonDiskSampling.h"												

class Ecosystem
{
public:
	Ecosystem(int id);

	void GeneratePoissonPositions();

	std::vector<float> layerRadii;

	std::vector<std::vector<glm::vec2>> poissonPositions;
};
#endif
