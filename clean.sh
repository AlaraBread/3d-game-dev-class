#!/bin/sh

cd "$(dirname "$0")"

rm gf3d

rm -r build

cd src
make clean
