#!/bin/bash

# if not on inside backend folder then move to backend folder. 
if [ "$(basename "$PWD")" != "backend" ]; then
  cd backend || exit
fi

# if not build folder exists then create it
if [ ! -d "build" ]; then
  mkdir build
fi  

# move to build folder
cd build || exit
cmake ..
cmake --build .
./unittest
