#!/bin/bash

ERROR() {
    echo -e "\e[41m E: $1 \e[0m"
}

clear
cd sphen
echo "Building..."
if [ ! -d "build" ]; then
    mkdir build || { ERROR "Error to create build dir"; exit 1; }
fi

cd build || { ERROR "Error to acess build dir"; exit 1; }

echo "Compiling with CMake..."
if [ ! -f "Sphen" ]; then
	cmake ../CMakeLists.txt || { ERROR "Compilation Error"; exit 1; }
	
fi
make #VERBOSE=1

cd ../..
if [ ! -f "sphen/build/Sphen" ]; then
 { ERROR "Sphen executable not found or created" exit 1; }
fi
./sphen/build/Sphen 
