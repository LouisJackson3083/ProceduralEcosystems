#include"Patch.h"

Patch::Patch(glm::vec3 input_offset, int input_size, int input_subdivision, float input_amplitude, Noise* input_noise) {
	offset = input_offset;
	size = input_size;
	subdivision = input_subdivision;
	amplitude = input_amplitude;
	noise = input_noise;
	UpdateMesh();
}

void Patch::UpdateMesh() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();

	// int size - the size in metres of the whole patch, where
	// 1 = 1m x 1m patch
	// 2 = 2m x 2m patch
	// 
	// int subdivision - how many times the patch should be subdivided, where
	// 2 = 2x2 quad patch ,
	// 3 = 3x3 quad patch ,
	// 4 = 4x4 quad patch ...
	
	// Used to keep track of the current vertex so that indice instantiation is correct
	int vertexIndex = 0;
	// This is the scalar we times each vertex by so the size does not change as we up the subdivisions
	float scalar = ((float)size / (float)(subdivision));
	
	for (int i = 0; i < subdivision + 1; i++) {
		for (int j = 0; j < subdivision + 1; j++) {
			// Push the current vertex onto the vector
			float x = (float)i * scalar;
			float z = (float)j * scalar;
			vertices.push_back(Vertex
				{
					glm::vec3{x, noise->get(offset[0] + x, offset[1] + z) * amplitude, z}, // Positions
					glm::vec3(0.0f, 1.0f, 0.0f), // Normals
					glm::vec3(0.0f, 0.0f, 0.0f), // Colors
					glm::vec2(x, z) // Tex UV - Needs updating
				}
			);

			if (i < subdivision && j < subdivision) {
				// triangle 1
				indices.push_back(vertexIndex);
				indices.push_back(vertexIndex + subdivision + 2);
				indices.push_back(vertexIndex + subdivision + 1);
				//std::cout << "Added triangle 1: " << vertexIndex << "," << vertexIndex + size + 1 << "," << vertexIndex + size << std::endl;
				// triangle 2
				indices.push_back(vertexIndex + subdivision + 2);
				indices.push_back(vertexIndex);
				indices.push_back(vertexIndex + 1);
				//std::cout << "Added triangle 2: " << vertexIndex + size + 1 << "," << vertexIndex << "," << vertexIndex + 1 << std::endl;
			}

			//std::cout << "at index " << vertexIndex << ": " << vertices.size() << ", " << indices.size() << std::endl;

			vertexIndex++;
		}
	}

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);


	// arg[1] = Specifies the index of the generic vertex attribute to be modified.
	// arg[2] = Specifies the number of components per generic vertex attribute.
	// arg[3] GL_FLOAT = type of data (i.e. float or int)
	// arg[4] sizeof(PatchVertex) = Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex 
	// attributes are understood to be tightly packed in the array. The initial value is 0.
	// arg[5] offset = Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer 
	// currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Patch::Draw
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
	VAO.Bind();
	// Draw primitives, number of indices, datatype of indices, index of indices

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	// Transform the matrices to their correct form
	trans = glm::translate(trans, offset);
	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}