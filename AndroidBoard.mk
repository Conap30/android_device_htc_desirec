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

LOCAL_PATH := $(call my-dir)

$(call add-radio-file,recovery/images/firmware_install.565)

file := $(TARGET_OUT_KEYLAYOUT)/desirec-keypad.kl
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/desirec-keypad.kl | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_ROOT_OUT)/init.desirec.rc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/init.desirec.rc | $(ACP)
	$(transform-prebuilt-to-target)

include device/htc/desirec/AndroidBoardCommon.mk

-include vendor/htc/desirec/AndroidBoardVendor.mk
