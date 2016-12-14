LOCAL_PATH := $(call my-dir)

CFG_TEEC_PUBLIC_PATH = $(LOCAL_PATH)/../../../ca_export_$(TARGET_ARCH)

################################################################################
# Build provision                                                              #
################################################################################
include $(CLEAR_VARS)
LOCAL_CFLAGS += -DANDROID_BUILD
LOCAL_CFLAGS += -Wall

LOCAL_SRC_FILES += provision.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ta/include \
		$(CFG_TEEC_PUBLIC_PATH)/include

LOCAL_SHARED_LIBRARIES := libteec
LOCAL_MODULE := tee_provision
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
