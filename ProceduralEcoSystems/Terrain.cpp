#include "Terrain.h"

Terrain::Terrain(int input_size, int input_subdivision, float input_amplitude, Noise* input_noise) {
	noise = input_noise;
	size = input_size;
	subdivision = input_subdivision;
	amplitude = input_amplitude;

	for (int i = 0; i < 5; i++) {
		patches.push_back(
			Patch(
				glm::vec3{i * size, -24.0f, 0.0f}, 
				size, 
				subdivision / (i + 1), 
				amplitude, 
				noise
			)
		);
	}

	UpdatePatches();
}

void Terrain::UpdatePatches() {
	for (int i = 0; i < patches.size(); i++) {
		patches[i].offset = glm::vec3{ i * size, -24.0f, 0.0f };
		patches[i].amplitude = amplitude;
		patches[i].size = size;
		patches[i].subdivision = subdivision / (i + 1);
		patches[i].UpdateMesh();
	}
}


void Terrain::Draw
(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
)
{
	for (int i = 0; i < patches.size(); i++) {
		patches[i].Draw(shader, camera);
	}
}