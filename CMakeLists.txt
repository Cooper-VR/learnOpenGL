cmake_minimum_required(VERSION 3.10)
project(hello_opengl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- GLAD library ---
add_library(glad_obj OBJECT include/glad/glad.c)
target_include_directories(glad_obj PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(glad STATIC $<TARGET_OBJECTS:glad_obj>)
add_dependencies(glad glad_obj)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# --- Main program ---
add_executable(hello
    main.cpp
    include/shaders/shader.cpp
    include/camera/camera.cpp
)

target_include_directories(hello PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/glfw/include
)

# --- Link order ---
if (WIN32)
    target_link_libraries(hello PRIVATE
        glad
        ${CMAKE_SOURCE_DIR}/libs/glfw/lib-mingw-w64/libglfw3.a
        opengl32
        gdi32
    )
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(hello PRIVATE
        glad
        glfw
        ${OPENGL_LIBRARIES}
        m
        dl
        pthread
    )
endif()

add_dependencies(hello glad)
