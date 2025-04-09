#include "trianglemesh.h"


/**
 * 메시데이터로 부터 OpenGL 버퍼 생성 및 초기화.  
 * 정점속성 => 0:위치, 1:법선, 2:텍스쳐좌표, 3:탄젠트
 */
void TriangleMesh::initBuffers( std::vector<GLuint>* indices, std::vector<GLfloat>* points,
                                std::vector<GLfloat>* normals, std::vector<GLfloat>* texCoords,
                                std::vector<GLfloat>* tangents )
{

    if ( !buffers.empty() )
    {
        deleteBuffers();
    }

    // Must have data for indices, points, and normals
    if ( indices == nullptr || points == nullptr || normals == nullptr )
    {
        return;
    }

    nVerts = ( GLuint )indices->size();

    GLuint indexBuf = 0, posBuf = 0, normBuf = 0, tcBuf = 0, tangentBuf = 0;

    // 인덱스 버퍼
    glGenBuffers( 1, &indexBuf );
    buffers.push_back( indexBuf );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuf );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof( GLuint ), indices->data(), GL_STATIC_DRAW );

    // 위치 버퍼
    glGenBuffers( 1, &posBuf );
    buffers.push_back( posBuf );
    glBindBuffer( GL_ARRAY_BUFFER, posBuf );
    glBufferData( GL_ARRAY_BUFFER, points->size() * sizeof( GLfloat ), points->data(), GL_STATIC_DRAW );
    // 법선 버퍼
    glGenBuffers( 1, &normBuf );
    buffers.push_back( normBuf );
    glBindBuffer( GL_ARRAY_BUFFER, normBuf );
    glBufferData( GL_ARRAY_BUFFER, normals->size() * sizeof( GLfloat ), normals->data(), GL_STATIC_DRAW );
    // 텍스쳐좌표 버퍼
    if ( texCoords != nullptr )
    {
        glGenBuffers( 1, &tcBuf );
        buffers.push_back( tcBuf );
        glBindBuffer( GL_ARRAY_BUFFER, tcBuf );
        glBufferData( GL_ARRAY_BUFFER, texCoords->size() * sizeof( GLfloat ), texCoords->data(), GL_STATIC_DRAW );
    }
    // 탄젠트 버퍼
    if ( tangents != nullptr )
    {
        glGenBuffers( 1, &tangentBuf );
        buffers.push_back( tangentBuf );
        glBindBuffer( GL_ARRAY_BUFFER, tangentBuf );
        glBufferData( GL_ARRAY_BUFFER, tangents->size() * sizeof( GLfloat ), tangents->data(), GL_STATIC_DRAW );
    }

    // VAO ( Vertex Array Object )
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuf );

    glBindBuffer( GL_ARRAY_BUFFER, posBuf );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 0 ); // Vertex position

    glBindBuffer( GL_ARRAY_BUFFER, normBuf );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( 1 ); // Normal

    if ( texCoords != nullptr )
    {
        glBindBuffer( GL_ARRAY_BUFFER, tcBuf );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, 0 );
        glEnableVertexAttribArray( 2 ); // Tex coord
    }

    if ( tangents != nullptr )
    {
        glBindBuffer( GL_ARRAY_BUFFER, tangentBuf );
        glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, 0, 0 );
        glEnableVertexAttribArray( 3 ); // Tangents
    }

    glBindVertexArray( 0 );
}

/**
 * 'GL_TRIANGLES'로 렌더링.
 */
void TriangleMesh::render() const
{
    if ( vao == 0 ) // 유효하지 않음. 렌더링 안함.
    {
        return;
    }

    glBindVertexArray( vao );
    glDrawElements( GL_TRIANGLES, nVerts, GL_UNSIGNED_INT, 0 );
    glBindVertexArray( 0 );
}

TriangleMesh::~TriangleMesh()
{
    deleteBuffers();
}

void TriangleMesh::deleteBuffers()
{
    if ( buffers.size() > 0 )
    {
        glDeleteBuffers( ( GLsizei )buffers.size(), buffers.data() );
        buffers.clear();
    }

    if ( vao != 0 )
    {
        glDeleteVertexArrays( 1, &vao );
        vao = 0;
    }
}
