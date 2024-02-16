#ifndef GUI_CLASS_H
#define GUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Texture.h"
#include"Noise.h"
#include"Terrain.h"
#include"Plant.h"
#include<charconv>

class GUI {
public:

	GUI(GLFWwindow* window);

	GUI(
		GLFWwindow* window, 
		Noise* input_noise, 
		Terrain* input_terrain,
		Camera* input_camera,
		Plant* input_plant
	);

	void NewNoiseTextures();

	void NewPlantTextures();

	void NewFrame();

	void Update();

	void CleanUp();

	std::vector<Texture> noiseTextures;
	std::vector<Texture> plantTextures;
	Noise* noise;
	Terrain* terrain;
	Camera* camera;
	Plant* plant;

private:
	// Plant sliders
	float sliderPlantPitch;
	float sliderPlantYaw;
	float sliderPlantBendStrength;
	int sliderPlantSegments;
	int sliderPlantLeaves;
	int sliderPlantLeafLength;
	float sliderPlantLengthVariance;
	float sliderPlantPitchVariance;
	float sliderPlantBendVariance;


	// Noise sliders
	float sliderScale;
	float sliderLacunarity;
	float sliderPersistance;
	int sliderOctaves;

	// Erosion sliders
	int sliderErosionNumDroplets;
	int sliderErosionLifetime;
	float sliderErosionInertia;
	float sliderErosionDepositSpeed;
	float sliderErosionErodeSpeed;
	float sliderErosionEvaporateSpeed;
	bool boolErosion;


	// Terrain/Patch sliders
	int sliderPatchSize;
	int sliderPatchSubdivision;
	float sliderPatchAmplitude;
	int sliderRenderDistance;

	bool boolWireframe;

	// Camera stuff
	glm::vec2 lastCamPos = glm::vec2(0, 0);
};

#endif