Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2 #v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)


add_library( napp_test_lib1  lib1.cpp)
target_link_libraries(napp_test_lib1 PRIVATE ${NAPP_TARGET_NAME} )

file(GLOB files unit-*.cpp)
foreach(file ${files})
  get_filename_component(file_basename ${file} NAME_WE)
  #string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})
  set(testcase napp_test_${file_basename})
  add_executable(${testcase}  ${file})
  target_link_libraries(${testcase} PRIVATE ${NAPP_TARGET_NAME} Catch2::Catch2WithMain)

  if ( ${testcase} STREQUAL  "napp_test_unit-lib" )
    target_link_libraries(${testcase} PRIVATE napp_test_lib1 )
  endif()

  add_test(NAME "${testcase}"
            COMMAND ${testcase}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
