#!/bin/bash

config="/home/$USER/.config/i3-app-launcher"

mkdir -v $config
cp example_config "$config/config"
cp fonts/* "$config/font.otf"
cd build
chmod +x build.sh
./build.sh
