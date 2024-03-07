#ifndef PATCH_CLASS_H
#define PATCH_CLASS_H

#include<vector>
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<iostream>
#include"Texture.h"
#include"Camera.h"
#include"EBO.h"
#include"VBO.h"
#include"VAO.h"
#include"Noise.h"


class Patch {
public:

	Noise* noise;
	std::vector<Texture*> textures;
	std::vector<TerrainVertex> vertices;
	std::vector<GLuint> indices;
	VAO VAO;

	int size; // the size in metres of the whole patch, where
	// 1 = 1m x 1m patch
	// 2 = 2m x 2m patch
	int subdivision; // how many times the patch should be subdivided, where
	// 2 = 2x2 quad patch ,
	// 3 = 3x3 quad patch ,
	// 4 = 4x4 quad patch ...
	float amplitude;
	glm::vec3 offset;
	glm::vec2 corner_data;

	Patch(
		glm::vec2 input_corner_data,
		glm::vec3 input_offset,
		int input_size,
		int input_subdivision,
		float input_amplitude,
		Noise* input_noise,
		std::vector<Texture*> input_textures
	);

	void GenerateVertices();


	float GetHeight(int vertexID, float scalar);

	void Draw(
		Shader& shader,
		Camera& camera
	);

};



#endif