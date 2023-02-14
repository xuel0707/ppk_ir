/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "ngbe_phy.h"

#ifndef _NGBE_PHY_ZTE_H_
#define _NGBE_PHY_ZTE_H_

#define NGBE_PHYID_ZTE			0x00005044U

s32 ngbe_read_phy_reg_zte(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			u16 *phy_data);
s32 ngbe_write_phy_reg_zte(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			u16 phy_data);

s32 ngbe_check_phy_link_zte(struct ngbe_hw *hw, 
						   u32 *speed, 
						   bool *link_up);

s32 ngbe_reset_phy_zte(struct ngbe_hw *hw);
u32 ngbe_setup_phy_link_zte(struct ngbe_hw *hw,
		u32 speed, bool autoneg_wait_to_complete);

#endif /* _NGBE_PHY_ZTE_H_ */