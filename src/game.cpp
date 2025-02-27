#include "game.hpp"

#include "bug.hpp"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

inline static void reset_game(std::vector<BugData>& bugs, RobotData& robot, MapData& map) {
    robot = RobotData{map.get_pos_from_grid(map.start_pos), robot.lifes};

    bugs = std::vector<BugData>{5, map.get_pos_from_grid(map.spawner_pos)};
    set_bugs_dead_time(bugs);
}

GameData::GameData() : textures{ROOT_PATH "/assets/hammer.png", ROOT_PATH "/assets/gold_coin.png", ROOT_PATH "/assets/spawner.png", ROOT_PATH "/assets/wall.png", ROOT_PATH "/assets/portal.png", ROOT_PATH "/assets/robot.png", ROOT_PATH "/assets/bug.png", ROOT_PATH "/assets/start.png", ROOT_PATH "/assets/empty.png", ROOT_PATH "/assets/heart.png"} {
    change_state(GameState::START_SCREEN);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xffffffff);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x000000ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x29adffff);

    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xffffffff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x303030ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x59ddffff);

    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xffffffff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0xadd8e6ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x59ddffff);

    SetTargetFPS(fps_count);
}

void GameData::change_state(GameState new_state) {
    switch (new_state) {
        case GameState::RUNNING: {
            u16 robot_lifes{};
            u16 bugs_count{};

            load_from_file(ROOT_PATH "/map.txt", running.map, robot_lifes, bugs_count);

            running.robot = RobotData{running.map.get_pos_from_grid(running.map.start_pos), robot_lifes};

            running.bugs = std::vector<BugData>{bugs_count, running.map.get_pos_from_grid(running.map.spawner_pos)};
            set_bugs_dead_time(running.bugs);

            running.first = true;
            break;
        }

        case GameState::EDIT_MODE:
            load_from_file(ROOT_PATH "/map.txt", edit_mode.map, edit_mode.robot_lifes, edit_mode.bugs_count);
            break;

        default:
            break;
    }

    state = new_state;
}

void GameData::StartScreenType::run(GameData& game) {
    {
        if (game_btn) {
            game.change_state(GameState::RUNNING);
        }

        if (edit_btn) {
            game.change_state(GameState::EDIT_MODE);
        }

        if (settings_btn) {
            game.change_state(GameState::SETTINGS);
        }

        if (map_selector_btn) {
            game.change_state(GameState::MAP_SELECTOR);
        }

        if (exit_btn) {
            game.change_state(GameState::EXIT);
        }
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("BOTMAN", WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.1f, 60, WHITE);

        game_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.3f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "start game");

        edit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.4f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "edit mode");

        settings_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.5f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "settings");

        map_selector_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.6f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "map selector");

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "quit");

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

// TODO
// make an option to display a grid
// so you know what square you are clicking at
void GameData::EditModeType::run(GameData& game) {
    float current_frame = GetTime();
    game.dt = game.last_frame - current_frame;
    game.last_frame = current_frame;

    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            v2 pos = map.get_grid_from_pos(GetMousePosition());
            if (map.in_range(pos)) {
                if (chosen_tile == Tile::SPAWNER) {
                    map.set_tile(map.spawner_pos, Tile::EMPTY);
                    map.spawner_pos = pos;
                } else if (chosen_tile == Tile::START_POS) {
                    map.set_tile(map.start_pos, Tile::EMPTY);
                    map.start_pos = pos;
                } else if (chosen_tile == Tile::PORTAL) {
                    map.set_tile(map.portal_pos[0], Tile::EMPTY);
                    map.portal_pos[0] = pos;
                }

                map.set_tile(pos, chosen_tile);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            v2 pos = map.get_grid_from_pos(GetMousePosition());
            if (map.in_range(pos) && chosen_tile == Tile::PORTAL) {
                map.set_tile(map.portal_pos[1], Tile::EMPTY);
                map.portal_pos[1] = pos;
                map.set_tile(pos, chosen_tile);
            }
        }

        if (IsKeyPressed(KEY_ONE)) {
            chosen_tile = Tile::EMPTY;
        } else if (IsKeyPressed(KEY_TWO)) {
            chosen_tile = Tile::WALL;
        } else if (IsKeyPressed(KEY_THREE)) {
            chosen_tile = Tile::PELLET;
        } else if (IsKeyPressed(KEY_FOUR)) {
            chosen_tile = Tile::HAMMER;
        } else if (IsKeyPressed(KEY_FIVE)) {
            chosen_tile = Tile::SPAWNER;
        } else if (IsKeyPressed(KEY_SIX)) {
            chosen_tile = Tile::START_POS;
        } else if (IsKeyPressed(KEY_SEVEN)) {
            chosen_tile = Tile::PORTAL;
        }

        if (exit_btn) {
            game.change_state(GameState::START_SCREEN);
        }

        if (save_btn) {
            if (map.get_tile(map.spawner_pos) != Tile::SPAWNER) {
                err_msg = ErrMsgType::SPAWNER;
            } else if (map.get_tile(map.start_pos) != Tile::START_POS) {
                err_msg = ErrMsgType::START_POS;
            } else if ((map.get_tile(map.portal_pos[0]) == Tile::PORTAL && map.get_tile(map.portal_pos[1]) != Tile::PORTAL) ||
                        (map.get_tile(map.portal_pos[0]) != Tile::PORTAL && map.get_tile(map.portal_pos[1]) == Tile::PORTAL) ||
                        map.portal_pos[0] == map.portal_pos[1]
                    ) {
                err_msg = ErrMsgType::PORTALS;
            } else {
                show_map_name_textbox = true;
                err_msg = ErrMsgType::NONE;
            }
        }

        if (map_name_textbox) {
            if (std::string_view{""} == map_name) {
                err_msg = ErrMsgType::MAP_NAME;
            } else {
                save_to_file(map_name, map, robot_lifes, bugs_count);
                std::memset(map_name, 0, 128);
            }

            show_map_name_textbox = false;
            map_name_textbox = false;
        }

        if (robot_lifes_increase) {
            if (robot_lifes < 9 && robot_lifes >= 1) {
                robot_lifes += 1;
            }
        }

        if (robot_lifes_decrease) {
            if (robot_lifes <= 9 && robot_lifes > 1) {
                robot_lifes -= 1;
            }
        }

        if (bugs_count_increase) {
            if (bugs_count < 9 && bugs_count >= 1) {
                bugs_count += 1;
            }
        }

        if (bugs_count_decrease) {
            if (bugs_count <= 9 && bugs_count > 1) {
                bugs_count -= 1;
            }
        }
    }

    {
        game.textures.portal.progress(game.dt);
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        map.render(game.textures);

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (map.WIDTH * map.GRID_WIDTH),
                    50
                }, "go back");

        DrawText("chosen tile:", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f), WINDOW_HEIGHT * 0.05f, 20, WHITE);

        const Texture2D& chosen_tile_texture = game.textures.get_texture_from_tile(chosen_tile);
        switch (chosen_tile) {
            case Tile::WALL:
                DrawTexturePro(
                        chosen_tile_texture,
                        {
                            0,
                            0,
                            static_cast<float>(game.textures.wall.width),
                            static_cast<float>(game.textures.wall.height)
                        },
                        {
                            ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f) + MeasureText("chosen tile: ", 20),
                            WINDOW_HEIGHT * 0.05f,
                            static_cast<float>(map.GRID_WIDTH),
                            static_cast<float>(map.GRID_HEIGHT)
                        },
                        {0, map.GRID_HEIGHT * 0.25f},
                        0.0f, WHITE
                    );
                break;

            case Tile::PORTAL:
                DrawTexturePro(
                        chosen_tile_texture,
                        {
                            static_cast<float>(game.textures.portal.width * game.textures.portal.frame),
                            0,
                            static_cast<float>(game.textures.portal.width),
                            static_cast<float>(game.textures.portal.height)
                        },
                        {
                            ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f) + MeasureText("chosen tile: ", 20),
                            WINDOW_HEIGHT * 0.05f,
                            static_cast<float>(map.GRID_WIDTH),
                            static_cast<float>(map.GRID_HEIGHT)
                        },
                        {0, map.GRID_HEIGHT * 0.25f},
                        0.0f,
                        WHITE
                    );
                break;

            default:
                DrawTexturePro(
                        chosen_tile_texture,
                        {
                            0,
                            0,
                            static_cast<float>(chosen_tile_texture.width),
                            static_cast<float>(chosen_tile_texture.height)
                        },
                        {
                            ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f) + MeasureText("chosen tile: ", 20),
                            WINDOW_HEIGHT * 0.05f,
                            static_cast<float>(map.GRID_WIDTH),
                            static_cast<float>(map.GRID_HEIGHT)
                        },
                        {0, map.GRID_HEIGHT * 0.25f},
                        0.0f,
                        WHITE
                    );
                break;
        }

        for (u8 i = 0; i <= static_cast<u8>(Tile::PORTAL); i++) {
            DrawText((std::to_string(i + 1) + " ->").c_str(), ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f), (WINDOW_HEIGHT * (0.05f * (i + 3))), 20, WHITE);

            const Texture2D& texture = game.textures.get_texture_from_tile(static_cast<Tile>(i));

            switch (i) {
                case static_cast<u8>(Tile::WALL):
                    DrawTexturePro(texture,
                                {0, 0, static_cast<float>(game.textures.wall.width), static_cast<float>(game.textures.wall.height)},
                                {((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f) + (10 * 6), WINDOW_HEIGHT * (0.05f * (i + 3)), static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)},
                                {map.GRID_WIDTH / 2.0f, map.GRID_HEIGHT * 0.3f},
                                0.0f,
                                WHITE
                            );
                    break;

                case static_cast<u8>(Tile::PORTAL):
                    DrawTexturePro(texture,
                                {static_cast<float>(game.textures.portal.width * game.textures.portal.frame), 0, static_cast<float>(game.textures.portal.width), static_cast<float>(game.textures.portal.height)},
                                {((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f) + (10 * 6), WINDOW_HEIGHT * (0.05f * (i + 3)), static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)},
                                {map.GRID_WIDTH / 2.0f, map.GRID_HEIGHT * 0.3f},
                                0.0f,
                                WHITE
                            );
                    break;

                default:
                    DrawTexturePro(texture, {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)}, {((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f) + (10 * 6), WINDOW_HEIGHT * (0.05f * (i + 3)), static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)}, {map.GRID_WIDTH / 2.0f, map.GRID_HEIGHT * 0.3f}, 0.0f, WHITE);
                    break;
            }
        }

        save_btn = GuiButton({
                    ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f),
                    WINDOW_HEIGHT * (0.05f * (static_cast<u8>(Tile::PORTAL) + 5)),
                    0.4f * WINDOW_WIDTH - 0.5f * (map.WIDTH * map.GRID_WIDTH),
                    50
                }, "save map");

        switch (err_msg) {
            case ErrMsgType::START_POS:
                DrawText("There needs to be one start position.", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f), WINDOW_HEIGHT * (0.05f * (static_cast<u8>(Tile::PORTAL) + 7)), 15, WHITE);
                break;
            case ErrMsgType::SPAWNER:
                DrawText("There needs to be one spawner.", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f), WINDOW_HEIGHT * (0.05f * (static_cast<u8>(Tile::PORTAL) + 7)), 15, WHITE);
                break;
            case ErrMsgType::PORTALS:
                DrawText("There need to be exactly zero\nor two portals.", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f), WINDOW_HEIGHT * (0.05f * (static_cast<u8>(Tile::PORTAL) + 7)), 15, WHITE);
                break;
            case ErrMsgType::MAP_NAME:
                DrawText("Please provide a proper map name.", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f), WINDOW_HEIGHT * (0.05f * (static_cast<u8>(Tile::PORTAL) + 7)), 15, WHITE);
                break;

            case ErrMsgType::NONE:
                break;
        }

        if (show_map_name_textbox) {
            map_name_textbox = GuiTextBox({
                    ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) + (map.WIDTH * map.GRID_WIDTH) + (WINDOW_WIDTH * 0.05f),
                    WINDOW_HEIGHT * (0.05f * (static_cast<u8>(Tile::PORTAL) + 7)),
                    0.4f * WINDOW_WIDTH - 0.5f * (map.WIDTH * map.GRID_WIDTH),
                    50
                }, map_name, 128, true);
        }

        auto robot_lifes_str = std::to_string(robot_lifes) + " ";
        robot_lifes_str.insert(0, " ");

        DrawText("robot lifes:", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - (map.GRID_WIDTH * 2) - MeasureText(" 2 ", 20) - MeasureText("robot lifes: ", 20), WINDOW_HEIGHT * 0.05f, 20, WHITE);
        robot_lifes_decrease = GuiButton(
                {
                    ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - (map.GRID_WIDTH * 2) - MeasureText(" 2 ", 20),
                    (WINDOW_HEIGHT * 0.05f) - (map.GRID_HEIGHT * 0.25f),
                    static_cast<float>(map.GRID_WIDTH),
                    static_cast<float>(map.GRID_HEIGHT)
                },
                "-"
            );
        DrawText(robot_lifes_str.c_str(), ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - map.GRID_WIDTH - MeasureText(" 2 ", 20), WINDOW_HEIGHT * 0.05f, 20, WHITE);
        robot_lifes_increase = GuiButton(
                {
                    ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - map.GRID_WIDTH,
                    (WINDOW_HEIGHT * 0.05f) - (map.GRID_HEIGHT * 0.25f),
                    static_cast<float>(map.GRID_WIDTH),
                    static_cast<float>(map.GRID_HEIGHT)
                },
                "+"
            );

        auto bugs_count_str = std::to_string(bugs_count) + " ";
        bugs_count_str.insert(0, " ");

        DrawText("bugs count:", ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - (map.GRID_WIDTH * 2) - MeasureText(" 2 " , 20) - MeasureText("bugs count: ", 20), WINDOW_HEIGHT * 0.1f, 20, WHITE);
        bugs_count_decrease = GuiButton(
                {
                    ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - (map.GRID_WIDTH * 2) - MeasureText(" 2 ", 20),
                    (WINDOW_HEIGHT * 0.1f) - (map.GRID_HEIGHT * 0.25f),
                    static_cast<float>(map.GRID_WIDTH),
                    static_cast<float>(map.GRID_HEIGHT)
                },
                "-"
            );
        DrawText(bugs_count_str.c_str(), ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - map.GRID_WIDTH - MeasureText(" 2 ", 20), WINDOW_HEIGHT * 0.1f, 20, WHITE);
        bugs_count_increase = GuiButton(
                {
                    ((WINDOW_WIDTH - (map.WIDTH * map.GRID_WIDTH)) * 0.5f) - (WINDOW_WIDTH * 0.05f) - map.GRID_WIDTH,
                    (WINDOW_HEIGHT * 0.1f) - (map.GRID_HEIGHT * 0.25f),
                    static_cast<float>(map.GRID_WIDTH),
                    static_cast<float>(map.GRID_HEIGHT)
                },
                "+"
            );

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

