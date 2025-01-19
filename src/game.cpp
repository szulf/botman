#include "game.hpp"
#include "constants.hpp"

#include "raylib.h"
#include "raymath.h"
#include "robot.hpp"
#include <algorithm>
#include <print>

namespace botman
{

Game::Game() : m_map{{600, 100}, ROOT_PATH "map.txt", m_tm}, m_robot{m_map.get_pos_from_grid(m_map.get_start_pos()), "robot.png", m_tm, m_map}
{
    ::SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    ::InitWindow(constants::WIDTH, constants::HEIGHT, constants::GAME_NAME.data());

    ::SetTargetFPS(m_max_fps);

    m_tm.load_texture({"robot.png", "bug.png"});
}

Game::~Game()
{
    ::CloseWindow();
}

auto Game::run() -> void
{
    auto& tiles = m_map.get_tiles();

    float last_frame{};
    // vec2 last_grid_pos{};
    while (!::WindowShouldClose())
    {
        float current_frame{static_cast<float>(GetTime())};
        m_delta_time = last_frame - current_frame;
        last_frame = current_frame;

        if (IsKeyPressed(KEY_UP))
        {
            m_robot.move(Movement::UP);
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            m_robot.move(Movement::DOWN);
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            m_robot.move(Movement::LEFT);
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            m_robot.move(Movement::RIGHT);
        }

        auto grid_pos = m_map.get_grid_from_pos(m_robot.get_pos());
        // if (m_map.in_about_center(m_robot.get_pos()) && !(grid_pos.x == last_grid_pos.x && grid_pos.y == last_grid_pos.y))
        if (m_map.in_about_center(m_robot.get_pos()) && m_robot.get_next_move() != Movement::NONE)
        {
            m_robot.rotate();
            // if (m_robot.rotate())
            // {
            //     last_grid_pos = grid_pos;
            // }
        }

        m_robot.update_pos(m_delta_time);

        if (robot_collides_with_pellet())
        {
            auto& pellets = m_map.get_pellets();
            auto it = std::ranges::find(pellets, grid_pos);
            if (it != pellets.end())
            {
                pellets.erase(it);
                tiles[grid_pos.x][grid_pos.y] = Tile::EMPTY;
                m_score += 10;
            }
        }

        if (robot_collides_with_hammer())
        {
            auto& hammers = m_map.get_hammers();
            auto it = std::ranges::find(hammers, grid_pos);
            if (it != hammers.end())
            {
                hammers.erase(it);
                tiles[grid_pos.x][grid_pos.y] = Tile::EMPTY;
                // TODO
                // set smashing mode to true here
            }
        }
        ::BeginDrawing();
        ::ClearBackground(WHITE);

        ::DrawFPS(20, 20);
        ::DrawText(("score: " + std::to_string(m_score)).c_str(), 100, 400, 50, BLACK);

        m_map.draw();

        m_robot.draw();

        ::EndDrawing();
    }
}

inline auto Game::robot_collides_with_pellet() -> bool
{
    auto grid_pos = m_map.get_grid_from_pos(m_robot.get_pos());
    auto pos = m_map.get_pos_from_grid(grid_pos);

    return m_map.get_tiles()[grid_pos.x][grid_pos.y] == Tile::PELLET && m_robot.collides({pos.x - (constants::PELLET_WIDTH / 2.0f), pos.y - (constants::PELLET_HEIGHT/ 2.0f), constants::PELLET_WIDTH, constants::PELLET_HEIGHT});
}

inline auto Game::robot_collides_with_hammer() -> bool
{
    auto grid_pos = m_map.get_grid_from_pos(m_robot.get_pos());
    auto pos = m_map.get_pos_from_grid(grid_pos);

    return m_map.get_tiles()[grid_pos.x][grid_pos.y] == Tile::HAMMER && m_robot.collides({pos.x - (constants::HAMMER_WIDTH / 2.0f), pos.y - (constants::HAMMER_HEIGHT/ 2.0f), constants::HAMMER_WIDTH, constants::HAMMER_HEIGHT});
}

}
