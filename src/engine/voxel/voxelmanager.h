#pragma once

#include "../terrainmanager.h"
#include "chunkgenerator.h"

namespace CS1972Engine {
namespace Voxel {

class Chunk;

class VoxelManager : public TerrainManager {
public:
    explicit VoxelManager(ChunkGenerator *generator);
    virtual ~VoxelManager();

private:
    ChunkGenerator *m_generator;
    std::list<Chunk *> m_chunks;

public:
    virtual void tick(float seconds) override;
    virtual void draw() override;
};

}
}
