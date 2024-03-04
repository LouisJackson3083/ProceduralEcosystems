#include"GUI.h"


GUI::GUI(
	GLFWwindow* window, 
	Noise* input_noise, 
	Terrain* input_terrain, 
	Camera* input_camera,
	std::vector<Plant>* input_plants,
	Grass* input_grass,
	Ecosystem* input_ecosystem
) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Plant
	plants = input_plants;
	plantGUIData.push_back(PlantGUIData
		{
			(*plants)[0].plantNumber,
			(*plants)[0].pitch,
			(*plants)[0].bendStrength,
			(*plants)[0].yaw,
			(*plants)[0].pitchVariance,
			(*plants)[0].bendVariance,
			(*plants)[0].segments,
			(*plants)[0].leafLength,
			(*plants)[0].lengthVariance,
			(*plants)[0].scaleVariance,
			(*plants)[0].scale,
			(*plants)[0].maxLeaves,
			(*plants)[0].minLeaves
		}
	);


	// Grass
	grass = input_grass;
	sliderGrassBlades = grass->blades;
	sliderGrassLength = grass->length;
	sliderGrassLengthVariance = grass->lengthVariance;
	sliderGrassPitchVariance = grass->pitchVariance;

	// Noise
	noise = input_noise;
	sliderScale = noise->scale;
	sliderOctaves = noise->octaves;
	sliderLacunarity = noise->lacunarity;
	sliderPersistance = noise->persistance;

	// Terrain
	terrain = input_terrain;
	sliderPatchSize = terrain->size;
	sliderPatchSubdivision = terrain->subdivision / 3;
	sliderPatchAmplitude = terrain->amplitude;
	grass->amplitude = sliderPatchAmplitude;
	sliderRenderDistance = terrain->render_distance;

	// Erosion
	sliderErosionNumDroplets = noise->numDroplets;
	sliderErosionLifetime = noise->dropletLifetime;
	sliderErosionInertia = noise->inertia;
	sliderErosionDepositSpeed = noise->depositSpeed;
	sliderErosionErodeSpeed = noise->erodeSpeed;
	sliderErosionEvaporateSpeed = noise->evaporateSpeed;
	boolErosion = terrain->useErosion;

	// Ecosystem stuff
	ecosystem = input_ecosystem;
	sliderPoissonRadii = ecosystem->layerRadii;


	camera = input_camera;

	renderGrass = true;
	renderTerrain = true;
	renderPlants = true;
	boolWireframe = false;

	NewNoiseTextures();
	
	NewPlantTextures();

	// ImFileDialog requires you to set the CreateTexture and DeleteTexture
	ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
		GLuint tex;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return (void*)tex;
		};
	ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
		GLuint texID = (GLuint)((uintptr_t)tex);
		glDeleteTextures(1, &texID);
		};


}

void GUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::NewNoiseTextures() {
	noiseTextures.clear();
	noiseTextures.push_back(Texture(noise, 0, 0.0f, false, "diffuse", 0));
}

void GUI::NewPlantTextures() {
	plantTextures.clear();
	plantTextures.push_back(Texture("diffuse", 0));
}

void GUI::NewPoissonDiskTexture() {
	poissonTextures.clear();
	poissonTextures.push_back(Texture("diffuse", 0, ecosystem->layerRadii));
}

