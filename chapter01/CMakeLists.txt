
set( target chapter01 )

set( chapter01_SOURCES
    scenebasic.cpp
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
    #GLEW::GLEW
    glad::glad
    glfw
    glm::glm
    ${OPENGL_gl_LIBRARY}
)

include( ../cmake/CommonMacros.cmake )
SET_OUTPUT_NAMES( ${target} )

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

if( MSVC )
    file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE_INIT} )
    # set_property( TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" )
    # set_property( TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )
    # set_property( TARGET ${target} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )
    # set_property( TARGET ${target} PROPERTY VS_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )
endif()

# if( MSVC )
#     file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG} )
# else()
#     file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
# endif()
