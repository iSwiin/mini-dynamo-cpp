# mini-dynamo-cpp

A small C++17 client library + CLI (`dynamoctl`) for the Mini Dynamo Go cluster.

This repo is designed to hit the job-posting requirements:
- C++ project using **CMake**
- Builds on Linux with **clang/gcc**
- Runs tests under **sanitizers** in CI (ASan/UBSan)
- Shows “production discipline”: retries, timeouts, tests, CI

## What it does
- `libdynamo_client`: C++ client that can `PUT` and `GET` keys from a Mini Dynamo cluster
- `dynamoctl`: CLI for quick interaction (put/get/health)

The client implements simple routing using a consistent-hash ring (primary node first), and falls back to other nodes if the primary fails.

## Prereqs
- CMake >= 3.20
- A C++17 compiler (clang or gcc)
- libcurl development headers

On Ubuntu:
```bash
sudo apt-get update
sudo apt-get install -y cmake g++ clang libcurl4-openssl-dev
```

## Build + test (Linux/macOS)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Run against your Mini Dynamo cluster
Start your Go cluster (from your Go repo):
```bash
docker compose up -d --build
```

Then from this repo:
```bash
./build/dynamoctl health
./build/dynamoctl put hello world
./build/dynamoctl get hello
```

## Config
By default, `dynamoctl` looks for `nodes.docker.json` in the current directory.
If it doesn't exist, it falls back to `localhost:9001-9003`.

You can pass a config:
```bash
./build/dynamoctl --config nodes.local.json put k v
```

Supported formats:

### Minimal JSON
```json
{ "vnodes": 128, "nodes": [
  { "id": "n1", "host": "localhost", "port": 9001 },
  { "id": "n2", "host": "localhost", "port": 9002 },
  { "id": "n3", "host": "localhost", "port": 9003 }
]}
```

### Minimal text
```txt
vnodes=128
n1 localhost 9001
n2 localhost 9002
n3 localhost 9003
```

> Note: This project intentionally uses a tiny JSON parser (regex-based) for a narrow schema to keep the repo self-contained.


