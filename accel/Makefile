# Copyright 2019-2022 Xilinx, Inc.
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
# vitis makefile-generator v2.0.6

############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<hw/hw_emu/sw_emu/> PLATFORM=<FPGA platform> HOST_ARCH=<x86/aarch64>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<hw/hw_emu/sw_emu/> PLATFORM=<FPGA platform> HOST_ARCH=<x86/aarch64>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make xclbin TARGET=<hw/hw_emu/sw_emu/> PLATFORM=<FPGA platform> HOST_ARCH=<x86/aarch64>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  make host TARGET=<hw/hw_emu/sw_emu/>"
	$(ECHO) "      Command to build host application."
	$(ECHO) "      By default, HOST_ARCH=x86. HOST_ARCH is required for SoC shells"
	$(ECHO) ""
	$(ECHO) "  NOTE: For embedded devices, e.g. zcu102/zcu104/vck190, HOST_ARCH is either aarch32 or aarch64."
	$(ECHO) "      a.IF Download the platform, and common-image from Xilinx Download Center(Suggested):"
	$(ECHO) "        Run the sdk.sh script from the common-image directory to install sysroot using the command : ./sdk.sh -y -d ./ -p "
	$(ECHO) "        Unzip the rootfs file : gunzip ./rootfs.ext4.gz"
	$(ECHO) "        export SYSROOT=< path-to-platform-sysroot >"
	$(ECHO) "      b. User could also define SYSROOT, K_IMAGE and ROOTFS by themselves: "
	$(ECHO) "        export SYSROOT=< path-to-platform-sysroot >"
	$(ECHO) "        export K_IMAGE=< path-to-Image-files >"
	$(ECHO) "        export ROOTFS=< path-to-rootfs >"
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

############################## Setting up Project Variables ##############################

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))

ROOT_DIR=$(shell git rev-parse --show-toplevel)
OPENCV_INCLUDE = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/opencv4
OPENCV_LIB = /opt/petalinux/2022.1/sysroots/cortexa72-cortexa53-xilinx-linux/usr/lib
# When we source the SDK we get the ENV SDKTARGETSYSROOT which is the sysroot
SYSROOT = $(SDKTARGETSYSROOT)
DEVICE = $(ROOT_DIR)/platform/jdboard/export/jdboard/jdboard.xpfm
#DEVICE=/home/nlbutts/xilinx/Vitis/2022.1/base_platforms/xilinx_zcu104_base_202210_1/xilinx_zcu104_base_202210_1.xpfm
TARGET ?= hw
HOST_ARCH ?= aarch64
VISLIB_DIR = $(ROOT_DIR)/Vitis_Libraries/vision
COMP_DIR = $(ROOT_DIR)/Vitis_Libraries/data_compression
SEC_DIR = $(ROOT_DIR)/Vitis_Libraries/security
K_IMAGE = $(ROOT_DIR)/plinux/images/linux/Image
ROOTFS = $(ROOT_DIR)/plinux/images/linux/rootfs.ext4
XILINX_XRT = $(ROOT_DIR)/XRT

#setting PLATFORM
ifeq ($(PLATFORM),)
PLATFORM := $(DEVICE)
endif
ifeq ($(PLATFORM),)
PLATFORM := xilinx_u200_gen3x16_xdma_2_202110_1
endif

# #################### Checking if PLATFORM in whitelist ############################
PLATFORM_ALLOWLIST +=  u200 zcu102 zcu104 u50
PLATFORM_BLOCKLIST +=  u280 u250

include ./utils.mk
TEMP_DIR := _x_temp.$(TARGET).$(PLATFORM_NAME)
TEMP_REPORT_DIR := $(CUR_DIR)/reports/_x.$(TARGET).$(PLATFORM_NAME)
BUILD_DIR := build_dir.$(TARGET).$(PLATFORM_NAME)
ifneq ($(RESULT_DIR),)
BUILD_DIR = $(RESULT_DIR)
endif
BUILD_REPORT_DIR := $(CUR_DIR)/reports/_build.$(TARGET).$(PLATFORM_NAME)
EMCONFIG := $(BUILD_DIR)/emconfig.json
XCLBIN_DIR := $(CUR_DIR)/$(BUILD_DIR)
export XCL_BINDIR = $(XCLBIN_DIR)

