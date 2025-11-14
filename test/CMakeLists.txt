set(source_files
  example.cpp
  include_deps.cpp
  types_target.cpp
  types_json.cpp
  types_both.cpp
  types_positional.cpp
)
foreach(src IN LISTS source_files)
  cmake_path(GET src STEM name_we)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES CXX_EXTENSIONS OFF)
  target_link_libraries(test-${name_we} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
  target_compile_options(test-${name_we} PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(test-${name_we} PRIVATE -Wconversion -Wsign-conversion -Wshadow)
  target_compile_options(test-${name_we} PRIVATE -Werror)
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}> -h)
endforeach()

add_test(NAME types_positional-pos COMMAND test-types_positional 0 0 0 0 0 0.0 x 0,1)
