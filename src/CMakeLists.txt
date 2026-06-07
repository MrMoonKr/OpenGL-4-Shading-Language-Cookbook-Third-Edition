function( configure_demo_runtime_assets target )
    if( EXISTS "${CMAKE_SOURCE_DIR}/media" )
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/media $<TARGET_FILE_DIR:${target}>/media
            COMMENT "Copying media folder to runtime output"
        )
    endif()

    if( EXISTS "${CMAKE_SOURCE_DIR}/fonts" )
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:${target}>/fonts
            COMMENT "Copying fonts folder to runtime output"
        )
    endif()
endfunction()

add_subdirectory(common)
add_subdirectory(ch00)
add_subdirectory(ch01)
#add_subdirectory(ch02)
#add_subdirectory(ch03)
#add_subdirectory(ch04)
#add_subdirectory(ch05)
add_subdirectory(ch12)
add_subdirectory(ch13)
add_subdirectory(ch14)
add_subdirectory(ch15)
