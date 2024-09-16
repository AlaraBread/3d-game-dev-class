#!/bin/sh

cd "$(dirname "$0")"

mkdir -p assets/compiled_shaders

for i in assets/shaders/*.frag
do
	glslangValidator -V "$i" -o "assets/compiled_shaders/`basename -s .frag "$i"`_frag.spv"
done

for i in assets/shaders/*.vert
do
	glslangValidator -V "$i" -o "assets/compiled_shaders/`basename -s .vert "$i"`_vert.spv"
done

cd src
make
