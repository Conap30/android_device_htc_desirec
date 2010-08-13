#!/bin/sh

# Copyright (C) 2010 The Android Open Source Project
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

# Use this in addition to extract-files.sh to get up-to-date verions
# of those files.
unzip -j -o ../../../signed-dream_devphone_userdebug-ota-14721.zip system/etc/AudioPara4.csv system/etc/firmware/brf6300.bin -d ../../../vendor/htc/dream/proprietary
