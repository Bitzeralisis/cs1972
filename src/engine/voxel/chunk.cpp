#include "chunk.h"
#include "chunkgenerator.h"

using namespace CS1972Engine::Voxel;

Chunk::Chunk(VoxelManager *parent, int x, int y, int z, ChunkGenerator *g, bool proto)
    : m_parent(parent)
    , m_x(x)
    , m_y(y)
    , m_z(z)
{
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    m_vertices = 0;

    // Non proto-chunks generate themselves immediately on construction
    if (!proto) {
        generateBlocks(g);
        generateVao();
    }
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Chunk::generateBlocks(ChunkGenerator *g) {
    g->generate(m_blocks, m_x, m_y, m_z);
}

void Chunk::generateVao() {
    // Count number of faces that need to be rendered
    int nfaces = 0;
    for (int i = 0; i < CHUNK_SIZE_BLOCKS; ++i) {
        int x = CHUNK_X_AT(i);
        int y = CHUNK_Y_AT(i);
        int z = CHUNK_Z_AT(i);
        BlockType t = CHUNK_DEFINITION_AT(x,y,z);
        // Skip transparent and special-render blocks
        if (t.transparent() || t.special())
            continue;
        // Left
        if (x == 0 || CHUNK_DEFINITION_AT(x-1,y,z).transparent())
            ++nfaces;
        // Right
        if (x == CHUNK_SIZE_X-1 || CHUNK_DEFINITION_AT(x+1,y,z).transparent())
            ++nfaces;
        // Front
        if (z == 0 || CHUNK_DEFINITION_AT(x,y,z-1).transparent())
            ++nfaces;
        // Back
        if (z == CHUNK_SIZE_Z-1 || CHUNK_DEFINITION_AT(x,y,z+1).transparent())
            ++nfaces;
        // Down
        if (y == 0 || CHUNK_DEFINITION_AT(x,y-1,z).transparent())
            ++nfaces;
        // Up
        if (y == CHUNK_SIZE_Y-1 || CHUNK_DEFINITION_AT(x,y+1,z).transparent())
            ++nfaces;
    }

    m_special.clear();

    // For each face, there are 6 vertex(3)-normal(3)-tex(2) coordinate sets, meaning 4*(3+3+2) floats per face
    m_vertices = 6*nfaces;
    GLfloat *data = new GLfloat[m_vertices*(3+3+2)];
    GLfloat *d = data;

/**
 * l - index of the "left-right" dimension the face is parallel to
 * m - index of the "up-down" dimension the face is parallel to
 * n - index of dimension the face is normal to
 * (x1,y1) - top-left of face
 * (x2,y2) - bottom-right of face
 * z - position of face, in dimension n
 * (i,j,k) - normal vector of face
 * (u1,v1) - texture coords of top-left of face
 * (u2,v2) - texture coords of bottom-right of face
 */
#define MAKE_FACE(l,m,n,x1,y1,x2,y2,z,i,j,k,u1,v1,u2,v2) { \
    d[l] = x1; d[m] = y1; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u1; d[7] = v1; \
    d += 8; \
    d[l] = x1; d[m] = y2; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u2; d[7] = v1; \
    d += 8; \
    d[l] = x2; d[m] = y2; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u2; d[7] = v2; \
    d += 8; \
    \
    d[l] = x2; d[m] = y2; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u2; d[7] = v2; \
    d += 8; \
    d[l] = x2; d[m] = y1; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u1; d[7] = v2; \
    d += 8; \
    d[l] = x1; d[m] = y1; d[n] = z; \
    d[3] = i; d[4] = j; d[5] = k; \
    d[6] = u1; d[7] = v1; \
    d += 8; \
}

    // Traverse array again but this time actually build the faces
    for (int i = 0; i < CHUNK_SIZE_BLOCKS; ++i) {
        int x = CHUNK_X_AT(i);
        int y = CHUNK_Y_AT(i);
        int z = CHUNK_Z_AT(i);
        BlockType t = CHUNK_DEFINITION_AT(x,y,z);
        // Add special-render blocks to special-render list
        if (t.special())
            m_special.push_back(glm::vec3(x,y,z));
        // Skip transparent and special-render blocks
        if (t.transparent() || t.special())
            continue;
        GLfloat bx = m_x + x;
        GLfloat by = m_y + y;
        GLfloat bz = m_z + z;
        GLfloat dt = 1.f/16.f;
        // Left
        if (x == 0 || CHUNK_DEFINITION_AT(x-1,y,z).transparent()) {
            MAKE_FACE(1,2,0,by+1.f,bz+1.f,by,bz,bx,-1.f,0.f,0.f,t.atlassidex(),t.atlassidey()+dt,t.atlassidex()+dt,t.atlassidey())
        }
        // Right
        if (x == CHUNK_SIZE_X-1 || CHUNK_DEFINITION_AT(x+1,y,z).transparent()) {
            MAKE_FACE(1,2,0,by+1.f,bz,by,bz+1.f,bx+1.f,1.f,0.f,0.f,t.atlassidex(),t.atlassidey()+dt,t.atlassidex()+dt,t.atlassidey())
        }
        // Front
        if (z == 0 || CHUNK_DEFINITION_AT(x,y,z-1).transparent()) {
            MAKE_FACE(1,0,2,by+1.f,bx,by,bx+1.f,bz,0.f,0.f,-1.f,t.atlassidex(),t.atlassidey()+dt,t.atlassidex()+dt,t.atlassidey())
        }
        // Back
        if (z == CHUNK_SIZE_Z-1 || CHUNK_DEFINITION_AT(x,y,z+1).transparent()) {
            MAKE_FACE(1,0,2,by+1.f,bx+1.f,by,bx,bz+1.f,0.f,0.f,1.f,t.atlassidex(),t.atlassidey()+dt,t.atlassidex()+dt,t.atlassidey())
        }
        // Down
        if (y == 0 || CHUNK_DEFINITION_AT(x,y-1,z).transparent()) {
            MAKE_FACE(0,2,1,bx+1.f,bz,bx,bz+1.f,by,0.f,0.f,-1.f,t.atlasbotx(),t.atlasboty()+dt,t.atlasbotx()+dt,t.atlasboty())
        }
        // Up
        if (y == CHUNK_SIZE_Y-1 || CHUNK_DEFINITION_AT(x,y+1,z).transparent()) {
            MAKE_FACE(0,2,1,bx,bz,bx+1.f,bz+1.f,by+1.f,0.f,1.f,0.f,t.atlastopx(),t.atlastopy()+dt,t.atlastopx()+dt,t.atlastopy());
        }
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, m_vertices*8*sizeof(GLfloat), data, GL_STATIC_DRAW);

    // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, 0);
    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*3));
    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, (void*)(sizeof(GLfloat)*6));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete data;
}

