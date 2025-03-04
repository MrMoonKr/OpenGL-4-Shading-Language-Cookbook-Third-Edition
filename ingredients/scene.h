#pragma once

#include <glm/glm.hpp>

/**
 * @brief 씬을 나타내는 추상 클래스. 
 *
 * 상태패턴을 이용하여 SceneRunner에 의해 여러 종류의 씬을 실행할 수 있도록 한다.
 */
class Scene
{
protected:
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    bool m_animate;

public:
    int width;
    int height;

public:
    Scene() : m_animate( true ), width( 800 ), height( 600 )
    {
    }
    virtual ~Scene()
    {
    }

    void setDimensions( int w, int h )
    {
        width = w;
        height = h;
    }

    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene() = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t ) = 0;

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize( int, int ) = 0;

    void animate( bool value )
    {
        m_animate = value;
    }
    bool animating()
    {
        return m_animate;
    }


};
