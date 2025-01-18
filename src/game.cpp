#include "game.hpp"
#include "constants.hpp"

#include "raylib.h"
#include <print>

namespace botman
{

Game::Game() : m_map{{100, 100}, ROOT_PATH "map.txt"}
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(constants::WIDTH, constants::HEIGHT, constants::GAME_NAME.data());
}

Game::~Game()
{
    CloseWindow();
}

auto Game::run() -> void
{
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        m_map.draw();

        EndDrawing();
    }
}

}
