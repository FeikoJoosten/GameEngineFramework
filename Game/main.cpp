#include <iostream>
#include <ThirdParty/EASTL-master/include/EASTL/vector.h>
#include <functional>
#include "Engine/engine.hpp"
#include "Engine/Components/ModelComponent.hpp"
#include "Engine/Components/TransformComponent.hpp"
#include "Game/ImGui/ImGuiRenderer.hpp"

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
	eastl::vector<std::string> arguments;

	// Allocate beforehand to get no reallocations
	arguments.reserve(static_cast<size_t>(argumentCount));

#if defined(_WIN32) && defined(UNICODE)
	// Convert arguments from UCS-2 to UTF-8 strings
	// (support for every language ever) on Windows
	for (std::string::size_type i = 0;
		i < static_cast<std::string::size_type>(argumentCount); ++i)
	{
		size_t length = wcslen(wideArguments[i]);
		arguments.emplace_back(std::string());
		utf8::utf16to8(wideArguments[i], wideArguments[i] + length,
			std::back_inserter(arguments[i]));
		std::cout << arguments[i].c_str() << std::endl;
	}
	// --
#else
	// Convert arguments to strings (without converting to UTF-8 on Windows)
	// Mostly used on platforms like not-Windows.
	for (std::string::size_type i = 0;
		i < static_cast<std::string::size_type>(argumentCount); ++i)
	{
		arguments.emplace_back(std::string(inArguments[i]));
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

	//// Put test stuff here

	Engine::Engine::CreateAndReturnWindow(640, 480, "");
	Engine::Engine::CreateAndReturnRenderer();

	Engine::Engine::CreateAndReturnInputManager();
	
	ImGuiRenderer imGuiRenderer;

	while(!Engine::Engine::GetWindow().ShouldClose())
	{
		Engine::Engine::Update();
	}

	return 0;
}
