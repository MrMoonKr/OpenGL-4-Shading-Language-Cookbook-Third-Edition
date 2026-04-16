# Variables

set( target common )

set( common_src
        aabb.h
        cookbookogl.h
        cube.cpp cube.h
        # drawables.h
        glslprogram.cpp glslprogram.h
        glutils.cpp glutils.h
        grid.cpp grid.h 
        objmesh.cpp objmesh.h
        plane.cpp plane.h
        random.h
        scene.h
        scenerunner.h
        skybox.cpp skybox.h
        sphere.cpp sphere.h
        teapot.cpp teapot.h
        teapotdata.h
        teapotpatch.cpp teapotpatch.h
        stbimpl.cpp
        texture.cpp texture.h
        torus.cpp torus.h
        trianglemesh.cpp trianglemesh.h
        utils.h 
        #glad/src/glad.c 
        #stbimpl.cpp
)

# Target

add_library( ${target} STATIC 
    ${common_src}
)

target_include_directories( ${target}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries( ${target} PUBLIC
    glad::glad 
    glm::glm
    glfw
    imgui::imgui
    imgui::backend_glfw
    imgui::backend_opengl3
)

target_link_libraries( ${target} PRIVATE
    stb::stb
)

if( UNIX AND NOT APPLE )
    target_link_libraries( ${target} PUBLIC ${CMAKE_DL_LIBS} )
endif()

if( MSVC )
    target_compile_options( ${target} PRIVATE /utf-8 )
endif()
