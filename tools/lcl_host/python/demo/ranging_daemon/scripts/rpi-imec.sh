#!/bin/bash

RANGING_PATH=/home/pi/ranging
DAEMON_PATH=${RANGING_PATH}/python/demo/ranging_daemon
HTML_PATH=${RANGING_PATH}/python/demo/ranging_daemon/html

usage() {
  echo "Usage $0: [<options>]"
  echo "          [--run]                           (run ranging daemon)"
  echo "          [--create-db]                     (create required directories/links/files for the daemon to run)"
  echo "          [--rm-db]                         (remove database and configuration; need to execute create-db afterwards)"
  echo "          [-h|--help]                       (show usage; this message)"
}

if [ "$EUID" -ne 0 ];	then 
  echo "Must be root"
  exit 1
fi

ARG1=""
ACTION="none"
POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
      --run)
      ACTION="run"
      shift # past argument
      ;;
      --rm-db)
      ACTION="rm-db"
      shift # past argument
      ;;
      --create-db)
      ACTION="create-db"
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

su -c "mkdir -p ${RANGING_PATH}" pi

case $ACTION in
    run)
      echo "ACTION: run"
      ## Set the CPU scaling governor to performance
      echo -n performance | tee /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
      python3 ${DAEMON_PATH}/ranging_daemon.py
    ;;
    rm-db)
      echo "ACTION: rm-db"
      rm -rf ${HTML_PATH}/data
    ;;
    create-db)
      echo "ACTION: create-db"
      if [ -L /var/www/html ]; then
          rm -rf /var/www/html
      else
          su -c "mv /var/www/html /var/www/html_old" pi
      fi
      ln -sf ${HTML_PATH} /var/www/html 
      
      mkdir -p ${HTML_PATH}/data
      chmod 777 ${HTML_PATH}/data
      touch ${HTML_PATH}/data/ranging.db
      chmod 777 ${HTML_PATH}/data/ranging.db

      ln -sf /dev/shm/info.json ${HTML_PATH}/data/info.json
    ;;
    *)    # unknown option
    echo "unknown/unspecified option"
    usage
    exit 1
    ;;
esac