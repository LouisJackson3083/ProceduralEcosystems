#include"Test.h"

Test::Test(int N) {

	int size = (int) sqrt(N);
	//int size = 4;
	int vertexIndex = 0;
	/*std::cout << "BEFORE: " << vertices.size() << ", " << indices.size() << std::endl;
	vertices.resize(size*size*sizeof(Vertex));
	indices.resize((size - 1) * (size - 1) * 6 * sizeof(float));
	std::cout << "after: " << vertices.size() << ", " << indices.size() << std::endl;
	*/

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			std::cout << vertexIndex << ": " << i << ", " << j << std::endl;
			
			vertices.push_back(Vertex
				{
					glm::vec3{i, 0.0f, j}, // Positions
					glm::vec3(0.0f, 1.0f, 0.0f), // Normals
					glm::vec3(0.0f, 0.0f, 0.0f), // Colors
					glm::vec2(i, j) // Tex UV
				}
			);
			if (i < size - 1 && j < size - 1) {
				// triangle 1
				indices.push_back(vertexIndex);
				indices.push_back(vertexIndex + size + 1);
				indices.push_back(vertexIndex + size);
				//std::cout << "Added triangle 1: " << vertexIndex << "," << vertexIndex + size + 1 << "," << vertexIndex + size << std::endl;
				// triangle 2
				indices.push_back(vertexIndex + size + 1);
				indices.push_back(vertexIndex);
				indices.push_back(vertexIndex + 1);
				//std::cout << "Added triangle 2: " << vertexIndex + size + 1 << "," << vertexIndex << "," << vertexIndex + 1 << std::endl;
			}

			//std::cout << "at index " << vertexIndex << ": " << vertices.size() << ", " << indices.size() << std::endl;

			vertexIndex++;
		}
	}
	std::cout << "sizes of vertices, indices: " << vertices.size() << ", " << indices.size() << std::endl;

	/*
	vertices.push_back(Vertex
		{
			glm::vec3{0.0f, 0.0f, 0.0f}, // Positions
			glm::vec3(0.0f, 1.0f, 0.0f), // Normals
			glm::vec3(0.0f, 0.0f, 0.0f), // Colors
			glm::vec2(0.0f, 0.0f) // Tex UV
		}
	);
	vertices.push_back(Vertex
		{
			glm::vec3{0.0f, 0.0f, 1.0f}, // Positions
			glm::vec3(0.0f, 1.0f, 0.0f), // Normals
			glm::vec3(0.0f, 0.0f, 0.0f), // Colors	
			glm::vec2(0.0f, 1.0f) // Tex UV
		}
	);
	vertices.push_back(Vertex
		{
			glm::vec3{1.0f, 0.0f, 0.0f}, // Positions
			glm::vec3(0.0f, 1.0f, 0.0f), // Normals
			glm::vec3(0.0f, 0.0f, 0.0f), // Colors
			glm::vec2(1.0f, 0.0f) // Tex UV
		}
	);
	vertices.push_back(Vertex
		{
			glm::vec3{1.0f, 0.0f, 1.0f}, // Positions
			glm::vec3(0.0f, 1.0f, 0.0f), // Normals
			glm::vec3(0.0f, 0.0f, 0.0f), // Colors
			glm::vec2(1.0f, 1.0f) // Tex UV
		}
	);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);*/

	VAO1.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices);

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	std::cout << "AAA" << std::endl;
}

void Test::Draw
(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
) 
{
	shader.Activate();
	// Exports the camera Position to the Fragment Shader for specular lighting
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the Vertex Shader of the pyramid
	camera.Matrix(shader, "camMatrix");
	// Binds textures so that they appear in the rendering
	texture.Bind();
	// Bind the VAO so OpenGL knows to use it
	VAO1.Bind();
	// Draw primitives, number of indices, datatype of indices, index of indices
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}