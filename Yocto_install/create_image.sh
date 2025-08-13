#!/bin/bash
YOCTO_DIR="$HOME/yocto"
POKY_DIR="$YOCTO_DIR/poky"
BUILD_DIR="$YOCTO_DIR/build"


source "$POKY_DIR/oe-init-build-env" "$BUILD_DIR"

MACHINE=beaglebone-yocto bitbake core-image-minimal
