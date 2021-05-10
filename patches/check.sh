#!/bin/sh

rootdirectory="$PWD"
dirs="frameworks/av frameworks/base frameworks/native hardware/interfaces packages/apps/FMRadio system/bt system/core system/netd external/wpa_supplicant_8"

RED='\033[0;31m'
NC='\033[0m'

for dir in $dirs ; do
	cd $rootdirectory
	cd $dir
	echo -e "${RED}Applying ${NC}$dir ${RED}patches...${NC}\n"
	git apply -v --check $rootdirectory/device/bq/mt8163-common/patches/$dir/*.patch
done

# -----------------------------------
echo -e "Done !\n"
cd $rootdirectory

