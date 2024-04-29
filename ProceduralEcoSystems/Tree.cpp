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
	branchMinHeight = height / 2;
	branchMaxHeight = height + 1.0f;
	radius = 0.5f;
	radiusFalloff = 1.5f;
	radiusFalloffRate = 0.5f;


	textures.push_back(Texture("./Resources/Textures/bark2.png", "diffuse", 0));
	tree_texture_filepaths.push_back(std::string("./Resources/Textures/bark2.png"));
	textures.push_back(Texture("./Resources/Textures/bark2Spec.png", "specular", 1));
	tree_texture_filepaths.push_back(std::string("./Resources/Textures/bark2Spec.png"));

	branchTextures.push_back(Texture("./Resources/Textures/spruceTree1.png", "diffuse", 0));
	branch_texture_filepaths.push_back(std::string("./Resources/Textures/spruceTree1.png"));
	branchTextures.push_back(Texture("./Resources/Textures/spruceTree1Spec.png", "specular", 1));
	branch_texture_filepaths.push_back(std::string("./Resources/Textures/spruceTree1Spec.png"));

	pitch = 4.0f;
	minBranches = 24;
	maxBranches = 24;
	number_of_branches = 4;
	branchSegments = 2;
	vertices_per_branch = branchSegments * 2;
	branchLength = 5;
	bendStrength = 0.2f;
	pitchVariance = 0.3f;
	bendVariance = 0.1f;
	lengthVariance = 0.2f;
	scaleVariance = 0.2f;
	scale = 1.5f;

	trunkJitter = 0.0f;
	trunkJitterFalloff = 1.5f;
	trunkJitterFalloffRate = 0.5f;
	segmentJitter = 0.0f;

	//init ecosystem variables
	layer = 0;
	ecosystemDominance = 1;
	ecosystemOxygenUpperLimit = 1.0f;
	ecosystemOxygenLowerLimit = 0.0f;
	ecosystemRootingStrength = 0.5f;
	ecosystemPropagationDistance = 0.0f;
	ecosystemPropagationAmount = 0.0f;

	GenerateBranchBin();
	GenerateVertices();
}

