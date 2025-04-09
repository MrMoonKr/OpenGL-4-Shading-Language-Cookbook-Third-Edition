set( target chapter03 )

set( chapter03_SOURCES
    main.cpp
    scenesubroutine.cpp
    scenediffuse.cpp
    scenetwoside.cpp
    sceneflat.cpp
    scenediscard.cpp
    scenephong.cpp
)

add_executable( ${target} 
    ${chapter03_SOURCES} 
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