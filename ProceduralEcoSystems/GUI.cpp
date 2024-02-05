#include"GUI.h"


GUI::GUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

GUI::GUI(GLFWwindow* window, Noise* input_noise, Terrain* input_terrain, Camera* input_camera) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	noise = input_noise;
	sliderScale = noise->scale;
	sliderOctaves = noise->octaves;
	sliderLacunarity = noise->lacunarity;
	sliderPersistance = noise->persistance;

	terrain = input_terrain;
	sliderPatchSize = terrain->size;
	sliderPatchSubdivision = terrain->subdivision / 3;
	sliderPatchAmplitude = terrain->amplitude;
	sliderRenderDistance = terrain->render_distance;

	camera = input_camera;

	boolWireframe = false;

	texture = Texture(noise, "diffuse", 0);
}

void GUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::Update() {
	ImGui::Begin("Noise Control");

	ImGui::Text("Camera Position = %f, %f", camera->Position[0], camera->Position[2]);
	ImGui::Text("Camera Anchor Position = %f, %f", camera->anchorPosition[0], camera->anchorPosition[2]);
	ImGui::Text("Total Triangles = %d", ((sliderRenderDistance * 8) + 1) * ((int)std::pow(sliderPatchSubdivision * 3, 2) * 2));
	
	//if ((int)(camera->Position[0] / terrain->size) != lastCamPos[0] or (int)(camera->Position[0] / terrain->size)) {
	//	lastCamPos = glm::vec2((int)(camera->Position[0] / terrain->size), (int)(camera->Position[2] / terrain->size));
	//	//std::cout << "AAAAA" << std::endl;
	//}

	ImGui::Image((void*)(intptr_t) texture.ID, ImVec2(256.0f, 256.0f));

	//Noise Sliders
	if (ImGui::CollapsingHeader("Noise Sliders")) {
		bool boolScale = ImGui::SliderFloat("Scale", &sliderScale, 0.0f, 5.0f);
		bool boolOctaves = ImGui::SliderInt("Octaves", &sliderOctaves, 0, 50);
		bool boolPersistance = ImGui::SliderFloat("Persistance", &sliderPersistance, 0.0f, 5.0f);
		bool boolLacunarity = ImGui::SliderFloat("Lacunarity", &sliderLacunarity, 0.0f, 5.0f);

		// Slider Updates
		if (boolScale ||
			boolLacunarity ||
			boolPersistance ||
			boolOctaves
			) {
			noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
			texture.Delete();
			texture = Texture(noise, "diffuse", 0);
		}
	}

	// Patch Sliders
	if (ImGui::CollapsingHeader("Terrain/Patch Sliders")) {
		bool boolPatchSize = ImGui::SliderInt("Patch Size", &sliderPatchSize, 1, 32);
		bool boolPatchSubdivision = ImGui::SliderInt("Patch Subdivisions", &sliderPatchSubdivision, 1, 128);
		bool boolPatchAmplitude = ImGui::SliderFloat("Patch Amplitude", &sliderPatchAmplitude, 0.0f, 64.0f);
		bool boolRenderDistance = ImGui::SliderInt("Render Distance", &sliderRenderDistance, 1, 16);

		// Patch Updates
		if (boolPatchSize ||
			boolPatchSubdivision ||
			boolPatchAmplitude
			) {
			terrain->amplitude = sliderPatchAmplitude;
			terrain->size = sliderPatchSize;
			terrain->subdivision = sliderPatchSubdivision * 3;
		}

		if (boolRenderDistance) {
			terrain->UpdateRenderDistance(sliderRenderDistance);
			terrain->UpdatePatches();
		}
	}

	// Buttons
	if (ImGui::Button("New Noise Seed")) {
		noise->updateSeed(rand());
		texture.Delete();
		texture = Texture(noise, "diffuse", 0);
	}
	if (ImGui::Button("Update Patch/Mesh")) {

		camera->anchorPosition = camera->Position;
		terrain->cameraPosition = glm::vec2(camera->Position[0], camera->Position[2]);
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

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	texture.Delete();
}
