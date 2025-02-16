#pragma once

#include "constants.hpp"
#include "map.hpp"
#include "robot.hpp"
#include "bug.hpp"

#include "raylib.h"

#include <vector>

enum class GameState : u8 {
    START_SCREEN,
    EDIT_MODE,
    SETTINGS,
    MAP_SELECTOR,
    RUNNING,
    WON,
    LOST,
    EXIT,
};

struct TexturesType {
    Texture2D hammer{};
    Texture2D portal{};
    Texture2D pellet{};

    Texture2D robot_walk{};

    Texture2D bug_walk{};
};

struct GameData {
public:
    GameData();
    ~GameData();

    // TODO
    // maybe change an operator= overload on GameState enum
    void set_state(GameState state);

public:
    GameState state{};

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

        Tile chosen_tile{};

        char map_name[128];

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

// TODO
// change these names
void start_screen(GameData& game);
void edit_mode(MapData& map, GameData& game);
void settings(GameData& game);
void map_selector(GameData& game);
void running(std::vector<BugData>& bugs, RobotData& robot, MapData& map, GameData& game);
void won();
void lost();