EXE_FILE_DEPS :=
BINARY_CONTAINERS_DEPS :=
RUN_DEPS :=

# get global setting
ifeq ($(HOST_ARCH), x86)
CXXFLAGS += -fmessage-length=0 -I$(CUR_DIR)/src/ -I$(XILINX_XRT)/include -I$(XILINX_HLS)/include -std=c++14 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label
LDFLAGS += -pthread -L$(XILINX_XRT)/lib -L$(XILINX_HLS)/lnx64/tools/fpo_v7_0  -Wl,--as-needed -lOpenCL -lxrt_coreutil -lgmp -lmpfr -lIp_floating_point_v7_0_bitacc_cmodel
VPP_FLAGS += -t $(TARGET) --platform $(XPLATFORM) --save-temps
VPP_LDFLAGS += --optimize 2 -R 2
else ifeq ($(HOST_ARCH), aarch64)
CXXFLAGS += -I$(CUR_DIR)/src/ -fmessage-length=0 --sysroot=$(SYSROOT)  -I$(SYSROOT)/usr/include/xrt -I$(XILINX_HLS)/include -std=c++14 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label
LDFLAGS += -pthread -L$(SYSROOT)/usr/lib -L$(XILINX_VITIS_AIETOOLS)/lib/aarch64.o -Wl,--as-needed -lxilinxopencl -lxrt_coreutil
VPP_FLAGS += -t $(TARGET) --platform $(XPLATFORM) --save-temps --config connectivity.cfg
VPP_LDFLAGS += --optimize 2 -R 2
VPP_LDFLAGS_krnl_ISPPipeline += --profile.data all:all:all
endif
CXXFLAGS += $(EXTRA_CXXFLAGS)
VPP_FLAGS += $(EXTRA_VPP_FLAGS)

########################## Setting up Host Variables ##########################
ifeq ($(TARGET),sw_emu)
CXXFLAGS += -D SW_EMU_TEST
endif
ifeq ($(TARGET),hw_emu)
CXXFLAGS += -D HW_EMU_TEST
endif

#Inclue Required Host Source Files
HOST_SRCS1 += xf_isp_tb.cpp $(VISLIB_DIR)/ext/xcl2/xcl2.cpp

HOST_SRCS2 += lz4.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/lz4OCLHost.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/lz4Base.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/lz4App.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/compress/compressApp.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/xcl2/xcl2.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/cmdparser/cmdlineparser.cpp
HOST_SRCS2 += $(COMP_DIR)/common/libs/logger/logger.cpp
HOST_SRCS2 += $(COMP_DIR)/common/thirdParty/xxhash/xxhash.c
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/xcl2/
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/compress
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/cmdparser
CXXFLAGS_COMP += -I$(COMP_DIR)/common/libs/logger
CXXFLAGS_COMP += -I$(COMP_DIR)/L1/include/hw
CXXFLAGS_COMP += -I$(COMP_DIR)/common/thirdParty/xxhash

HOST_SRCS3 += xf_stereo_pipeline_tb.cpp $(VISLIB_DIR)/ext/xcl2/xcl2.cpp
CXXFLAGS_VIS += -I$(VISLIB_DIR)/L1/include
CXXFLAGS_VIS += -I$(VISLIB_DIR)/ext/xcl2/

