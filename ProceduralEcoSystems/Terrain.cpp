#include "Terrain.h"

Terrain::Terrain(int input_size, int input_subdivision, float input_amplitude, int input_render_distance, Noise* input_noise, Shader* input_terrainShader) {
	render_distance = input_render_distance;
	size = input_size;
	subdivision = (input_subdivision * 3) + 1;
	amplitude = input_amplitude;
	noise = input_noise;
	cameraPosition = glm::vec2(0.0f, 0.0f);
	terrainShader = input_terrainShader;

	snowStartHeight = 64.0f;
	snowBlendDistance = 5.0f;
	rockStartHeight = 48.0f;
	rockBlendDistance = 10.0f;
	slopeAmount = 0.75f;
	slopeBlendDist = 0.05f;

	textures.push_back(Texture("./Resources/Textures/snow.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/snowSpec.png", "specular", 1));
	textures.push_back(Texture("./Resources/Textures/rock.png", "diffuse", 2));
	textures.push_back(Texture("./Resources/Textures/rockSpec.png", "specular", 3));
	textures.push_back(Texture("./Resources/Textures/dirt.png", "diffuse", 4));
	textures.push_back(Texture("./Resources/Textures/dirtSpec.png", "specular", 5));

	for (int j = 0; j < (render_distance * 8) + 1; j++) {
		patches.push_back(
			Patch(
				glm::vec2{ 0.0f, 0.0f },
				glm::vec3{ 0.0f, 0.0f, 0.0f },
				40,
				4,
				0.0f,
				noise,
				{ &textures[0], &textures[1], &textures[2], &textures[3], &textures[4], &textures[5] }
			)
		);
	}

	UpdatePatches();
}

void Terrain::UpdateTextureValues() {
	terrainShader->Activate();
	glUniform1f(glGetUniformLocation(terrainShader->ID, "snow_height"), snowStartHeight);
	glUniform1f(glGetUniformLocation(terrainShader->ID, "snow_blend_distance"), snowBlendDistance);
	glUniform1f(glGetUniformLocation(terrainShader->ID, "rock_height"), rockStartHeight);
	glUniform1f(glGetUniformLocation(terrainShader->ID, "rock_blend_distance"), rockBlendDistance);
	glUniform1f(glGetUniformLocation(terrainShader->ID, "slope_amount"), slopeAmount);
	glUniform1f(glGetUniformLocation(terrainShader->ID, "slope_blend_distance"), slopeBlendDist);
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
				{ &textures[0], &textures[1], &textures[2], &textures[3], &textures[4], &textures[5] }
			)
		);
	}
}

int Terrain::GetPatchAt(float x, float z) {

	int guess = -1;
	int patchIndex = 0;
	for (int j = 0; j < render_distance; j++) {
		for (int i = 0; i < 9; i++) {
			int x2 = ((i % 3) - 1) * size * std::pow(3, j);
			int z2 = ((i / 3) - 1) * size * std::pow(3, j);
			if ((x != 0 or z != 0) and patchIndex < render_distance * 8) {
				patchIndex++;
			}
			if (copysign(1, x) != copysign(1, x2) || copysign(1, z) != copysign(1, z2)) {
				continue;
			}
			if (x2 < x || z2 < z) {
				continue;
			}
			guess = patchIndex - 1;
		}
	}
	if (guess == -1) {
		return patchIndex;
	}
	
	return guess;
}

void Terrain::SaveTerrain(std::string file) {
	std::ofstream myfile;
	myfile.open(file);
	// Save appearance variables
	myfile << noise->fastNoiseLite.GetNoiseType() << ",";
	myfile << noise->fastNoiseLite.GetFractalType() << ",";

	myfile << noise->scale << ",";
	myfile << noise->octaves << ",";
	myfile << noise->persistance << ",";
	myfile << noise->lacunarity << ",";

	myfile << size << ",";
	myfile << subdivision << ",";
	myfile << amplitude << ",";
	myfile << render_distance << ",";

	myfile << snowStartHeight << ",";
	myfile << snowBlendDistance << ",";
	myfile << rockStartHeight << ",";
	myfile << rockBlendDistance << ",";
	myfile << slopeAmount << ",";
	myfile << slopeBlendDist << ",";

	myfile.close();
}

