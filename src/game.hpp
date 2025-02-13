#pragma once

#include "constants.hpp"
#include "map.hpp"
#include "robot.hpp"
#include "bug.hpp"

#include "raylib.h"

#include <vector>

enum GameStateType : u8 {
    GAME_START_SCREEN,
    GAME_EDIT_MODE,
    GAME_SETTINGS,
    GAME_MAP_SELECTOR,
    GAME_RUNNING,
    GAME_WON,
    GAME_LOST,
};

struct GameData {
    GameStateType state{};

    bool close_window{};

    float mean_fps{};

    float dt{};
    float last_frame{};

    struct textures_type {
        Texture2D hammer{};
        Texture2D portal{};
        Texture2D pellet{};
    } textures;

    struct start_screen_type {
        bool game_btn{};
        bool edit_btn{};
        bool settings_btn{};
        bool quit_btn{};
        bool map_selector_btn{};
    } start_screen;

    struct edit_mode_type {
        MapData map{};

        TileType chosen_tile{};

        bool exit_btn{};
        bool save_btn{};
        bool show_save_menu{};
    } edit_mode;

    struct settings_type {
        bool back_btn{};
    } settings;

    struct map_selector_type {
        bool maps_reload{};

        bool exit_btn{};
    } map_selector;

    struct running_type {
        MapData map{};
        RobotData robot{};
        std::vector<BugData> bugs{};
    } running;

};

void set_game_state(GameStateType state, GameData& game_data);

// TODO
// change these names
void start_screen(GameData& game);
// TODO
// give an option to display a grid
// so you know what square you are clicking at
void edit_mode(MapData& map, GameData& game);
void settings(GameData& game);
void map_selector(GameData& game);
void running(std::vector<BugData>& bugs_data, RobotData& robot_data, MapData& map_data, GameData& game);
void won();
void lost();