Tree::Tree(Noise* input_noise, int input_segments) {
	noise = input_noise;

	for (int i = 0; i < 10; i++) {
		positions.push_back(glm::vec2(i, 0.0f));
	}

	//init appearance variables
	resolution = 8;
	segments = input_segments;
	vertices_per_trunk = resolution * segments;
	height = 5.0f;
	branchMinHeight = height / 2;
	branchMaxHeight = height + 1.0f;
	radius = 0.5f;
	radiusFalloff = 1.5f;
	radiusFalloffRate = 0.5f;


	textures.push_back(Texture("./Resources/Textures/bark2.png", "diffuse", 0));
	tree_texture_filepaths.push_back(std::string("./Resources/Textures/bark2.png"));
	textures.push_back(Texture("./Resources/Textures/bark2Spec.png", "specular", 1));
	tree_texture_filepaths.push_back(std::string("./Resources/Textures/bark2Spec.png"));

	branchTextures.push_back(Texture("./Resources/Textures/spruceTree1.png", "diffuse", 0));
	branch_texture_filepaths.push_back(std::string("./Resources/Textures/spruceTree1.png"));
	branchTextures.push_back(Texture("./Resources/Textures/spruceTree1Spec.png", "specular", 1));
	branch_texture_filepaths.push_back(std::string("./Resources/Textures/spruceTree1Spec.png"));

	pitch = 4.0f;
	minBranches = 24;
	maxBranches = 24;
	number_of_branches = 4;
	branchSegments = input_segments;
	vertices_per_branch = branchSegments * 2;
	branchLength = 5;
	bendStrength = 0.2f;
	pitchVariance = 0.3f;
	bendVariance = 0.1f;
	lengthVariance = 0.2f;
	scaleVariance = 0.2f;
	scale = 1.5f;

	trunkJitter = 0.0f;
	trunkJitterFalloff = 1.5f;
	trunkJitterFalloffRate = 0.5f;
	segmentJitter = 0.0f;

	//init ecosystem variables
	layer = 0;
	ecosystemDominance = 1;
	ecosystemOxygenUpperLimit = 1.0f;
	ecosystemOxygenLowerLimit = 0.0f;
	ecosystemRootingStrength = 0.5f;
	ecosystemPropagationDistance = 0.0f;
	ecosystemPropagationAmount = 0.0f;

	GenerateBranchBin();
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

		pitch = std::stof(results[6]);
		branchSegments = std::stoi(results[7]);
		maxBranches = std::stoi(results[8]);
		minBranches = std::stoi(results[9]);

		branchLength = std::stoi(results[10]);
		bendStrength = std::stof(results[11]);

		pitchVariance = std::stof(results[12]);
		bendVariance = std::stof(results[13]);
		lengthVariance = std::stof(results[14]);
		scaleVariance = std::stof(results[15]);
		scale = std::stof(results[16]);

		textures.push_back(Texture(results[17].c_str(), "diffuse", 0));
		tree_texture_filepaths.push_back(results[17]);
		textures.push_back(Texture(results[18].c_str(), "specular", 1));
		tree_texture_filepaths.push_back(results[18]);

		branchTextures.push_back(Texture(results[19].c_str(), "diffuse", 0));
		branch_texture_filepaths.push_back(results[19]);
		branchTextures.push_back(Texture(results[20].c_str(), "specular", 1));
		branch_texture_filepaths.push_back(results[20]);

		// Ecosystem parameters
		layer = std::stoi(results[21]);
		ecosystemDominance = std::stoi(results[22]);
		ecosystemOxygenUpperLimit = std::stof(results[23]);
		ecosystemOxygenLowerLimit = std::stof(results[24]);
		ecosystemRootingStrength = std::stof(results[25]);
		ecosystemPropagationDistance = std::stof(results[26]);
		ecosystemPropagationAmount = std::stof(results[27]);

		branchMinHeight = std::stof(results[28]);
		branchMaxHeight = std::stof(results[29]);

		trunkJitter = std::stof(results[30]);
		trunkJitterFalloff = std::stof(results[31]);
		trunkJitterFalloffRate = std::stof(results[32]);
		segmentJitter = std::stof(results[33]);

		GenerateBranchBin();
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
		pitch,
		branchSegments,
		maxBranches,
		minBranches,
		branchLength,
		bendStrength,
		pitchVariance,
		bendVariance,
		lengthVariance,
		scaleVariance,
		scale,
		layer,
		ecosystemDominance,
		ecosystemOxygenUpperLimit,
		ecosystemOxygenLowerLimit,
		ecosystemRootingStrength,
		ecosystemPropagationDistance,
		ecosystemPropagationAmount,
		branchMinHeight,
		branchMaxHeight,
		trunkJitter,
		trunkJitterFalloff,
		trunkJitterFalloffRate,
		segmentJitter,
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

	myfile << treeGUIData->sliderBranchPitch << ",";
	myfile << treeGUIData->sliderBranchSegments << ",";
	myfile << treeGUIData->sliderMaxBranches << ",";
	myfile << treeGUIData->sliderMinBranches << ",";
	myfile << treeGUIData->sliderBranchLength << ",";
	myfile << treeGUIData->sliderBendStrength << ",";
	myfile << treeGUIData->sliderPitchVariance << ",";
	myfile << treeGUIData->sliderBendVariance << ",";
	myfile << treeGUIData->sliderLengthVariance << ",";
	myfile << treeGUIData->sliderScaleVariance << ",";
	myfile << treeGUIData->sliderScale << ",";

	myfile << tree_texture_filepaths[0] << ",";
	myfile << tree_texture_filepaths[1] << ",";

	myfile << branch_texture_filepaths[0] << ",";
	myfile << branch_texture_filepaths[1] << ",";

	// Save ecosystem variables
	myfile << treeGUIData->sliderTreeLayer << ",";
	myfile << treeGUIData->sliderTreeDominance << ",";
	myfile << treeGUIData->sliderTreeOxygenUpperLimit << ",";
	myfile << treeGUIData->sliderTreeOxygenLowerLimit << ",";
	myfile << treeGUIData->sliderTreeRootingStrength << ",";
	myfile << treeGUIData->sliderTreePropagationDistance << ",";
	myfile << treeGUIData->sliderTreePropagationAmount << ",";

	myfile << treeGUIData->sliderBranchMinHeight << ",";
	myfile << treeGUIData->sliderBranchMaxHeight << ",";

	myfile << treeGUIData->sliderTrunkJitter << ",";
	myfile << treeGUIData->sliderTrunkJitterFalloff << ",";
	myfile << treeGUIData->sliderTrunkJitterFalloffRate << ",";
	myfile << treeGUIData->sliderSegmentJitter << ",";

	myfile.close();
}

