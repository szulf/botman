#include "robot.hpp"
#include "game.hpp"

#include "raylib.h"
#include "raymath.h"
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

    const u16 WIDTH = 1200;
    const u16 HEIGHT = 800;
    InitWindow(WIDTH, HEIGHT, "botman");
    SetExitKey(KEY_NULL);

    GameData game{};

    while (!WindowShouldClose() && !game.close_window) {
        game.mean_fps = (game.mean_fps + GetFPS()) / 2.0f;

        switch (game.state) {
            case GameState::START_SCREEN:
                start_screen(game);
                break;

            case GameState::EDIT_MODE:
                edit_mode(game.edit_mode.map, game);
                break;

            case GameState::SETTINGS:
                settings(game);
                break;

            case GameState::MAP_SELECTOR:
                map_selector(game);
                break;

            case GameState::RUNNING:
                running(game.running.bugs, game.running.robot, game.running.map, game);
                break;

            case GameState::WON:
                won();
                break;

            case GameState::LOST:
                lost();
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
