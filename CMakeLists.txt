cmake_minimum_required(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Var din SFML-installation finns
set(SFML_DIR "C:/libraries/SFML-3.0.2/lib/cmake/SFML")

# Ladda SFML (INGEN FetchContent längre)
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)

add_executable(main src/main.cpp)

file(COPY src/fonts DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY src/texts DESTINATION ${CMAKE_BINARY_DIR}/bin)
target_compile_features(main PRIVATE cxx_std_17)

# Exakt samma syntax som ditt original-exempel!
target_link_libraries(main PRIVATE SFML::Graphics)
