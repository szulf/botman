#pragma once

#include "misc.hpp"
#include <filesystem>

class Pacman
{
public:
    Pacman(const Vec2& start_pos, std::filesystem::path sprite_path);
    ~Pacman();

    auto draw() const -> void;
    auto move(const Movement& movement) -> void;
    auto reset_movement() -> void;
    auto rotate() -> void;
    auto update_pos() -> void;
    auto collides(const Rectangle& rect) -> bool;

    inline auto get_movement() const -> const Vec2& { return m_movement; }

    inline auto get_pos() const -> const Vec2& { return m_pos; }

private:
    Vec2 m_movement;
    Vec2 m_pos;

    float m_rotation;
    Rectangle m_src_rect;
    Rectangle m_dest_rect;
    Movement m_next_move;
    float m_time_between_moves;

    const Texture2D m_sprite;
};
