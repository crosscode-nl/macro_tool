cmake_minimum_required(VERSION 3.8.2)

project(macro_tool_examples LANGUAGES C CXX)

file(GLOB EXAMPLES_SRC "example??.cpp")

foreach(EXAMPLE_CPP ${EXAMPLES_SRC})
    get_filename_component(EXAMPLE ${EXAMPLE_CPP} NAME_WE)
    message("Adding target ${EXAMPLE} with source file ${EXAMPLE_CPP}")
    add_executable(${EXAMPLE} ${EXAMPLE_CPP})
    target_link_libraries(${EXAMPLE} ${CMAKE_PROJECT_NAME})
    target_compile_features(${EXAMPLE} PUBLIC cxx_std_17)
endforeach()
