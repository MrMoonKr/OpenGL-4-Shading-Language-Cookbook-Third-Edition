# Variables

set( target ingredients )

set( ingredients_SOURCES
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
        texture.cpp texture.h
        torus.cpp torus.h
        trianglemesh.cpp trianglemesh.h
        utils.h 
        #glad/src/glad.c 
        #stbimpl.cpp
)

# Target

add_library( ${target} STATIC 
    ${ingredients_SOURCES}
)

target_include_directories( ${target} PUBLIC 
    glad/include
)

target_link_libraries( ${target} PUBLIC 
    glad::glad glm::glm
)

if( UNIX AND NOT APPLE )
    target_link_libraries( ${target} PUBLIC ${CMAKE_DL_LIBS} )
endif()

