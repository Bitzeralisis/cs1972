#include "voxelmanager.h"
#include "chunk.h"

using namespace CS1972Engine::Voxel;

VoxelManager::VoxelManager(World *parent, const BlockType *blockDefs, ChunkGenerator *generator)
    : TerrainManager(parent)
    , m_blockDefs(blockDefs)
    , m_generator(generator)
{
    m_genDone.store(false);
    m_genThread = new std::thread(thread_chunkStreamWorker, this);
}

VoxelManager::~VoxelManager() {
    // Wait for chunk streamer to finish working and join/delete it
    m_genDone.store(true);
    m_protoWait.notify_all();
    m_genThread->join();
    delete m_genThread;

    delete m_generator;

    // Deallocate all chunks
    for (std::list<Chunk *>::iterator it = m_allChunks.begin(); it != m_allChunks.end(); ++it)
        delete *it;
}

void VoxelManager::thread_chunkStreamWorker(VoxelManager *self) {
    while (!self->m_genDone.load()) {
        // Wait for work
        self->m_protoWait.wait_for(self->m_protoWaitLock, std::chrono::seconds(1));
        if (self->m_genDone.load())
            return;

        // Get one chunk to generate
        self->m_protoLock.lock();
        if (self->m_protoChunks.empty()) {
            self->m_protoLock.unlock();
            continue;
        }
        Chunk *chunk = self->m_protoChunks.front();
        self->m_protoChunks.pop_front();
        self->m_protoLock.unlock();

        // Generate the blocks; this takes a long time
        chunk->generateBlocks(self->m_generator);

        // Put finished chunk into finished chunks list
        self->m_protoLock.lock();
        self->m_protoDone.push_back(chunk);
        self->m_protoLock.unlock();
    }
}

Block VoxelManager::getBlock(int x, int y, int z) {
    // Find chunk in which this block exists (if there is a chunk)
    int x2 = glm::floor((float)x/CHUNK_SIZE_X)*CHUNK_SIZE_X;
    int y2 = glm::floor((float)y/CHUNK_SIZE_Y)*CHUNK_SIZE_Y;
    int z2 = glm::floor((float)z/CHUNK_SIZE_Z)*CHUNK_SIZE_Z;
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
        if (x2 == (*it)->x() && y2 == (*it)->y() && z2 == (*it)->z())
            return (*it)->blockAtGlobal(x, y, z);
    return Block{0};
}

void VoxelManager::setBlock(int x, int y, int z, Block b) {
    // Find chunk in which this block exists (if there is a chunk)
    int x2 = glm::floor((float)x/CHUNK_SIZE_X)*CHUNK_SIZE_X;
    int y2 = glm::floor((float)y/CHUNK_SIZE_Y)*CHUNK_SIZE_Y;
    int z2 = glm::floor((float)z/CHUNK_SIZE_Z)*CHUNK_SIZE_Z;
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        if (x2 == (*it)->x() && y2 == (*it)->y() && z2 == (*it)->z()) {
            (*it)->setBlockGlobal(x, y, z, b);
            (*it)->generateVao();
            break;
        }
    }
}

