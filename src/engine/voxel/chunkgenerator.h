#pragma once

#include "chunkdef.h"
#include "block.h"

namespace CS1972Engine {
namespace Voxel {

class ChunkGenerator {
public:
    virtual ~ChunkGenerator() { }
public:
    virtual void generate(Block (&chunk)[CHUNK_SIZE_BLOCKS], int x, int y, int z) = 0;
};

}
}
