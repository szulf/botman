#include "game.hpp"

#include "raygui.h"
#include "raylib.h"
#include "raymath.h"

#include <iostream>
#include <vector>
#include <string>

inline static void reset_game(std::vector<BugData>& bug_datas, RobotData& robot_data, MapData& map_data) {
    u8 bug_i = 1;
    for (auto& bug : bug_datas) {
        bug.tint.a = 255;
        bug.pos = map_data.get_pos_from_grid(map_data.spawner_pos);
        bug.state = BugState::RESPAWNING;
        bug.dead_time = GetTime() + bug_i;
        bug.death_display = false;
        bug_i++;
    }

    robot_data.pos = map_data.get_pos_from_grid(map_data.start_pos);
    robot_data.next_move = Movement::LEFT;
    robot_data.time_between_moves = 0;
    robot_data.movement = {-1, 0};

    robot_data.flip = Flip::LEFT;
    robot_data.texture_accumulator = 0;
}

GameData::GameData() {
    set_state(GameState::START_SCREEN);

    textures.hammer = LoadTexture(ROOT_PATH "/assets/hammer.png");
    textures.portal = LoadTexture(ROOT_PATH "/assets/portal.png");
    textures.pellet = LoadTexture(ROOT_PATH "/assets/gold_coin.png");
    textures.robot_walk = LoadTexture(ROOT_PATH "/assets/robot.png");
    textures.bug_walk = LoadTexture(ROOT_PATH "/assets/bug_test.png");
}

GameData::~GameData() {
    UnloadTexture(textures.hammer);
    UnloadTexture(textures.portal);
    UnloadTexture(textures.pellet);
    UnloadTexture(textures.robot_walk);
    UnloadTexture(textures.bug_walk);
}

void GameData::set_state(GameState new_state) {
    switch (new_state) {
        case GameState::RUNNING: {
            running.map = MapData{{200, 50}};

            // TODO
            // Change this to maybe a constructor on RobotData
            running.robot.pos = running.map.get_pos_from_grid(running.map.start_pos);
            running.robot.next_move = Movement::LEFT;

            running.bugs = std::vector<BugData>{5, running.map.get_pos_from_grid(running.map.spawner_pos)};

            break;
        }

        case GameState::EDIT_MODE:
            edit_mode.map = MapData{{200, 50}};
            break;

        case GameState::EXIT:
            close_window = true;

        default:
            break;
    }

    state = new_state;
}

void start_screen(GameData& game) {
    if (game.start_screen.game_btn) {
        game.set_state(GameState::RUNNING);
    }

    if (game.start_screen.edit_btn) {
        game.set_state(GameState::EDIT_MODE);
    }

    if (game.start_screen.settings_btn) {
        game.set_state(GameState::SETTINGS);
    }

    if (game.start_screen.quit_btn) {
        game.set_state(GameState::EXIT);
    }

    if (game.start_screen.map_selector_btn) {
        game.set_state(GameState::MAP_SELECTOR);
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
            v2 pos = map.get_grid_from_pos(GetMousePosition());
            if (map.in_range(pos)) {
                if (game.edit_mode.chosen_tile == Tile::SPAWNER) {
                    map.set_tile(map.spawner_pos, Tile::EMPTY);
                    map.spawner_pos = pos;
                } else if (game.edit_mode.chosen_tile == Tile::START_POS) {
                    map.set_tile(map.start_pos, Tile::EMPTY);
                    map.start_pos = pos;
                } else if (game.edit_mode.chosen_tile == Tile::PORTAL) {
                    map.set_tile(map.portal_pos[0], Tile::EMPTY);
                    map.portal_pos[0] = pos;
                }

                map.set_tile(pos, game.edit_mode.chosen_tile);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            v2 pos = map.get_grid_from_pos(GetMousePosition());
            if (map.in_range(pos) && game.edit_mode.chosen_tile == Tile::PORTAL) {
                map.set_tile(map.portal_pos[1], Tile::EMPTY);
                map.portal_pos[1] = pos;
                map.set_tile(pos, game.edit_mode.chosen_tile);
            }
        }

        if (IsKeyPressed(KEY_ONE)) {
            game.edit_mode.chosen_tile = Tile::EMPTY;
        } else if (IsKeyPressed(KEY_TWO)) {
            game.edit_mode.chosen_tile = Tile::WALL;
        } else if (IsKeyPressed(KEY_THREE)) {
            game.edit_mode.chosen_tile = Tile::PELLET;
        } else if (IsKeyPressed(KEY_FOUR)) {
            game.edit_mode.chosen_tile = Tile::HAMMER;
        } else if (IsKeyPressed(KEY_FIVE)) {
            game.edit_mode.chosen_tile = Tile::SPAWNER;
        } else if (IsKeyPressed(KEY_SIX)) {
            game.edit_mode.chosen_tile = Tile::START_POS;
        } else if (IsKeyPressed(KEY_SEVEN)) {
            game.edit_mode.chosen_tile = Tile::PORTAL;
        }

        if (game.edit_mode.exit_btn) {
            game.set_state(GameState::START_SCREEN);
        }

        if (game.edit_mode.save_btn) {
            if (
                    map.get_tile(map.spawner_pos) == Tile::SPAWNER &&
                    map.get_tile(map.start_pos) == Tile::START_POS &&
                    (
                     (map.get_tile(map.portal_pos[0]) == Tile::PORTAL && map.get_tile(map.portal_pos[1]) == Tile::PORTAL) ||
                     (map.get_tile(map.portal_pos[0]) != Tile::PORTAL && map.get_tile(map.portal_pos[1]) != Tile::PORTAL)
                    ) &&
                    map.portal_pos[0] != map.portal_pos[1]
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

        map.render(game.textures);

        DrawText(print_tile(game.edit_mode.chosen_tile).data(), 50, 50, 20, BLACK);
        DrawFPS(10, 10);

        game.edit_mode.exit_btn = GuiButton({100, 100, 50, 50}, "go back");
        game.edit_mode.save_btn = GuiButton({100, 200, 50, 50}, "save");

        if (game.edit_mode.show_save_menu) {
            if (GuiTextBox({100, 250, 100, 50}, game.edit_mode.map_name, 128, true)) {
                game.edit_mode.show_save_menu = false;
                map.save(game.edit_mode.map_name);
            }
        }

        EndDrawing();
    }
}

void settings(GameData& game) {
    if (game.settings.back_btn) {
        game.set_state(GameState::START_SCREEN);
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
        game.set_state(GameState::START_SCREEN);
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
                game.set_state(GameState::LOST);
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
            robot.move(Movement::UP, game.dt, map);
        } else if (IsKeyPressed(KEY_DOWN)) {
            robot.move(Movement::DOWN, game.dt, map);
        } else if (IsKeyPressed(KEY_LEFT)) {
            robot.move(Movement::LEFT, game.dt, map);
        } else if (IsKeyPressed(KEY_RIGHT)) {
            robot.move(Movement::RIGHT, game.dt, map);
        } else {
            robot.move(Movement::NONE, game.dt, map);
        }
    }

    // --------
    // GAMEPLAY
    // --------
    {
        robot.collect(map, game);

        for (auto& bug : bugs) {
            bug.move(game.dt, robot, map);

            bug.collide(robot, map);
        }
    }

    // ---------
    // RENDERING
    // ---------
    {
        BeginDrawing();
        ClearBackground(WHITE);

        map.render(game.textures);

        robot.render(map, game.textures);

        for (const auto& bug : bugs) {
            bug.render(map, game.textures);
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
