#include"Grass.h"

Grass::Grass(Noise* input_noise) {
	noise = input_noise;


	for (int i = 0; i < 10; i++) {
		positions.push_back(glm::vec2(i, 0.0f));
		positionsLowPoly.push_back(glm::vec2(i, 0.0f));
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
	ecosystemPropagationDistance = 0.0f;
	ecosystemPropagationAmount = 0.0f;

	textures.push_back(Texture("./Resources/Textures/blade1.png", "diffuse", 0));
	textures.push_back(Texture("./Resources/Textures/blade1Spec.png", "specular", 1));
	textures2.push_back(Texture("./Resources/Textures/grassBlades.png", "diffuse", 0));
	textures2.push_back(Texture("./Resources/Textures/blade1Spec.png", "specular", 1));
	GenerateVertices();

}

void Grass::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();
	lowPolyVertices.clear();
	lowPolyIndices.clear();

	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < positions.size(); i++) {
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

	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < positionsLowPoly.size(); i++) {
		for (int j = 0; j < 4; j++) {
			lowPolyVertices.push_back(PlantVertex
				{
					glm::vec3( // Positions
						positionsLowPoly[i][0],
						noise->get(positionsLowPoly[i][0], positionsLowPoly[i][1]) * noise->amplitude,
						positionsLowPoly[i][1]
					)
				}
			);
		}

		lowPolyIndices.push_back((i * 4));
		lowPolyIndices.push_back((i * 4) + 3);
		lowPolyIndices.push_back((i * 4) + 1);

		lowPolyIndices.push_back((i * 4));
		lowPolyIndices.push_back((i * 4) + 3);
		lowPolyIndices.push_back((i * 4) + 2);
	}

	vao.Bind();
	// Generates PlantVertex Buffer Object and links it to vertices
	VBO vbo(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO ebo(indices);


	// arg[1] = Specifies the index of the generic vertex attribute to be modified.
	// arg[2] = Specifies the number of components per generic vertex attribute.
	// arg[3] GL_FLOAT = type of data (i.e. float or int)
	// arg[4] sizeof(PatchVertex) = Specifies the byte offset between consecutive generic vertex attributes. If stride is 0, the generic vertex 
	// attributes are understood to be tightly packed in the array. The initial value is 0.
	// arg[5] offset = Specifies a offset of the first component of the first generic vertex attribute in the array in the data store of the buffer 
	// currently bound to the GL_ARRAY_BUFFER target. The initial value is 0.
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(PlantVertex), (void*)0);

	// Unbind all to prevent accidentally modifying them
	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();


	lowPolyvao.Bind();
	VBO lowPolyVbo(lowPolyVertices);
	EBO lowPolyEbo(lowPolyIndices);
	lowPolyvao.LinkAttrib(lowPolyVbo, 0, 3, GL_FLOAT, sizeof(PlantVertex), (void*)0);
	lowPolyvao.Unbind();
	lowPolyVbo.Unbind();
	lowPolyEbo.Unbind();
}

void Grass::Draw
(
	Shader& shader,
	Shader& shader2,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation
)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	// Bind the VAO so OpenGL knows to use it
	vao.Bind();

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




	shader2.Activate();
	lowPolyvao.Bind();
	unsigned int numDiffuse2 = 0;
	unsigned int numSpecular2 = 0;
	for (unsigned int i = 0; i < textures2.size(); i++)
	{
		std::string num;
		std::string type = textures2[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse2++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular2++);
		}
		textures2[i].texUnit(shader2, (type + num).c_str(), i);
		textures2[i].Bind();
	}
	glUniform3f(glGetUniformLocation(shader2.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader2, "camMatrix");
	glUniform1f(glGetUniformLocation(shader2.ID, "time"), crntTime);
	glUniform1f(glGetUniformLocation(shader2.ID, "bladeLength"), length);
	glUniform1f(glGetUniformLocation(shader2.ID, "pitchVariance"), pitchVariance);
	glUniform1f(glGetUniformLocation(shader2.ID, "lengthVariance"), lengthVariance);
	glUniform1f(glGetUniformLocation(shader2.ID, "scale"), scale);
	glUniform1f(glGetUniformLocation(shader2.ID, "scaleVariance"), scaleVariance);

	glDrawElements(GL_TRIANGLES, lowPolyIndices.size(), GL_UNSIGNED_INT, 0);
}