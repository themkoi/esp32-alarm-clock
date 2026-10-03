#!/bin/bash

./build_icons.sh ./raw_icons/mine_png/24x24 icons 24 16 1 &
./build_icons.sh ./raw_icons/mine_png/16x16 icons 16 16 1 &

./build_icons.sh ./raw_icons/svg_weather weather 50 1 &
./build_icons.sh ./raw_icons/svg_weather weather 40 1 &
./build_icons.sh ./raw_icons/svg_weather weather 21 1 &

wait
echo "All builds done."
