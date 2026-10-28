cmake_minimum_required(VERSION 3.15...4.4)
set(CMAKE_CXX_STANDARD 23)

project(Cpp-Template VERSION 1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

set(SRC_FILES 
	src/main.cpp 
)

# FetchContent_Declare(
#     imgui
#     GIT_REPOSITORY https://github.com/ocornut/imgui.git
#     GIT_TAG        docking   
# )
# FetchContent_MakeAvailable(imgui)

add_executable(main ${SRC_FILES})
target_link_libraries(main)	
