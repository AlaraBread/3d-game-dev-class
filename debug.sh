#!/bin/sh

cd "$(dirname "$0")"

./build.sh
valgrind --tool=callgrind ./gf3d
