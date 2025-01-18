#include "Robot.hpp"
#include "GameState.hpp"
#include "misc.hpp"
#include "raylib.h"
#include "raymath.h"
#include <print>

Robot::Robot(const Vec2& start_pos, std::filesystem::path sprite_path) : m_pos{get_pos_from_grid(start_pos)}, m_next_move{Movement::LEFT}, m_sprite{LoadTexture(sprite_path.c_str())}
{
    m_src_rect = {0, 0, game_state.GRID_WIDTH, game_state.GRID_HEIGHT};
    m_dest_rect = {m_pos.x, m_pos.y, game_state.GRID_WIDTH, game_state.GRID_HEIGHT};
}

Robot::~Robot()
{
    UnloadTexture(m_sprite);
}

auto Robot::draw() const -> void
{
    DrawTexturePro(m_sprite, m_src_rect, m_dest_rect, {game_state.GRID_WIDTH / 2.0f, game_state.GRID_HEIGHT / 2.0f}, m_rotation, WHITE);
}

auto Robot::move(const Movement& movement) -> void
{
    switch (movement) {
        case Movement::UP:
            if (m_movement == Vec2{0.0f, 1.0f})
            {
                m_movement = {0.0f, -1.0f};
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
                m_src_rect.width = -std::abs(m_src_rect.width);
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
                m_src_rect.width = std::abs(m_src_rect.width);
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

auto Robot::reset_movement() -> void
{
    m_movement = {0.0f, 0.0f};
    m_next_move = Movement::LEFT;
    m_pos = get_pos_from_grid(game_state.start_pos);
}

auto Robot::rotate() -> void
{
    Vec2 next_pos = get_grid_from_pos(m_pos);
    switch (m_next_move) {
        case Movement::UP:
            next_pos.y += -1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {0.0f, -1.0f};
                m_pos = get_center_of(m_pos);
            }
            break;
        case Movement::DOWN:
            next_pos.y += 1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {0.0f, 1.0f};
                m_pos = get_center_of(m_pos);
            }
            break;
        case Movement::LEFT:
            next_pos.x += -1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {-1.0f, 0.0f};
                m_pos = get_center_of(m_pos);
                m_src_rect.width = -std::abs(m_src_rect.width);
            }
            break;
        case Movement::RIGHT:
            next_pos.x += 1.0f;
            if (game_state.map[next_pos.x][next_pos.y] != Tile::WALL && game_state.map[next_pos.x][next_pos.y] != Tile::SPAWNER)
            {
                m_movement = {1.0f, 0.0f};
                m_pos = get_center_of(m_pos);
                m_src_rect.width = std::abs(m_src_rect.width);
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
        m_frame = 0;
    }
}

auto Robot::update_pos() -> void
{
    m_pos += m_movement * (game_state.delta_time * game_state.MOVE_SPEED);
    m_dest_rect.x = m_pos.x;
    m_dest_rect.y = m_pos.y;
    m_src_rect.x = 32 * m_frame;
}

auto Robot::collides(const Rectangle& rect) const -> bool
{
    return CheckCollisionRecs(m_dest_rect, rect);
}
