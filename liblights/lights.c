/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "lights"

#include <cutils/log.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>


#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#define LIGHT_ATTENTION 1
#define LIGHT_NOTIFY 2

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static struct light_state_t g_notify;
static int g_attention = 0;
static struct light_state_t g_no_action;
static struct light_state_t g_battery;
static int g_haveAmberLed = 0;
static int g_backlight = 255;
static int g_trackball = -1;
static int g_haveTrackballLight = 0;

char const*const AMBER_LED_FILE = "/sys/class/leds/amber/brightness";
char const*const GREEN_LED_FILE = "/sys/class/leds/green/brightness";
char const*const TRACKBALL_FILE = "/sys/class/leds/jogball-backlight/brightness";
char const*const VTKEY_FILE = "/sys/class/leds/vtkey-backlight/brightness";

char const*const BUTTON_FILE = "/sys/class/leds/button-backlight/brightness";
char const*const VTKEY_BLINK_FILE = "/sys/class/leds/vtkey-backlight/blink";
char const*const AMBER_BLINK_FILE = "/sys/class/leds/amber/blink";
char const*const GREEN_BLINK_FILE = "/sys/class/leds/green/blink";
char const*const TRACKBALL_BLINK_FILE = "/sys/class/leds/jogball-backlight/blink";

char const*const LCD_BACKLIGHT_FILE = "/sys/class/leds/lcd-backlight/brightness";

enum {
	LED_AMBER,
	LED_GREEN,
	LED_WHITE,
	LED_BLANK,
};

/**
 * Aux method, write int to file
 */
static int write_int (const char* path, int value) {
	int fd;
	static int already_warned = 0;

	fd = open(path, O_RDWR);
	if (fd < 0) {
		if (already_warned == 0) {
			LOGV("write_int failed to open %s\n", path);
			already_warned = 1;
		}
		return -errno;
	}

	char buffer[20];
	int bytes = snprintf(buffer, sizeof(buffer), "%d\n",value);
	int written = write (fd, buffer, bytes);
	close (fd);

	return written == -1 ? -errno : 0;

}

void init_globals (void) {
	pthread_mutex_init (&g_lock, NULL);
	g_haveTrackballLight = (access(TRACKBALL_FILE, W_OK) == 0) ? 1 : 0;
	g_haveAmberLed = (access(AMBER_LED_FILE, W_OK) == 0) ? 1 : 0;
}

static int is_lit (struct light_state_t const* state) {
	return state->color & 0x00ffffff;
}

static unsigned int led_color(struct light_state_t const* state) {
	unsigned int colorRGB = state->color & 0x00ffffff;
	unsigned int color = LED_BLANK;
	if ((colorRGB >> 8)&0xFF)
		color = LED_GREEN;
	if ((colorRGB >> 16)&0xFF)
		color = LED_AMBER;
	if ((colorRGB >> 0)&0xFF)
		color = LED_WHITE;
	return color;
}

static void handle_speaker_battery_locked (struct light_device_t *dev) {
	/* notification is higher priority so handle it first: */
	unsigned int colorRGB = g_notify.color & 0x00ffffff;
	unsigned int color = led_color(&g_notify);
	unsigned int speakerLightAvailable = 1;

	switch (g_notify.flashMode) {
		case LIGHT_FLASH_TIMED:
			switch (color) {
				case LED_AMBER:
					write_int (GREEN_LED_FILE, 0);
					write_int (AMBER_LED_FILE, 1);
					write_int (AMBER_BLINK_FILE, 1);
					speakerLightAvailable = 0;
					break;
				case LED_GREEN:
					write_int (AMBER_LED_FILE, 0);
					write_int (GREEN_LED_FILE, 1);
					write_int (GREEN_BLINK_FILE, 1);
					speakerLightAvailable = 0;
					break;
				case LED_WHITE:
					write_int (TRACKBALL_FILE, 3);
					write_int (TRACKBALL_BLINK_FILE, 1);
					break;
				case LED_BLANK:
					write_int (AMBER_BLINK_FILE, 0);
					write_int (GREEN_BLINK_FILE, 0);
					write_int (TRACKBALL_FILE, 0);
					break;
			}
			break;
		case LIGHT_FLASH_NONE:
			switch (color) {
				case LED_AMBER:
					write_int (AMBER_LED_FILE, 1);
					write_int (GREEN_LED_FILE, 0);
					write_int (AMBER_BLINK_FILE, 0);
					speakerLightAvailable = 0;
					break;
				case LED_GREEN:
					write_int (AMBER_LED_FILE, 0);
					write_int (GREEN_LED_FILE, 1);
					write_int (GREEN_BLINK_FILE, 0);
					speakerLightAvailable = 0;
					break;
				case LED_WHITE:
					write_int (TRACKBALL_FILE, 3);
					write_int (TRACKBALL_BLINK_FILE, 0);
					break;
				case LED_BLANK:
					write_int (TRACKBALL_FILE, 0);
					break;
			}
			break;
		default:
			SLOGE("handle_speaker_battery_locked colorRGB=%08X, unknown mode %d for notification\n",
					colorRGB, g_notify.flashMode);
	}

	// if notifications left speaker light available, then battery might use it:
	if (speakerLightAvailable && is_lit(&g_battery)) {
		colorRGB = g_battery.color & 0x00ffffff;
		color = led_color(&g_battery);
		switch (g_battery.flashMode) {
		case LIGHT_FLASH_TIMED:
			switch (color) {
				case LED_AMBER:
					write_int (GREEN_LED_FILE, 0);
					write_int (AMBER_LED_FILE, 1);
					write_int (AMBER_BLINK_FILE, 1);
					speakerLightAvailable = 0;
					break;
				case LED_GREEN:
					write_int (AMBER_LED_FILE, 0);
					write_int (GREEN_LED_FILE, 1);
					write_int (GREEN_BLINK_FILE, 1);
					speakerLightAvailable = 0;
					break;
				case LED_BLANK:
					write_int (AMBER_BLINK_FILE, 0);
					write_int (GREEN_BLINK_FILE, 0);
					write_int (TRACKBALL_FILE, 0);
					break;
				default:
					SLOGE("handle_speaker_battery_locked colorRGB=%08X, unknown color for battery flash\n",
						colorRGB);
					break;
		}
			break;
		case LIGHT_FLASH_NONE:
			switch (color) {
				case LED_AMBER:
					write_int (GREEN_LED_FILE, 0);
					write_int (AMBER_LED_FILE, 1);
					write_int (AMBER_BLINK_FILE, 0);
					speakerLightAvailable = 0;
					break;
				case LED_GREEN:
					write_int (AMBER_LED_FILE, 0);
					write_int (GREEN_LED_FILE, 1);
					write_int (GREEN_BLINK_FILE, 0);
					speakerLightAvailable = 0;
					break;
				case LED_BLANK:
					speakerLightAvailable = 0;
					write_int (TRACKBALL_FILE, 0);
					break;
				default:
					SLOGE("handle_speaker_battery_locked colorRGB=%08X, unknown color for battery\n",
						colorRGB);
					break;
			}
			break;
		default:
			SLOGE("handle_speaker_battery_locked colorRGB=%08X, unknown mode %d for battery\n",
				colorRGB, g_battery.flashMode);
		}
	}

	// finally if both notifications and battery are not using speaker light turn it off:
	if (speakerLightAvailable) {
		write_int (AMBER_LED_FILE, 0);
		write_int (GREEN_LED_FILE, 0);
	}

}

