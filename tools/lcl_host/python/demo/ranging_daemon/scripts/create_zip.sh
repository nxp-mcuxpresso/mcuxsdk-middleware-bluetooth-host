#!/bin/bash
# -----------------------------------------------------------------------
# Author: Jochem Govers
# -----------------------------------------------------------------------
#  CONFIDENTIAL and PROPRIETARY
#  COPYRIGHT (c) Stichting IMEC Nederland, 2018
#
#  All rights are reserved. Reproduction in whole or in part is
#  prohibited without the written consent of the copyright owner
# -----------------------------------------------------------------------
function cpdir () {
	last=${@: -1}
	if [ ! -d "${last}" ]; then
		mkdir -p "${last}"
	fi
	cp -Rf "$@"
}

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <output-filename>"
  exit
fi
ROOT_PATH=../../../..
OUTPUT_PATH=$1

RELEASE_PATH=`mktemp -d -p .`

echo "Copying files.."
# cpdir ~nrbrpoc/projectdata/build_artifacts/*.pyd ${RELEASE_PATH}/python/Instruments
# cpdir ${ROOT_PATH}/software/algo/build/rpi/libpython/*.so ${RELEASE_PATH}/python/Instruments
# cpdir ${ROOT_PATH}/software/algo/build/rpi/libpython/ranging.py ${RELEASE_PATH}/python/Instruments
cpdir ${ROOT_PATH}/software/algo/RangingMath/Python/Swig/ranging.py ${RELEASE_PATH}/python/Instruments

cpdir ${ROOT_PATH}/python/logging.ini ${RELEASE_PATH}/python
cpdir ${ROOT_PATH}/python/Generic/*utils.py ${RELEASE_PATH}/python/Generic
cpdir ${ROOT_PATH}/python/Instruments/range_estimator_pkg/*.py ${RELEASE_PATH}/python/Instruments/range_estimator_pkg
rm -r ${RELEASE_PATH}/python/Instruments/range_estimator_pkg/*AoA*
rm -r ${RELEASE_PATH}/python/Instruments/range_estimator_pkg/*JADE*
rm -r ${RELEASE_PATH}/python/Instruments/range_estimator_pkg/*Atmel*
rm -r ${RELEASE_PATH}/python/Instruments/range_estimator_pkg/*2D*
cpdir ${ROOT_PATH}/python/Instruments/tracking_pkg/*.py ${RELEASE_PATH}/python/Instruments/tracking_pkg
cpdir ${ROOT_PATH}/python/Instruments/ComPort.py ${RELEASE_PATH}/python/Instruments
cpdir ${ROOT_PATH}/python/Instruments/RangingPlatform.py ${RELEASE_PATH}/python/Instruments
cpdir ${ROOT_PATH}/python/Instruments/RangingPlatformGeneric.py ${RELEASE_PATH}/python/Instruments
cpdir ${ROOT_PATH}/python/Instruments/RangingPlatformNxp.py ${RELEASE_PATH}/python/Instruments
cpdir ${ROOT_PATH}/python/Instruments/boardinfo.nxp.csv ${RELEASE_PATH}/python/Instruments

cpdir ${ROOT_PATH}/python/demo/common/*.py ${RELEASE_PATH}/python/demo/common
cpdir ${ROOT_PATH}/python/demo/ranging_daemon/*.py ${RELEASE_PATH}/python/demo/ranging_daemon
cpdir ${ROOT_PATH}/python/demo/ranging_daemon/html ${RELEASE_PATH}/python/demo/ranging_daemon
cpdir ${ROOT_PATH}/python/demo/ranging_daemon/scripts/chromium* ${RELEASE_PATH}/python/demo/ranging_daemon/scripts
cpdir ${ROOT_PATH}/python/demo/ranging_daemon/scripts/*.sh ${RELEASE_PATH}/python/demo/ranging_daemon/scripts

# Write SHA1 of this GIT-clone
SHA1=$(git rev-parse HEAD)

echo "Creating release zip file.."
cd ${RELEASE_PATH}
echo $SHA1 > SHA1.txt
zip -qr ../${OUTPUT_PATH} *
cd ..

echo "Cleaning up.."
rm -rf ${RELEASE_PATH}
