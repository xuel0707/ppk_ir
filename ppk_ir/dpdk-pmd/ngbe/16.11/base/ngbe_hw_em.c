/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "ngbe_type.h"
#include "ngbe_mbx.h"
#include "ngbe_phy.h"
#include "ngbe_vf.h"
#include "ngbe_eeprom.h"
#include "ngbe_mng.h"
#include "ngbe_hw.h"
#include "ngbe_hw_em.h"

/**
 *  ngbe_check_mac_link_em - Determine link and speed status
 *  @hw: pointer to hardware structure
 *  @speed: pointer to link speed
 *  @link_up: true when link is up
 *  @link_up_wait_to_complete: bool used to wait for link up or not
 *
 *  Reads the links register to determine if link is up and the current speed
 **/
s32 ngbe_check_mac_link_em(struct ngbe_hw *hw, u32 *speed,
			bool *link_up, bool link_up_wait_to_complete)
{
	u32 i, reg;
	s32 status = 0;

	DEBUGFUNC("ngbe_check_mac_link_em");

#ifndef RTE_NGBE_POLL
	reg = rd32(hw, NGBE_GPIOINTSTAT);
	wr32(hw, NGBE_GPIOEOI,reg);
#endif

	if (link_up_wait_to_complete) {
		for (i = 0; i < hw->mac.max_link_up_time; i++) {
			status = hw->phy.check_link(hw, speed, link_up);
			if (*link_up)
				break;
			msec_delay(100);
		}		
	} else
		status = hw->phy.check_link(hw, speed, link_up);
	
	return status;
}

/**
 *  ngbe_get_link_capabilities_em - Determines link capabilities
 *  @hw: pointer to hardware structure
 *  @speed: pointer to link speed
 *  @autoneg: true when autoneg or autotry is enabled
 *
 *  Determines the link capabilities by reading the AUTOC register.
 **/
s32 ngbe_get_link_capabilities_em(struct ngbe_hw *hw,
				      u32 *speed,
				      bool *autoneg)
{
	s32 status = 0;
	u16 value = 0;

	DEBUGFUNC("\n");

	hw->mac.autoneg = *autoneg;

	if (hw->phy.type == ngbe_phy_rtl) {
		*speed = NGBE_LINK_SPEED_1GB_FULL |
			NGBE_LINK_SPEED_100M_FULL |
			NGBE_LINK_SPEED_10M_FULL;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_1000BASE_T |
				NGBE_PHYSICAL_LAYER_100BASE_TX;
	}

	if (hw->phy.type == ngbe_phy_yt8521s_sfi) {
		ngbe_read_phy_reg_ext_yt(hw, YT_CHIP, 0, &value);
		if ((value & YT_CHIP_MODE_MASK) == YT_CHIP_MODE_SEL(1))
		*speed = NGBE_LINK_SPEED_1GB_FULL;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_1000BASE_T;
	}

	return status;
}

/**
 *  ngbe_setup_mac_link_em - Set MAC link speed
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true when waiting for completion is needed
 *
 *  Set the link speed in the AUTOC register and restarts link.
 **/
s32 ngbe_setup_mac_link_em(struct ngbe_hw *hw,
			       u32 speed,
			       bool autoneg_wait_to_complete)
{
	s32 status;

	DEBUGFUNC("\n");

	/* Setup the PHY according to input speed */
	status = hw->phy.setup_link(hw, speed, autoneg_wait_to_complete);

	return status;
}

#if 0
/**
 *  ngbe_setup_copper_link - Set the PHY autoneg advertised field
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true if waiting is needed to complete
 *
 *  Restarts link on PHY and MAC based on settings passed in.
 **/
static s32 ngbe_setup_copper_link(struct ngbe_hw *hw,
					 u32 speed,
					 bool autoneg_wait_to_complete)
{
	s32 status;

	DEBUGFUNC("\n");

	/* Setup the PHY according to input speed */
	status = hw->phy.setup_link(hw, speed, autoneg_wait_to_complete);

	return status;
}
#endif

