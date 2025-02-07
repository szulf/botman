#include "map.hpp"
#include "robot.hpp"

#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <ctime>
#include <array>
#include <string>
#include <span>

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
        case TileType::PORTAL:
            return "portal";
    }

    return "";
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

    return "";
}

struct BugData {
    v2 pos{};
    v2 movement{};

    Texture2D texture{};
    u8 texture_frame{};
    Color tint{WHITE};

    BugStateType state{};
    float dead_time{};
    bool death_display{};

    // not proud of this
    float flash_delay{};

    std::vector<v2> path{};
    v2 last_pos{};
    v2 last_movement{};
};

inline BugData init_bug(const v2& pos, u8 idx) {
    return BugData{
        .pos = pos,
        .texture = LoadTexture(ROOT_PATH "/assets/bug.png"),
        .state = BugStateType::RESPAWNING,
        .dead_time = static_cast<float>(GetTime() + idx),
    };
}

Rectangle bug_get_rect(const BugData& bug_data, const MapData& map_data) {
    return {bug_data.pos.x - map_data.GRID_WIDTH / 2.0f, bug_data.pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)};
}

void render_bug(const BugData& bug_data, const MapData& map_data) {
    if (bug_data.death_display) {
        DrawText("100", bug_data.pos.x - 20, bug_data.pos.y - 25, 20, GREEN);
    }

    DrawTexturePro(bug_data.texture, {static_cast<float>(map_data.GRID_WIDTH * bug_data.texture_frame), 0, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {bug_data.pos.x, bug_data.pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, bug_data.tint);
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

            if (get_tile(successor.pos, map_data) == TileType::WALL || get_tile(successor.pos, map_data) == TileType::SPAWNER) {
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

    return {0, 0};
}

void bug_move(float dt, BugData& bug_data, const RobotData& robot_data, const MapData& map_data) {
    if (bug_data.state == BugStateType::DEAD && GetTime() - bug_data.dead_time < 1) {
        bug_data.death_display = true;
        if (GetTime() - bug_data.flash_delay > 0.2) {
            bug_data.tint.a = bug_data.tint.a == 255 ? 80 : 255;
            bug_data.flash_delay = GetTime();
        }
        return;
    }

    if (bug_data.state == BugStateType::DEAD) {
        bug_data.death_display = false;
        bug_data.tint.a = 80;
    }

    if (bug_data.state == BugStateType::DEAD && in_about_center(bug_data.pos, map_data) && get_grid_from_pos(bug_data.pos, map_data) == map_data.spawner_pos) {
        bug_data.state = BugStateType::RESPAWNING;
        bug_data.dead_time = GetTime();
        bug_data.pos = get_grid_center(bug_data.pos, map_data);
        bug_data.tint.a = 255;
        return;
    }

    if (bug_data.state == BugStateType::RESPAWNING && GetTime() - bug_data.dead_time > 1) {
        bug_data.state = BugStateType::ALIVE;
        return;
    }

    if (bug_data.state == BugStateType::RESPAWNING) {
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
    if ((get_tile(next_grid_pos, map_data) == TileType::WALL || (get_tile(next_grid_pos, map_data) == TileType::SPAWNER && next_grid_pos != bug_data.path.back())) && CheckCollisionRecs({next_pos.x - map_data.GRID_WIDTH / 2.0f, next_pos.y - map_data.GRID_HEIGHT / 2.0f, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, bug_get_rect(bug_data, map_data))) {
        bug_data.pos = get_grid_center(bug_data.pos, map_data);
        return;
    }

    bug_data.pos -= bug_data.movement * dt * MOVEMENT_SPEED * 0.85f;
}

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

void bug_collide(BugData& bug_data, RobotData& robot_data, MapData& map_data) {
    if (bug_data.state != BugStateType::ALIVE) {
        return;
    }

    if (CheckCollisionRecs(bug_get_rect(bug_data, map_data), robot_get_rect(robot_data, map_data))) {
        if (robot_data.smashing_mode) {
            bug_data.state = BugStateType::DEAD;
            bug_data.dead_time = GetTime();
            map_data.score += 100;
        } else {
            robot_data.lifes -= 1;
            robot_data.is_dead = true;
            robot_data.dead_delay = GetTime();
        }
    }
}

// TODO
// play animations upon death for both bugs and robot
//
// TODO
// bugs should be able to pathfind and walk through portals
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
// make the hitbox of the robot a little smaller ?????
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
    std::array<BugData, 5> bugs = {
        // IF YOU CHANGE THE NUMBERS IN HERE ALSO CHANGE THE NUMBERS IN THE RESTART CODE
        init_bug(get_pos_from_grid(map.spawner_pos, map), 1),
        init_bug(get_pos_from_grid(map.spawner_pos, map), 2),
        init_bug(get_pos_from_grid(map.spawner_pos, map), 3),
        init_bug(get_pos_from_grid(map.spawner_pos, map), 4),
        init_bug(get_pos_from_grid(map.spawner_pos, map), 5),
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

                    if (IsKeyPressed(KEY_SPACE)) {
                        reset_game(bugs, robot, map);
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
