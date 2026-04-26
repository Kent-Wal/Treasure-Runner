# Treasure Runner

Treasure Runner is a terminal-based dungeon exploration game built in C for a school project. The game features room-based movement, collectible treasures, map loading, player state management, and a curses-based user interface.

The project focuses on low-level memory management, modular program design, file-based configuration, and structured game engine logic.

## Features

- Terminal-based gameplay using a curses-style interface
- Room and tile-based player movement
- Treasure collection system
- Collision and movement validation
- Configurable game setup through external files
- Modular engine design separating game logic from display logic
- Dynamic memory management for rooms, player data, and game state
- Error handling through return/status codes
- Unit/integration testing support through a higher-level test wrapper

## Project Overview

The game is structured around a custom engine that loads generated game data, stores it in persistent internal structures, and manages player interaction during gameplay.

Core responsibilities of the engine include:

- Loading room and map data
- Tracking the player’s current location
- Handling movement requests
- Detecting blocked paths or invalid moves
- Managing treasure pickup behavior
- Managing locked vs. unlocked portals
- Maintaining game state across turns
- Cleaning up dynamically allocated memory

One important design detail is that game data loaded from the generator is copied into the engine’s own structures. This allows the engine to safely manage its own memory and avoid relying on temporary generated data.

## Technical Highlights

- **Language:** C, Python
- **Interface:** Terminal UI using curses
- **Concepts Used:**
  - Pointers and dynamic memory allocation
  - Struct-based data modelling
  - File/config parsing
  - Modular C design
  - Python wrapper
  - Error/status code handling
  - Graph-style room connectivity
  - Unit and integration testing

## Game Logic

The player moves through a grid or room-based environment while searching for treasures. Each move is validated by the engine before being applied.

Movement can result in statuses such as:

- Successful move
- Blocked movement
- Invalid direction
- Treasure interaction
- Game completion or failure condition

## Project Layout

- `c/`: C engine source, headers, tools, and C tests
- `python/`: Python bindings, models, UI, and Python tests
- `assets/`: INI configs and runtime assets
- `dist/`: runtime shared libraries used by Python/C integration

## Build and Run (Docker)

Build image:

```bash
docker compose build
```

Open shell in container:

```bash
docker compose run --rm app
```

Inside the container:

```bash
make dist
source env.sh
cd python
python3 run_game.py --config ../assets/starter.ini --profile ../assets/player_profile.json
```

## License

This project is licensed under the MIT License — see the [LICENSE](./LICENSE) file for details.