static void
ngbe_reset_misc_em(struct ngbe_hw *hw)
{
	int i;

	wr32(hw, NGBE_ISBADDRL, hw->isb_dma & 0xFFFFFFFF);
	wr32(hw, NGBE_ISBADDRH, hw->isb_dma >> 32);

	/* receive packets that size > 2048 */
	wr32m(hw, NGBE_MACRXCFG,
		NGBE_MACRXCFG_JUMBO, NGBE_MACRXCFG_JUMBO);

	wr32m(hw, NGBE_FRMSZ, NGBE_FRMSZ_MAX_MASK,
		NGBE_FRMSZ_MAX(NGBE_FRAME_SIZE_DFT));

	/* clear counters on read */
	wr32m(hw, NGBE_MACCNTCTL,
		NGBE_MACCNTCTL_RC, NGBE_MACCNTCTL_RC);

	wr32m(hw, NGBE_RXFCCFG,
		NGBE_RXFCCFG_FC, NGBE_RXFCCFG_FC);
	wr32m(hw, NGBE_TXFCCFG,
		NGBE_TXFCCFG_FC, NGBE_TXFCCFG_FC);

	wr32m(hw, NGBE_MACRXFLT,
		NGBE_MACRXFLT_PROMISC, NGBE_MACRXFLT_PROMISC);

	wr32m(hw, NGBE_RSTSTAT,
		NGBE_RSTSTAT_TMRINIT_MASK, NGBE_RSTSTAT_TMRINIT(30));

	/* errata 4: initialize mng flex tbl and wakeup flex tbl*/
	wr32(hw, NGBE_MNGFLEXSEL, 0);
	for (i = 0; i < 16; i++) {
		wr32(hw, NGBE_MNGFLEXDWL(i), 0);
		wr32(hw, NGBE_MNGFLEXDWH(i), 0);
		wr32(hw, NGBE_MNGFLEXMSK(i), 0);
	}
	wr32(hw, NGBE_LANFLEXSEL, 0);
	for (i = 0; i < 16; i++) {
		wr32(hw, NGBE_LANFLEXDWL(i), 0);
		wr32(hw, NGBE_LANFLEXDWH(i), 0);
		wr32(hw, NGBE_LANFLEXMSK(i), 0);
	}

	/* set pause frame dst mac addr */
	wr32(hw, NGBE_RXPBPFCDMACL, 0xC2000001);
	wr32(hw, NGBE_RXPBPFCDMACH, 0x0180);

	wr32(hw, NGBE_MDIOMODE, 0xF);

	wr32m(hw, NGBE_GPIE, NGBE_GPIE_MSIX, NGBE_GPIE_MSIX);

	if (hw->gpio_ctl == 1) {
		/* gpio0 is used to power on/off control*/
		wr32(hw, NGBE_GPIODIR, NGBE_GPIODIR_DDR(1));
		wr32(hw, NGBE_GPIODATA, NGBE_GPIOBIT_0);
	}

	ngbe_init_thermal_sensor_thresh(hw);

	/* enable mac transmiter */
	wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_TE, NGBE_MACTXCFG_TE);

	/* sellect GMII */
	wr32m(hw, NGBE_MACTXCFG,
		NGBE_MACTXCFG_SPEED_MASK, NGBE_MACTXCFG_SPEED_1G);

	for (i = 0; i < 4; i++) {
		wr32m(hw, NGBE_IVAR(i), 0x80808080, 0);
	}

	hw->an_errcnt = 0;
	hw->an_retry = false;
}

/**
 *  ngbe_reset_hw_em - Perform hardware reset
 *  @hw: pointer to hardware structure
 *
 *  Resets the hardware by resetting the transmit and receive units, masks
 *  and clears all interrupts, perform a PHY reset, and perform a link (MAC)
 *  reset.
 **/
