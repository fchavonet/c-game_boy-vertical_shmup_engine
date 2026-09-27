GBDK_HOME ?= /opt/gbdk
LCC := $(GBDK_HOME)/bin/lcc

TARGET := build/vertical_shmup_engine.gb

SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS) Makefile | build
	$(LCC) -o $(TARGET) $(SOURCES)

build:
	mkdir -p build

clean:
	rm -rf build