void GUI::Update() {
	#pragma region NoiseRegion
	ImGui::Begin("Noise Control");

	// Image Stuff
	if (ImGui::TreeNodeEx("Noise Image")) {
		ImGui::Image((void*)(intptr_t)noiseTextures[0].ID, ImVec2(256.0f, 256.0f));

		if (ImGui::TreeNodeEx("Alternate Images")) {
			if (noiseTextures.size() <= 3) {
				noiseTextures.push_back(Texture(noise, 1, sliderScale, false, "diffuse", 0));
				noiseTextures.push_back(Texture(noise, 2, sliderScale, terrain->useErosion, "diffuse", 0));
				noiseTextures.push_back(Texture(noise, 3, sliderScale, terrain->useErosion, "diffuse", 0));
			}
			ImGui::Text("Steepness");
			ImGui::Image((void*)(intptr_t)noiseTextures[1].ID, ImVec2(256.0f, 256.0f));
			ImGui::Text("Erosion");
			ImGui::Image((void*)(intptr_t)noiseTextures[2].ID, ImVec2(256.0f, 256.0f));
			ImGui::Text("Noise + Erosion Overlay");
			ImGui::Image((void*)(intptr_t)noiseTextures[3].ID, ImVec2(256.0f, 256.0f));


			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	// Noise Sliders
	if (ImGui::TreeNodeEx("Noise Sliders")) {
		bool boolScale = ImGui::SliderFloat("Scale", &sliderScale, 0.00001f, 5.0f);
		bool boolOctaves = ImGui::SliderInt("Octaves", &sliderOctaves, 1, 50);
		bool boolPersistance = ImGui::SliderFloat("Persistance", &sliderPersistance, 0.0f, 5.0f);
		bool boolLacunarity = ImGui::SliderFloat("Lacunarity", &sliderLacunarity, 0.0f, 5.0f);

		// Noise Type
		if (ImGui::TreeNodeEx("Noise Type")) {
			if (ImGui::Button("OpenSimplex2")) {
				noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
			}
			ImGui::SameLine();
			if (ImGui::Button("OpenSimplex2S")) {
				noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cellular")) {
				noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
			}
			if (ImGui::Button("Perlin")) {
				noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
			}
			ImGui::SameLine();
			if (ImGui::Button("NoiseType_ValueCubic")) {
				noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
			}
			ImGui::SameLine();
			if (ImGui::Button("NoiseType_Value")) {
				noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Value);
			}
			ImGui::TreePop();
		}

		// Fractal Noise Type
		if (ImGui::TreeNodeEx("Fractal Noise Type")) {
			if (ImGui::Button("None")) {
				noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_None);
			}
			ImGui::SameLine();
			if (ImGui::Button("FBm")) {
				noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_FBm);
			}
			ImGui::SameLine();
			if (ImGui::Button("Ridged")) {
				noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_Ridged);
			}
			ImGui::SameLine();
			if (ImGui::Button("Domain Warp Progressive")) {
				noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_DomainWarpProgressive);
			}

			if (ImGui::Button("Ping Pong")) {
				noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_PingPong);
			}
			ImGui::SameLine();
			if (ImGui::Button("Domain Warp Independent")) {
				noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_DomainWarpIndependent);
			}
			ImGui::TreePop();
		}

		// Slider Updates
		if (boolScale ||
			boolLacunarity ||
			boolPersistance ||
			boolOctaves
			) {
			noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
			NewNoiseTextures();
		}

		ImGui::TreePop();
	}

	// Erosion Sliders
	if (ImGui::TreeNodeEx("Erosion Sliders")) {

		bool boolErosionNumDroplets = ImGui::SliderInt("Number of Droplets", &sliderErosionNumDroplets, 1, 200);
		bool boolErosionLifetime = ImGui::SliderInt("Lifetime of Droplets", &sliderErosionLifetime, 1, 50);
		bool boolErosionInertia = ImGui::SliderFloat("Inertia", &sliderErosionInertia, 0.0f, 1.0f);
		bool boolErosionDepositSpeed = ImGui::SliderFloat("Deposit Speed", &sliderErosionDepositSpeed, 0.0f, 1.0f);
		bool boolErosionErodeSpeed = ImGui::SliderFloat("Erosion Speed", &sliderErosionErodeSpeed, 0.0f, 1.0f);
		bool boolErosionEvaporateSpeed = ImGui::SliderFloat("Evaporation Speed", &sliderErosionEvaporateSpeed, 0.0f, 1.0f);

		// Slider Updates
		if (boolErosionNumDroplets ||
			boolErosionLifetime ||
			boolErosionInertia ||
			boolErosionDepositSpeed ||
			boolErosionErodeSpeed ||
			boolErosionEvaporateSpeed
			) {
			noise->updateErosionValues(
				sliderErosionNumDroplets,
				sliderErosionLifetime, 
				sliderErosionInertia, 
				sliderErosionDepositSpeed,
				sliderErosionErodeSpeed,
				sliderErosionEvaporateSpeed
			);
		}

		// Buttons
		if (ImGui::Button("Update Erosion Texture")) {
			noise->generateErosionMap();
			NewNoiseTextures();
		}

		// Use Erosion Checkbox
		ImGui::Checkbox("Toggle Erosion", &boolErosion);

		if (terrain->useErosion != boolErosion) {
			if (boolErosion) {
				terrain->useErosion = true;
				terrain->UpdatePatches();
			}
			else {
				terrain->useErosion = false;
				terrain->UpdatePatches();
			}
		}
		
		ImGui::TreePop();
	}

	// Patch Sliders
	if (ImGui::TreeNodeEx("Terrain/Patch Sliders")) {
		bool boolPatchSize = ImGui::SliderInt("Patch Size", &sliderPatchSize, 1, 32);
		bool boolPatchSubdivision = ImGui::SliderInt("Patch Subdivisions", &sliderPatchSubdivision, 1, 128);
		bool boolPatchAmplitude = ImGui::SliderFloat("Patch Amplitude", &sliderPatchAmplitude, 0.0f, 64.0f);
		bool boolRenderDistance = ImGui::SliderInt("Render Distance", &sliderRenderDistance, 1, 16);

		if (boolRenderDistance) {
			terrain->size = sliderPatchSize;
			terrain->subdivision = (sliderPatchSubdivision * 3) + 1;
			terrain->amplitude = sliderPatchAmplitude;
			grass->amplitude = sliderPatchAmplitude;
			terrain->UpdateRenderDistance(sliderRenderDistance);
			terrain->UpdatePatches();
		}
		ImGui::TreePop();
	}

	// Presets
	if (ImGui::TreeNodeEx("Terrain Presets")) {
		if (ImGui::Button("Plains")) {
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_Ridged);

			sliderScale = 0.5f;
			sliderOctaves = 6.0f;
			sliderPersistance = 2.0f;
			sliderLacunarity = 0.6f;

			sliderPatchSize = 3.0f;
			sliderPatchSubdivision = 4.0f;
			sliderPatchAmplitude = 20.0f;
			sliderRenderDistance = 5.0f;


			noise->updateSeed(rand());
			noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
			NewNoiseTextures();
			terrain->amplitude = sliderPatchAmplitude;
			grass->amplitude = sliderPatchAmplitude;
			terrain->size = sliderPatchSize;
			terrain->subdivision = (sliderPatchSubdivision * 3) + 1;
			terrain->UpdateRenderDistance(sliderRenderDistance);
			terrain->UpdatePatches();
		}
		if (ImGui::Button("Mountain")) {
			noise->fastNoiseLite.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
			noise->fastNoiseLite.SetFractalType(FastNoiseLite::FractalType_Ridged);

			sliderScale = 0.45f;
			sliderOctaves = 5.0f;
			sliderPersistance = 0.2f;
			sliderLacunarity = 0.35f;

			sliderPatchSize = 7.0f;
			sliderPatchSubdivision = 28.0f;
			sliderPatchAmplitude = 64.0f;
			sliderRenderDistance = 3;

			noise->updateSeed(rand());
			noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
			NewNoiseTextures();
			terrain->amplitude = sliderPatchAmplitude;
			grass->amplitude = sliderPatchAmplitude;
			terrain->size = sliderPatchSize;
			terrain->subdivision = (sliderPatchSubdivision * 3) + 1;
			terrain->UpdateRenderDistance(sliderRenderDistance);
			terrain->UpdatePatches();
		}
		ImGui::TreePop();
	}

	// Buttons
	if (ImGui::Button("New Noise Seed")) {
		noise->updateSeed(rand());
		noise->generateErosionMap();
		NewNoiseTextures();
	}
	// Buttons
	/*if (ImGui::Button("Update Terrain to Current Location")) {
		noise->updateSeed(rand());
		NewNoiseTextures();
	}*/
	if (ImGui::Button("Update Patch/Mesh")) {
		terrain->size = sliderPatchSize;
		terrain->subdivision = (sliderPatchSubdivision * 3) + 1;
		terrain->amplitude = sliderPatchAmplitude;
		terrain->UpdatePatches();
	}



	ImGui::End();
	#pragma endregion
	
	#pragma region RenderRegion
	ImGui::Begin("Render Control");

	ImGui::Text("Camera Position = %f, %f", camera->Position[0], camera->Position[2]);
	ImGui::Text("Total Triangles = %d", ((sliderRenderDistance * 8) + 1)* ((int)std::pow(sliderPatchSubdivision * 3, 2) * 2));

	// Wireframe Checkbox
	ImGui::Checkbox("Toggle Wireframe", &boolWireframe);
	if (boolWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Enable wireframe mode
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Disable wireframe mode
	}
	ImGui::Checkbox("Toggle Terrain", &renderTerrain);
	ImGui::Checkbox("Toggle Grass", &renderGrass);
	ImGui::Checkbox("Toggle Plants", &renderPlants);

	ImGui::End();
	#pragma endregion

	#pragma region PlantControl
	ImGui::Begin("Grass/Plant Control");

	if (ImGui::TreeNodeEx("Plant Sliders")) {


		for (int i = 0; i < (*plants).size(); i++) {
			std::string nodeName = std::string("Plant ") + std::to_string(i+1) + std::string(" Control");
			if (ImGui::TreeNodeEx(nodeName.data())) {
				if (ImGui::TreeNodeEx("Textures")) {
					ImGui::Text("Diffuse Texture");
					ImGui::Image((void*)(intptr_t)(*plants)[i].textures[0].ID, ImVec2(256.0f, 256.0f));
					if (ImGui::Button("Change Diffuse Texture"))
						ifd::FileDialog::Instance().Open("DiffuseTextureDialog", "Change Diffuse Texture", ".*", false, "./Resources/Textures/");

					if (ifd::FileDialog::Instance().IsDone("DiffuseTextureDialog")) {
						if (ifd::FileDialog::Instance().HasResult()) {
							const std::filesystem::path res = ifd::FileDialog::Instance().GetResult();
							(*plants)[i].ChangeTextures(res.u8string().c_str(), 0);
						}
						ifd::FileDialog::Instance().Close();
					}

					ImGui::Text("Specular Texture");
					ImGui::Image((void*)(intptr_t)(*plants)[i].textures[1].ID, ImVec2(256.0f, 256.0f));
					if (ImGui::Button("Change Specular Texture"))
						ifd::FileDialog::Instance().Open("SpecularTextureDialog", "Change Specular Texture", ".*", false, "./Resources/Textures/");

					if (ifd::FileDialog::Instance().IsDone("SpecularTextureDialog")) {
						if (ifd::FileDialog::Instance().HasResult()) {
							const std::filesystem::path res = ifd::FileDialog::Instance().GetResult();
							(*plants)[i].ChangeTextures(res.u8string().c_str(), 1);
						}
						ifd::FileDialog::Instance().Close();
					}

					ImGui::TreePop();

				}
				ImGui::Text("Plant/Leaf Control");
				bool boolPlantNumber = ImGui::SliderInt("Num of Plant", &plantGUIData[i].sliderPlantNumber, 0, 200);
				bool boolPlantMaxLeaves = ImGui::SliderInt("Max Leaves Per Plant", &plantGUIData[i].sliderPlantMaxLeaves, plantGUIData[i].sliderPlantMinLeaves, 15);
				bool boolPlantMinLeaves = ImGui::SliderInt("Min Leaves Per Plant", &plantGUIData[i].sliderPlantMinLeaves, 1, plantGUIData[i].sliderPlantMaxLeaves);
				ImGui::Text("Angles");
				bool boolPlantPitch = ImGui::SliderFloat("Pitch", &plantGUIData[i].sliderPlantPitch, -6.0f, 6.0f);
				bool boolPlantYaw = ImGui::SliderFloat("Yaw", &plantGUIData[i].sliderPlantYaw, -6.0f, 6.0f);
				bool boolPlantBendStrength = ImGui::SliderFloat("Bend Strength", &plantGUIData[i].sliderPlantBendStrength, -0.5f, 0.5f);
				ImGui::Text("Length/Size Control");
				bool boolPlantSegments = ImGui::SliderInt("Leaf Segments", &plantGUIData[i].sliderPlantSegments, 0, 15);
				bool boolPlantLength = ImGui::SliderInt("Leaf Length", &plantGUIData[i].sliderPlantLeafLength, 0, 15);
				bool boolPlantScale = ImGui::SliderFloat("Scale", &plantGUIData[i].sliderPlantScale, 0.0f, 2.0f);
				ImGui::Text("Variation Control");
				bool boolPlantLengthVariance = ImGui::SliderFloat("Length Variance", &plantGUIData[i].sliderPlantLengthVariance, 0.0f, 5.0f);
				bool boolPlantPitchVariance = ImGui::SliderFloat("Pitch Variance", &plantGUIData[i].sliderPlantPitchVariance, 0.0f, 5.0f);
				bool boolPlantBendVariance = ImGui::SliderFloat("Bend Variance", &plantGUIData[i].sliderPlantBendVariance, 0.0f, 5.0f);
				bool boolPlantScaleVariance = ImGui::SliderFloat("Scale Variance", &plantGUIData[i].sliderPlantScaleVariance, 0.0f, 5.0f);
				if (boolPlantPitch ||
					boolPlantYaw ||
					boolPlantBendStrength ||
					boolPlantSegments ||
					boolPlantMaxLeaves ||
					boolPlantMinLeaves ||
					boolPlantLength ||
					boolPlantLengthVariance ||
					boolPlantPitchVariance ||
					boolPlantBendVariance ||
					boolPlantScaleVariance ||
					boolPlantScale
					) {
					(*plants)[i].pitch = plantGUIData[i].sliderPlantPitch;
					(*plants)[i].yaw = plantGUIData[i].sliderPlantYaw;
					(*plants)[i].bendStrength = plantGUIData[i].sliderPlantBendStrength;
					(*plants)[i].segments = plantGUIData[i].sliderPlantSegments;
					(*plants)[i].vertices_per_leaf = 2*plantGUIData[i].sliderPlantSegments;
					(*plants)[i].maxLeaves = plantGUIData[i].sliderPlantMaxLeaves;
					(*plants)[i].minLeaves = plantGUIData[i].sliderPlantMinLeaves;
					(*plants)[i].leafLength = plantGUIData[i].sliderPlantLeafLength;
					(*plants)[i].lengthVariance = plantGUIData[i].sliderPlantLengthVariance;
					(*plants)[i].pitchVariance = plantGUIData[i].sliderPlantPitchVariance;
					(*plants)[i].bendVariance = plantGUIData[i].sliderPlantBendVariance;
					(*plants)[i].scaleVariance = plantGUIData[i].sliderPlantScaleVariance;
					(*plants)[i].scale = plantGUIData[i].sliderPlantScale;
					(*plants)[i].GenerateVertices();
				}

				if (boolPlantNumber) {
					(*plants)[i].plantNumber = plantGUIData[i].sliderPlantNumber;
					(*plants)[i].GeneratePlantBin();
					(*plants)[i].GenerateVertices();
				}

				if (ImGui::Button("New Seed")) {
					(*plants)[i].GeneratePlantBin();
					(*plants)[i].GenerateVertices();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save Plant")) {
					ifd::FileDialog::Instance().Save("PlantSaveDialog", "Save a plant", "*.plant {.plant}", "./Resources/PlantData/");
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Plant")) {
					(*plants).erase((*plants).begin() + i);
					plantGUIData.erase(plantGUIData.begin() + i);
				}

				if (ifd::FileDialog::Instance().IsDone("PlantSaveDialog")) {
					if (ifd::FileDialog::Instance().HasResult()) {
						std::string res = ifd::FileDialog::Instance().GetResult().u8string();
						SavePlantData((*plants)[i], &plantGUIData[i], res);
					}
					ifd::FileDialog::Instance().Close();
				}

				ImGui::TreePop();
			}
		}


		// Buttons
		if (ImGui::Button("New Plant")) {
			(*plants).push_back(0);
			plantGUIData.push_back(PlantGUIData
				{
					(*plants).back().plantNumber,
					(*plants).back().pitch,
					(*plants).back().bendStrength,
					(*plants).back().yaw,
					(*plants).back().pitchVariance,
					(*plants).back().bendVariance,
					(*plants).back().segments,
					(*plants).back().leafLength,
					(*plants).back().lengthVariance,
					(*plants).back().scaleVariance,
					(*plants).back().scale,
					(*plants).back().maxLeaves,
					(*plants).back().minLeaves
				}
			);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Plant"))
			ifd::FileDialog::Instance().Open("LoadPlantDialog", "Load Plant", "*.plant {.plant}", false, "./Resources/PlantData/");

		if (ifd::FileDialog::Instance().IsDone("LoadPlantDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {
				std::string res = ifd::FileDialog::Instance().GetResult().u8string();
				LoadPlantData(res);
			}
			ifd::FileDialog::Instance().Close();
		}


		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Grass Sliders")) {
		bool boolGrassBlades = ImGui::SliderInt("Leaves", &sliderGrassBlades, 0, 100);
		bool boolGrassLength = ImGui::SliderFloat("Leaf Length", &sliderGrassLength, 0.01f, 15.0f);
		bool boolGrassLengthVariance = ImGui::SliderFloat("Length Variance", &sliderGrassLengthVariance, 0.0f, 5.0f);
		bool boolGrassPitchVariance = ImGui::SliderFloat("Pitch Variance", &sliderGrassPitchVariance, 0.0f, 5.0f);

		// Patch Updates
		if (boolGrassBlades ||
			boolGrassLength ||
			boolGrassLengthVariance ||
			boolGrassPitchVariance
			) {
			grass->blades = sliderGrassBlades;
			grass->length = sliderGrassLength;
			grass->lengthVariance = sliderGrassLengthVariance;
			grass->pitchVariance = sliderGrassPitchVariance;
			grass->GenerateVertices();
		}
		ImGui::TreePop();
	}


	if (ImGui::TreeNodeEx("Distribution Texture")) {
		ImGui::Image((void*)(intptr_t)plantTextures[0].ID, ImVec2(256.0f, 256.0f));
		ImGui::TreePop();
	}

	ImGui::End();
	#pragma endregion


	#pragma region Distribution
	ImGui::Begin("EcoSystem Control");

	if (ImGui::TreeNodeEx("Poisson Disk Image")) {
		if (poissonTextures.empty()) { NewPoissonDiskTexture(); }
		ImGui::Image((void*)(intptr_t)poissonTextures[0].ID, ImVec2(256.0f, 256.0f));

		ImGui::Text("Layer Radii Control");
		for (int i = 0; i < 3; i++) {
			std::string nodeName = std::string("Layer ") + std::to_string(i + 1) + std::string(" radius");
			bool boolPoissonRadii = ImGui::SliderFloat(nodeName.data(), &sliderPoissonRadii[i], 0.0f, 50.0f);
			if (boolPoissonRadii) {
				ecosystem->layerRadii = sliderPoissonRadii;
			}
		}
		
		if (ImGui::Button("Refresh Texture")) {
			NewPoissonDiskTexture();
		}
		
		ImGui::TreePop();
	}

	ImGui::End();
	#pragma endregion

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void GUI::SavePlantData(Plant plant, PlantGUIData* plantGUIData, std::string file) {
	std::ofstream myfile;
	myfile.open(file);
	myfile << plantGUIData->sliderPlantNumber << ",";

	myfile << plantGUIData->sliderPlantPitch << ",";
	myfile << plantGUIData->sliderPlantBendStrength << ",";
	myfile << plantGUIData->sliderPlantYaw << ",";
	myfile << plantGUIData->sliderPlantPitchVariance << ",";
	myfile << plantGUIData->sliderPlantBendVariance << ",";

	myfile << plantGUIData->sliderPlantSegments << ",";
	myfile << plantGUIData->sliderPlantLeafLength << ",";
	myfile << plantGUIData->sliderPlantLengthVariance << ",";

	myfile << plantGUIData->sliderPlantScaleVariance << ",";
	myfile << plantGUIData->sliderPlantScale << ",";
	myfile << plantGUIData->sliderPlantMaxLeaves << ",";
	myfile << plantGUIData->sliderPlantMinLeaves << ",";

	myfile << plant.plant_texture_filepaths[0] << ",";
	myfile << plant.plant_texture_filepaths[1] << ",";
	
	myfile.close();
}

void GUI::LoadPlantData(std::string file) {
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

		plantGUIData.push_back(PlantGUIData
			{
				std::stoi(results[0]),
				std::stof(results[1]),
				std::stof(results[2]),
				std::stof(results[3]),
				std::stof(results[4]),
				std::stof(results[5]),
				std::stoi(results[6]),
				std::stoi(results[7]),
				std::stof(results[8]),
				std::stof(results[9]),
				std::stof(results[10]),
				std::stoi(results[11]),
				std::stoi(results[12]),
			}
		);
		(*plants).push_back(Plant(plantGUIData.back(), results[13], results[14]));
		myfile.close();
	}
}

void GUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	noiseTextures.clear();
}