void VoxelManager::streamChunksAround(glm::vec3 pos, float in, float out, bool proto) {
    glm::vec2 xzPos = glm::vec2(pos.x, pos.z);

    // Calculate hard bounds on possible new chunk ids to stream in
    glm::vec2 inMin = glm::floor((xzPos - glm::vec2(in)) / glm::vec2(CHUNK_SIZE_X, CHUNK_SIZE_Z));
    glm::vec2 inMax = glm::floor((xzPos + glm::vec2(in)) / glm::vec2(CHUNK_SIZE_X, CHUNK_SIZE_Z));

    // Stream in new chunks
    for (int x = inMin.x; x <= inMax.x; ++x) {
        for (int y = inMin.y; y <= inMax.y; ++y) {
            glm::vec2 newIdx(x, y);

            // Make sure this chunk id is actually inside streaming radius
            glm::vec2 newCenter = glm::vec2((x+0.5f)*CHUNK_SIZE_X, (y+0.5f)*CHUNK_SIZE_Z);
            if (glm::distance2(xzPos, newCenter) > in*in)
                continue;

            // Check if this chunk already exists
            bool exists = false;
            for (std::list<Chunk *>::iterator it = m_allChunks.begin(); it != m_allChunks.end(); ++it) {
                glm::vec2 chunkIdx = glm::vec2((*it)->x(), (*it)->z()) / glm::vec2(CHUNK_SIZE_X, CHUNK_SIZE_Z);
                if (newIdx == chunkIdx) {
                    exists = true;
                    break;
                }
            }
            if (exists)
                continue;

            if (proto)
                m_protoLock.lock();
            Chunk *c1 = new Chunk(this, x*CHUNK_SIZE_X, 0, y*CHUNK_SIZE_Z, m_generator, proto);
            Chunk *c2 = new Chunk(this, x*CHUNK_SIZE_X, CHUNK_SIZE_Y, y*CHUNK_SIZE_Z, m_generator, proto);
            m_allChunks.push_back(c1);
            m_allChunks.push_back(c2);
            if (!proto) {
                m_chunks.push_back(c1);
                m_chunks.push_back(c2);
            } else {
                m_protoChunks.push_back(c1);
                m_protoChunks.push_back(c2);
                m_protoLock.unlock();
            }
        }
    }

    // Stream out chunks
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        glm::vec2 chunkCenter = glm::vec2((*it)->x() + 0.5f*CHUNK_SIZE_X, (*it)->z() + 0.5f*CHUNK_SIZE_Z);
        if (glm::distance2(xzPos, chunkCenter) > out*out) {
            m_allChunks.remove(*it);
            delete *it;
            it = m_chunks.erase(it);
        }
    }

    // Check for a proto-chunk that has finished generating blocks, generate its VAO, and add to chunks
    m_protoLock.lock();
    if (!m_protoDone.empty()) {
        Chunk *chunk = m_protoDone.front();
        m_protoDone.pop_front();
        m_protoLock.unlock();
        chunk->generateVao();
        m_chunks.push_back(chunk);
    } else {
        m_protoLock.unlock();
    }

    // Wake up streaming thread if it's asleep
    m_protoWait.notify_all();
}

void VoxelManager::tick(float seconds) {
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        (*it)->tick(seconds);
    }
}

void VoxelManager::draw() {
    for (std::list<Chunk *>::iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        if (!graphics().camera->frustumCullAABB((*it)->aabb())) {
            (*it)->draw();
        }
    }
}

glm::vec3 VoxelManager::collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1) const {
    glm::vec3 p0 = pos0;
    glm::vec3 p1 = pos1;
    // Must sweep in each dimension across all chunks at once, or else boxes start running into edges at chunk boundaries
    for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
        p1 = (*it)->collideAABB(aabb, p0, p1, 1);
    p0[1] = p1[1];
    // Collide the dimension we are moving in first so that we run into corners correctly
    if (glm::abs((p1-p0)[0]) > glm::abs((p1-p0)[2])) {
        for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
            p1 = (*it)->collideAABB(aabb, p0, p1, 0);
        p0[0] = p1[0];
        for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
            p1 = (*it)->collideAABB(aabb, p0, p1, 2);
        p0[2] = p1[2];
    } else {
        for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
            p1 = (*it)->collideAABB(aabb, p0, p1, 2);
        p0[2] = p1[2];
        for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
            p1 = (*it)->collideAABB(aabb, p0, p1, 0);
        p0[0] = p1[0];
    }
    return p1;
}

float VoxelManager::rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const {
    bool retval = false;
    float maxDist = std::numeric_limits<float>::infinity();
    glm::vec3 i, n;
    for (std::list<Chunk *>::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it) {
        if ((*it)->rayCast(p, v, range, i, n)) {
            float dist = glm::distance2(p, i);
            if (dist < maxDist) {
                maxDist = dist;
                intersect = i;
                normal = n;
                retval = true;
            }
        }
    }
    return retval;
}
