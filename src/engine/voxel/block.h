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
    const bool m_transparent;
    const bool m_passable;
    const bool m_special;
    const float m_atlastopx;
    const float m_atlastopy;
    const float m_atlassidex;
    const float m_atlassidey;
    const float m_atlasbotx;
    const float m_atlasboty;
public:
    BlockType() = delete;
    constexpr BlockType(const char *const name, bool transparent, bool passable,
                        float atlastopx, float atlastopy, float atlassidex, float atlassidey, float atlasbotx, float atlasboty)
        : m_name(name)
        , m_transparent(transparent)
        , m_passable(passable)
        , m_special(false)
        , m_atlastopx(atlastopx)
        , m_atlastopy(atlastopy)
        , m_atlassidex(atlassidex)
        , m_atlassidey(atlassidey)
        , m_atlasbotx(atlasbotx)
        , m_atlasboty(atlasboty)
    { }
    constexpr BlockType(const char *const name, bool transparent, bool passable, bool special)
        : m_name(name)
        , m_transparent(transparent)
        , m_passable(passable)
        , m_special(special)
        , m_atlastopx(0.f)
        , m_atlastopy(0.f)
        , m_atlassidex(0.f)
        , m_atlassidey(0.f)
        , m_atlasbotx(0.f)
        , m_atlasboty(0.f)
    { }
    constexpr const char* name() const { return m_name; }
    constexpr bool transparent() const { return m_transparent; }
    constexpr bool passable() const { return m_passable; }
    constexpr bool special() const { return m_special; }
    constexpr float atlastopx() const { return m_atlastopx; }
    constexpr float atlastopy() const { return m_atlastopy; }
    constexpr float atlassidex() const { return m_atlassidex; }
    constexpr float atlassidey() const { return m_atlassidey; }
    constexpr float atlasbotx() const { return m_atlasbotx; }
    constexpr float atlasboty() const { return m_atlasboty; }
};

}
}
