#include "pnchunkgenerator.h"
#include "../util/CommonIncludes.h"

using namespace Minecraft;

PNChunkGenerator::PNChunkGenerator(int seed)
    : m_seed(seed)
{ }

void PNChunkGenerator::generate(CS1972Engine::Voxel::Block (&chunk)[CHUNK_SIZE_BLOCKS], int x, int y, int z) {
    float *heightMap = new float[CHUNK_SIZE_X*CHUNK_SIZE_Z];
    float *heightMap2 = new float[CHUNK_SIZE_X*CHUNK_SIZE_Z];

    for (int zrel = 0; zrel < CHUNK_SIZE_Z; ++zrel)
        for (int xrel = 0; xrel < CHUNK_SIZE_X; ++xrel) {
            heightMap[xrel+CHUNK_SIZE_X*zrel] = perlinNoise2D((x+xrel)/32.f, (z+zrel)/32.f, 0.5f, 3) * 48.f;
            ++m_seed;
            heightMap2[xrel+CHUNK_SIZE_X*zrel] = perlinNoise2D((x+xrel)/32.f, (z+zrel)/32.f, 0.5f, 3) * 8.f + 3.f;
            --m_seed;
        }

    for (int i = 0; i < CHUNK_SIZE_BLOCKS; ++i) {
        int xrel = CHUNK_X_AT(i);
        int ypos = CHUNK_Y_AT(i) + y;
        int zrel = CHUNK_Z_AT(i);
        int hmi = xrel+CHUNK_SIZE_X*zrel;
        float dHeight = ypos - heightMap[hmi];
        if (dHeight > 0.f) chunk[i].type = 0;
        else if (dHeight > -1.f) chunk[i].type = 1;
        else if (dHeight > -1.f*heightMap2[hmi]) chunk[i].type = 3;
        else chunk[i].type = 2;
    }

    delete heightMap;
    delete heightMap2;
}

float PNChunkGenerator::perlinNoise2D(float x, float y, float p, int n) {
    float total = 0.f;

    for (int i = 0; i < n; ++i) {
        float frequency = pow(2.f, i);
        float amplitude = pow(p, i);
        total += interpolatedNoise2D(x*frequency, y*frequency) * amplitude;
    }

    return total;
}

float PNChunkGenerator::interpolatedNoise2D(float x, float y) {
    int xIp = (int)x;
    int yIp = (int)y;
    float xFp = x - xIp;
    float yFp = y - yIp;
    if (xFp < 0.f) {
        xFp += 1.f;
        xIp -= 1;
    }
    if (yFp < 0.f) {
        yFp += 1.f;
        yIp -= 1;
    }

    float v1 = smoothedNoise2D(xIp, yIp);
    float v2 = smoothedNoise2D(xIp+1, yIp);
    float v3 = smoothedNoise2D(xIp, yIp+1);
    float v4 = smoothedNoise2D(xIp+1, yIp+1);

    float i1 = interpolateCosine(v1, v2, xFp);
    float i2 = interpolateCosine(v3, v4, xFp);

    return interpolateCosine(i1, i2, yFp);
}

float PNChunkGenerator::smoothedNoise2D(int x, int y) {
    float corners = (noise2D(x-1, y-1) + noise2D(x+1, y-1) + noise2D(x-1, y+1) + noise2D(x+1, y+1)) / 16.f;
    float sides = (noise2D(x-1, y) + noise2D(x+1, y) + noise2D(x, y-1) + noise2D(x, y+1)) / 8.f;
    float center = noise2D(x, y) / 4.f;
    return corners + sides + center;
}

float PNChunkGenerator::noise2D(int x, int y) {
    int p = (x+y) * (x+y+1) / 2 + x;
    p = (p+m_seed) * (p+m_seed+1) / 2 + p;
    srand(p);
    return (float)rand() / RAND_MAX;
}

float PNChunkGenerator::interpolateCosine(float a, float b, float x) {
    float f = 0.5f - 0.5f*glm::cos(x*glm::pi<float>());
    return a*(1.0-f) + b*f;
}
