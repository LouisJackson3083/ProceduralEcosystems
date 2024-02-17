#ifndef NEWTERRAIN_CLASS_H
#define NEWTERRAIN_CLASS_H

#include"NewPatch.h"
#include <cmath>

class NewTerrain {
public:
	std::vector<NewPatch> patches;
	int render_distance;
	int size;
	int subdivision;
	float amplitude;
	bool useErosion;
	glm::vec2 cameraPosition;
	Noise* noise;
	std::vector<Texture> textures;

	NewTerrain(int input_size, int input_subdivision, float input_amplitude, int input_render_distance, Noise* input_noise);

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