#pragma once

#include "misc.hpp"
#include <filesystem>

class Robot
{
public:
    Robot(const Vec2& start_pos, std::filesystem::path sprite_path);
    ~Robot();

    auto draw() const -> void ;
    auto move(const Movement& movement) -> void;
    auto reset_movement() -> void;
    auto rotate() -> void;
    auto update_pos() -> void;
    auto collides(const Rectangle& rect) const -> bool;

    inline auto get_movement() const -> const Vec2& { return m_movement; }

    inline auto get_pos() const -> const Vec2& { return m_pos; }

private:
    Vec2 m_movement{0, 0};
    Vec2 m_pos{};

    float m_rotation{0.0f};
    Rectangle m_src_rect{};
    Rectangle m_dest_rect{};
    Movement m_next_move{};
    float m_time_between_moves{0.0f};
    uint8_t m_frame{};

    const Texture2D m_sprite{};
};
