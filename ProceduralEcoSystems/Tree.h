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
	// BRANCH STUFF
	float sliderBranchPitch;
	int sliderBranchSegments;
	int sliderMaxBranches;
	int sliderMinBranches;
	int sliderBranchLength;
	float sliderBendStrength;
	float sliderPitchVariance;
	float sliderBendVariance;
	float sliderLengthVariance;
	float sliderScaleVariance;
	float sliderScale;

	// ======================================
	// ECOSYSTEM STUFF
	int sliderTreeLayer;
	int sliderTreeDominance;
	float sliderTreeOxygenUpperLimit;
	float sliderTreeOxygenLowerLimit;
	float sliderTreeRootingStrength;
	float sliderTreeMoistureRequirement;
	float sliderTreeInteractionLevel;

	float sliderBranchMinHeight;

};

class Tree {
public:
	std::vector<Texture> textures;
	std::vector<std::string> tree_texture_filepaths;
	std::vector<PlantVertex> vertices;
	std::vector<GLuint> indices;
	VAO vao;

	std::vector<Texture> branchTextures;
	std::vector<std::string> branch_texture_filepaths;
	std::vector<PlantVertex> branchVertices;
	std::vector<GLuint> branchIndices;
	VAO vaoBranch;


	std::vector<glm::vec2> positions;

	Noise* noise;

	// Trunk Parameters
	int resolution;
	int vertices_per_trunk;
	int segments;
	float height;
	float radius;
	float radiusFalloff;
	float radiusFalloffRate;

	// Branch parameters
	float pitch;
	int branchSegments;
	int maxBranches;
	int minBranches;
	int branchLength;
	float bendStrength;
	float pitchVariance;
	float bendVariance;
	float lengthVariance;
	float scaleVariance;
	float scale;
	// vvvvv does not need to be stored in file
	int vertices_per_branch;
	int number_of_branches;

	// Ecosystem parameters
	int layer;
	int ecosystemDominance;
	float ecosystemOxygenUpperLimit;
	float ecosystemOxygenLowerLimit;
	float ecosystemRootingStrength;
	float ecosystemMoistureRequirement;
	float ecosystemInteractionLevel;

	float branchMinHeight;


	std::vector<int> branch_bin;

	Tree(Noise* input_noise);

	Tree(std::string file, Noise* input_noise);

	void SaveTreeData(TreeGUIData* treeGUIData, std::string file);

	TreeGUIData GetGUIData();

	void UpdateValues(TreeGUIData treeGUIData);

	void ChangeTreeTextures(const char* texture, const int type);
	void ChangeBranchTextures(const char* texture, const int type);

	void GenerateBranchBin();

	void GenerateVertices();

	void DrawTrunks(
		Shader& trunkShader,
		Camera& camera
	);

	void DrawBranches(
		Shader& branchShader,
		Camera& camera
	);

};

#endif