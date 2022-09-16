#include "Game/ImGui/ImGuiRenderer.hpp"
#include "Engine/Utility/Event.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Renderer/IMGUI/imgui.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Window/Window.hpp"

#include <glm/detail/type_vec.hpp>


ImGuiRenderer::ImGuiRenderer()
{
	Engine::Renderer::Get()->PostRenderComponentsRenderEvent += Sharp::EventHandler::Bind(this, &ImGuiRenderer::MainMenu);
}

ImGuiRenderer::~ImGuiRenderer()
{
	Engine::Renderer::Get()->PostRenderComponentsRenderEvent -= Sharp::EventHandler::Bind(this, &ImGuiRenderer::MainMenu);
}

static bool openMainMenu = true;
static bool open = true;

void ImGuiRenderer::MainMenu()
{
	// Center the window
	const std::shared_ptr<Engine::Window> window = Engine::Window::Get();
	const glm::vec2 windowSize = glm::vec2(window->GetWidth(), window->GetHeight());
	constexpr ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse;

	// Main Menu
	if (openMainMenu)
	{
		const ImVec2 imGuiWindowSize = ImVec2(200, 75);
		ImGui::SetNextWindowSize(imGuiWindowSize, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2((windowSize.x * 0.5f), (windowSize.y * 0.5f)), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5));
		ImGui::Begin("MainMenu", &open, ImVec2(420, 50), 0.f, windowFlags);

		if (ImGui::Button("Quit Game", ImVec2(200, 30)))
		{
			window->SetShouldClose(true);
		}
		ImGui::End();
	}
	// --
}
