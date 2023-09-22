/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "txgbe_mbx.h"
#include "txgbe_vf.h"
#include "txgbe_hv_vf.h"

/**
 * Hyper-V variant - just a stub.
 * @hw: unused
 * @mc_addr_list: unused
 * @mc_addr_count: unused
 * @next: unused
 * @clear: unused
 */
static s32 txgbevf_hv_update_mc_addr_list_vf(struct txgbe_hw *hw, u8 *mc_addr_list,
				 u32 mc_addr_count, txgbe_mc_addr_itr next,
				 bool clear)
{
	UNREFERENCED_PARAMETER(hw, mc_addr_list, mc_addr_count, next, clear);

	return TXGBE_ERR_FEATURE_NOT_SUPPORTED;
}

/**
 * Hyper-V variant - just a stub.
 * @hw: unused
 * @xcast_mode: unused
 */
static s32 txgbevf_hv_update_xcast_mode(struct txgbe_hw *hw, int xcast_mode)
{
	UNREFERENCED_PARAMETER(hw, xcast_mode);

	return TXGBE_ERR_FEATURE_NOT_SUPPORTED;
}

/**
 * Hyper-V variant - just a stub.
 * @hw: unused
 * @vlan: unused
 * @vind: unused
 * @vlan_on: unused
 * @vlvf_bypass: unused
 */
static s32 txgbevf_hv_set_vfta_vf(struct txgbe_hw *hw, u32 vlan, u32 vind,
				  bool vlan_on, bool vlvf_bypass)
{
	UNREFERENCED_PARAMETER(hw, vlan, vind, vlan_on, vlvf_bypass);

	return TXGBE_ERR_FEATURE_NOT_SUPPORTED;
}

static s32 txgbevf_hv_set_uc_addr_vf(struct txgbe_hw *hw, u32 index, u8 *addr)
{
	UNREFERENCED_PARAMETER(hw, index, addr);

	return TXGBE_ERR_FEATURE_NOT_SUPPORTED;
}

/**
 * Hyper-V variant - just a stub.
 */
static s32 txgbevf_hv_reset_hw_vf(struct txgbe_hw *hw)
{
	UNREFERENCED_PARAMETER(hw);

	return TXGBE_ERR_FEATURE_NOT_SUPPORTED;
}

/**
 * Hyper-V variant - just a stub.
 */
static s32 txgbevf_hv_set_rar_vf(struct txgbe_hw *hw, u32 index, u8 *addr, u32 vlan, u32 vind)
{
	UNREFERENCED_PARAMETER(hw, index, addr, vlan, vind);

	return TXGBE_ERR_FEATURE_NOT_SUPPORTED;
}

/**
 * Hyper-V variant; there is no mailbox communication.
 * @hw: pointer to hardware structure
 * @speed: pointer to link speed
 * @link_up: true is link is up, false otherwise
 * @autoneg_wait_to_complete: unused
 *
 */
static s32 txgbevf_hv_check_mac_link_vf(struct txgbe_hw *hw,
					u32 *speed,
					bool *link_up,
					bool autoneg_wait_to_complete)
{
	struct txgbe_mbx_info *mbx = &hw->mbx;
	struct txgbe_mac_info *mac = &hw->mac;
	u32 links_reg;
	UNREFERENCED_PARAMETER(autoneg_wait_to_complete);

	/* If we were hit with a reset drop the link */
	if (!mbx->check_for_rst(hw, 0) || !mbx->timeout)
		mac->get_link_status = true;

	if (!mac->get_link_status)
		goto out;

	/* if link status is down no point in checking to see if pf is up */
	links_reg = rd32(hw, TXGBE_VFSTATUS);
	if (!(links_reg & TXGBE_VFSTATUS_UP))
		goto out;

	/* for SFP+ modules and DA cables it can take up to 500usecs
	 * before the link status is correct
	 */
	if (mac->type == txgbe_mac_raptor_vf) {
		int i;

		for (i = 0; i < 5; i++) {
			usec_delay(100);
			links_reg = rd32(hw, TXGBE_VFSTATUS);

			if (!(links_reg & TXGBE_VFSTATUS_UP))
				goto out;
		}
	}

	switch (links_reg & TXGBE_VFSTATUS_BW_MASK) {
	case TXGBE_VFSTATUS_BW_10G:
		*speed = TXGBE_LINK_SPEED_10GB_FULL;
		break;
	case TXGBE_VFSTATUS_BW_1G:
		*speed = TXGBE_LINK_SPEED_1GB_FULL;
		break;
	case TXGBE_VFSTATUS_BW_100M:
		*speed = TXGBE_LINK_SPEED_100M_FULL;
		break;
	default:
		*speed = TXGBE_LINK_SPEED_UNKNOWN;
	}

	/* if we passed all the tests above then the link is up and we no
	 * longer need to check for link
	 */
	mac->get_link_status = false;

out:
	*link_up = !mac->get_link_status;
	return 0;
}

/**
 * txgbevf_hv_set_rlpml_vf - Set the maximum receive packet length
 * @hw: pointer to the HW structure
 * @max_size: value to assign to max frame size
 * Hyper-V variant.
 **/
static s32 txgbevf_hv_set_rlpml_vf(struct txgbe_hw *hw, u16 max_size)
{
	/* If we are on Hyper-V, we implement this functionality
	 * differently.
	 */
	/* CRC == 4 */
	wr32m(hw, TXGBE_FRMSZ, TXGBE_FRMSZ_MAX_MASK,
		TXGBE_FRMSZ_MAX(max_size + ETH_FCS_LEN));

	return 0;
}

/**
 *  txgbevf_hv_negotiate_api_version_vf - Negotiate supported API version
 *  @hw: pointer to the HW structure
 *  @api: integer containing requested API version
 *  Hyper-V version - only txgbe_mbox_api_10 supported.
 **/
static int txgbevf_hv_negotiate_api_version_vf(struct txgbe_hw *hw, int api)
{
	UNREFERENCED_PARAMETER(hw);

	/* Hyper-V only supports api version txgbe_mbox_api_10 */
	if (api != txgbe_mbox_api_10)
		return TXGBE_ERR_INVALID_ARGUMENT;

	return 0;
}

/**
 *  txgbe_init_ops_hvf - Initialize the pointers for vf
 *  @hw: pointer to hardware structure
 *
 *  This will assign function pointers, adapter-specific functions can
 *  override the assignment of generic function pointers by assigning
 *  their own adapter-specific function pointers.
 *  Does not touch the hardware.
 **/
s32 txgbe_init_ops_hvf(struct txgbe_hw *hw)
{
	/* Set defaults for VF then override applicable Hyper-V
	 * specific functions
	 */
	txgbe_init_ops_vf(hw);

	hw->mac.reset_hw = txgbevf_hv_reset_hw_vf;
	hw->mac.check_link = txgbevf_hv_check_mac_link_vf;
	hw->mac.negotiate_api_version = txgbevf_hv_negotiate_api_version_vf;
	hw->mac.set_rar = txgbevf_hv_set_rar_vf;
	hw->mac.update_mc_addr_list = txgbevf_hv_update_mc_addr_list_vf;
	hw->mac.update_xcast_mode = txgbevf_hv_update_xcast_mode;
	hw->mac.set_uc_addr = txgbevf_hv_set_uc_addr_vf;
	hw->mac.set_vfta = txgbevf_hv_set_vfta_vf;
	hw->mac.set_rlpml = txgbevf_hv_set_rlpml_vf;

	return 0;
}
