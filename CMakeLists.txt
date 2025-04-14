cmake_minimum_required( VERSION 3.30 )


# 의존 패키지 관리자 ( https://vcpkg.io/en/ )

set( VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "" )
set( CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" )
# set( VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "" )
# set( CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" )


# 프로젝트

project( GLSLCOOKBOOK 
    VERSION 1.0
    DESCRIPTION "OpenGL 4 Shading Language Cookbook 3rd Edition"
    LANGUAGES CXX
)


# 전역 설정

set( CMAKE_CXX_STANDARD 17 )
set( CXX_STANDARD_REQUIRED ON )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

if( MSVC )
    add_definitions( /DNOMINMAX )
endif()

if( UNIX )
    message( STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message( STATUS "Setting GLVND option.")
    set( OpenGL_GL_PREFERENCE GLVND )
endif()


# 의존 패키지

find_package( glad CONFIG REQUIRED )
# target_link_libraries( main PRIVATE glad::glad )
find_package( glfw3 CONFIG REQUIRED )
# target_link_libraries( main PRIVATE glfw )
find_package( glm REQUIRED )
# target_link_libraries( main PRIVATE glm::glm )
# target_link_libraries( main PRIVATE glm::glm-header-only )
find_package( Stb REQUIRED )
# target_include_directories( main PRIVATE ${Stb_INCLUDE_DIR} )
find_package( imgui CONFIG REQUIRED )
# target_link_libraries( main PRIVATE imgui::imgui )


# 하위 프로젝트

include_directories( ingredients )

add_subdirectory( ingredients )
add_subdirectory( chapter00 )
add_subdirectory( chapter01 )
add_subdirectory( chapter02 )
add_subdirectory( chapter03 )
add_subdirectory( chapter04 )
# add_subdirectory( chapter05 )
# add_subdirectory( chapter06 )
# add_subdirectory( chapter07 )
# add_subdirectory( chapter08 )
# add_subdirectory( chapter09 )
# add_subdirectory( chapter10 )
# add_subdirectory( chapter11 )


# media 폴더를 실행 폴더( Debug, Release )로 복사 샘플 코드
# add_custom_command(
#     TARGET ${target} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_CURRENT_SOURCE_DIR}/media $<TARGET_FILE_DIR:${target}>/media
#     COMMENT "Copying 'media' folder to build output directory"
# )
