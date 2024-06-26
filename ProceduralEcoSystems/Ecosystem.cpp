#include "Ecosystem.h"

Ecosystem::Ecosystem(Grass* input_grass, std::vector<Plant>* input_plants, std::vector<Tree>* input_trees, Noise* input_noise, Terrain* input_terrain) {
	plants = input_plants;
	trees = input_trees;
	noise = input_noise;
	terrain = input_terrain;
	grass = input_grass;
	layerRadii = { 10.0f, 5.0f, 5.0f, 0.1f };
	lowPolyGrassLimit = 256.0f;
	terrainSizeLOCAL = 0.0f;
}

void Ecosystem::GeneratePoissonPositions(float terrainSize, float grassSize1, float grassSize2) {
	poissonPositions.clear();
	RecalculateLayers();

	lowPolyGrassLimit = grassSize1;
	terrainSizeLOCAL = terrainSize;

	auto kXMin = std::array<float, 2>{{-terrainSize, -terrainSize}};
	auto kXMax = std::array<float, 2>{{terrainSize, terrainSize}};

	auto grass_kXMin1 = std::array<float, 2>{{-grassSize1, -grassSize1}};
	auto grass_kXMax1 = std::array<float, 2>{{grassSize1, grassSize1}};
	auto grass_kXMin2 = std::array<float, 2>{{-grassSize2, -grassSize2}};
	auto grass_kXMax2 = std::array<float, 2>{{grassSize2, grassSize2}};

	std::cout << "\n---- BEGIN SAMPLING " << std::endl;

	// Minimal amount of information provided to sampling function.
	for (int i = 0; i < 3; i++) {
		std::cout << "---- SAMPLING LAYER" << i+1 << std::endl;
		const auto samples = thinks::PoissonDiskSampling(layerRadii[i], kXMin, kXMax);
		std::vector<glm::vec2> currPositions;

		for (const auto& sample : samples) {
			currPositions.push_back(glm::vec2(sample[0], sample[1]));
		}
		poissonPositions.push_back(currPositions);
	}

	// Grass stuff
	std::cout << "---- SAMPLING LAYER" << 4 << std::endl;
	const auto samples = thinks::PoissonDiskSampling(layerRadii[3], grass_kXMin1, grass_kXMax1);
	std::vector<glm::vec2> currPositions;
	for (const auto& sample : samples) {

		currPositions.push_back(glm::vec2(sample[0], sample[1]));
	}
	poissonPositions.push_back(currPositions);

	if (grass_kXMin1 != grass_kXMin2) {
		std::cout << "---- SAMPLING LAYER" << 5 << std::endl;
		const auto samples2 = thinks::PoissonDiskSampling(layerRadii[3] * 2.0f, grass_kXMin2, grass_kXMax2);
		currPositions.clear();
		for (const auto& sample : samples2) {

			currPositions.push_back(glm::vec2(sample[0], sample[1]));
		}

		poissonPositions.push_back(currPositions);
	}
	else {
		poissonPositions.push_back(currPositions);
	}

	std::cout << "---- FINISHED SAMPLING \n" << std::endl;


}

void Ecosystem::RecalculateLayers() {
	layerIndices.clear();
	std::vector<int> layer0Indices;
	std::vector<int> layer1Indices;
	std::vector<int> layer2Indices;

	for (int i = 0; i < plants->size(); i++) {
		(*plants)[i].positions.clear();
		switch ((*plants)[i].layer) {
		case 1:
			layer1Indices.push_back(i);
			break;
		default:
			layer2Indices.push_back(i);
			break;
		}
	}

	for (int i = 0; i < trees->size(); i++) {
		(*trees)[i].positions.clear();
		layer0Indices.push_back(i);
	}

	grass->positions.clear();
	grass->positionsLowPoly.clear();

	layerIndices.push_back(layer0Indices);
	layerIndices.push_back(layer1Indices);
	layerIndices.push_back(layer2Indices);
}

