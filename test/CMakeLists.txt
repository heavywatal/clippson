link_libraries(${PROJECT_NAME}::${PROJECT_NAME})

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  add_compile_options(-stdlib=libc++)
endif()
add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(source_files
  example.cpp
)
foreach(src IN LISTS source_files)
  get_filename_component(name_we ${src} NAME_WE)
  add_executable(test-${name_we} ${src})
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}>)
endforeach()
