set( target ch13 )

set( ch13_sources
    main.cpp
    chapter13_app.cpp
    chapter13_app.h
)

add_executable( ${target}
    ${ch13_sources}
)

target_compile_definitions( ${target} PRIVATE
    GLFW_INCLUDE_NONE
)

target_link_libraries( ${target} PRIVATE
    common
)

if( MSVC )
    target_compile_options( ${target} PRIVATE /utf-8 )
endif()

add_custom_command(
    TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shader $<TARGET_FILE_DIR:${target}>/shader
    COMMENT "Copying chapter 13 shaders"
)

configure_demo_runtime_assets( ${target} )
