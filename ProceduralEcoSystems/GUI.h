#ifndef GUI_CLASS_H
#define GUI_CLASS_H
#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include"Texture.h"
#include"Noise.h"
#include"Terrain.h"
#include"Plant.h"
#include"Grass.h"
#include<charconv>
#include<string>
#include"ImFileDialog.h"
#include <iostream>
#include <fstream>



class GUI {
public:

	GUI(
		GLFWwindow* window, 
		Noise* input_noise, 
		Terrain* input_terrain,
		Camera* input_camera,
		std::vector<Plant>* input_plants,
		Grass* input_grass
	);

	void NewNoiseTextures();

	void NewPlantTextures();

	void NewFrame();

	void Update();

	void CleanUp();

	void SavePlantData(Plant plant, PlantGUIData* plantGUIData, std::string file);

	void LoadPlantData(std::string file);

	std::vector<Texture> noiseTextures;
	std::vector<Texture> plantTextures;
	Noise* noise;
	Terrain* terrain;
	Camera* camera;

	std::vector<Plant>* plants;
	std::vector<PlantGUIData> plantGUIData;
	Plant* plant;
	Grass* grass;

	bool renderTerrain;
	bool renderGrass;
	bool renderPlants;
	bool boolWireframe;

private:

	// Grass sliders
	int sliderGrassBlades;
	float sliderGrassLength;
	float sliderGrassLengthVariance;
	float sliderGrassPitchVariance;

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


	// Camera stuff
	glm::vec2 lastCamPos = glm::vec2(0, 0);
};

#endif