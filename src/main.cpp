#include "GameState.hpp"
#include "Bug.hpp"
#include "Robot.hpp"
#include "misc.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <print>
#include <queue>
#include "raylib.h"
#include "raymath.h"

GameState game_state;

// FIX
// movement absolutely breaks below 30fps
// gonna leave it for now tbh

// TODO
// add ghost eating
//
// TODO
// add winning after eating all pellets on map
//
// TODO
// Add a portal to the passage in the middle of the map
// represented as numbers in the map editor
// dont really know how to do this
//
// TODO
// think of a complete movement remake
// so that it works on all fps
//
// TODO
// maybe make this easier somehow
//
// TODO
// maybe add some randomization to ghosts movement
// dont really know how

// TODO
// move this
struct Node
{
    Vec2 pos;
    int32_t cost;
};

auto dijkstra(const Vec2& pacman_pos, const Vec2& ghost_pos) -> std::vector<Vec2>
{
    const Vec2 pv = {pacman_pos.x - game_state.MAP_POS.x - 1, pacman_pos.y - game_state.MAP_POS.y - 1};
    const Vec2 gv = {ghost_pos.x - game_state.MAP_POS.x - 1, ghost_pos.y - game_state.MAP_POS.y - 1};

    constexpr std::array directions{Vec2{-1, 0}, Vec2{1, 0}, Vec2{0, -1}, Vec2{0, 1}};
    std::vector<std::vector<int32_t>> dist(game_state.MAP_WIDTH, std::vector(game_state.MAP_HEIGHT, std::numeric_limits<int32_t>::max()));
    std::vector<std::vector<Vec2>> parent(game_state.MAP_WIDTH, std::vector(game_state.MAP_HEIGHT, Vec2{-1, -1}));

    constexpr auto cmp = [](const Node& a, const Node& b) {return a.cost > b.cost; };
    std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq;
    pq.push({gv, 0});
    dist[gv.x][gv.y] = 0;

    while (!pq.empty())
    {
        const Node curr = pq.top();
        pq.pop();

        if (curr.pos.x == pv.x && curr.pos.y == pv.y)
        {
            std::vector<Vec2> path;
            for (auto at = pv; at != gv; at = parent[at.x][at.y])
            {
                path.push_back(at);
            }
            std::ranges::reverse(path);
            return path;
        }

        for (const auto& dir : directions)
        {
            const Vec2 nv = {curr.pos.x + dir.x, curr.pos.y + dir.y};

            if (nv.x >= 0 && nv.x < game_state.MAP_WIDTH && nv.y >= 0 && nv.y < game_state.MAP_HEIGHT && game_state.map[nv.x + game_state.MAP_POS.x + 1][nv.y + game_state.MAP_POS.y + 1] != Tile::WALL && game_state.map[nv.x + game_state.MAP_POS.x + 1][nv.y + game_state.MAP_POS.y + 1] != Tile::SPAWNER)
            {
                const int new_cost = curr.cost + 1;

                if (new_cost < dist[nv.x][nv.y])
                {
                    dist[nv.x][nv.y] = new_cost;
                    pq.push({nv, new_cost});
                    parent[nv.x][nv.y] = curr.pos;
                }
            }
        }
    }

    return {};
}

