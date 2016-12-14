CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

OBJS = provision.o

ifeq ($(MODE), 64)
TEEC_EXPORT=$(TDK_DIR)/ca_export_arm64
endif

ifeq ($(MODE), 32)
TEEC_EXPORT=$(TDK_DIR)/ca_export_arm
endif

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include
LDADD += -lteec -L$(TEEC_EXPORT)/lib

.PHONY: all
all: tee_provision

tee_provision: $(OBJS)
	$(CC) $(LDADD) -o $@ $<

.PHONY: clean clean-objs
clean-objs:
	rm -f $(OBJS)
clean:
	rm -f tee_provision
