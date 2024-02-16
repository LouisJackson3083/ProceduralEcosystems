#include"GUI.h"


GUI::GUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

GUI::GUI(
	GLFWwindow* window, 
	Noise* input_noise, 
	Terrain* input_terrain, 
	Camera* input_camera,
	Plant* input_plant
) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Plant
	plant = input_plant;
	sliderPlantPitch = plant->pitch;
	sliderPlantYaw = plant->yaw;
	sliderPlantBendStrength = plant->bendStrength;
	sliderPlantSegments = plant->segments;
	sliderPlantLeaves = plant->leaves;
	sliderPlantLeafLength = plant->leafLength;
	sliderPlantLengthVariance = plant->lengthVariance;
	sliderPlantPitchVariance = plant->pitchVariance;
	sliderPlantBendVariance = plant->bendVariance;

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
	sliderRenderDistance = terrain->render_distance;

	// Erosion
	sliderErosionNumDroplets = noise->numDroplets;
	sliderErosionLifetime = noise->dropletLifetime;
	sliderErosionInertia = noise->inertia;
	sliderErosionDepositSpeed = noise->depositSpeed;
	sliderErosionErodeSpeed = noise->erodeSpeed;
	sliderErosionEvaporateSpeed = noise->evaporateSpeed;
	boolErosion = terrain->useErosion;

	camera = input_camera;

	boolWireframe = false;

	NewNoiseTextures();
	NewPlantTextures();
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

void GUI::Update() {
	#pragma region NoiseRegion
	ImGui::Begin("Noise Control");

	ImGui::Text("Camera Position = %f, %f", camera->Position[0], camera->Position[2]);
	ImGui::Text("Total Triangles = %d", ((sliderRenderDistance * 8) + 1) * ((int)std::pow(sliderPatchSubdivision * 3, 2) * 2));
	
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

		// Patch Updates
		if (boolPatchSize ||
			boolPatchSubdivision ||
			boolPatchAmplitude
			) {
			terrain->size = sliderPatchSize;
			terrain->subdivision = sliderPatchSubdivision * 3;
			terrain->amplitude = sliderPatchAmplitude;
		}

		if (boolRenderDistance) {
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
			sliderPatchSubdivision = 3.0f;
			sliderPatchAmplitude = 20.0f;
			sliderRenderDistance = 5.0f;

			noise->updateSeed(rand());
			noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
			NewNoiseTextures();
			terrain->amplitude = sliderPatchAmplitude;
			terrain->size = sliderPatchSize;
			terrain->subdivision = sliderPatchSubdivision * 3;
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
			sliderPatchSubdivision = 29.0f;
			sliderPatchAmplitude = 64.0f;
			sliderRenderDistance = 3;

			noise->updateSeed(rand());
			noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
			NewNoiseTextures();
			terrain->amplitude = sliderPatchAmplitude;
			terrain->size = sliderPatchSize;
			terrain->subdivision = sliderPatchSubdivision * 3;
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

		terrain->UpdatePatches();
	}
	// Wireframe Checkbox
	ImGui::Checkbox("Toggle Wireframe", &boolWireframe);
	if (boolWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Enable wireframe mode
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Disable wireframe mode
	}



	ImGui::End();
	#pragma endregion

	#pragma region NoiseRegion
	ImGui::Begin("Plant Control");

	if (ImGui::TreeNodeEx("Plant Texture")) {
		ImGui::Image((void*)(intptr_t)plantTextures[0].ID, ImVec2(256.0f, 256.0f));
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("Plant Sliders")) {

		ImGui::Text("Angle Control");
		bool boolPlantPitch = ImGui::SliderFloat("Pitch", &sliderPlantPitch, -6.0f, 6.0f);
		bool boolPlantYaw = ImGui::SliderFloat("Yaw", &sliderPlantYaw, -6.0f, 6.0f);
		bool boolPlantBendStrength = ImGui::SliderFloat("Bend Strength", &sliderPlantBendStrength, -0.5f, 0.5f);
		ImGui::Text("Leaf Control");
		bool boolPlantSegments = ImGui::SliderInt("Segments", &sliderPlantSegments, 0, 15);
		bool boolPlantLeaves = ImGui::SliderInt("Leaves", &sliderPlantLeaves, 0, 15);
		bool boolPlantLength = ImGui::SliderInt("Leaf Length", &sliderPlantLeafLength, 0, 15);
		ImGui::Text("Variation Control");
		bool boolPlantLengthVariance = ImGui::SliderFloat("Length Variance", &sliderPlantLengthVariance, 0.0f, 0.5f);
		bool boolPlantPitchVariance = ImGui::SliderFloat("Pitch Variance", &sliderPlantPitchVariance, 0.0f, 50.5f);
		bool boolPlantBendVariance = ImGui::SliderFloat("Bend Variance", &sliderPlantBendVariance, 0.0f, 0.5f);

		// Patch Updates
		if (boolPlantPitch ||
			boolPlantYaw ||
			boolPlantBendStrength ||
			boolPlantSegments ||
			boolPlantLeaves ||
			boolPlantLength ||
			boolPlantLengthVariance ||
			boolPlantPitchVariance ||
			boolPlantBendVariance
			) {
			plant->pitch = sliderPlantPitch;
			plant->yaw = sliderPlantYaw;
			plant->bendStrength = sliderPlantBendStrength;
			plant->segments = sliderPlantSegments;
			plant->leaves = sliderPlantLeaves;
			plant->leafLength = sliderPlantLeafLength;
			plant->lengthVariance = sliderPlantLengthVariance;
			plant->pitchVariance = sliderPlantPitchVariance;
			plant->bendVariance = sliderPlantBendVariance;
			plant->GenerateVertices();
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
