#pragma once

#include "../engine/voxel/chunkgenerator.h"

namespace Minecraft {

class PNChunkGenerator : public CS1972Engine::Voxel::ChunkGenerator {
public:
    explicit PNChunkGenerator(int seed);
    virtual ~PNChunkGenerator() { }

private:
    int m_seed;

public:
    virtual void generate(CS1972Engine::Voxel::Block (&chunk)[CHUNK_SIZE_BLOCKS], int x, int y, int z);

private:
    float perlinNoise2D(float x, float y, float p, int n);
    float interpolatedNoise2D(float x, float y);
    float smoothedNoise2D(int x, int y);
    float noise2D(int x, int y);
    float interpolateCosine(float a, float b, float x);
};

}
