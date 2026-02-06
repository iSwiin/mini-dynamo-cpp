# mini-dynamo-cpp

This project is a C++17 client + CLI for my mini dynamo cluster. 

I built this to make it easy to interact with my mini dynamo cluster from C++. It’s a small client library and a CLI for basic operations (health checks, put/get). 
It also includes a test setup so the project is easy to run and verify on Linux.

---

## What’s in here

- **`dynamo_client` (library):** a small C++ client you can embed in other programs.
- **`dynamoctl` (CLI):** a command-line tool for `health`, `put`, `get` (and whatever else you add later).
- **Build system:** CMake.
- **Tests:** unit tests via `ctest`.
- **CI:** builds with clang + gcc and runs with ASan/UBSan.

---
## Requirements

This is easiest on Linux. On Windows, I build it in WSL Ubuntu.

### Ubuntu / WSL packages
```bash
sudo apt-get update
sudo apt-get install -y cmake g++ clang libcurl4-openssl-dev
```

## Quick start

### 1) Start the Mini Dynamo cluster (Go project)

In your Go Mini Dynamo repo (the one with `docker-compose.yml`):

```bash
docker compose up -d --build
docker compose ps
```

You should have **3 nodes** up (`n1`/`n2`/`n3`).

### 2) Build this repo

From the `mini-dynamo-cpp` repo root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

### 3) Run the CLI

```bash
./build/dynamoctl health
./build/dynamoctl put hello world
./build/dynamoctl get hello
```

If your cluster is running, `health` should show **200s** and `get` should return `world`.

## Config

By default the CLI reads a simple JSON config (`nodes.local.json`) that points at:

- `http://localhost:9001`
- `http://localhost:9002`
- `http://localhost:9003`

If you move ports or run a different setup, update the config file (or pass a different one if you add that flag).

## Project layout (roughly)

```makefile
include/     # public headers
src/         # library implementation
tools/       # CLI (dynamoctl)
tests/       # unit tests
cmake/       # CMake helpers (sanitizers)
.github/     # CI workflow
Jenkinsfile  # Jenkins pipeline (mirrors CI)
```

