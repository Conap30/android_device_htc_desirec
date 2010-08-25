# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# config.mk
#
# Product-specific compile-time definitions.
#

# WARNING: This line must come *before* including the proprietary
# variant, so that it gets overwritten by the parent (which goes
# against the traditional rules of inheritance).

USE_CAMERA_STUB := true

# inherit from the proprietary version
-include vendor/htc/desirec/BoardConfigVendor.mk

TARGET_NO_BOOTLOADER := true

TARGET_BOARD_PLATFORM := msm7k
TARGET_BOARD_PLATFORM_GPU := qcom

# ARMv6-compatible processor rev 5 (v6l)
TARGET_CPU_ABI := armeabi-v6l
TARGET_CPU_ABI2 := armeabi
TARGET_ARCH_VARIANT := armv5te
TARGET_BOOTLOADER_BOARD_NAME := desirec

# Wifi related defines
BOARD_WPA_SUPPLICANT_DRIVER := CUSTOM
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := libWifiApi
BOARD_WLAN_TI_STA_DK_ROOT   := system/wlan/ti/sta_dk_4_0_4_32
WIFI_DRIVER_MODULE_PATH     := "/system/lib/modules/wlan.ko"
WIFI_DRIVER_MODULE_ARG      := ""
WIFI_DRIVER_MODULE_NAME     := "wlan"
WIFI_FIRMWARE_LOADER        := "wlan_loader"

BOARD_USES_GENERIC_AUDIO := false

# Use HTC USB Function Switch to enable tethering via USB
BOARD_USE_HTC_USB_FUNCTION_SWITCH := true

BOARD_KERNEL_CMDLINE := no_console_suspend=1 console=null
BOARD_KERNEL_BASE := 0x11200000

BOARD_HAVE_BLUETOOTH := true

BOARD_EGL_CFG := device/htc/desirec/egl.cfg

BOARD_VENDOR_USE_AKMD := akm8973

BOARD_VENDOR_QCOM_AMSS_VERSION := 4410

BOARD_USES_QCOM_LIBS := true

BOARD_USES_ECLAIR_LIBCAMERA := true

# # cat /proc/mtd
# dev:    size   erasesize  name
# mtd0: 000a0000 00020000 "misc"
# mtd1: 00420000 00020000 "recovery"
# mtd2: 002c0000 00020000 "boot"
# mtd3: 0f000000 00020000 "system"
# mtd4: 05000000 00020000 "cache"
# mtd5: 09120000 00020000 "userdata"
BOARD_BOOTIMAGE_PARTITION_SIZE := 002c0000
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00420000
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 0x0f000000
BOARD_USERDATAIMAGE_PARTITION_SIZE := 0x01920000
# The size of a block that can be marked bad.
BOARD_FLASH_BLOCK_SIZE := 131072

TARGET_RECOVERY_UI_LIB := librecovery_ui_heroc librecovery_ui_generic

TARGET_RECOVERY_UPDATER_LIBS += librecovery_updater_htc

TARGET_RELEASETOOLS_EXTENSIONS := device/htc/common

PRODUCT_SPECIFIC_DEFINES += TARGET_PRELINKER_MAP=\$(TOP)/device/htc/desirec/prelink-linux-arm-desirec.map
