cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(GB)

# All source files
file(GLOB SRC_FILES *.c*)

# Add the GLM include directori
include_directories(/opt/homebrew/Cellar/glm/1.0.1/include/)

add_executable(app ${SRC_FILES} ${SRC_FILES_C})

# C++17 standard
set_target_properties(app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS TRUE
)

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl -lpthread")

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(app glfw)

include_directories("../../dependencies/GLAD/include")
include_directories("../../dependencies/glm/include")
include_directories("../../dependencies/")
