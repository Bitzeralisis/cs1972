#include "voxelmanager.h"
#include "chunk.h"

using namespace CS1972Engine::Voxel;

VoxelManager::VoxelManager(ChunkGenerator *generator)
    : m_generator(generator)
{
    m_chunks.push_back(new Chunk(this, 0, 0, 0, generator));
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
