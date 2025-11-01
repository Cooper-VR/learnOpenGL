# learnOpenGL

Minimal OpenGL learning project.

What is this?
This project is a small starter for learning modern OpenGL. It contains a minimal application that creates a window, initializes OpenGL context, loads simple shaders, and renders a basic scene. It's intended as a hands-on scaffold for experimenting with GLFW/GLAD (or GLEW), shaders, and basic rendering techniques.

## File structure


├── CMakeLists.txt  
├── README.md  
├── src/  
│   └── main.cpp  
├── include/  
│   └── (headers)  
├── shaders/  
│   ├── vertex.glsl  
│   └── fragment.glsl  
├── assets/  
│   └── (textures, models)  
└── build/ (out-of-source build directory)

## Prerequisites

- CMake  
- A compiler (MinGW on Windows, GCC/Clang on Linux)  
- OpenGL and any libraries you use (GLFW, GLEW/GLAD, glm, etc.)

## Build

Windows (MinGW):
```bash
mkdir -p build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```
Linux:
```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```
## Run

After building, run the produced executable from the build directory (name depends on CMake setup). For example:
./your_executable_name   (Linux)
your_executable_name.exe (Windows)

## Notes

- Use out-of-source builds (the build/ directory) to keep the repository clean.  
- Adjust CMakeLists.txt to link the libraries you prefer (GLFW, GLAD/GLEW, glm).  
- Replace shaders and assets as you experiment.
