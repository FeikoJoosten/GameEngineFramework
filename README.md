# Game Engine Framework 

[Docmentation:](https://feikojoosten.github.io/GameEngineFramework/)
This project is a free to use C++ engine framework.

This framework is a basis that can be used for the creation of a custom engine.
This framework was initially created over the time period of around 16 weeks as a school exercise. However, as of September 2022 I started on reworking this entire repository so it is more in line with my own standards.

Feel free to download and edit this project, but be sure to credit the original project.

A public [Trello board](https://trello.com/b/Txt0bnM5/engine-framework) is available for insights of future plans

# How to use
Set the `Engine` project as the startup project.
At this point in time you'll see a model on screen. Future plans are to have an actual scene view and game view (Unity Engine inspired) so you can actually modify your scene.

By pressing `left ctrl`, `left shift` and `e` you can open a simple developer menu with pretty much no functionality.

By opening `Engine/Utility/Defines.hpp` you can change the graphics API from `OpenGL` to `Vulkan` by changing `RENDERER OpenGL` to `RENDERER Vulkan`.
To support the Vulkan renderer, be sure to install the [VULKAN SDK](https://vulkan.lunarg.com/).

# Third Party Support
* [Assimp](https://github.com/assimp/assimp)
    Allows the loading of models/animations/textures.
* [Cereal](https://github.com/USCiLab/cereal)
    Serialization support of the project
* [GaInput](https://github.com/jkuhlmann/gainput)
    Cross-platform input system.
* [GLEW](http://glew.sourceforge.net/)
    OpenGL library
* [GLFW](https://www.glfw.org/)
    Window creation library
* [GLM](https://github.com/g-truc/glm)
    OpenGL Math library
* [IMGUI](https://github.com/ocornut/imgui)
    GUI library with support for OpenGL and Vulkan
* [nameof](https://github.com/Neargye/nameof/)
    C++ 17 utility script which allows you to retrieve names of variables/classes
* [Sharp::Event](https://www.codeproject.com/Tips/1069718/Sharp-Tools-A-NET-like-Event-in-Cplusplus)
    C++ implementation of C# Events
* [stb](https://github.com/nothings/stb)
    Used for loading of images
* [utf8cpp](https://github.com/nemtrif/utfcpp)
    Used for reading of unicode strings
* [Vulkan](https://www.vulkan.org/)
    Used for Vulkan renderer
