#include "map.hpp"
#include "robot.hpp"
#include "bug.hpp"

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <ctime>
#include <string>
#include <span>

inline void reset_game(std::span<BugData> bug_datas, RobotData& robot_data, MapData& map_data) {
    for (u8 i = 1; auto& bug : bug_datas) {
        bug.tint.a = 255;
        bug.pos = get_pos_from_grid(map_data.spawner_pos, map_data);
        bug.state = BUG_RESPAWNING;
        bug.dead_time = GetTime() + i;
        bug.death_display = false;
        i++;
    }

    robot_data.pos = get_pos_from_grid(map_data.start_pos, map_data);
    robot_data.next_move = MOVE_LEFT;
    robot_data.time_between_moves = 0;
    robot_data.movement = {-1, 0};

    robot_data.flip = -1;
    robot_data.texture_accumulator = 0;
}

inline bool in_map_range(const v2& grid_pos, const MapData& map_data) {
    return grid_pos.x > 0 && grid_pos.x < map_data.WIDTH - 1 && grid_pos.y > 0 && grid_pos.y < map_data.HEIGHT - 1;
}

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
int main() {
    srand(time(0));

    const u16 WIDTH = 1200;
    const u16 HEIGHT = 800;
    InitWindow(WIDTH, HEIGHT, "botman");
    SetExitKey(KEY_NULL);

    GameData game{};
    MapData map = load_map({200, 50});
    RobotData robot = {
        .pos = get_pos_from_grid(map.start_pos, map),
        .next_move = MOVE_LEFT,
        .texture = LoadTexture(ROOT_PATH "/assets/robot.png"),
    };

    std::vector<BugData> bugs{
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
    };

    // this should not be here
    Texture2D hammer_texture = LoadTexture(ROOT_PATH "/assets/hammer.png");
    Texture2D portal_texture = LoadTexture(ROOT_PATH "/assets/portal.png");
    Texture2D pellet_texture = LoadTexture(ROOT_PATH "/assets/gold_coin.png");

    bool close_window = false;

    // SetTargetFPS(10);
    float mean_fps{};

    float dt{};
    float last_frame{};
    static bool first = true;
    while (!WindowShouldClose() && !close_window) {
        mean_fps = (mean_fps + GetFPS()) / 2.0f;

        switch (game.state) {
            case GAME_RUNNING: {
                float current_frame = GetTime();
                dt = last_frame - current_frame;
                last_frame = current_frame;

                if (robot.is_dead) {
                    if (GetTime() - robot.dead_delay < 1) {
                        continue;
                    } else {
                        if (robot.lifes == 0) {
                            game.state = GAME_LOST;
                        } else {
                            first = true;
                            reset_game(bugs, robot, map);
                        }
                        robot.is_dead = false;
                    }
                }

                if (first) {
                    dt = 0.0f;
                    first = false;
                }

                // -----
                // INPUT
                // -----
                {
                    if (IsKeyPressed(KEY_UP)) {
                        robot_move(MOVE_UP, dt, robot, map);
                    } else if (IsKeyPressed(KEY_DOWN)) {
                        robot_move(MOVE_DOWN, dt, robot, map);
                    } else if (IsKeyPressed(KEY_LEFT)) {
                        robot_move(MOVE_LEFT, dt, robot, map);
                    } else if (IsKeyPressed(KEY_RIGHT)) {
                        robot_move(MOVE_RIGHT, dt, robot, map);
                    } else {
                        robot_move(MOVE_NONE, dt, robot, map);
                    }
                }

                // --------
                // GAMEPLAY
                // --------
                {
                    robot_collect(robot, map, game);

                    for (auto& bug : bugs) {
                        bug_move(dt, bug, robot, map);

                        bug_collide(bug, robot, map);
                    }
                }

                // ---------
                // RENDERING
                // ---------
                {
                    BeginDrawing();
                    ClearBackground(WHITE);

                    render_map(map, hammer_texture, portal_texture, pellet_texture);

                    render_robot(robot, map);

                    DrawText(std::to_string(map.score).c_str(), 50, 100, 50, BLACK);

                    if (robot.smashing_mode) {
                        DrawText("s", 50, 150, 50, BLACK);
                    }

                    DrawFPS(10, 10);

                    for (const auto& bug_data : bugs) {
                        render_bug(bug_data, map);
                    }

                    EndDrawing();
                }
                break;
            }

            case GAME_WON: {
                BeginDrawing();
                ClearBackground(WHITE);

                DrawText("YOU WON!", map.pos.x, map.pos.y, 50, BLACK);

                EndDrawing();
                break;
            }

            case GAME_LOST: {
                BeginDrawing();
                ClearBackground(WHITE);

                DrawText("YOU LOST!", map.pos.x, map.pos.y, 50, BLACK);

                EndDrawing();
                break;
            }

            case GAME_START_SCREEN: {
                static bool game_btn = false;
                static bool edit_btn = false;
                static bool settings_btn = false;
                static bool quit_btn = false;

                if (game_btn) {
                    game.state = GAME_RUNNING;
                }

                if (edit_btn) {
                    game.state = GAME_EDIT_MODE;
                }

                if (settings_btn) {
                    game.state = GAME_SETTINGS;
                }

                if (quit_btn) {
                    close_window = true;
                }

                BeginDrawing();
                ClearBackground(WHITE);

                DrawText("HELLO!", map.pos.x, map.pos.y, 50, BLACK);

                game_btn = GuiButton({100, 100, 50, 50}, "game");
                edit_btn = GuiButton({200, 100, 50, 50}, "edit mode");
                settings_btn = GuiButton({300, 100, 50, 50}, "settings");
                quit_btn = GuiButton({400, 100, 50, 50}, "quit");

                EndDrawing();
                break;
            }

            // TODO
            // give an option to display a grid
            // so you know what square you are clicking at
            case GAME_EDIT_MODE: {
                static TileType chosen_tile = TILE_EMPTY;
                static bool exit_btn = false;
                static bool save_btn = false;
                static bool show_save_menu = false;

                // -----
                // INPUT
                // -----
                {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        v2 pos = get_grid_from_pos(GetMousePosition(), map);
                        if (in_map_range(pos, map)) {
                            if (chosen_tile == TILE_SPAWNER) {
                                set_tile(map.spawner_pos, TILE_EMPTY, map);
                                map.spawner_pos = pos;
                            } else if (chosen_tile == TILE_START_POS) {
                                set_tile(map.start_pos, TILE_EMPTY, map);
                                map.start_pos = pos;
                            } else if (chosen_tile == TILE_PORTAL) {
                                set_tile(map.portal_pos[0], TILE_EMPTY, map);
                                map.portal_pos[0] = pos;
                            }

                            set_tile(pos, chosen_tile, map);
                        }
                    }

                    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                        v2 pos = get_grid_from_pos(GetMousePosition(), map);
                        if (in_map_range(pos, map) && chosen_tile == TILE_PORTAL) {
                            set_tile(map.portal_pos[1], TILE_EMPTY, map);
                            map.portal_pos[1] = pos;
                            set_tile(pos, chosen_tile, map);
                        }
                    }

                    if (IsKeyPressed(KEY_ONE)) {
                        chosen_tile = TILE_EMPTY;
                    } else if (IsKeyPressed(KEY_TWO)) {
                        chosen_tile = TILE_WALL;
                    } else if (IsKeyPressed(KEY_THREE)) {
                        chosen_tile = TILE_PELLET;
                    } else if (IsKeyPressed(KEY_FOUR)) {
                        chosen_tile = TILE_HAMMER;
                    } else if (IsKeyPressed(KEY_FIVE)) {
                        chosen_tile = TILE_SPAWNER;
                    } else if (IsKeyPressed(KEY_SIX)) {
                        chosen_tile = TILE_START_POS;
                    } else if (IsKeyPressed(KEY_SEVEN)) {
                        chosen_tile = TILE_PORTAL;
                    }

                    if (exit_btn) {
                        game.state = GAME_START_SCREEN;
                        map = load_map({200, 50});
                    }

                    if (save_btn) {
                        if (
                                get_tile(map.spawner_pos, map) == TILE_SPAWNER &&
                                get_tile(map.start_pos, map) == TILE_START_POS &&
                                (
                                 (get_tile(map.portal_pos[0], map) == TILE_PORTAL && get_tile(map.portal_pos[1], map) == TILE_PORTAL) ||
                                 (get_tile(map.portal_pos[0], map) != TILE_PORTAL && get_tile(map.portal_pos[1], map) != TILE_PORTAL)
                                )
                            ) {
                            show_save_menu = true;
                        } else {
                            // TODO
                            // show a message saying there needs to be a spawner, start_pos and two or none portals
                        }
                    }
                }

                // ---------
                // RENDERING
                // ---------
                {
                    BeginDrawing();
                    ClearBackground(WHITE);

                    render_map(map, hammer_texture, portal_texture, pellet_texture);

                    DrawText(print_tile(chosen_tile), 50, 50, 20, BLACK);

                    exit_btn = GuiButton({100, 100, 50, 50}, "go back");
                    save_btn = GuiButton({100, 200, 50, 50}, "save");

                    if (show_save_menu) {
                        char map_name[128];
                        if (GuiTextBox({100, 250, 100, 50}, map_name, 128, true)) {
                            show_save_menu = false;
                            save_map(map_name, map);
                            printf("%s\n", map_name);
                        }
                    }

                    EndDrawing();
                }
                break;
            }

            case GAME_SETTINGS: {
                BeginDrawing();
                ClearBackground(WHITE);

                DrawText("not yet D:", map.pos.x, map.pos.y, 50, BLACK);
                bool back_btn = GuiButton({200, 100, 50, 50}, "go back");

                EndDrawing();

                if (back_btn) {
                    game.state = GAME_START_SCREEN;
                }

                break;
            }
        }
    }

    printf("fps: %f\n", mean_fps);

    UnloadTexture(hammer_texture);
    UnloadTexture(portal_texture);
    UnloadTexture(robot.texture);
    for (const auto& bug : bugs) {
        UnloadTexture(bug.texture);
    }
    CloseWindow();
    return 0;
}
