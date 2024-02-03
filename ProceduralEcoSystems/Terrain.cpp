#include "Terrain.h"

Terrain::Terrain(int input_render_distance, int input_size, int input_subdivision, float input_amplitude, Noise* input_noise) {
	render_distance = input_render_distance;
	size = input_size;
	subdivision = input_subdivision * 3;
	amplitude = input_amplitude;
	noise = input_noise;

	textures.push_back(Texture("./Resources/Textures/dirt.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/dirtSpec.png", "specular", 1));
	textures.push_back(Texture("./Resources/Textures/grass.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/grassSpec.png", "specular", 1));


	std::cout << render_distance*8 << std::endl;
	int patchIndex = 0;
	for (int j = 0; j < render_distance*8; j++) {
		patches.push_back(
			Patch(
				glm::vec2(0.0f, 0.0f),
				glm::vec3{ 0.0f, 0.0f, 0.0f },
				size,
				subdivision,
				amplitude,
				noise,
				{ &textures[0], &textures[1] }
			)
		);
	}

	UpdatePatches();
}

void Terrain::UpdateRenderDistance(int input_render_distance) {
	render_distance = input_render_distance;
	patches.clear();

	for (int j = 0; j < render_distance * 8; j++) {
		patches.push_back(
			Patch(
				glm::vec2(0.0f, 0.0f),
				glm::vec3{ 0.0f, 0.0f, 0.0f },
				size,
				subdivision,
				amplitude,
				noise,
				{ &textures[0], &textures[1] }
			)
		);
	}
}

void Terrain::UpdatePatches() {

	int patchIndex = 0;
	for (int j = 0; j < render_distance; j++) {
		for (int i = 0; i < 9; i++) {
			int x = ((i % 3) - 1) * size * std::pow(3, j);
			int z = ((i / 3) - 1) * size * std::pow(3, j);
			if (x != 0 or z != 0) {
				patches[patchIndex].corner_data = glm::vec2{ (i % 3) - 1 , (i / 3) - 1 };
				patches[patchIndex].offset = glm::vec3{ x, -24.0f, z };
				patches[patchIndex].amplitude = amplitude;
				patches[patchIndex].size = size * std::pow(3, j);
				patches[patchIndex].textures = { &textures[2], &textures[3] };
				patches[patchIndex].subdivision = subdivision;
				patches[patchIndex].UpdateMesh();
				patchIndex++;
			}
		}
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