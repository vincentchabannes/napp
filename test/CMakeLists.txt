

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)


#file(GLOB files src/unit-*.cpp)
file(GLOB files *.cpp)

foreach(file ${files})
  get_filename_component(file_basename ${file} NAME_WE)
  #string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})
  set(testcase ${file_basename})
  #add_executable(${testcase} $<TARGET_OBJECTS:doctest_main> ${file})
  add_executable(${testcase}  ${file})
  target_link_libraries(${testcase} PRIVATE ${NAPP_TARGET_NAME} Catch2::Catch2WithMain)

  add_test(NAME "${testcase}"
            COMMAND ${testcase}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
