#ifndef NOISEGUI_CLASS_H
#define NOISEGUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Texture.h"

class NoiseGUI {
public:

	NoiseGUI(GLFWwindow* window);

	NoiseGUI(GLFWwindow* window, float** noiseMap);

	void NewFrame();

	void Update();

	void CleanUp();

	Texture texture = Texture("./Resources/Textures/pop_cat.png", "diffuse", 0);

private:
};

#endif