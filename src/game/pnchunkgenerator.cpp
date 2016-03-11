#include "pnchunkgenerator.h"

using namespace Minecraft;

PNChunkGenerator::PNChunkGenerator(int seed)
    : m_seed(seed)
{
    m_chaosPositions[0] = glm::vec2(glm::floor(20.f*rand()/RAND_MAX)+100.f, glm::floor(80.f*rand()/RAND_MAX)-40.f);
    m_chaosPositions[1] = glm::vec2(glm::floor(400.f*rand()/RAND_MAX)-200.f, glm::floor(100.f*rand()/RAND_MAX)+600.f);
    m_chaosPositions[2] = glm::vec2(glm::floor(200.f*rand()/RAND_MAX)+400.f, glm::floor(200.f*rand()/RAND_MAX));
    m_chaosPositions[3] = glm::vec2(glm::floor(400.f*rand()/RAND_MAX)-200.f, glm::floor(100.f*rand()/RAND_MAX)-600.f);
    /*for (int i = 0; i < 4; ++i) {
        m_chaosPositions[i] = glm::vec2(5.f, i+32.f);
    }*/
}

void PNChunkGenerator::generate(CS1972Engine::Voxel::Block (&chunk)[CHUNK_SIZE_BLOCKS], int x, int y, int z) {
    float baseTemp = z/300.f; // Approximate

    float *heightMap = new float[CHUNK_SIZE_X*CHUNK_SIZE_Z];
    float *heightMap2 = new float[CHUNK_SIZE_X*CHUNK_SIZE_Z];
    float *roughness = new float[CHUNK_SIZE_X*CHUNK_SIZE_Z];
    float *tempMap = new float[CHUNK_SIZE_X*CHUNK_SIZE_Z];

    // heightMap - base height, used for large flat areas
    m_noise2Dcache.clear();
    m_smoothedNoise2Dcache.clear();
    for (int zrel = 0; zrel < CHUNK_SIZE_Z; ++zrel) {
        for (int xrel = 0; xrel < CHUNK_SIZE_X; ++xrel) {
            heightMap[xrel+CHUNK_SIZE_X*zrel] = perlinNoise2D((x+xrel)/72.f, (z+zrel)/72.f, 0.25f, 2, 1.f) / 1.25f * 19.f + 1.f;
        }
    }

    // heightMap2 - hill height, used for rolling hills
    m_noise2Dcache.clear();
    m_smoothedNoise2Dcache.clear();
    ++m_seed;
    for (int zrel = 0; zrel < CHUNK_SIZE_Z; ++zrel) {
        for (int xrel = 0; xrel < CHUNK_SIZE_X; ++xrel) {
            heightMap2[xrel+CHUNK_SIZE_X*zrel] = perlinNoise2D((x+xrel)/20.f, (z+zrel)/26.f, 0.5f, 3, 0.f) / 1.75f * 40.f;
        }
    }

    // roughness - weight of heighMap2 added onto heightMap
    m_noise2Dcache.clear();
    m_smoothedNoise2Dcache.clear();
    ++m_seed;
    for (int zrel = 0; zrel < CHUNK_SIZE_Z; ++zrel) {
        for (int xrel = 0; xrel < CHUNK_SIZE_X; ++xrel) {
            int i = xrel+CHUNK_SIZE_X*zrel;
            roughness[i] = perlinNoise2D((x+xrel)/256.f, (z+zrel)/256.f, 0.5f, 2, 0.25f)/1.5f;
            // Use cbrt to cause values to be more extreme in [0,1)
            roughness[i] = 0.5f*(std::cbrt(2.f*roughness[i]-1.f) + 1.f);
        }
    }

    // tempMap - variation in temperature (only needed when it can actually cause biome to change)
    if (baseTemp <= 2.1f && baseTemp >= -2.1f) {
        m_noise2Dcache.clear();
        m_smoothedNoise2Dcache.clear();
        ++m_seed;
        for (int zrel = 0; zrel < CHUNK_SIZE_Z; ++zrel) {
            for (int xrel = 0; xrel < CHUNK_SIZE_X; ++xrel) {
                tempMap[xrel+CHUNK_SIZE_X*zrel] = 0.96f * perlinNoise2D((x+xrel)/64.f, (z+zrel)/64.f, 0.5f, 3, 0.f) / 1.75f;
                // Make biome boundaries "jagged"
                tempMap[xrel+CHUNK_SIZE_X*zrel] += 0.04f * perlinNoise2D((x+xrel)/4.f, (z+zrel)/4.f, 0.5f, 1, 0.f);
            }
        }
        --m_seed;
    } else {
        for (int i = 0; i < CHUNK_SIZE_Z*CHUNK_SIZE_X; ++i)
            tempMap[i] = 0.f;
    }

    m_seed -= 2;

    for (int i = 0; i < CHUNK_SIZE_BLOCKS; ++i) {
        int xrel = CHUNK_X_AT(i);
        int ypos = CHUNK_Y_AT(i) + y;
        int zrel = CHUNK_Z_AT(i);
        int hmi = xrel+CHUNK_SIZE_X*zrel;
        float dHeight = ypos - (roughness[hmi]*heightMap2[hmi] + heightMap[hmi]);
        float temp = (zrel+z)/300.f + (2.f*tempMap[hmi]-1.f);
        if (temp < -1.f) {
            // Cold biome
            if (ypos == 0) chunk[i].type = 2;
            else if (dHeight > 1.f) chunk[i].type = 0;
            else if (dHeight > -1.f) chunk[i].type = 4;
            else if (dHeight > -6.f) chunk[i].type = 3;
            else chunk[i].type = 2;
        } else if (temp > 1.f) {
            // Hot biome
            if (ypos == 0) chunk[i].type = 2;
            else if (dHeight > 0.f) chunk[i].type = 0;
            else if (dHeight > -6.f) chunk[i].type = 5;
            else chunk[i].type = 2;
        } else {
            // Temperate biome
            if (ypos == 0) chunk[i].type = 2;
            else if (dHeight > 0.f) chunk[i].type = 0;
            else if (dHeight > -1.f) chunk[i].type = 1;
            else if (dHeight > -6.f) chunk[i].type = 3;
            else chunk[i].type = 2;
        }
    }

    // Place chaos blocks
    for (int i = 0; i < 4; ++i) {
        if (m_chaosBlocks[i]) {
            int xrel = m_chaosPositions[i].x - x;
            int zrel = m_chaosPositions[i].y - z;
            if (xrel >= 0 && xrel < CHUNK_SIZE_X &&
                zrel >= 0 && zrel < CHUNK_SIZE_Z) {
                int hmi = xrel+CHUNK_SIZE_X*zrel;
                int yrel = glm::floor(roughness[hmi]*heightMap2[hmi] + heightMap[hmi]) + 1.f - y;
                if (yrel >= 0 && yrel < CHUNK_SIZE_Y)
                    chunk[CHUNK_BLOCK_AT(xrel,yrel,zrel)].type = 6;
            }
        }
    }

    delete heightMap;
    delete tempMap;
}

