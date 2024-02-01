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
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Texture texture = Texture("./Resources/Textures/pop_cat.png", "diffuse", 0);
	VAO VAO;

	int size;
	int subdivision;
	float amplitude;

	Patch(int input_size, int input_subdivision, float input_amplitude, Noise* input_noise);

	void UpdateMesh();

	void Draw(
		Shader& shader,
		Camera& camera,
		glm::mat4 matrix = glm::mat4(1.0f),
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	);

};



#endif