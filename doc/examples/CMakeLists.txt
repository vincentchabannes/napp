
file(GLOB files *.cpp)

foreach(file ${files})
  get_filename_component(file_basename ${file} NAME_WE)
  set(testcase napp_doc_${file_basename})
  add_executable(${testcase}  ${file})
  target_link_libraries(${testcase} PRIVATE ${NAPP_TARGET_NAME} )
endforeach()

