add_executable(example Example.cpp)

set_property(TARGET example PROPERTY CXX_STANDARD 14)

if(MSVC)
  set_target_properties(example PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif(MSVC)

target_link_libraries (example ConsensLib)

add_test(NAME Exampletest COMMAND example)
