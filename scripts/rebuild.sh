#!/bin/sh

rm -rf build/
mkdir build/
cd build/
bear cmake ../
bear cmake --build .
