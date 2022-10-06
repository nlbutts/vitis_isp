.DEFAULT_GOAL := help

.SILENT: help help-details

# verbosity
V ?= 1
ifeq ($(V),0)
E = @
DN = >/dev/null
DNERR = 2>/dev/null
else
E =
DN =
DNERR =
endif

ROOT_DIR=$(shell git rev-parse --show-toplevel)
GIT_COMMITTER_EMAIL=$(shell git config user.email)
GIT_COMMITTER_NAME=$(shell git config user.name)

EDGE_COMMON_SW = $(ROOT_DIR)/plinux/images/linux
XSA_FILE = $(ROOTDIR)/jd106_extend/MPSoC_ext_platform_wrapper.xsa
DEVICE = $(ROOT_DIR)/platform/jd106/export/jd106/jd106.xpfm
SYSROOT = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux
HOST_ARCH = aarch64
OPENCV_INCLUDE = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/opencv4
OPENCV_LIB = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/lib
TARGET = hw
ROOT_TAR = $(ROOTDIR)/plinux/images/linux/rootfs.tar.gz
SDK = $(ROOTDIR)/plinux/images/linux/sdk.sh
XCLBIN = $(ROOTDIR)/accel/build_dir.hw.MPSoC_ext_platform_wrapper/krnl_ISPPipeline_xo.xclbin

# Check to make sure the ssh agent is running with a key
.PHONY: check-env
check-env:
ifndef XILINX_VITIS
	$(error XILINX_VITIS is not defined)
else
	$(E)echo "Vitis is defined"
endif
ifndef PETALINUX
	$(error PETALINUX is not defined)
else
	$(E)echo "PETALINUX is defined"
endif

.PHONY: patch-files
patch-files:
	$(E)cd Vitis_Libraries; pwd; \
	$(E)patch -p1 --forward < ../patches/0001_use_uram.patch || true

# Check that the ssh-agent is running and the submodules are updated
$(XSA_FILE): jd106_extend/build.tcl jd106_extend/jd106_extend.tcl
	cd jd106_extend; \
	vivado -source jd106_extend.tcl -mode batch; \
	vivado -source build.tcl -mode batch -project jd106_extend/jd106_extend.xpr

$(DEVICE): $(XSA_FILE)
	cd platform; xsct build.tcl

$(ROOT_TAR): $(XSA_FILE)
	cd plinux; petalinux-config --get-hw-description $(XSA_FILE) --silentconfig; \
	petalinux-build

$(SDK):
	cd plinux; petalinux-build --sdk

$(XCLBIN):
	make -C accel all

.PHONY: host
host:
	make -C accel host

all: check-env patch-files $(XSA_FILE) $(DEVICE) $(ROOT_TAR) $(SDK) $(XCLBIN)

.PHONY: help
help: ## Show help information
	$(E)$(foreach mkfile, \
		$(sort $(MAKEFILE_LIST)), \
		printf "\n\t\033[1;33m%s\033[0m\n" $(subst $(ROOT_DIR)/,,$(mkfile)) ; \
		grep -H -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(mkfile) | sort | \
		awk 'BEGIN {FS=":|##"}; {printf "\033[36m%25s\033[0m -%s\n", $$2, $$4}' ; )
