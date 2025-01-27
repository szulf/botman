#include "constants.hpp"
#include "map.hpp"
#include "robot.hpp"

#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <limits>
#include <print>
#include <string>
#include <unordered_set>


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

struct BugData {
    v2 pos{0, 0};
    v2 movement{0, 0};

    Texture2D texture{};
    u8 texture_frame{0};

    // TODO
    // think of something better than this
    std::vector<v2> path{};
    v2 last_pos{0, 0};
    v2 last_movement{0, 0};
};

inline BugData init_bug(const v2& pos) {
    return BugData{
        .pos = pos,
        .texture = LoadTexture(ROOT_PATH "/assets/bug.png"),
    };
}

struct Node {
    v2 pos;
    v2 parent;

    float f{0};
    float g{0};
    float h{0};

    bool operator==(const Node& other) const {
        return pos == other.pos;
    }
};

struct NodeHash {
    size_t operator()(const Node& n) const {
        size_t combined = std::hash<float>()(n.pos.x);
        combined ^= std::hash<float>()(n.pos.y) + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        combined ^= std::hash<float>()(n.g) + 0x9e3779b9 + (combined << 6) + (combined >> 2);

        return combined;
    }
};

std::vector<v2> find_shortest_path(const v2& start_grid_pos, const v2& end_grid_pos, const MapData& map_data) {
    // TODO
    // idk why closed_set.find() doesnt work
    // it it supposed to be about O(1) which is better than the current implementation
    // I cant figure it out tho, try to change this later

    if (start_grid_pos == end_grid_pos) {
        return {};
    }

    const auto NodeCmp = [](const Node& n1, const Node& n2) { return n1.pos == n2.pos; };
    std::unordered_set<Node, NodeHash, decltype(NodeCmp)> closed_set;
    std::unordered_set<Node, NodeHash, decltype(NodeCmp)> open_set{};
    open_set.insert({start_grid_pos});

    while (!open_set.empty()) {
        Node q{};
        u32 lowest_cost{std::numeric_limits<u32>::max()};
        for (const auto& n : open_set) {
            if (n.f < lowest_cost) {
                lowest_cost = n.f;
                q = n;
            }
        }
        open_set.erase(open_set.find(q));

        std::array<Node, 4> successors{
            Node{.pos = {.x = q.pos.x + 1, .y = q.pos.y}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x - 1, .y = q.pos.y}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x, .y = q.pos.y + 1}, .parent = q.pos},
            Node{.pos = {.x = q.pos.x, .y = q.pos.y - 1}, .parent = q.pos},
        };

        for (auto& successor : successors) {
            if (successor.pos == end_grid_pos) {
                closed_set.insert(q);
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
                    open_set.erase(it_os);
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
                open_set.insert(successor);
            }
        }

        closed_set.insert(q);
    };

    return {};
}

void bug_move(float dt, BugData& bug_data, const RobotData& robot_data, const MapData& map_data) {
    auto grid_pos = get_grid_from_pos(bug_data.pos, map_data);

    if (in_about_center(bug_data.pos, map_data) && bug_data.last_pos != grid_pos) {
        bug_data.path = find_shortest_path(grid_pos, get_grid_from_pos(robot_data.pos, map_data), map_data);
        if (bug_data.path.size() == 0) {
            return;
        }
        bug_data.movement = bug_data.path[0] - grid_pos;
        bug_data.last_pos = grid_pos;
        if (bug_data.movement != bug_data.last_movement) {
            bug_data.pos = get_grid_center(bug_data.pos, map_data);
            bug_data.last_movement = bug_data.movement;
        }
    }

    bug_data.pos -= bug_data.movement * dt * MOVEMENT_SPEED * 0.85f;
}

// TODO
// bugs
//
// TODO later
// edit mode
//
// copy todo from old project
int main() {
    srand(time(0));

    InitWindow(1200, 800, "botman");

    MapData map = load_map({200, 50});
    RobotData robot = {
        .pos = get_pos_from_grid(map.start_pos, map),
        .next_move = MovementType::LEFT,
        .texture = LoadTexture(ROOT_PATH "/assets/robot.png"),
    };
    std::vector<BugData> bugs{5, init_bug(get_pos_from_grid(map.spawner_pos, map))};

    float mean_fps{};
    // float last_time{static_cast<float>(GetTime())};

    float dt{};
    float last_frame{};
    static bool first = true;
    while (!WindowShouldClose()) {
        mean_fps = (mean_fps + GetFPS()) / 2.0f;

        float current_frame = GetTime();
        dt = last_frame - current_frame;
        last_frame = current_frame;
        if (first) {
            dt = 0.0f;
            first = false;
        }

        // ----------------
        // PROCESSING INPUT
        // ----------------
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

        // -------------------
        // SOME GAMEPLAY STUFF
        // -------------------
        {
            robot_collect(robot, map);

            for (auto& bug : bugs) {
                bug_move(dt, bug, robot, map);
            }
        }

        // ---------
        // RENDERING
        // ---------
        {
            BeginDrawing();
            ClearBackground(WHITE);

            render_map(map);

            // for (const auto& grid_pos : path) {
            //     auto pos = get_pos_from_grid(grid_pos, map);
            //     DrawRectangleRec({pos.x - map.GRID_WIDTH / 2.0f, pos.y - map.GRID_HEIGHT / 2.0f, map.GRID_WIDTH, map.GRID_HEIGHT}, GREEN);
            // }

            render_robot(robot, map);

            DrawText(std::to_string(map.score).c_str(), 50, 100, 50, BLACK);

            DrawFPS(10, 10);

            for (const auto& bug_data : bugs) {
                DrawTexturePro(bug_data.texture, {static_cast<float>(map.GRID_WIDTH * bug_data.texture_frame), 0, static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)}, {bug_data.pos.x, bug_data.pos.y, static_cast<float>(map.GRID_WIDTH), static_cast<float>(map.GRID_HEIGHT)}, {map.GRID_WIDTH / 2.0f, map.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
            }

            EndDrawing();
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
