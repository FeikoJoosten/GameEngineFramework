#include "Engine/Application/Application.hpp"
#include "Engine/AssetManagement/AssetManager.hpp"
#include "Engine/Camera/CameraComponent.hpp"
#include "Engine/CommandLineArguments/CommandLineArgumentsManager.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Components/LightComponent.hpp"
#include "Engine/Entity/EntitySystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Utility/EngineImGui.hpp"
//#include "Game/ImGui/ImGuiRenderer.hpp"

#if defined(_WIN32) && defined(UNICODE)
#include <utf8.h>
#endif

namespace Engine {

#if defined(_WIN32) && defined(UNICODE)
	extern "C" void wParseArguments(int argumentCount, wchar_t* wideArguments[]);

	extern "C" void wParseArguments(int argumentCount, wchar_t* wideArguments[])
#else
	void ParseArguments(int argumentCount, char* inArguments[])
#endif
	{
		// Vector of strings to parse the arguments
		std::vector<std::string> arguments;

		// Allocate beforehand to get no reallocations
		arguments.reserve(static_cast<size_t>(argumentCount));

#if defined(_WIN32) && defined(UNICODE)
		// Convert arguments from UCS-2 to UTF-8 strings
		// (support for every language ever) on Windows
		for (std::string::size_type i = 0;
			i < static_cast<std::string::size_type>(argumentCount); ++i) {
			const size_t length = wcslen(wideArguments[i]);
			arguments.emplace_back(std::string());
			utf8::utf16to8(wideArguments[i], wideArguments[i] + length, std::back_inserter(arguments[i]));
		}
		// --
#else
		// Convert arguments to strings (without converting to UTF-8 on Windows)
		// Mostly used on platforms like not-Windows.
		for (std::string::size_type i = 0;
			i < static_cast<std::string::size_type>(argumentCount); ++i) {
			arguments.emplace_back(std::string(inArguments[i]));
		}
		// --
#endif
		Engine::GetOrCreateCommandLineArgumentsManager(arguments);
	}

#if defined(_WIN32) && defined(UNICODE)
	extern "C" int wmain(int argumentCount, wchar_t* wideArguments[]);

	extern "C" int wmain(int argumentCount, wchar_t* wideArguments[])
#else
	int main(int argumentCount, char* inArguments[])
#endif
	{
#if defined(_WIN32) && defined(UNICODE)
		wParseArguments(argumentCount, wideArguments);
#else
		ParseArguments(argumentCount, inArguments);
#endif

		AssetManager::Get()->SetExecutablePath(Engine::GetOrCreateCommandLineArgumentsManager()->GetAllCommandLineArguments()[0]);
		//Engine::GetWindow();
		Engine::GetRenderer();
		Engine::GetInputManager();
		Engine::GetCameraManager(); // TODO: Replace this with scene/game view so that will auto-initialize the camera manager somehow
		std::shared_ptr<EngineImGui> engineImGui = std::make_shared<EngineImGui>();

		//// Put test stuff here

		// ImGuiRenderer handles the main menu menu's. It works standalone, but has to be cleared in the correct order.
		//std::unique_ptr<ImGuiRenderer> imGuiRenderer = std::make_unique<ImGuiRenderer>();
		const std::shared_ptr<EntitySystem> entitySystem = EntitySystem::Get();
		const std::shared_ptr<Entity> cameraEntity = entitySystem->CreateEntity("Camera").lock();
		cameraEntity->AddComponent<TransformComponent>(glm::vec3(0.f, 100.f, 200.f), glm::vec3(0.f, glm::radians(180.f), 0.f));
		cameraEntity->AddComponent<CameraComponent>(60.f, 0.1f, 1000.f);
		
		const std::shared_ptr<Entity> lockedEntity = entitySystem->CreateEntity("Amazing Entity").lock();
		const std::shared_ptr<Model> model = lockedEntity->AddComponent<ModelComponent>("xbot.fbx")->GetModel();
		//std::vector<std::string> modelAnimations {};
		if(model->HasAnimations()) {
			//model->GetAnimations(modelAnimations);
			//model->SetAnimation(modelAnimations[0], true);
			model->SetLooping(true);
		}
		lockedEntity->AddComponent<LightComponent>("light", LIGHT_AMBIENT_LIGHT, glm::vec3(0.f, 50.f, 0.f), glm::vec3(), glm::vec3(1.f, 1.f, 1.f), 3000.f, 100.f, 10.f, 10.f);
		lockedEntity->AddComponent<TransformComponent>();
		
		// TODO: Let the engine update the game instead of the other way around. Maybe move the main to the engine?
		while (!Window::Get()->ShouldClose())
		{
			Application::Update();
		}
		//modelAnimations.clear();
		//imGuiRenderer.reset();
		Application::Quit();

		return 0;
	}
}