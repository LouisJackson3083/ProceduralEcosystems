#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include"Patch.h"
#include"shaderClass.h"
#include <cmath>
#include <math.h>

class Terrain {
public:
	std::vector<Patch> patches;
	int render_distance;
	int size;
	int subdivision;
	float amplitude;
	glm::vec2 cameraPosition;
	Noise* noise;
	std::vector<Texture> textures;
	Shader* terrainShader;

	// Terrain Texture stuff
	float snowStartHeight;
	float snowBlendDistance;
	float rockStartHeight;
	float rockBlendDistance;
	float slopeAmount;
	float slopeBlendDist;
	int grassTextureRenderDistance;



	Terrain(int input_size, int input_subdivision, float input_amplitude, int input_render_distance, Noise* input_noise, Shader* input_terrainShader);

	void UpdateTextureValues();

	void UpdateRenderDistance(int input_render_distance);

	void SaveTerrain(std::string file);

	void LoadTerrainData(std::string file);

	int GetPatchAt(float x, float z);

	void UpdatePatches();

	void Draw(
		Shader& shader,
		Camera& camera,
		glm::mat4 matrix = glm::mat4(1.0f),
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	);

};

#endif