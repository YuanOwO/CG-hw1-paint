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
    pkg_check_modules(GLUT REQUIRED IMPORTED_TARGET glut)
    pkg_check_modules(MESA_GL REQUIRED IMPORTED_TARGET gl)
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
)

# -------------------------
# Link libraries
# -------------------------

if(APPLE)
    target_link_libraries(main PRIVATE
        PkgConfig::MESA_GL
        PkgConfig::GLUT
        "-framework CoreGraphics"
    )
else()
    target_link_libraries(main PRIVATE
        OpenGL::GL
        GLUT::GLUT
    )
endif()


##############################
# 編譯選項
#   cmake -DCMAKE_BUILD_TYPE=Debug ..
#   cmake -DCMAKE_BUILD_TYPE=Release ..
##############################

# # Debug 編譯選項
# set(CMAKE_C_FLAGS_DEBUG "-g -O1 -fsanitize=undefined")
# set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -fsanitize=undefined")

# # Release 編譯選項
# set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# -------------------------
# Compile options
# -------------------------

target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-g -O1 -fsanitize=undefined>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

# Logic tests run without a window or an OpenGL context.
include(CTest)
if(BUILD_TESTING)
    add_executable(draw_test
        tests/draw_test.cpp
        src/draw/canvas.cpp
        src/draw/geometry.cpp
        src/draw/fill.cpp
    )
    target_include_directories(draw_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME draw_logic COMMAND draw_test)
endif()
