#include "Terrain.h"

Terrain::Terrain(int input_size, int input_subdivision, float input_amplitude, int input_render_distance, Noise* input_noise) {
	render_distance = input_render_distance;
	size = input_size;
	subdivision = input_subdivision * 3;
	amplitude = input_amplitude;
	noise = input_noise;
	cameraPosition = glm::vec2(0.0f, 0.0f);
	useErosion = false;

	textures.push_back(Texture("./Resources/Textures/dirt.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/dirtSpec.png", "specular", 1));
	textures.push_back(Texture("./Resources/Textures/grass.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/grassSpec.png", "specular", 1));
	textures.push_back(Texture("./Resources/Textures/pop_cat.png", "diffuse", 0));

	int patchIndex = 0;
	for (int j = 0; j < (render_distance * 8) + 1; j++) {
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
		patchIndex++;
	}

	UpdatePatches();
}

void Terrain::UpdateRenderDistance(int input_render_distance) {
	render_distance = input_render_distance;
	patches.clear();

	for (int j = 0; j < (render_distance * 8) + 1; j++) {
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

void Terrain::UpdateErosion() {
	// Update noise value erosion
	if (useErosion = false) {
		noise->erosionMapSize = std::max(256, (3 * subdivision * std::min(render_distance, 3)));
		noise->generateErosionMap();
	}
	UpdatePatches();
}

void Terrain::UpdatePatches() {

	int patchIndex = 0;
	for (int j = 0; j < render_distance; j++) {
		for (int i = 0; i < 9; i++) {
			int x = ((i % 3) - 1) * size * std::pow(3, j);
			int z = ((i / 3) - 1) * size * std::pow(3, j);
			if ((x != 0 or z != 0) and patchIndex < render_distance * 8) {
				patches[patchIndex].corner_data = glm::vec2{ (i % 3) - 1, (i / 3) - 1 };
				patches[patchIndex].offset = glm::vec3{ x + cameraPosition[0] , -24.0f, z + cameraPosition[1] };
				patches[patchIndex].amplitude = amplitude;
				patches[patchIndex].size = size * std::pow(3, j);
				patches[patchIndex].textures = { &textures[2], &textures[3] };
				patches[patchIndex].subdivision = subdivision;
				patches[patchIndex].useErosion = useErosion;
				patches[patchIndex].UpdateMesh();
				patchIndex++;
			}
		}
	}

	patches[patchIndex].corner_data = glm::vec2{ 0.0f, 0.0f };
	patches[patchIndex].offset = glm::vec3{ cameraPosition[0] , -24.0f, cameraPosition[1] };
	patches[patchIndex].amplitude = amplitude;
	patches[patchIndex].size = size;
	patches[patchIndex].textures = { &textures[2], &textures[3] };
	patches[patchIndex].subdivision = subdivision;
	patches[patchIndex].useErosion = useErosion;
	patches[patchIndex].UpdateMesh();

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

	// Handle camera anchor position
	/*float distance = std::pow(camera.Position[0] - camera.anchorPosition[0], 2.0f) + std::pow(camera.Position[2] - camera.anchorPosition[2], 2.0f);
	if (distance > (float)size/9.0f) {
		camera.anchorPosition = camera.Position;
		cameraPosition = glm::vec2(camera.Position[0], camera.Position[2]);
		UpdatePatches();
	}*/

	for (int i = 0; i < patches.size(); i++) {
		patches[i].Draw(shader, camera);
	}
}