#include "Ecosystem.h"

Ecosystem::Ecosystem(std::vector<Plant>* input_plants, Noise* input_noise, Terrain* input_terrain) {
	plants = input_plants;
	noise = input_noise;
	terrain = input_terrain;
	layerRadii = { 30.0f, 20.0f, 15.0f };
}

void Ecosystem::GeneratePoissonPositions(float terrainSize) {
	poissonPositions.clear();
	RecalculateLayers();

	auto kXMin = std::array<float, 2>{{-terrainSize, -terrainSize}};
	auto kXMax = std::array<float, 2>{{terrainSize, terrainSize}};

	// Minimal amount of information provided to sampling function.
	for (int i = 0; i < 3; i++) {
		const auto samples = thinks::PoissonDiskSampling(layerRadii[i], kXMin, kXMax);

		/*class std::array<float, 2>
		class std::vector<class std::array<float, 2>, class std::allocator<class std::array<float, 2> > >*/

		std::vector<glm::vec2> currPositions;

		for (const auto& sample : samples) {
			currPositions.push_back(glm::vec2(sample[0], sample[1]));
		}
		poissonPositions.push_back(currPositions);
	}
}

void Ecosystem::RecalculateLayers() {
	layerIndices.clear();
	std::vector<int> layer0Indices;
	std::vector<int> layer1Indices;
	std::vector<int> layer2Indices;

	for (int i = 0; i < plants->size(); i++) {
		(*plants)[i].positions.clear();
		switch ((*plants)[i].layer) {
			case 0:
				layer0Indices.push_back(i);
				break;
			case 1:
				layer1Indices.push_back(i);
				break;
			default:
				layer2Indices.push_back(i);
				break;
		}
	}
	layerIndices.push_back(layer0Indices);
	layerIndices.push_back(layer1Indices);
	layerIndices.push_back(layer2Indices);
}

void Ecosystem::DistributePositions() {
	float terrainScalar = (float)terrain->size / ((float)terrain->subdivision - 1);
	std::cout << "TerrainScalar " << terrainScalar << std::endl;

	for (int i = 0; i < poissonPositions.size(); i++) {
		if (layerIndices[i].size() == 0) { continue; }
		for (int p = 0; p < poissonPositions[i].size(); p++) {
			glm::vec3 heightAndGradient = noise->EcosystemGetHeightAndGradients(poissonPositions[i][p][0], poissonPositions[i][p][1], terrainScalar, terrain->amplitude);
			std::vector<int> potential_plants;

			for (int j = 0; j < layerIndices[i].size(); j++) {
				if ((*plants)[layerIndices[i][j]].ecosystemRootingStrength < heightAndGradient[2] ||
					(*plants)[layerIndices[i][j]].ecosystemRootingStrength < heightAndGradient[1] ||
					(*plants)[layerIndices[i][j]].ecosystemOxygenLowerLimit > heightAndGradient[0] / terrain->amplitude ||
					(*plants)[layerIndices[i][j]].ecosystemOxygenUpperLimit < heightAndGradient[0] / terrain->amplitude
					) {
					continue;
				}
				potential_plants.push_back(layerIndices[i][j]);
			}

			if (!potential_plants.empty()) {
				int selection = potential_plants[rand() % potential_plants.size()];
				(*plants)[selection].positions.push_back(poissonPositions[i][p]);
			}
			
		}
	}

	for (int i = 0; i < plants->size(); i++) {
		(*plants)[i].GeneratePlantBin();
		(*plants)[i].GenerateVertices();
	}
}