s32 ngbe_reset_hw_em(struct ngbe_hw *hw)
{
	s32 status;

	DEBUGFUNC("ngbe_reset_hw_em");

	/* Call adapter stop to disable tx/rx and clear interrupts */
	status = hw->mac.stop_hw(hw);
	if (status != 0)
		return status;

	/* Identify PHY and related function pointers */
	status = ngbe_init_phy(hw);
	if (status)
		return status;

	/* Reset PHY */
	if (hw->phy.reset_disable == false)
		hw->phy.reset_hw(hw);

	/*
	 * Issue global reset to the MAC.  Needs to be SW reset if link is up.
	 * If link reset is used when link is up, it might reset the PHY when
	 * mng is using it.  If link is down or the flag to force full link
	 * reset is set, then perform link reset.
	 */

	wr32(hw, NGBE_RST, NGBE_RST_LAN(hw->bus.lan_id));
	ngbe_flush(hw);
	msec_delay(50);

	ngbe_reset_misc_em(hw);
	hw->mac.clear_hw_cntrs(hw);

	msec_delay(50);

	/* Store the permanent mac address */
	hw->mac.get_mac_addr(hw, hw->mac.perm_addr);

	/*
	 * Store MAC address from RAR0, clear receive address registers, and
	 * clear the multicast table.  Also reset num_rar_entries to 32,
	 * since we modify this value when programming the SAN MAC address.
	 */
	hw->mac.num_rar_entries = NGBE_EM_RAR_ENTRIES;
	hw->mac.init_rx_addrs(hw);

	return status;
}

/**
 *  ngbe_setup_fc_em - Set up flow control
 *  @hw: pointer to hardware structure
 *
 *  Called at init time to set up flow control.
 **/
s32 ngbe_setup_fc_em(struct ngbe_hw *hw)
{
	s32 err = 0;
	u16 reg_cu = 0;

	DEBUGFUNC("ngbe_setup_fc");

	/* Validate the requested mode */
	if (hw->fc.strict_ieee && hw->fc.requested_mode == ngbe_fc_rx_pause) {
		DEBUGOUT(
			   "ngbe_fc_rx_pause not valid in strict IEEE mode\n");
		err = NGBE_ERR_INVALID_LINK_SETTINGS;
		goto out;
	}

	/*
	 * 1gig parts do not have a word in the EEPROM to determine the
	 * default flow control setting, so we explicitly set it to full.
	 */
	if (hw->fc.requested_mode == ngbe_fc_default)
		hw->fc.requested_mode = ngbe_fc_full;

	/*
	 * The possible values of fc.requested_mode are:
	 * 0: Flow control is completely disabled
	 * 1: Rx flow control is enabled (we can receive pause frames,
	 *    but not send pause frames).
	 * 2: Tx flow control is enabled (we can send pause frames but
	 *    we do not support receiving pause frames).
	 * 3: Both Rx and Tx flow control (symmetric) are enabled.
	 * other: Invalid.
	 */
	switch (hw->fc.requested_mode) {
	case ngbe_fc_none:
		/* Flow control completely disabled by software override. */
		break;
	case ngbe_fc_tx_pause:
		/*
		 * Tx Flow control is enabled, and Rx Flow control is
		 * disabled by software override.
		 */
		if (hw->phy.type == ngbe_phy_mvl_sfi ||
			hw->phy.type == ngbe_phy_yt8521s_sfi)
			reg_cu |= MVL_FANA_ASM_PAUSE;
		else
			reg_cu |= 0x800; //need to merge rtl and mvl on page 0
		break;
	case ngbe_fc_rx_pause:
		/*
		 * Rx Flow control is enabled and Tx Flow control is
		 * disabled by software override. Since there really
		 * isn't a way to advertise that we are capable of RX
		 * Pause ONLY, we will advertise that we support both
		 * symmetric and asymmetric Rx PAUSE, as such we fall
		 * through to the fc_full statement.  Later, we will
		 * disable the adapter's ability to send PAUSE frames.
		 */
	case ngbe_fc_full:
		/* Flow control (both Rx and Tx) is enabled by SW override. */
		if (hw->phy.type == ngbe_phy_mvl_sfi ||
			hw->phy.type == ngbe_phy_yt8521s_sfi)
			reg_cu |= MVL_FANA_SYM_PAUSE;
		else
			reg_cu |= 0xC00; //need to merge rtl and mvl on page 0
			//reg_cu |= RTL_ANAR_APAUSE | RTL_ANAR_PAUSE;
		break;
	default:
		DEBUGOUT("Flow control param set incorrectly\n");
		err = NGBE_ERR_CONFIG;
		goto out;
		break;
	}

	/*
	 * AUTOC restart handles negotiation of 1G on backplane
	 * and copper.
	 */
	if (hw->phy.media_type == ngbe_media_type_copper) {
		switch (hw->phy.type) {
		case ngbe_phy_rtl:
			err = ngbe_set_phy_pause_adv_rtl(hw, reg_cu);
			break;
		case ngbe_phy_mvl:
			err = ngbe_set_phy_pause_adv_mvl(hw, reg_cu);
			break;
		default:
			hw->phy.write_reg(hw, NGBE_MD_AUTO_NEG_ADVT,
			      	NGBE_MD_DEV_AUTO_NEG, reg_cu);
			break;
		}
	} else if (hw->phy.media_type == ngbe_media_type_fiber) {
		switch (hw->phy.type) {
		case ngbe_phy_mvl_sfi:
			ngbe_set_phy_pause_adv_mvl(hw, reg_cu);
			break;
		default:
			break;
		}
	}

out:
	return err;
}

