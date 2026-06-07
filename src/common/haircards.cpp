#include "haircards.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

HairCards::HairCards( float width, float height, int cardCount )
{
    std::vector<GLfloat> positions;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texcoords;
    std::vector<GLfloat> tangents;
    std::vector<GLuint> indices;

    positions.reserve( cardCount * 12 );
    normals.reserve( cardCount * 12 );
    texcoords.reserve( cardCount * 8 );
    tangents.reserve( cardCount * 16 );
    indices.reserve( cardCount * 6 );

    const glm::vec3 cardVertices[ 4 ] = {
        glm::vec3( -width * 0.5f, 0.0f, 0.0f ),
        glm::vec3( width * 0.5f, 0.0f, 0.0f ),
        glm::vec3( width * 0.5f, height, 0.0f ),
        glm::vec3( -width * 0.5f, height, 0.0f ),
    };
    const glm::vec2 cardUv[ 4 ] = {
        glm::vec2( 0.0f, 0.0f ),
        glm::vec2( 1.0f, 0.0f ),
        glm::vec2( 1.0f, 1.0f ),
        glm::vec2( 0.0f, 1.0f ),
    };

    for ( int cardIndex = 0; cardIndex < cardCount; ++cardIndex )
    {
        const float angle = glm::two_pi<float>() * static_cast<float>( cardIndex ) / static_cast<float>( cardCount );
        const glm::mat4 rotation = glm::rotate( glm::mat4( 1.0f ), angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
        const glm::vec3 normal = glm::normalize( glm::vec3( rotation * glm::vec4( 0.0f, 0.0f, 1.0f, 0.0f ) ) );
        const glm::vec3 strandDirection = glm::normalize( glm::vec3( rotation * glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f ) ) );
        const GLuint vertexOffset = static_cast<GLuint>( positions.size() / 3 );

        for ( int vertexIndex = 0; vertexIndex < 4; ++vertexIndex )
        {
            const glm::vec3 rotatedVertex = glm::vec3( rotation * glm::vec4( cardVertices[ vertexIndex ], 1.0f ) );
            positions.push_back( rotatedVertex.x );
            positions.push_back( rotatedVertex.y );
            positions.push_back( rotatedVertex.z );

            normals.push_back( normal.x );
            normals.push_back( normal.y );
            normals.push_back( normal.z );

            texcoords.push_back( cardUv[ vertexIndex ].x );
            texcoords.push_back( cardUv[ vertexIndex ].y );

            tangents.push_back( strandDirection.x );
            tangents.push_back( strandDirection.y );
            tangents.push_back( strandDirection.z );
            tangents.push_back( 1.0f );
        }

        indices.push_back( vertexOffset + 0 );
        indices.push_back( vertexOffset + 1 );
        indices.push_back( vertexOffset + 2 );
        indices.push_back( vertexOffset + 0 );
        indices.push_back( vertexOffset + 2 );
        indices.push_back( vertexOffset + 3 );
    }

    initBuffers( &indices, &positions, &normals, &texcoords, &tangents );
}
