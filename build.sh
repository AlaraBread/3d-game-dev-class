#!/bin/sh

mkdir -p assets/compiled_shaders

glslangValidator -V assets/shaders/default.frag -o assets/compiled_shaders/frag.spv
glslangValidator -V assets/shaders/default.vert -o assets/compiled_shaders/vert.spv
glslangValidator -V assets/shaders/sky.frag -o assets/compiled_shaders/sky_frag.spv
glslangValidator -V assets/shaders/sky.vert -o assets/compiled_shaders/sky_vert.spv
glslangValidator -V assets/shaders/sprite.frag -o assets/compiled_shaders/sprite_frag.spv
glslangValidator -V assets/shaders/sprite.vert -o assets/compiled_shaders/sprite_vert.spv

cd src
make
