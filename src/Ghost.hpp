#pragma once

#include "misc.hpp"
#include <filesystem>
#include <vector>

class Ghost
{
public:
    Ghost(const Vec2& start_pos, std::filesystem::path sprite_path);
    ~Ghost();

    auto draw() const -> void;
    auto move(const std::vector<Vec2>& moves) -> void;
    auto update_pos() -> void;

    inline auto start_moving() -> void { m_moving = true; }

    inline auto get_pos() const -> const Vec2& { return m_pos; }

    inline auto get_dest_rect() const -> const Rectangle& { return m_dest_rect; }

private:
    Vec2 m_movement{0, 0};
    Vec2 m_pos{};
    bool m_moving{false};

    Rectangle m_src_rect{};
    Rectangle m_dest_rect{};

    const Texture2D m_sprite{};
};
