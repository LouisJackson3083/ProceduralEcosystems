#include"Plant.h"

Plant::Plant(int id) {
	pitch = 2.09f;
	yaw = 2.09f;
	bendStrength = -0.14f;

	plantNumber = 10;

	segments = 7;
	vertices_per_leaf = segments * 2;
	maxLeaves = 3;
	minLeaves = 1;
	leafLength = 5;

	lengthVariance = 0.0f;
	pitchVariance = 0.0f;
	bendVariance = 0.0f;
	scaleVariance = 0.0f;
	scale = 1.0f;

	textures.push_back(Texture("./Resources/Textures/fern1.png", "diffuse", 0));
	plant_texture_filepaths.push_back(std::string("./Resources/Textures/fern1.png"));
	textures.push_back(Texture("./Resources/Textures/fern1Spec.png", "specular", 1));
	plant_texture_filepaths.push_back(std::string("./Resources/Textures/fern1Spec.png"));
	GeneratePlantBin();
	GenerateVertices();
}

Plant::Plant(PlantGUIData plantGUIData, std::string texDiffuse, std::string texSpecular) {
	plantNumber = plantGUIData.sliderPlantNumber;
	pitch = plantGUIData.sliderPlantPitch;
	yaw = plantGUIData.sliderPlantYaw;
	bendStrength = plantGUIData.sliderPlantBendStrength;
	segments = plantGUIData.sliderPlantSegments;
	vertices_per_leaf = segments * 2;
	maxLeaves = plantGUIData.sliderPlantMaxLeaves;
	minLeaves = plantGUIData.sliderPlantMinLeaves;
	leafLength = plantGUIData.sliderPlantLeafLength;
	lengthVariance = plantGUIData.sliderPlantLengthVariance;
	pitchVariance = plantGUIData.sliderPlantPitchVariance;
	bendVariance = plantGUIData.sliderPlantBendVariance;
	scaleVariance = plantGUIData.sliderPlantScaleVariance;
	scale = plantGUIData.sliderPlantScale;

	textures.push_back(Texture(texDiffuse.c_str(), "diffuse", 0));
	plant_texture_filepaths.push_back(texDiffuse);
	textures.push_back(Texture(texSpecular.c_str(), "specular", 1));
	plant_texture_filepaths.push_back(texSpecular);
	GeneratePlantBin();
	GenerateVertices();
}


void Plant::ChangeTextures(const char* texture, const int type) {
	if (type == 0) {	
		plant_texture_filepaths[type] = std::string(texture);
		textures[type] = Texture(texture, "diffuse", type);
	}
	else if (type == 1) {
		plant_texture_filepaths[type] = std::string(texture);
		textures[type] = Texture(texture, "specular", type);
	}
}

void Plant::GeneratePlantBin() {
	// Initialize the random number of leaves for each plant
	plant_bin.clear();
	number_of_leaves = 0;

	for (int i = 0; i < plantNumber; i++) {
		plant_bin.push_back(rand() % maxLeaves + 1);
		number_of_leaves = number_of_leaves + plant_bin.back();
	}
}

void Plant::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();

	int plant_id = 0;
	int plant_id_leaf_count = 0;
	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < vertices_per_leaf * number_of_leaves; i++) {
		int currIndex = i % vertices_per_leaf;

		// Calculate what plant we are
		if (currIndex == 0) {
			if (plant_bin[plant_id] == plant_id_leaf_count) {
				plant_id++;
				plant_id_leaf_count = 1;
			}
			else {
				plant_id_leaf_count += 1;
			}
		}

		vertices.push_back(PlantVertex
			{
				glm::vec3(2 * plant_id, 0.0f, 0.0f) // Positions
			}
		);
		if (currIndex % 2 == 0 and currIndex < vertices_per_leaf - 2) {
			indices.push_back(i);
			indices.push_back(i + 3);
			indices.push_back(i + 1);

			indices.push_back(i);
			indices.push_back(i + 2);
			indices.push_back(i + 3);
		}
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
	vao.LinkAttrib(vbo, 0, 1, GL_FLOAT, sizeof(PlantVertex), (void*)0);

	// Unbind all to prevent accidentally modifying them
	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}

void Plant::Draw
(
	Shader& shader,
	Camera& camera
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
	glUniform1f(glGetUniformLocation(shader.ID, "pitch"), pitch);
	glUniform1f(glGetUniformLocation(shader.ID, "yaw"), yaw);
	glUniform1f(glGetUniformLocation(shader.ID, "bendStrength"), bendStrength);
	glUniform1f(glGetUniformLocation(shader.ID, "time"), crntTime);
	glUniform1i(glGetUniformLocation(shader.ID, "segments"), segments);
	glUniform1i(glGetUniformLocation(shader.ID, "maxLeaves"), maxLeaves);
	glUniform1i(glGetUniformLocation(shader.ID, "vertices_per_leaf"), vertices_per_leaf);
	glUniform1i(glGetUniformLocation(shader.ID, "leafLength"), leafLength);
	glUniform1f(glGetUniformLocation(shader.ID, "pitchVariance"), pitchVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "bendVariance"), bendVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "lengthVariance"), lengthVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "scaleVariance"), scaleVariance);
	glUniform1f(glGetUniformLocation(shader.ID, "scale"), scale);


	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}