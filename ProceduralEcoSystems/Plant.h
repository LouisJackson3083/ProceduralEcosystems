#ifndef PLANT_CLASS_H
#define PLANT_CLASS_H

#include<vector>
#include<glad/glad.h>
#include<glm/glm.hpp>
#include"Texture.h"
#include"Camera.h"
#include"EBO.h"
#include"VBO.h"
#include"VAO.h"
#include <random>
#include<string>

struct PlantGUIData
{

	// Direction Sliders
	float sliderPlantPitch;
	float sliderPlantBendStrength;
	float sliderPlantYaw;
	float sliderPlantPitchVariance;
	float sliderPlantBendVariance;

	// Length Sliders
	int sliderPlantSegments;
	int sliderPlantLeafLength;
	float sliderPlantLengthVariance;

	// Scale sliders
	float sliderPlantScaleVariance;
	float sliderPlantScale;

	// Leaf Sliders
	int sliderPlantMaxLeaves;
	int sliderPlantMinLeaves;

	int sliderPlantLayer;
};

class Plant {
public:

	std::vector<Texture> textures;
	std::vector<std::string> plant_texture_filepaths;
	std::vector<PlantVertex> vertices;
	std::vector<GLuint> indices;
	VAO vao;

	int segments;
	int vertices_per_leaf;
	int leafLength;

	int maxLeaves;
	int minLeaves;

	float pitch;
	float yaw;
	float bendStrength;

	float lengthVariance;
	float pitchVariance;
	float bendVariance;
	float scaleVariance;
	float scale;

	int layer;
	int number_of_leaves;

	std::vector<int> plant_bin;
	std::vector<glm::vec2> positions;
	Noise* noise;

	Plant(Noise* input_noise);

	Plant(std::string file, Noise* input_noise);

	void UpdateValues(PlantGUIData plantGuiData);

	void ChangeTextures(const char* texture, const int type);

	void GeneratePlantBin();

	void GenerateVertices();

	void Draw(
		Shader& shader,
		Camera& camera
	);
};

#endif