#include "Bug.hpp"
#include "GameState.hpp"
#include "misc.hpp"
#include "raylib.h"
#include "raymath.h"
#include <print>

Bug::Bug(const Vec2& start_pos, std::filesystem::path sprite_path) : m_pos{get_pos_from_grid(start_pos)}, m_sprite{LoadTexture(sprite_path.c_str())}
{
    m_src_rect = {0.0f, 0.0f, game_state.GRID_WIDTH, game_state.GRID_HEIGHT};
    m_dest_rect = {m_pos.x, m_pos.y, game_state.GRID_WIDTH, game_state.GRID_HEIGHT};
}

Bug::~Bug()
{
    UnloadTexture(m_sprite);
}

auto Bug::draw() const -> void
{
    DrawTexturePro(m_sprite, m_src_rect, m_dest_rect, {game_state.GRID_WIDTH / 2.0f, game_state.GRID_HEIGHT / 2.0f}, 0.0f, WHITE);
}

auto Bug::move(const std::vector<Vec2>& moves) -> void
{
    if (!m_moving)
    {
        return;
    }

    if (moves.empty())
    {
        m_movement = {0, 0};
        return;
    }

    Vec2 grid_pos = get_grid_from_pos(m_pos);
    grid_pos -= game_state.MAP_POS;
    grid_pos.x -= 1;
    grid_pos.y -= 1;
    const Vec2 last_movement = m_movement;
    m_movement = moves[0] - grid_pos;

    const Vec2 next_pos = grid_pos + m_movement;
    if (game_state.map[next_pos.x + game_state.MAP_POS.x + 1][next_pos.y + game_state.MAP_POS.y + 1] == Tile::WALL)
    {
        m_movement = {0, 0};
        m_frame = 0;
    }

    if (last_movement != m_movement)
    {
        m_pos = get_center_of(m_pos);
    }

    if (m_movement == Vec2{1, 0})
    {
        m_src_rect.width = -std::abs(m_src_rect.width);
    }
    else if (m_movement == Vec2{-1, 0})
    {
        m_src_rect.width = std::abs(m_src_rect.width);
    }

    m_frame++;
    if (m_frame > 3)
    {
        m_frame = 0;
    }
}

auto Bug::update_pos() -> void
{
    if (!m_moving)
    {
        return;
    }

    m_pos += m_movement * (game_state.delta_time * (game_state.MOVE_SPEED * 0.85f));
    m_dest_rect.x = m_pos.x;
    m_dest_rect.y = m_pos.y;
    m_src_rect.x = 32 * m_frame;
}
