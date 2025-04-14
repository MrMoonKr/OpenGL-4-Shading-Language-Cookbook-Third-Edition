set( target chapter02 )

set( chapter02_SOURCES 
    main.cpp 
    sceneseparable.cpp
    scenebasic_uniformblock.cpp
    scenebasic_attrib.cpp
    scenebasic_uniform.cpp 
)

add_executable( ${target} 
    ${chapter02_SOURCES} 
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
