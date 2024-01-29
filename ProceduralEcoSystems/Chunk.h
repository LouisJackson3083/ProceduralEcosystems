#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include<glad/glad.h>
#include<glm/glm.hpp>
#include<vector>
#include"Mesh.h"
#include"Texture.h"
#include"VAO.h"
#include"EBO.h"
#include"Noise.h"
#include"Camera.h"


class Chunk {
public:

	int size;
	int triangleIndex;
	
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;

	//std::vector <glm::vec3> vertices;
	//std::vector <GLuint> triangles;
	//std::vector <glm::vec2> uvs;

	// VAO VAO;

	Chunk();
	Chunk(int input_size);
	void AddTriangle(int a, int b, int c);
	void CreateMesh();
	void Draw(Shader& shader, Camera& camera);
};

#endif