#!/bin/bash

# Build script for MUD Server

set -e  # Exit on any error

echo "Building MUD Server..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Run cmake
cmake ..

# Build the project
make -j$(nproc)

echo "Build complete!"
echo "Executable: ./bin/mud_server"
echo ""
echo "To run the server:"
echo "  cd build && ./bin/mud_server [port]"
echo ""
echo "To connect via telnet:"
echo "  telnet localhost 8080"
