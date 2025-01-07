#include "Pacman.hpp"
#include "GameState.hpp"
#include "misc.hpp"
#include "raylib.h"
#include "raymath.h"
#include <print>

Pacman::Pacman(const Vec2& start_pos, std::filesystem::path sprite_path) : m_movement {Vec2{0, 0}}, m_pos{get_pos_from_grid(start_pos)}, m_rotation{0.0f}, m_next_move{Movement::LEFT}, m_time_between_moves{0.0f}, m_sprite{LoadTexture(sprite_path.c_str())}
{
    m_src_rect = {0.0f, 0.0f, static_cast<float>(m_sprite.width), static_cast<float>(m_sprite.height)};
    m_dest_rect = {m_pos.x, m_pos.y, static_cast<float>(m_sprite.width), static_cast<float>(m_sprite.height)};
}

Pacman::~Pacman()
{
    UnloadTexture(m_sprite);
}

auto Pacman::draw() const -> void
{
    DrawTexturePro(m_sprite, m_src_rect, m_dest_rect, {m_sprite.width / 2.0f, m_sprite.height / 2.0f}, m_rotation, WHITE);
}

auto Pacman::move(const Movement& movement) -> void
{
    switch (movement) {
        case Movement::UP:
            if (m_movement == Vec2{0.0f, 1.0f})
            {
                m_movement = {0.0f, -1.0f};
                m_rotation = 270.0f;
            }
            else
            {
                m_next_move = Movement::UP;
                m_time_between_moves = GetTime();
            }
            break;

        case Movement::DOWN:
            if (m_movement == Vec2{0.0f, -1.0f})
            {
                m_movement = {0.0f, 1.0f};
                m_rotation = 90.0f;
            }
            else
            {
                m_next_move = Movement::DOWN;
                m_time_between_moves = GetTime();
            }
            break;

        case Movement::LEFT:
            if (m_movement == Vec2{1.0f, 0.0f})
            {
                m_movement = {-1.0f, 0.0f};
                m_rotation = 180.0f;
            }
            else
            {
                m_next_move = Movement::LEFT;
                m_time_between_moves = GetTime();
            }
            break;

        case Movement::RIGHT:
            if (m_movement == Vec2{-1.0f, 0.0f})
            {
                m_movement = {1.0f, 0.0f};
                m_rotation = 0.0f;
            }
            else
            {
                m_next_move = Movement::RIGHT;
                m_time_between_moves = GetTime();
            }
            break;

        default:
            break;
    }
}

auto Pacman::reset_movement() -> void
{
    m_movement = {0.0f, 0.0f};
    m_next_move = Movement::LEFT;
    m_pos = get_pos_from_grid(game_state.start_pos);
}

auto Pacman::rotate() -> void
{
    Vec2 next_pos = get_grid_from_pos(m_pos);
    switch (m_next_move) {
        case Movement::UP:
            next_pos.y += -1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {0.0f, -1.0f};
                m_rotation = 270.0f;
                m_pos = get_center_of(m_pos);
            }
            break;
        case Movement::DOWN:
            next_pos.y += 1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {0.0f, 1.0f};
                m_rotation = 90.0f;
                m_pos = get_center_of(m_pos);
            }
            break;
        case Movement::LEFT:
            next_pos.x += -1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {-1.0f, 0.0f};
                m_rotation = 180.0f;
                m_pos = get_center_of(m_pos);
            }
            break;
        case Movement::RIGHT:
            next_pos.x += 1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {1.0f, 0.0f};
                m_rotation = 0.0f;
                m_pos = get_center_of(m_pos);
            }
            break;
        default:
            break;
    }
    m_next_move = Movement::NONE;

    next_pos = get_grid_from_pos(m_pos);
    next_pos += m_movement;
    if (game_state.map[next_pos.x][next_pos.y] == Tile::WALL || game_state.map[next_pos.x][next_pos.y] == Tile::SPAWNER)
    {
        m_movement = {0.0f, 0.0f};
        m_pos = get_center_of(m_pos);
    }
}

auto Pacman::update_pos() -> void
{
    m_pos += m_movement * (game_state.delta_time * game_state.MOVE_SPEED);
    m_dest_rect.x = m_pos.x;
    m_dest_rect.y = m_pos.y;
}

auto Pacman::collides(const Rectangle& rect) -> bool
{
    return CheckCollisionRecs(m_dest_rect, rect);
}
