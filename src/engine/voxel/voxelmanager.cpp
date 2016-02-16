#include "voxelmanager.h"
#include "chunk.h"

using namespace CS1972Engine::Voxel;

VoxelManager::VoxelManager(const BlockType *blockDefs, ChunkGenerator *generator)
    : m_blockDefs(blockDefs)
    , m_generator(generator)
{
    for (int x = -1; x <= 0; ++x)
        for (int y = -1; y <= 0; ++y) {
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
        (*it)->draw();
    }
}
