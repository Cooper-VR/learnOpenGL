cmake_minimum_required(VERSION 3.10)
project(hello_opengl C)

set(CMAKE_C_STANDARD 11)

add_executable(hello
    main.c
    include/glad/glad.c
    include/shaders/shader.c
    include/matrixMath/matrix_vector_math.c
)

# Includes
target_include_directories(hello PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/glfw/include
)

if (WIN32)
    # Link to the actual static lib
    target_link_libraries(hello PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/glfw/lib-mingw-w64/libglfw3.a
        opengl32
        gdi32
    )
else()
    target_link_libraries(hello PRIVATE
        glfw
        GL
        m
        dl
        pthread
    )
endif()
