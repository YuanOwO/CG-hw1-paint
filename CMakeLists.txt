cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Find packages
# -------------------------

if(NOT APPLE)
    # Windows / Linux: FreeGLUT
    # macOS uses the system GLUT and OpenGL frameworks (see Link libraries).
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
endif()

# -------------------------
# Source files
# -------------------------

file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

file(GLOB_RECURSE CXX_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# -------------------------
# Executable
# -------------------------

add_executable(graphtoria
    ${C_SOURCES}
    ${CXX_SOURCES}
)

add_custom_command(TARGET graphtoria POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:graphtoria>/assets"
    COMMENT "Copying assets"
)

# -------------------------
# Include directories
# -------------------------

target_include_directories(graphtoria PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# -------------------------
# Link libraries
# -------------------------

if(APPLE)
    # The system GLUT framework (Apple OpenGL, hardware accelerated).
    # The OpenGL framework also provides GLU.
    target_link_libraries(graphtoria PRIVATE
        "-framework GLUT"
        "-framework OpenGL"
    )

    # OpenGL and GLUT are deprecated on macOS since 10.14.
    target_compile_definitions(graphtoria PRIVATE GL_SILENCE_DEPRECATION)
else()
    target_link_libraries(graphtoria PRIVATE
        OpenGL::GL
        OpenGL::GLU
        GLUT::GLUT
    )
endif()

if(MINGW)
    target_link_options(graphtoria PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

# -------------------------
# Compile options
# -------------------------

target_compile_options(graphtoria PRIVATE
    $<$<CONFIG:Debug>:-g -O1 -fsanitize=undefined>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

target_link_options(graphtoria PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=undefined>
)
