#!/system/bin/sh

echo 3 > sys/class/leds/jogball-backlight/brightness

sleep 3

echo 0 > sys/class/leds/jogball-backlight/brightness

insmod system/lib/modules/loop.ko
insmod system/lib/modules/cryptoloop.ko
