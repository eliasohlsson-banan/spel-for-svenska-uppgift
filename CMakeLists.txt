cmake_minimum_required(VERSION 3.28)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SFML_DIR "C:/libraries/SFML-3.0.2/lib/cmake/SFML")

find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System)

add_executable(main src/main.cpp)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/bin)
target_compile_features(main PRIVATE cxx_std_17)


target_link_libraries(main PRIVATE SFML::Graphics)


# Custom target to copy assets
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    COMMENT "Copying assets folder to output directory"
)

# Ensure executable depends on assets
add_dependencies(main copy_assets)

# yes i used ai for this, fuck off, cmake fucking sucks i hate this bullshit kys