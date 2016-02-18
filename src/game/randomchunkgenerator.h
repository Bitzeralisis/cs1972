#pragma once

#include "../engine/voxel/chunkgenerator.h"

namespace Minecraft {

class RandomChunkGenerator : public CS1972Engine::Voxel::ChunkGenerator {
public:
    RandomChunkGenerator() { }
    virtual ~RandomChunkGenerator() { }

public:
    virtual void generate(CS1972Engine::Voxel::Block (&chunk)[CHUNK_SIZE_BLOCKS], int x, int y, int z);
};

}
