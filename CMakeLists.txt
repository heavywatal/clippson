cmake_minimum_required(VERSION 3.1)
project(clippson
  VERSION 0.7.1
  LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(subrepo_files
  clipp/include/clipp.h
  json/src/json.hpp
  json/src/json_fwd.hpp
)
foreach(file IN LISTS subrepo_files)
  configure_file(${file} ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME}/ COPYONLY)
endforeach()

set(VERSION_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${VERSION_CONFIG} COMPATIBILITY AnyNewerVersion
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-config
)
install(EXPORT ${PROJECT_NAME}-config
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME}::
)
install(FILES ${VERSION_CONFIG}
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
