# Game Engine Framework 

[Docmentation:](https://feikojoosten.github.io/GameEngineFramework/)
This project is a free to use C++ engine framework.

This framework is a basis that can be used for the creation of a custom engine.
This framework was created over the time period of around 16 weeks.

This framework DOES NOT include an audio player implementation.

Feel free to download and edit this project, but be sure to credit the original project.

# How to use
Set the `Game` project as the startup project and make sure the x64 target is selected.
A window should pop up with a simple menu stating a `Quit Game` button.

By pressing `left ctrl`, `left shift` and `e` you can open a simple developer menu with pretty much no functionality.

By opening `Engine/Utility/Defines.hpp` you can change the graphics API from `OpenGL` to `Vulkan` by changing `RENDERER OpenGL` to `RENDERER Vulkan`.
To support the Vulkan renderer, be sure to install the [VULKAN SDK](https://vulkan.lunarg.com/).

# Third Party Support
* [Assimp](https://github.com/assimp/assimp)
    Allows the loading of models/animations/textures.
* [Box2D](https://github.com/erincatto/Box2D)
    Simple collision detection system.
* [Cereal](https://github.com/USCiLab/cereal)
    Serialization support of the project.
* [EASTL](https://github.com/electronicarts/EASTL)
    Makes use of EASTL as much as possible.
* [GaInput](https://github.com/jkuhlmann/gainput)
    Cross-platform input system.
* [GLM](https://github.com/g-truc/glm)
    OpenGL Math library

## Please note
This project will no longer be worked on and is purely for portfolio.
By default there is a simple object rendering which can be rendered using OpenGL and Vulkan.
The OpenGL renderer has no lighting support, the Vulkan renderer does.
