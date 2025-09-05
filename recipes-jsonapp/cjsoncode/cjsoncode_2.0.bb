DESCRIPTION = "cJSON Parse Creation"
SECTION = "cJSON"
LICENSE = "CLOSED"

# Disable SPDX completely
#SPDX_DISABLE = "1"
#do_create_spdx[noexec] = "1"
#do_collect_spdx_deps[noexec] = "1"


SRC_URI = "git://github.com/rathod-om/Practice_repo.git;protocol=ssh;branch=main"

SRCREV = "dc521b25ce0913957f3ee2e8adb7b880038fbd49"
#SRCREV = "AUTOINC"

S = "${WORKDIR}/git"

DEPENDS += "cjsonlib"

do_compile() {
        ${CC} ${CFLAGS} ${LDFLAGS} \
        -I${STAGING_INCDIR} \
        ravi/c_program/003_json_generic/json_generic.c -o json_generic_git -lcjson
}

do_install() {
        install -d ${D}${bindir}
        install -m 0755 json_generic_git ${D}${bindir}
}

