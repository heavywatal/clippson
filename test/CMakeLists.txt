link_libraries(${PROJECT_NAME}::${PROJECT_NAME})
add_compile_options(-Wall -Wextra -pedantic)

set(source_files
  example.cpp
  types_target.cpp
  types_json.cpp
  types_both.cpp
  types_positional.cpp
)
foreach(src IN LISTS source_files)
  get_filename_component(name_we ${src} NAME_WE)
  add_executable(test-${name_we} ${src})
  set_target_properties(test-${name_we} PROPERTIES
    CXX_EXTENSIONS OFF
  )
  add_test(NAME ${name_we} COMMAND $<TARGET_FILE:test-${name_we}> -h)
endforeach()

add_test(NAME types_positional-pos COMMAND test-types_positional 0 0 0 0 0 0.0 x 0,1)
