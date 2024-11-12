cmake_minimum_required(VERSION 3.15)

project(napp VERSION 0.3 LANGUAGES CXX)

##
## MAIN_PROJECT CHECK
## determine if nap is built as a subproject (using add_subdirectory) or if it is the main project
##
set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MAIN_PROJECT ON)

  if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
  endif(NOT CMAKE_BUILD_TYPE)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(NAPP_TARGET_NAME  ${PROJECT_NAME})
set(NAPP_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
if(NOT DEFINED NAPP_CONFIG_INSTALL_DIR )
  set(NAPP_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE INTERNAL "")
endif()
set(NAPP_TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(NAPP_CMAKE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(NAPP_CMAKE_PROJECT_TARGETS_FILE "${NAPP_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake")


option(NAPP_ENABLE_TESTS        "Build the unit tests when NAPP_ENABLE_TESTS is enabled." ${MAIN_PROJECT})
option(NAPP_ENABLE_DOC        "Build the doc examples tests when NAPP_ENABLE_DOC is enabled." ${MAIN_PROJECT})
option(NAPP_ENABLE_EXAMPLES        "Build examples when NAPP_ENABLE_EXAMPLES is enabled." ${MAIN_PROJECT})

# option(NAPP_MultipleHeaders     "Use non-amalgamated version of the library." OFF)
# if (NAPP_MultipleHeaders)
#   set(NAPP_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include")
#   message(STATUS "Using the multi-header code from ${NAPP_INCLUDE_BUILD_DIR}")
# else()
#   set(NAPP_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include")
#   message(STATUS "Using the single-header code from ${NAPP_INCLUDE_BUILD_DIR}")
# endif()

set(NAPP_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include/")


add_library(${NAPP_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${NAPP_TARGET_NAME} ALIAS ${NAPP_TARGET_NAME})
target_compile_features(${NAPP_TARGET_NAME} INTERFACE cxx_std_17)
target_include_directories(
    ${NAPP_TARGET_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${NAPP_INCLUDE_BUILD_DIR}>
    $<INSTALL_INTERFACE:include>
)


##
## TESTS
## create and configure the unit test target
##
if (NAPP_ENABLE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()


if (NAPP_ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

if (NAPP_ENABLE_DOC)
  add_subdirectory(doc)
endif()



install(
  DIRECTORY ${NAPP_INCLUDE_BUILD_DIR}
  DESTINATION ${NAPP_INCLUDE_INSTALL_DIR}
  )
export(
  TARGETS ${NAPP_TARGET_NAME}
  NAMESPACE ${PROJECT_NAME}::
  FILE ${NAPP_CMAKE_PROJECT_TARGETS_FILE}
  )
install(
  TARGETS ${NAPP_TARGET_NAME}
  EXPORT ${NAPP_TARGETS_EXPORT_NAME}
  INCLUDES DESTINATION ${NAPP_INCLUDE_INSTALL_DIR}
  )
# Install CMake package configuration files
write_basic_package_version_file(
  "${NAPP_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"   # Create this file as explained below
  "${NAPP_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${NAPP_CONFIG_INSTALL_DIR}
)
install(
  EXPORT ${NAPP_TARGETS_EXPORT_NAME}
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${NAPP_CONFIG_INSTALL_DIR}
  )
install(
  FILES
    "${NAPP_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake"
    "${NAPP_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${NAPP_CONFIG_INSTALL_DIR}
)
