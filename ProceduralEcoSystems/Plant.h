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
	int sliderPlantNumber;

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
};

class Plant {
public:

	std::vector<Texture> textures;
	std::vector<std::string> plant_texture_filepaths;
	std::vector<PlantVertex> vertices;
	std::vector<GLuint> indices;
	VAO vao;

	int plantNumber;

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
	

	int number_of_leaves;
	std::vector<int> plant_bin;

	Plant(int id);

	Plant(PlantGUIData plantGuiData, std::string texDiffuse, std::string texSpecular);

	void ChangeTextures(const char* texture, const int type);

	void GeneratePlantBin();

	void GenerateVertices();

	void Draw(
		Shader& shader,
		Camera& camera
	);
};

#endif