#include "robot.hpp"
#include "game.hpp"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <ctime>

// TODO
// entrance screen
// - play, enter edit mode, change setting(max fps, etc.)
//
// TODO
// *edit mode
// - gui for it
// - saving and loading from different named files(on game start still just load from ROOT_PATH "/map.txt")
// - maybe a map selector when entering game mode, that would look at all .txt files inside of map/ directory
// - map files should also store how many bugs are there, how many lifes does the robot have
//
// TODO
// map selector
// - separate button in the main menu
//
// TODO
// art
// - for
//   - bugs
//   - lifes
//   - walls
//   - spawner
//   - robot with hammer (animations)
//   - deaths (animations)
//   - portals (animations)
//
// TODO
// music
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
int main() {
    srand(time(0));

    InitWindow(WIDTH, HEIGHT, "botman");
    SetExitKey(KEY_NULL);

    GameData game{};

    while (!WindowShouldClose() && !game.close_window) {
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

    printf("fps: %f\n", game.mean_fps);

    CloseWindow();
    return 0;
}
