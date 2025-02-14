#include "game.hpp"

#include "raygui.h"

#include <vector>
#include <string>

inline static void reset_game(std::vector<BugData>& bug_datas, RobotData& robot_data, MapData& map_data) {
    u8 bug_i = 1;
    for (auto& bug : bug_datas) {
        bug.tint.a = 255;
        bug.pos = get_pos_from_grid(map_data.spawner_pos, map_data);
        bug.state = BUG_RESPAWNING;
        bug.dead_time = GetTime() + bug_i;
        bug.death_display = false;
        bug_i++;
    }

    robot_data.pos = get_pos_from_grid(map_data.start_pos, map_data);
    robot_data.next_move = MOVE_LEFT;
    robot_data.time_between_moves = 0;
    robot_data.movement = {-1, 0};

    robot_data.flip = FLIP_LEFT;
    robot_data.texture_accumulator = 0;
}

void init_game(GameData& game) {
    set_game_state(GAME_START_SCREEN, game);

    game.textures.hammer = LoadTexture(ROOT_PATH "/assets/hammer.png");
    game.textures.portal = LoadTexture(ROOT_PATH "/assets/portal.png");
    game.textures.pellet = LoadTexture(ROOT_PATH "/assets/gold_coin.png");
    game.textures.robot_walk = LoadTexture(ROOT_PATH "/assets/robot.png");
    game.textures.bug_walk = LoadTexture(ROOT_PATH "/assets/bug_test.png");
}

void close_game(GameData& game) {
    UnloadTexture(game.textures.hammer);
    UnloadTexture(game.textures.portal);
    UnloadTexture(game.textures.pellet);
    UnloadTexture(game.textures.robot_walk);
    UnloadTexture(game.textures.bug_walk);
}

void set_game_state(GameStateType state, GameData& game) {
    switch (state) {
        case GAME_RUNNING: {
            game.running.map = load_map({200, 50});

            game.running.robot.pos = get_pos_from_grid(game.running.map.start_pos, game.running.map);
            game.running.robot.next_move = MOVE_LEFT;

            game.running.bugs = {
                init_bug(get_pos_from_grid(game.running.map.spawner_pos, game.running.map)),
                init_bug(get_pos_from_grid(game.running.map.spawner_pos, game.running.map)),
                init_bug(get_pos_from_grid(game.running.map.spawner_pos, game.running.map)),
                init_bug(get_pos_from_grid(game.running.map.spawner_pos, game.running.map)),
                init_bug(get_pos_from_grid(game.running.map.spawner_pos, game.running.map)),
            };

            break;
        }

        case GAME_EDIT_MODE:
            game.edit_mode.map = load_map({200, 50});
            break;

        case GAME_EXIT:
            game.close_window = true;

        default:
            break;
    }

    game.state = state;
}

void start_screen(GameData& game) {
    if (game.start_screen.game_btn) {
        set_game_state(GAME_RUNNING, game);
    }

    if (game.start_screen.edit_btn) {
        set_game_state(GAME_EDIT_MODE, game);
    }

    if (game.start_screen.settings_btn) {
        set_game_state(GAME_SETTINGS, game);
    }

    if (game.start_screen.quit_btn) {
        set_game_state(GAME_EXIT, game);
    }

    if (game.start_screen.map_selector_btn) {
        set_game_state(GAME_MAP_SELECTOR, game);
    }

    BeginDrawing();
    ClearBackground(WHITE);

    DrawText("HELLO!", 200, 50, 50, BLACK);
    DrawFPS(10, 10);

    game.start_screen.game_btn = GuiButton({100, 100, 50, 50}, "game");
    game.start_screen.edit_btn = GuiButton({200, 100, 50, 50}, "edit mode");
    game.start_screen.settings_btn = GuiButton({300, 100, 50, 50}, "settings");
    game.start_screen.quit_btn = GuiButton({400, 100, 50, 50}, "quit");
    game.start_screen.map_selector_btn = GuiButton({500, 100, 50, 50}, "maps");

    EndDrawing();
}

