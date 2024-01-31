#ifndef GUI_CLASS_H
#define GUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Texture.h"
#include"Noise.h"
#include"Patch.h"

class GUI {
public:

	GUI(GLFWwindow* window);

	GUI(GLFWwindow* window, Noise* input_noise, Patch* input_patch);

	void NewFrame();

	void Update();

	void CleanUp();

	Texture texture = Texture("./Resources/Textures/pop_cat.png", "diffuse", 0);
	Noise* noise;
	Patch* patch;

private:
	float sliderScale;
	float sliderLacunarity;
	float sliderPersistance;
	int sliderOctaves;

	// Patch stuff
	int sliderPatchSize;
	int sliderPatchSubdivision;
	float sliderPatchAmplitude;

	bool boolWireframe;
};

#endif