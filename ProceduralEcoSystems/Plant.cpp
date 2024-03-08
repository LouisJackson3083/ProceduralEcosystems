#include"Plant.h"
#include <random>

Plant::Plant(Noise* input_noise) {
	noise = input_noise;


	for (int i = 0; i < 10; i++) {
		positions.push_back(glm::vec2(i, 0.0f));
	}

	//init appearance variables
	pitch = 2.09f;
	yaw = 2.09f;
	bendStrength = -0.14f;
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

	//init ecosystem variables
	layer = 2;
	ecosystemDominance = 1;
	ecosystemOxygenUpperLimit = 1.0f;
	ecosystemOxygenLowerLimit = 0.0f;
	ecosystemRootingStrength = 0.5f;
	ecosystemMoistureRequirement = 0.0f;
	ecosystemInteractionLevel = 0.0f;

	GeneratePlantBin();
	GenerateVertices();
}

Plant::Plant(std::string file, Noise* input_noise) {
	noise = input_noise;
	for (int i = 0; i < 10; i++) {
		positions.push_back(glm::vec2(i, 0.0f));
	}

	std::string line;
	std::ifstream myfile(file);
	if (myfile.is_open())
	{
		std::getline(myfile, line);
		std::vector<std::string> results;
		std::stringstream  ss(line);
		std::string str;
		while (getline(ss, str, ',')) {
			results.push_back(str);
		}

		//region appearance
		pitch = std::stof(results[0]);
		bendStrength = std::stof(results[1]);
		yaw = std::stof(results[2]);
		pitchVariance = std::stof(results[3]);
		bendVariance = std::stof(results[4]);

		segments = std::stoi(results[5]);
		vertices_per_leaf = 2 * segments;
		leafLength = std::stoi(results[6]);
		lengthVariance = std::stof(results[7]);

		scaleVariance = std::stof(results[8]);
		scale = std::stof(results[9]);

		maxLeaves = std::stoi(results[10]);
		minLeaves = std::stoi(results[11]);

		textures.push_back(Texture(results[12].c_str(), "diffuse", 0));
		plant_texture_filepaths.push_back(results[12]);
		textures.push_back(Texture(results[13].c_str(), "specular", 1));
		plant_texture_filepaths.push_back(results[13]);
		
		// Ecosystem parameters
		layer = std::stoi(results[14]);
		ecosystemDominance = std::stoi(results[15]);
		ecosystemOxygenUpperLimit = std::stof(results[16]);
		ecosystemOxygenLowerLimit = std::stof(results[17]);
		ecosystemRootingStrength = std::stof(results[18]);
		ecosystemMoistureRequirement = std::stof(results[19]);
		ecosystemInteractionLevel = std::stof(results[20]);

		GeneratePlantBin();
		GenerateVertices();

		myfile.close();
	}
}

PlantGUIData Plant::GetGUIData() {
	return PlantGUIData
	{
		pitch,
		bendStrength,
		yaw,
		pitchVariance,
		bendVariance,
		segments,
		leafLength,
		lengthVariance,
		scaleVariance,
		scale,
		maxLeaves,
		minLeaves,
		layer,
		ecosystemDominance,
		ecosystemOxygenUpperLimit,
		ecosystemOxygenLowerLimit,
		ecosystemRootingStrength,
		ecosystemMoistureRequirement,
		ecosystemInteractionLevel
	};
}

void Plant::SavePlantData(PlantGUIData* plantGUIData, std::string file) {
	std::ofstream myfile;
	myfile.open(file);

	// Save appearance variables
	myfile << plantGUIData->sliderPlantPitch << ",";
	myfile << plantGUIData->sliderPlantBendStrength << ",";
	myfile << plantGUIData->sliderPlantYaw << ",";
	myfile << plantGUIData->sliderPlantPitchVariance << ",";
	myfile << plantGUIData->sliderPlantBendVariance << ",";

	myfile << plantGUIData->sliderPlantSegments << ",";
	myfile << plantGUIData->sliderPlantLeafLength << ",";
	myfile << plantGUIData->sliderPlantLengthVariance << ",";

	myfile << plantGUIData->sliderPlantScaleVariance << ",";
	myfile << plantGUIData->sliderPlantScale << ",";
	myfile << plantGUIData->sliderPlantMaxLeaves << ",";
	myfile << plantGUIData->sliderPlantMinLeaves << ",";

	myfile << plant_texture_filepaths[0] << ",";
	myfile << plant_texture_filepaths[1] << ",";

	// Save ecosystem variables
	myfile << plantGUIData->sliderPlantLayer << ",";
	myfile << plantGUIData->sliderPlantDominance << ",";
	myfile << plantGUIData->sliderPlantOxygenUpperLimit << ",";
	myfile << plantGUIData->sliderPlantOxygenLowerLimit << ",";
	myfile << plantGUIData->sliderPlantRootingStrength << ",";
	myfile << plantGUIData->sliderPlantMoistureRequirement << ",";
	myfile << plantGUIData->sliderPlantInteractionLevel << ",";

	myfile.close();
}


void Plant::UpdateValues(PlantGUIData plantGUIData) {
	pitch = plantGUIData.sliderPlantPitch;
	yaw = plantGUIData.sliderPlantYaw;
	bendStrength = plantGUIData.sliderPlantBendStrength;
	segments = plantGUIData.sliderPlantSegments;
	vertices_per_leaf = 2 * plantGUIData.sliderPlantSegments;
	maxLeaves = plantGUIData.sliderPlantMaxLeaves;
	minLeaves = plantGUIData.sliderPlantMinLeaves;
	leafLength = plantGUIData.sliderPlantLeafLength;
	lengthVariance = plantGUIData.sliderPlantLengthVariance;
	pitchVariance = plantGUIData.sliderPlantPitchVariance;
	bendVariance = plantGUIData.sliderPlantBendVariance;
	scaleVariance = plantGUIData.sliderPlantScaleVariance;
	scale = plantGUIData.sliderPlantScale;
	layer = plantGUIData.sliderPlantLayer;
	ecosystemDominance = plantGUIData.sliderPlantDominance;
	ecosystemOxygenUpperLimit = plantGUIData.sliderPlantOxygenUpperLimit;
	ecosystemOxygenLowerLimit = plantGUIData.sliderPlantOxygenLowerLimit;
	ecosystemRootingStrength = plantGUIData.sliderPlantRootingStrength;
	ecosystemMoistureRequirement = plantGUIData.sliderPlantMoistureRequirement;
	ecosystemInteractionLevel = plantGUIData.sliderPlantInteractionLevel;
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

	for (int i = 0; i < positions.size(); i++) {
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
				glm::vec3( // Positions
					positions[plant_id][0], 
					noise->get(positions[plant_id][0], positions[plant_id][1]) * noise->amplitude, 
					positions[plant_id][1]
				)
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
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(PlantVertex), (void*)0);

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