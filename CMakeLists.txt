cmake_minimum_required(VERSION 3.0)
project("junctions")

include("cmake/nucleus.cmake")

if(NUCLEUS_BUILD_UNITTESTS)
  option("JUNCTIONS_BUILD_UNITTESTS" "Enable unit testing for this library.")
endif()

set("junctions_INCLUDE_FILES"
  "include/junctions/event_manager.h"
)

set("junctions_SOURCE_FILES"
  "src/event_manager.cpp"
)

add_library("junctions" ${junctions_INCLUDE_FILES} ${junctions_SOURCE_FILES})
target_include_directories("junctions" PUBLIC "${junctions_SOURCE_DIR}/include")
target_link_libraries("junctions" "nucleus")

if(JUNCTIONS_BUILD_UNITTESTS)
  set("junctions_TEST_FILES"
    "tests/event_manager_tests.cpp"
  )

  add_executable("junctions_tests" ${junctions_TEST_FILES})
  target_link_libraries("junctions_tests" "gtest_main" "gtest" "junctions")
endif()
