# Botman

![gif](https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExaWt4cDE3ZjBqYjJoNWJkdGJ2YWo1dTFhaGNjcTB2NjJyYXhzdWZoYyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/Uk07jT9BEhZpQY4kZJ/giphy.gif)

Pacman like game, written in c++ with raylib.

#### Featuring
- Built in map creator/editor
- Handmade textures
- Useless score system

## Gameplay
Use arrow keys to move the robot. \
Collect hammers to smash bugs. \
Collect all coins to win.

## Compilation

Clone the repo
```bash
git clone https://github.com/szulf/botman && cd botman
```

Build the cmake cache file
```bash
mkdir build
cmake -B build -S .
```

Build the project
```bash
cmake --build build --config Release
```

And run
```bash
build/botman
```

Or you can just use the import feature in VS.

## Known bugs
- not winning after collecting all pellets
- bugs get stuck in a corner of the map
- maps directory doesnt get created and game crashes
- Bugs sometimes walk through walls
- score doesnt reset after losing the game

