#include"Plant.h"

Plant::Plant(int id) {
	pitch = 2.09f;
	yaw = 2.09f;
	bendStrength = -0.14f;

	segments = 7;
	leaves = 3;
	leafLength = 5;

	lengthVariance = 0.0f;
	pitchVariance = 0.0f;
	bendVariance = 0.0f;


	textures.push_back(Texture("./Resources/Textures/fern1.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/fern1Spec.png", "specular", 1));
	GenerateVertices();

}

void Plant::ChangeTextures(const char* texture, const int type) {
	std::cout << texture << std::endl;
	if (type == 0) {
		textures[type] = Texture(texture, "diffuse", type);
	}
	else if (type == 1) {
		textures[type] = Texture(texture, "specular", type);
	}
	
}

void Plant::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();

	int vertices_per_leaf = segments * 2;


	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < vertices_per_leaf*leaves; i++) {
		int currIndex = i % vertices_per_leaf;

		vertices.push_back(PlantVertex
			{
				glm::vec3(0.0f, i, 0.0f), // Positions
			}
		);
		if (currIndex % 2 == 0 and currIndex < vertices_per_leaf - 2) {
			indices.push_back(i);
			indices.push_back(i + 3);
			indices.push_back(i + 1);

			indices.push_back(i);
			indices.push_back(i + 2);
			indices.push_back(i + 3);
			//std::cout << i << " : " << i << ", " << i + 1 << ", " << i + 3 << std::endl;
		}
	}

	VAO.Bind();
	// Generates PlantVertex Buffer Object and links it to vertices
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
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(PlantVertex), (void*)0);

	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

void Plant::Draw
(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	// Bind the VAO so OpenGL knows to use it
	VAO.Bind();

	// Binds textures so that they appear in the rendering
	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	// Exports the camera Position to the Fragment Shader for specular lighting
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the PlantVertex Shader of the pyramid
	camera.Matrix(shader, "camMatrix");

	float crntTime = glfwGetTime();


	// Push the floats to the vertex shader
	glUniform1f(glGetUniformLocation(shader.ID, "pitch"), pitch);
	glUniform1f(glGetUniformLocation(shader.ID, "yaw"), yaw);
	glUniform1f(glGetUniformLocation(shader.ID, "bendStrength"), bendStrength);
	glUniform1f(glGetUniformLocation(shader.ID, "time"), crntTime);
	glUniform1i(glGetUniformLocation(shader.ID, "segments"), segments);
	glUniform1i(glGetUniformLocation(shader.ID, "leaves"), leaves);
	glUniform1i(glGetUniformLocation(shader.ID, "leafLength"), leafLength);
	glUniform1f(glGetUniformLocation(shader.ID, "pitchVariance"), pitchVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "bendVariance"), bendVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "lengthVariance"), lengthVariance);


	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}