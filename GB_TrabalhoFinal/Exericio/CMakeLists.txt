cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(GB)

# All source files
file(GLOB SRC_FILES *.c*)

# Include directories for dependencies
include_directories("../../dependencies/GLAD/include")
include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

# Add the executable
add_executable(app ${SRC_FILES})

# C++17 standard
set_target_properties(app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS TRUE
)


# Find and link GLFW
# find_package(glfw3 3.4 REQUIRED)
target_link_libraries(app glfw config++)