static int set_light_buttons (struct light_device_t* dev,
		struct light_state_t const* state) {
	int err = 0;
	int on = is_lit (state);
	pthread_mutex_lock (&g_lock);
	err = write_int (BUTTON_FILE, on?255:0);
	err = write_int (VTKEY_FILE, on?1:0);
	pthread_mutex_unlock (&g_lock);

	return 0;
}


static int
handle_trackball_light_locked(struct light_device_t* dev)
{
    int mode = g_attention;

    if (mode == 7 && g_backlight) {
        mode = 0;
    }
    LOGV("%s g_backlight = %d, mode = %d, g_attention = %d\n",
        __func__, g_backlight, mode, g_attention);
    // If the value isn't changing, don't set it, because this
    // can reset the timer on the breathing mode, which looks bad.
    if (g_trackball == mode) {
        return 0;
    }

    return write_int(TRACKBALL_FILE, mode);
}


static int rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int set_light_backlight(struct light_device_t* dev,
		struct light_state_t const* state) {
	int err = 0;
	int brightness = rgb_to_brightness(state);
	LOGV("%s brightness=%d color=0x%08x",
		__func__,brightness, state->color);
	pthread_mutex_lock(&g_lock);
	g_backlight = brightness;
	err = write_int(LCD_BACKLIGHT_FILE, brightness);
	pthread_mutex_unlock(&g_lock);
	return err;
}

static int set_light_battery (struct light_device_t* dev,
		struct light_state_t const* state) {
	pthread_mutex_lock (&g_lock);
	g_battery = *state;
	handle_speaker_battery_locked(dev);
	pthread_mutex_unlock (&g_lock);

	return 0;
}

static int set_light_notifications (struct light_device_t* dev,
		struct light_state_t const* state) {
	pthread_mutex_lock (&g_lock);
	g_notify = *state;
	handle_speaker_battery_locked (dev);
	pthread_mutex_unlock (&g_lock);
	return 0;
}

static int
set_light_attention(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    LOGV("set_light_attention g_trackball=%d color=0x%08x",
            g_trackball, state->color);
    if (state->flashMode == LIGHT_FLASH_HARDWARE) {
        g_attention = state->flashOnMS;
    } else if (state->flashMode == LIGHT_FLASH_NONE) {
        g_attention = 0;
    }
    if (g_haveTrackballLight) {
        handle_trackball_light_locked(dev);
    }
    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int close_lights (struct light_device_t *dev) {
	free (dev);
	return 0;
}


static int open_lights (const struct hw_module_t* module, char const* name,
		struct hw_device_t** device) {
	int (*set_light)(struct light_device_t* dev,
			struct light_state_t const* state);

	if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
        	set_light = set_light_backlight;
	}
	else if (0 == strcmp(LIGHT_ID_BUTTONS, name)) {
		set_light = set_light_buttons;
	}
	else if (0 == strcmp(LIGHT_ID_BATTERY, name)) {
		set_light = set_light_battery;
	}
	else if (0 == strcmp(LIGHT_ID_ATTENTION, name)) {
		set_light = set_light_attention;
	}
	else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name))  {
		set_light = set_light_notifications;
	}
	else {
		return -EINVAL;
	}
	
	pthread_once (&g_init, init_globals);
	struct light_device_t *dev = malloc (sizeof (struct light_device_t));
	memset (dev,0,sizeof(*dev));

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = 0;
	dev->common.module = (struct hw_module_t*)module;
	dev->common.close = (int (*)(struct hw_device_t*))close_lights;
	dev->set_light = set_light;

	*device = (struct hw_device_t*) dev;
	return 0;

}

static struct hw_module_methods_t lights_module_methods = {
	.open = open_lights,
};

const struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = LIGHTS_HARDWARE_MODULE_ID,
	.name = "HeroC lights module",
	.author = "Ameer Ghouse",
	.methods = &lights_module_methods,
};
