#include"GUI.h"
#include<math.h>
#include"Terrain.h"
#include"Noise.h"
#include"Terrain.h"
#include"Grass.h"
#include"Plant.h"
#include"Ecosystem.h"
#include"Tree.h"
#include<typeinfo>


// Define the width and height of the screen
const unsigned int width = 1700;
const unsigned int height = 900;
// Defines the amount of samples to use for Anti Aliasing
unsigned int samples = 8;

float skyboxVertices[] =
{
	// Skybox Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};
unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};


float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

int main()
{
	#pragma region GLSetup
	// initialize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // defines the opengl version to use (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, samples); // defines the number of samples to use in anti aliasing
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // program only uses modern glfw functions, so we only need the core profile
	GLFWwindow* window = glfwCreateWindow(width, height, "Procedural Eco-Systems", NULL, NULL); // Create a window
	// terminate if the window is not created
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// make the window the current context
	glfwMakeContextCurrent(window);
	// load opengl with glad
	gladLoadGL();
	// create opengl viewport
	glViewport(0, 0, width, height);
	#pragma endregion

	#pragma region ShaderSetup
	// init all of our shader programs
	Shader shaderProgram("default.vert", "default.frag");
	Shader terrainShader("terrain.vert", "terrain.frag");
	Shader plantShader("plant.vert", "plant.frag");
	Shader trunkShader("tree.vert", "tree.frag");
	Shader branchShader("branch.vert", "branch.frag");
	Shader grassShader("grass.vert", "grass.frag");
	Shader grassShader2("grass2.vert", "grass.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");
	// init light stuff
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	// bind the light color and position to all our shaders
	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	plantShader.Activate();
	glUniform4f(glGetUniformLocation(plantShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(plantShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	trunkShader.Activate();
	glUniform4f(glGetUniformLocation(trunkShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(trunkShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	branchShader.Activate();
	glUniform4f(glGetUniformLocation(branchShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(branchShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	grassShader.Activate();
	glUniform4f(glGetUniformLocation(grassShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	grassShader2.Activate();
	glUniform4f(glGetUniformLocation(grassShader2.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassShader2.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	terrainShader.Activate();
	glUniform4f(glGetUniformLocation(terrainShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(terrainShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
	
	#pragma endregion

	// Enable textures, depth buffers, face culling and anti aliasing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);


	// Init all the things needed for the ecosystem!
	Camera camera(width, height, glm::vec3(4.0f, 2.0f, 8.0f));
	Noise noise(0.07f, 4.0f, 2.0f, 0.6f, rand());
	Terrain terrain(1, 1, 0.0f, 3, &noise, &terrainShader);
	std::vector<Plant> plants;
	std::vector<Tree> trees;
	Grass grass(&noise);
	Ecosystem ecosystem(&grass, &plants, &trees, &noise, &terrain);

	GUI GUI(window, &noise, &terrain, &camera, &plants, &trees, &grass, &ecosystem);
	GUI.renderGrass = false;
	GUI.renderTerrain = false;
	GUI.renderPlants = false;
	GUI.renderSkybox = false;
	GUI.renderTrees = false;

	std::cout << "Loaded . . ." << std::endl;
	//GUI.LoadEcosystem("./Resources/PlantData/bigForest.eco");

	// init variables for FPS counter
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	#pragma region SkyboxSetup
	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// load skybox textures
	std::string facesCubemap[6] =
	{
		"./Resources/Textures/Skybox/right.jpg",
		"./Resources/Textures/Skybox/left.jpg",
		"./Resources/Textures/Skybox/top.jpg",
		"./Resources/Textures/Skybox/bottom.jpg",
		"./Resources/Textures/Skybox/front.jpg",
		"./Resources/Textures/Skybox/back.jpg"
	};

	// create the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// wrap to avoid seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// enable as might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// cycle through all the textures and attach to the skybox
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
	#pragma endregion

	// Every frame loop
	while (!glfwWindowShouldClose(window))
	{
		// HANDLE FPS COUNTER
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string Title = FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, Title.c_str());
			prevTime = crntTime;
			counter = 0;
		}	

		GUI.NewFrame();
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);

		// handle inputs
		camera.Inputs(window);
		// update the camera matrix to all Vertex Shaders
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);

		glDisable(GL_CULL_FACE);

		if (GUI.renderPlants) {
			for (int i = 0; i < plants.size(); i++) {
				plants[i].Draw(plantShader, camera);
			}
		}
		if (GUI.renderGrass) {
			grass.Draw(grassShader, grassShader2, camera);
		}

		if (GUI.renderTrees) {
			// Branches are not rendered with face culling
			for (int i = 0; i < trees.size(); i++) {
				trees[i].DrawBranches(branchShader, camera);
			}
			// trunks are!
			glEnable(GL_CULL_FACE);
			for (int i = 0; i < trees.size(); i++) {
				trees[i].DrawTrunks(trunkShader, camera);
			}
		}

		glEnable(GL_CULL_FACE);

		if (GUI.renderTerrain) {
			terrain.Draw(terrainShader, camera);
		}


		#pragma region RenderSkybox
		if (GUI.renderSkybox) {
			glDepthFunc(GL_LEQUAL);

			skyboxShader.Activate();
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);
			view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
			projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
		}
		#pragma endregion

		GUI.Update();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up and delete all the objects we've created
	GUI.CleanUp();
	shaderProgram.Delete();
	terrainShader.Delete();
	plantShader.Delete();
	trunkShader.Delete();
	branchShader.Delete();
	grassShader.Delete();
	grassShader2.Delete();
	skyboxShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}