#!/bin/bash

config="/home/$USER/.config/i3-app-launcher"

mkdir -v $config
mv example_config $config
cd build
chmod +x build.sh
./build.sh
