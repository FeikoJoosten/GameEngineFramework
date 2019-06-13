cd Source
for %%f in (*.*.spv) do del /Q %%f
for %%f in (*.*) do %VULKAN_SDK%\Bin32\glslangValidator.exe -V %%f -o %%f.spv
for %%f in (*.*.spv) do move /y %%f "..\Compiled\"
for %%f in (*.*.spv) do del /Q %%f
cd ..