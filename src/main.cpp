#include "map.hpp"
#include "robot.hpp"

#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <array>
#include <limits>
#include <string>

const char* print_tile(TileType tile) {
    switch (tile) {
        case TileType::EMPTY:
            return "empty";
        case TileType::WALL:
            return "wall";
        case TileType::PELLET:
            return "pellet";
        case TileType::HAMMER:
            return "hammer";
        case TileType::SPAWNER:
            return "spawner";
        case TileType::START_POS:
            return "start_pos";
    }
}

enum class BugStateType : u8 {
    ALIVE,
    DEAD,
    RESPAWNING,
};

const char* print_bug_state(BugStateType bug_state) {
    switch (bug_state) {
        case BugStateType::ALIVE:
            return "alive";
        case BugStateType::DEAD:
            return "dead";
        case BugStateType::RESPAWNING:
            return "respawning";
    }
}

struct BugData {
    v2 pos{};
    v2 movement{};

    Texture2D texture{};
    u8 texture_frame{};

    BugStateType state{BugStateType::DEAD};
    float dead_time{};

    std::vector<v2> path{};
    v2 last_pos{};
    v2 last_movement{};
};

inline BugData init_bug(const v2& pos, u8 idx) {
    return BugData{
        .pos = pos,
        .texture = LoadTexture(ROOT_PATH "/assets/bug.png"),
        .dead_time = static_cast<float>(GetTime() + idx),
    };
}

struct Node {
    v2 pos;
    v2 parent;

    float f{};
    float g{};
    float h{};

    bool operator==(const Node& other) const {
        return pos == other.pos;
    }
};

std::vector<v2> find_path(const v2& start_grid_pos, const v2& end_grid_pos, const MapData& map_data) {
    if (start_grid_pos == end_grid_pos) {
        return {};
    }

    std::vector<Node> closed_set;
    std::vector<Node> open_set{};
    open_set.push_back({start_grid_pos});

    while (!open_set.empty()) {
        Node q{};
        u32 lowest_cost{std::numeric_limits<u32>::max()};
        for (const auto& n : open_set) {
            if (n.f < lowest_cost) {
                lowest_cost = n.f;
                q = n;
            }
        }
        open_set.erase(std::ranges::find(open_set, Node{q}));

        std::array<Node, 4> successors{
            Node{.pos = {.x = q.pos.x + 1, .y = q.pos.y}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x - 1, .y = q.pos.y}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x, .y = q.pos.y + 1}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x, .y = q.pos.y - 1}, .parent = q.pos},
        };

        for (auto& successor : successors) {
            if (successor.pos == end_grid_pos) {
                closed_set.push_back(q);
                Node curr = successor;
                std::vector<v2> path;

                while (curr.pos != start_grid_pos) {
                    path.push_back(curr.pos);

                    for (const auto& n : closed_set) {
                        if (n == Node{.pos = curr.parent}) {
                            curr = n;
                            break;
                        }
                    }
                }

                std::ranges::reverse(path);

                return path;
            }

            if (map_data.get_tile(successor.pos) == TileType::WALL || map_data.get_tile(successor.pos) == TileType::SPAWNER) {
                continue;
            }

            successor.g = q.g + 1;
            successor.h = std::abs(successor.pos.x - end_grid_pos.x) + std::abs(successor.pos.y - end_grid_pos.y) + rand() % 20;
            successor.f = successor.g + successor.h;

            Node it_os{};
            bool found_os{};
            for (const auto& n : open_set) {
                if (n.pos == successor.pos) {
                    it_os = n;
                    found_os = true;
                    break;
                }
            }
            if (found_os) {
                if (it_os.f < successor.f) {
                    continue;
                } else {
                    open_set.erase(std::ranges::find(open_set, it_os));
                }
            }

            Node it_cs{};
            bool found_cs{};
            for (const auto& n : closed_set) {
                if (n.pos == successor.pos) {
                    it_cs = n;
                    found_cs = true;
                    break;
                }
            }
            if (found_cs) {
                if (it_cs.f < successor.f) {
                    continue;
                }
            } else {
                open_set.push_back(successor);
            }
        }

        closed_set.push_back(q);
    };

    return {};
}

enum class QuadrantType : u8 {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
};

v2 find_furthest_grid_pos(const v2& grid_pos, const MapData& map_data) {
    QuadrantType quadrant = static_cast<QuadrantType>(round(grid_pos.x / map_data.WIDTH) + round(grid_pos.y / map_data.HEIGHT) * 2);

    switch (quadrant) {
        case QuadrantType::TOP_LEFT:
            return {static_cast<float>(map_data.WIDTH - 2), static_cast<float>(map_data.HEIGHT - 2)};
        case QuadrantType::TOP_RIGHT:
            return {1, static_cast<float>(map_data.HEIGHT - 1)};
        case QuadrantType::BOTTOM_LEFT:
            return {static_cast<float>(map_data.WIDTH - 2), 1};
        case QuadrantType::BOTTOM_RIGHT:
            return {1, 1};
    }
}

Rectangle bug_get_rect(const BugData& bug_data, const MapData& map_data) {
    return {bug_data.pos.x - map_data.GRID_WIDTH / 2.0f, bug_data.pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
}

void bug_move(float dt, BugData& bug_data, const RobotData& robot_data, const MapData& map_data) {
    if (bug_data.state == BugStateType::DEAD && GetTime() - bug_data.dead_time < 1) {
        return;
    }

    auto grid_pos = get_grid_from_pos(bug_data.pos, map_data);

    if (in_about_center(bug_data.pos, map_data) && bug_data.last_pos != grid_pos) {
        if (bug_data.state == BugStateType::DEAD) {
            bug_data.path = find_path(grid_pos, map_data.spawner_pos, map_data);
        } else if (robot_data.smashing_mode) {
            bug_data.path = find_path(grid_pos, find_furthest_grid_pos(get_grid_from_pos(robot_data.pos, map_data), map_data), map_data);
        } else {
            bug_data.path = find_path(grid_pos, get_grid_from_pos(robot_data.pos, map_data), map_data);
        }
        if (bug_data.path.size() == 0) {
            bug_data.pos = get_grid_center(bug_data.pos, map_data);
            return;
        }
        bug_data.movement = bug_data.path[0] - grid_pos;
        bug_data.last_pos = grid_pos;
        if (bug_data.movement != bug_data.last_movement) {
            bug_data.pos = get_grid_center(bug_data.pos, map_data);
            bug_data.last_movement = bug_data.movement;
        }
    }

    v2 next_grid_pos = get_grid_from_pos(bug_data.pos, map_data) + bug_data.movement;
    v2 next_pos = get_pos_from_grid(next_grid_pos, map_data);
    if ((map_data.get_tile(next_grid_pos) == TileType::WALL || (map_data.get_tile(next_grid_pos) == TileType::SPAWNER && next_grid_pos != bug_data.path.back())) && CheckCollisionRecs({next_pos.x - map_data.GRID_WIDTH / 2.0f, next_pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, bug_get_rect(bug_data, map_data))) {
        bug_data.pos = get_grid_center(bug_data.pos, map_data);
        return;
    }

    bug_data.pos -= bug_data.movement * dt * MOVEMENT_SPEED * 0.85f;
}

void bug_collide(BugData& bug_data, const RobotData& robot_data, MapData& map_data) {
    if (CheckCollisionRecs(bug_get_rect(bug_data, map_data), robot_get_rect(robot_data, map_data))) {
        if (robot_data.smashing_mode) {
            bug_data.state = BugStateType::DEAD;
            bug_data.dead_time = GetTime();
        } else {
            // TODO
            // change this to taking hearts away, instead of losing the game immediately
            map_data.state = GameStateType::LOST;
        }
    }
}

// TODO
// proper gameplay mechanics
// - losing a heart after colliding in a bug(when not in smashing mode)
// - losing the game after losing all hearts
//
// TODO
// bugs
// - dying and respawning after colliding with the robot(when its in smashing mode)
//   - flashing or playing some animation after death
//   - transparent while returning
// -* spawining in with delay
//
// TODO
// portal to the passage in the middle of the map
//
// TODO 
// edit mode
// - gui for it
// - saving and loading from different named files(on game start still just load from ROOT_PATH "/map.txt")
//
// TODO
// entrance screen
// - play, enter edit mode, change setting(max fps, etc.)
//
// TODO
// better art ahhhhh
//
// TODO
// change map_data to game_data ????
// or just make a new struct game_data and move some variables from map_data to there
//
// TODO
// lower ram usage ????
// i might be wrong but i think this thing takes 400mb of ram to run which is a little insane tbh
int main() {
    srand(time(0));

    InitWindow(1200, 800, "botman");

    MapData map = load_map({200, 50});
    RobotData robot = {
        .pos = get_pos_from_grid(map.start_pos, map),
        .next_move = MovementType::LEFT,
        .texture = LoadTexture(ROOT_PATH "/assets/robot.png"),
    };
    BugData bugs[1] = {
        init_bug(get_pos_from_grid(map.spawner_pos, map), 1),
        // init_bug(get_pos_from_grid(map.spawner_pos, map), 1),
        // init_bug(get_pos_from_grid(map.spawner_pos, map), 2),
        // init_bug(get_pos_from_grid(map.spawner_pos, map), 3),
        // init_bug(get_pos_from_grid(map.spawner_pos, map), 4),
    };

    SetTargetFPS(60);
    float mean_fps{};

    float dt{};
    float last_frame{};
    static bool first = true;
    while (!WindowShouldClose()) {
        mean_fps = (mean_fps + GetFPS()) / 2.0f;

        switch (map.state) {
            case GameStateType::LOST:
            case GameStateType::RUNNING: {
                float current_frame = GetTime();
                dt = last_frame - current_frame;
                last_frame = current_frame;
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

                    render_map(map);

                    render_robot(robot, map);

                    DrawText(std::to_string(map.score).c_str(), 50, 100, 50, BLACK);

                    if (robot.smashing_mode) {
                        DrawText("s", 50, 150, 50, BLACK);
                    }

                    DrawFPS(10, 10);

                    for (const auto& bug_data : bugs) {
                        DrawTexturePro(bug_data.texture, {static_cast<float>(map.GRID_WIDTH * bug_data.texture_frame), 0, static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)}, {bug_data.pos.x, bug_data.pos.y, static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)}, {map.GRID_WIDTH / 2.0f, map.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
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

            // case GameStateType::LOST: {
            //     BeginDrawing();
            //     ClearBackground(WHITE);
            //
            //     DrawText("YOU LOST!", map.pos.x, map.pos.y, 50, BLACK);
            //
            //     EndDrawing();
            //     break;
            // }
        }
    }

    printf("fps: %f\n", mean_fps);

    UnloadTexture(robot.texture);
    for (const auto& bug : bugs) {
        UnloadTexture(bug.texture);
    }
    CloseWindow();
    return 0;
}
