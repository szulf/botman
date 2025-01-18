#include "GameState.hpp"
#include "Bug.hpp"
#include "Robot.hpp"
#include "misc.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <print>
#include "raylib.h"
#include "raymath.h"

GameState game_state;

// FIX
// movement absolutely breaks below 30fps
// gonna leave it for now tbh

// TODO
// add bug smashing
//
// TODO
// add winning after collecting all pellets on map
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
// might not be necessary anymore
//
// TODO
// maybe add some randomization to bugs movement
// dont really know how
//
// TODO
// make a gui for the map editor
//
// TODO
// make a gui for the entrance screen

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
        find_shortest_path(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[0].get_pos())),
        find_shortest_path(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[1].get_pos())),
        find_shortest_path(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[2].get_pos())),
        find_shortest_path(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[3].get_pos())),
        find_shortest_path(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bugs[4].get_pos())),
    };

    // const float start_time = GetTime();
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
                // reset bugs position here
                // dont know if necessary
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

                // Collecting hammers
                if (game_state.map[pos.x][pos.y] == Tile::HAMMER)
                {
                    game_state.smashing_mode = GetTime() + 5.0f;
                    game_state.map[pos.x][pos.y] = Tile::EMPTY;
                    auto it = std::ranges::find(game_state.hammers, pos);
                    game_state.hammers.erase(it);
                }

                // disabling smashing mode
                if (game_state.smashing_mode <= GetTime())
                {
                    game_state.smashing_mode = 0.0f;
                }
            }
            robot.update_pos();

            for (uint8_t i = 0; auto& bug : bugs)
            {
                // TODO
                // need to change this
                // if (current_frame - start_time > (i + 1) * 2)
                // {
                //     bug.live();
                // }

                if (in_about_center_of_grid(bug.get_pos()))
                {
                    // if in eating mode
                    if (game_state.smashing_mode > GetTime())
                    {
                        bug.move(paths[i]);
                        const Vec2 opposite_pacman_pos = get_opposite_grid_pos(get_grid_from_pos(robot.get_pos()));
                        paths[i] = find_shortest_path(opposite_pacman_pos, get_grid_from_pos(bug.get_pos()));
                    }
                    else
                    {
                        bug.move(paths[i]);
                        paths[i] = find_shortest_path(get_grid_from_pos(robot.get_pos()), get_grid_from_pos(bug.get_pos()));
                    }
                }

                // if in eating mode
                if (game_state.smashing_mode > GetTime())
                {
                    if (robot.collides(bug.get_rect()))
                    {
                        bug.die();
                    }
                }

                bug.update_pos();
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
                    // Place bug spawner
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
                    // Place bug eating ball
                    const Vec2 mouse_pos = GetMousePosition();
                    const Vec2 grid_pos = get_grid_from_pos(mouse_pos);
                    if (game_state.map[grid_pos.x][grid_pos.y] == Tile::EMPTY)
                    {
                        game_state.map[grid_pos.x][grid_pos.y] = Tile::HAMMER;
                        game_state.hammers.emplace_back(grid_pos);
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
                else if (game_state.map[grid_pos.x][grid_pos.y] == Tile::HAMMER)
                {
                    const auto it = std::ranges::find(game_state.hammers, grid_pos);
                    if (it != game_state.pellets.end())
                    {
                        game_state.hammers.erase(it);
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

        for (const auto& hammer : game_state.hammers)
        {
            draw_hammer(hammer);
        }

        draw_grid();

        draw_spawner();

        if (!editing_mode)
        {
            robot.draw();

            for (const auto& bug : bugs)
            {
                bug.draw();

                // if (pacman.collides(bug.get_dest_rect()))
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
