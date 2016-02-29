#pragma once

#include "../terrainmanager.h"
#include "chunkgenerator.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

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

    std::thread *m_genThread;
    std::atomic_bool m_genDone;
    std::mutex m_protoLock;
    std::mutex m_protoWaitMut;
    std::unique_lock<std::mutex> m_protoWaitLock = std::unique_lock<std::mutex>(m_protoWaitMut);
    std::condition_variable m_protoWait;
    std::list<Chunk *> m_allChunks;
    std::list<Chunk *> m_protoChunks;
    std::list<Chunk *> m_protoDone;

private:
    static void thread_chunkStreamWorker(VoxelManager *self);

public:
    const VALUE_ACCESSOR(BlockType*,blockDefs)
    Block getBlock(int x, int y, int z);
    void setBlock(int x, int y, int z, Block block);

    void streamChunksAround(glm::vec3 pos, float in, float out, bool proto = true);

    virtual void tick(float seconds) override;
    virtual void draw() override;
    virtual glm::vec3 collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const override;
    virtual bool rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const override;
};

}
}
