/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "ngbe_phy_mtd.h"

/**
 *  ngbe_read_phy_reg_mtd - Reads a value from a specified PHY register without
 *  the SWFW lock
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit address of PHY register to read
 *  @device_type: 5 bit device type
 *  @phy_data: Pointer to read data from PHY register
 **/
s32 ngbe_read_phy_reg_mtd(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			   u16 *phy_data)
{
	return 0;
}

/**
 *  ngbe_write_phy_reg_mtd - Writes a value to specified PHY register
 *  without SWFW lock
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit PHY register to write
 *  @device_type: 5 bit device type
 *  @phy_data: Data to write to the PHY register
 **/
s32 ngbe_write_phy_reg_mtd(struct ngbe_hw *hw, u32 reg_addr,
				u32 device_type, u16 phy_data)
{
	return 0;
}

