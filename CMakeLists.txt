cmake_minimum_required( VERSION 3.24 )

# 사용 제거 중
# 의존 패키지 관리자 ( https://vcpkg.io/en/ )

# set( VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "" )
# set( CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" )
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
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/Debug" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release" )

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/libs" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/libs/Debug" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/libs/Release" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/libs" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/libs/Debug" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/libs/Release" )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )

if( MSVC )
    add_definitions( /DNOMINMAX )
endif()

if( UNIX )
    message( STATUS "Running Linux compiling. Options [GLVND] for modern OpenGL or [LEGACY] for historical OpenGl.")
    message( STATUS "Setting GLVND option.")
    set( OpenGL_GL_PREFERENCE GLVND )
endif()


# find_package( glad CONFIG REQUIRED )
# target_link_libraries( main PRIVATE glad::glad )
# find_package( glfw3 CONFIG REQUIRED )
# target_link_libraries( main PRIVATE glfw )
# find_package( glm REQUIRED )
# target_link_libraries( main PRIVATE glm::glm )
# target_link_libraries( main PRIVATE glm::glm-header-only )
# find_package( Stb REQUIRED )
# target_include_directories( main PRIVATE ${Stb_INCLUDE_DIR} )
# find_package( imgui CONFIG REQUIRED )
# target_link_libraries( main PRIVATE imgui::imgui )

include( cmake/Dep_GLFW.cmake )
include( cmake/Dep_GLAD.cmake )
include( cmake/Dep_GLEW.cmake )
include( cmake/Dep_GLM.cmake )
include( cmake/Dep_IMGUI.cmake )
include( cmake/Dep_STB.cmake )


# 하위 프로젝트

# include_directories( ingredients )

# add_subdirectory( ingredients )
# add_subdirectory( chapter00 )
# add_subdirectory( chapter01 )
# add_subdirectory( chapter02 )
# add_subdirectory( chapter03 )
# add_subdirectory( chapter04 )
# add_subdirectory( chapter05 )
# add_subdirectory( chapter06 )
# add_subdirectory( chapter07 )
# add_subdirectory( chapter08 )
# add_subdirectory( chapter09 )
# add_subdirectory( chapter10 )
# add_subdirectory( chapter11 )

add_subdirectory( src )


# media 폴더를 실행 폴더( Debug, Release )로 복사 샘플 코드
# add_custom_command(
#     TARGET ${target} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_CURRENT_SOURCE_DIR}/media $<TARGET_FILE_DIR:${target}>/media
#     COMMENT "Copying 'media' folder to build output directory"
# )
