cmake_minimum_required(VERSION 3.21)
project(clippson
  VERSION 0.8.8
  LANGUAGES CXX)

cmake_policy(SET CMP0169 OLD)  # 3.28
set(CMAKE_VERBOSE_MAKEFILE ON)
include(CMakePrintHelpers)
include(FetchContent)
include(GNUInstallDirs)
cmake_print_variables(CMAKE_INSTALL_PREFIX)

function(import_env variable)
  if(DEFINED ENV{${variable}})
    set(${variable} $ENV{${variable}} PARENT_SCOPE)
  endif()
endfunction()

function(git_fetch package version repo)
  string(TOUPPER "${package}" upackage)
  import_env("FETCHCONTENT_SOURCE_DIR_${upackage}")
  if(IS_DIRECTORY "${FETCHCONTENT_SOURCE_DIR_${upackage}}")
    set(tag "HEAD")
  else()
    set(tag "${version}")
  endif()
  cmake_print_variables(FETCHCONTENT_SOURCE_DIR_${upackage} tag)
  set(FETCHCONTENT_QUIET OFF)
  FetchContent_Declare(
    ${package}
    GIT_REPOSITORY https://github.com/${repo}.git
    GIT_TAG ${tag}
    GIT_SHALLOW ON
  )
  FetchContent_GetProperties(${package})
  if(NOT ${package}_POPULATED)
    FetchContent_Populate(${package})
  endif()
  set(${package}_SOURCE_DIR ${${package}_SOURCE_DIR} PARENT_SCOPE)
  cmake_print_variables(${package}_SOURCE_DIR)
endfunction()

git_fetch(clipp v1.2.3.1 heavywatal/clipp)
git_fetch(json v3.12.0 heavywatal/json)

set(clipp_srcs ${clipp_SOURCE_DIR}/include/clipp.h)
set(json_srcs ${json_SOURCE_DIR}/src/json.hpp ${json_SOURCE_DIR}/src/json_fwd.hpp)
foreach(src IN LISTS clipp_srcs json_srcs)
  cmake_print_variables(src)
  configure_file(${src} ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} COPYONLY)
endforeach()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-config
)

set(config_destination share/cmake/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}-config
  DESTINATION ${config_destination}
  NAMESPACE ${PROJECT_NAME}::
)

set(version_file ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(${version_file}
  COMPATIBILITY AnyNewerVersion
)
install(FILES ${version_file}
  DESTINATION ${config_destination}
)

include(CTest)
if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
  add_subdirectory(test)
endif()
