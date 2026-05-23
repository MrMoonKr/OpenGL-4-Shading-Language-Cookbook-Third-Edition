include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET GLEW::GLEW)
    return()
endif()

dep_resolve_paths("glew" dep_source_dir dep_binary_dir)

set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLEW_CUSTOM_OUTPUT_DIRS ON CACHE BOOL "" FORCE)

FetchContent_Declare(glew
    URL "https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip"
    URL_HASH "SHA256=09e0083ae46930aba9b53e72c92ee1a557e24ed393526fec26cb0ebabd834720"
    SOURCE_DIR "${dep_source_dir}"
    BINARY_DIR "${dep_binary_dir}"
    SOURCE_SUBDIR build/cmake
)

FetchContent_MakeAvailable(glew)

if(TARGET glew_s)
    target_include_directories(glew_s
        PUBLIC
            $<BUILD_INTERFACE:${dep_source_dir}/include>
    )
endif()

if(TARGET glew)
    target_include_directories(glew
        PUBLIC
            $<BUILD_INTERFACE:${dep_source_dir}/include>
    )
endif()

if(TARGET glew_s AND NOT TARGET GLEW::glew_s)
    add_library(GLEW::glew_s ALIAS glew_s)
endif()

if(TARGET glew AND NOT TARGET GLEW::glew)
    add_library(GLEW::glew ALIAS glew)
endif()

if(TARGET glew_s AND NOT TARGET GLEW::GLEW)
    add_library(GLEW::GLEW ALIAS glew_s)
endif()

if(MSVC)
    if(TARGET glew_s)
        target_compile_options(glew_s PRIVATE /utf-8)
    endif()

    if(TARGET glew)
        target_compile_options(glew PRIVATE /utf-8)
    endif()
endif()
