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
    include/model/mesh/mesh.cpp
    include/model/model.cpp
    include/loaders/stb_image.cpp
    ${IMGUI_SOURCES}
)

# --- Include directories ---
target_include_directories(learnOpenGL PRIVATE
    include
    include/imgui
    include/imgui/backends
    libs/glfw/include
    libs/assimp/include
)

# --- Platform specific linking ---
if (WIN32)
    target_link_libraries(learnOpenGL PRIVATE
        glad
        ${CMAKE_SOURCE_DIR}/libs/glfw/lib-mingw-w64/libglfw3.a
        opengl32
        gdi32
        ${CMAKE_SOURCE_DIR}/libs/assimp/lib/libassimp.dll.a
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    # On Linux, use system packages for GLFW + Assimp if available
    find_package(glfw3 QUIET)
    find_package(assimp QUIET)

    if (NOT glfw3_FOUND)
        message(WARNING "GLFW not found — linking manually")
        target_link_directories(learnOpenGL PRIVATE ${CMAKE_SOURCE_DIR}/libs/glfw/lib-linux)
        target_link_libraries(learnOpenGL PRIVATE glfw)
    else()
        target_link_libraries(learnOpenGL PRIVATE glfw)
    endif()

    if (NOT assimp_FOUND)
        message(WARNING "Assimp not found — linking manually")
        target_link_directories(learnOpenGL PRIVATE ${CMAKE_SOURCE_DIR}/libs/assimp/bin)
        target_link_libraries(learnOpenGL PRIVATE assimp)
    else()
        target_link_libraries(learnOpenGL PRIVATE assimp::assimp)
    endif()

    target_link_libraries(learnOpenGL PRIVATE
        glad
        ${OPENGL_LIBRARIES}
        dl
        Threads::Threads
    )
endif()
