# Copyright (C) 2007 The Android Open Source Project
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

-include vendor/htc/desirec/BoardConfigVendor.mk

TARGET_BOOTLOADER_BOARD_NAME := desirec

# # cat /proc/mtd
# dev:    size   erasesize  name
# mtd0: 000c0000 00020000 "hboot"
# mtd1: 00500000 00020000 "recovery"
# mtd2: 00280000 00020000 "boot"
# mtd3: 04380000 00020000 "system"
# mtd4: 04380000 00020000 "cache"
# mtd5: 04ac0000 00020000 "userdata"
#BOARD_BOOTIMAGE_PARTITION_SIZE := 0x00280000
#BOARD_RECOVERYIMAGE_PARTITION_SIZE := 0x00500000
#BOARD_SYSTEMIMAGE_PARTITION_SIZE := 0x04380000
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 0x04ac0000

BOARD_BOOTIMAGE_MAX_SIZE := 2621440
BOARD_RECOVERYIMAGE_MAX_SIZE := 5242880
BOARD_SYSTEMIMAGE_MAX_SIZE := 70778880
BOARD_USERDATAIMAGE_MAX_SIZE := 78381056
# The size of a block that can be marked bad.
BOARD_FLASH_BLOCK_SIZE := 131072
BOARD_CAMERA_LIBRARIES := libcameraservice libcamera

BOARD_KERNEL_BASE := 0x11200000

BOARD_USES_QCOM_LIBS := true

BOARD_USES_ECLAIR_LIBCAMERA := true


TARGET_RECOVERY_UI_LIB := librecovery_ui_desirec librecovery_ui_htc

include device/htc/desirec/BoardConfigCommon.mk
