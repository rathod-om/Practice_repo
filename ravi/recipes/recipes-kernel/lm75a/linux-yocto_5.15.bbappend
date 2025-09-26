FILESEXTRAPATHS:prepend := "${THISDIR}/linux-yocto:"

SRC_URI += " file://0001-lm75a-temp-read-using-sysfs.patch \ 
	file://enable-sysfs-lm75a.cfg \
"
#SRC_URI += " file://lm75a.patch \
#	file://0001-am335x-add-lm75a-sensor.patch \
#	file://0002-added-source-file-lm75a.c.patch \
#	file://0003-error-handling.patch \
#	file://added_HWMON_and_LM75_support.cfg \ 
#"


