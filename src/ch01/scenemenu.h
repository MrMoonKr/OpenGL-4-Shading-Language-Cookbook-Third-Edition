#pragma once

#include "scene.h"

#include "cookbookogl.h"
#include <string>

class SceneMenu : public Scene
{
private:
    GLuint vaoHandle;
    GLuint programHandle;

public:
    SceneMenu();

    void initScene();
    void update( float t );
    void render();
    void resize( int, int );

private:
    void linkMe( GLint vertShader, GLint fragShader );
    void compileShaderProgram();
    void writeShaderBinary();
    void loadShaderBinary( GLint );
    void loadSpirvShader();

    std::string getShaderInfoLog( GLuint shader );
    std::string getProgramInfoLog( GLuint program );

};
