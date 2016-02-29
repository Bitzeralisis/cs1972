#include "randomchunkgenerator.h"
#include "../util/CommonIncludes.h"

using namespace Minecraft;

void RandomChunkGenerator::generate(CS1972Engine::Voxel::Block (&chunk)[CHUNK_SIZE_BLOCKS], int, int, int) {
    for (int i = 0; i < CHUNK_SIZE_BLOCKS; ++i) {
        chunk[i].type = rand() % 3;
    }
}
