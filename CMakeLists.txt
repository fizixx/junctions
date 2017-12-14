cmake_minimum_required(VERSION 3.7.2)

project(junctions)

# Options

set(CMAKE_CXX_STANDARD 14)

# Dependencies

include("cmake/nucleus.cmake")

# junctions

set(junctions_INCLUDE_FILES
    "include/junctions/entity.h"
    "include/junctions/entity_manager.h"
    "include/junctions/system_manager.h"
    "include/junctions/utils.h"
    )

set(junctions_SOURCE_FILES
    "src/entity.cpp"
    "src/entity_manager.cpp"
    "src/system_manager.cpp"
    )

add_library(junctions ${junctions_INCLUDE_FILES} ${junctions_SOURCE_FILES})
target_include_directories(junctions PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(junctions nucleus)
set_property(TARGET junctions PROPERTY FOLDER junctions)

# junctions_tests

set("junctions_TEST_FILES"
    "tests/entity_manager_tests.cpp"
    "tests/system_manager_tests.cpp"
    )

add_executable(junctions_tests ${junctions_TEST_FILES})
target_link_libraries(junctions_tests googletest junctions)
set_property(TARGET junctions_tests PROPERTY FOLDER junctions)

add_subdirectory("example")
