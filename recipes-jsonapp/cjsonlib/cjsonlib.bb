DESCRIPTION = "cJSON is an ultralightweight JSON parser in ANSI C"
HOMEPAGE = "https://github.com/DaveGamble/cJSON"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=218947f77e8cb8e2fa02918dc41c50d0"

SRC_URI = "git://github.com/DaveGamble/cJSON.git;branch=master;protocol=https"
SRCREV = "8f2beb57ddad1f94bed899790b00f46df893ccac"  

S = "${WORKDIR}/git"

inherit cmake

EXTRA_OECMAKE = "-DENABLE_CJSON_TEST=OFF"

do_install:append() {
    install -d ${D}${includedir}
    install -m 0644 ${S}/cJSON.h ${D}${includedir}
}

