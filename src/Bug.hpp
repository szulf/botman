#pragma once

#include "misc.hpp"
#include <filesystem>
#include <vector>

class Bug
{
public:
    Bug(const Vec2& start_pos, std::filesystem::path sprite_path);
    ~Bug();

    auto draw() const -> void;
    auto move(const std::vector<Vec2>& moves) -> void;
    auto update_pos() -> void;

    inline auto live() -> void { m_alive = true; }
    inline auto die() -> void { m_alive = false; }

    inline auto get_pos() const -> const Vec2& { return m_pos; }

    inline auto get_rect() const -> const Rectangle& { return m_dest_rect; }

private:
    Vec2 m_movement{0, 0};
    Vec2 m_pos{};
    bool m_alive{true};

    Rectangle m_src_rect{};
    Rectangle m_dest_rect{};
    uint8_t m_frame{};

    const Texture2D m_sprite{};
};
