#pragma once

#include "constants.hpp"

#include "raylib.h"
#include <filesystem>
#include <vector>

namespace botman
{

enum class Tile
{
    EMPTY,
    WALL,
    PELLET,
    START_POS,
    SPAWNER,
    HAMMER,
};

class Map
{
public:
    Map(const vec2& pos, const std::filesystem::path& path);
    Map(const vec2& pos) : m_pos{pos} {}

    auto draw() -> void;

    auto load_from_file(const std::filesystem::path& path) -> void;
    auto save_to_file(const std::filesystem::path& path) -> void;

    // grid - local to map grid position,
    // pos - global space on the screen
    auto get_pos_from_grid(const vec2& grid_pos) -> vec2;
    // grid - local to map grid position,
    // pos - global space on the screen
    auto get_grid_from_pos(const vec2& pos) -> vec2;

private:
    auto m_draw_grid() -> void;
    auto m_draw_walls() -> void;
    auto m_draw_pellets() -> void;
    auto m_draw_hammers() -> void;
    auto m_draw_spawner() -> void;

private:
    vec2 m_pos;

    std::array<std::array<Tile, constants::MAP_HEIGHT>, constants::MAP_WIDTH> m_tiles{};
    std::vector<vec2> m_walls{};
    std::vector<vec2> m_pellets{};
    std::vector<vec2> m_hammers{};
    vec2 m_start_pos{};
    vec2 m_spawner_pos{};

};

}
