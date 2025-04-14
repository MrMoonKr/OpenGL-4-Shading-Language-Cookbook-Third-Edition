
set( target chapter00 )

set( chapter00_SOURCES
    main.cpp
)

add_executable( ${target} 
    ${chapter00_SOURCES} 
)

target_link_libraries( ${target} PRIVATE
    glad::glad
    glfw
)

