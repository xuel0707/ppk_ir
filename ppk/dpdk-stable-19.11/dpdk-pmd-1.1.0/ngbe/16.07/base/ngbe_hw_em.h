/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_HW_EM_H_
#define _NGBE_HW_EM_H_

#include "ngbe_type.h"

#define NGBE_EM_MAX_TX_QUEUES 8
#define NGBE_EM_MAX_RX_QUEUES 8
#define NGBE_EM_RAR_ENTRIES   32
#define NGBE_EM_MC_TBL_SIZE   32
#define NGBE_EM_VFT_TBL_SIZE  128
#define NGBE_EM_RX_PB_SIZE	  42 /*KB*/
#define NGBE_EM_MAX_MSIX_VECTORS 0x09

s32 ngbe_check_mac_link_em(struct ngbe_hw *hw, u32 *speed,
			bool *link_up, bool link_up_wait_to_complete);
s32 ngbe_get_link_capabilities_em(struct ngbe_hw *hw,
				      u32 *speed,
				      bool *autoneg);
s32 ngbe_setup_mac_link_em(struct ngbe_hw *hw,
			       u32 speed,
			       bool autoneg_wait_to_complete);
s32 ngbe_reset_hw_em(struct ngbe_hw *hw);
s32 ngbe_setup_fc_em(struct ngbe_hw *hw);
s32 ngbe_validate_eeprom_checksum_em(struct ngbe_hw *hw,
					   u16 *checksum_val);
void ngbe_init_pf_em(struct ngbe_hw *hw);

#endif /* _NGBE_HW_EM_H_ */
