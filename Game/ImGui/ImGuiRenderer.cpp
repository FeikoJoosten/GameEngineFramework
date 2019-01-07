#include "Game/ImGui/ImGuiRenderer.hpp"
#include "Engine/Utility/Event.hpp"
#include "Engine/engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"

ImGuiRenderer::ImGuiRenderer()
{
	Engine::Engine::GetEngine().lock()->GetRenderer().lock()->OnRender += Sharp::EventHandler::Bind(MainMenu);
}

ImGuiRenderer::~ImGuiRenderer()
{
	Engine::Engine::GetEngine().lock()->GetRenderer().lock()->OnRender -= Sharp::EventHandler::Bind(MainMenu);
}

static bool openMainMenu = true;
static bool open = true;

void ImGuiRenderer::MainMenu()
{
	// Center the window
	glm::vec2 windowSize = glm::vec2(Engine::Engine::GetEngine().lock()->GetWindow().lock()->GetWidth(), Engine::Engine::GetEngine().lock()->GetWindow().lock()->GetHeight());
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse;

	// Main Menu
	if (openMainMenu)
	{
		ImVec2 imGuiWindowSize = ImVec2(200, 75);
		ImGui::SetNextWindowSize(imGuiWindowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2((windowSize.x * 0.5f), (windowSize.y * 0.5f)), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5));
		ImGui::Begin("MainMenu", &open, ImVec2(420, 50), 0.f, windowFlags);

		if (ImGui::Button("Quit Game", ImVec2(200, 30)))
		{
			Engine::Engine::GetEngine().lock()->GetWindow().lock()->SetShouldClose(true);
		}
		ImGui::End();
	}
	// --
}
