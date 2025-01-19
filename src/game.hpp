#pragma once

#include "map.hpp"
#include "robot.hpp"
#include "texture_manager.hpp"

namespace botman
{

class Game
{
public:
    Game();
    ~Game();

    auto run() -> void;

private:
    inline auto robot_collides_with_pellet() -> bool;
    inline auto robot_collides_with_hammer() -> bool;

private:
    u16 m_max_fps{165};
    float m_delta_time{};

    u32 m_score{0};

    TextureManager m_tm{};
    Map m_map;

    Robot m_robot;

};

}
