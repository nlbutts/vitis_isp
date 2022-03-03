FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " file://bsp.cfg"
SRC_URI_append = " file://defconfig"
KERNEL_FEATURES_append = " bsp.cfg"
