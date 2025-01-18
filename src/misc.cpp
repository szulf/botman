#include "misc.hpp"
#include "GameState.hpp"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <print>
#include <fstream>
#include <queue>

auto get_grid_from_pos(const Vec2& pos) -> Vec2
{
    return {std::floor(pos.x / game_state.GRID_WIDTH), std::floor(pos.y / game_state.GRID_HEIGHT)};
}

auto get_pos_from_grid(const Vec2& grid_pos) -> Vec2
{
    return {(grid_pos.x * game_state.GRID_WIDTH) + (game_state.GRID_WIDTH / 2.0f), (grid_pos.y * game_state.GRID_HEIGHT) + (game_state.GRID_HEIGHT / 2.0f)};
}

auto get_center_of(const Vec2& pos) -> Vec2
{
    const Vec2 grid_pos = get_grid_from_pos(pos);
    return {(grid_pos.x * game_state.GRID_WIDTH) + (game_state.GRID_WIDTH / 2.0f), (grid_pos.y * game_state.GRID_HEIGHT) + (game_state.GRID_HEIGHT / 2.0f)};
}

auto in_about_center_of_grid(const Vec2& pos) -> bool
{
    const Vec2 about_pos = {std::floor(pos.x), std::floor(pos.y)};
    const Vec2 center_pos = get_center_of(pos);

    for (int i = 0; i <= 5; i++)
    {
        if ((about_pos + Vec2{static_cast<float>(i), 0}) == center_pos || (about_pos + Vec2{static_cast<float>(0), static_cast<float>(i)}) == center_pos)
        {
            return true;
        }
    }

    return false;
}

auto get_opposite_grid_pos(const Vec2& grid_pos) -> Vec2
{
    const Vec2 local_grid_pos = {grid_pos.x - game_state.MAP_POS.x - 1, grid_pos.y - game_state.MAP_POS.y - 1};
    return {game_state.MAP_WIDTH - 2 - local_grid_pos.x + game_state.MAP_POS.x, game_state.MAP_HEIGHT - 2 - local_grid_pos.y + game_state.MAP_POS.y};
}

auto draw_wall(const Vec2& grid_pos) -> void
{
    const Vec2 pos = get_pos_from_grid(grid_pos);
    const Rectangle rect = {pos.x, pos.y, game_state.GRID_WIDTH, game_state.GRID_HEIGHT};
    DrawRectanglePro(rect, {game_state.GRID_WIDTH / 2.0f, game_state.GRID_HEIGHT / 2.0f}, 0.0f, BLACK);
}

auto draw_spawner() -> void
{
    const Vec2 pos = get_pos_from_grid(game_state.spawner_pos);
    const Rectangle rect = {pos.x, pos.y, game_state.GRID_WIDTH, game_state.GRID_HEIGHT};
    DrawRectanglePro(rect, {game_state.GRID_WIDTH / 2.0f, game_state.GRID_HEIGHT / 2.0f}, 0.0f, RED);
}

auto draw_pellet(const Vec2& grid_pos) -> void
{
    const Vec2 pos = get_pos_from_grid(grid_pos);
    const Rectangle rect = {pos.x, pos.y, game_state.PELLET_WIDTH, game_state.PELLET_HEIGHT};
    DrawRectanglePro(rect, {game_state.PELLET_WIDTH / 2.0f, game_state.PELLET_HEIGHT / 2.0f}, 0.0f, BLACK);
}

auto draw_hammer(const Vec2& grid_pos) -> void
{
    const Vec2 pos = get_pos_from_grid(grid_pos);
    const Rectangle rect = {pos.x, pos.y, game_state.EATING_BALL_WIDTH, game_state.EATING_BALL_HEIGHT};
    DrawRectanglePro(rect, {game_state.EATING_BALL_WIDTH / 2.0f, game_state.EATING_BALL_HEIGHT / 2.0f}, 0.0f, RED);
}

auto draw_grid() -> void
{
    for (int16_t i = 0; i < game_state.HEIGHT / game_state.GRID_HEIGHT; i++)
    {
        DrawLineV({0, static_cast<float>(game_state.GRID_HEIGHT * (i + 1))}, {game_state.WIDTH, static_cast<float>(game_state.GRID_HEIGHT * (i + 1))}, BLACK);
    }
    for (int16_t i = 0; i < game_state.WIDTH / game_state.GRID_WIDTH; i++)
    {
        DrawLineV({static_cast<float>(game_state.GRID_WIDTH * (i + 1)), 0}, {static_cast<float>(game_state.GRID_WIDTH * (i + 1)), game_state.HEIGHT}, BLACK);
    }
}

auto draw_boundaries() -> void
{
    for (uint16_t i = game_state.MAP_POS.x; i < game_state.MAP_POS.x + 17; i++)
    {
        draw_wall({static_cast<float>(i), game_state.MAP_POS.y});
        draw_wall({static_cast<float>(i), game_state.MAP_POS.y + 21});
    }
    for (uint16_t i = game_state.MAP_POS.y; i < game_state.MAP_POS.y + 21; i++)
    {
        draw_wall({game_state.MAP_POS.x, static_cast<float>(i)});
        draw_wall({game_state.MAP_POS.x + 16, static_cast<float>(i)});
    }
}

