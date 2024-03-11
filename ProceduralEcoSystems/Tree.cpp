#include "Tree.h"

Tree::Tree(Noise* input_noise) {
	noise = input_noise;

	for (int i = 0; i < 10; i++) {
		positions.push_back(glm::vec2(i, 0.0f));
	}

	//init appearance variables
	resolution = 8;
	segments = 6;
	vertices_per_trunk = resolution * segments;
	height = 5.0f;
	radius = 0.5f;
	radiusFalloff = 1.5f;
	radiusFalloffRate = 0.5f;


	textures.push_back(Texture("./Resources/Textures/bark2.png", "diffuse", 0));
	tree_texture_filepaths.push_back(std::string("./Resources/Textures/bark2.png"));
	textures.push_back(Texture("./Resources/Textures/bark2Spec.png", "specular", 1));
	tree_texture_filepaths.push_back(std::string("./Resources/Textures/bark2Spec.png"));

	//init ecosystem variables
	layer = 0;
	ecosystemDominance = 1;
	ecosystemOxygenUpperLimit = 1.0f;
	ecosystemOxygenLowerLimit = 0.0f;
	ecosystemRootingStrength = 0.5f;
	ecosystemMoistureRequirement = 0.0f;
	ecosystemInteractionLevel = 0.0f;

	GenerateVertices();
}

Tree::Tree(std::string file, Noise* input_noise) {
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
		resolution = std::stoi(results[0]);
		segments = std::stoi(results[1]);
		height = std::stof(results[2]);
		radius = std::stof(results[3]);
		radiusFalloff = std::stof(results[4]);
		radiusFalloffRate = std::stof(results[5]);

		textures.push_back(Texture(results[6].c_str(), "diffuse", 0));
		tree_texture_filepaths.push_back(results[6]);
		textures.push_back(Texture(results[7].c_str(), "specular", 1));
		tree_texture_filepaths.push_back(results[7]);

		// Ecosystem parameters
		layer = std::stoi(results[8]);
		ecosystemDominance = std::stoi(results[9]);
		ecosystemOxygenUpperLimit = std::stof(results[10]);
		ecosystemOxygenLowerLimit = std::stof(results[11]);
		ecosystemRootingStrength = std::stof(results[12]);
		ecosystemMoistureRequirement = std::stof(results[13]);
		ecosystemInteractionLevel = std::stof(results[14]);

		GenerateVertices();

		myfile.close();
	}
}

TreeGUIData Tree::GetGUIData() {
	return TreeGUIData
	{
		resolution,
		segments,
		height,
		radius,
		radiusFalloff,
		radiusFalloffRate,
		layer,
		ecosystemDominance,
		ecosystemOxygenUpperLimit,
		ecosystemOxygenLowerLimit,
		ecosystemRootingStrength,
		ecosystemMoistureRequirement,
		ecosystemInteractionLevel
	};
}


void Tree::SaveTreeData(TreeGUIData* treeGUIData, std::string file) {
	std::ofstream myfile;
	myfile.open(file);

	// Save appearance variables
	myfile << treeGUIData->sliderTreeResolution << ",";
	myfile << treeGUIData->sliderTreeSegments << ",";
	myfile << treeGUIData->sliderTreeHeight << ",";
	myfile << treeGUIData->sliderTreeRadius << ",";
	myfile << treeGUIData->sliderTreeRadiusFalloff << ",";
	myfile << treeGUIData->sliderTreeRadiusFalloffRate << ",";

	myfile << tree_texture_filepaths[0] << ",";
	myfile << tree_texture_filepaths[1] << ",";

	// Save ecosystem variables
	myfile << treeGUIData->sliderTreeLayer << ",";
	myfile << treeGUIData->sliderTreeDominance << ",";
	myfile << treeGUIData->sliderTreeOxygenUpperLimit << ",";
	myfile << treeGUIData->sliderTreeOxygenLowerLimit << ",";
	myfile << treeGUIData->sliderTreeRootingStrength << ",";
	myfile << treeGUIData->sliderTreeMoistureRequirement << ",";
	myfile << treeGUIData->sliderTreeInteractionLevel << ",";

	myfile.close();
}