void Chunk::tick(float seconds) {
    m_time += seconds;
}

void Chunk::draw() {
    graphics().shaderColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
    graphics().shaderUseTexture(true);
    graphics().shaderBindTexture(graphics().getTexture("atlas"));
    graphics().shaderMTransform(glm::mat4(1.f));
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices);
    glBindVertexArray(0);

    // Draw special-render blocks
    graphics().shaderColor(glm::vec4(0.5f+0.5f*glm::cos(3.f*glm::pi<float>()*m_time), 0.f, 1.f, 1.f));
    graphics().shaderUseTexture(false);
    for (std::list<glm::vec3>::iterator it = m_special.begin(); it != m_special.end(); ++it) {
        graphics().shaderMTransform(glm::translate(glm::mat4(1.f), (*it)+glm::vec3(0.5f, 0.5f, 0.5f)+glm::vec3(m_x,m_y,m_z)));
        graphics().drawBox();
    }
}

glm::vec3 Chunk::collideAABB(const csm::aabb &aabb, const glm::vec3 &pos0, const glm::vec3 &pos1, int dimension) const {
    const csm::aabb t0(aabb.pos+pos0, aabb.size);
    const csm::aabb t1(aabb.pos+pos1, aabb.size);

    // Broadphase aabb - does this aabb even pass through this chunk?
    const csm::aabb self = this->aabb();
    const glm::vec3 bpmin = glm::floor(glm::min(t0.pos, t1.pos));
    const glm::vec3 bpmax = glm::ceil(glm::max(t0.pos+t0.size, t1.pos+t1.size));
    const csm::aabb bp(bpmin, bpmax-bpmin);
    if (!bp.intersects(self))
        return pos1;

/**
 * START - glm::vec3 that the object starts at; set by this macro to the resolved position after this sweep
 * END - glm::vec3 that the object ends at; set by this macro to the sweep dimension resolved after this sweep
 * SWEEP - index of dimension being swept
 * SWEEP_M_VAR - m_x, m_y, or m_z
 * OTHER1 - index of dimension not being swept
 * OTHER2 - index of other dimension not being swept
 * ORDER0 - i, j, or k, depending on which one corresponds to 0 (where i is OTHER1, j is OTHER2, k is SWEEP)
 * ORDER1 - same as ORDER0 but corresponding to 1
 * ORDER2 - same as ORDER0 but corresponding to 2
 */
#define SWEEP_TEST(START,END,SWEEP,SWEEP_M_VAR,OTHER1,OTHER2,ORDER0,ORDER1,ORDER2) { \
    glm::vec3 move = END-START; \
    if (move[SWEEP] != 0.f) { \
        /* Broadphase aabb for just sweep in this dimension */ \
        glm::vec3 sweepmin = glm::floor(START); \
        glm::vec3 sweepmax = glm::ceil(START+aabb.size-glm::vec3(1.f,1.f,1.f)); \
        sweepmin[SWEEP] = move[SWEEP] > 0.f ? glm::ceil(START[SWEEP]+aabb.size[SWEEP]) : glm::floor(END[SWEEP]); \
        sweepmax[SWEEP] = move[SWEEP] > 0.f ? glm::ceil(END[SWEEP]+aabb.size[SWEEP]-1.f) : glm::floor(START[SWEEP]-1.f); \
 \
        /* Bound broadphase values within chunk's aabb for blocks we are testing collisions against */ \
        sweepmin = glm::max(sweepmin, self.pos); \
        sweepmax = glm::min(sweepmax, self.pos+self.size+glm::vec3(-1.f, -1.f, -1.f)); \
 \
        /* Make broadphase values into local space */ \
        sweepmin -= glm::vec3(m_x, m_y, m_z); \
        sweepmax -= glm::vec3(m_x, m_y, m_z); \
 \
        /* Figure out wheter to sweep low to high or high to low based on direction we are travelling */ \
        int lo = move[SWEEP] > 0.f ? sweepmin[SWEEP] : sweepmax[SWEEP]; \
        int hi = move[SWEEP] > 0.f ? sweepmax[SWEEP] : sweepmin[SWEEP]; \
        int dir = move[SWEEP] > 0.f ? 1 : -1; \
 \
        /* Sweep through the blocks */ \
        for (int i = lo; dir*i <= dir*hi; i += dir) { \
            for (int j = sweepmin[OTHER1]; j <= sweepmax[OTHER1]; ++j) { \
                for (int k = sweepmin[OTHER2]; k <= sweepmax[OTHER2]; ++k) { \
                    if (!CHUNK_DEFINITION_AT(ORDER0,ORDER1,ORDER2).passable()) { \
                        /* Collided with a block */ \
                        END[SWEEP] = dir>0?i+SWEEP_M_VAR-aabb.size[SWEEP]:i+SWEEP_M_VAR+1.f; \
                        goto done##SWEEP; \
                    } \
                } \
            } \
        } \
    } \
\
    /* Some say this is the only legitimate use of goto */ \
    done##SWEEP:; \
    START[SWEEP] = END[SWEEP]; \
}

    glm::vec3 startPos = t0.pos;
    glm::vec3 endPos = t1.pos;
    if (dimension == 0) SWEEP_TEST(startPos,endPos,0,m_x,1,2,i,j,k)
    else if (dimension == 1) SWEEP_TEST(startPos,endPos,1,m_y,2,0,k,i,j)
    else if (dimension == 2) SWEEP_TEST(startPos,endPos,2,m_z,1,0,k,j,i)

    return endPos-aabb.pos;
}

