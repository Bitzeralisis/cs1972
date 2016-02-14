#pragma once

namespace CS1972Engine {
namespace Voxel {

typedef unsigned char BLOCKID;

struct Block {
    BLOCKID type;
};

struct BlockType {
private:
    const char *const m_name;
    const unsigned int m_data;
public:
    BlockType() = delete;
    constexpr BlockType(const char *const name, bool transparent, bool passable, int tilesheetx, int tilesheety)
        : m_name(name)
        , m_data((transparent << 0) +
                 (passable << 1) +
                 ((tilesheetx && 0xFF) << 8) +
                 ((tilesheety && 0xFF) << 16))
    { }
    constexpr bool transparent() const { return (m_data >> 0) & 0x1; }
    constexpr bool passable() const { return (m_data >> 1) & 0x1; }
    constexpr int tilesheetx() const { return (m_data >> 8) & 0xFF; }
    constexpr int tilesheety() const { return (m_data >> 16) & 0xFF; }
};

constexpr BlockType BLOCK_DEFINITIONS[2] = {
    BlockType("air", true, true, 0, 0),
    BlockType("grass", false, false, 1, 0)
};

}
}
