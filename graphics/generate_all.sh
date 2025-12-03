#!/bin/bash

./scripts/convert_bitmap.py -q src/bitmaps/*

# ./scripts/generate_font.py -q src/fonts/small.png 38 9
./scripts/generate_font.py -q src/fonts/small_bold.png 11 12
./scripts/generate_font.py -q src/fonts/big.png 10 37
