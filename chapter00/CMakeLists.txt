
set( target chapter00 )

set( chapter00_SOURCES
    main.cpp
)

add_executable( ${target} 
    ${chapter00_SOURCES} 
)

target_link_libraries( ${target} PRIVATE
    GLEW::GLEW
    glfw
)

include( ../cmake/CommonMacros.cmake )
SET_OUTPUT_NAMES( ${target} )


