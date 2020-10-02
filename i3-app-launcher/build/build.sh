#!/bin/bash

if g++ -c `find ../src -type f -iname "*.cpp"` -Wall ; then
if g++ `find . -type f -iname "*.o"` -o i3-app-launcher -lsfml-graphics -lsfml-window -lsfml-system -v -O3; then
echo -e "\ndone building!"
fi
fi
