#include "map.hpp"
#include "robot.hpp"

#include "raylib.h"
#include <cmath>
#include <print>
#include <string>

const char* print_movement(MovementType move) {
    switch (move) {
        case MovementType::NONE:
            return "none";
        case MovementType::LEFT:
            return "left";
        case MovementType::RIGHT:
            return "right";
        case MovementType::UP:
            return "up";
        case MovementType::DOWN:
            return "down";
    };
}

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

inline void process_input(float dt, RobotData& robot_data, const MapData& map_data) {
    if (IsKeyPressed(KEY_UP)) {
        robot_move(MovementType::UP, dt, robot_data, map_data);
    } else if (IsKeyPressed(KEY_DOWN)) {
        robot_move(MovementType::DOWN, dt, robot_data, map_data);
    } else if (IsKeyPressed(KEY_LEFT)) {
        robot_move(MovementType::LEFT, dt, robot_data, map_data);
    } else if (IsKeyPressed(KEY_RIGHT)) {
        robot_move(MovementType::RIGHT, dt, robot_data, map_data);
    } else {
        robot_move(MovementType::NONE, dt, robot_data, map_data);
    }
}

struct BugData {
    v2 pos{};

    Texture2D texture{};
    u8 texture_frame{};
};

inline void render(const RobotData& robot_data, const std::vector<BugData>& bugs_data, const MapData& map_data) {
    BeginDrawing();
    ClearBackground(WHITE);

    render_map(map_data);

    render_robot(robot_data, map_data);

    DrawText(std::to_string(map_data.score).c_str(), 50, 100, 50, BLACK);

    DrawFPS(10, 10);

    for (u8 i = 0; const auto& bug_data : bugs_data) {
        DrawTexturePro(bug_data.texture, {static_cast<float>(map_data.GRID_WIDTH * bug_data.texture_frame), 0, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {bug_data.pos.x + (map_data.GRID_WIDTH * i), bug_data.pos.y, static_cast<float>(map_data.GRID_WIDTH), static_cast<float>(map_data.GRID_HEIGHT)}, {map_data.GRID_WIDTH / 2.0f, map_data.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
        i++;
    }

    EndDrawing();
}

// TODO
// bugs
//
// TODO later
// edit mode
//
// copy todo from old project

inline BugData init_bug(const v2& pos) {
    return BugData{
        .pos = pos,
        .texture = LoadTexture(ROOT_PATH "/assets/bug.png"),
    };
}

int main() {
    InitWindow(1200, 800, "botman");

    MapData map = load_map({200, 50});
    RobotData robot = {
        .pos = get_pos_from_grid(map.start_pos, map),
        .next_move = MovementType::LEFT,
        .texture = LoadTexture(ROOT_PATH "/assets/robot.png"),
    };

    std::vector<BugData> bugs{5, init_bug(get_pos_from_grid(map.spawner_pos, map))};

    float fps{};

    float dt{};
    float last_frame{};
    while (!WindowShouldClose()) {
        fps = (fps + GetFPS()) / 2.0f;

        float current_frame = GetTime();
        dt = last_frame - current_frame;
        last_frame = current_frame;

        process_input(dt, robot, map);

        robot_collect(robot, map);

        render(robot, bugs, map);
    }

    std::println("fps: {}", fps);

    UnloadTexture(robot.texture);
    for (const auto& bug : bugs) {
        UnloadTexture(bug.texture);
    }
    CloseWindow();
    return 0;
}
