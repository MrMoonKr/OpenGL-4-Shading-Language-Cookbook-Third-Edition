
set( target chapter01 )

set( chapter01_SOURCES
    scenebasic.cpp scenebasic.h
    scenemenu.cpp scenemenu.h
    main.cpp
)

add_executable( ${target} 
    ${chapter01_SOURCES} 
)

target_compile_definitions( ${target} PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    GLFW_INCLUDE_NONE
)

target_link_libraries( ${target} PRIVATE
    ingredients
    glad::glad
    glfw
    glm::glm
)

# shader 폴더를 실행 폴더( Debug, Release )로 복사
add_custom_command(
    TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shader $<TARGET_FILE_DIR:${target}>/shader
    COMMENT "Copying data folder to build output directory"
)

add_custom_command(
    TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:${target}>/fonts
    COMMENT "Copying fonts folder to build output directory"
)