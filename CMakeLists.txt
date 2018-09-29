cmake_minimum_required(VERSION 3.1)
project(clippson
  VERSION 0.7.3
  LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(subrepo_files
  clipp/include/clipp.h
  json/src/json.hpp
  json/src/json_fwd.hpp
)
foreach(file IN LISTS subrepo_files)
  configure_file(${file} ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/ COPYONLY)
endforeach()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-config
)
install(EXPORT ${PROJECT_NAME}-config
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME}::
)

include(CMakePackageConfigHelpers)
set(VERSION_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
write_basic_package_version_file(
  ${VERSION_CONFIG} COMPATIBILITY AnyNewerVersion
)
install(FILES ${VERSION_CONFIG}
  DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
)

include(CTest)
if(BUILD_TESTING AND ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
  add_subdirectory(test)
endif()
