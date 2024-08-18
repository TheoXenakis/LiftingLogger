cmake_minimum_required(VERSION 3.26)
project(LiftingLogger)

set(CMAKE_CXX_STANDARD 23)

# Define the executable and its source files
cmake_minimum_required(VERSION 3.26)
project(LiftingLogger)

set(CMAKE_CXX_STANDARD 23)

# Add the include directory for the JSON library
include_directories(${PROJECT_SOURCE_DIR}/include)

# Define the executable and its source files
add_executable(LiftingLogger
        src/main.cpp
        src/helperfunctions.cpp
        src/helperfunctions.h
        src/main.h
)