export V?=0
CA_MODE64 = 64
CA_MODE32 = 32
cur_dir=$(CURDIR)

export CA_CROSS_COMPILE_64=aarch64-linux-gnu-
export CA_CROSS_COMPILE_32=arm-linux-gnueabihf-
export TA_CROSS_COMPILE=arm-linux-gnueabihf-

export TDK_DIR=$(cur_dir)/../tdk

.PHONY: all
all: ca ta

.PHONY: ca
ca: tee_provision
tee_provision:
	make -C ca CROSS_COMPILE=$(CA_CROSS_COMPILE_32) MODE=$(CA_MODE32)
	make -C ca clean-objs

.PHONY: ta
ta:
	make -C ta CROSS_COMPILE=$(TA_CROSS_COMPILE)

.PHONY: clean
clean:
	make -C ta clean
	make -C ca clean
