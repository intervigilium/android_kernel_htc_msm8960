/* arch/arm/mach-msm/board-m4-keypad.c
 *
 * Copyright (C) 2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/gpio_event.h>
#include <linux/keyreset.h>
#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <mach/gpio.h>
#include <linux/delay.h>
#include "board-m4.h"

#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "board_m4."

static void config_gpio_table(uint32_t *table, int len)
{
	int n, rc;
	for (n = 0; n < len; n++) {
		rc = gpio_tlmm_config(table[n], GPIO_CFG_ENABLE);
		if (rc) {
			pr_err("[keypad]%s: gpio_tlmm_config(%#x)=%d\n",
				__func__, table[n], rc);
			break;
		}
	}
}

static struct gpio_event_direct_entry m4_keypad_input_map[] = {
	{
		.gpio = MSM_VOL_UPz,
		.code = KEY_VOLUMEUP,
	},
	{
		.gpio = MSM_VOL_DOWNz,
		.code = KEY_VOLUMEDOWN,
	 },
};

static void m4_setup_input_gpio(void)
{
	uint32_t inputs_gpio_table[] = {
		GPIO_CFG(MSM_PWR_KEY_MSMz, 0, GPIO_CFG_INPUT,
			GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
		GPIO_CFG(MSM_VOL_UPz, 0, GPIO_CFG_INPUT,
			GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
		GPIO_CFG(MSM_VOL_DOWNz, 0, GPIO_CFG_INPUT,
			GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	};

	config_gpio_table(inputs_gpio_table, ARRAY_SIZE(inputs_gpio_table));
}

uint32_t hw_clr_gpio_table[] = {
	GPIO_CFG(MSM_HW_RST_CLRz, 0, GPIO_CFG_INPUT,
		GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
	GPIO_CFG(MSM_HW_RST_CLRz, 0, GPIO_CFG_OUTPUT,
		GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
};

static struct gpio_event_input_info m4_keypad_input_info = {
	.info.func             = gpio_event_input_func,
	.flags                 = GPIOEDF_PRINT_KEYS,
	.type                  = EV_KEY,
#if BITS_PER_LONG != 64 && !defined(CONFIG_KTIME_SCALAR)
	.debounce_time.tv.nsec = 20 * NSEC_PER_MSEC,
# else
	.debounce_time.tv64    = 20 * NSEC_PER_MSEC,
# endif
	.keymap                = m4_keypad_input_map,
	.keymap_size           = ARRAY_SIZE(m4_keypad_input_map),
};

static struct gpio_event_info *m4_keypad_info[] = {
	&m4_keypad_input_info.info,
};

static struct gpio_event_platform_data m4_keypad_data = {
	.names = {
		"device-keypad",
		NULL,
	},
	.info = m4_keypad_info,
	.info_count = ARRAY_SIZE(m4_keypad_info),
};

static struct platform_device m4_keypad_input_device = {
	.name = GPIO_EVENT_DEV_NAME,
	.id = 0,
	.dev		= {
		.platform_data	= &m4_keypad_data,
	},
};
static struct keyreset_platform_data m4_reset_keys_pdata = {

	.keys_down = {
		KEY_POWER,
		KEY_VOLUMEDOWN,
		KEY_VOLUMEUP,
		0
	},
};

struct platform_device m4_reset_keys_device = {
	.name = KEYRESET_NAME,
	.dev.platform_data = &m4_reset_keys_pdata,
};

int __init m4_init_keypad(void)
{
	printk(KERN_DEBUG "[KEY]%s\n", __func__);

	m4_setup_input_gpio();

	if (platform_device_register(&m4_reset_keys_device))
		printk(KERN_WARNING "%s: register reset key fail\n", __func__);

	return platform_device_register(&m4_keypad_input_device);
}
