include_guard(GLOBAL)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps" CACHE PATH "FetchContent base directory" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED OFF CACHE BOOL "" FORCE)

function(dep_resolve_paths name out_source_dir out_binary_dir)
    set(dep_source_dir "${CMAKE_SOURCE_DIR}/deps/${name}-src")
    set(dep_binary_dir "${CMAKE_BINARY_DIR}/_deps/${name}-build")

    set(${out_source_dir} "${dep_source_dir}" PARENT_SCOPE)
    set(${out_binary_dir} "${dep_binary_dir}" PARENT_SCOPE)
endfunction()

function(dep_declare_fetchcontent name git_repository git_tag source_dir binary_dir)
    cmake_parse_arguments(DEP "" "SOURCE_SUBDIR" "" ${ARGN})

    set(fetchcontent_args
        GIT_REPOSITORY ${git_repository}
        GIT_TAG ${git_tag}
        SOURCE_DIR "${source_dir}"
        BINARY_DIR "${binary_dir}"
    )

    if(DEP_SOURCE_SUBDIR)
        list(APPEND fetchcontent_args SOURCE_SUBDIR "${DEP_SOURCE_SUBDIR}")
    endif()

    FetchContent_Declare(${name} ${fetchcontent_args})
endfunction()
