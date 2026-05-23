include_guard(GLOBAL)

include(cmake/Dependencies.cmake)

if(TARGET imgui)
    return()
endif()

dep_resolve_paths("imgui" dep_source_dir dep_binary_dir)
dep_declare_fetchcontent(imgui "https://github.com/ocornut/imgui.git" "2dc64f99bcb7c16f5a314add49ec5db8905fa940" "${dep_source_dir}" "${dep_binary_dir}")

FetchContent_GetProperties(imgui)
if(NOT EXISTS "${dep_source_dir}/imgui.h")
    FetchContent_Populate(imgui)
else()
    set(imgui_SOURCE_DIR "${dep_source_dir}")
    set(imgui_BINARY_DIR "${dep_binary_dir}")
endif()

if(TARGET imgui)
    return()
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
add_library(imgui::imgui ALIAS imgui)
target_include_directories(imgui
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${imgui_SOURCE_DIR}/misc/freetype
)

add_library(imgui_backend_opengl3 STATIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui::backend_opengl3 ALIAS imgui_backend_opengl3)
target_link_libraries(imgui_backend_opengl3
    PUBLIC
        imgui::imgui
        glad::glad
)

add_library(imgui_backend_win32 STATIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
)
add_library(imgui::backend_win32 ALIAS imgui_backend_win32)
target_link_libraries(imgui_backend_win32
    PUBLIC
        imgui::imgui
)

add_library(imgui_backend_dx11 STATIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)
add_library(imgui::backend_dx11 ALIAS imgui_backend_dx11)
target_link_libraries(imgui_backend_dx11
    PUBLIC
        imgui::imgui
        d3d11
)

add_library(imgui_backend_glfw STATIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
)
add_library(imgui::backend_glfw ALIAS imgui_backend_glfw)
target_link_libraries(imgui_backend_glfw
    PUBLIC
        imgui::imgui
        glfw
)

if(MSVC)
    foreach(target
        imgui
        imgui_backend_opengl3
        imgui_backend_win32
        imgui_backend_dx11
        imgui_backend_glfw
    )
        target_compile_options(${target} PRIVATE /utf-8)
    endforeach()
endif()