/**
 *  ngbe_validate_eeprom_checksum_em - Validate EEPROM checksum
 *  @hw: pointer to hardware structure
 *  @checksum_val: calculated checksum
 *
 *  Performs checksum calculation and validates the EEPROM checksum.  If the
 *  caller does not need checksum_val, the value can be NULL.
 **/
s32 ngbe_validate_eeprom_checksum_em(struct ngbe_hw *hw,
					   u16 *checksum_val)
{
	u32 eeprom_cksum_devcap = 0;
	int err = 0;

	DEBUGFUNC("ngbe_validate_eeprom_checksum_em");
	UNREFERENCED_PARAMETER(checksum_val);

	/* Check EEPROM only once */
	if (hw->bus.lan_id == 0) {
		wr32(hw, NGBE_CALSUM_CAP_STATUS, 0x0);
		wr32(hw, NGBE_EEPROM_VERSION_STORE_REG, 0x0);
	} else {
		eeprom_cksum_devcap = rd32(hw, NGBE_CALSUM_CAP_STATUS);
//		saved_version = rd32(hw, NGBE_EEPROM_VERSION_STORE_REG);
	}

	if (hw->bus.lan_id == 0 || eeprom_cksum_devcap == 0) {
		err = ngbe_hic_check_cap(hw);
		if (err != 0) {
			PMD_INIT_LOG(ERR, 
				"The EEPROM checksum is not valid: %d", err);
			return -EIO;
		}
	}

	return err;
}

void ngbe_init_pf_em(struct ngbe_hw *hw)
{
	hw->rom.validate_checksum = ngbe_validate_eeprom_checksum_em;
	hw->mac.mcft_size	= NGBE_EM_MC_TBL_SIZE;
	hw->mac.vft_size	= NGBE_EM_VFT_TBL_SIZE;
	hw->mac.num_rar_entries	= NGBE_EM_RAR_ENTRIES;
	hw->mac.rx_pb_size	= NGBE_EM_RX_PB_SIZE;
	hw->mac.max_rx_queues	= NGBE_EM_MAX_RX_QUEUES;
	hw->mac.max_tx_queues	= NGBE_EM_MAX_TX_QUEUES;

	hw->mac.default_speeds = NGBE_LINK_SPEED_10M_FULL |
				 NGBE_LINK_SPEED_100M_FULL |
				 NGBE_LINK_SPEED_1GB_FULL;
}
