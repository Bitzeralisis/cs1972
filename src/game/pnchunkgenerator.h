#pragma once

#include "../engine/voxel/chunkgenerator.h"
#include "../util/CommonIncludes.h"
#include <unordered_map>

namespace Minecraft {

class PNChunkGenerator : public CS1972Engine::Voxel::ChunkGenerator {
public:
    explicit PNChunkGenerator(int seed);
    virtual ~PNChunkGenerator() { }

private:
    int m_seed;

    glm::vec2 m_chaosPositions[4];
    bool m_chaosBlocks[4] = { true, true, true, true };

    std::unordered_map<int,float> m_noise2Dcache;
    std::unordered_map<int,float> m_smoothedNoise2Dcache;

public:
    int whichChaosBlock(glm::vec2 pos) const { for (int i = 0; i < 4; ++i) if (m_chaosPositions[i] == pos) return i; return -1; }
    inline glm::vec2 getChaosBlock(int i) const { return m_chaosPositions[i]; }
    inline bool hasChaosBlock(int i) const { return m_chaosBlocks[i]; }
    inline void setChaosBlock(int i, bool b) { m_chaosBlocks[i] = b; }
    virtual void generate(CS1972Engine::Voxel::Block (&chunk)[CHUNK_SIZE_BLOCKS], int x, int y, int z);

private:
    float perlinNoise2D(float x, float y, float p, int n, float s);
    float interpolatedNoise2D(float x, float y, float s);
    float smoothedNoise2D(int x, int y, float s);
    float noise2D(int x, int y);
    float interpolateCosine(float a, float b, float x);
};

}
