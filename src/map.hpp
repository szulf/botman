#pragma once

#include "constants.hpp"

#include <vector>

enum class TileType : u8 {
    EMPTY,
    WALL,
    PELLET,
    HAMMER,
    SPAWNER,
    START_POS,
};

enum class GameStateType : u8 {
    RUNNING,
    WON,
    LOST,
};

struct MapData {
    static constexpr u32 WIDTH{17};
    static constexpr u32 HEIGHT{22};

    static constexpr u32 GRID_WIDTH{32};
    static constexpr u32 GRID_HEIGHT{32};

    v2 pos{};

    std::vector<TileType> tiles;
    v2 start_pos{};
    v2 spawner_pos{};

    u32 score{};
    u16 pellet_count{};

    GameStateType state{};

    MapData(const v2& map_pos) : pos{map_pos}, tiles{WIDTH * HEIGHT, TileType::EMPTY} {}

    inline TileType get_tile(v2 pos) const {
        return tiles[pos.x + WIDTH * pos.y];
    }

    inline TileType get_tile(u32 x, u32 y) const {
        return tiles[x + WIDTH * y];
    }

    inline void set_tile(v2 pos, TileType val) {
        tiles[pos.x + WIDTH * pos.y] = val;
    }
};

MapData load_map(const v2& map_pos);
v2 get_grid_from_pos(const v2& pos, const MapData& map_data);
v2 get_pos_from_grid(const v2& grid_pos, const MapData& map_data);
void render_map(const MapData& map_data);
