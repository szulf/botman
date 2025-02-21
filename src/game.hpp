#pragma once

#include "constants.hpp"
#include "map.hpp"
#include "robot.hpp"
#include "bug.hpp"
#include "textures.hpp"

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

struct GameData {
public:
    GameData();

    void change_state(GameState state);

public:
    GameState state{};

    float mean_fps{};

    float dt{};
    float last_frame{};

    TexturesType textures;

    bool show_fps{};

    // Absolutely hate passing in GameData to run methods, but dont really know how to change that
    struct StartScreenType {
        bool game_btn{};
        bool edit_btn{};
        bool settings_btn{};
        bool map_selector_btn{};
        bool exit_btn{};

        void run(GameData& game);
    } start_screen;

    struct EditModeType {
        MapData map{};

        Tile chosen_tile{};

        char map_name[128]{};

        bool exit_btn{};
        bool save_btn{};
        bool map_name_textbox{};

        bool show_map_name_textbox{};
        enum class ErrMsgType {
            NONE,
            START_POS,
            SPAWNER,
            PORTALS,
            MAP_NAME,
        } err_msg{};

        void run(GameData& game);
    } edit_mode;

    struct SettingsType {
        bool exit_btn{};

        void run(GameData& game);
    } settings;

    struct MapSelectorType {
        bool maps_reload{};

        bool exit_btn{};

        void run(GameData& game);
    } map_selector;

    struct RunningType {
        MapData map{};
        RobotData robot{};
        std::vector<BugData> bugs{};

        bool first{true};
        bool exit_btn{};

        void run(GameData& game);
    } running;

    struct WonType {
        bool exit_btn{};

        void run(GameData& game);
    } won;

    struct LostType {
        bool exit_btn{};

        void run(GameData& game);
    } lost;

};

