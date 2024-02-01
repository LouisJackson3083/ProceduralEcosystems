#include "Terrain.h"

Terrain::Terrain(int input_size, int input_subdivision, float input_amplitude, Noise* input_noise) {
	noise = input_noise;
	size = input_size;
	subdivision = input_subdivision;
	amplitude = input_amplitude;

	patches.push_back(Patch(size, subdivision, amplitude, noise));

	UpdatePatches();
}

void Terrain::UpdatePatches() {
	for (int i = 0; i < patches.size(); i++) {
		patches[i].amplitude = amplitude;
		patches[i].size = size;
		patches[i].subdivision = subdivision;
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