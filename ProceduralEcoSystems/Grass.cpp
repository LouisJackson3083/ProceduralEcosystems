#include"Grass.h"

Grass::Grass(Noise* input_noise) {
	noise = input_noise;


	for (int i = 0; i < 10; i++) {
		positions.push_back(glm::vec2(i, 0.0f));
	}

	length = 5.0f;
	lengthVariance = 1.0f;
	pitchVariance = 0.03f;
	scale = 0.05f;
	scaleVariance = 0.05f;

	//init ecosystem variables
	ecosystemDominance = 1;
	ecosystemOxygenUpperLimit = 1.0f;
	ecosystemOxygenLowerLimit = 0.0f;
	ecosystemRootingStrength = 0.5f;
	ecosystemMoistureRequirement = 0.0f;
	ecosystemInteractionLevel = 0.0f;

	textures.push_back(Texture("./Resources/Textures/blade1.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/blade1Spec.png", "specular", 1));
	GenerateVertices();

}

void Grass::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();

	int radius = 25;

	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < positions.size(); i++) {


		float r = radius * sqrt(((float)rand() / (RAND_MAX)));
		float theta = ((float)rand() / (RAND_MAX)) * 6.283185307179586476925286766559;
		float x = r * cos(theta);
		float y = r * sin(theta);

		for (int j = 0; j < 4; j++) {
			vertices.push_back(PlantVertex
				{
					glm::vec3( // Positions
						positions[i][0],
						noise->get(positions[i][0], positions[i][1]) * noise->amplitude,
						positions[i][1]
					)
				}
			);
		}

		// Blade 1
		indices.push_back((i * 4));
		indices.push_back((i * 4) + 2);
		indices.push_back((i * 4) + 1);

		// Blade 2
		indices.push_back((i * 4));
		indices.push_back((i * 4) + 3);
		indices.push_back((i * 4) + 1);
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

void Grass::Draw
(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation
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
	glUniform1f(glGetUniformLocation(shader.ID, "time"), crntTime);
	glUniform1f(glGetUniformLocation(shader.ID, "bladeLength"), length);
	glUniform1f(glGetUniformLocation(shader.ID, "pitchVariance"), pitchVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "lengthVariance"), lengthVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "scale"), scale);
	glUniform1f(glGetUniformLocation(shader.ID, "scaleVariance"), scaleVariance);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}