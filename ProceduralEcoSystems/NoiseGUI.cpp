#include"NoiseGUI.h"


NoiseGUI::NoiseGUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

NoiseGUI::NoiseGUI(GLFWwindow* window, Noise* input_noise) {
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

	texture = Texture(noise, "diffuse", 0);
}

void NoiseGUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void NoiseGUI::Update() {

	ImGui::Begin("DebugWindow");
	ImGui::Text("HELLO");
	ImGui::Image((void*)(intptr_t) texture.ID, ImVec2(256.0f, 256.0f));
	bool boolScale = ImGui::SliderFloat("Scale", &sliderScale, 0.0f, 100.0f);
	bool boolOctaves = ImGui::SliderInt("Octaves", &sliderOctaves, 0, 50);
	bool boolPersistance = ImGui::SliderFloat("Persistance", &sliderPersistance, 0.0f, 5.0f);
	bool boolLacunarity = ImGui::SliderFloat("Lacunarity", &sliderLacunarity, 0.0f, 5.0f);
	
	if (boolScale ||
		boolLacunarity ||
		boolPersistance ||
		boolOctaves
		) {
		noise->updateNoiseValues(sliderScale, sliderOctaves, sliderPersistance, sliderLacunarity);
		texture.Delete();
		texture = Texture(noise, "diffuse", 0);
	}
	if (ImGui::Button("New Seed!")) {
		noise->updateSeed(rand());
		texture.Delete();
		texture = Texture(noise, "diffuse", 0);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void NoiseGUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	texture.Delete();
}
