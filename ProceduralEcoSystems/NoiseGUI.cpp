#include"NoiseGUI.h"


NoiseGUI::NoiseGUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

NoiseGUI::NoiseGUI(GLFWwindow* window, float** noiseMap) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	texture = Texture(noiseMap, "diffuse", 0);
	std::cout << texture.ID << std::endl;

}

void NoiseGUI::NewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void NoiseGUI::Update() {

	ImGui::Begin("DebugWindow");
	ImGui::Text("HELLO");
	ImGui::Image((void*)(intptr_t) texture.ID, ImVec2(512.0f, 512.0f));
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void NoiseGUI::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
