#
# Copyright 2019-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# makefile-generator v1.0.4
#

# ####################################### Help Section #####################################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""
	$(ECHO) "  make sd_card TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64>"
	$(ECHO) "      Command to prepare sd_card files."
	$(ECHO) "      This target is only used in embedded device."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86>"
	$(ECHO) "      Command to run application in emulation or on board."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<sw_emu/hw_emu/hw> DEVICE=<FPGA platform> HOST_ARCH=<aarch32/aarch64/x86>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host HOST_ARCH=<aarch32/aarch64/x86>"
	$(ECHO) "      Command to build host application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH is required for embedded devices"
	$(ECHO) ""
	$(ECHO) "  NOTE: For embedded devices, e.g. zcu102/zcu104/vck190, env variable SYSROOT, EDGE_COMMON_SW and PERL need to be set first, and HOST_ARCH is either aarch32 or aarch64. For example,"
	$(ECHO) "       export SYSROOT=< path-to-platform-sysroot >"
	$(ECHO) "       export EDGE_COMMON_SW=< path-to-rootfs-and-Image-files >"
	$(ECHO) "       export PERL=<path-to-perl-installation-location >"
	$(ECHO) ""


ROOT_DIR=$(shell git rev-parse --show-toplevel)
OPENCV_INCLUDE = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/opencv4
OPENCV_LIB = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/usr/lib
#SYSROOT = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux
# When we source the SDK we get the ENV SDKTARGETSYSROOT which is the sysroot
SYSROOT = $(SDKTARGETSYSROOT)
DEVICE = $(ROOT_DIR)/platform/jdboard/export/jdboard/jdboard.xpfm
EDGE_COMMON_SW = $(ROOT_DIR)/plinux/images/linux
XSA_FILE = $(ROOT_DIR)/board/MPSoC_ext_platform_wrapper.xsa
TARGET ?= hw
HOST_ARCH ?= aarch64

# #################### Checking if DEVICE in blacklist #############################
ifeq ($(findstring u280, $(DEVICE)), u280)
$(error [ERROR]: This project is not supported for $(DEVICE).)
endif
ifeq ($(findstring u250, $(DEVICE)), u250)
$(error [ERROR]: This project is not supported for $(DEVICE).)
endif

# #################### Checking if DEVICE in whitelist ############################
ifneq ($(findstring u200, $(DEVICE)), u200)
ifneq ($(findstring zcu102, $(DEVICE)), zcu102)
ifneq ($(findstring zcu104, $(DEVICE)), zcu104)
ifneq ($(findstring u50, $(DEVICE)), u50)
$(warning [WARNING]: This project has not been tested for $(DEVICE). It may or may not work.)
endif
endif
endif
endif