auto load_map() -> void
{
    for (uint16_t i = game_state.MAP_POS.x; i < game_state.MAP_POS.x + 17; i++)
    {
        game_state.map[i][game_state.MAP_POS.y] = Tile::WALL;
        game_state.map[i][game_state.MAP_POS.y + 21] = Tile::WALL;
    }
    for (uint16_t i = game_state.MAP_POS.y; i < game_state.MAP_POS.y + 21; i++)
    {
        game_state.map[game_state.MAP_POS.x][i] = Tile::WALL;
        game_state.map[game_state.MAP_POS.x + 16][i] = Tile::WALL;
    }

    std::ifstream file{ROOT_PATH "/map.txt"};
    std::string line;
    for (uint16_t i = 0; std::getline(file, line); i++)
    {
        for (uint16_t j = 0; j < line.length(); j++)
        {
            switch (line[j])
            {
                case 'X':
                    game_state.walls.push_back({static_cast<float>(j) + game_state.MAP_POS.x, static_cast<float>(i) + game_state.MAP_POS.y});
                    game_state.map[j + game_state.MAP_POS.x][i + game_state.MAP_POS.y] = Tile::WALL;
                    break;
                case 'S':
                    game_state.start_pos = {static_cast<float>(j) + game_state.MAP_POS.x, static_cast<float>(i) + game_state.MAP_POS.y};
                    game_state.map[j + game_state.MAP_POS.x][i + game_state.MAP_POS.y] = Tile::START_POS;
                    break;
                case 'P':
                    game_state.pellets.push_back({static_cast<float>(j) + game_state.MAP_POS.x, static_cast<float>(i) + game_state.MAP_POS.y});
                    game_state.map[j + game_state.MAP_POS.x][i + game_state.MAP_POS.y] = Tile::PELLET;
                    break;
                case 'G':
                    game_state.spawner_pos = {static_cast<float>(j) + game_state.MAP_POS.x, static_cast<float>(i) + game_state.MAP_POS.y};
                    game_state.map[j + game_state.MAP_POS.x][i + game_state.MAP_POS.y] = Tile::SPAWNER;
                    break;
                case 'H':
                    game_state.hammers.push_back({static_cast<float>(j) + game_state.MAP_POS.x, static_cast<float>(i) + game_state.MAP_POS.y});
                    game_state.map[j + game_state.MAP_POS.x][i + game_state.MAP_POS.y] = Tile::HAMMER;
                    break;
            }
        }
    }
}

auto save_map() -> void
{
    std::ofstream file{ROOT_PATH "/map.txt"};
    file << "#################\n";

    for (uint8_t i = 1; i <= game_state.MAP_HEIGHT - 2; i++)
    {
        file << "#";
        for (uint8_t j = 1; j <= game_state.MAP_WIDTH - 2; j++)
        {
            switch (game_state.map[j + game_state.MAP_POS.x][i + game_state.MAP_POS.y])
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

auto find_shortest_path(const Vec2& robot_pos, const Vec2& bug_pos) -> std::vector<Vec2>
{
    const Vec2 pv = {robot_pos.x - game_state.MAP_POS.x - 1, robot_pos.y - game_state.MAP_POS.y - 1};
    const Vec2 bv = {bug_pos.x - game_state.MAP_POS.x - 1, bug_pos.y - game_state.MAP_POS.y - 1};

    constexpr std::array directions{Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1}};
    std::vector<std::vector<int32_t>> dist{game_state.MAP_WIDTH, std::vector(game_state.MAP_HEIGHT, std::numeric_limits<int32_t>::max())};
    std::vector<std::vector<Vec2>> parent(game_state.MAP_WIDTH, std::vector(game_state.MAP_HEIGHT, Vec2{-1, -1}));

    constexpr auto cmp = [](const Node& a, const Node& b) {return a.cost > b.cost; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq;
    pq.push({bv, 0});
    dist[bv.x][bv.y] = 0;

    while (!pq.empty())
    {
        const Node curr = pq.top();
        pq.pop();

        if (curr.pos.x == pv.x && curr.pos.y == pv.y)
        {
            std::vector<Vec2> path;
            for (auto at = pv; at != bv; at = parent[at.x][at.y])
            {
                path.push_back(at);
            }
            std::ranges::reverse(path);
            return path;
        }

        for (const auto& dir : directions)
        {
            const Vec2 nv = {curr.pos.x + dir.x, curr.pos.y + dir.y};

            if ((nv.x >= 0 && nv.x < game_state.MAP_WIDTH && nv.y >= 0 && nv.y < game_state.MAP_HEIGHT && game_state.map[nv.x + game_state.MAP_POS.x + 1][nv.y + game_state.MAP_POS.y + 1] != Tile::WALL && game_state.map[nv.x + game_state.MAP_POS.x + 1][nv.y + game_state.MAP_POS.y + 1] != Tile::SPAWNER) || (nv == pv && game_state.map[nv.x + game_state.MAP_POS.x + 1][nv.y + game_state.MAP_POS.y + 1] == Tile::WALL))
            {
                const int new_cost = curr.cost + 1;

                if (new_cost < dist[nv.x][nv.y])
                {
                    dist[nv.x][nv.y] = new_cost;
                    pq.push({nv, new_cost});
                    parent[nv.x][nv.y] = curr.pos;
                }
            }
        }
    }

    return {};
}
