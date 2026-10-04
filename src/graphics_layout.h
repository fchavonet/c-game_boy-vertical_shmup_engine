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

#define GFX_LEGACY_HUD_FIRST_SPRITE_ID 15u
#define GFX_LEGACY_HUD_SPRITE_COUNT 8u

#define GFX_ENEMY_SHOT_FIRST_SPRITE_ID 23u
#define GFX_ENEMY_SHOT_SPRITE_COUNT 6u

#define GFX_POWERUP_SPRITE_ID 29u

#define GFX_BOSS_FIRST_SPRITE_ID 30u
#define GFX_BOSS_SPRITE_COUNT 4u

#define GFX_EFFECT_FIRST_SPRITE_ID 34u
#define GFX_EFFECT_SPRITE_COUNT 4u

/*
 * Sprite tile data.
 */

#define GFX_PLAYER_TILE_ID 0u
#define GFX_PLAYER_SHOT_TILE_ID 1u

#define GFX_ENEMY_TILE_ID 2u
#define GFX_ENEMY_RESISTANT_TILE_ID 3u
#define GFX_ENEMY_SPREAD_TILE_ID 4u

#define GFX_ENEMY_SHOT_TILE_ID 14u
#define GFX_POWERUP_TILE_ID 15u

#define GFX_BOSS_FIRST_TILE_ID 16u
#define GFX_BOSS_TILE_COUNT 4u

#define GFX_EXPLOSION_FIRST_TILE_ID 20u
#define GFX_EXPLOSION_TILE_COUNT 3u

/*
 * Background and window tile data.
 */

#define GFX_HUD_BLANK_TILE_ID 128u
#define GFX_HUD_SEPARATOR_TILE_ID 129u

/* Tiles 130 and 131 are available (former individual life icon). */

#define GFX_HUD_FIRST_DIGIT_TILE_ID 132u
#define GFX_HUD_DIGIT_TILE_COUNT 20u

#define GFX_BACKGROUND_FIRST_TILE_ID 152u
#define GFX_BACKGROUND_TILE_COUNT 3u

#define GFX_BACKGROUND_BLANK_TILE_ID 152u
#define GFX_BACKGROUND_SMALL_STAR_TILE_ID 153u
#define GFX_BACKGROUND_LARGE_STAR_TILE_ID 154u

#define GFX_PAUSE_FIRST_TILE_ID 155u
#define GFX_PAUSE_TILE_COUNT 12u

/* Precomposed life strips; follows pause tiles 155..166.
 * Up to 3 states x 5 columns x 2 rows for gaps from 0 to 8 pixels.
 */
#define GFX_HUD_LIVES_FIRST_TILE_ID 167u
#define GFX_HUD_LIVES_TILE_CAPACITY 30u

/*
 * Hardware sprite coordinate offsets.
 */

#define GFX_SPRITE_OFFSET_X 8
#define GFX_SPRITE_OFFSET_Y 16

#endif
