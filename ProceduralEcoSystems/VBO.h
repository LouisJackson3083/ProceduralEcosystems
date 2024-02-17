#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>
#include<iostream>

// Structure to standardize the vertices used in the meshes
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

// Structure to standardize the vertices used in the patches
struct PatchVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

// Structure to standardize the vertices used in the patches
struct GrassVertex
{
	glm::vec3 position;
};

struct StupidVertex 
{
	float height;
};



class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(std::vector<Vertex>& vertices);
	VBO(std::vector<PatchVertex>& vertices);
	VBO(std::vector<GrassVertex>& vertices);
	VBO(std::vector<StupidVertex>& vertices);
	VBO(std::vector<glm::vec3>& vertices);
	VBO(std::vector<glm::mat4>& mat4s);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif