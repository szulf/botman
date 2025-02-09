#include "map.hpp"
#include "robot.hpp"
#include "bug.hpp"

#include "raylib.h"
#include "raymath.h"
#include <ctime>
#include <string>
#include <span>

inline void reset_game(std::span<BugData> bug_datas, RobotData& robot_data, MapData& map_data) {
    for (u8 i = 1; auto& bug : bug_datas) {
        bug.tint.a = 255;
        bug.pos = get_pos_from_grid(map_data.spawner_pos, map_data);
        bug.state = BugStateType::RESPAWNING;
        bug.dead_time = GetTime() + i;
        i++;
    }

    robot_data.pos = get_pos_from_grid(map_data.start_pos, map_data);
    robot_data.next_move = MovementType::LEFT;
    robot_data.time_between_moves = 0;
    robot_data.movement = {-1, 0};

    robot_data.flip = -1;
    robot_data.rotation = RotationType::SIDE;
    robot_data.texture_accumulator = 0;
}


// TODO
// play animations upon death for both bugs and robot
//
// TODO
// entrance screen
// - play, enter edit mode, change setting(max fps, etc.)
//
// TODO
// edit mode
// - gui for it
// - saving and loading from different named files(on game start still just load from ROOT_PATH "/map.txt")
//
// TODO
// art
// - for
//   - robot(animated)
//   - bugs(animated)
//   - lifes
//
// TODO
// music
//
// TODO
// change map_data to game_data ????
// or just make a new struct game_data and move some variables from map_data to there
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

    MapData map = load_map({200, 50});
    RobotData robot = {
        .pos = get_pos_from_grid(map.start_pos, map),
        .next_move = MovementType::LEFT,
        .texture = LoadTexture(ROOT_PATH "/assets/robot_test.png"),
    };

    std::vector<BugData> bugs{
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
        init_bug(get_pos_from_grid(map.spawner_pos, map)),
    };
    Texture2D hammer_texture = LoadTexture(ROOT_PATH "/assets/hammer.png");

    // SetTargetFPS(10);
    float mean_fps{};

    float dt{};
    float last_frame{};
    static bool first = true;
    while (!WindowShouldClose()) {
        mean_fps = (mean_fps + GetFPS()) / 2.0f;

        switch (map.state) {
            case GameStateType::RUNNING: {
                float current_frame = GetTime();
                dt = last_frame - current_frame;
                last_frame = current_frame;

                if (robot.is_dead) {
                    if (GetTime() - robot.dead_delay < 1) {
                        continue;
                    } else {
                        if (robot.lifes == 0) {
                            map.state = GameStateType::LOST;
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
                        robot_move(MovementType::UP, dt, robot, map);
                    } else if (IsKeyPressed(KEY_DOWN)) {
                        robot_move(MovementType::DOWN, dt, robot, map);
                    } else if (IsKeyPressed(KEY_LEFT)) {
                        robot_move(MovementType::LEFT, dt, robot, map);
                    } else if (IsKeyPressed(KEY_RIGHT)) {
                        robot_move(MovementType::RIGHT, dt, robot, map);
                    } else {
                        robot_move(MovementType::NONE, dt, robot, map);
                    }
                }

                // --------
                // GAMEPLAY
                // --------
                {
                    robot_collect(robot, map);

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

                    render_map(map, hammer_texture);

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

            case GameStateType::WON: {
                BeginDrawing();
                ClearBackground(WHITE);

                DrawText("YOU WON!", map.pos.x, map.pos.y, 50, BLACK);

                EndDrawing();
                break;
            }

            case GameStateType::LOST: {
                BeginDrawing();
                ClearBackground(WHITE);

                DrawText("YOU LOST!", map.pos.x, map.pos.y, 50, BLACK);

                EndDrawing();
                break;
            }
        }
    }

    printf("fps: %f\n", mean_fps);

    UnloadTexture(hammer_texture);
    UnloadTexture(robot.texture);
    for (const auto& bug : bugs) {
        UnloadTexture(bug.texture);
    }
    CloseWindow();
    return 0;
}
