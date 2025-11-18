#!/bin/bash
set -e 		# Exit when anything fails

GCOV_EXCLUDE='--exclude ../../../libs/common/src/arm'

usage() {
  echo "Usage $0: [<options>]"
  echo "          [--run]                           (run all tests)"
  echo "          [--coverage]                      (create coverage report; --run is implied)"
  echo "          [--memtrace]                      (enable memtrace)"
  echo "          [--stopwatch]                     (enable stopwatch)"
  echo "          [--no_rangecheck]                 (disable range check)"
  echo "          [-h|--help]                       (show usage; this message)"
}

DO_RUN=0
COVERAGE=0
USE_MEMTRACE=0
USE_STOPWATCH=0
USE_RANGE_CHECK=1

POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
      --run)
      DO_RUN=1;
      shift # past argument
      ;;
      --coverage)
      DO_RUN=1;
      COVERAGE=1;
      USE_RANGE_CHECK=0;
      shift # past argument
      ;;
      --memtrace)
      USE_MEMTRACE=1;
      shift # past argument
      ;;
      --stopwatch)
      USE_STOPWATCH=1;
      shift # past argument
      ;;
      --no_rangecheck)
      USE_RANGE_CHECK=0;
      shift # past argument
      ;;
      -h|--help)
      usage
      exit
      ;;
      *)    # unknown option
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
      ;;
  esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

# remove old logging-file (if exists)
CURDIR=$(pwd)
rm -f ${CURDIR}/testRanging.log

# This function builds the code, executes the elf-file and cleans-up afterwards
build_run_clean() {
    DO_RUN=$2
    LOG_FILE=$7
    cd $1

    make clean
    make all COVERAGE=$3 USE_MEMTRACE=$4 USE_STOPWATCH=$5 USE_RANGE_CHECK=$6 -j6

    if [ $DO_RUN -eq 1 ]; then
        echo $LOG_FILE
        echo $(pwd)
        find . -maxdepth 2 -type f -name '*.elf' | while read line; do
            echo "Processing directory '$line'"
            $line 2>&1 | tee -a $LOG_FILE
        done
    fi

    if [ $COVERAGE -eq 1 ]; then
        # construct filter and run coverage over the annotated executable run
        FILTER=../../../libs/"$(cut -d'/' -f2 <<<"$1")"
        echo "Coverage FILTER = $FILTER"
        gcovr --root ../../.. --filter $FILTER $GCOV_EXCLUDE --json --output trace.json
    fi

    make clean
    cd ../../..
    echo
}

# find all directories named 'gcc' and do build, execute the elf-file and clean-up
find tests -maxdepth 3 -type d -name 'gcc' | while read line; do
    echo "Processing directory '$line'"
    build_run_clean $line $DO_RUN $COVERAGE $USE_MEMTRACE $USE_STOPWATCH $USE_RANGE_CHECK ${CURDIR}/testRanging.log
done


if [ $COVERAGE -eq 1 ]; then
    # cleanup coverage output from previous run
    rm -rf ./gcovoutput
    mkdir ./gcovoutput

    # find and combine all the coverage json tracefiles
    JSON=`find tests/ -name *.json  -exec echo -n "--add-tracefile {} " \;`

    gcovr $JSON --config gcovr_linux64.cfg
fi