void Ecosystem::DistributePositions() {
	float terrainScalar = terrain->patches[0].normalOffset;

	// Handle tree distribution
	if (layerIndices[0].size() != 0) {
		for (int p = 0; p < poissonPositions[0].size(); p++) {
			//terrainScalar = terrain->patches[terrain->GetPatchAt(poissonPositions[0][p][0], poissonPositions[0][p][1])].normalOffset / 2.0f;
			glm::vec2 heightAndGradient = noise->EcosystemGetHeightAndGradients(poissonPositions[0][p][0], poissonPositions[0][p][1], terrainScalar, terrain->amplitude);
			std::vector<int> potential_trees;

			for (int j = 0; j < layerIndices[0].size(); j++) {
				if ((*trees)[layerIndices[0][j]].ecosystemRootingStrength > heightAndGradient[1] ||
					(*trees)[layerIndices[0][j]].ecosystemOxygenLowerLimit > heightAndGradient[0] ||
					(*trees)[layerIndices[0][j]].ecosystemOxygenUpperLimit < heightAndGradient[0]
					) {
					continue;
				}

				for (int d = 0; d < (*trees)[layerIndices[0][j]].ecosystemDominance; d++) {
					potential_trees.push_back(layerIndices[0][j]);
				}
			}
			if (!potential_trees.empty()) {
				int selection = potential_trees[rand() % potential_trees.size()];
				(*trees)[selection].positions.push_back(poissonPositions[0][p]);

				// Cluster stuff
				float clusterChance = (rand() % 1000) / 1000.0f;
				if ((*trees)[selection].ecosystemPropagationAmount >= clusterChance) {

					float angle = rand() % 6;
					float length = (*trees)[selection].radius * 2.0f + (2.0f * (*trees)[selection].ecosystemPropagationDistance);
					int numClusters = (rand() % 3) + 1;
					float angleSector = 6.283 / (float)numClusters;

					for (int c = 0; c < numClusters; c++) {
						float rnd_seed = (rand() % 1000) / 1000;
						float a = (angle + (angleSector * c) + (2.0f * rnd_seed - 0.5f));
						float l = length + (2.0f * (*trees)[selection].ecosystemPropagationDistance * (rnd_seed - 0.5f));

						glm::vec2 clusterPosition = glm::vec2(
							poissonPositions[0][p][0] + l * cos(a),
							poissonPositions[0][p][1] + l * sin(a)
						);
						if (std::abs(clusterPosition[0]) < terrainSizeLOCAL && std::abs(clusterPosition[1]) < terrainSizeLOCAL) {
							(*trees)[selection].positions.push_back(clusterPosition);
						}
					}
				}
			}
		}
	}

	// Handle plants and small plant distribution
	for (int i = 1; i < poissonPositions.size() - 2; i++) {
		if (layerIndices[i].size() == 0) { continue; }
		for (int p = 0; p < poissonPositions[i].size(); p++) {
			//terrainScalar = terrain->patches[terrain->GetPatchAt(poissonPositions[0][p][0], poissonPositions[0][p][1])].normalOffset / 2.0f;
			glm::vec2 heightAndGradient = noise->EcosystemGetHeightAndGradients(poissonPositions[i][p][0], poissonPositions[i][p][1], terrainScalar, terrain->amplitude);
			std::vector<int> potential_plants;


			for (int j = 0; j < layerIndices[i].size(); j++) {
				if ((*plants)[layerIndices[i][j]].ecosystemRootingStrength > heightAndGradient[1] ||
					(*plants)[layerIndices[i][j]].ecosystemOxygenLowerLimit > heightAndGradient[0] ||
					(*plants)[layerIndices[i][j]].ecosystemOxygenUpperLimit < heightAndGradient[0]
					) {
					continue;
				}
				for (int d = 0; d < (*plants)[layerIndices[i][j]].ecosystemDominance; d++) {
					potential_plants.push_back(layerIndices[i][j]);
				}
				
			}

			if (!potential_plants.empty()) {
				int selection = potential_plants[rand() % potential_plants.size()];
				(*plants)[selection].positions.push_back(poissonPositions[i][p]);

				// Cluster stuff
				float clusterChance = (rand() % 1000) / 1000.0f;
				if ((*plants)[selection].ecosystemPropagationAmount >= clusterChance) {

					float angle = rand() % 6;
					float length = (*plants)[selection].leafLength * (*plants)[selection].scale + (2.0f * (*plants)[selection].ecosystemPropagationDistance);
					int numClusters = (rand() % 3) + 1;
					float angleSector = 6.283 / (float)numClusters;

					for (int c = 0; c < numClusters; c++) {
						float rnd_seed = (rand() % 1000) / 1000;
						float a = (angle + (angleSector * c) + (2.0f * rnd_seed - 0.5f));
						float l = length + (2.0f * (*plants)[selection].ecosystemPropagationDistance * (rnd_seed - 0.5f));

						glm::vec2 clusterPosition = glm::vec2(
							poissonPositions[i][p][0] + l * cos(a),
							poissonPositions[i][p][1] + l * sin(a)
						);
						if (std::abs(clusterPosition[0]) < terrainSizeLOCAL && std::abs(clusterPosition[1]) < terrainSizeLOCAL) {
							(*plants)[selection].positions.push_back(clusterPosition);
						}
					}
				}
			}
			
		}
	}

	// Handle grass distribution
	for (int p = 0; p < poissonPositions[3].size(); p++) {
		glm::vec2 heightAndGradient = noise->EcosystemGetHeightAndGradients(poissonPositions[3][p][0], poissonPositions[3][p][1], terrainScalar, terrain->amplitude);
		if (grass->ecosystemRootingStrength > heightAndGradient[1] ||
			grass->ecosystemOxygenLowerLimit > heightAndGradient[0] ||
			grass->ecosystemOxygenUpperLimit < heightAndGradient[0]
			) {
			continue;
		}
		grass->positions.push_back(poissonPositions[3][p]);


		// Cluster stuff
		float clusterChance = (rand() % 1000) / 1000.0f;
		if (grass->ecosystemPropagationAmount >= clusterChance) {

			float angle = rand() % 6;
			float length = grass->scale + (2.0f * grass->ecosystemPropagationDistance);
			int numClusters = (rand() % 3) + 1;
			float angleSector = 6.283 / (float)numClusters;

			for (int c = 0; c < numClusters; c++) {
				float rnd_seed = (rand() % 1000) / 1000;
				float a = (angle + (angleSector * c) + (2.0f * rnd_seed - 0.5f));
				float l = length + (2.0f * grass->ecosystemPropagationDistance * (rnd_seed - 0.5f));

				glm::vec2 clusterPosition = glm::vec2(
					poissonPositions[3][p][0] + l * cos(a),
					poissonPositions[3][p][1] + l * sin(a)
				);
				if (std::abs(clusterPosition[0]) < terrainSizeLOCAL && std::abs(clusterPosition[1]) < terrainSizeLOCAL) {
					grass->positions.push_back(clusterPosition);
				}
			}
		}
	}
	// Handle grass distribution
	for (int p = 0; p < poissonPositions[4].size(); p++) {
		if (poissonPositions[4][p][0] <= lowPolyGrassLimit and poissonPositions[4][p][0] >= -lowPolyGrassLimit and poissonPositions[4][p][1] <= lowPolyGrassLimit and poissonPositions[4][p][1] >= -lowPolyGrassLimit) { continue; }
		glm::vec2 heightAndGradient = noise->EcosystemGetHeightAndGradients(poissonPositions[4][p][0], poissonPositions[4][p][1], terrainScalar, terrain->amplitude);
		if (grass->ecosystemRootingStrength > heightAndGradient[1] ||
			grass->ecosystemOxygenLowerLimit > heightAndGradient[0] ||
			grass->ecosystemOxygenUpperLimit < heightAndGradient[0]
			) {
			continue;
		}
		grass->positionsLowPoly.push_back(poissonPositions[4][p]);


		// Cluster stuff
		float clusterChance = (rand() % 1000) / 1000.0f;
		if (grass->ecosystemPropagationAmount >= clusterChance) {

			float angle = rand() % 6;
			float length = grass->scale + (2.0f * grass->ecosystemPropagationDistance);
			int numClusters = (rand() % 3) + 1;
			float angleSector = 6.283 / (float)numClusters;

			for (int c = 0; c < numClusters; c++) {
				float rnd_seed = (rand() % 1000) / 1000;
				float a = (angle + (angleSector * c) + (2.0f * rnd_seed - 0.5f));
				float l = length + (2.0f * grass->ecosystemPropagationDistance * (rnd_seed - 0.5f));

				glm::vec2 clusterPosition = glm::vec2(
					poissonPositions[4][p][0] + l * cos(a),
					poissonPositions[4][p][1] + l * sin(a)
				);
				if (std::abs(clusterPosition[0]) < terrainSizeLOCAL && std::abs(clusterPosition[1]) < terrainSizeLOCAL) {
					grass->positionsLowPoly.push_back(clusterPosition);
				}
			}
		}
	}

	for (int i = 0; i < plants->size(); i++) {
		(*plants)[i].GeneratePlantBin();
		(*plants)[i].GenerateVertices();
	}

	for (int i = 0; i < trees->size(); i++) {
		(*trees)[i].GenerateBranchBin();
		(*trees)[i].GenerateVertices();
	}

	grass->GenerateVertices();
}