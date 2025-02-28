set( target chapter01 )

set( chapter01_SOURCES
	scenebasic.cpp
	main.cpp
)

add_executable( ${target} ${chapter01_SOURCES} )

target_compile_definitions(${target}
		PRIVATE
		${DEFAULT_COMPILE_DEFINITIONS}
		GLFW_INCLUDE_NONE
		)

target_link_libraries( ${target}
        PRIVATE
        ingredients
		#GLEW::GLEW
		glad::glad
		glfw
		glm::glm
        ${OPENGL_gl_LIBRARY}
		)
if( MSVC )
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG})
else()
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
