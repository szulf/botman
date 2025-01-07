#pragma once

#include "raylib.h"

typedef Vector2 Vec2;

enum class Movement
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE,
};

enum class Tile
{
    EMPTY,
    WALL,
    PELLET,
    START_POS,
    SPAWNER,
};

auto get_grid_from_pos(const Vec2& pos) -> Vec2;

auto get_pos_from_grid(const Vec2& pos) -> Vec2;

auto get_center_of(const Vec2& pos) -> Vec2;

auto in_about_center_of_grid(const Vec2& pos) -> bool;

auto draw_wall(const Vec2& grid_pos) -> void;

auto draw_spawner() -> void;

auto draw_pellet(const Vec2& grid_pos) -> void;

auto draw_grid() -> void;

auto draw_boundaries() -> void;

auto load_map() -> void;

auto save_map() -> void;
