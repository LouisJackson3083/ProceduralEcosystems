#include"PlantGUI.h"


PlantGUI::PlantGUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

PlantGUI::PlantGUI(GLFWwindow* window, Grass* input_grass) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	// Plant sliders
	grass = input_grass;
	sliderPitch = grass->pitch;
	sliderYaw = grass->yaw;
	sliderBendStrength = grass->bendStrength;
	sliderSegments = grass->segments;
	sliderLeaves = grass->leaves;

}

void PlantGUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void PlantGUI::Update() {
	ImGui::Begin("Plant Control");

	// Patch Sliders
	if (ImGui::TreeNodeEx("Plant Sliders")) {
		bool boolPitch = ImGui::SliderFloat("Patch Size", &sliderPitch, -5.0f, 5.0f);

		// Patch Updates
		if (boolPitch) {
			grass->pitch = sliderPitch;
			grass->GenerateVertices();
		}
		ImGui::TreePop();
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void PlantGUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
