#! /bin/zsh

cmake -S . -B build && cmake --build build
printf "\rApplication running..."
./build/blossom_vulkan
