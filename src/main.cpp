#include "robot.hpp"
#include "game.hpp"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <ctime>

// TODO
// better win and lost screens
//
// TODO
// better running screen(better score display)
//
// TODO
// art
// - for
//   - robot (smashing animation)
//   - spawner (hole)
//   - bugs (death animation, when entering/leaving the hole(?))
//   - robot smashing bug for title screen
//
// TODO
// replace all the 'GetTime's with something
// probably accumulators like for animations
//
// TODO
// turns out the ROOT_PATH macro is a pretty bad idea(when you try to send a already compiled version of the game to someone it will not work)
// gotta figure out something else
int main() {
    srand(time(0));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "botman");
    SetExitKey(KEY_NULL);

    GameData game{};

    while (!WindowShouldClose() && game.state != GameState::EXIT) {
        switch (game.state) {
            case GameState::START_SCREEN:
                game.start_screen.run(game);
                break;

            case GameState::EDIT_MODE:
                game.edit_mode.run(game);
                break;

            case GameState::SETTINGS:
                game.settings.run(game);
                break;

            case GameState::MAP_SELECTOR:
                game.map_selector.run(game);
                break;

            case GameState::RUNNING:
                game.running.run(game);
                break;

            case GameState::WON:
                game.won.run(game);
                break;

            case GameState::LOST:
                game.lost.run(game);
                break;

            // just here to satisfy a warning
            case GameState::EXIT:
                break;
        }
    }

    CloseWindow();
    return 0;
}
