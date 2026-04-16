
set( TARGET1 ch00_glad )
set( TARGET2 ch00_glew )
set( TARGET3 ch00_app )

set( ch00_glad_SOURCES
    main_glad.cpp
)
set( ch00_glew_SOURCES
    main_glew.cpp
)
set( ch00_app_SOURCES
    main_app.cpp
    app.cpp
    app.h
)

add_executable( ${TARGET1} 
    ${ch00_glad_SOURCES} 
)
add_executable( ${TARGET2} 
    ${ch00_glew_SOURCES} 
)
add_executable( ${TARGET3} 
    ${ch00_app_SOURCES} 
)

target_compile_definitions( ${TARGET1} PRIVATE
    GLFW_INCLUDE_NONE
)
target_compile_definitions( ${TARGET2} PRIVATE
    GLFW_INCLUDE_NONE
)
target_compile_definitions( ${TARGET3} PRIVATE
    GLFW_INCLUDE_NONE
)

target_link_libraries( ${TARGET1} PRIVATE
    glad::glad
    glfw
)
target_link_libraries( ${TARGET2} PRIVATE
    GLEW::GLEW
    glfw
)
target_link_libraries( ${TARGET3} PRIVATE
    GLEW::GLEW
    glfw
)

if( MSVC )
    target_compile_options( ${TARGET1} PRIVATE /utf-8 )
    target_compile_options( ${TARGET2} PRIVATE /utf-8 )
    target_compile_options( ${TARGET3} PRIVATE /utf-8 )
endif()
