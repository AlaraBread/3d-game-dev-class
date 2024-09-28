#!/bin/sh

#./clean.sh
./build.sh
if [ $? -eq 0 ]
then
	./gf3d
fi
