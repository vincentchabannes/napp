
#file(GLOB files src/unit-*.cpp)
file(GLOB files *.cpp)

foreach(file ${files})
  get_filename_component(file_basename ${file} NAME_WE)
  #string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})
  set(testcase ${file_basename})
  #add_executable(${testcase} $<TARGET_OBJECTS:doctest_main> ${file})
  add_executable(${testcase}  ${file})
  target_link_libraries(${testcase} PRIVATE ${NAPP_TARGET_NAME})

  add_test(NAME "${testcase}"
            COMMAND ${testcase}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
