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
    u16 m_max_fps{60};
    float m_delta_time{};

    TextureManager m_tm{};
    Map m_map;

    Robot m_robot;

};

}
