#!/bin/bash

# build all
build() {
	cd $1
	echo $PWD
	make clean
	make -j6
	make clean
	cd ../../..
	echo
}

find . -maxdepth 3 -type d -name 'gcc' | while read line; do
    echo "Processing directory '$line'"
    build $line
done

