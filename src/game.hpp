#pragma once

#include "map.hpp"
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
    TextureManager m_tm{};
    Map m_map;

};

}
