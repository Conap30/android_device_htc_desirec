#!/system/bin/sh

echo 3 > sys/class/leds/jogball-backlight/brightness

sleep 3

echo 0 > sys/class/leds/jogball-backlight/brightness
