#ifndef TEST_CLASS_H
#define TEST_CLASS_H

#include<vector>
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<iostream>
#include"Mesh.h"

class Test {
public:
	int vertices_num;
	std::vector<Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector<Texture> textures;
	std::vector<Mesh> meshes;

	Test(int N);

	void Draw
	(
		Shader& shader,
		Camera& camera,
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)
	);

};



#endif