auto main() -> int
{
#if NDEBUG
    std::println("pacman in release mode");
#else
    std::println("pacman in debug mode");
#endif

    InitWindow(game_state.WIDTH, game_state.HEIGHT, "MazeMuncher");

    load_map();

    Robot robot{game_state.start_pos, ROOT_PATH "/assets/robot.png"};
    std::array bugs = {
        Bug{game_state.spawner_pos, ROOT_PATH "/assets/bug.png"},
        Bug{game_state.spawner_pos, ROOT_PATH "/assets/bug.png"},
        Bug{game_state.spawner_pos, ROOT_PATH "/assets/bug.png"},
        Bug{game_state.spawner_pos, ROOT_PATH "/assets/bug.png"},
        Bug{game_state.spawner_pos, ROOT_PATH "/assets/bug.png"},
    };

    bool editing_mode = false;

    float last_frame = 0.0f;

    SetTargetFPS(game_state.FPS);

    auto paths = std::array{
        dijkstra(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[0].get_pos())),
        dijkstra(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[1].get_pos())),
        dijkstra(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[2].get_pos())),
        dijkstra(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[3].get_pos())),
        dijkstra(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[4].get_pos())),
    };

    const float start_time = GetTime();
    while (!WindowShouldClose())
    {
        const float current_frame = GetTime();
        game_state.delta_time = current_frame - last_frame;
        last_frame = current_frame;

        if (!game_state.freeze)
        {
            if (IsKeyPressed(KEY_UP))
            {
                robot.move(Movement::UP);
            }
            else if (IsKeyPressed(KEY_DOWN))
            {
                robot.move(Movement::DOWN);
            }
            else if (IsKeyPressed(KEY_LEFT))
            {
                robot.move(Movement::LEFT);
            }
            else if (IsKeyPressed(KEY_RIGHT))
            {
                robot.move(Movement::RIGHT);
            }
        }
        if (IsKeyPressed(KEY_E))
        {
            if (editing_mode)
            {
                // TODO
                // reset ghosts position here
                std::ranges::find(game_state.walls, Vec2{1, 2});
                editing_mode = false;
                robot.reset_movement();
                save_map();
            }
            else
            {
                editing_mode = true;
            }
        }
        if (IsKeyPressed(KEY_SPACE))
        {
            // TODO
            // reset the map here
            game_state.freeze = false;
        }

        if (!game_state.freeze && !editing_mode)
        {
            if (in_about_center_of_grid(robot.get_pos()))
            {
                // TODO
                // maybe change this function name
                // and separate wall check
                robot.rotate();

                // Collecting pellets
                Vec2 pos = get_grid_from_pos(robot.get_pos());
                if (game_state.map[pos.x][pos.y] == Tile::PELLET)
                {
                    game_state.score += 10;
                    game_state.map[pos.x][pos.y] = Tile::EMPTY;
                    auto it = std::ranges::find(game_state.pellets, pos);
                    game_state.pellets.erase(it);
                }

                // Collecting eating balls
                if (game_state.map[pos.x][pos.y] == Tile::EATING_BALL)
                {
                    game_state.eating_mode = GetTime() + 5.0f;
                    game_state.map[pos.x][pos.y] = Tile::EMPTY;
                    auto it = std::ranges::find(game_state.eating_balls, pos);
                    game_state.eating_balls.erase(it);
                }

                // disabling eating mode
                if (game_state.eating_mode <= GetTime())
                {
                    game_state.eating_mode = 0.0f;
                }
            }
            robot.update_pos();

            for (uint8_t i = 0; auto& ghost : bugs)
            {
                if (current_frame - start_time > (i + 1) * 2)
                {
                    ghost.start_moving();
                }

                if (in_about_center_of_grid(ghost.get_pos()))
                {
                    // if in eating mode
                    if (game_state.eating_mode > GetTime())
                    {
                        ghost.move(paths[i]);
                        const Vec2 opposite_pacman_pos = get_opposite_grid_pos(get_grid_from_pos(robot.get_pos()));
                        paths[i] = dijkstra(get_grid_from_pos(opposite_pacman_pos), get_grid_from_pos(ghost.get_pos()));
                    }
                    else
                    {
                        ghost.move(paths[i]);
                        paths[i] = dijkstra(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(ghost.get_pos()));
                    }
                }
                ghost.update_pos();
                i++;
            }
        }

        if (editing_mode)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    // Place starting player square
                    const Vec2 mouse_pos = GetMousePosition();
                    const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                    if (game_state.map[grid_pos.x][grid_pos.y] == Tile::EMPTY)
                    {
                        game_state.map[game_state.start_pos.x][game_state.start_pos.y] = Tile::EMPTY;
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::START_POS;
                        game_state.start_pos = grid_pos;
                    }
                }
                else if (IsKeyDown(KEY_LEFT_SHIFT))
                {
                    // Place pellets
                    const Vec2 mouse_pos = GetMousePosition();
                    const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                    if (game_state.map[grid_pos.x][grid_pos.y] == Tile::EMPTY)
                    {
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::PELLET;
                        game_state.pellets.emplace_back(grid_pos);
                    }
                }
                else if (IsKeyDown(KEY_LEFT_ALT))
                {
                    // Place ghost spawner
                    const Vec2 mouse_pos = GetMousePosition();
                    const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                    if (game_state.map[grid_pos.x][grid_pos.y] == Tile::EMPTY)
                    {
                        game_state.map[game_state.spawner_pos.x][game_state.spawner_pos.y] = Tile::EMPTY;
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::SPAWNER;
                        game_state.spawner_pos = grid_pos;
                    }
                }
                else if (IsKeyDown(KEY_SPACE))
                {
                    // Place ghost eating ball
                    const Vec2 mouse_pos = GetMousePosition();
                    const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                    if (game_state.map[grid_pos.x][grid_pos.y] == Tile::EMPTY)
                    {
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::EATING_BALL;
                        game_state.eating_balls.emplace_back(grid_pos);
                    }
                }
                else
                {
                    // Place wall
                    const Vec2 mouse_pos = GetMousePosition();
                    const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                    if (game_state.map[grid_pos.x][grid_pos.y] == Tile::EMPTY)
                    {
                        game_state.walls.push_back(grid_pos);
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::WALL;
                    }
                }
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                // Remove wall and pellets
                const Vec2 mouse_pos = GetMousePosition();
                const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                if (game_state.map[grid_pos.x][grid_pos.y] == Tile::WALL)
                {
                    const auto it = std::ranges::find(game_state.walls, grid_pos);
                    if (it != game_state.walls.end())
                    {
                        game_state.walls.erase(it);
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::EMPTY;
                    }
                }
                else if (game_state.map[grid_pos.x][grid_pos.y] == Tile::PELLET)
                {
                    const auto it = std::ranges::find(game_state.pellets, grid_pos);
                    if (it != game_state.pellets.end())
                    {
                        game_state.pellets.erase(it);
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::EMPTY;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        draw_boundaries();

        for (const auto& wall : game_state.walls)
        {
            draw_wall(wall);
        }

        for (const auto& pellet : game_state.pellets)
        {
            draw_pellet(pellet);
        }

        for (const auto& eating_ball : game_state.eating_balls)
        {
            draw_eating_ball(eating_ball);
        }

        draw_grid();

        draw_spawner();

        if (!editing_mode)
        {
            robot.draw();

            for (const auto& ghost : bugs)
            {
                ghost.draw();

                // if (pacman.collides(ghost.get_dest_rect()))
                // {
                //     game_state.freeze = true;
                //     DrawText("you lose!", 200, 200, 50, BLACK);
                // }
            }
        }


        DrawFPS(10, 10);
        DrawText(std::to_string(game_state.score).c_str(), 100, 100, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
