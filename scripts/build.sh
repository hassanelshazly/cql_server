#!/bin/sh

if [ -e build/ ]; then
    cd build/
    cmake ../
    cmake --build .
else 
   ./scripts/rebuild.sh
fi
