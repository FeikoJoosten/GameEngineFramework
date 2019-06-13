#include <iostream>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>
#include <functional>
#include "Engine/engine.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Game/ImGui/ImGuiRenderer.hpp"
#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Components/LightComponent.hpp"

#if defined(_WIN32) && defined(UNICODE)
#include <utf8.h>
#include <direct.h>
#endif

#if defined(_WIN32) && defined(UNICODE)
extern "C" void wParseArguments(int argumentCount, wchar_t *wideArguments[]);

extern "C" void wParseArguments(int argumentCount, wchar_t *wideArguments[])
#else
void ParseArguments(int argumentCount, char *inArguments[])
#endif
{
	// Vector of strings to parse the arguments
	eastl::vector<eastl::string> arguments;

	// Allocate beforehand to get no reallocations
	arguments.reserve(static_cast<size_t>(argumentCount));

#if defined(_WIN32) && defined(UNICODE)
	// Convert arguments from UCS-2 to UTF-8 strings
	// (support for every language ever) on Windows
	for (eastl::string::size_type i = 0;
		i < static_cast<eastl::string::size_type>(argumentCount); ++i)
	{
		size_t length = wcslen(wideArguments[i]);
		arguments.emplace_back(eastl::string());
		utf8::utf16to8(wideArguments[i], wideArguments[i] + length,	std::back_inserter(arguments[i]));
		std::cout << arguments[i].c_str() << std::endl;
	}
	// --
#else
	// Convert arguments to strings (without converting to UTF-8 on Windows)
	// Mostly used on platforms like not-Windows.
	for (eastl::string::size_type i = 0;
		i < static_cast<eastl::string::size_type>(argumentCount); ++i)
	{
		arguments.emplace_back(eastl::string(inArguments[i]));
	}
	// --
#endif
}

#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain(int argumentCount, wchar_t *wideArguments[]);

extern "C" int wmain(int argumentCount, wchar_t *wideArguments[])
#else
int main(int argumentCount, char *inArguments[])
#endif
{
#if defined(_WIN32) && defined(UNICODE)
	wParseArguments(argumentCount, wideArguments);
#else
	ParseArguments(argumentCount, inArguments);
#endif
	Engine::Engine::GetEngine().lock()->SetIsPlaying(false);

	//// Put test stuff here

	// ImGuiRenderer handles the main menu menu's. It works standalone, but has to be cleared in the correct order.
	eastl::unique_ptr<ImGuiRenderer> imGuiRenderer = eastl::make_unique<ImGuiRenderer>();
	Engine::Engine::GetEngine().lock()->GetCamera().lock()->SetPostionAndRotation(glm::vec3(0, 30, -75), glm::vec3(-44.15f, 9.43f, 1));
	eastl::weak_ptr<Engine::EntitySystem> entitySystem = Engine::Engine::GetEngine().lock()->GetEntitySystem();
	eastl::weak_ptr<Engine::Entity> entity = entitySystem.lock()->CreateEntity("Amazing Entity");
	entity.lock()->AddComponent<Engine::TransformComponent>();
	entity.lock()->AddComponent<Engine::ModelComponent>("jeep1.fbx");
	entity.lock()->AddComponent<Engine::LightComponent>("light", LIGHT_POINT_LIGHT, glm::vec3(0, 50, 0), glm::vec3(), glm::vec3(1, 1, 1), 3000, 100, 10, 10);

	while (!Engine::Engine::GetEngine().lock()->GetWindow().lock()->ShouldClose())
	{
		Engine::Engine::GetEngine().lock()->Update();
	}
	imGuiRenderer.reset();
	Engine::Engine::GetEngine().lock()->Destroy();

	return 0;
}
