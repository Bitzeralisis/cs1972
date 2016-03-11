#pragma once

#include "chunkdef.h"
#include "block.h"
#include "voxelmanager.h"
#include "../game.h"
#include "../../util/CommonIncludes.h"
#include "../../util/HeaderUtils.h"
#include <list>

namespace CS1972Engine {

class Graphics;

namespace Voxel {

class ChunkGenerator;

class Chunk {
public:
    Chunk(VoxelManager *parent, int x, int y, int z, ChunkGenerator *generator, bool proto = false);
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
    std::list<glm::vec3> m_special;

    float m_time = 0.f;

private:
    REFERENCE_ACCESSOR_DEEP(Graphics,graphics,parent()->parent()->parent()->g)

public:
    VALUE_ACCESSOR(VoxelManager*,parent)
    VALUE_ACCESSOR(int,x)
    VALUE_ACCESSOR(int,y)
    VALUE_ACCESSOR(int,z)
    inline csm::aabb aabb() const { return csm::aabb(glm::vec3((float)m_x,(float)m_y,(float)m_z), glm::vec3((float)CHUNK_SIZE_X,(float)CHUNK_SIZE_Y,(float)CHUNK_SIZE_Z)); }
    inline Block blockAt(int x, int y, int z) const { return m_blocks[CHUNK_BLOCK_AT(x,y,z)]; }
    inline Block blockAtGlobal(int x, int y, int z) const { return m_blocks[CHUNK_BLOCK_AT(x-m_x,y-m_y,z-m_z)]; }
    inline void setBlock(int x, int y, int z, Block b) { m_blocks[CHUNK_BLOCK_AT(x,y,z)] = b; }
    inline void setBlockGlobal(int x, int y, int z, Block b) { m_blocks[CHUNK_BLOCK_AT(x-m_x,y-m_y,z-m_z)] = b; }

    void generateBlocks(ChunkGenerator *generator);
    void generateVao();

    void tick(float seconds);
    void draw();
    glm::vec3 collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1, int dimension) const;
    bool rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const;
};

}
}
