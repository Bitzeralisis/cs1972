#pragma once

#define CHUNK_SIZE_X (32)
#define CHUNK_SIZE_Y (32)
#define CHUNK_SIZE_Z (32)
#define CHUNK_SIZE_XZ_GLMVEC2 (glm::vec2(CHUNK_SIZE_X,CHUNK_SIZE_Z))
#define CHUNK_SIZE_GLMVEC3 (glm::vec3(CHUNK_SIZE_X,CHUNK_SIZE_Y,CHUNK_SIZE_Z))
#define CHUNK_SIZE_BLOCKS (CHUNK_SIZE_X*CHUNK_SIZE_Y*CHUNK_SIZE_Z)

#define CHUNK_X_AT(i) ((i)%CHUNK_SIZE_X)
#define CHUNK_Y_AT(i) ((i)/(CHUNK_SIZE_X*CHUNK_SIZE_Z))
#define CHUNK_Z_AT(i) (((i)/CHUNK_SIZE_X)%CHUNK_SIZE_Z)
#define CHUNK_BLOCK_AT(x,y,z) ((CHUNK_SIZE_X*CHUNK_SIZE_Z*(y))+(CHUNK_SIZE_X*(z))+(x))
#define CHUNK_DEFINITION_AT(x,y,z) (m_parent->blockDefs()[m_blocks[CHUNK_BLOCK_AT(x,y,z)].type])
