include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET stb::stb)
    return()
endif()

dep_resolve_paths("stb" dep_source_dir dep_binary_dir)
dep_declare_fetchcontent(
    stb
    "https://github.com/nothings/stb.git"
    "master"
    "${dep_source_dir}"
    "${dep_binary_dir}"
)

FetchContent_GetProperties(stb)
if(NOT EXISTS "${dep_source_dir}/stb_image.h")
    FetchContent_Populate(stb)
else()
    set(stb_SOURCE_DIR "${dep_source_dir}")
    set(stb_BINARY_DIR "${dep_binary_dir}")
endif()

if(NOT TARGET stb)
    add_library(stb INTERFACE)
    target_include_directories(stb
        INTERFACE
            "${stb_SOURCE_DIR}"
    )
endif()

if(NOT TARGET stb::stb)
    add_library(stb::stb ALIAS stb)
endif()
