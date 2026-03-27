#!/bin/bash

ERROR() {
    echo -e "\e[41m E: $1 \e[0m"
}

clear
cd sphen || { ERROR "Directory sphen not found"; exit 1; }
rm -rf build/
echo "Building..."
mkdir build || { ERROR "Error to create build dir"; exit 1; }
cd build || { ERROR "Error to access build dir"; exit 1; }

echo "Compiling with CMake..."
if [ ! -f "Sphen" ]; then
    cmake ../CMakeLists.txt || { ERROR "Compilation Error"; exit 1; }	
fi
make #VERBOSE=1

cd ../..
