#!/bin/sh

cd "$(dirname "$0")"

rm gf3d

rm -r assets/compiled_shaders
rm -r build

cd src
make clean
