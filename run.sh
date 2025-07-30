#!/bin/bash

# Ellenőrizzük, hogy a cmake telepítve van-e
if ! command -v cmake &> /dev/null
then
    echo "❌ A 'cmake' nincs telepítve. Telepítsd az alábbi paranccsal:"
    echo "   sudo apt install cmake"
    exit 1
fi

# Ellenőrizzük, hogy a make telepítve van-e
if ! command -v make &> /dev/null
then
    echo "❌ A 'make' nincs telepítve. Telepítsd az alábbi paranccsal:"
    echo "   sudo apt install build-essential"
    exit 1
fi

# Build könyvtár létrehozása és fordítás
mkdir -p build
cd build
cmake ..
make

# Bináris futtatása metrikus módban
if [[ -f GPSProcessor ]]; then
    ./GPSProcessor ../input.json ../output.json --metric
else
    echo "❌ A program fordítása nem sikerült. Nincs GPSProcessor bináris."
    exit 2
fi

