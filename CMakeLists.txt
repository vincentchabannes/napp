cmake_minimum_required(VERSION 3.15)

project(napp VERSION 0.1 LANGUAGES CXX)

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
set(NAPP_TARGET_NAME  ${PROJECT_NAME})


option(NAPP_ENABLE_TESTS        "Build the unit tests when BUILD_TESTING is enabled." ${MAIN_PROJECT})

# option(NAPP_MultipleHeaders     "Use non-amalgamated version of the library." OFF)
# if (NAPP_MultipleHeaders)
#   set(NAPP_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include")
#   message(STATUS "Using the multi-header code from ${NAPP_INCLUDE_BUILD_DIR}")
# else()
#   set(NAPP_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include")
#   message(STATUS "Using the single-header code from ${NAPP_INCLUDE_BUILD_DIR}")
# endif()

set(NAPP_INCLUDE_BUILD_DIR "${PROJECT_SOURCE_DIR}/include")


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
