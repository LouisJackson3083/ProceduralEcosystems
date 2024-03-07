#include"GUI.h"
#include"Model.h"
#include<math.h>
#include"Terrain.h"
#include"Noise.h"
#include"Terrain.h"
#include"Grass.h"
#include"Plant.h"
#include"Ecosystem.h"
#include<typeinfo>


const unsigned int width = 1700;
const unsigned int height = 900;
unsigned int samples = 8;

float randf()
{
	return -1.0f + (rand() / (RAND_MAX / 2.0f));
}

int main()
{
	// Initialize GLFWs
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, samples);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 1200 by 800 pixels, naming it "procedural eco systems"
	GLFWwindow* window = glfwCreateWindow(width, height, "Procedural Eco-Systems", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	Shader terrainShader("terrain.vert", "terrain.frag");
	Shader plantShader("plant.vert", "plant.frag");
	Shader grassShader("grass.vert", "grass.frag");
	//Shader instancedShader("instanced.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	plantShader.Activate();
	glUniform4f(glGetUniformLocation(plantShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(plantShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	grassShader.Activate();
	glUniform4f(glGetUniformLocation(grassShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	terrainShader.Activate();
	glUniform4f(glGetUniformLocation(terrainShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(terrainShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	/*instancedShader.Activate();
	glUniform4f(glGetUniformLocation(instancedShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(instancedShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);*/

	glEnable(GL_TEXTURE_2D);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glEnable(GL_MULTISAMPLE);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clo	ck-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(width, height, glm::vec3(4.0f, 2.0f, 8.0f));
	Noise noise(0.07f, 4.0f, 2.0f, 0.6f, rand());
	Terrain terrain(4, 14, 10.0f, 1, &noise);
	std::vector<Plant> plants;
	/*plants.push_back(Plant("./Resources/PlantData/fern.plant", &noise));
	plants.push_back(Plant("./Resources/PlantData/ivy.plant", &noise));*/
	Grass grass(&noise);
	Ecosystem ecosystem(&plants);
	GUI GUI(window, &noise, &terrain, &camera, &plants, &grass, &ecosystem);

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

	// Use this to disable VSync (not advized)a
	//glfwSwapInterval(0);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0)
		{
			// Creates  title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string Title = "Terrain - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, Title.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;

			// Use this if you have disabled VSync
			//camera.Inputs(window);
		}	

		GUI.NewFrame();

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs (delete this if you have disabled VSync)
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 1000.0f);


		glDisable(GL_CULL_FACE);
		if (GUI.renderPlants) {
			for (int i = 0; i < plants.size(); i++) {
				plants[i].Draw(plantShader, camera);
			}
		}
		if (GUI.renderGrass) {
			grass.Draw(grassShader, camera);
		}

		glEnable(GL_CULL_FACE);
		if (GUI.renderTerrain) {
			terrain.Draw(terrainShader, camera);
		}

		GUI.Update();

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	GUI.CleanUp();


	// Delete all the objects we've created
	shaderProgram.Delete();
	terrainShader.Delete();
	plantShader.Delete();
	grassShader.Delete();
	//instancedShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}