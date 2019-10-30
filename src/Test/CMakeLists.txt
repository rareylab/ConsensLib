function(build_test TEST_NAME TEST_FILE FLAGS)
  set(TEST_EXECUTABLE "${TEST_NAME}_tester")
  add_executable(${TEST_EXECUTABLE} main.cpp ${TEST_FILE})

  set_property(TARGET ${TEST_EXECUTABLE} PROPERTY CXX_STANDARD 14)

  if(MSVC)
    set_target_properties("${TEST_EXECUTABLE}" PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  endif(MSVC)

  target_link_libraries (${TEST_EXECUTABLE} ConsensLib)
  target_link_libraries (${TEST_EXECUTABLE} gtest_main)

  add_test(NAME ${TEST_NAME} COMMAND ${TEST_EXECUTABLE} ${FLAGS})
endfunction()

build_test(SimpleTest SimpleTest.cpp "")
build_test(CustomizedTest CustomizedTest.cpp "")
