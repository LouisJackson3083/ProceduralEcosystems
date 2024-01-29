#include"Chunk.h"

Chunk::Chunk() {}

Chunk::Chunk(int input_size) {
	size = input_size;
	vertices.resize(size * size);
	indices.resize((size - 1) * (size - 1) * 6);
}

void Chunk::AddTriangle(int a, int b, int c) {
	indices[triangleIndex] = a;
	indices[triangleIndex+1] = b;
	indices[triangleIndex+2] = c;
	triangleIndex += 3;
}

void Chunk::CreateMesh() {
	Texture textures[]
	{
		Texture("./Resources/Textures/pop_cat.png", "diffuse", 0)
	};

	//Mesh chunkMesh = Mesh(vertices, indices, textures, 1, {});
}


void Chunk::Draw(Shader& shader, Camera& camera) {
}