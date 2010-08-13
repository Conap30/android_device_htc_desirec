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

#
# This file is the build configuration for a full Android
# build for dream hardware. This cleanly combines a set of
# device-specific aspects (drivers) with a device-agnostic
# product configuration (apps).
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_small.mk)
$(call inherit-product, device/htc/desirec/device_desirec_us.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full.mk)


PRODUCT_PROPERTY_OVERRIDES += \
        ro.com.android.wifi-watchlist=GoogleGuest \
        ro.error.receiver.system.apps=com.google.android.feedback \
        ro.setupwizard.enterprise_mode=1 \
        ro.com.google.clientidbase=android-verizon \
        ro.com.google.locationfeatures=1 \
        ro.url.legal=http://www.google.com/intl/%s/mobile/android/basic/phone-legal.html \
        ro.url.legal.android_privacy=http://www.google.com/intl/%s/mobile/android/basic/privacy.html \
        ro.cdma.home.operator.numeric=310012 \
        ro.cdma.home.operator.alpha=verizon \
        ro.cdma.homesystem=128,64 \
        ro.cdma.data_retry_config=default_randomization=960000,960000,960000,960000,960000 \
        ro.config.vc_call_vol_steps=7 \
        ro.telephony.call_ring.multiple=false \
        ro.telephony.call_ring.delay=3000 \
        ro.setupwizard.enable_bypass=1 \
        ro.media.dec.jpeg.memcap=20000000 \
        dalvik.vm.lockprof.threshold=500 \
        dalvik.vm.dexopt-flags=m=y


# Discard inherited values and use our own instead.
PRODUCT_NAME := froyo_desirec	
PRODUCT_DEVICE := desirec
PRODUCT_MODEL := AOSP Froyo on desireC
