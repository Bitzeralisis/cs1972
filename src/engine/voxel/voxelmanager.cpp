#include "voxelmanager.h"
#include "chunk.h"

using namespace CS1972Engine::Voxel;

VoxelManager::VoxelManager(const BlockType *blockDefs, ChunkGenerator *generator)
    : m_blockDefs(blockDefs)
    , m_generator(generator)
{
    for (int x = -5; x <= 4; ++x)
        for (int y = -5; y <= 4; ++y) {
            m_chunks.push_back(new Chunk(this, x*CHUNK_SIZE_X, 0, y*CHUNK_SIZE_Z, generator));
            m_chunks.push_back(new Chunk(this, x*CHUNK_SIZE_X, CHUNK_SIZE_Y, y*CHUNK_SIZE_Z, generator));
        }
}

VoxelManager::~VoxelManager() { }

void VoxelManager::tick(float seconds) {
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        (*it)->tick(seconds);
    }
}

void VoxelManager::draw() {
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        if (!graphics().camera->frustumCullAABB((*it)->aabb())) {
            (*it)->draw();
        }
    }
}

glm::vec3 VoxelManager::collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const {
    glm::vec3 p0 = pos0;
    glm::vec3 p1 = pos1;
    for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        p1 = (*it)->collideAABB(aabb, p0, p1);
    }
    return p1;
}
