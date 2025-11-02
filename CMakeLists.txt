cmake_minimum_required(VERSION 3.10)
project(learnOpenGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output executable to project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# --- GLAD ---
add_library(glad STATIC include/glad/glad.c)
target_include_directories(glad PUBLIC include)

# --- IMGUI ---
set(IMGUI_SOURCES
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
)

# --- Executable sources ---
add_executable(learnOpenGL
    main.cpp
    include/shaders/shader.cpp
    include/camera/camera.cpp
    include/mesh/mesh.cpp
    ${IMGUI_SOURCES}
)

# --- Include directories ---
target_include_directories(learnOpenGL PRIVATE
    include
    include/imgui
    include/imgui/backends
    libs/glfw/include
)

# --- Link libraries ---
if (WIN32)
    target_link_libraries(learnOpenGL PRIVATE
        glad
        ${CMAKE_SOURCE_DIR}/libs/glfw/lib-mingw-w64/libglfw3.a
        opengl32
        gdi32
    )
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(learnOpenGL PRIVATE glad glfw ${OPENGL_gl_LIBRARY} dl)
endif()

# --- ASSIMP ---
set(ASSIMP_DIR ${CMAKE_SOURCE_DIR}/libs/assimp)
target_include_directories(learnOpenGL PRIVATE ${ASSIMP_DIR}/include)

if (WIN32)
    # link release libraries
    target_link_libraries(learnOpenGL PRIVATE
        ${ASSIMP_DIR}/lib/Release/assimp-vc143-mt.lib
        ${ASSIMP_DIR}/lib/Release/unit.lib
    )
endif()