void GameData::SettingsType::run(GameData& game) {
    {
        if (last_fps_count != game.fps_count) {
            SetTargetFPS(game.fps_count);
            last_fps_count = game.fps_count;
        }

        if (exit_btn) {
            game.change_state(GameState::START_SCREEN);
        }
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("show fps:", WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.3f, 20, WHITE);
        GuiCheckBox({
                    WINDOW_WIDTH * 0.05f + MeasureText("show fps: ", 20),
                    WINDOW_HEIGHT * 0.3f - (MapData::GRID_HEIGHT * 0.25f),
                    MapData::GRID_WIDTH,
                    MapData::GRID_HEIGHT
                }, "", &game.show_fps);

        DrawText("fps count:", WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.4f, 20, WHITE);
        GuiSliderBar({
                    WINDOW_WIDTH * 0.05f + MeasureText("fps count: ", 20),
                    WINDOW_HEIGHT * 0.4f - (MapData::GRID_HEIGHT * 0.25f),
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    MapData::GRID_HEIGHT
                }, nullptr, nullptr, &game.fps_count, 30, 500);

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "go back");

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

void GameData::MapSelectorType::run(GameData& game) {
    {
        if (maps_reload) {
            maps_reload = false;
        }

        if (exit_btn) {
            game.change_state(GameState::START_SCREEN);
        }
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "go back");

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

void GameData::RunningType::run(GameData& game) {
    float current_frame = GetTime();
    game.dt = game.last_frame - current_frame;
    game.last_frame = current_frame;

    if (robot.is_dead) {
        if (GetTime() - robot.dead_delay < 1) {
            return;
        } else {
            if (robot.lifes == 0) {
                game.change_state(GameState::LOST);
                return;
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

    {
        if (exit_btn) {
            game.change_state(GameState::START_SCREEN);
        }

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

    {
        if (robot.collect(map, game)) {
            return;
        }

        for (auto& bug : bugs) {
            bug.move(game.dt, robot, map);

            bug.collide(robot, map);
        }
    }

    {
        if (robot.movement != v2{0, 0}) {
            game.textures.robot.progress(game.dt);
        }

        for (u8 bug_idx = 0; const auto& bug : bugs) {
            if (bug.moving) {
                game.textures.bug.progress(bug_idx, game.dt);
            }

            bug_idx++;
        }

        game.textures.portal.progress(game.dt);
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "go back");

        map.render(game.textures);

        robot.render(map, game.textures);

        for (u8 bug_idx = 0; auto& bug : bugs) {
            bug.render(map, game.textures, bug_idx);
            bug_idx++;
        }

        for (u8 i = 0; i < robot.lifes; i++) {
            DrawTexturePro(game.textures.heart, {0, 0, static_cast<float>(game.textures.heart.width), static_cast<float>(game.textures.heart.height)}, {((WINDOW_WIDTH + (map.GRID_WIDTH * map.WIDTH)) * 0.5f) + (WINDOW_WIDTH * 0.05f * ((i % 3) + 1)), WINDOW_HEIGHT * 0.05f * (std::floor(i / 3.0f) + 1), map.GRID_WIDTH * 2.0f, map.GRID_HEIGHT * 2.0f}, {0, 0}, 0.0f, WHITE);
        }

        DrawText(std::to_string(map.score).c_str(), 50, 100, 50, WHITE);

        if (robot.smashing_mode) {
            DrawText("s", 50, 150, 50, WHITE);
        }

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

void GameData::WonType::run(GameData& game) {
    {
        if (exit_btn) {
            game.change_state(GameState::START_SCREEN);
        }
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("YOU WON!", 100, 100, 50, WHITE);

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "go back");

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

void GameData::LostType::run(GameData& game) {
    {
        if (exit_btn) {
            game.change_state(GameState::START_SCREEN);
        }
    }

    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("YOU LOST!", 100, 100, 50, WHITE);

        exit_btn = GuiButton({
                    WINDOW_WIDTH * 0.05f,
                    WINDOW_HEIGHT * 0.9f,
                    0.4f * WINDOW_WIDTH - 0.5f * (MapData::WIDTH * MapData::GRID_WIDTH),
                    50
                }, "go back");

        if (game.show_fps) {
            DrawFPS(10, 10);
        }

        EndDrawing();
    }
}

void load_from_file(std::string_view map_file_name, MapData& map_data, u16& robot_lifes, u16& bugs_count) {
    map_data.load(map_file_name);

    std::ifstream file{map_file_name.data(), std::ios::in};

    for (u8 i = 0; i < 22; i++) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    file >> robot_lifes;
    file >> bugs_count;
}

void save_to_file(std::string_view map_file_name, const MapData& map_data, u16 robot_lifes, u16 bugs_count) {
    map_data.save(map_file_name);

    std::ofstream file{ROOT_PATH "/maps/" + std::string(map_file_name) + ".txt", std::ios::out | std::ios::ate | std::ios::app};

    file << robot_lifes << std::endl;
    file << bugs_count << std::endl;
}
