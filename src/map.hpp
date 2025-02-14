#pragma once

#include "constants.hpp"

#include <vector>

enum TileType : u8 {
    TILE_EMPTY,
    TILE_WALL,
    TILE_PELLET,
    TILE_HAMMER,
    TILE_SPAWNER,
    TILE_START_POS,

    // TODO
    // for now only allowing two portals that link to each other
    // maybe change that in the future
    TILE_PORTAL,
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
    v2 portal_pos[2]{};

    u32 score{};
    u16 pellet_count{};
};

inline TileType get_tile(v2 pos, const MapData& map_data) {
    return map_data.tiles[pos.x + map_data.WIDTH * pos.y];
}

inline TileType get_tile(u32 x, u32 y, const MapData& map_data) {
    return map_data.tiles[x + map_data.WIDTH * y];
}

inline void set_tile(v2 pos, TileType tile, MapData& map_data) {
    map_data.tiles[pos.x + map_data.WIDTH * pos.y] = tile;
}

inline bool in_map_range(const v2& grid_pos, const MapData& map_data) {
    return grid_pos.x > 0 && grid_pos.x < map_data.WIDTH - 1 && grid_pos.y > 0 && grid_pos.y < map_data.HEIGHT - 1;
}

MapData load_map(const v2& map_pos);
void save_map(const char* map_name, const MapData& map_data);
v2 get_grid_from_pos(const v2& pos, const MapData& map_data);
v2 get_pos_from_grid(const v2& grid_pos, const MapData& map_data);
void render_map(const MapData& map_data, const Texture2D& hammer_texture, const Texture2D& potal_texture, const Texture2D& pellet_texture);
v2 get_second_portal_pos(const v2& portal_pos, const MapData& map_data);
const char* print_tile(TileType tile);
