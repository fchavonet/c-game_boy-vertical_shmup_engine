GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc
PNG2ASSET := $(GBDK_HOME)/bin/png2asset
PYTHON ?= python3
LCCFLAGS ?=

TARGET := build/vertical_shmup_engine.gb
ASSET_BUILD_DIR := build/assets
SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h)
GENERATED_C :=
GENERATED_H :=

.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
.PHONY: all clean

all: $(TARGET)

# $(1): C symbol; $(2): source PNG; $(3)/$(4): exact width/height.
# Grouped outputs require GNU Make 4.3+, as in the existing build.
define SPRITE_ASSET
GENERATED_C += $(ASSET_BUILD_DIR)/$(1).c
GENERATED_H += $(ASSET_BUILD_DIR)/$(1).h
$(ASSET_BUILD_DIR)/$(1).c $(ASSET_BUILD_DIR)/$(1).h &: $(2) tools/check_sprite_png.py Makefile | $(ASSET_BUILD_DIR)
	$(PYTHON) tools/check_sprite_png.py $(2) $(3) $(4)
	"$(PNG2ASSET)" $(2) -o $(ASSET_BUILD_DIR)/$(1).c -map -tiles_only -keep_palette_order -no_palettes -noflip -keep_duplicate_tiles
endef

$(eval $(call SPRITE_ASSET,player_sprite,assets/player/player.png,8,8))
$(eval $(call SPRITE_ASSET,enemy_standard_sprite,assets/enemies/standard.png,8,8))
$(eval $(call SPRITE_ASSET,enemy_resistant_sprite,assets/enemies/resistant.png,8,8))
$(eval $(call SPRITE_ASSET,enemy_spread_sprite,assets/enemies/spread.png,8,8))
$(eval $(call SPRITE_ASSET,enemy_shot_sprite,assets/projectiles/enemy.png,8,8))
$(eval $(call SPRITE_ASSET,player_shot_sprite,assets/projectiles/player.png,8,8))
$(eval $(call SPRITE_ASSET,powerup_sprite,assets/bonuses/powerup.png,8,8))
$(eval $(call SPRITE_ASSET,explosion_sprite,assets/effects/explosion.png,24,8))
$(eval $(call SPRITE_ASSET,boss_sprite,assets/enemies/boss.png,16,16))

$(TARGET): $(SOURCES) $(HEADERS) $(GENERATED_C) $(GENERATED_H) Makefile | build
	"$(LCC)" $(LCCFLAGS) -I$(ASSET_BUILD_DIR) -o $@ $(SOURCES) $(GENERATED_C)

build $(ASSET_BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf build
