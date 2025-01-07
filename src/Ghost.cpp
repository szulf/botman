#include "Ghost.hpp"
#include "GameState.hpp"
#include "misc.hpp"
#include "raylib.h"
#include "raymath.h"
#include <print>

Ghost::Ghost(const Vec2& start_pos, std::filesystem::path sprite_path) : m_pos{get_pos_from_grid(start_pos)}, m_sprite{LoadTexture(sprite_path.c_str())}
{
    m_src_rect = {0.0f, 0.0f, static_cast<float>(m_sprite.width), static_cast<float>(m_sprite.height)};
    m_dest_rect = {m_pos.x, m_pos.y, static_cast<float>(m_sprite.width), static_cast<float>(m_sprite.height)};
}

Ghost::~Ghost()
{
    UnloadTexture(m_sprite);
}

auto Ghost::draw() const -> void
{
    DrawTexturePro(m_sprite, m_src_rect, m_dest_rect, {m_sprite.width / 2.0f, m_sprite.height / 2.0f}, 0.0f, WHITE);
}

auto Ghost::move(const std::vector<Vec2>& moves) -> void
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

    if (last_movement != m_movement)
    {
        m_pos = get_center_of(m_pos);
    }
}

auto Ghost::update_pos() -> void
{
    if (!m_moving)
    {
        return;
    }

    m_pos += m_movement * (game_state.delta_time * (game_state.MOVE_SPEED - 20));
    m_dest_rect.x = m_pos.x;
    m_dest_rect.y = m_pos.y;
}
