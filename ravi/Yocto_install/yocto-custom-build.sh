#!/bin/bash

set -eu

# Configuration
YOCTO_DIR="$PWD/yocto"
POKY_DIR="$YOCTO_DIR/poky"
BUILD_DIR="$YOCTO_DIR/build"
BRANCH="kirkstone"
LOG_FILE="logs/Yocto-install-add-recipe-$(date +%d-%m-%y-%H-%M-%S).log"
LAYER="meta-drivers"
GIT_REPO="https://github.com/rathod-om/Practice_repo"
GIT_PATH="Practice_repo/recipes-kernel/"


mkdir -p logs
exec > >(tee -a "$LOG_FILE") 2>&1

 #Create directory structure
 if [ -d "$YOCTO_DIR" ]; then
	 echo -e "Directory already exists\n\tif you want to overwrite press 1 or if you want exit press any key: " 
	 read choice
	 echo "User input: $choice" 
	 if [ "$choice" == "1" ]; then
		 echo "overwriting"      
	 else
		 echo "safely exiting"
		 exit
	 fi
 else
	 mkdir -p "$YOCTO_DIR"
	 echo "creating Directory"
	# Install required packages
	 sudo apt update
	 sudo apt install -y \
		 gawk wget git diffstat unzip texinfo gcc build-essential \
		 chrpath socat cpio python3 python3-pip python3-pexpect \
		 xz-utils debianutils iputils-ping libsdl1.2-dev xterm \
		 python3-git locales zstd lz4 liblz4-tool libzstd-dev
		
 fi

# Generate locale

echo "============= Generate locale  ================"
sudo locale-gen en_US.UTF-8
sudo update-locale LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8

export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8


echo "Verifying locale settings..."
locale

# Clone Poky if not already present
if [ ! -d "$POKY_DIR" ]; then
	git clone -b "$BRANCH" git://git.yoctoproject.org/poky.git "$POKY_DIR"

else 
	echo "Poky already exists - skipping clone."
fi


# Install Yocto buildtools
if [ -x "$POKY_DIR/scripts/install-buildtools" ]; then
	echo "Installing Yocto buildtools..."
	"$POKY_DIR/scripts/install-buildtools" --base-url https://downloads.yoctoproject.org/releases/yocto
else 
	echo "install-buildtools not found in Poky."
fi

# Source build environment once
set +u
source "$POKY_DIR/oe-init-build-env" "$BUILD_DIR"
set -u


echo "================================================================="
echo "=============== To enter the build environment later, run : source poky/oe-init-build-env build ==============="
echo "=============== Setup is Done. ==============="
echo "================================================================="
echo "=============== Creating Creating Layer  ==============="


if [ ! -d "$BUILD_DIR/$LAYER" ]; then
	bitbake-layers create-layer $LAYER
	bitbake-layers add-layer $LAYER
fi


echo "=============== Creating cloning recipes from git  ==============="
cd $BUILD_DIR/$LAYER/

if [ ! -d recipes-jsonapp ]; then
	git clone --branch custom-recipe --single-branch $GIT_REPO
	
	cp -r $BUILD_DIR/$LAYER/$GIT_PATH .

	rm -rf $BUILD_DIR/$LAYER/Practice_repo  
fi

echo "=============== bitbaking the core-image-cjson code  ==============="


# Creating Image for beagle-bone-black
MACHINE=beaglebone-yocto bitbake core-image-minimal

echo "=============== Image built for cjson code  ==============="

