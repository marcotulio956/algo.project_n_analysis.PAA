# CMakeLists.txt
cmake_minimum_required(VERSION 3.25) # Or a higher version if needed for specific features
project(PAAAlgorithms  VERSION 1.0 DESCRIPTION "PPA Codes" LANGUAGES CXX)

# Set C++ standard to C++23 (experimental support)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: Disable GNU extensions if only standard features are desired

# Collect all .cpp files from src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Tell compiler where to find headers
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Extra compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