// TODO
// give an option to display a grid
// so you know what square you are clicking at
void edit_mode(MapData& map, GameData& game) {
    // -----
    // INPUT
    // -----
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            v2 pos = get_grid_from_pos(GetMousePosition(), map);
            if (in_map_range(pos, map)) {
                if (game.edit_mode.chosen_tile == TILE_SPAWNER) {
                    set_tile(map.spawner_pos, TILE_EMPTY, map);
                    map.spawner_pos = pos;
                } else if (game.edit_mode.chosen_tile == TILE_START_POS) {
                    set_tile(map.start_pos, TILE_EMPTY, map);
                    map.start_pos = pos;
                } else if (game.edit_mode.chosen_tile == TILE_PORTAL) {
                    set_tile(map.portal_pos[0], TILE_EMPTY, map);
                    map.portal_pos[0] = pos;
                }

                set_tile(pos, game.edit_mode.chosen_tile, map);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            v2 pos = get_grid_from_pos(GetMousePosition(), map);
            if (in_map_range(pos, map) && game.edit_mode.chosen_tile == TILE_PORTAL) {
                set_tile(map.portal_pos[1], TILE_EMPTY, map);
                map.portal_pos[1] = pos;
                set_tile(pos, game.edit_mode.chosen_tile, map);
            }
        }

        if (IsKeyPressed(KEY_ONE)) {
            game.edit_mode.chosen_tile = TILE_EMPTY;
        } else if (IsKeyPressed(KEY_TWO)) {
            game.edit_mode.chosen_tile = TILE_WALL;
        } else if (IsKeyPressed(KEY_THREE)) {
            game.edit_mode.chosen_tile = TILE_PELLET;
        } else if (IsKeyPressed(KEY_FOUR)) {
            game.edit_mode.chosen_tile = TILE_HAMMER;
        } else if (IsKeyPressed(KEY_FIVE)) {
            game.edit_mode.chosen_tile = TILE_SPAWNER;
        } else if (IsKeyPressed(KEY_SIX)) {
            game.edit_mode.chosen_tile = TILE_START_POS;
        } else if (IsKeyPressed(KEY_SEVEN)) {
            game.edit_mode.chosen_tile = TILE_PORTAL;
        }

        if (game.edit_mode.exit_btn) {
            set_game_state(GAME_START_SCREEN, game);
            map = load_map({200, 50});
        }

        if (game.edit_mode.save_btn) {
            if (
                    get_tile(map.spawner_pos, map) == TILE_SPAWNER &&
                    get_tile(map.start_pos, map) == TILE_START_POS &&
                    (
                     (get_tile(map.portal_pos[0], map) == TILE_PORTAL && get_tile(map.portal_pos[1], map) == TILE_PORTAL) ||
                     (get_tile(map.portal_pos[0], map) != TILE_PORTAL && get_tile(map.portal_pos[1], map) != TILE_PORTAL)
                    )
                ) {
                game.edit_mode.show_save_menu = true;
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

        render_map(map, game.textures.hammer, game.textures.portal, game.textures.pellet);

        DrawText(print_tile(game.edit_mode.chosen_tile), 50, 50, 20, BLACK);
        DrawFPS(10, 10);

        game.edit_mode.exit_btn = GuiButton({100, 100, 50, 50}, "go back");
        game.edit_mode.save_btn = GuiButton({100, 200, 50, 50}, "save");

        if (game.edit_mode.show_save_menu) {
            char map_name[128];
            if (GuiTextBox({100, 250, 100, 50}, map_name, 128, true)) {
                game.edit_mode.show_save_menu = false;
                save_map(map_name, map);
                printf("%s\n", map_name);
            }
        }

        EndDrawing();
    }
}

void settings(GameData& game) {
    if (game.settings.back_btn) {
        set_game_state(GAME_START_SCREEN, game);
    }

    BeginDrawing();
    ClearBackground(WHITE);

    DrawText("not yet D:", 100, 100, 50, BLACK);
    DrawFPS(10, 10);

    game.settings.back_btn = GuiButton({200, 100, 50, 50}, "go back");

    EndDrawing();
}

void map_selector(GameData& game) {
    if (game.map_selector.maps_reload) {
        game.map_selector.maps_reload = false;
    }

    if (game.map_selector.exit_btn) {
        set_game_state(GAME_START_SCREEN, game);
    }

    BeginDrawing();
    ClearBackground(WHITE);

    DrawFPS(10, 10);

    game.map_selector.exit_btn = GuiButton({100, 100, 50, 50}, "go back");

    EndDrawing();
}

void running(std::vector<BugData>& bugs, RobotData& robot, MapData& map, GameData& game) {
    static bool first = true;

    float current_frame = GetTime();
    game.dt = game.last_frame - current_frame;
    game.last_frame = current_frame;

    if (robot.is_dead) {
        if (GetTime() - robot.dead_delay < 1) {
            return;
        } else {
            if (robot.lifes == 0) {
                set_game_state(GAME_LOST, game);
            } else {
                first = true;
                reset_game(bugs, robot, map);
            }
            robot.is_dead = false;
        }
    }

    if (first) {
        game.dt = 0.0f;
        first = false;
    }

    // -----
    // INPUT
    // -----
    {
        if (IsKeyPressed(KEY_UP)) {
            robot_move(MOVE_UP, game.dt, robot, map);
        } else if (IsKeyPressed(KEY_DOWN)) {
            robot_move(MOVE_DOWN, game.dt, robot, map);
        } else if (IsKeyPressed(KEY_LEFT)) {
            robot_move(MOVE_LEFT, game.dt, robot, map);
        } else if (IsKeyPressed(KEY_RIGHT)) {
            robot_move(MOVE_RIGHT, game.dt, robot, map);
        } else {
            robot_move(MOVE_NONE, game.dt, robot, map);
        }
    }

    // --------
    // GAMEPLAY
    // --------
    {
        robot_collect(robot, map, game);

        for (auto& bug : bugs) {
            bug_move(game.dt, bug, robot, map);

            bug_collide(bug, robot, map);
        }
    }

    // ---------
    // RENDERING
    // ---------
    {
        BeginDrawing();
        ClearBackground(WHITE);

        render_map(map, game.textures.hammer, game.textures.portal, game.textures.pellet);

        render_robot(robot, map, game.textures);

        for (const auto& bug_data : bugs) {
            render_bug(bug_data, map, game.textures);
        }

        DrawText(std::to_string(map.score).c_str(), 50, 100, 50, BLACK);

        if (robot.smashing_mode) {
            DrawText("s", 50, 150, 50, BLACK);
        }

        DrawFPS(10, 10);

        EndDrawing();
    }
}

void won() {
    BeginDrawing();
    ClearBackground(WHITE);

    DrawText("YOU WON!", 100, 100, 50, BLACK);
    DrawFPS(10, 10);

    EndDrawing();
}

void lost() {
    BeginDrawing();
    ClearBackground(WHITE);

    DrawText("YOU LOST!", 100, 100, 50, BLACK);
    DrawFPS(10, 10);

    EndDrawing();
}
