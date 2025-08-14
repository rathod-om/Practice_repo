#!/bin/bash

set -eu

# Configuration
YOCTO_DIR="$PWD/yocto"
POKY_DIR="$YOCTO_DIR/poky"
BUILD_DIR="$YOCTO_DIR/build"
BRANCH="kirkstone"
LOG_FILE="logs/Yocto_install_add_recipe_$(date +%d_%m_%y_%H_%M_%S).log"

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
fi




# Install required packages
sudo apt update
sudo apt install -y \
    gawk wget git diffstat unzip texinfo gcc build-essential \
    chrpath socat cpio python3 python3-pip python3-pexpect \
    xz-utils debianutils iputils-ping libsdl1.2-dev xterm \
    python3-git locales zstd lz4 liblz4-tool libzstd-dev

# Generate locale

echo "============= Generate locale  ================"
sudo locale-gen en_US.UTF-8
sudo update-locale LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8

export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8


echo "Verifying locale settings..."
locale


#cd "$YOCTO_DIR"

# Clone Poky if not already present
if [ ! -d "$POKY_DIR" ]; then
    git clone -b "$BRANCH" git://git.yoctoproject.org/poky.git "$POKY_DIR"
fi

# Install Yocto buildtools
"$POKY_DIR/scripts/install-buildtools" --base-url https://downloads.yoctoproject.org/releases/yocto

# Source build environment once
set +u
source "$POKY_DIR/oe-init-build-env" "$BUILD_DIR"
set -u

# Disable LTO to prevent gcc-cross build issues
#echo 'INHERIT_REMOVE = "lto"' >> "$BUILD_DIR/conf/local.conf"

echo "================================================================="
echo "=============== To enter the build environment later, run : source poky/oe-init-build-env build ==============="
echo "=============== Setup is Done. ==============="
echo "================================================================="
echo "=============== Creating Creating Layer  ==============="

#cd ../

if [ ! -d "$BUILD_DIR/meta-jsonapp" ]; then
	bitbake-layers create-layer meta-jsonapp
	bitbake-layers add-layer meta-jsonapp
fi


echo "=============== Creating cloning recipes from git  ==============="
cd $BUILD_DIR/meta-jsonapp/

if [ ! -d recipes-jsonapp ]; then
	git clone https://github.com/rathod-om/Practice_repo

	cp -r $BUILD_DIR/meta-jsonapp/Practice_repo/ravi/recipes-jsonapp/ .
	
	rm -rf $BUILD_DIR/meta-jsonapp/Practice_repo  
fi

echo "=============== bitbaking the core-image-cjson code  ==============="

#cd $BUILD_DIR/meta-jsonapp/recipes-jsonapp/cjsonlib/
#pwd
#bitbake cjsonlib

#cd $BUILD_DIR/meta-jsonapp/recipes-jsonapp/cjsoncode/
#bitbake cjsoncode

#cd $BUILD_DIR/meta-jsonapp/recipes-jsonapp/image/

MACHINE=beaglebone-yocto bitbake core-image-cjson



# Creating Image for beagle-bone-black
#echo "=============== Creating Image for beagle-bone-black  ==============="
#MACHINE=beaglebone-yocto bitbake core-image-minimal
#/home/omrathod/Yocto_install/create_image.sh
echo "=============== Image built for cjson code  ==============="

