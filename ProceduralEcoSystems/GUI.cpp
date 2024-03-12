#include"GUI.h"


GUI::GUI(
	GLFWwindow* window,
	Noise* input_noise,
	Terrain* input_terrain,
	Camera* input_camera,
	std::vector<Plant>* input_plants,
	std::vector<Tree>* input_trees,
	Grass* input_grass,
	Ecosystem* input_ecosystem
) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Plant/Tree
	plants = input_plants;
	trees = input_trees;

	for (int i = 0; i < plants->size(); i++) {
		plantGUIData.push_back(PlantGUIData
			{
				(*plants)[i].pitch,
				(*plants)[i].bendStrength,
				(*plants)[i].yaw,
				(*plants)[i].pitchVariance,
				(*plants)[i].bendVariance,
				(*plants)[i].segments,
				(*plants)[i].leafLength,
				(*plants)[i].lengthVariance,
				(*plants)[i].scaleVariance,
				(*plants)[i].scale,
				(*plants)[i].maxLeaves,
				(*plants)[i].minLeaves,
				(*plants)[i].layer,
				(*plants)[i].ecosystemDominance,
				(*plants)[i].ecosystemOxygenUpperLimit,
				(*plants)[i].ecosystemOxygenLowerLimit,
				(*plants)[i].ecosystemRootingStrength,
				(*plants)[i].ecosystemMoistureRequirement,
				(*plants)[i].ecosystemInteractionLevel,
			}
		);
	}
	for (int i = 0; i < trees->size(); i++) {
		treeGUIData.push_back(TreeGUIData
			{
				(*trees)[i].resolution / 2,
				(*trees)[i].segments,
				(*trees)[i].height,
				(*trees)[i].radius,
				(*trees)[i].radiusFalloff,
				(*trees)[i].radiusFalloffRate,
				(*trees)[i].pitch,
				(*trees)[i].branchSegments,
				(*trees)[i].maxBranches,
				(*trees)[i].minBranches,
				(*trees)[i].branchLength,
				(*trees)[i].bendStrength,
				(*trees)[i].pitchVariance,
				(*trees)[i].bendVariance,
				(*trees)[i].lengthVariance,
				(*trees)[i].scaleVariance,
				(*trees)[i].scale,
				(*trees)[i].layer,
				(*trees)[i].ecosystemDominance,
				(*trees)[i].ecosystemOxygenUpperLimit,
				(*trees)[i].ecosystemOxygenLowerLimit,
				(*trees)[i].ecosystemRootingStrength,
				(*trees)[i].ecosystemMoistureRequirement,
				(*trees)[i].ecosystemInteractionLevel,
			}
		);
	}


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
	noise->amplitude = sliderPatchAmplitude;
	sliderRenderDistance = terrain->render_distance;

	// Erosion
	sliderErosionDropletRadii = noise->dropletRadii;
	sliderErosionLifetime = noise->dropletLifetime;
	sliderErosionInertia = noise->inertia;
	sliderErosionDepositSpeed = noise->depositSpeed;
	sliderErosionErodeSpeed = noise->erodeSpeed;
	sliderErosionEvaporateSpeed = noise->evaporateSpeed;

	// Ecosystem stuff
	ecosystem = input_ecosystem;
	sliderPoissonRadii = ecosystem->layerRadii;


	camera = input_camera;

	renderGrass = true;
	renderTerrain = true;
	renderPlants = true;
	renderTrees = true;
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
	noiseTextures.push_back(Texture(noise, 0, 0.0f, "diffuse", 0));
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

		if (ImGui::TreeNodeEx("Erosion Images")) {
			if (noiseTextures.size() <= 3) {
				noiseTextures.push_back(Texture(noise, 1, sliderScale, "diffuse", 0));
				noiseTextures.push_back(Texture(noise, 2, sliderScale, "diffuse", 0));
				noiseTextures.push_back(Texture(noise, 3, sliderScale, "diffuse", 0));
			}
			ImGui::Text("Steepness");
			ImGui::Image((void*)(intptr_t)noiseTextures[1].ID, ImVec2(256.0f, 256.0f));
			ImGui::Text("Erosion");
			ImGui::Image((void*)(intptr_t)noiseTextures[2].ID, ImVec2(256.0f, 256.0f));
			ImGui::Text("Noise + Erosion Overlay");
			ImGui::Image((void*)(intptr_t)noiseTextures[3].ID, ImVec2(256.0f, 256.0f));


			ImGui::TreePop();
		}
		bool boolErosionDropletRadii = ImGui::SliderFloat("Distance between Droplets", &sliderErosionDropletRadii, 0.0f, 10.0f);
		bool boolErosionLifetime = ImGui::SliderInt("Lifetime of Droplets", &sliderErosionLifetime, 1, 50);
		bool boolErosionInertia = ImGui::SliderFloat("Inertia", &sliderErosionInertia, 0.0f, 1.0f);
		bool boolErosionDepositSpeed = ImGui::SliderFloat("Deposit Speed", &sliderErosionDepositSpeed, 0.0f, 1.0f);
		bool boolErosionErodeSpeed = ImGui::SliderFloat("Erosion Speed", &sliderErosionErodeSpeed, 0.0f, 1.0f);
		bool boolErosionEvaporateSpeed = ImGui::SliderFloat("Evaporation Speed", &sliderErosionEvaporateSpeed, 0.0f, 1.0f);

		// Slider Updates
		if (boolErosionDropletRadii ||
			boolErosionLifetime ||
			boolErosionInertia ||
			boolErosionDepositSpeed ||
			boolErosionErodeSpeed ||
			boolErosionEvaporateSpeed
			) {
			noise->updateErosionValues(
				sliderErosionDropletRadii,
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

		if (noise->useErosion != boolErosion) {
			if (boolErosion) {
				noise->useErosion = true;
				noise->terrainSize = (int)terrain->size * std::pow(3, 1) / 2.0f;
				noise->terrainSubdivision = (int)terrain->subdivision;
				noise->generateErosionMap();
				terrain->UpdatePatches();
			}
			else {
				noise->useErosion = false;
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
			noise->amplitude = sliderPatchAmplitude;
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
			noise->amplitude = sliderPatchAmplitude;
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
			noise->amplitude = sliderPatchAmplitude;
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
		noise->amplitude = sliderPatchAmplitude;
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
	ImGui::Checkbox("Toggle Trees", &renderTrees);

	// Presets
	if (ImGui::TreeNodeEx("Presets")) {
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
			noise->amplitude = sliderPatchAmplitude;
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
			noise->amplitude = sliderPatchAmplitude;
			terrain->size = sliderPatchSize;
			terrain->subdivision = (sliderPatchSubdivision * 3) + 1;
			terrain->UpdateRenderDistance(sliderRenderDistance);
			terrain->UpdatePatches();
		}
		ImGui::TreePop();
	}

	ImGui::End();
	#pragma endregion

	#pragma region PlantControl
	ImGui::Begin("Grass/Plant/Tree Control");
	if (ImGui::TreeNodeEx("Tree Sliders")) {


		for (int i = 0; i < (*trees).size(); i++) {
			std::string nodeName = std::string("Tree ") + std::to_string(i + 1) + std::string(" Control");
			if (ImGui::TreeNodeEx(nodeName.data())) {
				if (ImGui::TreeNodeEx("Trunk Textures")) {
					ImGui::Text("Diffuse Texture");
					ImGui::Image((void*)(intptr_t)(*trees)[i].textures[0].ID, ImVec2(256.0f, 256.0f));
					if (ImGui::Button("Change Diffuse Texture"))
						ifd::FileDialog::Instance().Open("DiffuseTextureDialog", "Change Diffuse Texture", ".*", false, "./Resources/Textures/");

					if (ifd::FileDialog::Instance().IsDone("DiffuseTextureDialog")) {
						if (ifd::FileDialog::Instance().HasResult()) {
							const std::filesystem::path res = ifd::FileDialog::Instance().GetResult();
							(*trees)[i].ChangeTreeTextures(res.u8string().c_str(), 0);
						}
						ifd::FileDialog::Instance().Close();
					}

					ImGui::Text("Specular Texture");
					ImGui::Image((void*)(intptr_t)(*trees)[i].textures[1].ID, ImVec2(256.0f, 256.0f));
					if (ImGui::Button("Change Specular Texture"))
						ifd::FileDialog::Instance().Open("SpecularTextureDialog", "Change Specular Texture", ".*", false, "./Resources/Textures/");

					if (ifd::FileDialog::Instance().IsDone("SpecularTextureDialog")) {
						if (ifd::FileDialog::Instance().HasResult()) {
							const std::filesystem::path res = ifd::FileDialog::Instance().GetResult();
							(*trees)[i].ChangeTreeTextures(res.u8string().c_str(), 1);
						}
						ifd::FileDialog::Instance().Close();
					}

					ImGui::TreePop();

				}
				if (ImGui::TreeNodeEx("Branch Textures")) {
					ImGui::Text("Diffuse Texture");
					ImGui::Image((void*)(intptr_t)(*trees)[i].branchTextures[0].ID, ImVec2(256.0f, 256.0f));
					if (ImGui::Button("Change Diffuse Texture"))
						ifd::FileDialog::Instance().Open("DiffuseTextureDialog", "Change Diffuse Texture", ".*", false, "./Resources/Textures/");

					if (ifd::FileDialog::Instance().IsDone("DiffuseTextureDialog")) {
						if (ifd::FileDialog::Instance().HasResult()) {
							const std::filesystem::path res = ifd::FileDialog::Instance().GetResult();
							(*trees)[i].ChangeBranchTextures(res.u8string().c_str(), 0);
						}
						ifd::FileDialog::Instance().Close();
					}

					ImGui::Text("Specular Texture");
					ImGui::Image((void*)(intptr_t)(*trees)[i].branchTextures[1].ID, ImVec2(256.0f, 256.0f));
					if (ImGui::Button("Change Specular Texture"))
						ifd::FileDialog::Instance().Open("SpecularTextureDialog", "Change Specular Texture", ".*", false, "./Resources/Textures/");

					if (ifd::FileDialog::Instance().IsDone("SpecularTextureDialog")) {
						if (ifd::FileDialog::Instance().HasResult()) {
							const std::filesystem::path res = ifd::FileDialog::Instance().GetResult();
							(*trees)[i].ChangeBranchTextures(res.u8string().c_str(), 1);
						}
						ifd::FileDialog::Instance().Close();
					}

					ImGui::TreePop();

				}

				if (ImGui::TreeNodeEx("Appearance Control")) {
					ImGui::Text("Trunk Parameters");
					bool boolTreeResolution = ImGui::SliderInt("Trunk Resolution", &treeGUIData[i].sliderTreeResolution, 3, 32);
					bool boolTreeSegments = ImGui::SliderInt("Tree Segments", &treeGUIData[i].sliderTreeSegments, 2, 15);
					bool boolTreeHeight = ImGui::SliderFloat("Tree Height", &treeGUIData[i].sliderTreeHeight, 1.0f, 100.0f);
					bool boolTreeRadius = ImGui::SliderFloat("Trunk Radius", &treeGUIData[i].sliderTreeRadius, 0.0f, 1.0f);
					bool boolTreeRadiusFalloff = ImGui::SliderFloat("Trunk Radius Falloff", &treeGUIData[i].sliderTreeRadiusFalloff, 0.0f, 2.0f);
					bool boolTreeRadiusFalloffRate = ImGui::SliderFloat("Trunk Radius FalloffRate", &treeGUIData[i].sliderTreeRadiusFalloffRate, 0.0f, 2.0f);
					ImGui::Text("Branch Parameters");
					bool boolBranchPitch = ImGui::SliderFloat("Branch Pitch", &treeGUIData[i].sliderBranchPitch, 0.0f, 6.0f);
					bool boolBranchMinHeight = ImGui::SliderFloat("Min Start Height", &treeGUIData[i].sliderBranchMinHeight, 0.0f, treeGUIData[i].sliderTreeHeight);
					bool boolBranchSegments = ImGui::SliderInt("Branch Segments", &treeGUIData[i].sliderBranchSegments, 2, 16);
					bool boolMaxBranches = ImGui::SliderInt("Max num of Branches", &treeGUIData[i].sliderMaxBranches, treeGUIData[i].sliderMinBranches, 24);
					bool boolMinBranches = ImGui::SliderInt("Min num of Branches", &treeGUIData[i].sliderMinBranches, 1, treeGUIData[i].sliderMaxBranches);
					bool boolBranchLength = ImGui::SliderInt("Branch Length", &treeGUIData[i].sliderBranchLength, 1, 20);
					bool boolBendStrength = ImGui::SliderFloat("Bend Strength", &treeGUIData[i].sliderBendStrength, 0.0f, 5.0f);
					bool boolPitchVariance = ImGui::SliderFloat("Pitch Variance", &treeGUIData[i].sliderPitchVariance, 0.0f, 5.0f);
					bool boolBendVariance = ImGui::SliderFloat("Bend Variance", &treeGUIData[i].sliderBendVariance, 0.0f, 5.0f);
					bool boolLengthVariance = ImGui::SliderFloat("Length Variance", &treeGUIData[i].sliderLengthVariance, 0.0f, 5.0f);
					bool boolScaleVariance = ImGui::SliderFloat("Scale Variance", &treeGUIData[i].sliderScaleVariance, 0.0f, 5.0f);
					bool boolScale = ImGui::SliderFloat("Scale", &treeGUIData[i].sliderScale, 0.0f, 5.0f);

					if (boolBranchMinHeight ||
						boolTreeResolution ||
						boolTreeSegments ||
						boolTreeHeight ||
						boolTreeRadius ||
						boolTreeRadiusFalloff ||
						boolTreeRadiusFalloffRate ||
						boolBranchPitch ||
						boolBranchSegments ||
						boolMaxBranches ||
						boolMinBranches ||
						boolBranchLength ||
						boolBendStrength ||
						boolPitchVariance ||
						boolBendVariance ||
						boolLengthVariance ||
						boolScaleVariance ||
						boolScale
						) {
						(*trees)[i].UpdateValues(treeGUIData[i]);
						(*trees)[i].GenerateBranchBin();
						(*trees)[i].GenerateVertices();
					}
					ImGui::TreePop();
				}

				std::string saveTreeString = std::string("SaveTree") + std::to_string(i + 1) + std::string("Dialog");
				if (ImGui::Button("New Seed")) {
					(*trees)[i].GenerateBranchBin();
					(*trees)[i].GenerateVertices();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save Tree")) {
					ifd::FileDialog::Instance().Save(saveTreeString.data(), "Save a tree", "*.tree {.tree}", "./Resources/PlantData/");
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Tree")) {
					(*trees).erase((*trees).begin() + i);
					std::cout << treeGUIData.size() << std::endl;
					treeGUIData.erase(treeGUIData.begin() + i);
					ecosystem->RecalculateLayers();
				}

				if (ifd::FileDialog::Instance().IsDone(saveTreeString.data())) {
					if (ifd::FileDialog::Instance().HasResult()) {
						std::string res = ifd::FileDialog::Instance().GetResult().u8string();
						(*trees)[i].SaveTreeData(&treeGUIData[i], res);
					}
					ifd::FileDialog::Instance().Close();
				}

				ImGui::TreePop();
			}
		}


		// Buttons
		if (ImGui::Button("New Tree")) {
			(*trees).push_back(Tree(noise));
			treeGUIData.push_back((*trees).back().GetGUIData());
			ecosystem->RecalculateLayers();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Tree")) {
			ifd::FileDialog::Instance().Open("LoadTreeDialog", "Load Tree", "*.tree {.tree}", false, "./Resources/PlantData/");
		}

		if (ifd::FileDialog::Instance().IsDone("LoadTreeDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {

				std::string res = ifd::FileDialog::Instance().GetResult().u8string();

				(*trees).push_back(Tree(res, noise));
				treeGUIData.push_back((*trees).back().GetGUIData());
				ecosystem->RecalculateLayers();
			}
			ifd::FileDialog::Instance().Close();
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Plant Sliders")) {


		for (int i = 0; i < (*plants).size(); i++) {
			std::string nodeName = std::string("Plant ") + std::to_string(i + 1) + std::string(" Control");
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
				if (ImGui::TreeNodeEx("Appearance Control")) {
					ImGui::Text("Leaves per Plant");
					bool boolPlantMaxLeaves = ImGui::SliderInt("Max Leaves Per Plant", &plantGUIData[i].sliderPlantMaxLeaves, plantGUIData[i].sliderPlantMinLeaves, 15);
					bool boolPlantMinLeaves = ImGui::SliderInt("Min Leaves Per Plant", &plantGUIData[i].sliderPlantMinLeaves, 1, plantGUIData[i].sliderPlantMaxLeaves);
					ImGui::Text("Angle Control");
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
						(*plants)[i].UpdateValues(plantGUIData[i]);
						(*plants)[i].GeneratePlantBin();
						(*plants)[i].GenerateVertices();
					}
					ImGui::TreePop();
				}

				std::string savePlantString = std::string("SavePlant") + std::to_string(i + 1) + std::string("Dialog");
				if (ImGui::Button("New Seed")) {
					(*plants)[i].GeneratePlantBin();
					(*plants)[i].GenerateVertices();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save Plant")) {
					ifd::FileDialog::Instance().Save(savePlantString.data(), "Save a plant", "*.plant {.plant}", "./Resources/PlantData/");
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete Plant")) {
					(*plants).erase((*plants).begin() + i);
					plantGUIData.erase(plantGUIData.begin() + i);
					ecosystem->RecalculateLayers();
				}

				if (ifd::FileDialog::Instance().IsDone(savePlantString.data())) {
					if (ifd::FileDialog::Instance().HasResult()) {
						std::string res = ifd::FileDialog::Instance().GetResult().u8string();
						(*plants)[i].SavePlantData(&plantGUIData[i], res);
					}
					ifd::FileDialog::Instance().Close();
				}

				ImGui::TreePop();
			}
		}


		// Buttons
		if (ImGui::Button("New Plant")) {
			(*plants).push_back(Plant(noise));
			plantGUIData.push_back((*plants).back().GetGUIData());
			ecosystem->RecalculateLayers();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load Plant")) {
			ifd::FileDialog::Instance().Open("LoadPlantDialog", "Load Plant", "*.plant {.plant}", false, "./Resources/PlantData/");
		}

		if (ifd::FileDialog::Instance().IsDone("LoadPlantDialog")) {
			if (ifd::FileDialog::Instance().HasResult()) {

				std::string res = ifd::FileDialog::Instance().GetResult().u8string();

				(*plants).push_back(Plant(res, noise));
				plantGUIData.push_back((*plants).back().GetGUIData());
				ecosystem->RecalculateLayers();
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

	ImGui::End();
	#pragma endregion

	#pragma region Distribution
	ImGui::Begin("EcoSystem Control");

	if (ImGui::TreeNodeEx("Distribution")) {
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
		ImGui::SameLine();
		if (ImGui::Button("Update Plant Positions")) {
			float terrainSize = (float)terrain->size * std::pow(3, terrain->render_distance)/2.0f;
			ecosystem->GeneratePoissonPositions(terrainSize);
			ecosystem->DistributePositions();
			/*ecosystem->GeneratePoissonPositions();
			(*plants)[0].positions = ecosystem->poissonPositions[0];

			std::cout << ecosystem->poissonPositions[0].size() << std::endl;
			(*plants)[0].GeneratePlantBin();
			(*plants)[0].GenerateVertices();*/
		}
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Plant Parameters")) {
		if (ecosystem->layerIndices.empty()) { ecosystem->RecalculateLayers(); }

		if (ImGui::TreeNodeEx("Layer 0 Trees")) {
			for (int j = 0; j < ecosystem->layerIndices[0].size(); j++) { // For each plant
				std::string nodeName2 = std::string("Tree ") + std::to_string(j);
				if (ImGui::TreeNodeEx(nodeName2.data())) {
					int index = ecosystem->layerIndices[0][j];
					bool boolTreeDominance = ImGui::SliderInt("Dominance", &treeGUIData[index].sliderTreeDominance, 1, 10);
					bool boolTreeOxygenUpperLimit = ImGui::SliderFloat("Oxygen Upper Limit", &treeGUIData[index].sliderTreeOxygenUpperLimit, 0.0f, 1.0f);
					bool boolTreeOxygenLowerLimit = ImGui::SliderFloat("Oxygen Lower Limit", &treeGUIData[index].sliderTreeOxygenLowerLimit, 0.0f, 1.0f);
					bool boolTreeRootingStrength = ImGui::SliderFloat("Rooting Strength Level", &treeGUIData[index].sliderTreeRootingStrength, 0.0f, 1.0f);
					bool boolTreeMoistureRequirement = ImGui::SliderFloat("Moisture Requirement", &treeGUIData[index].sliderTreeMoistureRequirement, 0.0f, 1.0f);
					bool boolTreeInteractionLevel = ImGui::SliderFloat("Interaction Level", &treeGUIData[index].sliderTreeInteractionLevel, 0.0f, 1.0f);

					if (boolTreeDominance ||
						boolTreeOxygenUpperLimit ||
						boolTreeOxygenLowerLimit ||
						boolTreeRootingStrength ||
						boolTreeMoistureRequirement ||
						boolTreeInteractionLevel
						)
					{
						(*trees)[index].UpdateValues(treeGUIData[index]);
						(*trees)[index].GenerateVertices();
					}

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		// For the plant and small plant layers
		for (int i = 1; i < ecosystem->layerIndices.size(); i++) { // For each layer

			std::string nodeName = std::string("Layer ") + std::to_string(i) + std::string(" Plants");
			if (ImGui::TreeNodeEx(nodeName.data())) {

				for (int j = 0; j < ecosystem->layerIndices[i].size(); j++) { // For each plant
					std::string nodeName2 = std::string("Plant ") + std::to_string(j);
					if (ImGui::TreeNodeEx(nodeName2.data())) {
						int index = ecosystem->layerIndices[i][j];
						bool boolPlantDominance = ImGui::SliderInt("Dominance", &plantGUIData[index].sliderPlantDominance, 1, 10);
						bool boolPlantOxygenUpperLimit = ImGui::SliderFloat("Oxygen Upper Limit", &plantGUIData[index].sliderPlantOxygenUpperLimit, 0.0f, 1.0f);
						bool boolPlantOxygenLowerLimit = ImGui::SliderFloat("Oxygen Lower Limit", &plantGUIData[index].sliderPlantOxygenLowerLimit, 0.0f, 1.0f);
						bool boolPlantRootingStrength = ImGui::SliderFloat("Rooting Strength Level", &plantGUIData[index].sliderPlantRootingStrength, 0.0f, 1.0f);
						bool boolPlantMoistureRequirement = ImGui::SliderFloat("Moisture Requirement", &plantGUIData[index].sliderPlantMoistureRequirement, 0.0f, 1.0f);
						bool boolPlantInteractionLevel = ImGui::SliderFloat("Interaction Level", &plantGUIData[index].sliderPlantInteractionLevel, 0.0f, 1.0f);
						
						if (boolPlantDominance ||
							boolPlantOxygenUpperLimit ||
							boolPlantOxygenLowerLimit ||
							boolPlantRootingStrength ||
							boolPlantMoistureRequirement ||
							boolPlantInteractionLevel
							)
						{
							(*plants)[index].UpdateValues(plantGUIData[index]);
							(*plants)[index].GeneratePlantBin();
							(*plants)[index].GenerateVertices();
						}

						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();
	#pragma endregion

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	noiseTextures.clear();
}
