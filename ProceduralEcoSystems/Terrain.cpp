#include "Terrain.h"

Terrain::Terrain(Noise* input_noise, int input_size) {
	noise = input_noise;
	size = input_size;
	chunkList.resize(1);
}

void Terrain::GenerateTerrainMesh() {
	Chunk chunk = Chunk(size);
	int vertexIndex = 0;
	float offset = (size - 1) / 2.0f;

	for (int x = 0; x < size; x++) {
		for (int z = 0; z < size; z++) {
			chunk.vertices[vertexIndex] = glm::vec3{ offset+x, noise->get(x, z), offset-z };

			//std::cout << vertexIndex << ": " << offset + x << ", " << noise->get(x, z) << ", " << offset - z << std::endl;
			//std::cout << vertexIndex << ": " << chunk.vertices[vertexIndex].x << ", " << chunk.vertices[vertexIndex].y << ", " << chunk.vertices[vertexIndex].z << std::endl;

			chunk.uvs[vertexIndex] = glm::vec2{ x / (float)size, z / (float)size };

			if (x < size - 1 && z < size - 1) {
				chunk.AddTriangle(vertexIndex, vertexIndex + size + 1, vertexIndex + size);
				chunk.AddTriangle(vertexIndex + size + 1, vertexIndex, vertexIndex + 1);
			}
			vertexIndex++;
		}
	}

	chunk.CreateMesh();

	chunkList[0] = chunk;
}

void Terrain::DrawTerrain(Shader& shader, Camera& camera) {
	chunkList[0].Draw(shader, camera);
}