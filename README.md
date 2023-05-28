# Procedural-3D-Ray-Marching-Engine

A realtime OpenGL rendering engine that uses ray marching and ray tracing. Provides means for comparing performance metrics between the two rendering methods, along with supporting examples and custom scenes using ray marching, based on custom signed distance functions.

<kbd> <img src="https://user-images.githubusercontent.com/94473602/208584047-92380933-df19-4def-b6c0-11c76665d830.png"> </kbd>

## **Features**
* Ray Marching & Ray Tracing 
* Preset/example test scenes
  * 3D fractal rendering
* Custom scene editor (ray marching)
* Performance metrics and customizable parameters

## **Lighting Effects**
* Antialiasing (SSAAx4)
* Ambient Occlusion
* Reflections
* Phong-based lighting models

## **Libraries Used**
* [GLFW](https://www.glfw.org/download.html) 3.3 (compiled using Win32 due to binaries)
  * OpenGL library
* [GLM](https://github.com/g-truc/glm) (OpenGL Mathematics)
  * For useful math functions
* [Dear ImGui](https://github.com/ocornut/imgui)
  * Used to create rendering interface
 
## **Compiling**
### Requirements:
* C++ 17 Compiler or IDE
* [CMake](https://cmake.org/download/) (tested using version 3.26)
  * Ensure it is added to the system’s environment PATH variable
* OpenGL-compatible GPU  + Drivers
## Build Instructions
1. Clone/download repository to your machine
2. Open terminal in directory where Cmakelists.txt file is found, and run the following:
```
mkdir build
cd build
cmake ..
cmake –build .
```
3. If you would like to execute the project directly from the terminal, open a new terminal in ``` Procedural-3D-Ray-Marching-Engine\build\Debug ```, and run the following:
```
./OpenGL_Rendering_Engine
```
## **Images** 

||
| :---: |
| *Rendering Engine Layout* |
| ![enginelayoutimage](https://user-images.githubusercontent.com/94473602/216489917-f19a9b38-e8dc-4a12-a669-c15252612c29.PNG) |
| *3D "Mandelbulb" Fractal; power 8* |
| ![mandelbulb](https://user-images.githubusercontent.com/94473602/208584035-2631a828-e886-4dcc-8066-153b8c5d219b.png) |
| *Ray marched example scene with different operations (Union, Smooth Minimum, Subtraction)* |
| ![Screenshot 2023-01-29 230529 1000x1000](https://user-images.githubusercontent.com/94473602/216490160-d737c2ab-c089-4813-8288-e033fc4f1330.png) |


