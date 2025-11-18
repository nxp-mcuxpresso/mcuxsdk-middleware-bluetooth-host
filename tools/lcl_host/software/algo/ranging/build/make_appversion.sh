#!/bin/bash

git log -1 &> /dev/null
if [ $? -eq 128 ]; then
    # the HEAD does not exist. Therefore an arbitrary value is used
    export GIT_GET_REVISION=0000
    export GIT_GET_HASH=__NOHEAD_
    export GIT_GET_DATE=$(date +"%a, %d %b %Y %H:%M:%S %z")
else    
    export GIT_GET_REVISION=`git rev-list HEAD --count`
    export GIT_GET_HASH=`git rev-parse --short HEAD`
    export GIT_GET_DATE=`git show -s --format=%cD`
fi

OUTFILE=appversion.h

echo "#define APPVERSION" $GIT_GET_REVISION > $OUTFILE
echo "#define APPHASH \""$GIT_GET_HASH"\"" >> $OUTFILE
echo "#define APPDATE \""$GIT_GET_DATE"\"" >> $OUTFILE


TXTFILE=appversion.txt

echo "APPVERSION"  $GIT_GET_REVISION > $TXTFILE
echo "APPHASH    \""$GIT_GET_HASH"\"" >> $TXTFILE
echo "APPDATE    \""$GIT_GET_DATE"\"" >> $TXTFILE
