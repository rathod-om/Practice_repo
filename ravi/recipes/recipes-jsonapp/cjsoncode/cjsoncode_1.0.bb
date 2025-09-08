DESCRIPTION = "cJSON Parse Creation"
SECTION = "cJSON"
LICENSE = "CLOSED"

# Disable SPDX completely
#SPDX_DISABLE = "1"
#do_create_spdx[noexec] = "1"
#do_collect_spdx_deps[noexec] = "1"

SRC_URI = "file://all/"

#SRC_URI = "git://git@github.com:rathod-om/Practice_repo/blob/main/ravi/c_program/003_json_generic/json_generic.c;protocol=ssh;branch=main"

#SRCREV = "dc521b25ce0913957f3ee2e8adb7b880038fbd49"

#S = "${WORKDIR}/git"
S = "${WORKDIR}/all"

DEPENDS += "cjsonlib"

do_compile() {
        ${CC} ${CFLAGS} ${LDFLAGS} \
        -I${STAGING_INCDIR} \
        json_generic.c  -o json_generic -lcjson
}

do_install() {
        install -d ${D}${bindir}
        install -m 0755 json_generic ${D}${bindir}
}

