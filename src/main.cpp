#include "robot.hpp"
#include "game.hpp"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <ctime>
#include <iostream>

// TODO
// entrance screen
// - title screen
//
// TODO
// edit mode
// - gui for it
// - saving and loading from different named files(on game start still just load from ROOT_PATH "/map.txt")
// - map files should also store how many bugs are there, how many lifes does the robot have
//
// TODO
// map selector
// - separate button in the main menu
//
// TODO
// art
// - for
//   - robot with hammer (walking/death animations)
//   - bugs (death animations, when entering/leaving the spawner(?))
//   - spawner (hole)
//   - robot smashing bug for title screen
//
// TODO
// music
//
// TODO
// replace all the 'GetTime's with something
// probably accumulators like for animations
//
// TODO maybe
// lower ram usage ????
// i might be wrong but i think this thing takes 400mb of ram to run which is a little insane tbh
// 400mb in debug, 50mb in release
//
// TODO maybe
// would be cool to have an arena allocator and only allocate memory when entering different game states
// no idea how that would work with something like LoadTexture
// just an idea, probably wont do that
//
// TODO
// turns out the ROOT_PATH macro is a pretty bad idea(when you try to send a already compiled version of the game to someone it will not work)
// gotta figure out something else
int main() {
    srand(time(0));

    InitWindow(WIDTH, HEIGHT, "botman");
    SetExitKey(KEY_NULL);

    GameData game{};

    while (!WindowShouldClose() && game.state != GameState::EXIT) {
        game.mean_fps = (game.mean_fps + GetFPS()) / 2.0f;

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

    std::cout << "fps: " << game.mean_fps << std::endl;

    CloseWindow();
    return 0;
}
