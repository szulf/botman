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
    GAME_EXIT,
};

struct TexturesType {
    Texture2D hammer{};
    Texture2D portal{};
    Texture2D pellet{};

    Texture2D robot_walk{};

    Texture2D bug_walk{};
};

struct GameData {
    GameStateType state{};

    bool close_window{};

    float mean_fps{};

    float dt{};
    float last_frame{};

    TexturesType textures;

    struct StartScreenType {
        bool game_btn{};
        bool edit_btn{};
        bool settings_btn{};
        bool quit_btn{};
        bool map_selector_btn{};
    } start_screen;

    struct EditModeType {
        MapData map{};

        TileType chosen_tile{};

        bool exit_btn{};
        bool save_btn{};
        bool show_save_menu{};
    } edit_mode;

    struct SettingsType {
        bool back_btn{};
    } settings;

    struct MapSelectorType {
        bool maps_reload{};

        bool exit_btn{};
    } map_selector;

    struct RunningType {
        MapData map{};
        RobotData robot{};
        std::vector<BugData> bugs{};
    } running;

};

void init_game(GameData& game);
void close_game(GameData& game);
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
