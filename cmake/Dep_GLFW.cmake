include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET glfw)
    return()
endif()

dep_resolve_paths("glfw" dep_source_dir dep_binary_dir)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

dep_declare_fetchcontent(glfw "https://github.com/glfw/glfw.git" "3.4" "${dep_source_dir}" "${dep_binary_dir}")
FetchContent_MakeAvailable(glfw)

if(MSVC)
    target_compile_options(glfw PRIVATE /utf-8)
endif()
