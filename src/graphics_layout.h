#ifndef GRAPHICS_LAYOUT_H
#define GRAPHICS_LAYOUT_H

/*
 * Sprite slots.
 */

#define GFX_PLAYER_SPRITE_ID 0u

#define GFX_PLAYER_SHOT_FIRST_SPRITE_ID 1u
#define GFX_PLAYER_SHOT_SPRITE_COUNT 8u

#define GFX_ENEMY_FIRST_SPRITE_ID 9u
#define GFX_ENEMY_SPRITE_COUNT 6u

/*
 * Reserved slots previously used by the sprite HUD.
 */

#define GFX_LEGACY_HUD_FIRST_SPRITE_ID 15u
#define GFX_LEGACY_HUD_SPRITE_COUNT 8u

#define GFX_ENEMY_SHOT_FIRST_SPRITE_ID 23u
#define GFX_ENEMY_SHOT_SPRITE_COUNT 6u

#define GFX_POWERUP_SPRITE_ID 29u

#define GFX_BOSS_FIRST_SPRITE_ID 30u
#define GFX_BOSS_SPRITE_COUNT 4u

/*
 * Sprite tile data.
 */

#define GFX_PLAYER_TILE_ID 0u
#define GFX_PLAYER_SHOT_TILE_ID 1u
#define GFX_ENEMY_TILE_ID 2u
#define GFX_ENEMY_SHOT_TILE_ID 14u
#define GFX_POWERUP_TILE_ID 15u

#define GFX_BOSS_FIRST_TILE_ID 16u
#define GFX_BOSS_TILE_COUNT 1u

/*
 * Background and window tile data.
 */

#define GFX_HUD_BLANK_TILE_ID 128u
#define GFX_HUD_SEPARATOR_TILE_ID 129u

#define GFX_HUD_HEART_TOP_TILE_ID 130u
#define GFX_HUD_HEART_BOTTOM_TILE_ID 131u

#define GFX_HUD_FIRST_DIGIT_TILE_ID 132u
#define GFX_HUD_DIGIT_TILE_COUNT 20u

/*
 * Hardware sprite coordinate offsets.
 */

#define GFX_SPRITE_OFFSET_X 8
#define GFX_SPRITE_OFFSET_Y 16

#endif
