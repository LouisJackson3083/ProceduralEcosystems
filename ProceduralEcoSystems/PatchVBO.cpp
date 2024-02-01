#include"PatchVBO.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
PatchVBO::PatchVBO(std::vector<PatchVertex>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PatchVertex), vertices.data(), GL_STATIC_DRAW);
}

PatchVBO::PatchVBO(std::vector<glm::vec3>& vertices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
}

PatchVBO::PatchVBO(std::vector<glm::mat4>& mat4s)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, mat4s.size() * sizeof(glm::mat4), mat4s.data(), GL_STATIC_DRAW);
}

// Binds the PatchVBO
void PatchVBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the PatchVBO
void PatchVBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the PatchVBO
void PatchVBO::Delete()
{
	glDeleteBuffers(1, &ID);
}