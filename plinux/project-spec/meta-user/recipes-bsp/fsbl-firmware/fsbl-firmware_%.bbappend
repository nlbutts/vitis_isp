LICENSE = "MIT"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://0001-qspi-prescaler.patch"
