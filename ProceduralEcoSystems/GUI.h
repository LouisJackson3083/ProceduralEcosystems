#ifndef GUI_CLASS_H
#define GUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Texture.h"
#include"Noise.h"
#include"Terrain.h"
#include<charconv>

class GUI {
public:

	GUI(GLFWwindow* window);

	GUI(
		GLFWwindow* window, 
		Noise* input_noise, 
		Terrain* input_terrain,
		Camera* input_camera
	);

	void NewTextures();

	void NewFrame();

	void Update();

	void CleanUp();

	std::vector<Texture> noiseTextures;
	Noise* noise;
	Terrain* terrain;
	Camera* camera;

private:
	float sliderScale;
	float sliderLacunarity;
	float sliderPersistance;
	int sliderOctaves;

	// Terrain/Patch stuff
	int sliderPatchSize;
	int sliderPatchSubdivision;
	float sliderPatchAmplitude;
	int sliderRenderDistance;

	bool boolWireframe;

	// Camera stuff
	glm::vec2 lastCamPos = glm::vec2(0, 0);
};

#endif