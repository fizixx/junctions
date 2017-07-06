cmake_minimum_required(VERSION 3.7.2)
project("junctions")

# General options

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 14)

# Dependencies

option(JUNCTIONS_BUILD_DEPENDENCIES "Build all dependencies with this library")
if (JUNCTIONS_BUILD_DEPENDENCIES)
    include("cmake/nucleus.cmake")
endif ()

# junctions

set("junctions_INCLUDE_FILES"
        "include/junctions/entity.h"
        "include/junctions/entity_manager.h"
        "include/junctions/event_manager.h"
        "include/junctions/system_manager.h"
        "include/junctions/utils.h"
        )

set("junctions_SOURCE_FILES"
        "src/entity.cpp"
        "src/entity_manager.cpp"
        "src/event_manager.cpp"
        "src/system_manager.cpp"
        )

add_library("junctions" ${junctions_INCLUDE_FILES} ${junctions_SOURCE_FILES})
target_include_directories("junctions" PUBLIC "${junctions_SOURCE_DIR}/include")
target_link_libraries("junctions" "nucleus")
set_property(TARGET "junctions" PROPERTY FOLDER "junctions")

# junctions_tests

set("junctions_TEST_FILES"
        "tests/entity_manager_tests.cpp"
        "tests/event_manager_tests.cpp"
        "tests/system_manager_tests.cpp"
        )

add_executable("junctions_tests" ${junctions_TEST_FILES})
target_link_libraries("junctions_tests" "gtest" "gtest_main" "junctions")
set_property(TARGET "junctions_tests" PROPERTY FOLDER "junctions")

add_subdirectory("example")