float PNChunkGenerator::perlinNoise2D(float x, float y, float p, int n, float s) {
    float total = 0.f;

    for (int i = 0; i < n; ++i) {
        float frequency = pow(2.f, i);
        float amplitude = pow(p, i);
        total += interpolatedNoise2D(x*frequency, y*frequency, s) * amplitude;
    }

    return total;
}

float PNChunkGenerator::interpolatedNoise2D(float x, float y, float s) {
    int xIp = glm::floor(x);
    int yIp = glm::floor(y);
    float xFp = x - xIp;
    float yFp = y - yIp;

    float v1 = smoothedNoise2D(xIp, yIp, s);
    float v2 = smoothedNoise2D(xIp+1, yIp, s);
    float v3 = smoothedNoise2D(xIp, yIp+1, s);
    float v4 = smoothedNoise2D(xIp+1, yIp+1, s);

    float i1 = interpolateCosine(v1, v2, xFp);
    float i2 = interpolateCosine(v3, v4, xFp);

    return interpolateCosine(i1, i2, yFp);
}

float PNChunkGenerator::smoothedNoise2D(int x, int y, float s) {
    // Fix Cantor pairing for a reasonably large distance
    x += INT_MAX/4;
    y += INT_MAX/4;

    if (s == 0.f)
        return noise2D(x, y);

    int p = (x+y) * (x+y+1) / 2 + x;
    p = (p+m_seed) * (p+m_seed+1) / 2 + p;

    if (!m_smoothedNoise2Dcache.count(p)) {
        float corners = s*0.0625f * (noise2D(x-1, y-1) + noise2D(x+1, y-1) + noise2D(x-1, y+1) + noise2D(x+1, y+1));
        float sides = s*0.125f * (noise2D(x-1, y) + noise2D(x+1, y) + noise2D(x, y-1) + noise2D(x, y+1));
        float center = (0.75f*(1.f-s) + 0.25f) * noise2D(x, y);
        m_smoothedNoise2Dcache[p] = corners + sides + center;
    }

    return m_smoothedNoise2Dcache[p];
}

float PNChunkGenerator::noise2D(int x, int y) {
    int p = (x+y) * (x+y+1) / 2 + x;
    p = (p+m_seed) * (p+m_seed+1) / 2 + p;
    if (!m_noise2Dcache.count(p)) {
        srand(p);
        float val = (float)rand() / RAND_MAX;
        m_noise2Dcache[p] = val;
    }
    return m_noise2Dcache[p];
}

float PNChunkGenerator::interpolateCosine(float a, float b, float x) {
    float f = 0.5f - 0.5f*glm::cos(x*glm::pi<float>());
    return a*(1.0-f) + b*f;
}
