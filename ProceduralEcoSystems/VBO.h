#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>
#include<iostream>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

struct PatchVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

struct GrassVertex
{
	glm::vec3 position;
};

struct PlantVertex
{
	glm::vec3 position;
};

struct TerrainVertex 
{
	float height;
	glm::vec3 normal;
};



class VBO
{
public:
	GLuint ID;
	VBO(std::vector<Vertex>& vertices);
	VBO(std::vector<PatchVertex>& vertices);
	VBO(std::vector<GrassVertex>& vertices);
	VBO(std::vector<PlantVertex>& vertices);
	VBO(std::vector<TerrainVertex>& vertices);
	VBO(std::vector<glm::vec3>& vertices);
	VBO(std::vector<glm::mat4>& mat4s);

	void Bind();
	void Unbind();
	void Delete();
};

#endif