GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc
PNG2ASSET := $(GBDK_HOME)/bin/png2asset
PYTHON ?= python3
LCCFLAGS ?=

TARGET := build/vertical_shmup_engine.gb
GENERATED_C := build/assets/player_sprite.c
GENERATED_H := build/assets/player_sprite.h
SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h)

.DEFAULT_GOAL := all
.DELETE_ON_ERROR:
.PHONY: all clean

all: $(TARGET)

# Grouped outputs require GNU Make 4.3+ (the project's Make 4.4.1 supports this).
$(GENERATED_C) $(GENERATED_H) &: assets/player.png tools/check_player_png.py Makefile | build/generated
	$(PYTHON) tools/check_player_png.py assets/player.png
	"$(PNG2ASSET)" assets/player.png -o $(GENERATED_C) -map -tiles_only -keep_palette_order -no_palettes -noflip

$(TARGET): $(SOURCES) $(HEADERS) $(GENERATED_C) $(GENERATED_H) Makefile | build
	"$(LCC)" $(LCCFLAGS) -Ibuild/assets -o $@ $(SOURCES) $(GENERATED_C)

build build/generated:
	mkdir -p $@

clean:
	rm -rf build
