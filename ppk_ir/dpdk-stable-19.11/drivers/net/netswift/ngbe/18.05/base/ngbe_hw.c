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

#define NGBE_RAPTOR_MAX_TX_QUEUES 128
#define NGBE_RAPTOR_MAX_RX_QUEUES 128
#define NGBE_RAPTOR_RAR_ENTRIES   128
#define NGBE_RAPTOR_MC_TBL_SIZE   128
#define NGBE_RAPTOR_VFT_TBL_SIZE  128
#define NGBE_RAPTOR_RX_PB_SIZE	  512 /*KB*/
#define NGBE_RAPTOR_MAX_MSIX_VECTORS 0x40

//STATIC s32 ngbe_setup_copper_link_raptor(struct ngbe_hw *hw,
//					 u32 speed,
//					 bool autoneg_wait_to_complete);



STATIC s32 ngbe_mta_vector(struct ngbe_hw *hw, u8 *mc_addr);
STATIC s32 ngbe_get_san_mac_addr_offset(struct ngbe_hw *hw,
					 u16 *san_mac_offset);

/**
 * ngbe_device_supports_autoneg_fc - Check if device supports autonegotiation
 * of flow control
 * @hw: pointer to hardware structure
 *
 * This function returns true if the device supports flow control
 * autonegotiation, and false if it does not.
 *
 **/
bool ngbe_device_supports_autoneg_fc(struct ngbe_hw *hw)
{
	bool supported = false;
	u32 speed;
	bool link_up;

	DEBUGFUNC("ngbe_device_supports_autoneg_fc");

	switch (hw->phy.media_type) {
	case ngbe_media_type_fiber_qsfp:
	case ngbe_media_type_fiber:
		/* flow control autoneg black list */
		hw->mac.check_link(hw, &speed, &link_up, false);
		/* if link is down, assume supported */
		if (link_up)
			supported = speed == NGBE_LINK_SPEED_1GB_FULL ?
			true : false;
		else
			supported = true;

		break;
	case ngbe_media_type_backplane:
		supported = true;
		break;
	case ngbe_media_type_copper:
		if (hw->phy.type == ngbe_phy_rtl)
			supported = true;
		else {
			/* only some copper devices support flow control autoneg */
			supported = false;
		}
		break;
	default:
		break;
	}

	if (!supported)
		DEBUGOUT(
			      "Device %x does not support flow control autoneg",
			      hw->device_id);
	return supported;
}

/**
 *  ngbe_setup_fc - Set up flow control
 *  @hw: pointer to hardware structure
 *
 *  Called at init time to set up flow control.
 **/
s32 ngbe_setup_fc(struct ngbe_hw *hw)
{
	s32 err = 0;
	u32 reg = 0;
	u16 reg_cu = 0;
	u32 value = 0;
	u64 reg_bp = 0;
	bool locked = false;

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
	 * Set up the 1G and 10G flow control advertisement registers so the
	 * HW will be able to do fc autoneg once the cable is plugged in.  If
	 * we link at 10G, the 1G advertisement is harmless and vice versa.
	 */
	switch (hw->phy.media_type) {
	case ngbe_media_type_backplane:
		/* some MAC's need RMW protection on AUTOC */
		err = hw->mac.prot_autoc_read(hw, &locked, &reg_bp);
		if (err != 0)
			goto out;

		/* fall through - only backplane uses autoc */
	case ngbe_media_type_fiber_qsfp:
	case ngbe_media_type_fiber:
		//reg = rd32(hw, NGBE_PCS1GANA);

		//break;
	case ngbe_media_type_copper:
		hw->phy.read_reg(hw, NGBE_MD_AUTO_NEG_ADVT,
				     NGBE_MD_DEV_AUTO_NEG, &reg_cu);
		break;
	default:
		break;
	}

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
		reg &= ~(SR_MII_MMD_AN_ADV_PAUSE_SYM |
		SR_MII_MMD_AN_ADV_PAUSE_ASM);
		if (hw->phy.media_type == ngbe_media_type_backplane)
			reg_bp &= ~(NGBE_AUTOC_SYM_PAUSE |
				    NGBE_AUTOC_ASM_PAUSE);
		else if (hw->phy.media_type == ngbe_media_type_copper)
			reg_cu &= ~(NGBE_TAF_SYM_PAUSE | NGBE_TAF_ASM_PAUSE);
		break;
	case ngbe_fc_tx_pause:
		/*
		 * Tx Flow control is enabled, and Rx Flow control is
		 * disabled by software override.
		 */
		reg |= SR_MII_MMD_AN_ADV_PAUSE_ASM;
		reg &= ~SR_MII_MMD_AN_ADV_PAUSE_SYM;
		if (hw->phy.media_type == ngbe_media_type_backplane) {
			reg_bp |= NGBE_AUTOC_ASM_PAUSE;
			reg_bp &= ~NGBE_AUTOC_SYM_PAUSE;
		} else if (hw->phy.media_type == ngbe_media_type_copper) {
			reg_cu |= NGBE_TAF_ASM_PAUSE;
			reg_cu &= ~NGBE_TAF_SYM_PAUSE;
		}
		reg |= SR_MII_MMD_AN_ADV_PAUSE_ASM;
		reg_bp |= SR_AN_MMD_ADV_REG1_PAUSE_ASM;
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
		reg |= SR_MII_MMD_AN_ADV_PAUSE_SYM |
			SR_MII_MMD_AN_ADV_PAUSE_ASM;
		if (hw->phy.media_type == ngbe_media_type_backplane)
			reg_bp |= NGBE_AUTOC_SYM_PAUSE |
				  NGBE_AUTOC_ASM_PAUSE;
		else if (hw->phy.media_type == ngbe_media_type_copper)
			reg_cu |= NGBE_TAF_SYM_PAUSE | NGBE_TAF_ASM_PAUSE;
		reg |= SR_MII_MMD_AN_ADV_PAUSE_SYM |
			SR_MII_MMD_AN_ADV_PAUSE_ASM;
		reg_bp |= SR_AN_MMD_ADV_REG1_PAUSE_SYM |
			SR_AN_MMD_ADV_REG1_PAUSE_ASM;
		break;
	default:
		DEBUGOUT("Flow control param set incorrectly\n");
		err = NGBE_ERR_CONFIG;
		goto out;
		break;
	}

	/*
	 * Enable auto-negotiation between the MAC & PHY;
	 * the MAC will advertise clause 37 flow control.
	 */
//	wr32(hw, NGBE_PCS1GANA, reg);
//	reg = rd32(hw, NGBE_PCS1GLCTL);
//
//	/* Disable AN timeout */
//	if (hw->fc.strict_ieee)
//		reg &= ~NGBE_PCS1GLCTL_AN_1G_TIMEOUT_EN;
//
//	wr32(hw, NGBE_PCS1GLCTL, reg);
//	DEBUGOUT("Set up FC; PCS1GLCTL = 0x%08X\n", reg);
	value = rd32_epcs(hw, SR_MII_MMD_AN_ADV);
	value = (value & ~(SR_MII_MMD_AN_ADV_PAUSE_ASM |
		SR_MII_MMD_AN_ADV_PAUSE_SYM)) | reg;
	wr32_epcs(hw, SR_MII_MMD_AN_ADV, value);

	/*
	 * AUTOC restart handles negotiation of 1G and 10G on backplane
	 * and copper. There is no need to set the PCS1GCTL register.
	 *
	 */
	if (hw->phy.media_type == ngbe_media_type_backplane) {
//		reg_bp |= NGBE_AUTOC_AN_RESTART;
//		err = hw->mac.prot_autoc_write(hw, reg_bp, locked);
//		if (err)
//			goto out;
		value = rd32_epcs(hw, SR_AN_MMD_ADV_REG1);
		value = (value & ~(SR_AN_MMD_ADV_REG1_PAUSE_ASM |
			SR_AN_MMD_ADV_REG1_PAUSE_SYM)) |
			reg_bp;
		wr32_epcs(hw, SR_AN_MMD_ADV_REG1, value);
	} else if ((hw->phy.media_type == ngbe_media_type_copper) &&
		    (ngbe_device_supports_autoneg_fc(hw))) {
		switch (hw->phy.type) {
		case ngbe_phy_rtl:
			ngbe_set_phy_pause_adv_rtl(hw, reg_bp);
			break;
		case ngbe_phy_mvl:
			ngbe_set_phy_pause_adv_mvl(hw, reg_bp);
			break;
		default:
			hw->phy.write_reg(hw, NGBE_MD_AUTO_NEG_ADVT,
			      	NGBE_MD_DEV_AUTO_NEG, reg_cu);
			break;
		}
	}

	DEBUGOUT("Set up FC; PCS1GLCTL = 0x%08X\n", reg);
out:
	return err;
}

/**
 *  ngbe_start_hw - Prepare hardware for Tx/Rx
 *  @hw: pointer to hardware structure
 *
 *  Starts the hardware by filling the bus info structure and media type, clears
 *  all on chip counters, initializes receive address registers, multicast
 *  table, VLAN filter table, calls routine to set up link and flow control
 *  settings, and leaves transmit and receive units disabled and uninitialized
 **/
s32 ngbe_start_hw(struct ngbe_hw *hw)
{
	s32 err;

	DEBUGFUNC("ngbe_start_hw");

	/* Clear the VLAN filter table */
	hw->mac.clear_vfta(hw);

	/* Clear statistics registers */
	hw->mac.clear_hw_cntrs(hw);

	/* Setup flow control */
	err = hw->mac.setup_fc(hw);
	if (err != 0 && err != NGBE_NOT_IMPLEMENTED) {
		DEBUGOUT("Flow control setup failed, returning %d\n", err);
		return err;
	}

	/* Clear adapter stopped flag */
	hw->adapter_stopped = false;

	/* We need to run link autotry after the driver loads */
	hw->mac.autotry_restart = true;

	return 0;
}

/**
 *  ngbe_init_hw - Generic hardware initialization
 *  @hw: pointer to hardware structure
 *
 *  Initialize the hardware by resetting the hardware, filling the bus info
 *  structure and media type, clears all on chip counters, initializes receive
 *  address registers, multicast table, VLAN filter table, calls routine to set
 *  up link and flow control settings, and leaves transmit and receive units
 *  disabled and uninitialized
 **/
s32 ngbe_init_hw(struct ngbe_hw *hw)
{
	s32 status;

	DEBUGFUNC("ngbe_init_hw");

	ngbe_read_efuse(hw);
	ngbe_save_eeprom_version(hw);

	/* Reset the hardware */
	status = hw->mac.reset_hw(hw);
	if (status == 0) {
		/* Start the HW */
		status = hw->mac.start_hw(hw);
	}

	/* Initialize the LED link active for LED blink support */
	hw->mac.init_led_link_act(hw);

	if (status != 0)
		DEBUGOUT("Failed to initialize HW, STATUS = %d\n", status);

	return status;
}

/**
 *  ngbe_clear_hw_cntrs - Generic clear hardware counters
 *  @hw: pointer to hardware structure
 *
 *  Clears all hardware statistics counters by reading them from the hardware
 *  Statistics counters are clear on read.
 **/
s32 ngbe_clear_hw_cntrs(struct ngbe_hw *hw)
{
	u16 i = 0;

	DEBUGFUNC("ngbe_clear_hw_cntrs");

	/* QP Stats */
	/* don't write clear queue stats */
	for (i = 0; i < NGBE_MAX_QP; i++) {
		hw->qp_last[i].rx_qp_packets = 0;
		hw->qp_last[i].tx_qp_packets = 0;
		hw->qp_last[i].rx_qp_bytes = 0;
		hw->qp_last[i].tx_qp_bytes = 0;
		hw->qp_last[i].rx_qp_mc_packets = 0;
		hw->qp_last[i].tx_qp_mc_packets = 0;
		hw->qp_last[i].rx_qp_bc_packets = 0;
		hw->qp_last[i].tx_qp_bc_packets = 0;
	}

	/* PB Stats */
//	rd32(hw, NGBE_PBRXMISS);
	rd32(hw, NGBE_PBRXLNKXON);
	rd32(hw, NGBE_PBRXLNKXOFF);
	rd32(hw, NGBE_PBTXLNKXON);
	rd32(hw, NGBE_PBTXLNKXOFF);


	//rd32(hw, NGBE_MLFC);
	//rd32(hw, NGBE_MRFC);

	/* DMA Stats */
	rd32(hw, NGBE_DMARXPKT);
	rd32(hw, NGBE_DMATXPKT);
	
	rd64(hw, NGBE_DMARXOCTL);
	rd64(hw, NGBE_DMATXOCTL);

	/* MAC Stats */
	rd64(hw, NGBE_MACRXERRCRCL);
	rd64(hw, NGBE_MACRXMPKTL);
	rd64(hw, NGBE_MACTXMPKTL);

	rd64(hw, NGBE_MACRXPKTL);
	rd64(hw, NGBE_MACTXPKTL);
	rd64(hw, NGBE_MACRXGBOCTL);

	rd64(hw, NGBE_MACRXOCTL);
	rd32(hw, NGBE_MACTXOCTL);
	
	rd64(hw, NGBE_MACRX1to64L);
	rd64(hw, NGBE_MACRX65to127L);
	rd64(hw, NGBE_MACRX128to255L);
	rd64(hw, NGBE_MACRX256to511L);
	rd64(hw, NGBE_MACRX512to1023L);
	rd64(hw, NGBE_MACRX1024toMAXL);
	rd64(hw, NGBE_MACTX1to64L);
	rd64(hw, NGBE_MACTX65to127L);
	rd64(hw, NGBE_MACTX128to255L);
	rd64(hw, NGBE_MACTX256to511L);
	rd64(hw, NGBE_MACTX512to1023L);
	rd64(hw, NGBE_MACTX1024toMAXL);

	rd64(hw, NGBE_MACRXERRLENL);
	//rd32(hw, NGBE_RFC);
	rd32(hw, NGBE_MACRXOVERSIZE);
	rd32(hw, NGBE_MACRXJABBER);

	/* MACsec Stats */
	rd32(hw, NGBE_LSECTX_UTPKT);
	rd32(hw, NGBE_LSECTX_ENCPKT);
	rd32(hw, NGBE_LSECTX_PROTPKT);
	rd32(hw, NGBE_LSECTX_ENCOCT);
	rd32(hw, NGBE_LSECTX_PROTOCT);
	rd32(hw, NGBE_LSECRX_UTPKT);
	rd32(hw, NGBE_LSECRX_BTPKT);
	rd32(hw, NGBE_LSECRX_NOSCIPKT);
	rd32(hw, NGBE_LSECRX_UNSCIPKT);
	rd32(hw, NGBE_LSECRX_DECOCT);
	rd32(hw, NGBE_LSECRX_VLDOCT);
	rd32(hw, NGBE_LSECRX_UNCHKPKT);
	rd32(hw, NGBE_LSECRX_DLYPKT);
	rd32(hw, NGBE_LSECRX_LATEPKT);
	for (i = 0; i < 2; i++) {
		rd32(hw, NGBE_LSECRX_OKPKT(i));
		rd32(hw, NGBE_LSECRX_INVPKT(i));
		rd32(hw, NGBE_LSECRX_BADPKT(i));
	}
	for (i = 0; i < 4; i++) {
		rd32(hw, NGBE_LSECRX_INVSAPKT(i));
		rd32(hw, NGBE_LSECRX_BADSAPKT(i));
	}

	return 0;
}

/**
 *  ngbe_read_pba_string - Reads part number string from EEPROM
 *  @hw: pointer to hardware structure
 *  @pba_num: stores the part number string from the EEPROM
 *  @pba_num_size: part number string buffer length
 *
 *  Reads the part number string from the EEPROM.
 **/
s32 ngbe_read_pba_string(struct ngbe_hw *hw, u8 *pba_num,
				  u32 pba_num_size)
{
	s32 err;
	u16 data;
	u16 pba_ptr;
	u16 offset;
	u16 length;

	DEBUGFUNC("ngbe_read_pba_string");

	if (pba_num == NULL) {
		DEBUGOUT("PBA string buffer was null\n");
		return NGBE_ERR_INVALID_ARGUMENT;
	}

	err = hw->rom.readw_sw(hw, NGBE_PBANUM0_PTR, &data);
	if (err) {
		DEBUGOUT("NVM Read Error\n");
		return err;
	}

	err = hw->rom.readw_sw(hw, NGBE_PBANUM1_PTR, &pba_ptr);
	if (err) {
		DEBUGOUT("NVM Read Error\n");
		return err;
	}

	/*
	 * if data is not ptr guard the PBA must be in legacy format which
	 * means pba_ptr is actually our second data word for the PBA number
	 * and we can decode it into an ascii string
	 */
	if (data != NGBE_PBANUM_PTR_GUARD) {
		DEBUGOUT("NVM PBA number is not stored as string\n");

		/* we will need 11 characters to store the PBA */
		if (pba_num_size < 11) {
			DEBUGOUT("PBA string buffer too small\n");
			return NGBE_ERR_NO_SPACE;
		}

		/* extract hex string from data and pba_ptr */
		pba_num[0] = (data >> 12) & 0xF;
		pba_num[1] = (data >> 8) & 0xF;
		pba_num[2] = (data >> 4) & 0xF;
		pba_num[3] = data & 0xF;
		pba_num[4] = (pba_ptr >> 12) & 0xF;
		pba_num[5] = (pba_ptr >> 8) & 0xF;
		pba_num[6] = '-';
		pba_num[7] = 0;
		pba_num[8] = (pba_ptr >> 4) & 0xF;
		pba_num[9] = pba_ptr & 0xF;

		/* put a null character on the end of our string */
		pba_num[10] = '\0';

		/* switch all the data but the '-' to hex char */
		for (offset = 0; offset < 10; offset++) {
			if (pba_num[offset] < 0xA)
				pba_num[offset] += '0';
			else if (pba_num[offset] < 0x10)
				pba_num[offset] += 'A' - 0xA;
		}

		return 0;
	}

	err = hw->rom.read16(hw, pba_ptr, &length);
	if (err) {
		DEBUGOUT("NVM Read Error\n");
		return err;
	}

	if (length == 0xFFFF || length == 0) {
		DEBUGOUT("NVM PBA number section invalid length\n");
		return NGBE_ERR_PBA_SECTION;
	}

	/* check if pba_num buffer is big enough */
	if (pba_num_size  < (((u32)length * 2) - 1)) {
		DEBUGOUT("PBA string buffer too small\n");
		return NGBE_ERR_NO_SPACE;
	}

	/* trim pba length from start of string */
	pba_ptr++;
	length--;

	for (offset = 0; offset < length; offset++) {
		err = hw->rom.read16(hw, pba_ptr + offset, &data);
		if (err) {
			DEBUGOUT("NVM Read Error\n");
			return err;
		}
		pba_num[offset * 2] = (u8)(data >> 8);
		pba_num[(offset * 2) + 1] = (u8)(data & 0xFF);
	}
	pba_num[offset * 2] = '\0';

	return 0;
}

/**
 *  ngbe_read_pba_num - Reads part number from EEPROM
 *  @hw: pointer to hardware structure
 *  @pba_num: stores the part number from the EEPROM
 *
 *  Reads the part number from the EEPROM.
 **/
s32 ngbe_read_pba_num(struct ngbe_hw *hw, u32 *pba_num)
{
	s32 err;
	u16 data;

	DEBUGFUNC("ngbe_read_pba_num");

	err = hw->rom.readw_sw(hw, NGBE_PBANUM0_PTR, &data);
	if (err) {
		DEBUGOUT("NVM Read Error\n");
		return err;
	} else if (data == NGBE_PBANUM_PTR_GUARD) {
		DEBUGOUT("NVM Not supported\n");
		return NGBE_NOT_IMPLEMENTED;
	}
	*pba_num = (u32)(data << 16);

	err = hw->rom.readw_sw(hw, NGBE_PBANUM1_PTR, &data);
	if (err) {
		DEBUGOUT("NVM Read Error\n");
		return err;
	}
	*pba_num |= data;

	return 0;
}

/**
 *  ngbe_read_pba_raw
 *  @hw: pointer to the HW structure
 *  @eeprom_buf: optional pointer to EEPROM image
 *  @eeprom_buf_size: size of EEPROM image in words
 *  @max_pba_block_size: PBA block size limit
 *  @pba: pointer to output PBA structure
 *
 *  Reads PBA from EEPROM image when eeprom_buf is not NULL.
 *  Reads PBA from physical EEPROM device when eeprom_buf is NULL.
 *
 **/
s32 ngbe_read_pba_raw(struct ngbe_hw *hw, u16 *eeprom_buf,
		       u32 eeprom_buf_size, u16 max_pba_block_size,
		       struct ngbe_pba *pba)
{
	s32 err;
	u16 pba_block_size;

	if (pba == NULL)
		return NGBE_ERR_PARAM;

	if (eeprom_buf == NULL) {
		err = hw->rom.readw_buffer(hw, NGBE_PBANUM0_PTR, 2,
						     &pba->word[0]);
		if (err)
			return err;
	} else {
		if (eeprom_buf_size > NGBE_PBANUM1_PTR) {
			pba->word[0] = eeprom_buf[NGBE_PBANUM0_PTR];
			pba->word[1] = eeprom_buf[NGBE_PBANUM1_PTR];
		} else {
			return NGBE_ERR_PARAM;
		}
	}

	if (pba->word[0] == NGBE_PBANUM_PTR_GUARD) {
		if (pba->pba_block == NULL)
			return NGBE_ERR_PARAM;

		err = ngbe_get_pba_block_size(hw, eeprom_buf,
						   eeprom_buf_size,
						   &pba_block_size);
		if (err)
			return err;

		if (pba_block_size > max_pba_block_size)
			return NGBE_ERR_PARAM;

		if (eeprom_buf == NULL) {
			err = hw->rom.readw_buffer(hw, pba->word[1],
							     pba_block_size,
							     pba->pba_block);
			if (err)
				return err;
		} else {
			if (eeprom_buf_size > (u32)(pba->word[1] +
					      pba_block_size)) {
				memcpy(pba->pba_block,
				       &eeprom_buf[pba->word[1]],
				       pba_block_size * sizeof(u16));
			} else {
				return NGBE_ERR_PARAM;
			}
		}
	}

	return 0;
}

/**
 *  ngbe_write_pba_raw
 *  @hw: pointer to the HW structure
 *  @eeprom_buf: optional pointer to EEPROM image
 *  @eeprom_buf_size: size of EEPROM image in words
 *  @pba: pointer to PBA structure
 *
 *  Writes PBA to EEPROM image when eeprom_buf is not NULL.
 *  Writes PBA to physical EEPROM device when eeprom_buf is NULL.
 *
 **/
s32 ngbe_write_pba_raw(struct ngbe_hw *hw, u16 *eeprom_buf,
			u32 eeprom_buf_size, struct ngbe_pba *pba)
{
	s32 err;

	if (pba == NULL)
		return NGBE_ERR_PARAM;

	if (eeprom_buf == NULL) {
		err = hw->rom.writew_buffer(hw, NGBE_PBANUM0_PTR, 2,
						      &pba->word[0]);
		if (err)
			return err;
	} else {
		if (eeprom_buf_size > NGBE_PBANUM1_PTR) {
			eeprom_buf[NGBE_PBANUM0_PTR] = pba->word[0];
			eeprom_buf[NGBE_PBANUM1_PTR] = pba->word[1];
		} else {
			return NGBE_ERR_PARAM;
		}
	}

	if (pba->word[0] == NGBE_PBANUM_PTR_GUARD) {
		if (pba->pba_block == NULL)
			return NGBE_ERR_PARAM;

		if (eeprom_buf == NULL) {
			err = hw->rom.writew_buffer(hw, pba->word[1],
							      pba->pba_block[0],
							      pba->pba_block);
			if (err)
				return err;
		} else {
			if (eeprom_buf_size > (u32)(pba->word[1] +
					      pba->pba_block[0])) {
				memcpy(&eeprom_buf[pba->word[1]],
				       pba->pba_block,
				       pba->pba_block[0] * sizeof(u16));
			} else {
				return NGBE_ERR_PARAM;
			}
		}
	}

	return 0;
}

