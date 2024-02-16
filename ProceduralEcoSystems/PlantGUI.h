#ifndef PLANT_GUI_CLASS_H
#define PLANT_GUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Grass.h"

class PlantGUI {
public:

	PlantGUI(GLFWwindow* window);

	PlantGUI(
		GLFWwindow* window,
		Grass* input_grass
	);

	void NewTextures();

	void NewFrame();

	void Update();

	void CleanUp();

	Grass* grass;

private:
	// Plant sliders
	float sliderPitch;
	float sliderYaw;
	float sliderBendStrength;
	int sliderSegments;
	int sliderLeaves;
};

#endif