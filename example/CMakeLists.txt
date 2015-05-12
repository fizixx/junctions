
option("JUNCTIONS_BUILD_EXAMPLE" off)

if(JUNCTIONS_BUILD_EXAMPLE)
  include("../cmake/sfml.cmake")
  add_executable("junctions_example" WIN32 MACOSX_BUNDLE "example.cpp")
  target_link_libraries("junctions_example" "junctions" "sfml-graphics")
  if(WIN32)
    target_link_libraries("junctions_example" "sfml-main")
  endif()
endif()