void Tree::UpdateValues(TreeGUIData treeGUIData) {
	resolution = treeGUIData.sliderTreeResolution * 2;
	segments = treeGUIData.sliderTreeSegments;
	height = treeGUIData.sliderTreeHeight;
	radius = treeGUIData.sliderTreeRadius;
	radiusFalloff = treeGUIData.sliderTreeRadiusFalloff;
	radiusFalloffRate = treeGUIData.sliderTreeRadiusFalloffRate;
	layer = treeGUIData.sliderTreeLayer;
	ecosystemDominance = treeGUIData.sliderTreeDominance;
	ecosystemOxygenUpperLimit = treeGUIData.sliderTreeOxygenUpperLimit;
	ecosystemOxygenLowerLimit = treeGUIData.sliderTreeOxygenLowerLimit;
	ecosystemRootingStrength = treeGUIData.sliderTreeRootingStrength;
	ecosystemMoistureRequirement = treeGUIData.sliderTreeMoistureRequirement;
	ecosystemInteractionLevel = treeGUIData.sliderTreeInteractionLevel;
}

void Tree::ChangeTextures(const char* texture, const int type) {
	if (type == 0) {
		tree_texture_filepaths[type] = std::string(texture);
		textures[type] = Texture(texture, "diffuse", type);
	}
	else if (type == 1) {
		tree_texture_filepaths[type] = std::string(texture);
		textures[type] = Texture(texture, "specular", type);
	}
}

void Tree::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();
	vertices_per_trunk = resolution * segments;
	int number_of_trees = 3;
	int tree_id = -1;

	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < vertices_per_trunk * positions.size(); i++) {
		int curr_vertex = i % vertices_per_trunk;
		int curr_segment = i % resolution;

		if (curr_vertex == 0) {
			tree_id += 1;
		}

		vertices.push_back(PlantVertex
			{
				glm::vec3( // Positions
					positions[tree_id][0],
					noise->get(positions[tree_id][0], positions[tree_id][1]) * noise->amplitude,
					positions[tree_id][1]
				)
			}
		);

		if (curr_vertex < (vertices_per_trunk - resolution)) {

			if (curr_segment < resolution - 1) {
				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + resolution + 1);

				indices.push_back(i);
				indices.push_back(i + resolution + 1);
				indices.push_back(i + resolution);
			}
			else {
				indices.push_back(i);
				indices.push_back(i - resolution + 1);
				indices.push_back(i + 1);


				indices.push_back(i);
				indices.push_back(i + 1);
				indices.push_back(i + resolution);
			}
			
		}
		if (curr_vertex == vertices_per_trunk - 1) {
			int temp_i = i-2;
			indices.push_back(i);
			indices.push_back(i-2);
			indices.push_back(i-1);
			for (int j = 0; j < resolution - 2; j++) {
				indices.push_back(i);
				indices.push_back(temp_i - 1);
				indices.push_back(temp_i);
				temp_i--;
			}
			indices.push_back(i);
			indices.push_back(temp_i - 1);
			indices.push_back(temp_i);
		}
	}

	vao.Bind();
	// Generates TreeVertex Buffer Object and links it to vertices
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

void Tree::Draw
(
	Shader& trunkShader,
	Shader& branchShader,
	Camera& camera
)
{
	// Bind shader to be able to access uniforms
	trunkShader.Activate();
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
		textures[i].texUnit(trunkShader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	// Exports the camera Position to the Fragment Shader for specular lighting
	glUniform3f(glGetUniformLocation(trunkShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the TreeVertex Shader of the pyramid
	camera.Matrix(trunkShader, "camMatrix");

	float crntTime = glfwGetTime();

	// Push the floats to the vertex shader
	glUniform1f(glGetUniformLocation(trunkShader.ID, "height"), height);
	glUniform1f(glGetUniformLocation(trunkShader.ID, "radius"), radius);
	glUniform1f(glGetUniformLocation(trunkShader.ID, "radius_falloff"), radiusFalloff);
	glUniform1f(glGetUniformLocation(trunkShader.ID, "radius_falloff_rate"), radiusFalloffRate);
	glUniform1i(glGetUniformLocation(trunkShader.ID, "resolution"), resolution);
	glUniform1i(glGetUniformLocation(trunkShader.ID, "segments"), segments);
	glUniform1i(glGetUniformLocation(trunkShader.ID, "vertices_per_trunk"), vertices_per_trunk);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}