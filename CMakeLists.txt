cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Find packages
# -------------------------

if(APPLE)
    # Homebrew FreeGLUT uses Mesa/GLX; Apple's OpenGL framework cannot
    # use the context it creates.
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MESA_GL REQUIRED IMPORTED_TARGET gl)
    pkg_check_modules(MESA_GLU REQUIRED IMPORTED_TARGET glu)
    pkg_check_modules(GLUT REQUIRED IMPORTED_TARGET glut)
else()
    # Windows / Linux
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

add_executable(main
    ${C_SOURCES}
    ${CXX_SOURCES}
)

# -------------------------
# Include directories
# -------------------------

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# -------------------------
# Link libraries
# -------------------------

if(APPLE)
    target_link_libraries(main PRIVATE
        PkgConfig::MESA_GL
        PkgConfig::MESA_GLU
        PkgConfig::GLUT
        "-framework CoreGraphics"
    )
else()
    target_link_libraries(main PRIVATE
        OpenGL::GL
        OpenGL::GLU
        GLUT::GLUT
    )
endif()

# if(MINGW)
#     target_link_options(main PRIVATE
#         -static
#         -static-libgcc
#         -static-libstdc++
#     )
# endif()

# -------------------------
# Compile options
# -------------------------

target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-g -O1 -fsanitize=undefined>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=undefined>
)
