cmake_minimum_required(VERSION 3.19)

project(Stack_Ball_3D)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER em++)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(game src/main.cpp)


set(EXPORTED_RUNTIME_METHODS "[\
    ccall, cwrap
]")

if(EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS='[_setCameraPos, _setCameraTarget, _main, _setWindowSize]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -03 -s USE_WEBGL2=1 -s FULL_ES3=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ../assets")

    set_target_properties(game PROPERTIES OUTPUT_NAME "../../StackBall3D")
endif()