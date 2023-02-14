/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */


#include "ngbe_phy_zte.h"

#define ZTE_PHY_RST_WAIT_PERIOD               5

u32 ngbe_setup_phy_link_zte(struct ngbe_hw *hw,
		u32 speed, bool autoneg_wait_to_complete)
{
	u16 ngbe_phy_ccr = 0;

	DEBUGFUNC("\n");
	UNREFERENCED_PARAMETER(autoneg_wait_to_complete);
	/*
	 * Clear autoneg_advertised and set new values based on input link
	 * speed.
	 */
	if (speed) {
		hw->phy.autoneg_advertised = 0;

		if (speed & NGBE_LINK_SPEED_10GB_FULL)
			hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_10GB_FULL;

		if (speed & NGBE_LINK_SPEED_5GB_FULL)
			hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_5GB_FULL;

		if (speed & NGBE_LINK_SPEED_2_5GB_FULL)
			hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_2_5GB_FULL;

		if (speed & NGBE_LINK_SPEED_1GB_FULL)
			hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_1GB_FULL;

		if (speed & NGBE_LINK_SPEED_100M_FULL)
			hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_100M_FULL;

		if (speed & NGBE_LINK_SPEED_10M_FULL)
			hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_10M_FULL;
	}

	hw->phy.read_reg(hw, 0, 0, &ngbe_phy_ccr);
	
	if (speed & NGBE_LINK_SPEED_1GB_FULL) {
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_1GB_FULL;
		ngbe_phy_ccr |= RTL_BMCR_SPEED_SELECT1;/*bit 6*/
	}
	else if (speed & NGBE_LINK_SPEED_100M_FULL) {
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_100M_FULL;
		ngbe_phy_ccr |= RTL_BMCR_SPEED_SELECT0;/*bit 13*/
	}
	else if (speed & NGBE_LINK_SPEED_10M_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_10M_FULL;
	else
		return NGBE_LINK_SPEED_UNKNOWN;

	ngbe_phy_ccr |= RTL_BMCR_DUPLEX;/*restart autonegotiation*/
	hw->phy.write_reg(hw, 0, 0, ngbe_phy_ccr);

	return speed;
}

s32 ngbe_reset_phy_zte(struct ngbe_hw *hw)
{
	u32 i;
	u16 ctrl = 0;
	s32 status = 0;

	DEBUGFUNC("ngbe_reset_phy_zte");

	if (hw->phy.type != ngbe_phy_zte)
		return NGBE_ERR_PHY_TYPE;

    /*set control register[0x0] to reset mode*/
    ctrl = 1;
    /* mode reset */
	ctrl |= RTL_BMCR_RESET;
	status = hw->phy.write_reg(hw, 0, 0, ctrl);

	for (i = 0; i < ZTE_PHY_RST_WAIT_PERIOD; i++) {
		status = hw->phy.read_reg(hw, 0, 0, &ctrl);
		if (!(ctrl & RTL_BMCR_RESET))
			break;
		msleep(1);
	}

	if (i == ZTE_PHY_RST_WAIT_PERIOD) {
		DEBUGOUT("PHY reset polling failed to complete.\n");
		return NGBE_ERR_RESET_FAILED;
	}

	return status;
}

s32 ngbe_check_phy_link_zte(struct ngbe_hw *hw, 
						   u32 *speed, 
						   bool *link_up)
{
	s32 status = 0;
	u32 time_out;
	u32 max_time_out = 10;
	u16 phy_link = 0;
	u16 phy_speed = 0;
	u16 phy_data = 0;


	DEBUGFUNC("ngbe_check_phy_link_zte");

	/* Initialize speed and link to default case */
	*link_up = false;
	*speed = NGBE_LINK_SPEED_UNKNOWN;

	/*
	 * Check current speed and link status of the PHY register.
	 * This is a vendor specific register and may have to
	 * be changed for other copper PHYs.
	 */
	for (time_out = 0; time_out < max_time_out; time_out++) {
		usec_delay(10);
		status = hw->phy.read_reg(hw, 0x1a, 0, &phy_data);
		/*bit 6->0x0040*/
		phy_link = phy_data & 0x40;
		phy_speed = phy_data & 0xC000;
		if (phy_link == 0x40) {
			*link_up = true;
			
			if (phy_speed == 0x0200)
				*speed = NGBE_LINK_SPEED_1GB_FULL;
			else if (phy_speed == 0x0100)
				*speed = NGBE_LINK_SPEED_100M_FULL;
			else if (phy_speed == 0x0000)
				*speed = NGBE_LINK_SPEED_10M_FULL;

			break;
		}
	}

	return status;
}

