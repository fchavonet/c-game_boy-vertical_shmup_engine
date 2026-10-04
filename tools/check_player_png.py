#!/usr/bin/env python3
"""Check the 8x8 indexed PNG contract before calling GBDK png2asset.
Uses only the Python standard library. Pixel decoding is done by png2asset.
"""
import struct
import sys
from pathlib import Path


def validate(path):
    data = Path(path).read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError("Expected a PNG file.")
    position = 8
    header = None
    palette = None
    transparency = None
    while position + 12 <= len(data):
        size = struct.unpack_from(">I", data, position)[0]
        kind = data[position + 4:position + 8]
        end = position + 12 + size
        if end > len(data):
            raise ValueError("Truncated PNG file.")
        chunk = data[position + 8:position + 8 + size]
        if kind == b"IHDR":
            header = chunk
        elif kind == b"PLTE":
            palette = chunk
        elif kind == b"tRNS":
            transparency = chunk
        elif kind == b"IEND":
            break
        position = end
    if header is None or len(header) != 13:
        raise ValueError("Missing or invalid PNG header.")
    width, height, depth, mode, _, _, _ = struct.unpack(">IIBBBBB", header)
    if (width, height) != (8, 8):
        raise ValueError("Player image must be exactly 8x8 pixels.")
    if mode != 3 or depth not in (1, 2, 4, 8):
        raise ValueError("Export as an indexed-color PNG, preserving the template palette.")
    expected = bytes([255, 255, 255, 170, 170, 170, 85, 85, 85, 0, 0, 0])
    if palette != expected:
        raise ValueError(
            "Use exactly four palette entries, in this order: "
            "#FFFFFF, #AAAAAA, #555555, #000000. Export a 4-color indexed PNG."
        )
    if transparency is not None:
        if len(transparency) > 4 or any(alpha != 255 for alpha in transparency[1:]):
            raise ValueError("Only palette index 0 may be transparent.")
        if transparency and transparency[0] not in (0, 255):
            raise ValueError("Partial transparency is not supported.")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("Usage: python3 tools/check_player_png.py assets/player.png")
    try:
        validate(sys.argv[1])
    except (OSError, ValueError) as error:
        sys.exit("Player PNG: " + str(error))
