include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET glm::glm)
    return()
endif()

dep_resolve_paths("glm" dep_source_dir dep_binary_dir)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)

dep_declare_fetchcontent(
    glm
    "https://github.com/g-truc/glm.git"
    "1.0.1"
    "${dep_source_dir}"
    "${dep_binary_dir}"
)

FetchContent_MakeAvailable(glm)

if(TARGET glm AND NOT TARGET glm::glm)
    add_library(glm::glm ALIAS glm)
endif()
