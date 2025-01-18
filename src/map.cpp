#include "map.hpp"
#include "constants.hpp"
#include "raylib.h"

#include <cmath>
#include <fstream>

namespace botman
{

Map::Map(const vec2& pos, const std::filesystem::path& path) : m_pos{pos}
{
    load_from_file(path);
}

auto Map::draw() -> void
{
    m_draw_grid();
    m_draw_spawner();

    m_draw_walls();
    m_draw_pellets();
    m_draw_hammers();

}

auto Map::load_from_file(const std::filesystem::path& path) -> void
{
    for (u8 i = 0; i < 17; i++)
    {
        m_tiles[i][0] = Tile::WALL;
        m_tiles[i][21] = Tile::WALL;
    }
    for (u8 i = 0; i < 21; i++)
    {
        m_tiles[0][i] = Tile::WALL;
        m_tiles[16][i] = Tile::WALL;
    }

    std::ifstream file{ROOT_PATH "/map.txt"};
    std::string line;
    for (u8 i = 0; std::getline(file, line); i++)
    {
        for (u8 j = 0; j < line.length(); j++)
        {
            switch (line[j])
            {
                case '#':
                case 'X':
                    m_walls.emplace_back(vec2{static_cast<float>(j), static_cast<float>(i)});
                    m_tiles[j][i] = Tile::WALL;
                    break;
                case 'S':
                    m_start_pos = {static_cast<float>(j), static_cast<float>(i)};
                    m_tiles[j][i] = Tile::START_POS;
                    break;
                case 'P':
                    m_pellets.emplace_back(vec2{static_cast<float>(j), static_cast<float>(i)});
                    m_tiles[j][i] = Tile::PELLET;
                    break;
                case 'G':
                    m_spawner_pos = {static_cast<float>(j), static_cast<float>(i)};
                    m_tiles[j][i] = Tile::SPAWNER;
                    break;
                case 'H':
                    m_hammers.push_back({static_cast<float>(j), static_cast<float>(i)});
                    m_tiles[j][i] = Tile::HAMMER;
                    break;
            }
        }
    }
}

// TODO
// check if i and j are supposed to start at 1
auto Map::save_to_file(const std::filesystem::path& path) -> void
{
    std::ofstream file{ROOT_PATH "/map.txt"};
    file << "#################\n";

    for (u8 i = 1; i <= constants::MAP_HEIGHT - 2; i++)
    {
        file << "#";
        for (u8 j = 1; j <= constants::MAP_WIDTH - 2; j++)
        {
            switch (m_tiles[j][i])
            {
                case Tile::WALL:
                    file << "X";
                    break;
                case Tile::START_POS:
                    file << "S";
                    break;
                case Tile::EMPTY:
                    file << ".";
                    break;
                case Tile::PELLET:
                    file << "P";
                    break;
                case Tile::SPAWNER:
                    file << "G";
                    break;
                case Tile::HAMMER:
                    file << "H";
                    break;
            }
        }
        file << "#\n";
    }

    file << "#################\n";
}

auto Map::get_pos_from_grid(const vec2& grid_pos) -> vec2
{
    return {(grid_pos.x * constants::GRID_WIDTH) + (constants::GRID_WIDTH / 2.0f) + m_pos.x, (grid_pos.y * constants::GRID_HEIGHT) + (constants::GRID_HEIGHT / 2.0f) + m_pos.y};
}

auto Map::get_grid_from_pos(const vec2& pos) -> vec2
{
    return {std::floor(pos.x / constants::GRID_WIDTH), std::floor(pos.y / constants::GRID_HEIGHT)};
}

auto Map::m_draw_grid() -> void
{
    for (u8 i = 0; i < constants::MAP_WIDTH + 1; i++)
    {
        DrawLineV({i * constants::GRID_WIDTH + m_pos.x, 0 + m_pos.y}, {i * constants::GRID_WIDTH + m_pos.x, constants::MAP_HEIGHT * constants::GRID_HEIGHT + m_pos.y}, BLACK);
    }

    for (u8 i = 0; i < constants::MAP_HEIGHT + 1; i++)
    {
        DrawLineV({0 + m_pos.x, i * constants::GRID_HEIGHT + m_pos.y}, {constants::MAP_WIDTH * constants::GRID_WIDTH + m_pos.x, i * constants::GRID_HEIGHT + m_pos.y}, BLACK);
    }
}

auto Map::m_draw_walls() -> void
{
    for (const auto& wall : m_walls)
    {
        const vec2 pos = get_pos_from_grid(wall);
        const Rectangle rect = {pos.x, pos.y, constants::GRID_WIDTH, constants::GRID_HEIGHT};
        DrawRectanglePro(rect, {constants::GRID_WIDTH / 2.0f, constants::GRID_HEIGHT / 2.0f}, 0.0f, BLACK);
    }

}

auto Map::m_draw_pellets() -> void
{
    for (const auto& pellet : m_pellets)
    {
        const vec2 pos = get_pos_from_grid(pellet);
        const Rectangle rect = {pos.x, pos.y, constants::PELLET_WIDTH, constants::PELLET_HEIGHT};
        DrawRectanglePro(rect, {constants::PELLET_WIDTH / 2.0f, constants::PELLET_HEIGHT / 2.0f}, 0.0f, BLACK);
    }
}

auto Map::m_draw_hammers() -> void
{
    for (const auto& hammer : m_hammers)
    {
        const vec2 pos = get_pos_from_grid(hammer);
        const Rectangle rect = {pos.x, pos.y, constants::HAMMER_WIDTH, constants::HAMMER_HEIGHT};
        DrawRectanglePro(rect, {constants::HAMMER_WIDTH / 2.0f, constants::HAMMER_HEIGHT / 2.0f}, 0.0f, RED);
    }
}

auto Map::m_draw_spawner() -> void
{
    const vec2 pos = get_pos_from_grid(m_spawner_pos);
    const Rectangle rect = {pos.x, pos.y, constants::GRID_WIDTH, constants::GRID_HEIGHT};
    DrawRectanglePro(rect, {constants::GRID_WIDTH / 2.0f, constants::GRID_HEIGHT / 2.0f}, 0.0f, RED);
}

}
