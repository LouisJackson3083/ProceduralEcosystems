#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include"Patch.h"

class Terrain {
public:
	std::vector<Patch> patches;

	int size;
	int subdivision;
	float amplitude;
	Noise* noise;

	Terrain(int input_size, int input_subdivision, float input_amplitude, Noise* input_noise);

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