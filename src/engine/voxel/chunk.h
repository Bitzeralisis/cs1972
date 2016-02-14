#pragma once

#include "chunkdef.h"
#include "block.h"
#include "voxelmanager.h"
#include "../game.h"
#include "../../util/CommonIncludes.h"
#include "../../util/HeaderUtils.h"

#define CHUNK_BLOCK_AT(x,y,z) ((CHUNK_SIZE_X*CHUNK_SIZE_Z*(y))+(CHUNK_SIZE_X*(z))+(x))
#define CHUNK_DEFINITION_AT(x,y,z) (BLOCK_DEFINITIONS[m_blocks[CHUNK_BLOCK_AT(x,y,z)].type])

namespace CS1972Engine {

class Graphics;

namespace Voxel {

class ChunkGenerator;

class Chunk {
public:
    Chunk(VoxelManager *parent, int x, int y, int z, ChunkGenerator *generator);
    virtual ~Chunk();

private:
    VoxelManager *const m_parent;

    const int m_x;
    const int m_y;
    const int m_z;
    Block m_blocks[CHUNK_SIZE_BLOCKS];

    GLuint m_vao;
    GLuint m_vbo;
    GLsizei m_vertices;

private:
    REFERENCE_ACCESSOR_DEEP(Graphics,graphics,parent()->parent()->parent()->g)

    void generateVao();

public:
    VALUE_ACCESSOR(VoxelManager*,parent)
    VALUE_ACCESSOR(int,x)
    VALUE_ACCESSOR(int,y)
    VALUE_ACCESSOR(int,z)
    Block blockAt(int x, int y, int z) const { return m_blocks[CHUNK_BLOCK_AT(x,y,z)]; }
    Block blockAtGlobal(int x, int y, int z) const { return m_blocks[CHUNK_BLOCK_AT(x-m_x,y-m_y,z-m_z)]; }

    virtual void tick(float seconds);
    virtual void draw();
};

}
}
