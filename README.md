# Blossom

Written in C using the Raylib framework. Originally made as a final project my second Java class
at my university. I am remaking it using Raylib and C++ and plan to actually finish it.

The 2D engine has a fully working animation system, player controller, level editor, and a UI to work in.
- NOTE : The level editor could use a good amount of optimizations and a better way of navigating it.

![Preview](https://github.com/HunterLiles/Blossom/blob/main/resources/preview2.gif)

- TODO :
  - Figure out some optimizations for the level editor so that it isn't eating performance.
  - Add collisions.
  - Add particle system.

## Build Instructions
### Linux
- Note: Make sure you are in the root of the project.
```
mkdir build
cd build
cmake ..
make
./blossom
```
