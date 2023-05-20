# Procedural-3D-Ray-Marching-Engine

Realtime OpenGL rendering engine that has support for ray marching and ray tracing rendering. Provides means for comparing performance metrics between the two rendering methods, along with providing support for examples and custom scenes using ray marching, based on custom signed distance functions.

<kbd> <img src="https://user-images.githubusercontent.com/94473602/208584047-92380933-df19-4def-b6c0-11c76665d830.png"> </kbd>

## **Features**
* Ray marching and Ray tracing rendering
* Preset/example scenes
* Fractal rendering example (ray marching)
* Scene editor (ray marching)
* Performance metrics (graphs) and custom parameters

## **Lighting Effects**
* Antialiasing (SSAAx4)
* Ambient Occlusion
* Reflections
* Phong lighting model

## **Libraries Used**
* GLFW 3.3
* GLM
* Dear ImGui

## **Compiling**
Engine interface created using [ImGui](https://github.com/ocornut/imgui). In order to run the engine, you must download [CMake](https://cmake.org/download/)
### Requirements:
* C++ 17 Compiler
* CMake (tested using version 3.26)
* OpenGL-compatible GPU  + Drivers

## **Images** 

||
| :---: |
| *Rendering Engine Layout* |
| ![enginelayoutimage](https://user-images.githubusercontent.com/94473602/216489917-f19a9b38-e8dc-4a12-a669-c15252612c29.PNG) |
| *3D "Mandelbulb" Fractal; power 8* |
| ![mandelbulb](https://user-images.githubusercontent.com/94473602/208584035-2631a828-e886-4dcc-8066-153b8c5d219b.png) |
| *Ray marched example scene with different operations (Union, Smooth Minimum, Subtraction)* |
| ![Screenshot 2023-01-29 230529 1000x1000](https://user-images.githubusercontent.com/94473602/216490160-d737c2ab-c089-4813-8288-e033fc4f1330.png) |