CXXFLAGS +=  -D __SDSVHLS__
CXXFLAGS +=  -I $(SYSROOT)/usr/include -I $(SYSROOT)/usr/include/opencv4
CXXFLAGS += --sysroot=$(SYSROOT)
LDFLAGS +=  -L $(SYSROOT)/usr/lib -L ${SYSROOT}/opt/xilinx/xrt/lib
LDFLAGS +=  -l opencv_videoio -l opencv_imgcodecs -l opencv_core -l opencv_imgproc -l opencv_features2d -l opencv_flann -l opencv_video -l opencv_calib3d -l opencv_highgui

# workaround for opencv
ifeq (,$(findstring opencv,$(CXXFLAGS)))
CXXFLAGS += $(XRT_CXXFLAGS)
endif

EXE_NAME1 := isppipeline
EXE_FILE1 := $(BUILD_DIR)/$(EXE_NAME1)
EXE_FILE_DEPS1 := $(HOST_SRCS1) $(EXE_FILE_DEPS1)
EXE_NAME2 := lz4_comp
EXE_FILE2 := $(BUILD_DIR)/$(EXE_NAME2)
EXE_FILE_DEPS2 := $(HOST_SRCS2) $(EXE_FILE_DEPS2)
EXE_NAME3 := stereopipe
EXE_FILE3 := $(BUILD_DIR)/$(EXE_NAME3)
EXE_FILE_DEPS3 := $(HOST_SRCS3) $(EXE_FILE_DEPS3)

HOST_ARGS := ../data/16bit.png
PKG_HOST_ARGS = $(foreach args,$(HOST_ARGS),$(subst $(dir $(patsubst %/,%,$(args))),,$(args)))
LIBRARY_PATH := $(OPENCV_LIB):$(LD_LIBRARY_PATH):$(XILINX_XRT)/lib

########################## Kernel compiler global settings ##########################
VPP_FLAGS +=  -I config -I $(VISLIB_DIR)/L1/include/

######################### binary container global settings ##########################

ifeq ($(HOST_ARCH), x86)
BINARY_CONTAINERS += $(BUILD_DIR)/krnl_ISPPipeline.xclbin
else
BINARY_CONTAINERS += $(BUILD_DIR)/krnl_ISPPipeline_pkg.$(LINK_TARGET_FMT)
BINARY_CONTAINERS_PKG += $(BUILD_DIR)/krnl_ISPPipeline.xclbin
endif

# ################ Setting Rules for Binary Containers (Building Kernels) ################
$(TEMP_DIR)/ISPPipeline_accel.xo: xf_isp_accel.cpp
	$(ECHO) "Compiling Kernel: ISPPipeline_accel"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_ISPPipeline_accel) $(VPP_FLAGS) -k ISPPipeline_accel -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/ISPPipeline_accel.xo

$(TEMP_DIR)/RGB2YUV_accel.xo: RGB2YUV_accel.cpp
	$(ECHO) "Compiling Kernel: RGB2YUV_accel"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_ISPPipeline_accel) $(VPP_FLAGS) -k RGB2YUV_accel -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
#BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/RGB2YUV_accel.xo

$(TEMP_DIR)/Stereo_accel.xo: xf_stereo_pipeline_accel.cpp
	$(ECHO) "Compiling Kernel: Stereo_accel"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_ISPPipeline_accel) $(VPP_FLAGS) -k stereopipeline_accel -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o'$@' $^
#BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/Stereo_accel.xo

VPP_FLAGS_xilLz4CompDecomp += -I $(COMP_DIR)/L2/include -I $(COMP_DIR)/L1/include/hw -I $(SEC_DIR)/L1/include
$(TEMP_DIR)/xilLz4CompressStream.xo: $(COMP_DIR)/L2/src/lz4_compress_stream.cpp
	$(ECHO) "Compiling Kernel: xilLz4CompressStream"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_xilLz4CompDecomp) $(VPP_FLAGS) -k xilLz4CompressStream -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o'$@' '$<'
#BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/xilLz4CompressStream.xo

