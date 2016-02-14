#include "chunk.h"
#include "chunkgenerator.h"

#define MAKE_FACE(l,m,n,x1,y1,x2,y2,z,i,j,k,u1,v1,u2,v2) \
    d[l] = x1; d[m] = y1; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u1; d[7] = v1; \
    d += 8; \
    d[l] = x1; d[m] = y2; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u1; d[7] = v2; \
    d += 8; \
    d[l] = x2; d[m] = y2; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u2; d[7] = v2; \
    d += 8; \
    \
    d[l] = x2; d[m] = y2; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u2; d[7] = v2; \
    d += 8; \
    d[l] = x2; d[m] = y1; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u2; d[7] = v1; \
    d += 8; \
    d[l] = x1; d[m] = y1; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u1; d[7] = v1; \
    d += 8;

using namespace CS1972Engine::Voxel;

Chunk::Chunk(VoxelManager *parent, int x, int y, int z, ChunkGenerator *g)
    : m_parent(parent)
    , m_x(x)
    , m_y(y)
    , m_z(z)
{
    g->generate(m_blocks, x, y, z);

    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    m_vertices = 0;

    generateVao();
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Chunk::generateVao() {
    // Count number of faces that need to be rendered
    int nfaces = 0;
    for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                // Skip transparent blocks
                if (CHUNK_DEFINITION_AT(x,y,z).transparent())
                    continue;
                // Left
                if (x == 0 || CHUNK_DEFINITION_AT(x-1,y,z).transparent())
                    ++nfaces;
                // Right
                if (x == CHUNK_SIZE_X-1 || CHUNK_DEFINITION_AT(x+1,y,z).transparent())
                    ++nfaces;
                // Front
                if (z == 0 || CHUNK_DEFINITION_AT(x,y,z-1).transparent())
                    ++nfaces;
                // Back
                if (z == CHUNK_SIZE_Z-1 || CHUNK_DEFINITION_AT(x,y,z+1).transparent())
                    ++nfaces;
                // Down
                if (y == 0 || CHUNK_DEFINITION_AT(x,y-1,z).transparent())
                    ++nfaces;
                // Up
                if (y == CHUNK_SIZE_Y-1 || CHUNK_DEFINITION_AT(x,y+1,z).transparent())
                    ++nfaces;
            }
        }
    }

    // For each face, there are 6 vertex(3)-normal(3)-tex(2) coordinate sets, meaning 4*(3+3+2) floats per face
    m_vertices = 6*nfaces;
    std::cout << m_vertices << std::endl;
    GLfloat *data = new GLfloat[m_vertices*(3+3+2)];
    GLfloat *d = data;

    // Traverse array again but this time actually build the faces
    for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                GLfloat bx = m_x + x;
                GLfloat by = m_y + y;
                GLfloat bz = m_z + z;
                // Skip transparent blocks
                if (CHUNK_DEFINITION_AT(x,y,z).transparent())
                    continue;
                // Left
                if (x == 0 || CHUNK_DEFINITION_AT(x-1,y,z).transparent()) {
                    MAKE_FACE(1,2,0,by+1.f,bz+1.f,by,bz,bx,-1.f,0.f,0.f,0.f,0.f,1.f,1.f)
                }
                // Right
                if (x == CHUNK_SIZE_X-1 || CHUNK_DEFINITION_AT(x+1,y,z).transparent()) {
                    MAKE_FACE(1,2,0,by,bz+1.f,by+1.f,bz,bx+1.f,1.f,0.f,0.f,0.f,0.f,1.f,1.f)
                }
                // Front
                if (z == 0 || CHUNK_DEFINITION_AT(x,y,z-1).transparent()) {
                    MAKE_FACE(1,0,2,by+1.f,bx,by,bx+1.f,bz,0.f,0.f,-1.f,0.f,0.f,1.f,1.f)
                }
                // Back
                if (z == CHUNK_SIZE_Z-1 || CHUNK_DEFINITION_AT(x,y,z+1).transparent()) {
                    MAKE_FACE(1,0,2,by+1.f,bx+1.f,by,bx,bz+1.f,0.f,0.f,1.f,0.f,0.f,1.f,1.f)
                }
                // Down
                if (y == 0 || CHUNK_DEFINITION_AT(x,y-1,z).transparent()) {
                    MAKE_FACE(0,2,1,bx+1.f,bz,bx,bz+1.f,by,0.f,0.f,0.f,-1.f,0.f,1.f,1.f)
                }
                // Up
                if (y == CHUNK_SIZE_Y-1 || CHUNK_DEFINITION_AT(x,y+1,z).transparent()) {
                    MAKE_FACE(0,2,1,bx,bz,bx+1.f,bz+1.f,by+1.f,0.f,1.f,0.f,0.f,0.f,1.f,1.f)
                }
            }
        }
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, m_vertices*8*sizeof(GLfloat), data, GL_STATIC_DRAW);

    // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 0);
    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*3));
    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*6));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Chunk::tick(float seconds) { }

void Chunk::draw() {
    graphics().shaderColor(glm::vec3(1.f, 1.f, 1.f));
    graphics().shaderUseTexture(false);
    //graphics().shaderBindTexture(graphics().getTexture("tilemap"));
    glm::mat4 m = glm::mat4(1.f);
    graphics().shaderMTransform(m);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices);
    glBindVertexArray(0);
}
