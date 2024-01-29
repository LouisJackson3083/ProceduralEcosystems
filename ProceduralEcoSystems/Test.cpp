#include"Test.h"

Test::Test(int N) {
	vertices_num = N;
	vertices.resize(N);

	int size = sqrt(N);
	int vertexIndex = 0;
	int triangleIndex = 0;

	// Push back the popcat texture onto the textures vector
	Texture texture = Texture("./Resources/Textures/pop_cat.png", "diffuse", 0);
	textures.push_back(texture);

	// Loop through and make the vertices and triangles
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			// Add the vertex to the vertices list
			vertices.push_back
			(
				Vertex
				{
					glm::vec3{i, 0, j}, // Positions
					glm::vec3(0.0f, 0.0f, 0.0f), // Normals
					glm::vec3(1.0f, 1.0f, 1.0f), // 
					glm::vec2(1.0f, 1.0f) // Tex UV
				}
			);

			// Add triangles to the indices list
			indices.push_back(j + i * size);
			indices.push_back(j + (i + 1) * size);

			if (i < size - 1 && j < size - 1) {
				// triangle 1
				indices.push_back(vertexIndex);
				indices.push_back(vertexIndex + size + 1);
				indices.push_back(vertexIndex + size);
				std::cout << "Added triangle 1: " << vertexIndex << "," << vertexIndex + size + 1 << "," << vertexIndex + size << std::endl;
				// triangle 2
				indices.push_back(vertexIndex + size + 1);
				indices.push_back(vertexIndex);
				indices.push_back(vertexIndex + 1);
				std::cout << "Added triangle 2: " << vertexIndex + size + 1 << "," << vertexIndex << "," << vertexIndex + 1 << std::endl;
			}

			std::cout << vertexIndex << ": (" << i << "," << j << ")" << std::endl;

			vertexIndex++;
		}
	}
	meshes.push_back(Mesh(vertices, indices, textures, 0, {}));
}

void Test::Draw(Shader& shader, Camera& camera, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	// Go over all meshes and draw each one
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		{
			meshes[i].Mesh::Draw(shader, camera, {}, translation, rotation, scale);
			
			//std::cout << "This is getting called: " << i << ", " << std::endl;
		}
	}
}