void Terrain::LoadTerrainData(std::string file) {
	std::string line;
	std::ifstream myfile(file);
	if (myfile.is_open())
	{
		std::getline(myfile, line);
		std::vector<std::string> results;
		std::stringstream  ss(line);
		std::string str;
		while (getline(ss, str, ',')) {
			results.push_back(str);
		}

		switch (std::stoi(results[0])) {
		case 0:
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
			break;
		case 1:
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
			break;
		case 2:
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
			break;
		case 3:
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
			break;
		case 4:
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
			break;
		default:
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Value);
			break;
		}

		switch (std::stoi(results[1])) {
		case 0:
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_None);
			break;
		case 1:
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_FBm);
			break;
		case 2:
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_Ridged);
			break;
		case 3:
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_PingPong);
			break;
		case 4:
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
			break;
		default:
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_DomainWarpIndependent);
			break;
		}

		noise->updateSeed(rand());
		noise->updateNoiseValues(std::stof(results[2]), std::stof(results[3]), std::stof(results[4]), std::stof(results[5]));

		size = std::stof(results[6]);
		subdivision = std::stof(results[7]);
		amplitude = std::stof(results[8]);
		noise->amplitude = std::stof(results[8]);
		render_distance = std::stoi(results[9]);
		UpdateRenderDistance(render_distance);
		UpdatePatches();


		snowStartHeight = std::stof(results[10]);
		snowBlendDistance = std::stof(results[11]);
		rockStartHeight = std::stof(results[12]);
		rockBlendDistance = std::stof(results[13]);
		slopeAmount = std::stof(results[14]);
		slopeBlendDist = std::stof(results[15]);
		UpdateTextureValues();

		myfile.close();
	}
}
void Terrain::UpdatePatches() {

	int patchIndex = 0;
	for (int j = 0; j < render_distance; j++) {
		for (int i = 0; i < 9; i++) {
			int x = ((i % 3) - 1) * size * std::pow(3, j);
			int z = ((i / 3) - 1) * size * std::pow(3, j);

			if ((x != 0 or z != 0) and patchIndex < render_distance * 8) {
				patches[patchIndex].corner_data = glm::vec2{ (i % 3) - 1, (i / 3) - 1 };
				patches[patchIndex].offset = glm::vec3{ x + cameraPosition[0] , 0.0f, z + cameraPosition[1] };
				patches[patchIndex].amplitude = amplitude;
				patches[patchIndex].size = size * std::pow(3, j);
				patches[patchIndex].textures = { &textures[0], &textures[1], &textures[2], &textures[3], &textures[4], &textures[5] };
				patches[patchIndex].subdivision = subdivision;
				patches[patchIndex].GenerateVertices();
				patchIndex++;
			}
		}
	}

	patches[patchIndex].corner_data = glm::vec2{ 0.0f, 0.0f };
	patches[patchIndex].offset = glm::vec3{ cameraPosition[0] , 0.0f, cameraPosition[1] };
	patches[patchIndex].amplitude = amplitude;
	patches[patchIndex].size = size;
	patches[patchIndex].textures = { &textures[0], &textures[1], &textures[2], &textures[3], &textures[4], &textures[5] };
	patches[patchIndex].subdivision = subdivision;
	patches[patchIndex].GenerateVertices();
	UpdateTextureValues();
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


	// Bind shader to be able to access uniforms
	shader.Activate();
	glUniform1i(glGetUniformLocation(shader.ID, "subdivision"), subdivision);

	// Exports the camera Position to the Fragment Shader for specular lighting
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the Vertex Shader of the pyramid
	camera.Matrix(shader, "camMatrix");


	for (int i = 0; i < patches.size(); i++) {
		patches[i].Draw(shader, camera);
	}
}