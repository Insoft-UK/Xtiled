#!/bin/bash
DIR=$(dirname "$0")
cd "$DIR"
clear

~/GitHub/Insoft-UK/scripts/version_code.sh
make -j$(sysctl -n hw.ncpu) x86_64
make -j$(sysctl -n hw.ncpu) arm64
make -j$(sysctl -n hw.ncpu) universal
source ~/GitHub/notarization.sh

find build -type f -exec xattr -c {} \;
find build -type f -exec codesign --remove-signature {} \;
find build -type f -exec codesign --sign "Developer ID Application: $YOUR_NAME ($TEAM_ID)" --options runtime --timestamp {} \;

codesign --verify --verbose build/*

read -p "Press Enter to exit!"

# Close the Terminal window
osascript -e 'tell application "Terminal" to close window 1' & exit
