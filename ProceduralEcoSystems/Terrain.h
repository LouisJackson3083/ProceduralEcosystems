#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include"Mesh.h"
#include"Chunk.h"
#include<vector>
#include<glm/glm.hpp>

class Terrain
{
public:
	Noise* noise;
	int size;
	std::vector <Chunk> chunkList;

	Terrain(Noise* input_noise, int input_size);

	void DrawTerrain(Shader& shader, Camera& camera);
	void GenerateTerrainMesh();
};
#endif