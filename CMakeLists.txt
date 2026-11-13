cmake_minimum_required(VERSION 3.15...4.4)
set(CMAKE_CXX_STANDARD 23)

project(CHIP8 VERSION 1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

set(SRC_FILES 
	src/main.cpp 
)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL    
	GIT_TAG        main   
)
FetchContent_MakeAvailable(SDL3)

add_executable(main ${SRC_FILES})
target_link_libraries(main)	
