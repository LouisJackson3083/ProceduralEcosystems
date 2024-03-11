#ifndef TREE_CLASS_H
#define TREE_CLASS_H

#include"Texture.h"
#include"Camera.h"
#include"EBO.h"
#include"VBO.h"
#include"VAO.h"
#include<random>

struct TreeGUIData
{
	// ======================================
	// APPEARANCE STUFF
	int sliderTreeResolution;
	int sliderTreeSegments;
	float sliderTreeHeight;
	float sliderTreeRadius;
	float sliderTreeRadiusFalloff;
	float sliderTreeRadiusFalloffRate;

	// ======================================
	// ECOSYSTEM STUFF
	int sliderTreeLayer;
	int sliderTreeDominance;
	float sliderTreeOxygenUpperLimit;
	float sliderTreeOxygenLowerLimit;
	float sliderTreeRootingStrength;
	float sliderTreeMoistureRequirement;
	float sliderTreeInteractionLevel;

};

class Tree {
public:
	std::vector<Texture> textures;
	std::vector<std::string> tree_texture_filepaths;
	std::vector<PlantVertex> vertices;
	std::vector<GLuint> indices;
	VAO vao;

	std::vector<glm::vec2> positions;

	Noise* noise;
	int resolution;
	int vertices_per_trunk;
	int segments;
	float height;
	float radius;
	float radiusFalloff;
	float radiusFalloffRate;

	// Ecosystem parameters
	int layer;
	int ecosystemDominance;
	float ecosystemOxygenUpperLimit;
	float ecosystemOxygenLowerLimit;
	float ecosystemRootingStrength;
	float ecosystemMoistureRequirement;
	float ecosystemInteractionLevel;

	Tree(Noise* input_noise);

	Tree(std::string file, Noise* input_noise);

	void SaveTreeData(TreeGUIData* treeGUIData, std::string file);

	TreeGUIData GetGUIData();

	void UpdateValues(TreeGUIData treeGUIData);

	void ChangeTextures(const char* texture, const int type);

	void GenerateVertices();

	void Draw(
		Shader& trunkShader,
		Shader& branchShader,
		Camera& camera
	);

};

#endif