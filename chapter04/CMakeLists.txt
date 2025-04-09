set( target chapter04 )
set( chapter04_SOURCES
    scenemultilight.cpp scenemultilight.h
    sceneperfragment.cpp sceneperfragment.h
    main.cpp
    scenefog.cpp scenefog.h
    scenedirectional.cpp scenedirectional.h
    scenetoon.cpp scenetoon.h
    scenespot.cpp scenespot.h
    scenepbr.cpp scenepbr.h 
)

add_executable( ${target} 
    ${chapter04_SOURCES} 
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
