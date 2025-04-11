#!/bin/bash

# Update the system
echo "Updating the system..."
sudo apt update && sudo apt full-upgrade -y

# Install C++ compiler (g++ version 9 or greater)
echo "Installing C++ compiler..."
sudo apt install -y build-essential
sudo apt install -y gcc-9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 10
sudo update-alternatives --set gcc /usr/bin/gcc-9

# Install Python (version >= 3.8)
echo "Installing Python..."
sudo apt install -y python3.8
sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 10
sudo update-alternatives --set python3 /usr/bin/python3.8

# Install CMake (version >= 3.13)
echo "Installing CMake..."
sudo apt install -y cmake
sudo apt install -y libarchive-cpio-perl
sudo apt install -y libarchive-zip-perl

# Install build system (make)
echo "Installing build system..."
sudo apt install -y make

# Install tar (any recent version)
echo "Installing tar..."
sudo apt install -y tar

# Verify the installation
echo "Verification:"
echo "g++ --version"
g++ --version
echo "python3 --version"
python3 --version
echo "cmake --version"
cmake --version
echo "make --version"
make --version
echo "tar --version"
tar --version
