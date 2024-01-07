#include"Chunk.h"

Chunk::Chunk() {}

Chunk::Chunk(int input_size) {
	size = input_size;
	vertices.resize(size * size);
	uvs.resize(size * size);
	triangles.resize((size - 1) * (size - 1) * 6);
}

void Chunk::AddTriangle(int a, int b, int c) {
	triangles[triangleIndex] = a;
	triangles[triangleIndex+1] = b;
	triangles[triangleIndex+2] = c;
	triangleIndex += 3;
}

void Chunk::CreateMesh() {
	// Create VBO for vertices
	VBO VBO(vertices);
	EBO EBO(triangles);
	// Link VBO attributes to VAO
	VAO.Bind();
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
	// Unbind VAO to prevent modification
	VAO.Unbind();
}


void Chunk::Draw(Shader& shader, Camera& camera) {


	// Activate the shader program
	shader.Activate();

	// Bind the VAO before drawing
	VAO.Bind();

	/*for (int i = 0; i < vertices.size(); i++) {
		std::cout << i << ": " << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << std::endl;
	}*/

	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");


	// Draw the triangles
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO after drawing
	VAO.Unbind();
}