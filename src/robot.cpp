#include "robot.hpp"

#include "constants.hpp"
#include "movement.hpp"
#include "raylib.h"
#include "raymath.h"
#include <ostream>
#include <print>

namespace botman
{

auto Robot::draw() const -> void
{
    ::DrawTexturePro(m_tm.get_texture(m_texture_id), {static_cast<float>(constants::GRID_WIDTH * m_frame), 0, constants::GRID_WIDTH, constants::GRID_HEIGHT}, {m_pos.x, m_pos.y, constants::GRID_WIDTH, constants::GRID_HEIGHT}, {constants::GRID_WIDTH / 2.0f, constants::GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
}

auto Robot::move(const Movement& movement) -> void
{
    switch (movement) {
        case Movement::UP:
            if (m_movement == vec2{0, 1})
            {
                m_movement = {0, -1};
            }
            else
            {
                m_next_move = movement;
                m_time_between_moves = ::GetTime();
            }
            break;

        case Movement::DOWN:
            if (m_movement == vec2{0, -1})
            {
                m_movement = {0, 1};
            }
            else
            {
                m_next_move = movement;
                m_time_between_moves = ::GetTime();
            }
            break;

        case Movement::LEFT:
            if (m_movement == vec2{-1, 0})
            {
                m_movement = {1, 0};
            }
            else
            {
                m_next_move = movement;
                m_time_between_moves = ::GetTime();
            }
            break;

        case Movement::RIGHT:
            if (m_movement == vec2{1, 0})
            {
                m_movement = {-1, 0};
            }
            else
            {
                m_next_move = movement;
                m_time_between_moves = ::GetTime();
            }
            break;

        default:
            break;
    }
}

auto Robot::rotate() -> bool
{
    if (::GetTime() - m_time_between_moves >= 0.35f)
    {
        m_next_move = Movement::NONE;
        return false;
    }

    auto grid_pos = m_map.get_grid_from_pos(m_pos);
    switch (m_next_move)
    {
        case Movement::LEFT: {
            vec2 next_movement = {-1, 0};
            auto next_pos = grid_pos + next_movement;
            if (m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::WALL || m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::SPAWNER)
            {
                return false;
            }
            m_movement = next_movement;
            center_pos();
            return true;
            break;
        }

        case Movement::RIGHT: {
            vec2 next_movement = {1, 0};
            auto next_pos = grid_pos + next_movement;
            if (m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::WALL || m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::SPAWNER)
            {
                return false;
            }
            m_movement = next_movement;
            center_pos();
            return true;
            break;
        }

        case Movement::UP: {
            vec2 next_movement = {0, -1};
            auto next_pos = grid_pos + next_movement;
            if (m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::WALL || m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::SPAWNER)
            {
                return false;
            }
            m_movement = next_movement;
            center_pos();
            return true;
            break;
        }

        case Movement::DOWN: {
            vec2 next_movement = {0, 1};
            auto next_pos = grid_pos + next_movement;
            if (m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::WALL || m_map.get_tiles()[next_pos.x][next_pos.y] == Tile::SPAWNER)
            {
                return false;
            }
            m_movement = next_movement;
            center_pos();
            return true;
            break;
        }

        default:
            break;
    }

    return false;
}

auto Robot::update_pos(float dt) -> void
{
    auto next_grid_pos = m_map.get_grid_from_pos(m_pos) + m_movement;
    auto next_pos = m_map.get_pos_from_grid(next_grid_pos);

    if (m_map.get_tiles()[next_grid_pos.x][next_grid_pos.y] == Tile::WALL && collides({next_pos.x, next_pos.y, constants::GRID_WIDTH, constants::GRID_HEIGHT}))
    {
        m_movement = {0, 0};
        center_pos();
    }
    else
    {
        m_pos -= m_movement * dt * constants::MOVEMENT_SPEED;
    }
}

auto Robot::collides(const Rectangle& other) -> bool
{
    return ::CheckCollisionRecs({m_pos.x, m_pos.y, constants::GRID_WIDTH, constants::GRID_HEIGHT}, other);
}

auto Robot::center_pos() -> void
{
    m_pos = m_map.get_pos_from_grid(m_map.get_grid_from_pos(m_pos));
}

}
