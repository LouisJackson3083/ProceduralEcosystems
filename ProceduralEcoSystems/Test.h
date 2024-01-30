#ifndef TEST_CLASS_H
#define TEST_CLASS_H

#include<vector>
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<iostream>
#include"Mesh.h"
#include"EBO.h"
#include"VBO.h"
#include"VAO.h"

class Test {
public:

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Texture texture = Texture("./Resources/Textures/pop_cat.png", "diffuse", 0);
	VAO VAO1;

	Test(int N);


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