include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET glad_gl_core_46)
    return()
endif()

dep_resolve_paths("glad" dep_source_dir dep_binary_dir)
dep_declare_fetchcontent(
    glad
    "https://github.com/Dav1dde/glad.git"
    "v2.0.6"
    "${dep_source_dir}"
    "${dep_binary_dir}"
    SOURCE_SUBDIR cmake
)

FetchContent_GetProperties(glad)
if(NOT EXISTS "${dep_source_dir}/cmake/GladConfig.cmake")
    FetchContent_Populate(glad)
else()
    set(glad_SOURCE_DIR "${dep_source_dir}")
    set(glad_BINARY_DIR "${dep_binary_dir}")
endif()

if(NOT COMMAND glad_add_library)
    set(GLAD_SOURCES_DIR "${glad_SOURCE_DIR}")
    include("${glad_SOURCE_DIR}/cmake/GladConfig.cmake")
endif()

if(NOT TARGET glad_gl_core_46)
    glad_add_library(glad_gl_core_46 STATIC REPRODUCIBLE API gl:core=4.6)
endif()

if(NOT TARGET glad::glad)
    add_library(glad::glad ALIAS glad_gl_core_46)
endif()

if(MSVC)
    set_property(
        TARGET glad_gl_core_46
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
    target_compile_options(glad_gl_core_46 PRIVATE /utf-8)
endif()
