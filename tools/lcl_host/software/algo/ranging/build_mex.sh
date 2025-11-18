#!/bin/bash

# build all
build() {
	cd $1
	pwd
	make clean
	make -j6
	
	filename=$(basename ./bin/*.mexa64 .mexa64)
	cp ./bin/*.mexa64 $2
	cp ./bin/appversion.txt $2/${filename}.version
	cp ./bin/build.info $2/${filename}.buildinfo
	make clean
	cd $2
	echo 
}

CURDIR=$(pwd)
find . -maxdepth 3 -type d -name 'gcc' | while read line; do
    if [[ "$line" == *"mex"* ]]; then
	echo "Processing directory '$line'"
	build $line $CURDIR
    fi
done

rm -f ../../../matlab/matlab_lib/NRBLOC/5_RangingEngine/*.mexa64
rm -f ../../../matlab/matlab_lib/NRBLOC/5_RangingEngine/*.buildinfo
rm -f ../../../matlab/matlab_lib/NRBLOC/5_RangingEngine/*.version

mv *.mexa64 ../../../matlab/matlab_lib/NRBLOC/5_RangingEngine/
mv *.buildinfo ../../../matlab/matlab_lib/NRBLOC/5_RangingEngine/
mv *.version ../../../matlab/matlab_lib/NRBLOC/5_RangingEngine/
