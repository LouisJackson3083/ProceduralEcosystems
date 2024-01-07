#ifndef NOISEGUI_CLASS_H
#define NOISEGUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Texture.h"
#include"Noise.h"

class NoiseGUI {
public:

	NoiseGUI(GLFWwindow* window);

	NoiseGUI(GLFWwindow* window, Noise* input_noise);

	void NewFrame();

	void Update();

	void CleanUp();

	Texture texture = Texture("./Resources/Textures/pop_cat.png", "diffuse", 0);
	Noise* noise;

private:
	float sliderScale;
	float sliderLacunarity;
	float sliderPersistance;
	int sliderOctaves;
};

#endif