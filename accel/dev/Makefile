ROOT_DIR=$(shell git rev-parse --show-toplevel)
# When we source the SDK we get the ENV SDKTARGETSYSROOT which is the sysroot
SYSROOT = $(SDKTARGETSYSROOT)
VISLIB_DIR = $(ROOT_DIR)/Vitis_Libraries/vision

CXXFLAGS += -fmessage-length=0 -std=c++14 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label
LDFLAGS += -pthread -L$(XILINX_HLS)/lnx64/tools/fpo_v7_0  -Wl,--as-needed -lgmp -lmpfr -lIp_floating_point_v7_0_bitacc_cmodel
CXXFLAGS += -I$(VISLIB_DIR)/L1/include
CXXFLAGS += -I/usr/local/include/opencv4
CXXFLAGS += -I$(XILINX_HLS)/include

LDFLAGS += -l opencv_videoio -l opencv_imgcodecs -l opencv_core -l opencv_imgproc
LDFLAGS += -l opencv_features2d -l opencv_flann -l opencv_video -l opencv_calib3d
LDFLAGS += -l opencv_highgui -lopencv_core
LDFLAGS += -L/usr/local/lib

SRC_FILES = isp_test.cpp xf_isp_accel.cpp xf_cvt_color_accel.cpp xf_resize_accel.cpp xf_custom_convolution_accel.cpp
OBJDIR = obj
#OBJS = isp_test.o xf_isp_accel.o
OBJS := $(SRC_FILES:%.cpp=%.o)
VPATH = .:..

isp_test: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all
all: isp_test

clean:
	rm -f isp_test
	rm -f *.o