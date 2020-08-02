#!/bin/sh

set -e


if [ "$1" = "clean" ]; then

    rm -f d.zip

elif [ "$1" = "zip" ]; then

    dirnam=domenu

    rm -rf $dirnam
    mkdir -p $dirnam
    cp -r menu $dirnam
    unzip ./support/doug172.zip -d $dirnam > /dev/null
    rm $dirnam/doug172/bin/dougmenu.exe

    cd $dirnam
    set +e
    diff --recursive --new-file doug172 menu > domenu.pat
    set -e
    rm -rf doug172
    rm -rf menu
    cd ..


    cp ./support/doug172.zip $dirnam
    cp ./support/domenu.htm $dirnam
    cp ./support/basic.mnu $dirnam
    cp ./build.bat $dirnam

    rm -f d.zip
    7z a d.zip $dirnam
    rm -rf $dirnam

else

    printf "  Targets:\n"
    printf "      clean       Remove generated files\n"
    printf "      zip         Store files for FreeDOS in d.zip\n"
    printf "\n"

fi
