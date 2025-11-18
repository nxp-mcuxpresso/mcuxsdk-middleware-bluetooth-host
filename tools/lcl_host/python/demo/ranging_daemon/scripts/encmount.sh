#!/bin/bash

usage() {
  echo "Usage $0: [<options>] [<encrypt-path>]"
  echo "          [-m|--mount]                        (mount path using passphrase)"
  echo "          [[-p|--passphrase] <passphrase>]    (specify passphrase)"
  echo "          [[-u|--user] <user>]                (specify directory ownership)"
  echo "          [-u|--unmount]                      (unmount path)"
  echo "          [-h|--help]                         (show usage; this message)"
}

if [ "$EUID" -ne 0 ];	then 
  echo "Must be root"
  exit 1
fi

ACTION="none"
PASSPHRASE=""
_USER=""
POSITIONAL=()
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
      -u|--user)
      _USER="$2"
      shift # past argument
      shift # past value
      ;;
      -p|--passphrase)
      PASSPHRASE="$2"
      shift # past argument
      shift # past value
      ;;
      -m|--mount)
      ACTION="mount"
      shift # past argument
      ;;
      -u|--unmount)
      ACTION="unmount"
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

if [[ $# -ge 1 ]]; then
ENCPATH=`realpath $1`
else
usage
exit 1
fi

case $ACTION in
    unmount)
      if [[ $# -ne 1 ]]; then
        usage
        exit 1
      fi

      if [ ! -d "$ENCPATH" ];	then 
        echo "$ENCPATH does not exist, exiting.."
        exit 1
      fi
      umount ${ENCPATH}
    ;;
    mount)
      if [[ $# -ne 1 ]]; then
        usage
        exit 1
      fi
      if [ ! -d "$ENCPATH" ];	then 
        echo "$ENCPATH does not exist, creating folder.."
        mkdir -p ${ENCPATH}
      fi
      MOUNT_INFO=`mount | grep "$ENCPATH"`
      if [ ! -z "$MOUNT_INFO" ];	then 
        echo "$ENCPATH already mounted.."
        exit 1
      fi
      P_OPTS=""
      if [ ! -z "$PASSPHRASE" ];	then 
          P_OPTS=",passphrase_passwd=$PASSPHRASE"
      fi
      mount -t ecryptfs ${ENCPATH} ${ENCPATH} -o key=passphrase,ecryptfs_cipher=aes,ecryptfs_key_bytes=16,ecryptfs_passthrough=no,ecryptfs_enable_filename_crypto=no${P_OPTS}
      if [ ! -z "$_USER" ];	then 
          chown -R $_USER ${ENCPATH}
      fi
    ;;
    *)    # unknown option
    exit 1
    ;;
esac
