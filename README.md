# Treasure Runner

Treasure Runner is a C game engine with Python bindings and a terminal UI.

This repository includes:
- Native C shared library build
- Python wrapper and game UI
- C and Python test suites
- A custom Docker setup for reproducible builds on any host OS

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
make -C c test
cd python && pytest -q
python3 python/run_game.py --config /workspace/assets/starter.ini --profile /workspace/assets/player_profile.json
```