void Tree::UpdateValues(TreeGUIData treeGUIData) {
	resolution = treeGUIData.sliderTreeResolution * 2;
	segments = treeGUIData.sliderTreeSegments;
	height = treeGUIData.sliderTreeHeight;
	radius = treeGUIData.sliderTreeRadius;
	radiusFalloff = treeGUIData.sliderTreeRadiusFalloff;
	radiusFalloffRate = treeGUIData.sliderTreeRadiusFalloffRate;
	
	pitch = treeGUIData.sliderBranchPitch;
	branchSegments = treeGUIData.sliderBranchSegments;
	maxBranches = treeGUIData.sliderMaxBranches;
	minBranches = treeGUIData.sliderMinBranches;
	branchLength = treeGUIData.sliderBranchLength;
	bendStrength = treeGUIData.sliderBendStrength;
	pitchVariance = treeGUIData.sliderPitchVariance;
	bendVariance = treeGUIData.sliderBendVariance;
	lengthVariance = treeGUIData.sliderLengthVariance;
	scaleVariance = treeGUIData.sliderScaleVariance;
	scale = treeGUIData.sliderScale;

	layer = treeGUIData.sliderTreeLayer;
	ecosystemDominance = treeGUIData.sliderTreeDominance;
	ecosystemOxygenUpperLimit = treeGUIData.sliderTreeOxygenUpperLimit;
	ecosystemOxygenLowerLimit = treeGUIData.sliderTreeOxygenLowerLimit;
	ecosystemRootingStrength = treeGUIData.sliderTreeRootingStrength;
	ecosystemPropagationDistance = treeGUIData.sliderTreePropagationDistance;
	ecosystemPropagationAmount = treeGUIData.sliderTreePropagationAmount;

	branchMinHeight = treeGUIData.sliderBranchMinHeight;
	branchMaxHeight = treeGUIData.sliderBranchMaxHeight;

	trunkJitter = treeGUIData.sliderTrunkJitter;
	trunkJitterFalloff = treeGUIData.sliderTrunkJitterFalloff;
	trunkJitterFalloffRate = treeGUIData.sliderTrunkJitterFalloffRate;
	segmentJitter = treeGUIData.sliderSegmentJitter;
}

void Tree::ChangeTreeTextures(const char* texture, const int type) {
	if (type == 0) {
		tree_texture_filepaths[type] = std::string(texture);
		textures[type] = Texture(texture, "diffuse", type);
	}
	else if (type == 1) {
		tree_texture_filepaths[type] = std::string(texture);
		textures[type] = Texture(texture, "specular", type);
	}
}


void Tree::ChangeBranchTextures(const char* texture, const int type) {
	if (type == 0) {
		branch_texture_filepaths[type] = std::string(texture);
		branchTextures[type] = Texture(texture, "diffuse", type);
	}
	else if (type == 1) {
		branch_texture_filepaths[type] = std::string(texture);
		branchTextures[type] = Texture(texture, "specular", type);
	}
}

void Tree::GenerateVertices() {
	// Clear the vertices and indices vectors
	vertices.clear();
	indices.clear();
	branchVertices.clear();
	branchIndices.clear();

	if (positions.empty()) {
		return;
	}

	vertices_per_trunk = resolution * segments;
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

		// Cap top off with triangle fan
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

	tree_id = 0;
	int tree_id_branch_count = 0;
	vertices_per_branch = branchSegments * 2;
	std::default_random_engine gen;

	std::uniform_real_distribution<double> distribution(branchMinHeight, branchMaxHeight);
	float branch_height = distribution(gen);

	// Used to keep track of the current vertex so that indice instantiation is correct
	for (int i = 0; i < vertices_per_branch * number_of_branches; i++) {
		int currIndex = i % vertices_per_branch;

		// Calculate what plant we are
		if (currIndex == 0) {
			if (branch_bin[tree_id] == tree_id_branch_count) {
				tree_id++;
				tree_id_branch_count = 1;
				branch_height = distribution(gen);
			}
			else {
				tree_id_branch_count += 1;
				branch_height = distribution(gen);
			}
		}

		branchVertices.push_back(PlantVertex
			{
				glm::vec3( // Positions
					positions[tree_id][0],
					branch_height + noise->get(positions[tree_id][0], positions[tree_id][1]) * noise->amplitude,
					positions[tree_id][1]
				)
			}
		);

		if (currIndex % 2 == 0 and currIndex < vertices_per_branch - 2) {
			branchIndices.push_back(i);
			branchIndices.push_back(i + 3);
			branchIndices.push_back(i + 1);

			branchIndices.push_back(i);
			branchIndices.push_back(i + 2);
			branchIndices.push_back(i + 3);
		}
	}



	vao.Bind();
	// Generates Vertex Buffer Object and links it to vertices
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


	vaoBranch.Bind();
	VBO vboBranch(branchVertices);
	EBO eboBranch(branchIndices);
	vaoBranch.LinkAttrib(vboBranch, 0, 3, GL_FLOAT, sizeof(PlantVertex), (void*)0);
	vaoBranch.Unbind();
	vboBranch.Unbind();
	eboBranch.Unbind();
}

