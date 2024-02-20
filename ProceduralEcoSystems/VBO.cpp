#include"VBO.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

// Constructor that generates a Patch Vertex Buffer Object and links it to vertices
VBO::VBO(std::vector<PatchVertex>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PatchVertex), vertices.data(), GL_STATIC_DRAW);
}

// Constructor that generates a Grass Vertex Buffer Object and links it to vertices
VBO::VBO(std::vector<GrassVertex>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GrassVertex), vertices.data(), GL_STATIC_DRAW);
}

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(std::vector<StupidVertex>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StupidVertex), vertices.data(), GL_STATIC_DRAW);
}

VBO::VBO(std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
}

VBO::VBO(std::vector<glm::mat4>& mat4s)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, mat4s.size() * sizeof(glm::mat4), mat4s.data(), GL_STATIC_DRAW);
}

// Binds the VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}