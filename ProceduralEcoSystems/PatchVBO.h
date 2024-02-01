#ifndef PATCHVBO_CLASS_H
#define PATCHVBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>
#include<iostream>

// Structure to standardize the vertices used in the meshes
struct PatchVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texUV;
};



class PatchVBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	PatchVBO(std::vector<PatchVertex>& vertices);
	PatchVBO(std::vector<glm::vec3>& vertices);
	PatchVBO(std::vector<glm::mat4>& mat4s);

	// Binds the PatchVBO
	void Bind();
	// Unbinds the PatchVBO
	void Unbind();
	// Deletes the PatchVBO
	void Delete();
};

#endif