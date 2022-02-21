# vitis_isp
Vitis ISP Tests

This repo has a simple DCT test showing the GUI flow for HLS development.

I then took the isppipeline and made it into a HLS sim so a person could
quickly develop on the host. You can go to <vitis_library>/vision/L3/examples/isppipeline
and run their make file to re-create the ISP on hardware. There is a handy
script to source all of the environment variables.

# Build
Install Vitis and Petalilnux (see Xilinx docs)

Checkout this repo.

## Build Linux
Source the petalinux **settings.sh** file. In my installation it is:
`
. ~/xilinx/Petalinux/2021.2/settings.sh
cd <vitis_isp>/plinux
petalinux-build
petalinuxbuild -s
cd images/linux
./sdk.sh
`

This will take some time. But it will build the Linux distro, SDK, and install the SDK.

## Build DCT demo
Open Vitis_HLS tool and load the dct project.

## Build the Vitis DCT hardware project
Run the following, you may need to tweak **env.sh** based on the location of
various components. Refer to **env.sh** for details.

```
. <xilinx install directory>/Vitis/2021.2/settings64.sh
. <vitis_isp checkout directory>/scripts/env.sh
cd <vitis_isp checkout directory>/dct_example
make all TARGET=hw
```

This will take some time, but create a sd_card.img file. You can **dd** this
file onto to an SD card and boot the unit. To run the dct example, do the
following on the booted unit:
```
cd /media/sd-mmcblk0p1
./run_script.sh
```

This will load the xclbin file and run the **dct_example** program.

# Y2K22 bug fix
Xilinx has issues with 2022. See this post for a fix
https://support.xilinx.com/s/article/76960?language=en_US


# Boot faster
To boot faster grab the **pmufw**, **system.bit**, **bl31.elf**, and
**system.dtb** from the Extensible platform design.
The Petalinux build doesn't work for some reason.
Then grab the fsbl and u-boot from the Petalinux build.

Normally I copy :
**pmufw**
**system.bit**
**bl31.elf**
**system.dtb**

into the <vitis_isp directory>/plinux/images/linux directory, overwritting
the petalinux builds.

Then I run the following:
```
petalinux-package --boot --u-boot --fpga system.bit --force
```

This will create a new BOOT.BIN file that you can copy to the SD card.

Next I create a new boot.scr file:

```
# Generate boot.scr:
# mkimage -c none -A arm -T script -d boot.txt boot.scr
#
################
#setenv bootargs "rootwait root=/dev/mmcblk0p2 quiet systemd.show_status=0"
setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait"

#fatload mmc 0 0x01000000 Image.gz
sf probe
sf read 0x01000000 0x1420000 0x989680
unzip 0x01000000 0x00200000
booti 0x00200000 - 0x00100000
```

I then compress Image using gzip and copy it to the boot partition on the SD
card.
```
gzip -k Image
```

We now put the **BOOT.BIN** and the **Image.gz** in the QSPI flash. Use
the commands below in U-boot
```
fatload mmc 0 0x80000 BOOT.BIN
sf probe
sf update 0x80000 0 ${filesize}
fatload mmc 0 0x80000 Image.gz
sf update 0x80000 0x1420000 ${filesize}
fatload mmc 0 0x80000 boot.scr
sf update 0x80000 0x2000000 ${filesize}
```

Shut the unit off, configure it to boot from QSPI. Then boot. Loading the
BIT file, u-boot and the kernel should only take a few seconds now.

When u-boot starts up change the bootargs to:
```
setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait quiet systemd.show_status=0 init=/sbin/bootchartd"
saveenv
```

You can also create a script to do this update:
```
setenv update_boot "fatload mmc 0 0x80000 BOOT.BIN;sf probe; sf update 0x80000 0 ${filesize};fatload mmc 0 0x80000 boot.scr;sf update 0x80000 0x2000000 ${filesize}"
```

The unit should boot within 15-20 seconds.

## U-boot changes
Remove as many drivers as possible.

Set this guy:
CONFIG_SYS_MALLOC_CLEAR_ON_INIT=n

## Profile

I can't get boot chart to work. It will generate a bootchart.tgz, but pybootchartgui.py doesn't work.
`setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=1536M quiet systemd.show_status=0 init=/sbin/bootchartd"`


To profile kernel calls do the following:
Change the kernel config **CONFIG_LOG_BUF_SHIFT** and increase it by one.
Boot to u-boot and type the following then boot
`setenv bootargs "earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p2 rw rootwait cma=1536M quiet systemd.show_status=0 initcall_debug"`

One the unit is booted type:
`dmesg > boot.log`

Copy that file to the PC and in the kernel source tree run:
`scripts/bootgraph.pl boot.log > boot.svg`

# Start GStreamer app on startup
Petalinux uses SysV, which is terrible.

To start Gstreamer on the host use the following:
`gst-launch-1.0 udpsrc port=5000 ! 'application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)JPEG, a-framerate=(string)30.000000, payload=(int)26, ssrc=(uint)3167073459, timestamp-offset=(uint)2147749579, seqnum-offset=(uint)3676' ! rtpjitterbuffer latency=50 ! rtpjpegdepay ! jpegdec ! glimagesink`

