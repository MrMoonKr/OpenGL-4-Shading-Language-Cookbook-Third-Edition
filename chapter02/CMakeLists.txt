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

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
if( MSVC )
    file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE_INIT} )
endif()
