# MUD Server

A C++ telnet-based multiplayer text adventure server focusing on object-oriented design and networking patterns.

## Features

- Telnet server accepting multiple concurrent connections
- User authentication system with username/password
- Session management and persistence
- Object-oriented design with design patterns
- Modern C++17 features

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./bin/mud_server
```

Then connect via telnet:
```bash
telnet localhost 8080
```

## Project Structure

```
src/           - Source files
include/       - Header files
data/          - Data files (user accounts, etc.)
build/         - Build directory (created by cmake)
```

## Development Goals

This project focuses on learning:
- C++ object-oriented programming
- Design patterns (Factory, Observer, Command, etc.)
- Network programming with sockets
- Multi-threading and concurrency
- Session and state management
