/*
 * OCEDO Koala board support
 * Based on the MR1750 machine file
 *
 * Copyright (c) 2012 Qualcomm Atheros
 * Copyright (c) 2012-2013 Marek Lindner <marek@open-mesh.com>
 * Copyright (c) 2018 David Bauer <mail@david-bauer.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <linux/platform_data/phy-at803x.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define KOALA_GPIO_LED_POWER		22
#define KOALA_GPIO_LED_WLAN_5G		13
#define KOALA_GPIO_LED_WLAN_2G		23
#define KOALA_GPIO_LED_WLAN_SYS		19

#define KOALA_GPIO_BTN_RESET		17

#define KOALA_KEYS_POLL_INTERVAL	20	/* msecs */
#define KOALA_KEYS_DEBOUNCE_INTERVAL	(3 * KOALA_KEYS_POLL_INTERVAL)

#define KOALA_LAN_MAC_OFFSET		0
#define KOALA_WIFI2G_MAC_OFFSET		0x06
#define KOALA_WMAC_CALDATA_OFFSET	0x1000

static struct gpio_led koala_leds_gpio[] __initdata = {
	{
		.name		= "koala:green:power",
		.gpio		= KOALA_GPIO_LED_POWER,
		.active_low	= 1,
	},
	{
		.name		= "koala:red:wlan58",
		.gpio		= KOALA_GPIO_LED_WLAN_5G,
		.active_low	= 1,
	},
	{
		.name		= "koala:yellow:wlan2",
		.gpio		= KOALA_GPIO_LED_WLAN_2G,
		.active_low	= 1,
	},
	{
		.name		= "koala:blue:sys",
		.gpio		= KOALA_GPIO_LED_WLAN_SYS,
		.active_low	= 1,
		},
};

static struct gpio_keys_button koala_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = KOALA_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= KOALA_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};

static struct at803x_platform_data koala_at803x_data = {
	.disable_smarteee = 1,
	.enable_rgmii_rx_delay = 1,
	.enable_rgmii_tx_delay = 0,
	.fixup_rgmii_tx_delay = 1,
};

static struct mdio_board_info koala_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.mdio_addr = 5,
		.platform_data = &koala_at803x_data,
	},
};

static void __init koala_setup_qca955x_eth_cfg(u32 mask,
						unsigned int rxd,
						unsigned int rxdv,
						unsigned int txd,
						unsigned int txe)
{
	void __iomem *base;
	u32 t;

	base = ioremap(QCA955X_GMAC_BASE, QCA955X_GMAC_SIZE);

	t = mask;
	t |= rxd << QCA955X_ETH_CFG_RXD_DELAY_SHIFT;
	t |= rxdv << QCA955X_ETH_CFG_RDV_DELAY_SHIFT;
	t |= txd << QCA955X_ETH_CFG_TXD_DELAY_SHIFT;
	t |= txe << QCA955X_ETH_CFG_TXE_DELAY_SHIFT;

	__raw_writel(t, base + QCA955X_GMAC_REG_ETH_CFG);

	iounmap(base);
}

static void __init koala_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);
	u8 mac[6];

	ath79_eth0_pll_data.pll_1000 = 0xae000000;
	ath79_eth0_pll_data.pll_100 = 0xa0000101;
	ath79_eth0_pll_data.pll_10 = 0xa0001313;

	ath79_register_m25p80(NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(koala_leds_gpio),
				 koala_leds_gpio);
	ath79_register_gpio_keys_polled(-1, KOALA_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(koala_gpio_keys),
					koala_gpio_keys);

	ath79_init_mac(mac, art + KOALA_WIFI2G_MAC_OFFSET, 0);
	ath79_register_wmac(art + KOALA_WMAC_CALDATA_OFFSET, mac);
	ath79_register_pci();

	koala_setup_qca955x_eth_cfg(QCA955X_ETH_CFG_RGMII_EN, 3, 3, 0, 0);
	ath79_register_mdio(0, 0x0);

	mdiobus_register_board_info(koala_mdio0_info,
				    ARRAY_SIZE(koala_mdio0_info));

	ath79_init_mac(ath79_eth0_data.mac_addr, art + KOALA_LAN_MAC_OFFSET, 0);

	/* GMAC0 is connected to the RMGII interface */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(5);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;

	ath79_register_eth(0);
}

MIPS_MACHINE(ATH79_MACH_KOALA, "KOALA", "OCEDO Koala", koala_setup);
