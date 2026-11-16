cmake_minimum_required(VERSION 3.15...4.4)
set(CMAKE_CXX_STANDARD 23)

project(CHIP8 VERSION 1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

set(SRC_FILES 
	src/main.cpp 
    src/chip8.cpp
    src/window.cpp
)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/sfml/sfml.git   
	GIT_TAG        master   
)
FetchContent_MakeAvailable(SFML)

add_executable(main ${SRC_FILES})
target_link_libraries(main SFML::Graphics SFML::Audio SFML::System)	
