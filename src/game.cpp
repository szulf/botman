#include "game.hpp"
#include "constants.hpp"

#include "raylib.h"
#include "robot.hpp"
#include <print>

namespace botman
{

Game::Game() : m_map{{600, 100}, ROOT_PATH "map.txt", m_tm}, m_robot{m_map.get_pos_from_grid(m_map.get_start_pos()), "robot.png", m_tm, m_map}
{
    ::SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    ::InitWindow(constants::WIDTH, constants::HEIGHT, constants::GAME_NAME.data());

    ::SetTargetFPS(m_max_fps);
}

Game::~Game()
{
    ::CloseWindow();
}

auto Game::run() -> void
{
    float last_frame{};
    vec2 last_grid_pos{};
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
        if (m_map.in_about_center(m_robot.get_pos()) && !(grid_pos.x == last_grid_pos.x && grid_pos.y == last_grid_pos.y))
        {
            if (m_robot.rotate())
            {
                last_grid_pos = grid_pos;
            }
        }

        m_robot.update_pos(m_delta_time);

        ::BeginDrawing();
        ::ClearBackground(WHITE);

        ::DrawFPS(20, 20);

        m_map.draw();

        m_robot.draw();

        ::EndDrawing();
    }
}

}