/**
 *  ngbe_get_pba_block_size
 *  @hw: pointer to the HW structure
 *  @eeprom_buf: optional pointer to EEPROM image
 *  @eeprom_buf_size: size of EEPROM image in words
 *  @pba_data_size: pointer to output variable
 *
 *  Returns the size of the PBA block in words. Function operates on EEPROM
 *  image if the eeprom_buf pointer is not NULL otherwise it accesses physical
 *  EEPROM device.
 *
 **/
s32 ngbe_get_pba_block_size(struct ngbe_hw *hw, u16 *eeprom_buf,
			     u32 eeprom_buf_size, u16 *pba_block_size)
{
	s32 err;
	u16 pba_word[2];
	u16 length;

	DEBUGFUNC("ngbe_get_pba_block_size");

	if (eeprom_buf == NULL) {
		err = hw->rom.readw_buffer(hw, NGBE_PBANUM0_PTR, 2,
						     &pba_word[0]);
		if (err)
			return err;
	} else {
		if (eeprom_buf_size > NGBE_PBANUM1_PTR) {
			pba_word[0] = eeprom_buf[NGBE_PBANUM0_PTR];
			pba_word[1] = eeprom_buf[NGBE_PBANUM1_PTR];
		} else {
			return NGBE_ERR_PARAM;
		}
	}

	if (pba_word[0] == NGBE_PBANUM_PTR_GUARD) {
		if (eeprom_buf == NULL) {
			err = hw->rom.read16(hw, pba_word[1] + 0,
						      &length);
			if (err)
				return err;
		} else {
			if (eeprom_buf_size > pba_word[1])
				length = eeprom_buf[pba_word[1] + 0];
			else
				return NGBE_ERR_PARAM;
		}

		if (length == 0xFFFF || length == 0)
			return NGBE_ERR_PBA_SECTION;
	} else {
		/* PBA number in legacy format, there is no PBA Block. */
		length = 0;
	}

	if (pba_block_size != NULL)
		*pba_block_size = length;

	return 0;
}

/**
 *  ngbe_get_mac_addr - Generic get MAC address
 *  @hw: pointer to hardware structure
 *  @mac_addr: Adapter MAC address
 *
 *  Reads the adapter's MAC address from first Receive Address Register (RAR0)
 *  A reset of the adapter must be performed prior to calling this function
 *  in order for the MAC address to have been loaded from the EEPROM into RAR0
 **/
s32 ngbe_get_mac_addr(struct ngbe_hw *hw, u8 *mac_addr)
{
	u32 rar_high;
	u32 rar_low;
	u16 i;

	DEBUGFUNC("ngbe_get_mac_addr");
	
	wr32(hw, NGBE_ETHADDRIDX, 0);
	rar_high = rd32(hw, NGBE_ETHADDRH);
	rar_low = rd32(hw, NGBE_ETHADDRL);

	for (i = 0; i < 2; i++)
		mac_addr[i] = (u8)(rar_high >> (1 - i) * 8);

	for (i = 0; i < 4; i++)
		mac_addr[i + 2] = (u8)(rar_low >> (3 - i) * 8);

	return 0;
}

/**
 *  ngbe_set_lan_id_multi_port_pcie - Set LAN id for PCIe multiple port devices
 *  @hw: pointer to the HW structure
 *
 *  Determines the LAN function id by reading memory-mapped registers and swaps
 *  the port value if requested, and set MAC instance for devices that share
 *  CS4227.
 **/
void ngbe_set_lan_id_multi_port(struct ngbe_hw *hw)
{
	struct ngbe_bus_info *bus = &hw->bus;
	u32 reg = 0;

	DEBUGFUNC("ngbe_set_lan_id_multi_port");

	reg = rd32(hw, NGBE_PORTSTAT);
	bus->lan_id = NGBE_PORTSTAT_ID(reg);
	bus->func = bus->lan_id;
}

/**
 *  ngbe_stop_hw - Generic stop Tx/Rx units
 *  @hw: pointer to hardware structure
 *
 *  Sets the adapter_stopped flag within ngbe_hw struct. Clears interrupts,
 *  disables transmit and receive units. The adapter_stopped flag is used by
 *  the shared code and drivers to determine if the adapter is in a stopped
 *  state and should not touch the hardware.
 **/
s32 ngbe_stop_hw(struct ngbe_hw *hw)
{
	s32 status;
	u16 i;

	DEBUGFUNC("ngbe_stop_hw");

	/*
	 * Set the adapter_stopped flag so other driver functions stop touching
	 * the hardware
	 */
	hw->adapter_stopped = true;

	/* Disable the receive unit */
	ngbe_disable_rx(hw);

	/* Clear interrupt mask to stop interrupts from being generated */
	wr32(hw, NGBE_IENMISC, 0);
	wr32(hw, NGBE_IMS(0), NGBE_IMS_MASK);

	/* Clear any pending interrupts, flush previous writes */
	wr32(hw, NGBE_ICRMISC, NGBE_ICRMISC_MASK);
	wr32(hw, NGBE_ICR(0), NGBE_ICR_MASK);

	wr32(hw, NGBE_BMECTL, 0x3);

	/* Disable the receive unit by stopping each queue */
	for (i = 0; i < hw->mac.max_rx_queues; i++)
		wr32(hw, NGBE_RXCFG(i), 0);

	/* flush all queues disables */
	ngbe_flush(hw);
	msec_delay(2);

	/*
	 * Prevent the PCI-E bus from hanging by disabling PCI-E master
	 * access and verify no pending requests
	 */
	status = ngbe_set_pcie_master(hw, 0);

	if (status)
		return status;

	/* Disable the transmit unit.  Each queue must be disabled. */
	for (i = 0; i < hw->mac.max_tx_queues; i++)
		wr32(hw, NGBE_TXCFG(i), 0);

	ngbe_flush(hw);
	msec_delay(2);

	return 0;
}

/**
 *  ngbe_led_on - Turns on the software controllable LEDs.
 *  @hw: pointer to hardware structure
 *  @index: led number to turn on
 **/
s32 ngbe_led_on(struct ngbe_hw *hw, u32 index)
{
	u32 led_reg = rd32(hw, NGBE_LEDCTL);

	DEBUGFUNC("ngbe_led_on");

	if (index > 3)
		return NGBE_ERR_PARAM;

	/* To turn on the LED, set mode to ON. */
	led_reg |= NGBE_LEDCTL_100M;
	wr32(hw, NGBE_LEDCTL, led_reg);
	ngbe_flush(hw);

	return 0;
}

/**
 *  ngbe_led_off - Turns off the software controllable LEDs.
 *  @hw: pointer to hardware structure
 *  @index: led number to turn off
 **/
s32 ngbe_led_off(struct ngbe_hw *hw, u32 index)
{
	u32 led_reg = rd32(hw, NGBE_LEDCTL);

	DEBUGFUNC("ngbe_led_off");

	if (index > 3)
		return NGBE_ERR_PARAM;

	/* To turn off the LED, set mode to OFF. */
	led_reg &= ~NGBE_LEDCTL_100M;
	wr32(hw, NGBE_LEDCTL, led_reg);
	ngbe_flush(hw);

	return 0;
}

/**
 *  ngbe_validate_mac_addr - Validate MAC address
 *  @mac_addr: pointer to MAC address.
 *
 *  Tests a MAC address to ensure it is a valid Individual Address.
 **/
s32 ngbe_validate_mac_addr(u8 *mac_addr)
{
	s32 status = 0;

	DEBUGFUNC("ngbe_validate_mac_addr");

	/* Make sure it is not a multicast address */
	if (NGBE_IS_MULTICAST(mac_addr)) {
		status = NGBE_ERR_INVALID_MAC_ADDR;
	/* Not a broadcast address */
	} else if (NGBE_IS_BROADCAST(mac_addr)) {
		status = NGBE_ERR_INVALID_MAC_ADDR;
	/* Reject the zero address */
	} else if (mac_addr[0] == 0 && mac_addr[1] == 0 && mac_addr[2] == 0 &&
		   mac_addr[3] == 0 && mac_addr[4] == 0 && mac_addr[5] == 0) {
		status = NGBE_ERR_INVALID_MAC_ADDR;
	}
	return status;
}

/**
 *  ngbe_set_rar - Set Rx address register
 *  @hw: pointer to hardware structure
 *  @index: Receive address register to write
 *  @addr: Address to put into receive address register
 *  @vmdq: VMDq "set" or "pool" index
 *  @enable_addr: set flag that address is active
 *
 *  Puts an ethernet address into a receive address register.
 **/
s32 ngbe_set_rar(struct ngbe_hw *hw, u32 index, u8 *addr, u32 vmdq,
			  u32 enable_addr)
{
	u32 rar_low, rar_high;
	u32 rar_entries = hw->mac.num_rar_entries;

	DEBUGFUNC("ngbe_set_rar");

	/* Make sure we are using a valid rar index range */
	if (index >= rar_entries) {
		DEBUGOUT("RAR index %d is out of range.\n", index);
		return NGBE_ERR_INVALID_ARGUMENT;
	}

	/* setup VMDq pool selection before this RAR gets enabled */
	hw->mac.set_vmdq(hw, index, vmdq);

	/*
	 * HW expects these in little endian so we reverse the byte
	 * order from network order (big endian) to little endian
	 */
	rar_low = NGBE_ETHADDRL_AD0(addr[5]) |
		  NGBE_ETHADDRL_AD1(addr[4]) |
		  NGBE_ETHADDRL_AD2(addr[3]) |
		  NGBE_ETHADDRL_AD3(addr[2]);
	/*
	 * Some parts put the VMDq setting in the extra RAH bits,
	 * so save everything except the lower 16 bits that hold part
	 * of the address and the address valid bit.
	 */
	rar_high = rd32(hw, NGBE_ETHADDRH);
	rar_high &= ~NGBE_ETHADDRH_AD_MASK;
	rar_high |= (NGBE_ETHADDRH_AD4(addr[1]) |
		     NGBE_ETHADDRH_AD5(addr[0]));

	rar_high &= ~NGBE_ETHADDRH_VLD;
	if (enable_addr != 0)
		rar_high |= NGBE_ETHADDRH_VLD;

	wr32(hw, NGBE_ETHADDRIDX, index);
	wr32(hw, NGBE_ETHADDRL, rar_low);
	wr32(hw, NGBE_ETHADDRH, rar_high);

	return 0;
}

/**
 *  ngbe_clear_rar - Remove Rx address register
 *  @hw: pointer to hardware structure
 *  @index: Receive address register to write
 *
 *  Clears an ethernet address from a receive address register.
 **/
s32 ngbe_clear_rar(struct ngbe_hw *hw, u32 index)
{
	u32 rar_high;
	u32 rar_entries = hw->mac.num_rar_entries;

	DEBUGFUNC("ngbe_clear_rar");

	/* Make sure we are using a valid rar index range */
	if (index >= rar_entries) {
		DEBUGOUT("RAR index %d is out of range.\n", index);
		return NGBE_ERR_INVALID_ARGUMENT;
	}

	/*
	 * Some parts put the VMDq setting in the extra RAH bits,
	 * so save everything except the lower 16 bits that hold part
	 * of the address and the address valid bit.
	 */
	wr32(hw, NGBE_ETHADDRIDX, index);
	rar_high = rd32(hw, NGBE_ETHADDRH);
	rar_high &= ~(NGBE_ETHADDRH_AD_MASK | NGBE_ETHADDRH_VLD);

	wr32(hw, NGBE_ETHADDRL, 0);
	wr32(hw, NGBE_ETHADDRH, rar_high);

	/* clear VMDq pool/queue selection for this RAR */
	hw->mac.clear_vmdq(hw, index, BIT_MASK32);

	return 0;
}

/**
 *  ngbe_init_rx_addrs - Initializes receive address filters.
 *  @hw: pointer to hardware structure
 *
 *  Places the MAC address in receive address register 0 and clears the rest
 *  of the receive address registers. Clears the multicast table. Assumes
 *  the receiver is in reset when the routine is called.
 **/
s32 ngbe_init_rx_addrs(struct ngbe_hw *hw)
{
	u32 i;
	u32 psrctl;
	u32 rar_entries = hw->mac.num_rar_entries;

	DEBUGFUNC("ngbe_init_rx_addrs");

	/*
	 * If the current mac address is valid, assume it is a software override
	 * to the permanent address.
	 * Otherwise, use the permanent address from the eeprom.
	 */
	if (ngbe_validate_mac_addr(hw->mac.addr) ==
	    NGBE_ERR_INVALID_MAC_ADDR) {
		/* Get the MAC address from the RAR0 for later reference */
		hw->mac.get_mac_addr(hw, hw->mac.addr);

		DEBUGOUT(" Keeping Current RAR0 Addr =%.2X %.2X %.2X ",
			  hw->mac.addr[0], hw->mac.addr[1],
			  hw->mac.addr[2]);
		DEBUGOUT("%.2X %.2X %.2X\n", hw->mac.addr[3],
			  hw->mac.addr[4], hw->mac.addr[5]);
	} else {
		/* Setup the receive address. */
		DEBUGOUT("Overriding MAC Address in RAR[0]\n");
		DEBUGOUT(" New MAC Addr =%.2X %.2X %.2X ",
			  hw->mac.addr[0], hw->mac.addr[1],
			  hw->mac.addr[2]);
		DEBUGOUT("%.2X %.2X %.2X\n", hw->mac.addr[3],
			  hw->mac.addr[4], hw->mac.addr[5]);

		hw->mac.set_rar(hw, 0, hw->mac.addr, 0, true);
	}

	/* clear VMDq pool/queue selection for RAR 0 */
	hw->mac.clear_vmdq(hw, 0, BIT_MASK32);

	hw->addr_ctrl.overflow_promisc = 0;

	hw->addr_ctrl.rar_used_count = 1;

	/* Zero out the other receive addresses. */
	DEBUGOUT("Clearing RAR[1-%d]\n", rar_entries - 1);
	for (i = 1; i < rar_entries; i++) {
		wr32(hw, NGBE_ETHADDRIDX, i);
		wr32(hw, NGBE_ETHADDRL, 0);
		wr32(hw, NGBE_ETHADDRH, 0);
	}

	/* Clear the MTA */
	hw->addr_ctrl.mta_in_use = 0;
	psrctl = rd32(hw, NGBE_PSRCTL);
	psrctl &= ~(NGBE_PSRCTL_ADHF12_MASK | NGBE_PSRCTL_MCHFENA);
	psrctl |= NGBE_PSRCTL_ADHF12(hw->mac.mc_filter_type);
	wr32(hw, NGBE_PSRCTL, psrctl);

	DEBUGOUT(" Clearing MTA\n");
	for (i = 0; i < hw->mac.mcft_size; i++)
		wr32(hw, NGBE_MCADDRTBL(i), 0);

	ngbe_init_uta_tables(hw);

	return 0;
}

/**
 *  ngbe_add_uc_addr - Adds a secondary unicast address.
 *  @hw: pointer to hardware structure
 *  @addr: new address
 *  @vmdq: VMDq "set" or "pool" index
 *
 *  Adds it to unused receive address register or goes into promiscuous mode.
 **/
