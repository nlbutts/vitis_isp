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

TARGET = hw
BOARD ?= zcu106

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
.PHONY: board
board:
	cd board; \
	rm -rf $(BOARD); \
	vivado -source $(BOARD).tcl -mode batch; \
	vivado -source build.tcl -mode batch -project $(BOARD)/$(BOARD).xpr

.PHONY: platform
platform:
	cd platform; xsct build.tcl

.PHONY: linux
linux:
	cd plinux; petalinux-config --get-hw-description $(XSA_FILE) --silentconfig; \
	petalinux-build

.PHONY: sdk
sdk:
	cd plinux; petalinux-build --sdk

.PHONY: accel
accel:
	make -C accel xclbin
	make -C accel host
	make -C accel sd_card

.PHONY: host
host:
	make -C accel host

all: check-env patch-files board linux sdk platform accel

.PHONY: help
help: ## Show help information
	$(E)$(foreach mkfile, \
		$(sort $(MAKEFILE_LIST)), \
		printf "\n\t\033[1;33m%s\033[0m\n" $(subst $(ROOT_DIR)/,,$(mkfile)) ; \
		grep -H -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(mkfile) | sort | \
		awk 'BEGIN {FS=":|##"}; {printf "\033[36m%25s\033[0m -%s\n", $$2, $$4}' ; )