VPP_FLAGS_xilCompDatamover += -I $(COMP_DIR)/L2/include -I $(COMP_DIR)/L1/include/hw -I $(SEC_DIR)/L1/include
$(TEMP_DIR)/xilCompDatamover.xo: $(COMP_DIR)/L2/tests/src/block_comp_dm.cpp
	$(ECHO) "Compiling Kernel: xilCompDatamover"
	mkdir -p $(TEMP_DIR)
	$(VPP) -c $(VPP_FLAGS_xilCompDatamover) $(VPP_FLAGS) -k xilCompDatamover -I'$(<D)' --temp_dir $(TEMP_DIR) --report_dir $(TEMP_REPORT_DIR) -o $@ $^
#BINARY_CONTAINER_krnl_ISPPipeline_OBJS += $(TEMP_DIR)/xilCompDatamover.xo


BINARY_CONTAINERS_DEPS += $(BINARY_CONTAINER_krnl_ISPPipeline_OBJS)
$(BINARY_CONTAINERS): $(BINARY_CONTAINERS_DEPS)
	mkdir -p $(BUILD_DIR)
	$(VPP) -l $(VPP_FLAGS) --temp_dir $(TEMP_DIR) --report_dir $(BUILD_REPORT_DIR)/krnl_ISPPipeline $(VPP_LDFLAGS)  $(VPP_LDFLAGS_krnl_ISPPipeline) $(AIE_LDFLAGS) --config connectivity.cfg  -o $@ $^

############################## Setting Rules for Host (Building Host Executable) ##############################
ifeq ($(HOST_ARCH), x86)
$(EXE_FILE1): $(EXE_FILE_DEPS1) | check_env check_xrt
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
$(EXE_FILE2): $(EXE_FILE_DEPS2) | check_env check_xrt
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
$(EXE_FILE3): $(EXE_FILE_DEPS3) | check_env check_xrt
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

else
$(EXE_FILE1): $(EXE_FILE_DEPS1) | check_env check_sysroot
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXXFLAGS_VIS) $(LDFLAGS)
$(EXE_FILE2): $(EXE_FILE_DEPS2) | check_env check_sysroot
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXXFLAGS_COMP) $(LDFLAGS)
$(EXE_FILE3): $(EXE_FILE_DEPS3) | check_env check_sysroot
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(CXXFLAGS_VIS) $(LDFLAGS)
endif

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
DATA_FILE := ../data/16bit.png
DATA_DIR :=
SD_FILES += $(RUN_SCRIPT)
SD_FILES += $(EXE_FILE1)
SD_FILES += $(EXE_FILE2)
SD_FILES += $(EXE_FILE3)
SD_FILES += $(EMCONFIG)
SD_FILES += system.dtb
SD_FILES += boot.scr
SD_FILES += xrt.ini
SD_FILES += $(DATA_FILE)# where define DATAFILE in json
SD_FILES_WITH_PREFIX = $(foreach sd_file,$(SD_FILES), $(if $(filter $(sd_file),$(wildcard $(sd_file))), --package.sd_file $(sd_file)))
SD_DIRS_WITH_PREFIX = $(foreach sd_dir,$(DATA_DIR),--package.sd_dir $(sd_dir))
PACKAGE_FILES := $(BINARY_CONTAINERS)
PACKAGE_FILES += $(AIE_CONTAINER)
SD_CARD := $(CUR_DIR)/package_$(TARGET)
dfx_hw := off
$(SD_CARD): $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(BINARY_CONTAINERS) $(RUN_SCRIPT) $(EMCONFIG) check_kimage check_rootfs
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
RUN_DEPS += $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(BINARY_CONTAINERS) $(EMCONFIG)
RUN_DEPS += $(SD_CARD)

.PHONY: mkflag all run
mkflag:
	mkdir -p $(BUILD_DIR)
	rm -rf $(BUILD_DIR)/makefile_args.txt
	@for var in $(MAKEFLAGS); do echo $$var >> $(BUILD_DIR)/makefile_args.txt; done
