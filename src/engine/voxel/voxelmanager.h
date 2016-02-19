#pragma once

#include "../terrainmanager.h"
#include "chunkgenerator.h"

namespace CS1972Engine {
namespace Voxel {

class Chunk;

class VoxelManager : public TerrainManager {
public:
    explicit VoxelManager(const BlockType *blockDefs, ChunkGenerator *generator);
    virtual ~VoxelManager();

private:
    const BlockType *const m_blockDefs;
    ChunkGenerator *m_generator;
    std::list<Chunk *> m_chunks;

public:
    const VALUE_ACCESSOR(BlockType*,blockDefs)

    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec3 collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const override;
};

}
}
