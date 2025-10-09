cmake_minimum_required(VERSION 3.10)

# Set the project name
project(StudentManagementSystem)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES
        main.cpp
        src/*.cpp
        controllers/*.cpp
)

# Executable
add_executable(${PROJECT_NAME}
        ${SOURCES}

)

# Link libraries
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Drogon::Drogon
)

# Use C++20 features
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE .)
