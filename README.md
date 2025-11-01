# learnOpenGL

Minimal starter repo for following the learnOpenGL tutorials.

## Project layout

E:/cooperBower/github/learnOpenGL/
├─ README.md
├─ CMakeLists.txt
├─ src/
│  └─ main.cpp
├─ include/
│  └─ (headers)
├─ shaders/
│  └─ (vertex.glsl, fragment.glsl)
├─ assets/
│  └─ (textures, models)
└─ build/  (generated)

Adjust names/paths to match your code.

## Prerequisites

- CMake (>= 3.10 recommended)
- A C++ compiler (MinGW on Windows, gcc/clang on Linux)
- Libraries commonly used with OpenGL projects (install as needed):
    - GLFW
    - GLAD or GLEW
    - GLM
    - stb_image (or other helpers)

On Windows with MinGW, ensure MinGW bin is in PATH. On Linux, install the dev packages via your distro package manager.

## Building

Windows (MinGW)
mkdir -p build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .

Linux
mkdir -p build
cd build
cmake ..
cmake --build .

If you need a Release build:
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

## Running

From the build directory:
./YourExecutableName   (Linux)
YourExecutableName.exe (Windows)

## Notes

- If CMake cannot find dependencies, set CMAKE_PREFIX_PATH or provide paths via -D flags:
    -DCMAKE_PREFIX_PATH="path/to/deps"
- Keep shader and asset paths relative to the executable or provide a runtime asset path configuration.

## License

Add your preferred license file (e.g., LICENSE).