# ######################## Setting up Project Variables #################################
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%/L3/*}')
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
VISLIB_DIR = $(ROOT_DIR)/Vitis_Libraries/vision
COMP_DIR = $(ROOT_DIR)/Vitis_Libraries/data_compression
SEC_DIR = $(ROOT_DIR)/Vitis_Libraries/security

# ######################### Include environment variables in utils.mk ####################
include ./utils.mk
XDEVICE := $(call device2xsa, $(DEVICE))
TEMP_DIR := _x_temp.$(TARGET).$(XDEVICE)
TEMP_REPORT_DIR := $(CUR_DIR)/reports/_x.$(TARGET).$(XDEVICE)
BUILD_DIR := build_dir.$(TARGET).$(XDEVICE)
BUILD_REPORT_DIR := $(CUR_DIR)/reports/_build.$(TARGET).$(XDEVICE)
EMCONFIG := $(BUILD_DIR)/emconfig.json
XCLBIN_DIR := $(CUR_DIR)/$(BUILD_DIR)
export XCL_BINDIR = $(XCLBIN_DIR)

# ####################### Setting environment variables for embedded #####################
ifneq ($(HOST_ARCH), x86)
SYSROOT := $(SDKTARGETSYSROOT)
PERL ?= perl
SDCARD := sd_card
EMU_DIR := $(SDCARD)/data/emulation
endif

# ######################### Setting up Host Variables #########################
#Include Required Host Source Files
CXXFLAGS += -D__SDSVHLS__
CXXFLAGS_VIS += -I$(VISLIB_DIR)/L3/examples/isppipeline/build
CXXFLAGS_VIS += -I$(VISLIB_DIR)/ext/xcl2/
CXXFLAGS_VIS += -I$(VISLIB_DIR)/L1/include

CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/xcl2/
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/compress
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/cmdparser
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/logger
CXXFLAGS_COMP += -I$(COMP_DIR)/L1/include/hw
CXXFLAGS_COMP += -I$(COMP_DIR)/common/thirdParty/xxhash

LDFLAGS += -lopencv_videoio -lopencv_imgcodecs -lopencv_core -lopencv_imgproc -lopencv_features2d -lopencv_flann -lopencv_video -lopencv_calib3d -lopencv_highgui


ifeq ($(TARGET),sw_emu)
CXXFLAGS += -D SW_EMU_TEST
endif

ifeq ($(TARGET),hw_emu)
CXXFLAGS += -D HW_EMU_TEST
endif

# ######################### Host compiler global settings ############################
ifneq ($(HOST_ARCH), x86)
CXXFLAGS += --sysroot=$(SYSROOT)
CXXFLAGS += -I$(SYSROOT)/usr/include/xrt -I$(XILINX_HLS)/include -std=c++14 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label
else
CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_HLS)/include -std=c++11 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label
endif
LDFLAGS += -L$(XILINX_XRT)/lib -lOpenCL -lpthread -lrt -Wno-unused-label -Wno-narrowing -DVERBOSE
CXXFLAGS += -fmessage-length=0 -O3
CXXFLAGS += -I$(CUR_DIR)/src/

ifeq ($(HOST_ARCH), x86)
LDFLAGS += -L$(XILINX_HLS)/lnx64/tools/fpo_v7_0 -Wl,--as-needed -lgmp -lmpfr -lIp_floating_point_v7_0_bitacc_cmodel
endif
ifneq (,$(shell echo $(XPLATFORM) | awk '/u200/'))
CXXFLAGS += -I$(OPENCV_INCLUDE)
LDFLAGS += -L$(OPENCV_LIB)
else ifneq (,$(shell echo $(XPLATFORM) | awk '/zcu102/'))
CXXFLAGS += -I$(SYSROOT)/usr/include
CXXFLAGS += -I$(SYSROOT)/usr/include/opencv4
CXXFLAGS += --sysroot=$(SYSROOT)
LDFLAGS += -L$(SYSROOT)/usr/lib
LDFLAGS += -L${SYSROOT}/opt/xilinx/xrt/lib
else ifneq (,$(shell echo $(XPLATFORM) | awk '/u50/'))
CXXFLAGS += -I$(OPENCV_INCLUDE)
LDFLAGS += -L$o(OPENCV_LIB)
else ifneq (,$(shell echo $(XPLATFORM) | awk '/jdboard/'))
CXXFLAGS += -I$(SYSROOT)/usr/include
CXXFLAGS += -I$(SYSROOT)/usr/include/opencv4
CXXFLAGS += --sysroot=$(SYSROOT)
LDFLAGS += -L$(SYSROOT)/usr/lib
LDFLAGS += -L${SYSROOT}/opt/xilinx/xrt/lib
endif

# ################### Setting package and image directory #######################
EMBEDDED_PACKAGE_OUT = package_$(TARGET)
ifneq (,$(findstring zc706, $(XDEVICE)))
K_IMAGE := $(EDGE_COMMON_SW)/uImage
else
K_IMAGE := $(EDGE_COMMON_SW)/Image
endif

ifeq ($(TARGET),$(filter $(TARGET),sw_emu))
HOST_ARGS :=

PKG_HOST_ARGS = $(foreach args,$(HOST_ARGS),$(subst $(dir $(patsubst %/,%,$(args))),,$(args)))
LIBRARY_PATH =$(OPENCV_LIB):$(LD_LIBRARY_PATH):$(XILINX_XRT)/lib
endif
ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
HOST_ARGS :=

PKG_HOST_ARGS = $(foreach args,$(HOST_ARGS),$(subst $(dir $(patsubst %/,%,$(args))),,$(args)))
LIBRARY_PATH =$(OPENCV_LIB):$(LD_LIBRARY_PATH):$(XILINX_XRT)/lib
endif
ifeq ($(TARGET),$(filter $(TARGET),hw))
HOST_ARGS :=

PKG_HOST_ARGS = $(foreach args,$(HOST_ARGS),$(subst $(dir $(patsubst %/,%,$(args))),,$(args)))
LIBRARY_PATH =$(OPENCV_LIB):$(LD_LIBRARY_PATH):$(XILINX_XRT)/lib
endif

# ##################### Kernel compiler global settings ##########################
VPP_FLAGS += -t $(TARGET) --platform $(XPLATFORM) --save-temps --optimize 2
#VPP_FLAGS += --hls.jobs 8
VPP_LDFLAGS += --vivado.synth.jobs 8 --vivado.impl.jobs 8
VPP_FLAGS += -I$(VISLIB_DIR)/L1/include/

VPP_FLAGS +=
VPP_FLAGS +=

# Kernel args


# ############################ Host applications ###############################
EXE_NAME1 := isppipeline
EXE_FILE1 := $(BUILD_DIR)/$(EXE_NAME1)
EXE_NAME2 := lz4_comp
EXE_FILE2 := $(BUILD_DIR)/$(EXE_NAME2)
EXE_NAME3 := stereopipe
EXE_FILE3 := $(BUILD_DIR)/$(EXE_NAME3)

# ############################ Declaring Binary Containers ##########################

BINARY_CONTAINERS += $(BUILD_DIR)/krnl_ISPPipeline_xo.xclbin
BINARY_CONTAINERS_PKG += $(BUILD_DIR)/krnl_ISPPipeline.xclbin

VPP_LDFLAGS_compress_decompress_streaming_temp := --config $(CUR_DIR)/connectivity.cfg
VPP_LDFLAGS_krnl_ISPPipeline += $(VPP_LDFLAGS_compress_decompress_streaming_temp)

# ######################### Setting Targets of Makefile ################################
DATA_FILE +=

.PHONY: all clean cleanall docs emconfig
ifeq ($(HOST_ARCH), x86)
all:  check_version check_vpp check_platform check_xrt $(EXE_FILE) $(BINARY_CONTAINERS) emconfig
else
all:  check_version check_vpp check_platform check_sysroot $(EXE_FILE) $(BINARY_CONTAINERS) emconfig  sd_card
endif

.PHONY: check_env
check_env:
ifeq (,$(OPENCV_INCLUDE))
	@echo "Cannot find OpenCV include path. Please set OPENCV_INCLUDE variable" && false
endif
ifeq (,$(OPENCV_LIB))
	@echo "Cannot find Opencv lib path. Please set OPENCV_LIB variable" && false
endif

.PHONY: clean cleanall emconfig
emconfig: $(EMCONFIG)

.PHONY: host
ifeq ($(HOST_ARCH), x86)
host:  check_env check_xrt $(EXE_FILE)
else
host:  check_env check_sysroot $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3)
endif

.PHONY: xclbin
ifeq ($(HOST_ARCH), x86)
xclbin: check_vpp  check_xrt  $(BINARY_CONTAINERS)
else
xclbin: check_vpp  check_sysroot  $(BINARY_CONTAINERS)
endif

.PHONY: build
build: xclbin

# ################ Setting Rules for Binary Containers (Building Kernels) ################
ISPPipeline_accel_VPP_FLAGS += -I config
$(TEMP_DIR)/ISPPipeline_accel.xo: xf_isp_accel.cpp
	$(ECHO) "Compiling Kernel: ISPPipeline_accel"
	mkdir -p $(TEMP_DIR)
	echo $(VISLIB_DIR)
	$(VPP) -c $(ISPPipeline_accel_VPP_FLAGS) $(VPP_FLAGS) -k ISPPipeline_accel -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o'$@' $^
BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/ISPPipeline_accel.xo

Stereo_accel_VPP_FLAGS += -I config
$(TEMP_DIR)/Stereo_accel.xo: xf_stereo_pipeline_accel.cpp
	$(ECHO) "Compiling Kernel: Stereo_accel"
	mkdir -p $(TEMP_DIR)
	echo $(VISLIB_DIR)
	$(VPP) -c $(Stereo_accel_VPP_FLAGS) $(VPP_FLAGS) -k stereopipeline_accel -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o'$@' $^
BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/Stereo_accel.xo

VPP_FLAGS_xilLz4CompDecomp += -I $(COMP_DIR)/L2/include -I $(COMP_DIR)/L1/include/hw -I $(SEC_DIR)/L1/include
$(TEMP_DIR)/xilLz4CompressStream.xo: $(COMP_DIR)/L2/src/lz4_compress_stream.cpp
	$(ECHO) "Compiling Kernel: xilLz4CompressStream"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_xilLz4CompDecomp) $(VPP_FLAGS) -k xilLz4CompressStream -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o'$@' '$<'
BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/xilLz4CompressStream.xo

VPP_FLAGS_xilCompDatamover += -I $(COMP_DIR)/L2/include -I $(COMP_DIR)/L1/include/hw -I $(SEC_DIR)/L1/include
$(TEMP_DIR)/xilCompDatamover.xo: $(COMP_DIR)/L2/tests/src/block_comp_dm.cpp
	$(ECHO) "Compiling Kernel: xilCompDatamover"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_xilCompDatamover) $(VPP_FLAGS) -k xilCompDatamover -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
#BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/xilCompDatamover.xo

$(BINARY_CONTAINERS): $(BINARY_CONTAINER_krnl_ISPPipeline_OBJS)
	mkdir -p $(BUILD_DIR)
	$(VPP) -l $(VPP_FLAGS) --temp_dir $(TEMP_DIR) --report_dir $(BUILD_REPORT_DIR)/krnl_ISPPipeline $(VPP_LDFLAGS) $(VPP_LDFLAGS_krnl_ISPPipeline) -o '$@' $(+)


# ################# Setting Rules for Host (Building Host Executable) ################
HOST_SRCS1 += xf_isp_tb.cpp
HOST_SRCS1 += $(VISLIB_DIR)/ext/xcl2/xcl2.cpp

HOST_SRCS2 += lz4.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/lz4OCLHost.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/lz4Base.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/lz4App.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/compressApp.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/xcl2/xcl2.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/cmdparser/cmdlineparser.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/logger/logger.cpp
HOST_SRCS2 += $(COMP_DIR)/common/thirdParty/xxhash/xxhash.c

HOST_SRCS3 += xf_stereo_pipeline_tb.cpp
HOST_SRCS3 += $(VISLIB_DIR)/ext/xcl2/xcl2.cpp

$(EXE_FILE1):  $(HOST_SRCS1) | check_sysroot check_env
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXXFLAGS_VIS) $(LDFLAGS)

$(EXE_FILE2):  $(HOST_SRCS2) | check_sysroot check_env
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXXFLAGS_COMP) $(LDFLAGS)

$(EXE_FILE3):  $(HOST_SRCS3) | check_sysroot check_env
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXXFLAGS_VIS) $(LDFLAGS)


$(EMCONFIG):
	emconfigutil --platform $(XPLATFORM) --od $(BUILD_DIR)
############################## Preparing sdcard folder ##############################
ifneq ($(HOST_ARCH), x86)
RUN_SCRIPT := $(BUILD_DIR)/run_script.sh
$(RUN_SCRIPT):
	rm -rf $(RUN_SCRIPT)
	@echo 'export LD_LIBRARY_PATH=/mnt:/tmp:$(LIBRARY_PATH)' >> $(RUN_SCRIPT)
ifneq ($(filter sw_emu hw_emu, $(TARGET)),)
	@echo 'export XCL_EMULATION_MODE=$(TARGET)' >> $(RUN_SCRIPT)
endif
	@echo 'export XILINX_VITIS=/mnt' >> $(RUN_SCRIPT)
	@echo 'export XILINX_XRT=/usr' >> $(RUN_SCRIPT)
	@echo 'if [ -f platform_desc.txt  ]; then' >> $(RUN_SCRIPT)
	@echo '        cp platform_desc.txt /etc/xocl.txt' >> $(RUN_SCRIPT)
	@echo 'fi' >> $(RUN_SCRIPT)
	@echo './$(EXE_NAME) $(PKG_HOST_ARGS)' >> $(RUN_SCRIPT)
	@echo 'return_code=$$?' >> $(RUN_SCRIPT)
	@echo 'if [ $$return_code -ne 0 ]; then' >> $(RUN_SCRIPT)
	@echo '        echo "ERROR: Embedded host run failed, RC=$$return_code"' >> $(RUN_SCRIPT)
	@echo 'else' >> $(RUN_SCRIPT)
	@echo '        echo "INFO: TEST PASSED, RC=0"' >> $(RUN_SCRIPT)
	@echo 'fi' >> $(RUN_SCRIPT)
	@echo 'echo "INFO: Embedded host run completed."' >> $(RUN_SCRIPT)
	@echo 'exit $$return_code' >> $(RUN_SCRIPT)
DATA_FILE := $(XFLIB_DIR)/data/16bit.png
DATA_DIR :=
SD_FILES += $(RUN_SCRIPT)
SD_FILES += $(EXE_FILE)
SD_FILES += $(EMCONFIG)
SD_FILES += xrt.ini
SD_FILES += $(DATA_FILE)# where define DATAFILE in json
SD_FILES_WITH_PREFIX = $(foreach sd_file,$(SD_FILES), $(if $(filter $(sd_file),$(wildcard $(sd_file))), --package.sd_file $(sd_file)))
SD_DIRS_WITH_PREFIX = $(foreach sd_dir,$(DATA_DIR),--package.sd_dir $(sd_dir))
PACKAGE_FILES := $(BINARY_CONTAINERS)
PACKAGE_FILES += $(AIE_CONTAINER)
SD_CARD := $(CUR_DIR)/package_$(TARGET)
dfx_hw := off
$(SD_CARD): $(EXE_FILE) $(BINARY_CONTAINERS) $(RUN_SCRIPT) $(EMCONFIG) check_kimage check_rootfs
	@echo "Generating sd_card folder...."
	mkdir -p $(SD_CARD)
	chmod a+rx $(BUILD_DIR)/run_script.sh
ifeq ($(findstring _dfx_, $(PLATFORM_NAME)),_dfx_)
ifeq ($(TARGET),hw)
	$(VPP) -t $(TARGET) --platform $(XPLATFORM) -p $(PACKAGE_FILES) $(VPP_PACKAGE) -o $(BINARY_CONTAINERS_PKG)
	$(VPP) -t $(TARGET) --platform $(XPLATFORM) -p --package.out_dir  $(SD_CARD) --package.rootfs $(ROOTFS) --package.kernel_image $(K_IMAGE)  $(SD_FILES_WITH_PREFIX) $(SD_DIRS_WITH_PREFIX) --package.sd_file $(BINARY_CONTAINERS_PKG)
	@echo "### ***** sd_card generation done! ***** ###"
dfx_hw := on
endif
endif
ifeq ($(dfx_hw), off)
	$(VPP) -t $(TARGET) --platform $(XPLATFORM) -o $(BINARY_CONTAINERS_PKG) -p $(PACKAGE_FILES) $(VPP_PACKAGE) --package.out_dir  $(SD_CARD) --package.rootfs $(ROOTFS) --package.kernel_image $(K_IMAGE)  $(SD_FILES_WITH_PREFIX) $(SD_DIRS_WITH_PREFIX)
	@echo "### ***** sd_card generation done! ***** ###"
endif

.PHONY: sd_card
sd_card: $(SD_CARD)
endif
############################## Setting Essential Checks and Building Rules ##############################
RUN_DEPS += $(EXE_FILE) $(BINARY_CONTAINERS) $(EMCONFIG)
RUN_DEPS += $(SD_CARD)

# ################################# Cleaning Rules ##################################
cleanh:
	-$(RMDIR) $(EXE_FILE) vitis_* TempConfig system_estimate.xtxt *.rpt .run/
	-$(RMDIR) src/*.ll _xocc_* .Xil dltmp* xmltmp* *.log *.jou *.wcfg *.wdb sample_link.ini sample_compile.ini obj* bin* *.csv *.jpg *.jpeg *.png

cleank:
	-$(RMDIR) $(BUILD_DIR)/*.xclbin _vimage *xclbin.run_summary qemu-memory-_* emulation/ _vimage/ start_simulation.sh *.xclbin
	-$(RMDIR) _x_temp.*/_x.* _x_temp.*/.Xil _x_temp.*/profile_summary.* xo_* _x*
	-$(RMDIR) _x_temp.*/dltmp* _x_temp.*/kernel_info.dat _x_temp.*/*.log
	-$(RMDIR) _x_temp.*

cleanall: cleanh cleank
	-$(RMDIR) $(BUILD_DIR) sd_card* build_dir.* emconfig.json *.html $(TEMP_DIR) $(CUR_DIR)/reports *.csv *.run_summary $(CUR_DIR)/*.raw package_* run_script.sh .ipcache *.str
	-$(RMDIR) $(VISLIB_DIR)/common/data/*.xe2xd* $(VISLIB_DIR)/common/data/*.orig*

	-$(RMDIR) $(AIE_CONTAINERS) $(CUR_DIR)/Work $(CUR_DIR)/*.xpe $(CUR_DIR)/hw.o $(CUR_DIR)/*.xsa $(CUR_DIR)/xnwOut aiesimulator_output .AIE_SIM_CMD_LINE_OPTIONS

clean: cleanh
