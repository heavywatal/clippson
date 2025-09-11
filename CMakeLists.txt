cmake_minimum_required(VERSION 3.24)
project(clippson
  VERSION 0.8.10
  LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
include(CMakePrintHelpers)
include(FetchContent)
include(GNUInstallDirs)
cmake_print_variables(CMAKE_INSTALL_PREFIX)

FetchContent_Declare(clipp URL https://github.com/heavywatal/clipp/archive/refs/tags/v1.2.3.2.tar.gz)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)

FetchContent_MakeAvailable(clipp)
cmake_print_variables(clipp_SOURCE_DIR)

set(JSON_Install OFF)
set(JSON_MultipleHeaders OFF)
FetchContent_MakeAvailable(json)
cmake_print_variables(json_SOURCE_DIR)

foreach(src
  ${clipp_SOURCE_DIR}/include/clipp.h
  ${json_SOURCE_DIR}/single_include/nlohmann/json.hpp
  ${json_SOURCE_DIR}/single_include/nlohmann/json_fwd.hpp
)
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