#all: check_device  check_vpp check_platform mkflag $(RUN_DEPS)
all: check_vpp check_platform mkflag $(RUN_DEPS)
run: all
#hw_emu
ifneq (,$(filter hw_emu, $(TARGET)))
ifeq ($(HOST_ARCH), x86)
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
	XCL_EMULATION_MODE=$(TARGET) $(EXE_FILE1) $(HOST_ARGS)

else
	@echo $(RUN_DEPS)
	$(SD_CARD)/launch_$(TARGET).sh -no-reboot -run-app $(notdir $(RUN_SCRIPT))
	grep "TEST PASSED, RC=0" $(SD_CARD)/qemu_output.log || exit 1

endif
endif
#sw_emu
ifneq (,$(filter sw_emu, $(TARGET)))
ifeq ($(HOST_ARCH), x86)
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
	XCL_EMULATION_MODE=$(TARGET) $(EXE_FILE1) $(HOST_ARGS)

else
	@echo $(RUN_DEPS)
	$(SD_CARD)/launch_$(TARGET).sh -no-reboot -run-app $(notdir $(RUN_SCRIPT))
	grep "TEST PASSED, RC=0" $(SD_CARD)/qemu_output.log || exit 1

endif
endif
#hw
ifeq ($(TARGET), hw)
ifneq (,$(findstring aws-vu9p-f1, $(PLATFORM_NAME)))
ifeq (,$(wildcard $(BUILD_DIR)/krnl_ISPPipeline.awsxclbin))
	$(ECHO) "This makefile does not directly support converting .xclbin to .awsxclbin, please refer https://github.com/aws/aws-fpga/blob/master/Vitis/README.md for next operations"
else
	$(ECHO) "Running HW using generated .awsxclbin"
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
	$(EXE_FILE1) $(subst .xclbin,.awsxclbin,$(HOST_ARGS))

endif
else ifeq ($(HOST_ARCH), x86)
	LD_LIBRARY_PATH=$(LIBRARY_PATH):$$LD_LIBRARY_PATH \
	$(EXE_FILE1) $(HOST_ARGS)

else
	$(ECHO) "Please copy the content of sd_card folder and data to an SD Card and run on the board"
endif
endif

############################## Setting Targets ##############################

.PHONY: clean cleanall emconfig
emconfig: $(EMCONFIG)

.PHONY: host
ifeq ($(HOST_ARCH), x86)
host:  check_env check_xrt $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3)
else
host:  check_env check_sysroot $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3)
endif

.PHONY: xclbin
ifeq ($(HOST_ARCH), x86)
xclbin:  check_env check_vpp check_xrt $(BINARY_CONTAINERS)
else
xclbin:  check_env check_vpp check_sysroot $(BINARY_CONTAINERS)
endif

.PHONY: check_env
check_env:

############################## Cleaning Rules ##############################
cleanh:
	-$(RMDIR) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) vitis_* TempConfig system_estimate.xtxt *.rpt .run/
	-$(RMDIR) src/*.ll _xocc_* .Xil dltmp* xmltmp* *.log *.jou *.wcfg *.wdb sample_link.ini sample_compile.ini obj*  bin* *.csv *.jpg *.jpeg *.png

cleank:
	-$(RMDIR) $(BUILD_DIR)/*.xclbin _vimage *xclbin.run_summary qemu-memory-_* emulation/ _vimage/ pl*start_simulation. sh *.xclbin
	-$(RMDIR) _x_temp.*

cleanall: cleanh cleank
	-$(RMDIR) $(BUILD_DIR)  emconfig.json *.html $(TEMP_DIR) $(CUR_DIR)/reports *.csv *.run_summary  $(CUR_DIR)/*.raw package_*   $(BUILD_DIR)/run_script.sh .ipcache *.str
	-$(RMDIR)  $(CUR_DIR)/Work $(CUR_DIR)/*.xpe $(CUR_DIR)/hw.o $(CUR_DIR)/*.xsa $(CUR_DIR)/xnwOut
	-$(RMDIR)

clean: cleanh