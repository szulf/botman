#include "map.hpp"
#include "robot.hpp"
#include "bug.hpp"
#include "game.hpp"

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <ctime>
#include <string>
#include <span>
#include <filesystem>

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
// - either when going into game mode, or a separate button in the main menu
//
// TODO
// art
// - for
//   - *bugs
//   - lifes
//   - walls
//   - spawner
//   - robot with hammer
// - animation for
//   - portals
//   - robot death
//
// TODO
// music
//
// TODO
// lower ram usage ????
// i might be wrong but i think this thing takes 400mb of ram to run which is a little insane tbh
// 400mb in debug, 50mb in release
//
// TODO maybe
// would be cool to have an arena allocator and only allocate memory when entering different game states
// no idea how that would work with something like LoadTexture tho
// just an idea tho, probably wont do that
int main() {
    srand(time(0));

    const u16 WIDTH = 1200;
    const u16 HEIGHT = 800;
    InitWindow(WIDTH, HEIGHT, "botman");
    SetExitKey(KEY_NULL);

    GameData game{};
    set_game_state(GAME_START_SCREEN, game);

    // this should not be here
    game.textures.hammer = LoadTexture(ROOT_PATH "/assets/hammer.png");
    game.textures.portal = LoadTexture(ROOT_PATH "/assets/portal.png");
    game.textures.pellet = LoadTexture(ROOT_PATH "/assets/gold_coin.png");

    while (!WindowShouldClose() && !game.close_window) {
        game.mean_fps = (game.mean_fps + GetFPS()) / 2.0f;

        switch (game.state) {
            case GAME_START_SCREEN:
                start_screen(game);
                break;

            case GAME_EDIT_MODE:
                edit_mode(game.edit_mode.map, game);
                break;

            case GAME_SETTINGS:
                settings(game);
                break;

            case GAME_MAP_SELECTOR:
                map_selector(game);
                break;

            case GAME_RUNNING:
                running(game.running.bugs, game.running.robot, game.running.map, game);
                break;

            case GAME_WON:
                won();
                break;

            case GAME_LOST:
                lost();
                break;
        }
    }

    printf("fps: %f\n", game.mean_fps);

    UnloadTexture(game.textures.hammer);
    UnloadTexture(game.textures.portal);
    UnloadTexture(game.textures.pellet);
    UnloadTexture(game.running.robot.texture);
    for (const auto& bug : game.running.bugs) {
        UnloadTexture(bug.texture);
    }
    CloseWindow();
    return 0;
}