void ngbe_add_uc_addr(struct ngbe_hw *hw, u8 *addr, u32 vmdq)
{
	u32 rar_entries = hw->mac.num_rar_entries;
	u32 rar;

	DEBUGFUNC("ngbe_add_uc_addr");

	DEBUGOUT(" UC Addr = %.2X %.2X %.2X %.2X %.2X %.2X\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	/*
	 * Place this address in the RAR if there is room,
	 * else put the controller into promiscuous mode
	 */
	if (hw->addr_ctrl.rar_used_count < rar_entries) {
		rar = hw->addr_ctrl.rar_used_count;
		hw->mac.set_rar(hw, rar, addr, vmdq, true);
		DEBUGOUT("Added a secondary address to RAR[%d]\n", rar);
		hw->addr_ctrl.rar_used_count++;
	} else {
		hw->addr_ctrl.overflow_promisc++;
	}

	DEBUGOUT("ngbe_add_uc_addr Complete\n");
}

/**
 *  ngbe_update_uc_addr_list - Updates MAC list of secondary addresses
 *  @hw: pointer to hardware structure
 *  @addr_list: the list of new addresses
 *  @addr_count: number of addresses
 *  @next: iterator function to walk the address list
 *
 *  The given list replaces any existing list.  Clears the secondary addrs from
 *  receive address registers.  Uses unused receive address registers for the
 *  first secondary addresses, and falls back to promiscuous mode as needed.
 *
 *  Drivers using secondary unicast addresses must set user_set_promisc when
 *  manually putting the device into promiscuous mode.
 **/
s32 ngbe_update_uc_addr_list(struct ngbe_hw *hw, u8 *addr_list,
				      u32 addr_count, ngbe_mc_addr_itr next)
{
	u8 *addr;
	u32 i;
	u32 old_promisc_setting = hw->addr_ctrl.overflow_promisc;
	u32 uc_addr_in_use;
	u32 fctrl;
	u32 vmdq;

	DEBUGFUNC("ngbe_update_uc_addr_list");

	/*
	 * Clear accounting of old secondary address list,
	 * don't count RAR[0]
	 */
	uc_addr_in_use = hw->addr_ctrl.rar_used_count - 1;
	hw->addr_ctrl.rar_used_count -= uc_addr_in_use;
	hw->addr_ctrl.overflow_promisc = 0;

	/* Zero out the other receive addresses */
	DEBUGOUT("Clearing RAR[1-%d]\n", uc_addr_in_use+1);
	for (i = 0; i < uc_addr_in_use; i++) {
		wr32(hw, NGBE_ETHADDRIDX, i + 1);
		wr32(hw, NGBE_ETHADDRL, 0);
		wr32(hw, NGBE_ETHADDRH, 0);
	}

	/* Add the new addresses */
	for (i = 0; i < addr_count; i++) {
		DEBUGOUT(" Adding the secondary addresses:\n");
		addr = next(hw, &addr_list, &vmdq);
		ngbe_add_uc_addr(hw, addr, vmdq);
	}

	if (hw->addr_ctrl.overflow_promisc) {
		/* enable promisc if not already in overflow or set by user */
		if (!old_promisc_setting && !hw->addr_ctrl.user_set_promisc) {
			DEBUGOUT(" Entering address overflow promisc mode\n");
			fctrl = rd32(hw, NGBE_PSRCTL);
			fctrl |= NGBE_PSRCTL_UCP;
			wr32(hw, NGBE_PSRCTL, fctrl);
		}
	} else {
		/* only disable if set by overflow, not by user */
		if (old_promisc_setting && !hw->addr_ctrl.user_set_promisc) {
			DEBUGOUT(" Leaving address overflow promisc mode\n");
			fctrl = rd32(hw, NGBE_PSRCTL);
			fctrl &= ~NGBE_PSRCTL_UCP;
			wr32(hw, NGBE_PSRCTL, fctrl);
		}
	}

	DEBUGOUT("ngbe_update_uc_addr_list Complete\n");
	return 0;
}

/**
 *  ngbe_mta_vector - Determines bit-vector in multicast table to set
 *  @hw: pointer to hardware structure
 *  @mc_addr: the multicast address
 *
 *  Extracts the 12 bits, from a multicast address, to determine which
 *  bit-vector to set in the multicast table. The hardware uses 12 bits, from
 *  incoming rx multicast addresses, to determine the bit-vector to check in
 *  the MTA. Which of the 4 combination, of 12-bits, the hardware uses is set
 *  by the MO field of the MCSTCTRL. The MO field is set during initialization
 *  to mc_filter_type.
 **/
STATIC s32 ngbe_mta_vector(struct ngbe_hw *hw, u8 *mc_addr)
{
	u32 vector = 0;

	DEBUGFUNC("ngbe_mta_vector");

	switch (hw->mac.mc_filter_type) {
	case 0:   /* use bits [47:36] of the address */
		vector = ((mc_addr[4] >> 4) | (((u16)mc_addr[5]) << 4));
		break;
	case 1:   /* use bits [46:35] of the address */
		vector = ((mc_addr[4] >> 3) | (((u16)mc_addr[5]) << 5));
		break;
	case 2:   /* use bits [45:34] of the address */
		vector = ((mc_addr[4] >> 2) | (((u16)mc_addr[5]) << 6));
		break;
	case 3:   /* use bits [43:32] of the address */
		vector = ((mc_addr[4]) | (((u16)mc_addr[5]) << 8));
		break;
	default:  /* Invalid mc_filter_type */
		DEBUGOUT("MC filter type param set incorrectly\n");
		ASSERT(0);
		break;
	}

	/* vector can only be 12-bits or boundary will be exceeded */
	vector &= 0xFFF;
	return vector;
}

/**
 *  ngbe_set_mta - Set bit-vector in multicast table
 *  @hw: pointer to hardware structure
 *  @mc_addr: Multicast address
 *
 *  Sets the bit-vector in the multicast table.
 **/
void ngbe_set_mta(struct ngbe_hw *hw, u8 *mc_addr)
{
	u32 vector;
	u32 vector_bit;
	u32 vector_reg;

	DEBUGFUNC("ngbe_set_mta");

	hw->addr_ctrl.mta_in_use++;

	vector = ngbe_mta_vector(hw, mc_addr);
	DEBUGOUT(" bit-vector = 0x%03X\n", vector);

	/*
	 * The MTA is a register array of 128 32-bit registers. It is treated
	 * like an array of 4096 bits.  We want to set bit
	 * BitArray[vector_value]. So we figure out what register the bit is
	 * in, read it, OR in the new bit, then write back the new value.  The
	 * register is determined by the upper 7 bits of the vector value and
	 * the bit within that register are determined by the lower 5 bits of
	 * the value.
	 */
	vector_reg = (vector >> 5) & 0x7F;
	vector_bit = vector & 0x1F;
	hw->mac.mta_shadow[vector_reg] |= (1 << vector_bit);
}

/**
 *  ngbe_update_mc_addr_list - Updates MAC list of multicast addresses
 *  @hw: pointer to hardware structure
 *  @mc_addr_list: the list of new multicast addresses
 *  @mc_addr_count: number of addresses
 *  @next: iterator function to walk the multicast address list
 *  @clear: flag, when set clears the table beforehand
 *
 *  When the clear flag is set, the given list replaces any existing list.
 *  Hashes the given addresses into the multicast table.
 **/
s32 ngbe_update_mc_addr_list(struct ngbe_hw *hw, u8 *mc_addr_list,
				      u32 mc_addr_count, ngbe_mc_addr_itr next,
				      bool clear)
{
	u32 i;
	u32 vmdq;

	DEBUGFUNC("ngbe_update_mc_addr_list");

	/*
	 * Set the new number of MC addresses that we are being requested to
	 * use.
	 */
	hw->addr_ctrl.num_mc_addrs = mc_addr_count;
	hw->addr_ctrl.mta_in_use = 0;

	/* Clear mta_shadow */
	if (clear) {
		DEBUGOUT(" Clearing MTA\n");
		memset(&hw->mac.mta_shadow, 0, sizeof(hw->mac.mta_shadow));
	}

	/* Update mta_shadow */
	for (i = 0; i < mc_addr_count; i++) {
		DEBUGOUT(" Adding the multicast addresses:\n");
		ngbe_set_mta(hw, next(hw, &mc_addr_list, &vmdq));
	}

	/* Enable mta */
	for (i = 0; i < hw->mac.mcft_size; i++)
		wr32a(hw, NGBE_MCADDRTBL(0), i,
				      hw->mac.mta_shadow[i]);

	if (hw->addr_ctrl.mta_in_use > 0) {
		u32 psrctl = rd32(hw, NGBE_PSRCTL);
		psrctl &= ~(NGBE_PSRCTL_ADHF12_MASK | NGBE_PSRCTL_MCHFENA);
		psrctl |= NGBE_PSRCTL_MCHFENA |
			 NGBE_PSRCTL_ADHF12(hw->mac.mc_filter_type);
		wr32(hw, NGBE_PSRCTL, psrctl);
	}

	DEBUGOUT("ngbe_update_mc_addr_list Complete\n");
	return 0;
}

/**
 *  ngbe_enable_mc - Enable multicast address in RAR
 *  @hw: pointer to hardware structure
 *
 *  Enables multicast address in RAR and the use of the multicast hash table.
 **/
s32 ngbe_enable_mc(struct ngbe_hw *hw)
{
	struct ngbe_addr_filter_info *a = &hw->addr_ctrl;

	DEBUGFUNC("ngbe_enable_mc");

	if (a->mta_in_use > 0) {
		u32 psrctl = rd32(hw, NGBE_PSRCTL);
		psrctl &= ~(NGBE_PSRCTL_ADHF12_MASK | NGBE_PSRCTL_MCHFENA);
		psrctl |= NGBE_PSRCTL_MCHFENA |
			 NGBE_PSRCTL_ADHF12(hw->mac.mc_filter_type);
		wr32(hw, NGBE_PSRCTL, psrctl);
	}

	return 0;
}

/**
 *  ngbe_disable_mc - Disable multicast address in RAR
 *  @hw: pointer to hardware structure
 *
 *  Disables multicast address in RAR and the use of the multicast hash table.
 **/
s32 ngbe_disable_mc(struct ngbe_hw *hw)
{
	struct ngbe_addr_filter_info *a = &hw->addr_ctrl;

	DEBUGFUNC("ngbe_disable_mc");

	if (a->mta_in_use > 0) {
		u32 psrctl = rd32(hw, NGBE_PSRCTL);
		psrctl &= ~(NGBE_PSRCTL_ADHF12_MASK | NGBE_PSRCTL_MCHFENA);
		psrctl |= NGBE_PSRCTL_ADHF12(hw->mac.mc_filter_type);
		wr32(hw, NGBE_PSRCTL, psrctl);
	}
	return 0;
}

/**
 *  ngbe_fc_enable - Enable flow control
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according to the current settings.
 **/
s32 ngbe_fc_enable(struct ngbe_hw *hw)
{
	s32 err = 0;
	u32 mflcn_reg, fccfg_reg;
	u32 pause_time;
	u32 fcrtl, fcrth;

	DEBUGFUNC("ngbe_fc_enable");

	/* Validate the water mark configuration */
	if (!hw->fc.pause_time) {
		err = NGBE_ERR_INVALID_LINK_SETTINGS;
		goto out;
	}

	/* Low water mark of zero causes XOFF floods */
	if ((hw->fc.current_mode & ngbe_fc_tx_pause) &&
		hw->fc.high_water) {
		if (!hw->fc.low_water ||
			hw->fc.low_water >= hw->fc.high_water) {
			DEBUGOUT("Invalid water mark configuration\n");
			err = NGBE_ERR_INVALID_LINK_SETTINGS;
			goto out;
		}
	}

	/* Negotiate the fc mode to use */
	hw->mac.fc_autoneg(hw);

	/* Disable any previous flow control settings */
	mflcn_reg = rd32(hw, NGBE_RXFCCFG);
	mflcn_reg &= ~NGBE_RXFCCFG_FC;

	fccfg_reg = rd32(hw, NGBE_TXFCCFG);
	fccfg_reg &= ~NGBE_TXFCCFG_FC;
	/*
	 * The possible values of fc.current_mode are:
	 * 0: Flow control is completely disabled
	 * 1: Rx flow control is enabled (we can receive pause frames,
	 *    but not send pause frames).
	 * 2: Tx flow control is enabled (we can send pause frames but
	 *    we do not support receiving pause frames).
	 * 3: Both Rx and Tx flow control (symmetric) are enabled.
	 * other: Invalid.
	 */
	switch (hw->fc.current_mode) {
	case ngbe_fc_none:
		/*
		 * Flow control is disabled by software override or autoneg.
		 * The code below will actually disable it in the HW.
		 */
		break;
	case ngbe_fc_rx_pause:
		/*
		 * Rx Flow control is enabled and Tx Flow control is
		 * disabled by software override. Since there really
		 * isn't a way to advertise that we are capable of RX
		 * Pause ONLY, we will advertise that we support both
		 * symmetric and asymmetric Rx PAUSE.  Later, we will
		 * disable the adapter's ability to send PAUSE frames.
		 */
		mflcn_reg |= NGBE_RXFCCFG_FC;
		break;
	case ngbe_fc_tx_pause:
		/*
		 * Tx Flow control is enabled, and Rx Flow control is
		 * disabled by software override.
		 */
		fccfg_reg |= NGBE_TXFCCFG_FC;
		break;
	case ngbe_fc_full:
		/* Flow control (both Rx and Tx) is enabled by SW override. */
		mflcn_reg |= NGBE_RXFCCFG_FC;
		fccfg_reg |= NGBE_TXFCCFG_FC;
		break;
	default:
		DEBUGOUT("Flow control param set incorrectly\n");
		err = NGBE_ERR_CONFIG;
		goto out;
		break;
	}

	/* Set 802.3x based flow control settings. */
	wr32(hw, NGBE_RXFCCFG, mflcn_reg);
	wr32(hw, NGBE_TXFCCFG, fccfg_reg);


	/* Set up and enable Rx high/low water mark thresholds, enable XON. */
	if ((hw->fc.current_mode & ngbe_fc_tx_pause) &&
		hw->fc.high_water) {
		fcrtl = NGBE_FCWTRLO_TH(hw->fc.low_water) |
			NGBE_FCWTRLO_XON;
		fcrth = NGBE_FCWTRHI_TH(hw->fc.high_water) |
			NGBE_FCWTRHI_XOFF;
	} else {
		/*
		 * In order to prevent Tx hangs when the internal Tx
		 * switch is enabled we must set the high water mark
		 * to the Rx packet buffer size - 24KB.  This allows
		 * the Tx switch to function even under heavy Rx
		 * workloads.
		 */
		fcrtl = 0;
		fcrth = rd32(hw, NGBE_PBRXSIZE) - 24576;
	}
	wr32(hw, NGBE_FCWTRLO, fcrtl);
	wr32(hw, NGBE_FCWTRHI, fcrth);

	/* Configure pause time */
	pause_time = NGBE_RXFCFSH_TIME(hw->fc.pause_time);
	wr32(hw, NGBE_FCXOFFTM, pause_time * 0x00010000);

	/* Configure flow control refresh threshold value */
	wr32(hw, NGBE_RXFCRFSH, hw->fc.pause_time / 2);

out:
	return err;
}

/**
 *  ngbe_negotiate_fc - Negotiate flow control
 *  @hw: pointer to hardware structure
 *  @adv_reg: flow control advertised settings
 *  @lp_reg: link partner's flow control settings
 *  @adv_sym: symmetric pause bit in advertisement
 *  @adv_asm: asymmetric pause bit in advertisement
 *  @lp_sym: symmetric pause bit in link partner advertisement
 *  @lp_asm: asymmetric pause bit in link partner advertisement
 *
 *  Find the intersection between advertised settings and link partner's
 *  advertised settings
 **/
s32 ngbe_negotiate_fc(struct ngbe_hw *hw, u32 adv_reg, u32 lp_reg,
		       u32 adv_sym, u32 adv_asm, u32 lp_sym, u32 lp_asm)
{
	if ((!(adv_reg)) ||  (!(lp_reg))) {
		DEBUGOUT("Local or link partner's advertised flow control "
			      "settings are NULL. Local: %x, link partner: %x\n",
			      adv_reg, lp_reg);
		return NGBE_ERR_FC_NOT_NEGOTIATED;
	}

	if ((adv_reg & adv_sym) && (lp_reg & lp_sym)) {
		/*
		 * Now we need to check if the user selected Rx ONLY
		 * of pause frames.  In this case, we had to advertise
		 * FULL flow control because we could not advertise RX
		 * ONLY. Hence, we must now check to see if we need to
		 * turn OFF the TRANSMISSION of PAUSE frames.
		 */
		if (hw->fc.requested_mode == ngbe_fc_full) {
			hw->fc.current_mode = ngbe_fc_full;
			DEBUGOUT("Flow Control = FULL.\n");
		} else {
			hw->fc.current_mode = ngbe_fc_rx_pause;
			DEBUGOUT("Flow Control=RX PAUSE frames only\n");
		}
	} else if (!(adv_reg & adv_sym) && (adv_reg & adv_asm) &&
		   (lp_reg & lp_sym) && (lp_reg & lp_asm)) {
		hw->fc.current_mode = ngbe_fc_tx_pause;
		DEBUGOUT("Flow Control = TX PAUSE frames only.\n");
	} else if ((adv_reg & adv_sym) && (adv_reg & adv_asm) &&
		   !(lp_reg & lp_sym) && (lp_reg & lp_asm)) {
		hw->fc.current_mode = ngbe_fc_rx_pause;
		DEBUGOUT("Flow Control = RX PAUSE frames only.\n");
	} else {
		hw->fc.current_mode = ngbe_fc_none;
		DEBUGOUT("Flow Control = NONE.\n");
	}
	return 0;
}

/**
 *  ngbe_fc_autoneg_fiber - Enable flow control on 1 gig fiber
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according on 1 gig fiber.
 **/
STATIC s32 ngbe_fc_autoneg_fiber(struct ngbe_hw *hw)
{
	u32 pcs_anadv_reg, pcs_lpab_reg;
	s32 err = NGBE_ERR_FC_NOT_NEGOTIATED;

	if (hw->mac.type != ngbe_mac_sp)
		return NGBE_ERR_FC_NOT_SUPPORTED;
	/*
	 * On multispeed fiber at 1g, bail out if
	 * - link is up but AN did not complete, or if
	 * - link is up and AN completed but timed out
	 */
#if 0
	linkstat = rd32(hw, NGBE_PCS1GLSTA);
	if ((!!(linkstat & NGBE_PCS1GLSTA_AN_COMPLETE) == 0) ||
	    (!!(linkstat & NGBE_PCS1GLSTA_AN_TIMED_OUT) == 1)) {
		DEBUGOUT("Auto-Negotiation did not complete or timed out\n");
		return err;
	}
#endif
	pcs_anadv_reg = rd32_epcs(hw, SR_MII_MMD_AN_ADV);
	pcs_lpab_reg = rd32_epcs(hw, SR_MII_MMD_LP_BABL);

	err =  ngbe_negotiate_fc(hw, pcs_anadv_reg,
				      pcs_lpab_reg,
				      SR_MII_MMD_AN_ADV_PAUSE_SYM,
				      SR_MII_MMD_AN_ADV_PAUSE_ASM,
				      SR_MII_MMD_AN_ADV_PAUSE_SYM,
				      SR_MII_MMD_AN_ADV_PAUSE_ASM);

	return err;
}

/**
 *  ngbe_fc_autoneg_backplane - Enable flow control IEEE clause 37
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according to IEEE clause 37.
 **/
STATIC s32 ngbe_fc_autoneg_backplane(struct ngbe_hw *hw)
{
	u32 anlp1_reg, autoc_reg;
	s32 err = NGBE_ERR_FC_NOT_NEGOTIATED;

	/*
	 * On backplane, bail out if
	 * - backplane autoneg was not completed, or if
	 * - link partner is not AN enabled
	 */
#if 0 /* test by wangjian <begin> */
	links = rd32(hw, NGBE_LINKS);
	if ((links & NGBE_LINKS_KX_AN_COMP) == 0) {
		DEBUGOUT("Auto-Negotiation did not complete\n");
		return err;
	}

	if (hw->mac.type == ngbe_mac_sp) {
		links2 = rd32(hw, NGBE_LINKS2);
		if ((links2 & NGBE_LINKS2_AN_SUPPORTED) == 0) {
			DEBUGOUT("Link partner is not AN enabled\n");
			return err;
		}
	}
#endif /* test by wangjian <endif> */

	/*
	 * Read the 10g AN autoc and LP ability registers and resolve
	 * local flow control settings accordingly
	 */
	autoc_reg = rd32_epcs(hw, SR_AN_MMD_ADV_REG1);
	anlp1_reg = rd32_epcs(hw, SR_AN_MMD_LP_ABL1);

	err = ngbe_negotiate_fc(hw, autoc_reg,
		anlp1_reg,
		SR_AN_MMD_ADV_REG1_PAUSE_SYM,
		SR_AN_MMD_ADV_REG1_PAUSE_ASM,
		SR_AN_MMD_ADV_REG1_PAUSE_SYM,
		SR_AN_MMD_ADV_REG1_PAUSE_ASM);

	return err;
}

/**
 *  ngbe_fc_autoneg_copper - Enable flow control IEEE clause 37
 *  @hw: pointer to hardware structure
 *
 *  Enable flow control according to IEEE clause 37.
 **/
STATIC s32 ngbe_fc_autoneg_copper(struct ngbe_hw *hw)
{
	u8 technology_ability_reg = 0;
	u8 lp_technology_ability_reg = 0;
	u16 technology_ability_reg_ex = 0;
	u16 lp_technology_ability_reg_ex = 0;

	switch (hw->phy.type) {
	case ngbe_phy_rtl:
		ngbe_get_phy_advertised_pause_rtl(hw, 
						&technology_ability_reg);
		ngbe_get_phy_lp_advertised_pause_rtl(hw, 
						&lp_technology_ability_reg);
		break;
	case ngbe_phy_mvl:
	case ngbe_phy_mvl_sfi:
		ngbe_get_phy_advertised_pause_mvl(hw, 
						&technology_ability_reg);
		ngbe_get_phy_lp_advertised_pause_mvl(hw, 
						&lp_technology_ability_reg);
		break;
	default:
		hw->phy.read_reg(hw, NGBE_MD_AUTO_NEG_ADVT,
			     NGBE_MD_DEV_AUTO_NEG,
			     &technology_ability_reg_ex);
		hw->phy.read_reg(hw, NGBE_MD_AUTO_NEG_LP,
			     NGBE_MD_DEV_AUTO_NEG,
			     &lp_technology_ability_reg_ex);
		break;
	}

	return ngbe_negotiate_fc(hw, (u32)technology_ability_reg,
				  (u32)lp_technology_ability_reg,
				  NGBE_TAF_SYM_PAUSE, NGBE_TAF_ASM_PAUSE,
				  NGBE_TAF_SYM_PAUSE, NGBE_TAF_ASM_PAUSE);
}

/**
 *  ngbe_fc_autoneg - Configure flow control
 *  @hw: pointer to hardware structure
 *
 *  Compares our advertised flow control capabilities to those advertised by
 *  our link partner, and determines the proper flow control mode to use.
 **/
void ngbe_fc_autoneg(struct ngbe_hw *hw)
{
	s32 err = NGBE_ERR_FC_NOT_NEGOTIATED;
	u32 speed;
	bool link_up;

	DEBUGFUNC("ngbe_fc_autoneg");

	/*
	 * AN should have completed when the cable was plugged in.
	 * Look for reasons to bail out.  Bail out if:
	 * - FC autoneg is disabled, or if
	 * - link is not up.
	 */
	if (hw->fc.disable_fc_autoneg) {
		DEBUGOUT("Flow control autoneg is disabled");
		goto out;
	}

	hw->mac.check_link(hw, &speed, &link_up, false);
	if (!link_up) {
		DEBUGOUT("The link is down");
		goto out;
	}

	switch (hw->phy.media_type) {
	/* Autoneg flow control on fiber adapters */
	case ngbe_media_type_fiber_qsfp:
	case ngbe_media_type_fiber:
		if (hw->phy.type == ngbe_phy_mvl_sfi)
			err = ngbe_fc_autoneg_copper(hw);
		else if (speed == NGBE_LINK_SPEED_1GB_FULL)
			err = ngbe_fc_autoneg_fiber(hw);
		break;

	/* Autoneg flow control on backplane adapters */
	case ngbe_media_type_backplane:
		err = ngbe_fc_autoneg_backplane(hw);
		break;

	/* Autoneg flow control on copper adapters */
	case ngbe_media_type_copper:
		if (ngbe_device_supports_autoneg_fc(hw))
			err = ngbe_fc_autoneg_copper(hw);
		break;

	default:
		break;
	}

out:
	if (err == 0) {
		hw->fc.fc_was_autonegged = true;
	} else {
		hw->fc.fc_was_autonegged = false;
		hw->fc.current_mode = hw->fc.requested_mode;
	}
}

/**
 *  ngbe_set_pcie_master - Disable or Enable PCI-express master access
 *  @hw: pointer to hardware structure
 *
 *  Disables PCI-Express master access and verifies there are no pending
 *  requests. NGBE_ERR_MASTER_REQUESTS_PENDING is returned if master disable
 *  bit hasn't caused the master requests to be disabled, else 0
 *  is returned signifying master requests disabled.
 **/
s32 ngbe_set_pcie_master(struct ngbe_hw *hw, bool enable)
{
	struct rte_pci_device *pci_dev = (struct rte_pci_device *)hw->back;
	s32 status = 0;
	s32 ret = 0;
	u32 i;
	u16 reg;

	DEBUGFUNC("ngbe_set_pcie_master");

	ret = rte_pci_read_config(pci_dev, &reg,
			sizeof(reg), PCI_COMMAND);
	if (ret != sizeof(reg)) {
		DEBUGOUT("Cannot read command from PCI config space!\n");
		return -1;
	}

	if (enable)
		reg |= PCI_COMMAND_MASTER;
	else
		reg &= ~PCI_COMMAND_MASTER;

	ret = rte_pci_write_config(pci_dev, &reg,
			sizeof(reg), PCI_COMMAND);
	if (ret != sizeof(reg)) {
		DEBUGOUT("Cannot write command to PCI config space!\n");
		return -1;
	}

	if (enable)
		goto out;

	/* Exit if master requests are blocked */
	if (!(rd32(hw, NGBE_BMEPEND)) ||
	    NGBE_REMOVED(hw->hw_addr))
		goto out;

	/* Poll for master request bit to clear */
	for (i = 0; i < NGBE_PCI_MASTER_DISABLE_TIMEOUT; i++) {
		usec_delay(100);
		if (!(rd32(hw, NGBE_BMEPEND)))
			goto out;
	}

	PMD_DRV_LOG(DEBUG, "PCIe transaction pending bit also did not clear.");
	status = NGBE_ERR_MASTER_REQUESTS_PENDING;

out:
	return status;
}

/**
 *  ngbe_acquire_swfw_sync - Acquire SWFW semaphore
 *  @hw: pointer to hardware structure
 *  @mask: Mask to specify which semaphore to acquire
 *
 *  Acquires the SWFW semaphore through the GSSR register for the specified
 *  function (CSR, PHY0, PHY1, EEPROM, Flash)
 **/
s32 ngbe_acquire_swfw_sync(struct ngbe_hw *hw, u32 mask)
{
	u32 gssr = 0;
	u32 fwsm = 0;
	u32 swmask = NGBE_MNGSEM_SW(mask);
	u32 fwmask = NGBE_MNGSEM_FW(mask);
	u32 timeout = 200;
	u32 i;

	DEBUGFUNC("ngbe_acquire_swfw_sync");

	for (i = 0; i < timeout; i++) {
		/*
		 * SW NVM semaphore bit is used for access to all
		 * SW_FW_SYNC bits (not just NVM)
		 */
		if (ngbe_get_eeprom_semaphore(hw))
			return NGBE_ERR_SWFW_SYNC;

		gssr = rd32(hw, NGBE_MNGSEM);
		if (!(gssr & (fwmask | swmask))) {
			gssr |= swmask;
			wr32(hw, NGBE_MNGSEM, gssr);
			ngbe_release_eeprom_semaphore(hw);
			return 0;
		} else {
			/* Resource is currently in use by FW or SW */
			ngbe_release_eeprom_semaphore(hw);
			msec_delay(5);
		}
	}

	fwsm = rd32(hw, NGBE_MNGFWSYNC);
	DEBUGOUT("SWFW semaphore not granted: MNG_SWFW_SYNC = 0x%x, MNG_FW_SM = 0x%x\n",
			gssr, fwsm);

	msec_delay(5);
	return NGBE_ERR_SWFW_SYNC;
}

/**
 *  ngbe_release_swfw_sync - Release SWFW semaphore
 *  @hw: pointer to hardware structure
 *  @mask: Mask to specify which semaphore to release
 *
 *  Releases the SWFW semaphore through the GSSR register for the specified
 *  function (CSR, PHY0, PHY1, EEPROM, Flash)
 **/
void ngbe_release_swfw_sync(struct ngbe_hw *hw, u32 mask)
{
	u32 gssr;
	u32 swmask = mask;

	DEBUGFUNC("ngbe_release_swfw_sync");

	ngbe_get_eeprom_semaphore(hw);

	gssr = rd32(hw, NGBE_MNGSEM);
	gssr &= ~swmask;
	wr32(hw, NGBE_MNGSEM, gssr);

	ngbe_release_eeprom_semaphore(hw);
}

/**
 *  ngbe_disable_sec_rx_path - Stops the receive data path
 *  @hw: pointer to hardware structure
 *
 *  Stops the receive data path and waits for the HW to internally empty
 *  the Rx security block
 **/
s32 ngbe_disable_sec_rx_path(struct ngbe_hw *hw)
{
#define NGBE_MAX_SECRX_POLL 4000

	int i;
	int secrxreg;

	DEBUGFUNC("ngbe_disable_sec_rx_path");


	secrxreg = rd32(hw, NGBE_SECRXCTL);
	secrxreg |= NGBE_SECRXCTL_XDSA;
	wr32(hw, NGBE_SECRXCTL, secrxreg);
	for (i = 0; i < NGBE_MAX_SECRX_POLL; i++) {
		secrxreg = rd32(hw, NGBE_SECRXSTAT);
		if (secrxreg & NGBE_SECRXSTAT_RDY)
			break;
		else
			/* Use interrupt-safe sleep just in case */
			usec_delay(10);
	}

	/* For informational purposes only */
	if (i >= NGBE_MAX_SECRX_POLL)
		DEBUGOUT("Rx unit being enabled before security "
			 "path fully disabled.  Continuing with init.\n");

	return 0;
}

/**
 *  ngbe_enable_sec_rx_path - Enables the receive data path
 *  @hw: pointer to hardware structure
 *
 *  Enables the receive data path.
 **/
s32 ngbe_enable_sec_rx_path(struct ngbe_hw *hw)
{
	u32 secrxreg;

	DEBUGFUNC("ngbe_enable_sec_rx_path");

	secrxreg = rd32(hw, NGBE_SECRXCTL);
	secrxreg &= ~NGBE_SECRXCTL_XDSA;
	wr32(hw, NGBE_SECRXCTL, secrxreg);
	ngbe_flush(hw);

	return 0;
}

/**
 *  ngbe_disable_sec_tx_path - Stops the transmit data path
 *  @hw: pointer to hardware structure
 *
 *  Stops the transmit data path and waits for the HW to internally empty
 *  the Tx security block
 **/
int ngbe_disable_sec_tx_path(struct ngbe_hw *hw)
{
#define NGBE_MAX_SECTX_POLL 40

	int i;
	int sectxreg;

	sectxreg = rd32(hw, NGBE_SECTXCTL);
	sectxreg |= NGBE_SECTXCTL_XDSA;
	wr32(hw, NGBE_SECTXCTL, sectxreg);
	for (i = 0; i < NGBE_MAX_SECTX_POLL; i++) {
		sectxreg = rd32(hw, NGBE_SECTXSTAT);
		if (sectxreg & NGBE_SECTXSTAT_RDY)
			break;
		/* Use interrupt-safe sleep just in case */
		usec_delay(1000);
	}

	/* For informational purposes only */
	if (i >= NGBE_MAX_SECTX_POLL)
		PMD_DRV_LOG(DEBUG, "Tx unit being enabled before security "
			 "path fully disabled.  Continuing with init.");

	return 0;
}

/**
 *  ngbe_enable_sec_tx_path - Enables the transmit data path
 *  @hw: pointer to hardware structure
 *
 *  Enables the transmit data path.
 **/
int ngbe_enable_sec_tx_path(struct ngbe_hw *hw)
{
	uint32_t sectxreg;

	sectxreg = rd32(hw, NGBE_SECTXCTL);
	sectxreg &= ~NGBE_SECTXCTL_XDSA;
	wr32(hw, NGBE_SECTXCTL, sectxreg);
	ngbe_flush(hw);

	return 0;
}

/**
 *  ngbe_get_san_mac_addr_offset - Get SAN MAC address offset from the EEPROM
 *  @hw: pointer to hardware structure
 *  @san_mac_offset: SAN MAC address offset
 *
 *  This function will read the EEPROM location for the SAN MAC address
 *  pointer, and returns the value at that location.  This is used in both
 *  get and set mac_addr routines.
 **/
STATIC s32 ngbe_get_san_mac_addr_offset(struct ngbe_hw *hw,
					 u16 *san_mac_offset)
{
	s32 err;

	DEBUGFUNC("ngbe_get_san_mac_addr_offset");

	/*
	 * First read the EEPROM pointer to see if the MAC addresses are
	 * available.
	 */
	err = hw->rom.readw_sw(hw, NGBE_SAN_MAC_ADDR_PTR,
				      san_mac_offset);
	if (err) {
		DEBUGOUT("eeprom at offset %d failed",
			 NGBE_SAN_MAC_ADDR_PTR);
	}

	return err;
}

/**
 *  ngbe_get_san_mac_addr - SAN MAC address retrieval from the EEPROM
 *  @hw: pointer to hardware structure
 *  @san_mac_addr: SAN MAC address
 *
 *  Reads the SAN MAC address from the EEPROM, if it's available.  This is
 *  per-port, so set_lan_id() must be called before reading the addresses.
 *  set_lan_id() is called by identify_sfp(), but this cannot be relied
 *  upon for non-SFP connections, so we must call it here.
 **/
s32 ngbe_get_san_mac_addr(struct ngbe_hw *hw, u8 *san_mac_addr)
{
	u16 san_mac_data, san_mac_offset;
	u8 i;
	s32 err;

	DEBUGFUNC("ngbe_get_san_mac_addr");

	/*
	 * First read the EEPROM pointer to see if the MAC addresses are
	 * available.  If they're not, no point in calling set_lan_id() here.
	 */
	err = ngbe_get_san_mac_addr_offset(hw, &san_mac_offset);
	if (err || san_mac_offset == 0 || san_mac_offset == 0xFFFF)
		goto san_mac_addr_out;

	/* apply the port offset to the address offset */
	(hw->bus.func) ? (san_mac_offset += NGBE_SAN_MAC_ADDR_PORT1_OFFSET) :
			 (san_mac_offset += NGBE_SAN_MAC_ADDR_PORT0_OFFSET);
	for (i = 0; i < 3; i++) {
		err = hw->rom.read16(hw, san_mac_offset,
					      &san_mac_data);
		if (err) {
			DEBUGOUT("eeprom read at offset %d failed",
				 san_mac_offset);
			goto san_mac_addr_out;
		}
		san_mac_addr[i * 2] = (u8)(san_mac_data);
		san_mac_addr[i * 2 + 1] = (u8)(san_mac_data >> 8);
		san_mac_offset++;
	}
	return 0;

san_mac_addr_out:
	/*
	 * No addresses available in this EEPROM.  It's not an
	 * error though, so just wipe the local address and return.
	 */
	for (i = 0; i < 6; i++)
		san_mac_addr[i] = 0xFF;
	return 0;
}

/**
 *  ngbe_set_san_mac_addr - Write the SAN MAC address to the EEPROM
 *  @hw: pointer to hardware structure
 *  @san_mac_addr: SAN MAC address
 *
 *  Write a SAN MAC address to the EEPROM.
 **/
s32 ngbe_set_san_mac_addr(struct ngbe_hw *hw, u8 *san_mac_addr)
{
	s32 err;
	u16 san_mac_data, san_mac_offset;
	u8 i;

	DEBUGFUNC("ngbe_set_san_mac_addr");

	/* Look for SAN mac address pointer.  If not defined, return */
	err = ngbe_get_san_mac_addr_offset(hw, &san_mac_offset);
	if (err || san_mac_offset == 0 || san_mac_offset == 0xFFFF)
		return NGBE_ERR_NO_SAN_ADDR_PTR;

	/* Apply the port offset to the address offset */
	(hw->bus.func) ? (san_mac_offset += NGBE_SAN_MAC_ADDR_PORT1_OFFSET) :
			 (san_mac_offset += NGBE_SAN_MAC_ADDR_PORT0_OFFSET);

	for (i = 0; i < 3; i++) {
		san_mac_data = (u16)((u16)(san_mac_addr[i * 2 + 1]) << 8);
		san_mac_data |= (u16)(san_mac_addr[i * 2]);
		hw->rom.write16(hw, san_mac_offset, san_mac_data);
		san_mac_offset++;
	}

	return 0;
}

/**
 *  ngbe_insert_mac_addr - Find a RAR for this mac address
 *  @hw: pointer to hardware structure
 *  @addr: Address to put into receive address register
 *  @vmdq: VMDq pool to assign
 *
 *  Puts an ethernet address into a receive address register, or
 *  finds the rar that it is aleady in; adds to the pool list
 **/
s32 ngbe_insert_mac_addr(struct ngbe_hw *hw, u8 *addr, u32 vmdq)
{
	static const u32 NO_EMPTY_RAR_FOUND = BIT_MASK32;
	u32 first_empty_rar = NO_EMPTY_RAR_FOUND;
	u32 rar;
	u32 rar_low, rar_high;
	u32 addr_low, addr_high;

	DEBUGFUNC("ngbe_insert_mac_addr");

	/* swap bytes for HW little endian */
	addr_low  = addr[0] | (addr[1] << 8)
			    | (addr[2] << 16)
			    | (addr[3] << 24);
	addr_high = addr[4] | (addr[5] << 8);

	/*
	 * Either find the mac_id in rar or find the first empty space.
	 * rar_highwater points to just after the highest currently used
	 * rar in order to shorten the search.  It grows when we add a new
	 * rar to the top.
	 */
	for (rar = 0; rar < hw->mac.rar_highwater; rar++) {
		wr32(hw, NGBE_ETHADDRIDX, rar);
		rar_high = rd32(hw, NGBE_ETHADDRH);

		if (((NGBE_ETHADDRH_VLD & rar_high) == 0)
		    && first_empty_rar == NO_EMPTY_RAR_FOUND) {
			first_empty_rar = rar;
		} else if ((rar_high & 0xFFFF) == addr_high) {
			rar_low = rd32(hw, NGBE_ETHADDRL);
			if (rar_low == addr_low)
				break;    /* found it already in the rars */
		}
	}

	if (rar < hw->mac.rar_highwater) {
		/* already there so just add to the pool bits */
		ngbe_set_vmdq(hw, rar, vmdq);
	} else if (first_empty_rar != NO_EMPTY_RAR_FOUND) {
		/* stick it into first empty RAR slot we found */
		rar = first_empty_rar;
		ngbe_set_rar(hw, rar, addr, vmdq, true);
	} else if (rar == hw->mac.rar_highwater) {
		/* add it to the top of the list and inc the highwater mark */
		ngbe_set_rar(hw, rar, addr, vmdq, true);
		hw->mac.rar_highwater++;
	} else if (rar >= hw->mac.num_rar_entries) {
		return NGBE_ERR_INVALID_MAC_ADDR;
	}

	/*
	 * If we found rar[0], make sure the default pool bit (we use pool 0)
	 * remains cleared to be sure default pool packets will get delivered
	 */
	if (rar == 0)
		ngbe_clear_vmdq(hw, rar, 0);

	return rar;
}

/**
 *  ngbe_clear_vmdq - Disassociate a VMDq pool index from a rx address
 *  @hw: pointer to hardware struct
 *  @rar: receive address register index to disassociate
 *  @vmdq: VMDq pool index to remove from the rar
 **/
s32 ngbe_clear_vmdq(struct ngbe_hw *hw, u32 rar, u32 vmdq)
{
	u32 mpsar;
	u32 rar_entries = hw->mac.num_rar_entries;

	DEBUGFUNC("ngbe_clear_vmdq");

	/* Make sure we are using a valid rar index range */
	if (rar >= rar_entries) {
		DEBUGOUT("RAR index %d is out of range.\n", rar);
		return NGBE_ERR_INVALID_ARGUMENT;
	}

	wr32(hw, NGBE_ETHADDRIDX, rar);
	mpsar = rd32(hw, NGBE_ETHADDRASS);

	if (NGBE_REMOVED(hw->hw_addr))
		goto done;

	if (!mpsar)
		goto done;

	mpsar &= ~(1 << vmdq);
	wr32(hw, NGBE_ETHADDRASS, mpsar);

	/* was that the last pool using this rar? */
	if (mpsar == 0 && rar != 0)
		hw->mac.clear_rar(hw, rar);
done:
	return 0;
}

/**
 *  ngbe_set_vmdq - Associate a VMDq pool index with a rx address
 *  @hw: pointer to hardware struct
 *  @rar: receive address register index to associate with a VMDq index
 *  @vmdq: VMDq pool index
 **/
s32 ngbe_set_vmdq(struct ngbe_hw *hw, u32 rar, u32 vmdq)
{
	u32 mpsar;
	u32 rar_entries = hw->mac.num_rar_entries;

	DEBUGFUNC("ngbe_set_vmdq");

	/* Make sure we are using a valid rar index range */
	if (rar >= rar_entries) {
		DEBUGOUT("RAR index %d is out of range.\n", rar);
		return NGBE_ERR_INVALID_ARGUMENT;
	}

	wr32(hw, NGBE_ETHADDRIDX, rar);

	mpsar = rd32(hw, NGBE_ETHADDRASS);
	mpsar |= 1 << vmdq;
	wr32(hw, NGBE_ETHADDRASS, mpsar);

	return 0;
}

/**
 *  This function should only be involved in the IOV mode.
 *  In IOV mode, Default pool is next pool after the number of
 *  VFs advertized and not 0.
 *  MPSAR table needs to be updated for SAN_MAC RAR [hw->mac.san_mac_rar_index]
 *
 *  ngbe_set_vmdq_san_mac - Associate default VMDq pool index with a rx address
 *  @hw: pointer to hardware struct
 *  @vmdq: VMDq pool index
 **/
s32 ngbe_set_vmdq_san_mac(struct ngbe_hw *hw, u32 vmdq)
{
	u32 rar = hw->mac.san_mac_rar_index;

	DEBUGFUNC("ngbe_set_vmdq_san_mac");

	if (vmdq > 32)
		return -1;
	
	wr32(hw, NGBE_ETHADDRIDX, rar);
	wr32(hw, NGBE_ETHADDRASS, 1 << vmdq);

	return 0;
}

/**
 *  ngbe_init_uta_tables - Initialize the Unicast Table Array
 *  @hw: pointer to hardware structure
 **/
s32 ngbe_init_uta_tables(struct ngbe_hw *hw)
{
	int i;

	DEBUGFUNC("ngbe_init_uta_tables");
	DEBUGOUT(" Clearing UTA\n");

	for (i = 0; i < 128; i++)
		wr32(hw, NGBE_UCADDRTBL(i), 0);

	return 0;
}

/**
 *  ngbe_find_vlvf_slot - find the vlanid or the first empty slot
 *  @hw: pointer to hardware structure
 *  @vlan: VLAN id to write to VLAN filter
 *  @vlvf_bypass: true to find vlanid only, false returns first empty slot if
 *		  vlanid not found
 *
 *
 *  return the VLVF index where this VLAN id should be placed
 *
 **/
s32 ngbe_find_vlvf_slot(struct ngbe_hw *hw, u32 vlan, bool vlvf_bypass)
{
	s32 regindex, first_empty_slot;
	u32 bits;

	/* short cut the special case */
	if (vlan == 0)
		return 0;

	/* if vlvf_bypass is set we don't want to use an empty slot, we
	 * will simply bypass the VLVF if there are no entries present in the
	 * VLVF that contain our VLAN
	 */
	first_empty_slot = vlvf_bypass ? NGBE_ERR_NO_SPACE : 0;

	/* add VLAN enable bit for comparison */
	vlan |= NGBE_PSRVLAN_EA;

	/* Search for the vlan id in the VLVF entries. Save off the first empty
	 * slot found along the way.
	 *
	 * pre-decrement loop covering (NGBE_NUM_POOL - 1) .. 1
	 */
	for (regindex = NGBE_NUM_POOL; --regindex;) {
		wr32(hw, NGBE_PSRVLANIDX, regindex);
		bits = rd32(hw, NGBE_PSRVLAN);
		if (bits == vlan)
			return regindex;
		if (!first_empty_slot && !bits)
			first_empty_slot = regindex;
	}

	/* If we are here then we didn't find the VLAN.  Return first empty
	 * slot we found during our search, else error.
	 */
	if (!first_empty_slot)
		DEBUGOUT("No space in VLVF.\n");

	return first_empty_slot ? first_empty_slot : NGBE_ERR_NO_SPACE;
}

/**
 *  ngbe_set_vfta - Set VLAN filter table
 *  @hw: pointer to hardware structure
 *  @vlan: VLAN id to write to VLAN filter
 *  @vind: VMDq output index that maps queue to VLAN id in VLVFB
 *  @vlan_on: boolean flag to turn on/off VLAN
 *  @vlvf_bypass: boolean flag indicating updating default pool is okay
 *
 *  Turn on/off specified VLAN in the VLAN filter table.
 **/
s32 ngbe_set_vfta(struct ngbe_hw *hw, u32 vlan, u32 vind,
			   bool vlan_on, bool vlvf_bypass)
{
	u32 regidx, vfta_delta, vfta;
	s32 err;

	DEBUGFUNC("ngbe_set_vfta");

	if (vlan > 4095 || vind > 63)
		return NGBE_ERR_PARAM;

	/*
	 * this is a 2 part operation - first the VFTA, then the
	 * VLVF and VLVFB if VT Mode is set
	 * We don't write the VFTA until we know the VLVF part succeeded.
	 */

	/* Part 1
	 * The VFTA is a bitstring made up of 128 32-bit registers
	 * that enable the particular VLAN id, much like the MTA:
	 *    bits[11-5]: which register
	 *    bits[4-0]:  which bit in the register
	 */
	regidx = vlan / 32;
	vfta_delta = 1 << (vlan % 32);
	vfta = rd32(hw, NGBE_VLANTBL(regidx));

	/*
	 * vfta_delta represents the difference between the current value
	 * of vfta and the value we want in the register.  Since the diff
	 * is an XOR mask we can just update the vfta using an XOR
	 */
	vfta_delta &= vlan_on ? ~vfta : vfta;
	vfta ^= vfta_delta;

	/* Part 2
	 * Call ngbe_set_vlvf to set VLVFB and VLVF
	 */
	err = ngbe_set_vlvf(hw, vlan, vind, vlan_on, &vfta_delta,
					 vfta, vlvf_bypass);
	if (err != 0) {
		if (vlvf_bypass)
			goto vfta_update;
		return err;
	}

vfta_update:
	/* Update VFTA now that we are ready for traffic */
	if (vfta_delta)
		wr32(hw, NGBE_VLANTBL(regidx), vfta);

	return 0;
}

/**
 *  ngbe_set_vlvf - Set VLAN Pool Filter
 *  @hw: pointer to hardware structure
 *  @vlan: VLAN id to write to VLAN filter
 *  @vind: VMDq output index that maps queue to VLAN id in VLVFB
 *  @vlan_on: boolean flag to turn on/off VLAN in VLVF
 *  @vfta_delta: pointer to the difference between the current value of VFTA
 *		 and the desired value
 *  @vfta: the desired value of the VFTA
 *  @vlvf_bypass: boolean flag indicating updating default pool is okay
 *
 *  Turn on/off specified bit in VLVF table.
 **/
s32 ngbe_set_vlvf(struct ngbe_hw *hw, u32 vlan, u32 vind,
			   bool vlan_on, u32 *vfta_delta, u32 vfta,
			   bool vlvf_bypass)
{
	u32 bits;
	u32 portctl;
	s32 vlvf_index;

	DEBUGFUNC("ngbe_set_vlvf");

	if (vlan > 4095 || vind > 63)
		return NGBE_ERR_PARAM;

	/* If VT Mode is set
	 *   Either vlan_on
	 *     make sure the vlan is in VLVF
	 *     set the vind bit in the matching VLVFB
	 *   Or !vlan_on
	 *     clear the pool bit and possibly the vind
	 */
	portctl = rd32(hw, NGBE_PORTCTL);
	if (!(portctl & NGBE_PORTCTL_NUMVT_MASK))
		return 0;

	vlvf_index = ngbe_find_vlvf_slot(hw, vlan, vlvf_bypass);
	if (vlvf_index < 0)
		return vlvf_index;

	wr32(hw, NGBE_PSRVLANIDX, vlvf_index);
	bits = rd32(hw, NGBE_PSRVLANPLM(vind / 32));

	/* set the pool bit */
	bits |= 1 << (vind % 32);
	if (vlan_on)
		goto vlvf_update;

	/* clear the pool bit */
	bits ^= 1 << (vind % 32);

	if (!bits &&
	    !rd32(hw, NGBE_PSRVLANPLM(vind / 32))) {
		/* Clear VFTA first, then disable VLVF.  Otherwise
		 * we run the risk of stray packets leaking into
		 * the PF via the default pool
		 */
		if (*vfta_delta)
			wr32(hw, NGBE_PSRVLANPLM(vlan / 32), vfta);

		/* disable VLVF and clear remaining bit from pool */
		wr32(hw, NGBE_PSRVLAN, 0);
		wr32(hw, NGBE_PSRVLANPLM(vind / 32), 0);

		return 0;
	}

	/* If there are still bits set in the VLVFB registers
	 * for the VLAN ID indicated we need to see if the
	 * caller is requesting that we clear the VFTA entry bit.
	 * If the caller has requested that we clear the VFTA
	 * entry bit but there are still pools/VFs using this VLAN
	 * ID entry then ignore the request.  We're not worried
	 * about the case where we're turning the VFTA VLAN ID
	 * entry bit on, only when requested to turn it off as
	 * there may be multiple pools and/or VFs using the
	 * VLAN ID entry.  In that case we cannot clear the
	 * VFTA bit until all pools/VFs using that VLAN ID have also
	 * been cleared.  This will be indicated by "bits" being
	 * zero.
	 */
	*vfta_delta = 0;

vlvf_update:
	/* record pool change and enable VLAN ID if not already enabled */
	wr32(hw, NGBE_PSRVLANPLM(vind / 32), bits);
	wr32(hw, NGBE_PSRVLAN, NGBE_PSRVLAN_EA | vlan);

	return 0;
}

/**
 *  ngbe_clear_vfta - Clear VLAN filter table
 *  @hw: pointer to hardware structure
 *
 *  Clears the VLAN filer table, and the VMDq index associated with the filter
 **/
s32 ngbe_clear_vfta(struct ngbe_hw *hw)
{
	u32 offset;

	DEBUGFUNC("ngbe_clear_vfta");

	for (offset = 0; offset < hw->mac.vft_size; offset++)
		wr32(hw, NGBE_VLANTBL(offset), 0);

	for (offset = 0; offset < NGBE_NUM_POOL; offset++) {
		wr32(hw, NGBE_PSRVLANIDX, offset);
		wr32(hw, NGBE_PSRVLAN, 0);
		wr32(hw, NGBE_PSRVLANPLM(0), 0);
//		wr32(hw, NGBE_PSRVLANPLM(1), 0);
	}

	return 0;
}

/**
 *  ngbe_need_crosstalk_fix - Determine if we need to do cross talk fix
 *  @hw: pointer to hardware structure
 *
 *  Contains the logic to identify if we need to verify link for the
 *  crosstalk fix
 **/
static bool ngbe_need_crosstalk_fix(struct ngbe_hw *hw)
{

	/* Does FW say we need the fix */
	if (!hw->need_crosstalk_fix)
		return false;

	/* Only consider SFP+ PHYs i.e. media type fiber */
	switch (hw->phy.media_type) {
	case ngbe_media_type_fiber:
	case ngbe_media_type_fiber_qsfp:
		break;
	default:
		return false;
	}

	return true;
}

/**
 *  ngbe_check_mac_link - Determine link and speed status
 *  @hw: pointer to hardware structure
 *  @speed: pointer to link speed
 *  @link_up: true when link is up
 *  @link_up_wait_to_complete: bool used to wait for link up or not
 *
 *  Reads the links register to determine if link is up and the current speed
 **/
s32 ngbe_check_mac_link(struct ngbe_hw *hw, u32 *speed,
				 bool *link_up, bool link_up_wait_to_complete)
{
	u32 links_reg, links_orig;
	u32 i;

	DEBUGFUNC("ngbe_check_mac_link");

	/* If Crosstalk fix enabled do the sanity check of making sure
	 * the SFP+ cage is full.
	 */
	if (ngbe_need_crosstalk_fix(hw)) {
		u32 sfp_cage_full;

		switch (hw->mac.type) {
		case ngbe_mac_sp:
			sfp_cage_full = !rd32m(hw, NGBE_GPIODATA,
					NGBE_GPIOBIT_2);
			break;
		default:
			/* sanity check - No SFP+ devices here */
			sfp_cage_full = false;
			break;
		}

		if (!sfp_cage_full) {
			*link_up = false;
			*speed = NGBE_LINK_SPEED_UNKNOWN;
			return 0;
		}
	}

	/* clear the old state */
	links_orig = rd32(hw, NGBE_PORTSTAT);

	links_reg = rd32(hw, NGBE_PORTSTAT);

	if (links_orig != links_reg) {
		DEBUGOUT("LINKS changed from %08X to %08X\n",
			  links_orig, links_reg);
	}

	if (link_up_wait_to_complete) {
		for (i = 0; i < hw->mac.max_link_up_time; i++) {
			if (links_reg & NGBE_PORTSTAT_BW_100M) {
				*link_up = true;
				break;
			} else {
				*link_up = false;
			}
			msec_delay(100);
			links_reg = rd32(hw, NGBE_PORTSTAT);
		}
	} else {
		if (links_reg & NGBE_PORTSTAT_BW_100M)
			*link_up = true;
		else
			*link_up = false;
	}

	switch (links_reg & NGBE_PORTSTAT_BW_MASK) {
	case NGBE_PORTSTAT_BW_1G:
		*speed = NGBE_LINK_SPEED_1GB_FULL;
		break;
	case NGBE_PORTSTAT_BW_100M:
		*speed = NGBE_LINK_SPEED_100M_FULL;
		break;
	case NGBE_PORTSTAT_BW_10M:
		*speed = NGBE_LINK_SPEED_10M_FULL;
		break;
	default:
		*speed = NGBE_LINK_SPEED_UNKNOWN;
	}

	return 0;
}

/**
 *  ngbe_get_wwn_prefix - Get alternative WWNN/WWPN prefix from
 *  the EEPROM
 *  @hw: pointer to hardware structure
 *  @wwnn_prefix: the alternative WWNN prefix
 *  @wwpn_prefix: the alternative WWPN prefix
 *
 *  This function will read the EEPROM from the alternative SAN MAC address
 *  block to check the support for the alternative WWNN/WWPN prefix support.
 **/
s32 ngbe_get_wwn_prefix(struct ngbe_hw *hw, u16 *wwnn_prefix,
				 u16 *wwpn_prefix)
{
	u16 offset, caps;
	u16 alt_san_mac_blk_offset;

	DEBUGFUNC("ngbe_get_wwn_prefix");

	/* clear output first */
	*wwnn_prefix = 0xFFFF;
	*wwpn_prefix = 0xFFFF;

	/* check if alternative SAN MAC is supported */
	offset = NGBE_ALT_SAN_MAC_ADDR_BLK_PTR;
	if (hw->rom.readw_sw(hw, offset, &alt_san_mac_blk_offset))
		goto wwn_prefix_err;

	if ((alt_san_mac_blk_offset == 0) ||
	    (alt_san_mac_blk_offset == 0xFFFF))
		goto wwn_prefix_out;

	/* check capability in alternative san mac address block */
	offset = alt_san_mac_blk_offset + NGBE_ALT_SAN_MAC_ADDR_CAPS_OFFSET;
	if (hw->rom.read16(hw, offset, &caps))
		goto wwn_prefix_err;
	if (!(caps & NGBE_ALT_SAN_MAC_ADDR_CAPS_ALTWWN))
		goto wwn_prefix_out;

	/* get the corresponding prefix for WWNN/WWPN */
	offset = alt_san_mac_blk_offset + NGBE_ALT_SAN_MAC_ADDR_WWNN_OFFSET;
	if (hw->rom.read16(hw, offset, wwnn_prefix)) {
		DEBUGOUT("eeprom read at offset %d failed", offset);
	}

	offset = alt_san_mac_blk_offset + NGBE_ALT_SAN_MAC_ADDR_WWPN_OFFSET;
	if (hw->rom.read16(hw, offset, wwpn_prefix))
		goto wwn_prefix_err;

wwn_prefix_out:
	return 0;

wwn_prefix_err:
	DEBUGOUT("eeprom read at offset %d failed", offset);
	return 0;
}

/**
 *  ngbe_get_fcoe_boot_status - Get FCOE boot status from EEPROM
 *  @hw: pointer to hardware structure
 *  @bs: the fcoe boot status
 *
 *  This function will read the FCOE boot status from the iSCSI FCOE block
 **/
s32 ngbe_get_fcoe_boot_status(struct ngbe_hw *hw, u16 *bs)
{
	u16 offset, caps, flags;
	s32 status;

	DEBUGFUNC("ngbe_get_fcoe_boot_status");

	/* clear output first */
	*bs = ngbe_fcoe_bootstatus_unavailable;

	/* check if FCOE IBA block is present */
	offset = NGBE_FCOE_IBA_CAPS_BLK_PTR;
	status = hw->rom.read16(hw, offset, &caps);
	if (status != 0)
		goto out;

	if (!(caps & NGBE_FCOE_IBA_CAPS_FCOE))
		goto out;

	/* check if iSCSI FCOE block is populated */
	status = hw->rom.read16(hw, NGBE_ISCSI_FCOE_BLK_PTR, &offset);
	if (status != 0)
		goto out;

	if ((offset == 0) || (offset == 0xFFFF))
		goto out;

	/* read fcoe flags in iSCSI FCOE block */
	offset = offset + NGBE_ISCSI_FCOE_FLAGS_OFFSET;
	status = hw->rom.read16(hw, offset, &flags);
	if (status != 0)
		goto out;

	if (flags & NGBE_ISCSI_FCOE_FLAGS_ENABLE)
		*bs = ngbe_fcoe_bootstatus_enabled;
	else
		*bs = ngbe_fcoe_bootstatus_disabled;

out:
	return status;
}

/**
 *  ngbe_set_mac_anti_spoofing - Enable/Disable MAC anti-spoofing
 *  @hw: pointer to hardware structure
 *  @enable: enable or disable switch for MAC anti-spoofing
 *  @vf: Virtual Function pool - VF Pool to set for MAC anti-spoofing
 *
 **/
void ngbe_set_mac_anti_spoofing(struct ngbe_hw *hw, bool enable, int vf)
{
	int vf_target_shift = vf % 8;
	u32 pfvfspoof;

	pfvfspoof = rd32(hw, NGBE_POOLTXASMAC);
	if (enable)
		pfvfspoof |= (1 << vf_target_shift);
	else
		pfvfspoof &= ~(1 << vf_target_shift);
	wr32(hw, NGBE_POOLTXASMAC, pfvfspoof);
}

/**
 *  ngbe_set_vlan_anti_spoofing - Enable/Disable VLAN anti-spoofing
 *  @hw: pointer to hardware structure
 *  @enable: enable or disable switch for VLAN anti-spoofing
 *  @vf: Virtual Function pool - VF Pool to set for VLAN anti-spoofing
 *
 **/
void ngbe_set_vlan_anti_spoofing(struct ngbe_hw *hw, bool enable, int vf)
{
	int vf_target_shift = vf % 8;
	u32 pfvfspoof;

	pfvfspoof = rd32(hw, NGBE_POOLTXASVLAN);
	if (enable)
		pfvfspoof |= (1 << vf_target_shift);
	else
		pfvfspoof &= ~(1 << vf_target_shift);
	wr32(hw, NGBE_POOLTXASVLAN, pfvfspoof);
}

/**
 *  ngbe_get_device_caps - Get additional device capabilities
 *  @hw: pointer to hardware structure
 *  @device_caps: the EEPROM word with the extra device capabilities
 *
 *  This function will read the EEPROM location for the device capabilities,
 *  and return the word through device_caps.
 **/
s32 ngbe_get_device_caps(struct ngbe_hw *hw, u16 *device_caps)
{
	DEBUGFUNC("ngbe_get_device_caps");

	hw->rom.readw_sw(hw, NGBE_DEVICE_CAPS, device_caps);

	return 0;
}

/**
 *  ngbe_enable_relaxed_ordering_gen2 - Enable relaxed ordering
 *  @hw: pointer to hardware structure
 *
 **/
void ngbe_enable_relaxed_ordering_gen2(struct ngbe_hw *hw)
{
//	u32 regval;
//	u32 i;

	UNREFERENCED_PARAMETER(hw);

	DEBUGFUNC("ngbe_enable_relaxed_ordering_gen2");

//fixme:/* Enable relaxed ordering */
//	for (i = 0; i < hw->mac.max_tx_queues; i++) {
//		regval = rd32(hw, NGBE_DCA_TXCTRL_82599(i));
//		regval |= NGBE_DCA_TXCTRL_DESC_WRO_EN;
//		wr32(hw, NGBE_DCA_TXCTRL_82599(i), regval);
//	}
//
//	for (i = 0; i < hw->mac.max_rx_queues; i++) {
//		regval = rd32(hw, NGBE_DCA_RXCTRL(i));
//		regval |= NGBE_DCA_RXCTRL_DATA_WRO_EN |
//			  NGBE_DCA_RXCTRL_HEAD_WRO_EN;
//		wr32(hw, NGBE_DCA_RXCTRL(i), regval);
//	}

}

/**
 * ngbe_set_pba - Initialize Rx packet buffer
 * @hw: pointer to hardware structure
 * @num_pb: number of packet buffers to allocate
 * @headroom: reserve n KB of headroom
 * @strategy: packet buffer allocation strategy
 **/
void ngbe_set_pba(struct ngbe_hw *hw, int num_pb, u32 headroom,
			     int strategy)
{
	u32 pbsize = hw->mac.rx_pb_size;
	u32 rxpktsize, txpktsize, txpbthresh;

	UNREFERENCED_PARAMETER(hw);

	/* Reserve headroom */
	pbsize -= headroom;

	if (!num_pb)
		num_pb = 1;

	/* Divide remaining packet buffer space amongst the number of packet
	 * buffers requested using supplied strategy.
	 */
	switch (strategy) {
	case PBA_STRATEGY_EQUAL:
		rxpktsize = (pbsize / num_pb);
		rxpktsize <<= 10;
		wr32(hw, NGBE_PBRXSIZE, rxpktsize);
		break;
	default:
		break;
	}

	/* Only support an equally distributed Tx packet buffer strategy. */
	txpktsize = NGBE_PBTXSIZE_MAX / num_pb;
	txpbthresh = (txpktsize / 1024) - NGBE_TXPKT_SIZE_MAX;

	wr32(hw, NGBE_PBTXSIZE, txpktsize);
	wr32(hw, NGBE_PBTXDMATH, txpbthresh);

#if 0	
	/* Clear unused TCs, if any, to zero buffer size*/
	for (; i < NGBE_MAX_UP; i++) {
		wr32(hw, NGBE_PBRXSIZE(i), 0);
		wr32(hw, NGBE_PBTXSIZE(i), 0);
	}
#endif
}

/**
 * ngbe_clear_tx_pending - Clear pending TX work from the PCIe fifo
 * @hw: pointer to the hardware structure
 *
 * The MACs can experience issues if TX work is still pending
 * when a reset occurs.  This function prevents this by flushing the PCIe
 * buffers on the system.
 **/
void ngbe_clear_tx_pending(struct ngbe_hw *hw)
{
	//u32 gcr_ext;
	u32 hlreg0, i, poll;

	/*
	 * If double reset is not requested then all transactions should
	 * already be clear and as such there is no work to do
	 */
	if (!(hw->mac.flags & NGBE_FLAGS_DOUBLE_RESET_REQUIRED))
		return;

	/*
	 * Set loopback enable to prevent any transmits from being sent
	 * should the link come up.  This assumes that the RXCTRL.RXEN bit
	 * has already been cleared.
	 */
	hlreg0 = rd32(hw, NGBE_PSRCTL);
	wr32(hw, NGBE_PSRCTL, hlreg0 | NGBE_PSRCTL_LBENA);

	/* Wait for a last completion before clearing buffers */
	ngbe_flush(hw);
	msec_delay(3);

	/*
	 * Before proceeding, make sure that the PCIe block does not have
	 * transactions pending.
	 */
	poll = (800 * 11) / 10; /* PCIDEVCTRL2: 80ms(min) 34s(worst) */
	for (i = 0; i < poll; i++) {
		usec_delay(100);
		//value = NGBE_READ_PCIE_WORD(hw, NGBE_PCI_DEVICE_STATUS);
		//if (NGBE_REMOVED(hw->hw_addr))
		//	goto out;
		//if (!(value & NGBE_PCI_DEVICE_STATUS_TRANSACTION_PENDING))
		//	goto out;
	}

	/* initiate cleaning flow for buffers in the PCIe transaction layer */
	//fixme:gcr_ext = rd32(hw, NGBE_GCR_EXT);
	//wr32(hw, NGBE_GCR_EXT,
	//		gcr_ext | NGBE_GCR_EXT_BUFFERS_CLEAR);

	/* Flush all writes and allow 20usec for all transactions to clear */
	ngbe_flush(hw);
	usec_delay(20);

	/* restore previous register values */
	//wr32(hw, NGBE_GCR_EXT, gcr_ext);
	wr32(hw, NGBE_PSRCTL, hlreg0);
}

/**
 *  ngbe_get_thermal_sensor_data - Gathers thermal sensor data
 *  @hw: pointer to hardware structure
 *
 *  Returns the thermal sensor data structure
 **/
s32 ngbe_get_thermal_sensor_data(struct ngbe_hw *hw)
{
	struct ngbe_thermal_sensor_data *data = &hw->mac.thermal_sensor_data;
	s64 tsv;
	u32 ts_stat;

	DEBUGFUNC("ngbe_get_thermal_sensor_data");

	/* Only support thermal sensors attached to physical port 0 */
	if (hw->bus.lan_id != 0) {
		return NGBE_NOT_IMPLEMENTED;
	}
	
	ts_stat = rd32(hw, NGBE_TSSTAT);
	tsv = (s64)NGBE_TSSTAT_DATA(ts_stat);

	/* 216 < tsv < 876 */
	tsv = tsv > 876 ? tsv : 876 ;
	
	tsv = tsv/4;

	tsv = tsv - 40;

	data->sensor[0].temp = (s16)tsv;

	return 0;
}

/**
 *  ngbe_init_thermal_sensor_thresh - Inits thermal sensor thresholds
 *  @hw: pointer to hardware structure
 *
 *  Inits the thermal sensor thresholds according to the NVM map
 *  and save off the threshold and location values into mac.thermal_sensor_data
 **/
s32 ngbe_init_thermal_sensor_thresh(struct ngbe_hw *hw)
{
	struct ngbe_thermal_sensor_data *data = &hw->mac.thermal_sensor_data;

	DEBUGFUNC("ngbe_init_thermal_sensor_thresh");

	memset(data, 0, sizeof(struct ngbe_thermal_sensor_data));

	if (hw->bus.lan_id != 0)
		return NGBE_NOT_IMPLEMENTED;

	wr32(hw, NGBE_TSINTR,
		NGBE_TSINTR_AEN | NGBE_TSINTR_DEN);
	wr32(hw, NGBE_TSEN, NGBE_TSEN_ENA);


	data->sensor[0].alarm_thresh = 120;
	wr32(hw, NGBE_TSATHRE, 0x358);
	data->sensor[0].dalarm_thresh = 110;
	wr32(hw, NGBE_TSDTHRE, 0x330);
	
	return 0;
}

s32 ngbe_mac_check_overtemp(struct ngbe_hw *hw)
{
	s32 status = 0;
	u32 ts_state;

	DEBUGFUNC("ngbe_mac_check_overtemp");

	/* Check that the LASI temp alarm status was triggered */
	ts_state = rd32(hw, NGBE_TSALM);

	if (ts_state & NGBE_TSALM_HI)
		status = NGBE_ERR_UNDERTEMP;
	else if (ts_state & NGBE_TSALM_LO)
		status = NGBE_ERR_OVERTEMP;

	return status;
}

/**
 *  ngbe_get_orom_version - Return option ROM from EEPROM
 *
 *  @hw: pointer to hardware structure
 *  @nvm_ver: pointer to output structure
 *
 *  if valid option ROM version, nvm_ver->or_valid set to true
 *  else nvm_ver->or_valid is false.
 **/
void ngbe_get_orom_version(struct ngbe_hw *hw,
			    struct ngbe_nvm_version *nvm_ver)
{
	u16 offset, eeprom_cfg_blkh, eeprom_cfg_blkl;

	nvm_ver->or_valid = false;
	/* Option Rom may or may not be present.  Start with pointer */
	hw->rom.read16(hw, NVM_OROM_OFFSET, &offset);

	/* make sure offset is valid */
	if ((offset == 0x0) || (offset == NVM_INVALID_PTR))
		return;

	hw->rom.read16(hw, offset + NVM_OROM_BLK_HI, &eeprom_cfg_blkh);
	hw->rom.read16(hw, offset + NVM_OROM_BLK_LOW, &eeprom_cfg_blkl);

	/* option rom exists and is valid */
	if ((eeprom_cfg_blkl | eeprom_cfg_blkh) == 0x0 ||
	    eeprom_cfg_blkl == NVM_VER_INVALID ||
	    eeprom_cfg_blkh == NVM_VER_INVALID)
		return;

	nvm_ver->or_valid = true;
	nvm_ver->or_major = eeprom_cfg_blkl >> NVM_OROM_SHIFT;
	nvm_ver->or_build = (eeprom_cfg_blkl << NVM_OROM_SHIFT) |
			    (eeprom_cfg_blkh >> NVM_OROM_SHIFT);
	nvm_ver->or_patch = eeprom_cfg_blkh & NVM_OROM_PATCH_MASK;
}

/**
 *  ngbe_get_oem_prod_version - Return OEM Product version
 *
 *  @hw: pointer to hardware structure
 *  @nvm_ver: pointer to output structure
 *
 *  if valid OEM product version, nvm_ver->oem_valid set to true
 *  else nvm_ver->oem_valid is false.
 **/
void ngbe_get_oem_prod_version(struct ngbe_hw *hw,
				struct ngbe_nvm_version *nvm_ver)
{
	u16 rel_num, prod_ver, mod_len, cap, offset;

	nvm_ver->oem_valid = false;
	hw->rom.read16(hw, NVM_OEM_PROD_VER_PTR, &offset);

	/* Return is offset to OEM Product Version block is invalid */
	if (offset == 0x0 && offset == NVM_INVALID_PTR)
		return;

	/* Read product version block */
	hw->rom.read16(hw, offset, &mod_len);
	hw->rom.read16(hw, offset + NVM_OEM_PROD_VER_CAP_OFF, &cap);

	/* Return if OEM product version block is invalid */
	if (mod_len != NVM_OEM_PROD_VER_MOD_LEN ||
	    (cap & NVM_OEM_PROD_VER_CAP_MASK) != 0x0)
		return;

	hw->rom.read16(hw, offset + NVM_OEM_PROD_VER_OFF_L, &prod_ver);
	hw->rom.read16(hw, offset + NVM_OEM_PROD_VER_OFF_H, &rel_num);

	/* Return if version is invalid */
	if ((rel_num | prod_ver) == 0x0 ||
	    rel_num == NVM_VER_INVALID || prod_ver == NVM_VER_INVALID)
		return;

	nvm_ver->oem_major = prod_ver >> NVM_VER_SHIFT;
	nvm_ver->oem_minor = prod_ver & NVM_VER_MASK;
	nvm_ver->oem_release = rel_num;
	nvm_ver->oem_valid = true;
}

/**
 *  ngbe_get_etk_id - Return Etrack ID from EEPROM
 *
 *  @hw: pointer to hardware structure
 *  @nvm_ver: pointer to output structure
 *
 *  word read errors will return 0xFFFF
 **/
void ngbe_get_etk_id(struct ngbe_hw *hw, struct ngbe_nvm_version *nvm_ver)
{
	u16 etk_id_l, etk_id_h;

	if (hw->rom.read16(hw, NVM_ETK_OFF_LOW, &etk_id_l))
		etk_id_l = NVM_VER_INVALID;
	if (hw->rom.read16(hw, NVM_ETK_OFF_HI, &etk_id_h))
		etk_id_h = NVM_VER_INVALID;

	/* The word order for the version format is determined by high order
	 * word bit 15.
	 */
	if ((etk_id_h & NVM_ETK_VALID) == 0) {
		nvm_ver->etk_id = etk_id_h;
		nvm_ver->etk_id |= (etk_id_l << NVM_ETK_SHIFT);
	} else {
		nvm_ver->etk_id = etk_id_l;
		nvm_ver->etk_id |= (etk_id_h << NVM_ETK_SHIFT);
	}
}

void ngbe_disable_rx(struct ngbe_hw *hw)
{
	u32 pfdtxgswc;

	pfdtxgswc = rd32(hw, NGBE_PSRCTL);
	if (pfdtxgswc & NGBE_PSRCTL_LBENA) {
		pfdtxgswc &= ~NGBE_PSRCTL_LBENA;
		wr32(hw, NGBE_PSRCTL, pfdtxgswc);
		hw->mac.set_lben = true;
	} else {
		hw->mac.set_lben = false;
	}

	wr32m(hw, NGBE_PBRXCTL, NGBE_PBRXCTL_ENA, 0);
	wr32m(hw, NGBE_MACRXCFG, NGBE_MACRXCFG_ENA, 0);
}

void ngbe_enable_rx(struct ngbe_hw *hw)
{
	u32 pfdtxgswc;

	wr32m(hw, NGBE_MACRXCFG, NGBE_MACRXCFG_ENA, NGBE_MACRXCFG_ENA);
	wr32m(hw, NGBE_PBRXCTL, NGBE_PBRXCTL_ENA, NGBE_PBRXCTL_ENA);

	if (hw->mac.set_lben) {
		pfdtxgswc = rd32(hw, NGBE_PSRCTL);
		pfdtxgswc |= NGBE_PSRCTL_LBENA;
		wr32(hw, NGBE_PSRCTL, pfdtxgswc);
		hw->mac.set_lben = false;
	}
}

/**
 *  ngbe_setup_mac_link_multispeed_fiber - Set MAC link speed
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true when waiting for completion is needed
 *
 *  Set the link speed in the MAC and/or PHY register and restarts link.
 **/
s32 ngbe_setup_mac_link_multispeed_fiber(struct ngbe_hw *hw,
					  u32 speed,
					  bool autoneg_wait_to_complete)
{
	u32 link_speed = NGBE_LINK_SPEED_UNKNOWN;
	u32 highest_link_speed = NGBE_LINK_SPEED_UNKNOWN;
	s32 status = 0;
	u32 speedcnt = 0;
	u32 i = 0;
	bool autoneg, link_up = false;

	DEBUGFUNC("ngbe_setup_mac_link_multispeed_fiber");

	/* Mask off requested but non-supported speeds */
	status = hw->mac.get_link_capabilities(hw, &link_speed, &autoneg);
	if (status != 0)
		return status;

	speed &= link_speed;

	/* Try each speed one by one, highest priority first.  We do this in
	 * software because 10Gb fiber doesn't support speed autonegotiation.
	 */
	if (speed & NGBE_LINK_SPEED_10GB_FULL) {
		speedcnt++;
		highest_link_speed = NGBE_LINK_SPEED_10GB_FULL;

		/* Set the module link speed */
		switch (hw->phy.media_type) {
		case ngbe_media_type_fiber:
			hw->mac.set_rate_select_speed(hw,
				NGBE_LINK_SPEED_10GB_FULL);
			break;
		case ngbe_media_type_fiber_qsfp:
			/* QSFP module automatically detects MAC link speed */
			break;
		default:
			DEBUGOUT("Unexpected media type.\n");
			break;
		}

		/* Allow module to change analog characteristics (1G->10G) */
		msec_delay(40);

		status = hw->mac.setup_mac_link(hw,
				NGBE_LINK_SPEED_10GB_FULL,
				autoneg_wait_to_complete);
		if (status != 0)
			return status;

		/* Flap the Tx laser if it has not already been done */
		hw->mac.flap_tx_laser(hw);

		/* Wait for the controller to acquire link.  Per IEEE 802.3ap,
		 * Section 73.10.2, we may have to wait up to 500ms if KR is
		 * attempted.  uses the same timing for 10g SFI.
		 */
		for (i = 0; i < 5; i++) {
			/* Wait for the link partner to also set speed */
			msec_delay(100);

			/* If we have link, just jump out */
			status = hw->mac.check_link(hw, &link_speed,
				&link_up, false);
			if (status != 0)
				return status;

			if (link_up)
				goto out;
		}
	}

	if (speed & NGBE_LINK_SPEED_1GB_FULL) {
		speedcnt++;
		if (highest_link_speed == NGBE_LINK_SPEED_UNKNOWN)
			highest_link_speed = NGBE_LINK_SPEED_1GB_FULL;

		/* Set the module link speed */
		switch (hw->phy.media_type) {
		case ngbe_media_type_fiber:
			hw->mac.set_rate_select_speed(hw,
				NGBE_LINK_SPEED_1GB_FULL);
			break;
		case ngbe_media_type_fiber_qsfp:
			/* QSFP module automatically detects link speed */
			break;
		default:
			DEBUGOUT("Unexpected media type.\n");
			break;
		}

		/* Allow module to change analog characteristics (10G->1G) */
		msec_delay(40);

		status = hw->mac.setup_mac_link(hw,
				NGBE_LINK_SPEED_1GB_FULL,
				autoneg_wait_to_complete);
		if (status != 0)
			return status;

		/* Flap the Tx laser if it has not already been done */
		hw->mac.flap_tx_laser(hw);

		/* Wait for the link partner to also set speed */
		msec_delay(100);

		/* If we have link, just jump out */
		status = hw->mac.check_link(hw, &link_speed, &link_up, false);
		if (status != 0)
			return status;

		if (link_up)
			goto out;
	}

	/* We didn't get link.  Configure back to the highest speed we tried,
	 * (if there was more than one).  We call ourselves back with just the
	 * single highest speed that the user requested.
	 */
	if (speedcnt > 1)
		status = ngbe_setup_mac_link_multispeed_fiber(hw,
						      highest_link_speed,
						      autoneg_wait_to_complete);

out:
	/* Set autoneg_advertised value based on input link speed */
	hw->phy.autoneg_advertised = 0;

	if (speed & NGBE_LINK_SPEED_10GB_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_10GB_FULL;

	if (speed & NGBE_LINK_SPEED_1GB_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_1GB_FULL;

	return status;
}

/**
 *  ngbe_set_soft_rate_select_speed - Set module link speed
 *  @hw: pointer to hardware structure
 *  @speed: link speed to set
 *
 *  Set module link speed via the soft rate select.
 */
void ngbe_set_soft_rate_select_speed(struct ngbe_hw *hw,
					u32 speed)
{
	s32 status;
	u8 rs, eeprom_data;

	switch (speed) {
	case NGBE_LINK_SPEED_10GB_FULL:
		/* one bit mask same as setting on */
		rs = NGBE_SFF_SOFT_RS_SELECT_10G;
		break;
	case NGBE_LINK_SPEED_1GB_FULL:
		rs = NGBE_SFF_SOFT_RS_SELECT_1G;
		break;
	default:
		DEBUGOUT("Invalid fixed module speed\n");
		return;
	}

	/* Set RS0 */
	status = hw->phy.read_i2c_byte(hw, NGBE_SFF_SFF_8472_OSCB,
					   NGBE_I2C_EEPROM_DEV_ADDR2,
					   &eeprom_data);
	if (status) {
		DEBUGOUT("Failed to read Rx Rate Select RS0\n");
		goto out;
	}

	eeprom_data = (eeprom_data & ~NGBE_SFF_SOFT_RS_SELECT_MASK) | rs;

	status = hw->phy.write_i2c_byte(hw, NGBE_SFF_SFF_8472_OSCB,
					    NGBE_I2C_EEPROM_DEV_ADDR2,
					    eeprom_data);
	if (status) {
		DEBUGOUT("Failed to write Rx Rate Select RS0\n");
		goto out;
	}

	/* Set RS1 */
	status = hw->phy.read_i2c_byte(hw, NGBE_SFF_SFF_8472_ESCB,
					   NGBE_I2C_EEPROM_DEV_ADDR2,
					   &eeprom_data);
	if (status) {
		DEBUGOUT("Failed to read Rx Rate Select RS1\n");
		goto out;
	}

	eeprom_data = (eeprom_data & ~NGBE_SFF_SOFT_RS_SELECT_MASK) | rs;

	status = hw->phy.write_i2c_byte(hw, NGBE_SFF_SFF_8472_ESCB,
					    NGBE_I2C_EEPROM_DEV_ADDR2,
					    eeprom_data);
	if (status) {
		DEBUGOUT("Failed to write Rx Rate Select RS1\n");
		goto out;
	}
out:
	return;
}

/**
 *  ngbe_set_mac_type - Sets MAC type
 *  @hw: pointer to the HW structure
 *
 *  This function sets the mac type of the adapter based on the
 *  vendor ID and device ID stored in the hw structure.
 **/
s32 ngbe_set_mac_type(struct ngbe_hw *hw)
{
	s32 err = 0;

	DEBUGFUNC("ngbe_set_mac_type\n");

	if (hw->vendor_id != PCI_VENDOR_ID_WANGXUN) {
		DEBUGOUT("Unsupported vendor id: %x", hw->vendor_id);
		return NGBE_ERR_DEVICE_NOT_SUPPORTED;
	}

	switch (hw->sub_device_id) {
	case NGBE_SUB_DEV_ID_SP_KR_KX_KX4:
		hw->phy.media_type = ngbe_media_type_backplane;
		hw->mac.type = ngbe_mac_sp;
		break;
	case NGBE_SUB_DEV_ID_SP_XAUI:
	case NGBE_SUB_DEV_ID_SP_SGMII:
		hw->phy.media_type = ngbe_media_type_copper;
		hw->mac.type = ngbe_mac_sp;
		break;
	case NGBE_SUB_DEV_ID_SP_SFP:
		hw->phy.media_type = ngbe_media_type_fiber;
		hw->mac.type = ngbe_mac_sp;
		break;
	case NGBE_SUB_DEV_ID_SP_QSFP:
		hw->phy.media_type = ngbe_media_type_fiber_qsfp;
		hw->mac.type = ngbe_mac_sp;
		break;
	case NGBE_SUB_DEV_ID_EM_RTL_SGMII:
	case NGBE_SUB_DEV_ID_EM_MVL_RGMII:
	case NGBE_SUB_DEV_ID_EM_KR_KX_KX4:
	case NGBE_SUB_DEV_ID_EM_XAUI:
	case NGBE_SUB_DEV_ID_EM_T3_LOM:
	case NGBE_SUB_DEV_ID_EM_QSFP:
		hw->phy.media_type = ngbe_media_type_copper;
		hw->mac.type = ngbe_mac_em;
		hw->mac.link_type = ngbe_link_copper;
		break;
	case NGBE_SUB_DEV_ID_EM_RTL_YT8521S_SFP:
		hw->phy.media_type = ngbe_media_type_copper;
		hw->mac.type = ngbe_mac_em;
		hw->mac.link_type = ngbe_link_fiber;
		break;
	case NGBE_SUB_DEV_ID_EM_MVL_SFP:
	case NGBE_SUB_DEV_ID_EM_YT8521S_SFP:
		hw->phy.media_type = ngbe_media_type_fiber;
		hw->mac.type = ngbe_mac_em;
		hw->mac.link_type = ngbe_link_fiber;
		break;
	case NGBE_SUB_DEV_ID_EM_MVL_MIX:
		hw->phy.media_type = ngbe_media_type_unknown;
		hw->mac.type = ngbe_mac_em;
		break;
	case NGBE_SUB_DEV_ID_EM_VF:
		hw->phy.media_type = ngbe_media_type_virtual;
		hw->mac.type = ngbe_mac_em_vf;
		break;
	default:
		err = NGBE_ERR_DEVICE_NOT_SUPPORTED;
		hw->phy.media_type = ngbe_media_type_unknown;
		hw->mac.type = ngbe_mac_unknown;
		DEBUGOUT("Unsupported device id: %x", hw->device_id);
		break;
	}

	DEBUGOUT("ngbe_set_mac_type found mac: %d media: %d, returns: %d\n",
		  hw->mac.type, hw->phy.media_type, err);
	return err;
}

s32 ngbe_setup_sfp_modules(struct ngbe_hw *hw)
{
	s32 err = 0;
	//u16 list_offset, data_offset, data_value;

	DEBUGFUNC("ngbe_setup_sfp_modules");

	if (hw->phy.sfp_type == ngbe_sfp_type_unknown)
		return 0;

	ngbe_init_mac_ops(hw);

	//err = ngbe_get_sfp_init_sequence_offsets(hw, &list_offset,
	//					      &data_offset);
	//if (err != 0)
	//	return err;

	/* PHY config will finish before releasing the semaphore */
	err = hw->mac.acquire_swfw_sync(hw, NGBE_MNGSEM_SWPHY);
	if (err != 0)
		return NGBE_ERR_SWFW_SYNC;

	/* Release the semaphore */
	hw->mac.release_swfw_sync(hw, NGBE_MNGSEM_SWPHY);

	/* Delay obtaining semaphore again to allow FW access
	 * prot_autoc_write uses the semaphore too.
	 */
	msec_delay(hw->rom.semaphore_delay);

	/* Restart DSP and set SFI mode */
	//err = hw->mac.prot_autoc_write(hw,
	//	hw->mac.orig_autoc | NGBE_AUTOC_LMS_10Gs,
	//	false);

	if (err) {
		DEBUGOUT("sfp module setup not complete\n");
		return NGBE_ERR_SFP_SETUP_NOT_COMPLETE;
	}

	return err;
}

/**
 *  ngbe_prot_autoc_read_raptor - Hides MAC differences needed for AUTOC read
 *  @hw: pointer to hardware structure
 *  @locked: Return the if we locked for this read.
 *  @value: Value we read from AUTOC
 *
 *  For this part we need to wrap read-modify-writes with a possible
 *  FW/SW lock.  It is assumed this lock will be freed with the next
 *  prot_autoc_write_raptor().
 */
s32 ngbe_prot_autoc_read_raptor(struct ngbe_hw *hw, bool *locked, u64 *value)
{
	s32 err;
	bool lock_state = false;

	 /* If LESM is on then we need to hold the SW/FW semaphore. */
	if (ngbe_verify_lesm_fw_enabled_raptor(hw)) {
		err = hw->mac.acquire_swfw_sync(hw,
					NGBE_MNGSEM_SWPHY);
		if (err != 0)
			return NGBE_ERR_SWFW_SYNC;

		lock_state = true;
	}

	if (locked)
		*locked = lock_state;

	*value = ngbe_autoc_read(hw);
	return 0;
}

/**
 * ngbe_prot_autoc_write_raptor - Hides MAC differences needed for AUTOC write
 * @hw: pointer to hardware structure
 * @autoc: value to write to AUTOC
 * @locked: bool to indicate whether the SW/FW lock was already taken by
 *           previous prot_autoc_read_raptor.
 *
 * This part may need to hold the SW/FW lock around all writes to
 * AUTOC. Likewise after a write we need to do a pipeline reset.
 */
s32 ngbe_prot_autoc_write_raptor(struct ngbe_hw *hw, bool locked, u64 autoc)
{
	int err = 0;

	/* Blocked by MNG FW so bail */
	if (ngbe_check_reset_blocked(hw))
		goto out;

	/* We only need to get the lock if:
	 *  - We didn't do it already (in the read part of a read-modify-write)
	 *  - LESM is enabled.
	 */
	if (!locked && ngbe_verify_lesm_fw_enabled_raptor(hw)) {
		err = hw->mac.acquire_swfw_sync(hw,
					NGBE_MNGSEM_SWPHY);
		if (err != 0)
			return NGBE_ERR_SWFW_SYNC;

		locked = true;
	}

	ngbe_autoc_write(hw, autoc);
	err = ngbe_reset_pipeline_raptor(hw);

out:
	/* Free the SW/FW semaphore as we either grabbed it here or
	 * already had it when this function was called.
	 */
	if (locked)
		hw->mac.release_swfw_sync(hw, NGBE_MNGSEM_SWPHY);

	return err;
}

/**
 *  ngbe_get_link_capabilities_raptor - Determines link capabilities
 *  @hw: pointer to hardware structure
 *  @speed: pointer to link speed
 *  @autoneg: true when autoneg or autotry is enabled
 *
 *  Determines the link capabilities by reading the AUTOC register.
 **/
s32 ngbe_get_link_capabilities_raptor(struct ngbe_hw *hw,
				      u32 *speed,
				      bool *autoneg)
{
	s32 status = 0;
	u32 autoc = 0;

	DEBUGFUNC("ngbe_get_link_capabilities_raptor");

	/* Check if 1G SFP module. */
	if (hw->phy.sfp_type == ngbe_sfp_type_1g_cu_core0 ||
	    hw->phy.sfp_type == ngbe_sfp_type_1g_cu_core1 ||
	    hw->phy.sfp_type == ngbe_sfp_type_1g_lx_core0 ||
	    hw->phy.sfp_type == ngbe_sfp_type_1g_lx_core1 ||
	    hw->phy.sfp_type == ngbe_sfp_type_1g_sx_core0 ||
	    hw->phy.sfp_type == ngbe_sfp_type_1g_sx_core1) {
		*speed = NGBE_LINK_SPEED_1GB_FULL;
		*autoneg = true;
		return 0;
	}

	/*
	 * Determine link capabilities based on the stored value of AUTOC,
	 * which represents EEPROM defaults.  If AUTOC value has not
	 * been stored, use the current register values.
	 */
	if (hw->mac.orig_link_settings_stored)
		autoc = hw->mac.orig_autoc;
	else
		autoc = hw->mac.autoc_read(hw);

	switch (autoc & NGBE_AUTOC_LMS_MASK) {
	case NGBE_AUTOC_LMS_1G_LINK_NO_AN:
		*speed = NGBE_LINK_SPEED_1GB_FULL;
		*autoneg = false;
		break;

	case NGBE_AUTOC_LMS_10G_LINK_NO_AN:
		*speed = NGBE_LINK_SPEED_10GB_FULL;
		*autoneg = false;
		break;

	case NGBE_AUTOC_LMS_1G_AN:
		*speed = NGBE_LINK_SPEED_1GB_FULL;
		*autoneg = true;
		break;

	case NGBE_AUTOC_LMS_10Gs:
		*speed = NGBE_LINK_SPEED_10GB_FULL;
		*autoneg = false;
		break;

	case NGBE_AUTOC_LMS_KX4_KX_KR:
	case NGBE_AUTOC_LMS_KX4_KX_KR_1G_AN:
		*speed = NGBE_LINK_SPEED_UNKNOWN;
		if (autoc & NGBE_AUTOC_KR_SUPP)
			*speed |= NGBE_LINK_SPEED_10GB_FULL;
		if (autoc & NGBE_AUTOC_KX4_SUPP)
			*speed |= NGBE_LINK_SPEED_10GB_FULL;
		if (autoc & NGBE_AUTOC_KX_SUPP)
			*speed |= NGBE_LINK_SPEED_1GB_FULL;
		*autoneg = true;
		break;

	case NGBE_AUTOC_LMS_KX4_KX_KR_SGMII:
		*speed = NGBE_LINK_SPEED_100M_FULL;
		if (autoc & NGBE_AUTOC_KR_SUPP)
			*speed |= NGBE_LINK_SPEED_10GB_FULL;
		if (autoc & NGBE_AUTOC_KX4_SUPP)
			*speed |= NGBE_LINK_SPEED_10GB_FULL;
		if (autoc & NGBE_AUTOC_KX_SUPP)
			*speed |= NGBE_LINK_SPEED_1GB_FULL;
		*autoneg = true;
		break;

	case NGBE_AUTOC_LMS_SGMII_1G_100M:
		*speed = NGBE_LINK_SPEED_1GB_FULL |
			 NGBE_LINK_SPEED_100M_FULL |
			 NGBE_LINK_SPEED_10M_FULL;
		*autoneg = false;
		break;

	default:
		return NGBE_ERR_LINK_SETUP;
	}

	if (hw->phy.multispeed_fiber) {
		*speed |= NGBE_LINK_SPEED_10GB_FULL |
			  NGBE_LINK_SPEED_1GB_FULL;

		/* QSFP must not enable full auto-negotiation
		 * Limited autoneg is enabled at 1G
		 */
		if (hw->phy.media_type == ngbe_media_type_fiber_qsfp)
			*autoneg = false;
		else
			*autoneg = true;
	}

	return status;
}

/**
 *  ngbe_stop_mac_link_on_d3_raptor - Disables link on D3
 *  @hw: pointer to hardware structure
 *
 *  Disables link during D3 power down sequence.
 *
 **/
void ngbe_stop_mac_link_on_d3_raptor(struct ngbe_hw *hw)
{
#if 0
	u32 autoc2_reg;
	u16 ee_ctrl_2 = 0;

	DEBUGFUNC("ngbe_stop_mac_link_on_d3_raptor");
	hw->rom.read16(hw, NGBE_EEPROM_CTRL_2, &ee_ctrl_2);

	if (!ngbe_mng_present(hw) && !hw->wol_enabled &&
	    ee_ctrl_2 & NGBE_EEPROM_CCD_BIT) {
		autoc2_reg = rd32(hw, NGBE_AUTOC2);
		autoc2_reg |= NGBE_AUTOC_LINK_DSA_D3_MASK;
		wr32(hw, NGBE_AUTOC2, autoc2_reg);
	}
#else
	UNREFERENCED_PARAMETER(hw);
	//fixme
#endif
}

/**
 *  ngbe_start_mac_link_raptor - Setup MAC link settings
 *  @hw: pointer to hardware structure
 *  @autoneg_wait_to_complete: true when waiting for completion is needed
 *
 *  Configures link settings based on values in the ngbe_hw struct.
 *  Restarts the link.  Performs autonegotiation if needed.
 **/
s32 ngbe_start_mac_link_raptor(struct ngbe_hw *hw,
			       bool autoneg_wait_to_complete)
{
	//u32 autoc_reg;
	//u32 links_reg;
	//u32 i;
	s32 status = 0;
	bool got_lock = false;

	DEBUGFUNC("ngbe_start_mac_link_raptor");


	/*  reset_pipeline requires us to hold this lock as it writes to
	 *  AUTOC.
	 */
	if (ngbe_verify_lesm_fw_enabled_raptor(hw)) {
		status = hw->mac.acquire_swfw_sync(hw, NGBE_MNGSEM_SWPHY);
		if (status != 0)
			goto out;

		got_lock = true;
	}

	/* Restart link */
	ngbe_reset_pipeline_raptor(hw);

	if (got_lock)
		hw->mac.release_swfw_sync(hw, NGBE_MNGSEM_SWPHY);

	/* Only poll for autoneg to complete if specified to do so */
	if (autoneg_wait_to_complete) {
//		autoc_reg = rd32_epcs(hw, SR_AN_MMD_ADV_REG1);
//		if ((autoc_reg & NGBE_AUTOC_LMS_MASK) ==
//		     NGBE_AUTOC_LMS_KX4_KX_KR ||
//		    (autoc_reg & NGBE_AUTOC_LMS_MASK) ==
//		     NGBE_AUTOC_LMS_KX4_KX_KR_1G_AN ||
//		    (autoc_reg & NGBE_AUTOC_LMS_MASK) ==
//		     NGBE_AUTOC_LMS_KX4_KX_KR_SGMII) {
//			links_reg = 0; /* Just in case Autoneg time = 0 */
//			for (i = 0; i < NGBE_AUTO_NEG_TIME; i++) {
//				links_reg = rd32(hw, NGBE_PORTSTAT);
//				if (links_reg & NGBE_LINKS_KX_AN_COMP)
//					break;
//				msec_delay(100);
//			}
//			if (!(links_reg & NGBE_LINKS_KX_AN_COMP)) {
//				status = NGBE_ERR_AUTONEG_NOT_COMPLETE;
//				DEBUGOUT("Autoneg did not complete.\n");
//			}
//		}
	}

	/* Add delay to filter out noises during initial link setup */
	msec_delay(50);

out:
	return status;
}

/**
 *  ngbe_disable_tx_laser_multispeed_fiber - Disable Tx laser
 *  @hw: pointer to hardware structure
 *
 *  The base drivers may require better control over SFP+ module
 *  PHY states.  This includes selectively shutting down the Tx
 *  laser on the PHY, effectively halting physical link.
 **/
void ngbe_disable_tx_laser_multispeed_fiber(struct ngbe_hw *hw)
{
	u32 esdp_reg = rd32(hw, NGBE_GPIODATA);

	/* Blocked by MNG FW so bail */
	if (ngbe_check_reset_blocked(hw))
		return;

	/* Disable Tx laser; allow 100us to go dark per spec */
	esdp_reg |= (NGBE_GPIOBIT_0 | NGBE_GPIOBIT_1);
	wr32(hw, NGBE_GPIODATA, esdp_reg);
	ngbe_flush(hw);
	usec_delay(100);
}

/**
 *  ngbe_enable_tx_laser_multispeed_fiber - Enable Tx laser
 *  @hw: pointer to hardware structure
 *
 *  The base drivers may require better control over SFP+ module
 *  PHY states.  This includes selectively turning on the Tx
 *  laser on the PHY, effectively starting physical link.
 **/
void ngbe_enable_tx_laser_multispeed_fiber(struct ngbe_hw *hw)
{
	u32 esdp_reg = rd32(hw, NGBE_GPIODATA);

	/* Enable Tx laser; allow 100ms to light up */
	esdp_reg &= ~(NGBE_GPIOBIT_0 | NGBE_GPIOBIT_1);
	wr32(hw, NGBE_GPIODATA, esdp_reg);
	ngbe_flush(hw);
	msec_delay(100);
}

/**
 *  ngbe_flap_tx_laser_multispeed_fiber - Flap Tx laser
 *  @hw: pointer to hardware structure
 *
 *  When the driver changes the link speeds that it can support,
 *  it sets autotry_restart to true to indicate that we need to
 *  initiate a new autotry session with the link partner.  To do
 *  so, we set the speed then disable and re-enable the Tx laser, to
 *  alert the link partner that it also needs to restart autotry on its
 *  end.  This is consistent with true clause 37 autoneg, which also
 *  involves a loss of signal.
 **/
void ngbe_flap_tx_laser_multispeed_fiber(struct ngbe_hw *hw)
{
	DEBUGFUNC("ngbe_flap_tx_laser_multispeed_fiber");

	/* Blocked by MNG FW so bail */
	if (ngbe_check_reset_blocked(hw))
		return;

	if (hw->mac.autotry_restart) {
		ngbe_disable_tx_laser_multispeed_fiber(hw);
		ngbe_enable_tx_laser_multispeed_fiber(hw);
		hw->mac.autotry_restart = false;
	}
}

/**
 *  ngbe_set_hard_rate_select_speed - Set module link speed
 *  @hw: pointer to hardware structure
 *  @speed: link speed to set
 *
 *  Set module link speed via RS0/RS1 rate select pins.
 */
void ngbe_set_hard_rate_select_speed(struct ngbe_hw *hw,
					u32 speed)
{
	u32 esdp_reg = rd32(hw, NGBE_GPIODATA);

	switch (speed) {
	case NGBE_LINK_SPEED_10GB_FULL:
		esdp_reg |= (NGBE_GPIOBIT_4 | NGBE_GPIOBIT_5);
		break;
	case NGBE_LINK_SPEED_1GB_FULL:
		esdp_reg &= ~(NGBE_GPIOBIT_4 | NGBE_GPIOBIT_5);
		break;
	default:
		DEBUGOUT("Invalid fixed module speed\n");
		return;
	}

	wr32(hw, NGBE_GPIODATA, esdp_reg);
	ngbe_flush(hw);
}

/**
 *  ngbe_setup_mac_link_smartspeed - Set MAC link speed using SmartSpeed
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true when waiting for completion is needed
 *
 *  Implements the Intel SmartSpeed algorithm.
 **/
s32 ngbe_setup_mac_link_smartspeed(struct ngbe_hw *hw,
				    u32 speed,
				    bool autoneg_wait_to_complete)
{
	s32 status = 0;
	u32 link_speed = NGBE_LINK_SPEED_UNKNOWN;
	s32 i, j;
	bool link_up = false;
	u32 autoc_reg = rd32_epcs(hw, SR_AN_MMD_ADV_REG1);

	DEBUGFUNC("ngbe_setup_mac_link_smartspeed");

	 /* Set autoneg_advertised value based on input link speed */
	hw->phy.autoneg_advertised = 0;

	if (speed & NGBE_LINK_SPEED_10GB_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_10GB_FULL;

	if (speed & NGBE_LINK_SPEED_1GB_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_1GB_FULL;

	if (speed & NGBE_LINK_SPEED_100M_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_100M_FULL;

	if (speed & NGBE_LINK_SPEED_10M_FULL)
		hw->phy.autoneg_advertised |= NGBE_LINK_SPEED_10M_FULL;

	/*
	 * Implement Intel SmartSpeed algorithm.  SmartSpeed will reduce the
	 * autoneg advertisement if link is unable to be established at the
	 * highest negotiated rate.  This can sometimes happen due to integrity
	 * issues with the physical media connection.
	 */

	/* First, try to get link with full advertisement */
	hw->phy.smart_speed_active = false;
	for (j = 0; j < NGBE_SMARTSPEED_MAX_RETRIES; j++) {
		status = ngbe_setup_mac_link(hw, speed,
						    autoneg_wait_to_complete);
		if (status != 0)
			goto out;

		/*
		 * Wait for the controller to acquire link.  Per IEEE 802.3ap,
		 * Section 73.10.2, we may have to wait up to 1000ms if KR is
		 * attempted, or 200ms if KX/KX4/BX/BX4 is attempted, per
		 * Table 9 in the AN MAS.
		 */
		for (i = 0; i < 5; i++) {
			msec_delay(100);

			/* If we have link, just jump out */
			status = hw->mac.check_link(hw, &link_speed, &link_up,
						  false);
			if (status != 0)
				goto out;

			if (link_up)
				goto out;
		}
	}

	/*
	 * We didn't get link.  If we advertised KR plus one of KX4/KX
	 * (or BX4/BX), then disable KR and try again.
	 */
	if (((autoc_reg & NGBE_AUTOC_KR_SUPP) == 0) ||
	    ((autoc_reg & NGBE_AUTOC_KX_SUPP) == 0 &&
	     (autoc_reg & NGBE_AUTOC_KX4_SUPP) == 0))
		goto out;

	/* Turn SmartSpeed on to disable KR support */
	hw->phy.smart_speed_active = true;
	status = ngbe_setup_mac_link(hw, speed,
					    autoneg_wait_to_complete);
	if (status != 0)
		goto out;

	/*
	 * Wait for the controller to acquire link.  600ms will allow for
	 * the AN link_fail_inhibit_timer as well for multiple cycles of
	 * parallel detect, both 10g and 1g. This allows for the maximum
	 * connect attempts as defined in the AN MAS table 73-7.
	 */
	for (i = 0; i < 6; i++) {
		msec_delay(100);

		/* If we have link, just jump out */
		status = hw->mac.check_link(hw, &link_speed, &link_up, false);
		if (status != 0)
			goto out;

		if (link_up)
			goto out;
	}

	/* We didn't get link.  Turn SmartSpeed back off. */
	hw->phy.smart_speed_active = false;
	status = ngbe_setup_mac_link(hw, speed,
					    autoneg_wait_to_complete);

out:
	if (link_up && (link_speed == NGBE_LINK_SPEED_1GB_FULL))
		DEBUGOUT("Smartspeed has downgraded the link speed "
		"from the maximum advertised\n");
	return status;
}

/**
 *  ngbe_setup_mac_link - Set MAC link speed
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true when waiting for completion is needed
 *
 *  Set the link speed in the AUTOC register and restarts link.
 **/
s32 ngbe_setup_mac_link(struct ngbe_hw *hw,
			       u32 speed,
			       bool autoneg_wait_to_complete)
{
	bool autoneg = false;
	s32 status = 0;
	
	u64 autoc = hw->mac.autoc_read(hw);
	u64 pma_pmd_10gs = autoc & NGBE_AUTOC_10Gs_PMA_PMD_MASK;
	u64 pma_pmd_1g = autoc & NGBE_AUTOC_1G_PMA_PMD_MASK;
	u64 link_mode = autoc & NGBE_AUTOC_LMS_MASK;
	u64 current_autoc = autoc;
	u64 orig_autoc = 0;
	u32 links_reg;
	u32 i;
	u32 link_capabilities = NGBE_LINK_SPEED_UNKNOWN;

	DEBUGFUNC("ngbe_setup_mac_link");

	/* Check to see if speed passed in is supported. */
	status = hw->mac.get_link_capabilities(hw,
			&link_capabilities, &autoneg);
	if (status)
		return status;

	speed &= link_capabilities;
	if (speed == NGBE_LINK_SPEED_UNKNOWN) {
		return NGBE_ERR_LINK_SETUP;
	}

	/* Use stored value (EEPROM defaults) of AUTOC to find KR/KX4 support*/
	if (hw->mac.orig_link_settings_stored)
		orig_autoc = hw->mac.orig_autoc;
	else
		orig_autoc = autoc;

	link_mode = autoc & NGBE_AUTOC_LMS_MASK;
	pma_pmd_1g = autoc & NGBE_AUTOC_1G_PMA_PMD_MASK;

	if (link_mode == NGBE_AUTOC_LMS_KX4_KX_KR ||
	    link_mode == NGBE_AUTOC_LMS_KX4_KX_KR_1G_AN ||
	    link_mode == NGBE_AUTOC_LMS_KX4_KX_KR_SGMII) {
		/* Set KX4/KX/KR support according to speed requested */
		autoc &= ~(NGBE_AUTOC_KX_SUPP |
			   NGBE_AUTOC_KX4_SUPP |
			   NGBE_AUTOC_KR_SUPP);
		if (speed & NGBE_LINK_SPEED_10GB_FULL) {
			if (orig_autoc & NGBE_AUTOC_KX4_SUPP)
				autoc |= NGBE_AUTOC_KX4_SUPP;
			if ((orig_autoc & NGBE_AUTOC_KR_SUPP) &&
			    (hw->phy.smart_speed_active == false))
				autoc |= NGBE_AUTOC_KR_SUPP;
		}
		if (speed & NGBE_LINK_SPEED_1GB_FULL)
			autoc |= NGBE_AUTOC_KX_SUPP;
	} else if ((pma_pmd_1g == NGBE_AUTOC_1G_SFI) &&
		   (link_mode == NGBE_AUTOC_LMS_1G_LINK_NO_AN ||
		    link_mode == NGBE_AUTOC_LMS_1G_AN)) {
		/* Switch from 1G SFI to 10G SFI if requested */
		if ((speed == NGBE_LINK_SPEED_10GB_FULL) &&
		    (pma_pmd_10gs == NGBE_AUTOC_10Gs_SFI)) {
			autoc &= ~NGBE_AUTOC_LMS_MASK;
			autoc |= NGBE_AUTOC_LMS_10Gs;
		}
	} else if ((pma_pmd_10gs == NGBE_AUTOC_10Gs_SFI) &&
		   (link_mode == NGBE_AUTOC_LMS_10Gs)) {
		/* Switch from 10G SFI to 1G SFI if requested */
		if ((speed == NGBE_LINK_SPEED_1GB_FULL) &&
		    (pma_pmd_1g == NGBE_AUTOC_1G_SFI)) {
			autoc &= ~NGBE_AUTOC_LMS_MASK;
			if (autoneg || hw->phy.type == ngbe_phy_qsfp_intel)
				autoc |= NGBE_AUTOC_LMS_1G_AN;
			else
				autoc |= NGBE_AUTOC_LMS_1G_LINK_NO_AN;
		}
	}

	if (autoc == current_autoc) {
		return status;
	}

	autoc &= ~NGBE_AUTOC_SPEED_MASK;
	autoc |= NGBE_AUTOC_SPEED(speed);
	autoc |= (autoneg ? NGBE_AUTOC_AUTONEG : 0);

	/* Restart link */
	hw->mac.autoc_write(hw, autoc);

	/* Only poll for autoneg to complete if specified to do so */
	if (autoneg_wait_to_complete) {
		if (link_mode == NGBE_AUTOC_LMS_KX4_KX_KR ||
		    link_mode == NGBE_AUTOC_LMS_KX4_KX_KR_1G_AN ||
		    link_mode == NGBE_AUTOC_LMS_KX4_KX_KR_SGMII) {
			links_reg = 0; /*Just in case Autoneg time=0*/
			for (i = 0; i < NGBE_AUTO_NEG_TIME; i++) {
				links_reg = rd32(hw, NGBE_PORTSTAT);
				if (links_reg & NGBE_PORTSTAT_BW_100M)
					break;
				msec_delay(100);
			}
			if (!(links_reg & NGBE_PORTSTAT_BW_100M)) {
				status = NGBE_ERR_AUTONEG_NOT_COMPLETE;
				DEBUGOUT("Autoneg did not complete.\n");
			}
		}
	}

	/* Add delay to filter out noises during initial link setup */
	msec_delay(50);

	return status;
}

#if 0
/**
 *  ngbe_setup_copper_link_raptor - Set the PHY autoneg advertised field
 *  @hw: pointer to hardware structure
 *  @speed: new link speed
 *  @autoneg_wait_to_complete: true if waiting is needed to complete
 *
 *  Restarts link on PHY and MAC based on settings passed in.
 **/
STATIC s32 ngbe_setup_copper_link_raptor(struct ngbe_hw *hw,
					 u32 speed,
					 bool autoneg_wait_to_complete)
{
	s32 status;

	DEBUGFUNC("ngbe_setup_copper_link_raptor");

	/* Setup the PHY according to input speed */
	status = hw->phy.setup_link(hw, speed, autoneg_wait_to_complete);
	/* Set up MAC */
	ngbe_start_mac_link_raptor(hw, autoneg_wait_to_complete);

	return status;
}
#endif

static int
ngbe_check_flash_load(struct ngbe_hw *hw, u32 check_bit)
{
	u32 reg = 0;
	int err = 0;
	/* if there's flash existing */
	if (!(rd32(hw, NGBE_SPISTAT) & NGBE_SPISTAT_BPFLASH)) {
		u32 i;
		/* wait hw load flash done */
		for (i = 0; i < 10; i++) {
			reg = rd32(hw, NGBE_ILDRSTAT);
			if (!(reg & check_bit)) {
				/* done */
				break;
			}
			msleep(100);
		}
		if (i == 10) {
			err = NGBE_ERR_FLASH_LOADING_FAILED;
			//e_dev_err("HW Loading Flash failed: %d\n", err);
		}
	}
	return err;
}

static void
ngbe_reset_misc(struct ngbe_hw *hw)
{
	int i;

	wr32(hw, NGBE_ISBADDRL, hw->isb_dma & 0x00000000FFFFFFFF);
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

	ngbe_init_thermal_sensor_thresh(hw);

	/* enable mac transmiter */
	wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_TE, NGBE_MACTXCFG_TE);

	for (i = 0; i < 4; i++) {
		wr32m(hw, NGBE_IVAR(i), 0x80808080, 0);
	}
}

/**
 *  ngbe_reset_hw - Perform hardware reset
 *  @hw: pointer to hardware structure
 *
 *  Resets the hardware by resetting the transmit and receive units, masks
 *  and clears all interrupts, perform a PHY reset, and perform a link (MAC)
 *  reset.
 **/
s32 ngbe_reset_hw(struct ngbe_hw *hw)
{
	s32 status;
	u32 autoc;

	DEBUGFUNC("ngbe_reset_hw");

	/* Call adapter stop to disable tx/rx and clear interrupts */
	status = hw->mac.stop_hw(hw);
	if (status != 0)
		return status;

	/* flush pending Tx transactions */
	ngbe_clear_tx_pending(hw);

	/* PHY ops must be identified and initialized prior to reset */

	/* Identify PHY and related function pointers */
	status = ngbe_init_phy(hw);
	if (status == NGBE_ERR_SFP_NOT_SUPPORTED)
		return status;

	/* delete me: Initialize MAC and SFP module */
	status = ngbe_init_mac_ops(hw);
	if (status == NGBE_ERR_SFP_NOT_SUPPORTED)
		return status;

	/* Setup SFP module if there is one present. */
	if (hw->phy.sfp_setup_needed) {
		status = hw->mac.setup_sfp(hw);
		hw->phy.sfp_setup_needed = false;
	}
	if (status == NGBE_ERR_SFP_NOT_SUPPORTED)
		return status;

	/* Reset PHY */
	if (hw->phy.reset_disable == false)
		hw->phy.reset_hw(hw);

	/* remember AUTOC from before we reset */
	autoc = hw->mac.autoc_read(hw);

mac_reset_top:
	/*
	 * Issue global reset to the MAC.  Needs to be SW reset if link is up.
	 * If link reset is used when link is up, it might reset the PHY when
	 * mng is using it.  If link is down or the flag to force full link
	 * reset is set, then perform link reset.
	 */
	if (ngbe_mng_present(hw)) {
		ngbe_hic_reset(hw);
	} else {
		wr32(hw, NGBE_RST, NGBE_RST_LAN(hw->bus.lan_id));
		ngbe_flush(hw);
	}
	usec_delay(10);

	ngbe_reset_misc(hw);

	if (hw->bus.lan_id == 0) {
		status = ngbe_check_flash_load(hw,
				NGBE_ILDRSTAT_SWRST_LAN0);
	} else {
		status = ngbe_check_flash_load(hw,
				NGBE_ILDRSTAT_SWRST_LAN1);
	}
	if (status != 0)
		return status;

	msec_delay(50);

	/*
	 * Double resets are required for recovery from certain error
	 * conditions.  Between resets, it is necessary to stall to
	 * allow time for any pending HW events to complete.
	 */
	if (hw->mac.flags & NGBE_FLAGS_DOUBLE_RESET_REQUIRED) {
		hw->mac.flags &= ~NGBE_FLAGS_DOUBLE_RESET_REQUIRED;
		goto mac_reset_top;
	}

	/*
	 * Store the original AUTOC/AUTOC2 values if they have not been
	 * stored off yet.  Otherwise restore the stored original
	 * values since the reset operation sets back to defaults.
	 */
	if (hw->mac.orig_link_settings_stored == false) {
		hw->mac.orig_autoc = hw->mac.autoc_read(hw);
		hw->mac.autoc_write(hw, hw->mac.orig_autoc);
		hw->mac.orig_link_settings_stored = true;
	} else {
		hw->mac.orig_autoc = autoc;
	}

	/* Store the permanent mac address */
	hw->mac.get_mac_addr(hw, hw->mac.perm_addr);

	/*
	 * Store MAC address from RAR0, clear receive address registers, and
	 * clear the multicast table.  Also reset num_rar_entries to 32,
	 * since we modify this value when programming the SAN MAC address.
	 */
	hw->mac.num_rar_entries = 32;
	hw->mac.init_rx_addrs(hw);

	/* Store the permanent SAN mac address */
	hw->mac.get_san_mac_addr(hw, hw->mac.san_addr);

	/* Add the SAN MAC address to the RAR only if it's a valid address */
	if (ngbe_validate_mac_addr(hw->mac.san_addr) == 0) {
		/* Save the SAN MAC RAR index */
		hw->mac.san_mac_rar_index = hw->mac.num_rar_entries - 1;

		hw->mac.set_rar(hw, hw->mac.san_mac_rar_index,
				    hw->mac.san_addr, 0, true);

		/* clear VMDq pool/queue selection for this RAR */
		hw->mac.clear_vmdq(hw, hw->mac.san_mac_rar_index,
				       BIT_MASK32);

		/* Reserve the last RAR for the SAN MAC address */
		hw->mac.num_rar_entries--;
	}

	/* Store the alternative WWNN/WWPN prefix */
	hw->mac.get_wwn_prefix(hw, &hw->mac.wwnn_prefix,
				   &hw->mac.wwpn_prefix);

	return status;
}

/*
 * These defines allow us to quickly generate all of the necessary instructions
 * in the function below by simply calling out NGBE_COMPUTE_SIG_HASH_ITERATION
 * for values 0 through 15
 */
#define NGBE_ATR_COMMON_HASH_KEY \
		(NGBE_ATR_BUCKET_HASH_KEY & NGBE_ATR_SIGNATURE_HASH_KEY)
#define NGBE_COMPUTE_SIG_HASH_ITERATION(_n) \
do { \
	u32 n = (_n); \
	if (NGBE_ATR_COMMON_HASH_KEY & (0x01 << n)) \
		common_hash ^= lo_hash_dword >> n; \
	else if (NGBE_ATR_BUCKET_HASH_KEY & (0x01 << n)) \
		bucket_hash ^= lo_hash_dword >> n; \
	else if (NGBE_ATR_SIGNATURE_HASH_KEY & (0x01 << n)) \
		sig_hash ^= lo_hash_dword << (16 - n); \
	if (NGBE_ATR_COMMON_HASH_KEY & (0x01 << (n + 16))) \
		common_hash ^= hi_hash_dword >> n; \
	else if (NGBE_ATR_BUCKET_HASH_KEY & (0x01 << (n + 16))) \
		bucket_hash ^= hi_hash_dword >> n; \
	else if (NGBE_ATR_SIGNATURE_HASH_KEY & (0x01 << (n + 16))) \
		sig_hash ^= hi_hash_dword << (16 - n); \
} while (0)

/**
 *  ngbe_atr_compute_sig_hash_raptor - Compute the signature hash
 *  @input: input bitstream to compute the hash on
 *  @common: compressed common input dword
 *
 *  This function is almost identical to the function above but contains
 *  several optimizations such as unwinding all of the loops, letting the
 *  compiler work out all of the conditional ifs since the keys are static
 *  defines, and computing two keys at once since the hashed dword stream
 *  will be the same for both keys.
 **/
u32 ngbe_atr_compute_sig_hash_raptor(union ngbe_atr_hash_dword input,
				     union ngbe_atr_hash_dword common)
{
	u32 hi_hash_dword, lo_hash_dword, flow_vm_vlan;
	u32 sig_hash = 0, bucket_hash = 0, common_hash = 0;

	/* record the flow_vm_vlan bits as they are a key part to the hash */
	flow_vm_vlan = be_to_cpu32(input.dword);

	/* generate common hash dword */
	hi_hash_dword = be_to_cpu32(common.dword);

	/* low dword is word swapped version of common */
	lo_hash_dword = (hi_hash_dword >> 16) | (hi_hash_dword << 16);

	/* apply flow ID/VM pool/VLAN ID bits to hash words */
	hi_hash_dword ^= flow_vm_vlan ^ (flow_vm_vlan >> 16);

	/* Process bits 0 and 16 */
	NGBE_COMPUTE_SIG_HASH_ITERATION(0);

	/*
	 * apply flow ID/VM pool/VLAN ID bits to lo hash dword, we had to
	 * delay this because bit 0 of the stream should not be processed
	 * so we do not add the VLAN until after bit 0 was processed
	 */
	lo_hash_dword ^= flow_vm_vlan ^ (flow_vm_vlan << 16);

	/* Process remaining 30 bit of the key */
	NGBE_COMPUTE_SIG_HASH_ITERATION(1);
	NGBE_COMPUTE_SIG_HASH_ITERATION(2);
	NGBE_COMPUTE_SIG_HASH_ITERATION(3);
	NGBE_COMPUTE_SIG_HASH_ITERATION(4);
	NGBE_COMPUTE_SIG_HASH_ITERATION(5);
	NGBE_COMPUTE_SIG_HASH_ITERATION(6);
	NGBE_COMPUTE_SIG_HASH_ITERATION(7);
	NGBE_COMPUTE_SIG_HASH_ITERATION(8);
	NGBE_COMPUTE_SIG_HASH_ITERATION(9);
	NGBE_COMPUTE_SIG_HASH_ITERATION(10);
	NGBE_COMPUTE_SIG_HASH_ITERATION(11);
	NGBE_COMPUTE_SIG_HASH_ITERATION(12);
	NGBE_COMPUTE_SIG_HASH_ITERATION(13);
	NGBE_COMPUTE_SIG_HASH_ITERATION(14);
	NGBE_COMPUTE_SIG_HASH_ITERATION(15);

	/* combine common_hash result with signature and bucket hashes */
	bucket_hash ^= common_hash;
	bucket_hash &= NGBE_ATR_HASH_MASK;

	sig_hash ^= common_hash << 16;
	sig_hash &= NGBE_ATR_HASH_MASK << 16;

	/* return completed signature hash */
	return sig_hash ^ bucket_hash;
}

#define NGBE_COMPUTE_BKT_HASH_ITERATION(_n) \
do { \
	u32 n = (_n); \
	if (NGBE_ATR_BUCKET_HASH_KEY & (0x01 << n)) \
		bucket_hash ^= lo_hash_dword >> n; \
	if (NGBE_ATR_BUCKET_HASH_KEY & (0x01 << (n + 16))) \
		bucket_hash ^= hi_hash_dword >> n; \
} while (0)

/**
 *  ngbe_atr_compute_perfect_hash_raptor - Compute the perfect filter hash
 *  @input: input bitstream to compute the hash on
 *  @input_mask: mask for the input bitstream
 *
 *  This function serves two main purposes.  First it applies the input_mask
 *  to the atr_input resulting in a cleaned up atr_input data stream.
 *  Secondly it computes the hash and stores it in the bkt_hash field at
 *  the end of the input byte stream.  This way it will be available for
 *  future use without needing to recompute the hash.
 **/
void ngbe_atr_compute_perfect_hash_raptor(struct ngbe_atr_input *input,
					  struct ngbe_atr_input *input_mask)
{

	__be32 *dword_stream = (__be32 *)input;
	__be32 *mask_stream = (__be32 *)input_mask;
	u32 hi_hash_dword, lo_hash_dword, flow_vm_vlan;
	u32 bucket_hash = 0;
	u32 hi_dword = 0;
	u32 i = 0;

	/* Apply masks to input data */
	for (i = 0; i < 14; i++)
		dword_stream[i]  &= mask_stream[i];

	/* record the flow_vm_vlan bits as they are a key part to the hash */
	flow_vm_vlan = be_to_cpu32(dword_stream[0]);

	/* generate common hash dword */
	for (i = 1; i <= 13; i++)
		hi_dword ^= dword_stream[i];
	hi_hash_dword = be_to_cpu32(hi_dword);

	/* low dword is word swapped version of common */
	lo_hash_dword = (hi_hash_dword >> 16) | (hi_hash_dword << 16);

	/* apply flow ID/VM pool/VLAN ID bits to hash words */
	hi_hash_dword ^= flow_vm_vlan ^ (flow_vm_vlan >> 16);

	/* Process bits 0 and 16 */
	NGBE_COMPUTE_BKT_HASH_ITERATION(0);

	/*
	 * apply flow ID/VM pool/VLAN ID bits to lo hash dword, we had to
	 * delay this because bit 0 of the stream should not be processed
	 * so we do not add the VLAN until after bit 0 was processed
	 */
	lo_hash_dword ^= flow_vm_vlan ^ (flow_vm_vlan << 16);

	/* Process remaining 30 bit of the key */
	for (i = 1; i <= 15; i++)
		NGBE_COMPUTE_BKT_HASH_ITERATION(i);

	/*
	 * Limit hash to 13 bits since max bucket count is 8K.
	 * Store result at the end of the input stream.
	 */
	input->bkt_hash = bucket_hash & 0x1FFF;
}

/**
 *  ngbe_get_supported_physical_layer_raptor - Returns physical layer type
 *  @hw: pointer to hardware structure
 *
 *  Determines physical layer capabilities of the current configuration.
 **/
u64 ngbe_get_supported_physical_layer_raptor(struct ngbe_hw *hw)
{
	u64 physical_layer = NGBE_PHYSICAL_LAYER_UNKNOWN;
	u64 autoc = hw->mac.autoc_read(hw);
	u64 pma_pmd_10gs = autoc & NGBE_AUTOC_10Gs_PMA_PMD_MASK;
	u64 pma_pmd_10gp = autoc & NGBE_AUTOC_10G_PMA_PMD_MASK;
	u64 pma_pmd_1g = autoc & NGBE_AUTOC_1G_PMA_PMD_MASK;
	u16 ext_ability = 0;

	DEBUGFUNC("ngbe_get_support_physical_layer_raptor");

	hw->phy.identify(hw);

	switch (hw->phy.type) {
	case ngbe_phy_tn:
	case ngbe_phy_cu_unknown:
		hw->phy.read_reg(hw, NGBE_MD_PHY_EXT_ABILITY,
			NGBE_MD_DEV_PMA_PMD, &ext_ability);
		if (ext_ability & NGBE_MD_PHY_10GBASET_ABILITY)
			physical_layer |= NGBE_PHYSICAL_LAYER_10GBASE_T;
		if (ext_ability & NGBE_MD_PHY_1000BASET_ABILITY)
			physical_layer |= NGBE_PHYSICAL_LAYER_1000BASE_T;
		if (ext_ability & NGBE_MD_PHY_100BASETX_ABILITY)
			physical_layer |= NGBE_PHYSICAL_LAYER_100BASE_TX;
		return physical_layer;
	default:
		break;
	}

	switch (autoc & NGBE_AUTOC_LMS_MASK) {
	case NGBE_AUTOC_LMS_1G_AN:
	case NGBE_AUTOC_LMS_1G_LINK_NO_AN:
		if (pma_pmd_1g == NGBE_AUTOC_1G_KX_BX) {
			physical_layer = NGBE_PHYSICAL_LAYER_1000BASE_KX |
			    NGBE_PHYSICAL_LAYER_1000BASE_BX;
		} else
			/* SFI mode so read SFP module */
			goto sfp_check;
		break;
	case NGBE_AUTOC_LMS_10G_LINK_NO_AN:
		if (pma_pmd_10gp == NGBE_AUTOC_10G_CX4)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_CX4;
		else if (pma_pmd_10gp == NGBE_AUTOC_10G_KX4)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_KX4;
		else if (pma_pmd_10gp == NGBE_AUTOC_10G_XAUI)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_XAUI;
		break;
	case NGBE_AUTOC_LMS_10Gs:
		if (pma_pmd_10gs == NGBE_AUTOC_10Gs_KR) {
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_KR;
		} else if (pma_pmd_10gs == NGBE_AUTOC_10Gs_SFI)
			goto sfp_check;
		break;
	case NGBE_AUTOC_LMS_KX4_KX_KR:
	case NGBE_AUTOC_LMS_KX4_KX_KR_1G_AN:
		if (autoc & NGBE_AUTOC_KX_SUPP)
			physical_layer |= NGBE_PHYSICAL_LAYER_1000BASE_KX;
		if (autoc & NGBE_AUTOC_KX4_SUPP)
			physical_layer |= NGBE_PHYSICAL_LAYER_10GBASE_KX4;
		if (autoc & NGBE_AUTOC_KR_SUPP)
			physical_layer |= NGBE_PHYSICAL_LAYER_10GBASE_KR;
		break;
	default:
		break;
	}

	return physical_layer;

sfp_check:
	/* SFP check must be done last since DA modules are sometimes used to
	 * test KR mode -  we need to id KR mode correctly before SFP module.
	 * Call identify_sfp because the pluggable module may have changed */
	return ngbe_get_supported_phy_sfp_layer(hw);
}

/**
 *  ngbe_enable_rx_dma_raptor - Enable the Rx DMA unit
 *  @hw: pointer to hardware structure
 *  @regval: register value to write to RXCTRL
 *
 *  Enables the Rx DMA unit
 **/
s32 ngbe_enable_rx_dma_raptor(struct ngbe_hw *hw, u32 regval)
{

	DEBUGFUNC("ngbe_enable_rx_dma_raptor");

	/*
	 * Workaround silicon errata when enabling the Rx datapath.
	 * If traffic is incoming before we enable the Rx unit, it could hang
	 * the Rx DMA unit.  Therefore, make sure the security engine is
	 * completely disabled prior to enabling the Rx unit.
	 */

	hw->mac.disable_sec_rx_path(hw);

	if (regval & NGBE_PBRXCTL_ENA)
		ngbe_enable_rx(hw);
	else
		ngbe_disable_rx(hw);

	hw->mac.enable_sec_rx_path(hw);

	return 0;
}

/**
 *  ngbe_verify_lesm_fw_enabled_raptor - Checks LESM FW module state.
 *  @hw: pointer to hardware structure
 *
 *  Returns true if the LESM FW module is present and enabled. Otherwise
 *  returns false. Smart Speed must be disabled if LESM FW module is enabled.
 **/
bool ngbe_verify_lesm_fw_enabled_raptor(struct ngbe_hw *hw)
{
	bool lesm_enabled = false;
	u16 fw_offset, fw_lesm_param_offset, fw_lesm_state;
	s32 status;

	DEBUGFUNC("ngbe_verify_lesm_fw_enabled_raptor");

	/* get the offset to the Firmware Module block */
	status = hw->rom.read16(hw, NGBE_FW_PTR, &fw_offset);

	if ((status != 0) ||
	    (fw_offset == 0) || (fw_offset == 0xFFFF))
		goto out;

	/* get the offset to the LESM Parameters block */
	status = hw->rom.read16(hw, (fw_offset +
				     NGBE_FW_LESM_PARAMETERS_PTR),
				     &fw_lesm_param_offset);

	if ((status != 0) ||
	    (fw_lesm_param_offset == 0) || (fw_lesm_param_offset == 0xFFFF))
		goto out;

	/* get the LESM state word */
	status = hw->rom.read16(hw, (fw_lesm_param_offset +
				     NGBE_FW_LESM_STATE_1),
				     &fw_lesm_state);

	if ((status == 0) &&
	    (fw_lesm_state & NGBE_FW_LESM_STATE_ENABLED))
		lesm_enabled = true;

out:
	lesm_enabled = false; //fixme
	return lesm_enabled;
}

/**
 * ngbe_reset_pipeline_raptor - perform pipeline reset
 *
 *  @hw: pointer to hardware structure
 *
 * Reset pipeline by asserting Restart_AN together with LMS change to ensure
 * full pipeline reset.  This function assumes the SW/FW lock is held.
 **/
s32 ngbe_reset_pipeline_raptor(struct ngbe_hw *hw)
{
	s32 err;
	u64 autoc;

	autoc = hw->mac.autoc_read(hw);

	/* Enable link if disabled in NVM */
	if (autoc & NGBE_AUTOC_LINK_DIA_MASK) {
		autoc &= ~NGBE_AUTOC_LINK_DIA_MASK;
	}

	autoc |= NGBE_AUTOC_AN_RESTART;
	/* Write AUTOC register with toggled LMS[2] bit and Restart_AN */
	hw->mac.autoc_write(hw, autoc ^ NGBE_AUTOC_LMS_AN);

	/* Wait for AN to leave state 0 */

	err = 0;

	/* Write AUTOC register with original LMS field and Restart_AN */
	hw->mac.autoc_write(hw, autoc);
	ngbe_flush(hw);

	return err;
}

/* cmd_addr is used for some special command:
 * 1. to be sector address, when implemented erase sector command
 * 2. to be flash address when implemented read, write flash address
 */
u32 ngbe_fmgr_cmd_op(struct ngbe_hw *hw, u32 cmd, u32 cmd_addr)
{
	u32 cmd_val = 0;
	u32 time_out = 0;

	cmd_val = NGBE_SPICMD_CMD(cmd) | NGBE_SPICMD_CLK(3) | cmd_addr;
	wr32(hw, NGBE_SPICMD, cmd_val);
	while (1) {
		if (rd32(hw, NGBE_SPISTAT) & 0x1)
			break;

		if (time_out == 10000)
			return 1;

		time_out = time_out + 1;
		usec_delay(10);
	}

	return 0;
}

u32 ngbe_flash_read_dword(struct ngbe_hw *hw, u32 addr)
{
	u32 status = 0;

	status = ngbe_fmgr_cmd_op(hw, 1, addr);
	if (status)
		return status;

	return rd32(hw, NGBE_SPIDAT);
}

void ngbe_read_efuse(struct ngbe_hw *hw)
{
	u32 efuse[2];
	u8 lan_id = hw->bus.lan_id;

	efuse[0] = ngbe_flash_read_dword(hw , 0xfe010 + lan_id * 8);
	efuse[1] = ngbe_flash_read_dword(hw , 0xfe010 + lan_id * 8 + 4);

	DEBUGOUT("port %d efuse[0] = %08x, efuse[1] = %08x\n",
		lan_id, efuse[0], efuse[1]);

	hw->gphy_efuse[0] = efuse[0];
	hw->gphy_efuse[1] = efuse[1];
}

void ngbe_map_device_id(struct ngbe_hw *hw)
{
	u16 oem = hw->sub_system_id & NGBE_OEM_MASK;

	hw->is_pf = true;

	/* move subsystem_device_id to device_id */
	switch (hw->device_id) {
	case NGBE_DEV_ID_SP_VF:
		hw->sub_device_id = NGBE_SUB_DEV_ID_SP_VF;
		hw->is_pf = false;
		break;
	case NGBE_DEV_ID_SP2:
		hw->device_id = NGBE_DEV_ID_SP;
		switch (hw->sub_system_id) {
		case NGBE_SUB_DEV_ID_SP2_SGMII:
			hw->sub_device_id = NGBE_SUB_DEV_ID_SP_SGMII;
			break;
		}
		break;
	case NGBE_DEV_ID_EM_WX1860AL_W_VF:
	case NGBE_DEV_ID_EM_WX1860A2_VF:
	case NGBE_DEV_ID_EM_WX1860A2S_VF:
	case NGBE_DEV_ID_EM_WX1860A4_VF:
	case NGBE_DEV_ID_EM_WX1860A4S_VF:
	case NGBE_DEV_ID_EM_WX1860AL2_VF:
	case NGBE_DEV_ID_EM_WX1860AL2S_VF:
	case NGBE_DEV_ID_EM_WX1860AL4_VF:
	case NGBE_DEV_ID_EM_WX1860AL4S_VF:
	case NGBE_DEV_ID_EM_WX1860NCSI_VF:
	case NGBE_DEV_ID_EM_WX1860A1_VF:
	case NGBE_DEV_ID_EM_WX1860A1L_VF:
		hw->device_id = NGBE_DEV_ID_EM_VF;
		hw->sub_device_id = NGBE_SUB_DEV_ID_EM_VF;
		hw->is_pf = false;
		break;
	case NGBE_DEV_ID_EM_WX1860AL_W:
	case NGBE_DEV_ID_EM_WX1860A2:
	case NGBE_DEV_ID_EM_WX1860A2S:
	case NGBE_DEV_ID_EM_WX1860A4:
	case NGBE_DEV_ID_EM_WX1860A4S:
	case NGBE_DEV_ID_EM_WX1860AL2:
	case NGBE_DEV_ID_EM_WX1860AL2S:
	case NGBE_DEV_ID_EM_WX1860AL4:
	case NGBE_DEV_ID_EM_WX1860AL4S:
	case NGBE_DEV_ID_EM_WX1860NCSI:
	case NGBE_DEV_ID_EM_WX1860A1:
	case NGBE_DEV_ID_EM_WX1860A1L:
		hw->device_id = NGBE_DEV_ID_EM;
		if (oem == NGBE_M88E1512_SFP || oem == NGBE_LY_M88E1512_SFP)
			hw->sub_device_id = NGBE_SUB_DEV_ID_EM_MVL_SFP;
		else if (oem == NGBE_M88E1512_RJ45 ||
			(hw->sub_system_id == NGBE_SUB_DEV_ID_EM_M88E1512_RJ45))
			hw->sub_device_id = NGBE_SUB_DEV_ID_EM_MVL_RGMII;
		else if (oem == NGBE_M88E1512_MIX)
			hw->sub_device_id = NGBE_SUB_DEV_ID_EM_MVL_MIX;
		else if (oem == NGBE_YT8521S_SFP ||
			 oem == NGBE_YT8521S_SFP_GPIO ||
			 oem == NGBE_LY_YT8521S_SFP)
			hw->sub_device_id = NGBE_SUB_DEV_ID_EM_YT8521S_SFP;
		else if (oem == NGBE_INTERNAL_YT8521S_SFP ||
			 oem == NGBE_INTERNAL_YT8521S_SFP_GPIO)
			hw->sub_device_id = NGBE_SUB_DEV_ID_EM_RTL_YT8521S_SFP;
		else
			hw->sub_device_id = NGBE_SUB_DEV_ID_EM_RTL_SGMII;
		break;
	default:
		break;
	}

	if (oem == NGBE_LY_M88E1512_SFP || oem == NGBE_YT8521S_SFP_GPIO ||
			oem == NGBE_INTERNAL_YT8521S_SFP_GPIO ||
			oem == NGBE_LY_YT8521S_SFP)
		hw->gpio_ctl = true;
}

s32 ngbe_init_mac_ops(struct ngbe_hw *hw)
{
	struct ngbe_mac_info *mac = &hw->mac;

	DEBUGFUNC("ngbe_init_mac_ops");

	/*
	 * enable the laser control functions for SFP+ fiber
	 * and MNG not enabled
	 */
	if (hw->phy.media_type == ngbe_media_type_fiber) {
		mac->disable_tx_laser =
			ngbe_disable_tx_laser_multispeed_fiber;
		mac->enable_tx_laser =
			ngbe_enable_tx_laser_multispeed_fiber;
		mac->flap_tx_laser =
			ngbe_flap_tx_laser_multispeed_fiber;
	}

	if ((hw->phy.media_type == ngbe_media_type_fiber ||
	     hw->phy.media_type == ngbe_media_type_fiber_qsfp) &&
	    hw->phy.multispeed_fiber) {
		/* Set up dual speed SFP+ support */
		mac->setup_link = ngbe_setup_mac_link_multispeed_fiber;
		mac->setup_mac_link = ngbe_setup_mac_link;
		mac->set_rate_select_speed = ngbe_set_hard_rate_select_speed;
	} else if ((hw->phy.media_type == ngbe_media_type_backplane) &&
		    (hw->phy.smart_speed == ngbe_smart_speed_auto ||
		     hw->phy.smart_speed == ngbe_smart_speed_on) &&
		     !ngbe_verify_lesm_fw_enabled_raptor(hw)) {
		mac->setup_link = ngbe_setup_mac_link_smartspeed;
	} else {
		mac->setup_link = ngbe_setup_mac_link;
		if (mac->type == ngbe_mac_em) {
			mac->setup_link = ngbe_setup_mac_link_em;
			mac->setup_fc = ngbe_setup_fc_em;
			mac->reset_hw = ngbe_reset_hw_em;
			mac->check_link = ngbe_check_mac_link_em;
			mac->get_link_capabilities = ngbe_get_link_capabilities_em;
			mac->autoc_read = ngbe_mac_autoc_read_dummy;
			mac->autoc_write = ngbe_mac_autoc_write_dummy;
		}
	}

	return 0;
}

/**
 *  ngbe_init_ops_pf - Inits func ptrs and MAC type
 *  @hw: pointer to hardware structure
 *
 *  Initialize the function pointers and assign the MAC type.
 *  Does not touch the hardware.
 **/
s32 ngbe_init_ops_pf(struct ngbe_hw *hw)
{
	struct ngbe_bus_info *bus = &hw->bus;
	struct ngbe_mac_info *mac = &hw->mac;
	struct ngbe_phy_info *phy = &hw->phy;
	struct ngbe_rom_info *rom = &hw->rom;
	struct ngbe_flash_info *flash = &hw->flash;
	struct ngbe_mbx_info *mbx = &hw->mbx;

	UNREFERENCED_PARAMETER(flash);

	DEBUGFUNC("ngbe_init_ops_pf");

	/* BUS */
	bus->set_lan_id = ngbe_set_lan_id_multi_port;
	
	/* PHY */
	phy->identify = ngbe_identify_phy;
	phy->read_reg = ngbe_read_phy_reg;
	phy->write_reg = ngbe_write_phy_reg;
	phy->read_reg_unlocked = ngbe_read_phy_reg_mdi;
	phy->write_reg_unlocked = ngbe_write_phy_reg_mdi;
	phy->setup_link = ngbe_setup_phy_link;
	phy->get_firmware_version = ngbe_get_phy_firmware_version;
	phy->identify_sfp = ngbe_identify_fiber_module;
	phy->reset_hw = ngbe_reset_phy;
	phy->signal_set = ngbe_phy_signal_set;
	phy->led_oem_chk= ngbe_phy_led_oem_chk;

	/* MAC */
	mac->init_hw = ngbe_init_hw;
	mac->start_hw = ngbe_start_hw;
	mac->clear_hw_cntrs = ngbe_clear_hw_cntrs;
	mac->enable_rx_dma = ngbe_enable_rx_dma_raptor;
	mac->get_mac_addr = ngbe_get_mac_addr;
	mac->stop_hw = ngbe_stop_hw;
	mac->acquire_swfw_sync = ngbe_acquire_swfw_sync;
	mac->release_swfw_sync = ngbe_release_swfw_sync;

	mac->reset_hw = ngbe_reset_hw;
	mac->enable_relaxed_ordering = ngbe_enable_relaxed_ordering_gen2;
	mac->get_supported_physical_layer =
				    ngbe_get_supported_physical_layer_raptor;
	mac->disable_sec_rx_path = ngbe_disable_sec_rx_path;
	mac->enable_sec_rx_path = ngbe_enable_sec_rx_path;
	mac->disable_sec_tx_path = ngbe_disable_sec_tx_path;
	mac->enable_sec_tx_path = ngbe_enable_sec_tx_path;
	mac->get_san_mac_addr = ngbe_get_san_mac_addr;
	mac->set_san_mac_addr = ngbe_set_san_mac_addr;
	mac->get_device_caps = ngbe_get_device_caps;
	mac->get_wwn_prefix = ngbe_get_wwn_prefix;
	mac->get_fcoe_boot_status = ngbe_get_fcoe_boot_status;
	mac->autoc_read = ngbe_autoc_read;
	mac->autoc_write = ngbe_autoc_write;
	mac->prot_autoc_read = ngbe_prot_autoc_read_raptor;
	mac->prot_autoc_write = ngbe_prot_autoc_write_raptor;

	/* LEDs */
	mac->led_on = ngbe_led_on;
	mac->led_off = ngbe_led_off;

	/* RAR, Multicast, VLAN */
	mac->set_rar = ngbe_set_rar;
	mac->clear_rar = ngbe_clear_rar;
	mac->init_rx_addrs = ngbe_init_rx_addrs;
	mac->update_uc_addr_list = ngbe_update_uc_addr_list;
	mac->update_mc_addr_list = ngbe_update_mc_addr_list;
	mac->enable_mc = ngbe_enable_mc;
	mac->disable_mc = ngbe_disable_mc;
	mac->enable_rx = ngbe_enable_rx;
	mac->disable_rx = ngbe_disable_rx;
	mac->set_vmdq = ngbe_set_vmdq;
	mac->set_vmdq_san_mac = ngbe_set_vmdq_san_mac;
	mac->clear_vmdq = ngbe_clear_vmdq;
	mac->insert_mac_addr = ngbe_insert_mac_addr;
	mac->set_vfta = ngbe_set_vfta;
	mac->set_vlvf = ngbe_set_vlvf;
	mac->clear_vfta = ngbe_clear_vfta;
	mac->init_uta_tables = ngbe_init_uta_tables;
	mac->setup_sfp = ngbe_setup_sfp_modules;
	mac->set_mac_anti_spoofing = ngbe_set_mac_anti_spoofing;
	mac->set_vlan_anti_spoofing = ngbe_set_vlan_anti_spoofing;

	/* Flow Control */
	mac->fc_enable = ngbe_fc_enable;
	mac->setup_fc = ngbe_setup_fc;
	mac->fc_autoneg = ngbe_fc_autoneg;

	/* Link */
	mac->get_link_capabilities = ngbe_get_link_capabilities_raptor;
	mac->check_link = ngbe_check_mac_link;
	mac->setup_pba = ngbe_set_pba;

	/* Manageability interface */
	mac->set_fw_drv_ver = ngbe_hic_set_drv_ver;
	mac->get_thermal_sensor_data = ngbe_get_thermal_sensor_data;
	mac->init_thermal_sensor_thresh = ngbe_init_thermal_sensor_thresh;
	mac->check_overtemp = ngbe_mac_check_overtemp;

	mbx->init_params = ngbe_init_mbx_params_pf;
	mbx->read = ngbe_read_mbx_pf;
	mbx->write = ngbe_write_mbx_pf;
	mbx->read_posted = ngbe_read_posted_mbx;
	mbx->write_posted = ngbe_write_posted_mbx;
	mbx->check_for_msg = ngbe_check_for_msg_pf;
	mbx->check_for_ack = ngbe_check_for_ack_pf;
	mbx->check_for_rst = ngbe_check_for_rst_pf;

	/* EEPROM */
	rom->init_params = ngbe_init_eeprom_params;
	rom->read16 = ngbe_ee_read16;
	rom->readw_buffer = ngbe_ee_readw_buffer;
	rom->readw_sw = ngbe_ee_readw_sw;
	rom->read32 = ngbe_ee_read32;
	rom->write16 = ngbe_ee_write16;
	rom->writew_buffer = ngbe_ee_writew_buffer;
	rom->writew_sw = ngbe_ee_writew_sw;
	rom->write32 = ngbe_ee_write32;
	rom->validate_checksum = ngbe_validate_eeprom_checksum;
	rom->update_checksum = ngbe_update_eeprom_checksum;
	rom->calc_checksum = ngbe_calc_eeprom_checksum;

	mac->rar_highwater = 1;
	mac->mcft_size		= NGBE_RAPTOR_MC_TBL_SIZE;
	mac->vft_size		= NGBE_RAPTOR_VFT_TBL_SIZE;
	mac->num_rar_entries	= NGBE_RAPTOR_RAR_ENTRIES;
	mac->rx_pb_size		= NGBE_RAPTOR_RX_PB_SIZE;
	mac->max_rx_queues	= NGBE_RAPTOR_MAX_RX_QUEUES;
	mac->max_tx_queues	= NGBE_RAPTOR_MAX_TX_QUEUES;

	return 0;
}

/**
 *  ngbe_init_shared_code - Initialize the shared code
 *  @hw: pointer to hardware structure
 *
 *  This will assign function pointers and assign the MAC type and PHY code.
 *  Does not touch the hardware. This function must be called prior to any
 *  other function in the shared code. The ngbe_hw structure should be
 *  memset to 0 prior to calling this function.  The following fields in
 *  hw structure should be filled in prior to calling this function:
 *  hw_addr, back, device_id, vendor_id, subsystem_device_id,
 *  subsystem_vendor_id, and revision_id
 **/
s32 ngbe_init_shared_code(struct ngbe_hw *hw)
{
	s32 status = 0;

	DEBUGFUNC("ngbe_init_shared_code");

	/*
	 * Set the mac type
	 */
	ngbe_set_mac_type(hw);
	
	ngbe_init_ops_dummy(hw);
	switch (hw->mac.type) {
	case ngbe_mac_sp:
		break;
	case ngbe_mac_em:
		ngbe_init_ops_pf(hw);
		ngbe_init_mac_ops(hw);
		ngbe_init_pf_em(hw);
		break;
	case ngbe_mac_sp_vf:
	case ngbe_mac_em_vf:
		ngbe_init_ops_vf(hw);
		break;
	default:
		status = NGBE_ERR_DEVICE_NOT_SUPPORTED;
		break;
	}
	hw->mac.max_link_up_time = NGBE_LINK_UP_TIME;

	hw->bus.set_lan_id(hw);

	return status;
}