void Tree::GenerateBranchBin() {
	// Initialize the random number of leaves for each plant
	branch_bin.clear();
	number_of_branches = 0;

	for (int i = 0; i < positions.size(); i++) {
		branch_bin.push_back(std::max(rand() % maxBranches + 1, minBranches));
		number_of_branches = number_of_branches + branch_bin.back();
	}
}

void Tree::DrawTrunks
(
	Shader& trunkShader,
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
	glUniform1f(glGetUniformLocation(trunkShader.ID, "trunk_jitter"), trunkJitter);
	glUniform1f(glGetUniformLocation(trunkShader.ID, "segment_jitter"), segmentJitter);
	glUniform1f(glGetUniformLocation(trunkShader.ID, "trunk_jitter_falloff"), trunkJitterFalloff);
	glUniform1f(glGetUniformLocation(trunkShader.ID, "trunk_jitter_falloff_rate"), trunkJitterFalloffRate);
	glUniform1i(glGetUniformLocation(trunkShader.ID, "vertices_per_trunk"), vertices_per_trunk);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}


void Tree::DrawBranches
(
	Shader& branchShader,
	Camera& camera
)
{
	// Bind shader to be able to access uniforms
	branchShader.Activate();
	// Bind the VAO so OpenGL knows to use it
	vaoBranch.Bind();

	// Binds textures so that they appear in the rendering
	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;
	for (unsigned int i = 0; i < branchTextures.size(); i++)
	{
		std::string num;
		std::string type = branchTextures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		branchTextures[i].texUnit(branchShader, (type + num).c_str(), i);
		branchTextures[i].Bind();
	}

	// Exports the camera Position to the Fragment Shader for specular lighting
	glUniform3f(glGetUniformLocation(branchShader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	// Export the camMatrix to the TreeVertex Shader of the pyramid
	camera.Matrix(branchShader, "camMatrix");

	float crntTime = glfwGetTime();

	// Push the floats to the vertex shader
	glUniform1f(glGetUniformLocation(branchShader.ID, "height"), height);
	glUniform1f(glGetUniformLocation(branchShader.ID, "radius"), radius);
	glUniform1i(glGetUniformLocation(branchShader.ID, "branchSegments"), branchSegments);
	glUniform1i(glGetUniformLocation(branchShader.ID, "vertices_per_branch"), vertices_per_branch);
	glUniform1f(glGetUniformLocation(branchShader.ID, "pitch"), pitch);
	glUniform1f(glGetUniformLocation(branchShader.ID, "bendStrength"), bendStrength);
	glUniform1f(glGetUniformLocation(branchShader.ID, "time"), crntTime);
	glUniform1i(glGetUniformLocation(branchShader.ID, "segments"), segments);
	glUniform1i(glGetUniformLocation(branchShader.ID, "maxBranches"), maxBranches);
	glUniform1i(glGetUniformLocation(branchShader.ID, "branchLength"), branchLength);
	glUniform1f(glGetUniformLocation(branchShader.ID, "pitchVariance"), pitchVariance);
	glUniform1f(glGetUniformLocation(branchShader.ID, "bendVariance"), bendVariance);
	glUniform1f(glGetUniformLocation(branchShader.ID, "lengthVariance"), lengthVariance);
	glUniform1f(glGetUniformLocation(branchShader.ID, "scaleVariance"), scaleVariance);
	glUniform1f(glGetUniformLocation(branchShader.ID, "scale"), scale);



	glDrawElements(GL_TRIANGLES, branchIndices.size(), GL_UNSIGNED_INT, 0);
}