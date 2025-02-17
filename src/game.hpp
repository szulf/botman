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
    Texture2D pellet{};
    Texture2D wall{};
    Texture2D spawner{};

    struct PortalTextureType {
        Texture2D texture{};
        static constexpr u8 width{16};
        static constexpr u8 height{16};

        float texture_accumulator{};
        u8 texture_frame{};
    } portal;

    struct RobotTextureType {
        Texture2D texture{};

        static constexpr u8 width{16};
        static constexpr u8 height{16};
    } robot;

    struct BugTextureType {
        Texture2D texture;

        static constexpr u8 width{16};
        static constexpr u8 height{16};
    } bug;

    Texture2D start_pos;
    Texture2D empty;

    const Texture2D& get_texture_from_tile(Tile tile);
};

struct GameData {
public:
    GameData();
    ~GameData();

    void change_state(GameState state);

public:
    GameState state{};

    bool close_window{};

    float mean_fps{};

    float dt{};
    float last_frame{};

    TexturesType textures;

    // Absolutely hate passing in GameData to run methods, but dont really know how to change that
    struct StartScreenType {
        bool game_btn{};
        bool edit_btn{};
        bool settings_btn{};
        bool quit_btn{};
        bool map_selector_btn{};

        void run(GameData& game);
    } start_screen;

    struct EditModeType {
        MapData map{};

        Tile chosen_tile{};

        char map_name[128]{};

        bool exit_btn{};
        bool save_btn{};
        bool show_save_menu{};

        void run(GameData& game);
    } edit_mode;

    struct SettingsType {
        bool back_btn{};

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

