#pragma once

#include "constants.hpp"
#include "map.hpp"
#include "movement.hpp"

#include <string>

namespace botman
{

class Robot
{
public:
    Robot(const vec2& start_pos, std::string_view texture_id, TextureManager& tm, Map& map) : m_pos{start_pos}, m_texture_id{texture_id}, m_tm{tm}, m_map{map} {}

    auto draw() const -> void;

    auto set_pos(const vec2& pos) -> void { m_pos = pos; }
    auto get_pos() -> const vec2& { return m_pos; }

    auto move(const Movement& movement) -> void;
    auto rotate() -> bool;
    auto update_pos(float dt) -> void;

    auto collides(const Rectangle& other) -> bool;

    auto get_next_move() const -> const Movement& { return m_next_move; }

private:
    auto center_pos() -> void;

private:
    vec2 m_movement{-1, 0};
    vec2 m_pos{};

    std::string m_texture_id{};
    u8 m_frame{};

    Movement m_next_move{Movement::NONE};
    float m_time_between_moves{};

    TextureManager& m_tm;
    Map& m_map;

};

}