bool Chunk::rayCast(const glm::vec3 &p, const glm::vec3 &v, float range, glm::vec3 &intersect, glm::vec3 &normal) const {
    range *= glm::length(v);

    // First check for collision with entire chunk with ray-AABB collision test
    glm::vec3 pLoc = p - glm::vec3(m_x, m_y, m_z); // Local coord

    float t1 = (0.f - pLoc.x) / v.x;
    float t2 = (CHUNK_SIZE_X - pLoc.x) / v.x;
    float t3 = (0.f - pLoc.y) / v.y;
    float t4 = (CHUNK_SIZE_Y - pLoc.y) / v.y;
    float t5 = (0.f - pLoc.z) / v.z;
    float t6 = (CHUNK_SIZE_Z - pLoc.z) / v.z;

    float sweepMin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    float sweepMax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

    if (sweepMax <= 0.f || sweepMin >= sweepMax || sweepMin > range) {
        // Ray ahead of chunk || No intersect || Ray enters chunk too later
        return false;
    }

    // Do actual raycasting in this chunk
    int istep[3] = { v[0] < 0.f ? -1 : 1, v[1] < 0 ? -1 : 1, v[2] < 0.f ? -1 : 1 };
    int iexit[3] = { v[0] < 0.f ? 0 : CHUNK_SIZE_X-1, v[1] < 0.f ? 0 : CHUNK_SIZE_Y-1, v[2] < 0.f ? 0 : CHUNK_SIZE_Z-1 }; // Local coord
    glm::vec3 step(v[0] < 0.f ? -1.f : 1.f, v[1] < 0.f ? -1.f : 1.f, v[2] < 0.f ? -1.f : 1.f);
    glm::vec3 tdelta = step / v;

    glm::vec3 enter = step*glm::ceil(step*p); // ceil if positive step, floor if negative step
    glm::vec3 denter = enter-p;
    glm::vec3 tmax = denter / v;

    // posneg required to deal with position exactly on a block boundary properly
    glm::vec3 posneg(v[0] < 0.f ? 0.f : 1.f, v[1] < 0.f ? 0.f : 1.f, v[2] < 0.f ? 0.f : 1.f);
    glm::vec3 fpLoc = step*glm::ceil(step*pLoc) - 1.f*posneg;
    int ipos[3] = { (int)fpLoc[0], (int)fpLoc[1], (int)fpLoc[2] }; // Local coord

    while (1) {
        int d = tmax[0] < tmax[1] ? (tmax[0] < tmax[2] ? 0 : 2) : (tmax[1] < tmax[2] ? 1 : 2);
        ipos[d] += istep[d];
        tmax[d] += tdelta[d];
        if (istep[d]*ipos[d] > istep[d]*iexit[d] || tmax[d] > range)
            return false;

        if (ipos[0] >= 0 && ipos[0] < CHUNK_SIZE_X &&
            ipos[1] >= 0 && ipos[1] < CHUNK_SIZE_Y &&
            ipos[2] >= 0 && ipos[2] < CHUNK_SIZE_Z ) {
            if (!CHUNK_DEFINITION_AT(ipos[0],ipos[1],ipos[2]).passable()) {
                intersect = glm::vec3(ipos[0]+m_x,ipos[1]+m_y,ipos[2]+m_z);
                normal = glm::vec3(0.f);
                normal[d] = -1.f*step[d];
                return true;
            }
        }
    }
}
