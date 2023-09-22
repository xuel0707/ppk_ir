/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "ngbe_hw.h"
#include "ngbe_eeprom.h"
#include "ngbe_mng.h"
#include "ngbe_phy.h"

//STATIC void ngbe_i2c_start(struct ngbe_hw *hw);
//STATIC void ngbe_i2c_stop(struct ngbe_hw *hw);

s32 ngbe_mdi_map_register(mdi_reg_t *reg, mdi_reg_22_t *reg22)
{
	bool match = 1;
	switch (reg->device_type) {
	case NGBE_MD_DEV_PMA_PMD:
		switch (reg->addr) {
		case NGBE_MD_PHY_ID_HIGH:
		case NGBE_MD_PHY_ID_LOW:
			reg22->page = 0;
			reg22->addr = reg->addr;
			reg22->device_type = 0;
			break;
		default:
			match = 0;
		}
		break;
	default:
		match = 0;
		break;
	}

	if (!match) {
		reg22->page = reg->device_type;
		reg22->device_type = reg->device_type;
		reg22->addr = reg->addr;
	}

	return 0;
}

/**
 * ngbe_identify_extphy - Identify a single address for a PHY
 * @hw: pointer to hardware structure
 * @phy_addr: PHY address to probe
 *
 * Returns true if PHY found
 */
static bool ngbe_probe_phy(struct ngbe_hw *hw, u16 phy_addr)
{
	if (!ngbe_validate_phy_addr(hw, phy_addr)) {
		DEBUGOUT("Unable to validate PHY address 0x%04X\n",
			phy_addr);
		return false;
	}

	if (ngbe_get_phy_id(hw))
		return false;

	if (ngbe_get_phy_type_from_id(hw))
		return false;

	if (hw->phy.type == ngbe_phy_unknown) {
		u16 ext_ability = 0;
		hw->phy.read_reg(hw, NGBE_MD_PHY_EXT_ABILITY,
				 NGBE_MD_DEV_PMA_PMD,
				 &ext_ability);

		if (ext_ability & (NGBE_MD_PHY_10GBASET_ABILITY |
			NGBE_MD_PHY_1000BASET_ABILITY))
			hw->phy.type = ngbe_phy_cu_unknown;
		else
			hw->phy.type = ngbe_phy_generic;
	}

	return true;
}

/**
 *  ngbe_read_phy_if - Read NW_MNG_IF_SEL register
 *  @hw: pointer to hardware structure
 *
 *  Read NW_MNG_IF_SEL register and save field values, and check for valid field
 *  values.
 **/
static s32 ngbe_read_mng_if_sel(struct ngbe_hw *hw)
{
	/* Save NW management interface connected on board. This is used
	 * to determine internal PHY mode.
	 */
	hw->phy.nw_mng_if_sel = rd32(hw, NGBE_ETHPHYIF);

	/* If MDIO is connected to external PHY, then set PHY address. */
	if (hw->phy.nw_mng_if_sel & NGBE_ETHPHYIF_MDIO_ACT) {
		hw->phy.addr = NGBE_ETHPHYIF_MDIO_BASE(hw->phy.nw_mng_if_sel);
	}

	/* But, internal ethernet phy don't support mdio yet */
	//if (hw->phy.media_type == ngbe_media_type_fiber) {
	//	hw->phy.nw_mng_if_sel |= NGBE_ETHPHYIF_IO_EPHY;
	//}

	return 0;
}

/**
 *  ngbe_identify_phy - Get physical layer module
 *  @hw: pointer to hardware structure
 *
 *  Determines the physical layer module found on the current adapter.
 **/
s32 ngbe_identify_phy(struct ngbe_hw *hw)
{
	s32 err = NGBE_ERR_PHY_ADDR_INVALID;
	u16 phy_addr;

	DEBUGFUNC("ngbe_identify_phy");

	if (hw->phy.type != ngbe_phy_unknown)
		return 0;

	/* select claus22 */
	wr32(hw, NGBE_MDIOMODE, NGBE_MDIOMODE_MASK);
	
	for (phy_addr = 0; phy_addr < NGBE_MAX_PHY_ADDR; phy_addr++) {
		if (ngbe_probe_phy(hw, phy_addr)) {
			err = 0;
			break;
		}
	}

	/* Certain media types do not have a phy so an address will not
	 * be found and the code will take this path.  Caller has to
	 * decide if it is an error or not.
	 */
	if (err) {
		hw->phy.addr = 0;

		/* Raptor 10GBASE-T requires an external PHY */
		if (hw->phy.media_type == ngbe_media_type_copper) {
			return err;
		}

		err = ngbe_identify_fiber_module(hw); //fixme
	}

	if (hw->phy.type == ngbe_phy_unknown) {
		/* Set PHY type none if no PHY detected */
		hw->phy.type = ngbe_phy_none;
		err = 0;
	} else if (hw->phy.type == ngbe_phy_sfp_unsupported) {
		/* SFP module has been detected but is not supported */
		err = NGBE_ERR_SFP_NOT_SUPPORTED;
	}

	return err;
}

/**
 * ngbe_check_reset_blocked - check status of MNG FW veto bit
 * @hw: pointer to the hardware structure
 *
 * This function checks the MMNGC.MNG_VETO bit to see if there are
 * any constraints on link from manageability.  For MAC's that don't
 * have this bit just return faluse since the link can not be blocked
 * via this method.
 **/
s32 ngbe_check_reset_blocked(struct ngbe_hw *hw)
{
	u32 mmngc;

	DEBUGFUNC("ngbe_check_reset_blocked");

	mmngc = rd32(hw, NGBE_STAT);
	if (mmngc & NGBE_STAT_MNGVETO) {
		DEBUGOUT("MNG_VETO bit detected.\n");
		return true;
	}

	return false;
}

/**
 *  ngbe_validate_phy_addr - Determines phy address is valid
 *  @hw: pointer to hardware structure
 *  @phy_addr: PHY address
 *
 **/
bool ngbe_validate_phy_addr(struct ngbe_hw *hw, u32 phy_addr)
{
	u16 phy_id = 0;
	bool valid = false;

	DEBUGFUNC("ngbe_validate_phy_addr");

	if (hw->sub_device_id == NGBE_SUB_DEV_ID_EM_YT8521S_SFP)
		return 1;

	hw->phy.addr = phy_addr;
	hw->phy.read_reg(hw, NGBE_MD_PHY_ID_HIGH,
			     NGBE_MD_DEV_PMA_PMD, &phy_id);

	if (phy_id != 0xFFFF && phy_id != 0x0)
		valid = true;

	DEBUGOUT("PHY ID HIGH is 0x%04X\n", phy_id);

	return valid;
}

/**
 *  ngbe_get_phy_id - Get the phy type
 *  @hw: pointer to hardware structure
 *
 **/
s32 ngbe_get_phy_id(struct ngbe_hw *hw)
{
	u32 err;
	u16 phy_id_high = 0;
	u16 phy_id_low = 0;

	DEBUGFUNC("ngbe_get_phy_id");

	err = hw->phy.read_reg(hw, NGBE_MD_PHY_ID_HIGH,
				      NGBE_MD_DEV_PMA_PMD,
				      &phy_id_high);	
	hw->phy.id = (u32)(phy_id_high << 16);

	err = hw->phy.read_reg(hw, NGBE_MD_PHY_ID_LOW,
				NGBE_MD_DEV_PMA_PMD,
				&phy_id_low);
	hw->phy.id |= (u32)(phy_id_low & NGBE_PHY_REVISION_MASK);
	hw->phy.revision = (u32)(phy_id_low & ~NGBE_PHY_REVISION_MASK);

	DEBUGOUT("PHY_ID_HIGH 0x%04X, PHY_ID_LOW 0x%04X\n",
		  phy_id_high, phy_id_low);

	return err;
}

/**
 *  ngbe_get_phy_type_from_id - Get the phy type
 *
 **/
s32 ngbe_get_phy_type_from_id(struct ngbe_hw *hw)
{
	s32 status = 0;

	DEBUGFUNC("ngbe_get_phy_type_from_id");

	switch (hw->phy.id) {
	case NGBE_PHYID_MTD3310:
		hw->phy.type = ngbe_phy_cu_mtd;
		break;
	case NGBE_PHYID_RTL:
		hw->phy.type = ngbe_phy_rtl;
		break;
	case NGBE_PHYID_MVL:
		if (hw->phy.media_type == ngbe_media_type_fiber)
			hw->phy.type = ngbe_phy_mvl_sfi;
		else if (hw->phy.media_type == ngbe_media_type_copper)
			hw->phy.type = ngbe_phy_mvl;
		else
			status = ngbe_check_phy_mode_mvl(hw);
		break;
	case NGBE_PHYID_YT8521:
	case NGBE_PHYID_YT8531:
		if (hw->phy.media_type == ngbe_media_type_fiber)
			hw->phy.type = ngbe_phy_yt8521s_sfi;
		else
			hw->phy.type = ngbe_phy_yt8521s;
		break;		
	default:
		hw->phy.type = ngbe_phy_unknown;
		status = NGBE_ERR_DEVICE_NOT_SUPPORTED;
		break;
	}

	return status;
}

static s32
ngbe_reset_extphy(struct ngbe_hw *hw)
{
	u16 ctrl = 0;
	int err, i;

	/*
	 * Perform soft PHY reset to the PHY_XS.
	 * This will cause a soft reset to the PHY
	 */
	err = hw->phy.read_reg(hw, NGBE_MD_PORT_CTRL,
			NGBE_MD_DEV_GENERAL, &ctrl);
	if (err != 0)
		return err;
	ctrl |= NGBE_MD_PORT_CTRL_RESET;
	err = hw->phy.write_reg(hw, NGBE_MD_PORT_CTRL,
			NGBE_MD_DEV_GENERAL, ctrl);
	if (err != 0)
		return err;

	/*
	 * Poll for reset bit to self-clear indicating reset is complete.
	 * Some PHYs could take up to 3 seconds to complete and need about
	 * 1.7 usec delay after the reset is complete.
	 */
	for (i = 0; i < 30; i++) {
		msec_delay(100);
		err = hw->phy.read_reg(hw, NGBE_MD_PORT_CTRL,
			NGBE_MD_DEV_GENERAL, &ctrl);
		if (err != 0)
			return err;

		if (!(ctrl & NGBE_MD_PORT_CTRL_RESET)) {
			usec_delay(2);
			break;
		}
	}

	if (ctrl & NGBE_MD_PORT_CTRL_RESET) {
		err = NGBE_ERR_RESET_FAILED;
		DEBUGOUT("PHY reset polling failed to complete.\n");
	}

	return err;
}

/**
 *  ngbe_reset_phy - Performs a PHY reset
 *  @hw: pointer to hardware structure
 **/
s32 ngbe_reset_phy(struct ngbe_hw *hw)
{
	s32 err = 0;

	DEBUGFUNC("ngbe_reset_phy");

	if (hw->phy.type == ngbe_phy_unknown)
		err = ngbe_identify_phy(hw);

	if (err != 0 || hw->phy.type == ngbe_phy_none)
		return err;

	/* Don't reset PHY if it's shut down due to overtemp. */
	if (NGBE_ERR_OVERTEMP == hw->mac.check_overtemp(hw))
		return err;

	/* Blocked by MNG FW so bail */
	if (ngbe_check_reset_blocked(hw))
		return err;

	/*
	 * Perform soft PHY reset to the PHY_XS.
	 * This will cause a soft reset to the PHY
	 */
	switch (hw->phy.type) {
	case ngbe_phy_rtl:
		err = ngbe_reset_phy_rtl(hw);
		break;
	case ngbe_phy_mvl:
	case ngbe_phy_mvl_sfi:
		err = ngbe_reset_phy_mvl(hw);
		break;
	case ngbe_phy_yt8521s:
	case ngbe_phy_yt8521s_sfi:
		err = ngbe_reset_phy_yt(hw);
		break;
	case ngbe_phy_zte:
		err = ngbe_reset_phy_zte(hw);
		break;
	case ngbe_phy_cu_mtd:
		err = ngbe_reset_extphy(hw);
		break;
	default:
		//fixme
		break;
	}

	return err;
}

/**
 *  ngbe_read_phy_mdi - Reads a value from a specified PHY register without
 *  the SWFW lock
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit address of PHY register to read
 *  @device_type: 5 bit device type
 *  @phy_data: Pointer to read data from PHY register
 **/
s32 ngbe_read_phy_reg_mdi(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			   u16 *phy_data)
{
	u32 command, data;

	/* Setup and write the address cycle command */
	command = NGBE_MDIOSCA_REG(reg_addr) |
		  NGBE_MDIOSCA_DEV(device_type) |
		  NGBE_MDIOSCA_PORT(hw->phy.addr);
	wr32(hw, NGBE_MDIOSCA, command);

	command = NGBE_MDIOSCD_CMD_READ |
		  NGBE_MDIOSCD_BUSY |
		  NGBE_MDIOSCD_CLOCK(6);
	wr32(hw, NGBE_MDIOSCD, command);

	/*
	 * Check every 10 usec to see if the address cycle completed.
	 * The MDI Command bit will clear when the operation is
	 * complete
	 */
	if (!po32m(hw, NGBE_MDIOSCD, NGBE_MDIOSCD_BUSY,
		0, NULL, 100, 100)) {
		DEBUGOUT("PHY address command did not complete\n");
		return NGBE_ERR_PHY;
	}

	data = rd32(hw, NGBE_MDIOSCD);
	*phy_data = (u16)NGBE_MDIOSCD_DAT_R(data);

	return 0;
}

/**
 *  ngbe_read_phy_reg - Reads a value from a specified PHY register
 *  using the SWFW lock - this function is needed in most cases
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit address of PHY register to read
 *  @device_type: 5 bit device type
 *  @phy_data: Pointer to read data from PHY register
 **/
s32 ngbe_read_phy_reg(struct ngbe_hw *hw, u32 reg_addr,
			       u32 device_type, u16 *phy_data)
{
	s32 err;

	DEBUGFUNC("ngbe_read_phy_reg");


	err = hw->phy.read_reg_unlocked(hw, reg_addr, device_type, 
					phy_data);

	return err;
}

/**
 *  ngbe_write_phy_reg_mdi - Writes a value to specified PHY register
 *  without SWFW lock
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit PHY register to write
 *  @device_type: 5 bit device type
 *  @phy_data: Data to write to the PHY register
 **/
s32 ngbe_write_phy_reg_mdi(struct ngbe_hw *hw, u32 reg_addr,
				u32 device_type, u16 phy_data)
{
	u32 command;

	/* write command */
	command = NGBE_MDIOSCA_REG(reg_addr) |
		  NGBE_MDIOSCA_DEV(device_type) |
		  NGBE_MDIOSCA_PORT(hw->phy.addr);
	wr32(hw, NGBE_MDIOSCA, command);

	command = NGBE_MDIOSCD_CMD_WRITE |
		  NGBE_MDIOSCD_DAT(phy_data) |
		  NGBE_MDIOSCD_BUSY | 
		  NGBE_MDIOSCD_CLOCK(6);
	wr32(hw, NGBE_MDIOSCD, command);

	/* wait for completion */
	if (!po32m(hw, NGBE_MDIOSCD, NGBE_MDIOSCD_BUSY,
		0, NULL, 100, 100)) {
		TLOG_DEBUG("PHY write cmd didn't complete\n");
		return -TERR_PHY;
	}

	return 0;
}

/**
 *  ngbe_write_phy_reg - Writes a value to specified PHY register
 *  using SWFW lock- this function is needed in most cases
 *  @hw: pointer to hardware structure
 *  @reg_addr: 32 bit PHY register to write
 *  @device_type: 5 bit device type
 *  @phy_data: Data to write to the PHY register
 **/
s32 ngbe_write_phy_reg(struct ngbe_hw *hw, u32 reg_addr,
				u32 device_type, u16 phy_data)
{
	s32 err;

	DEBUGFUNC("ngbe_write_phy_reg");

	err = hw->phy.write_reg_unlocked(hw, reg_addr, device_type,
					 phy_data);

	return err;
}

/**
 *  ngbe_setup_phy_link - Set and restart auto-neg
 *  @hw: pointer to hardware structure
 *
 *  Restart auto-negotiation and PHY and waits for completion.
 **/
s32 ngbe_setup_phy_link(struct ngbe_hw *hw,
			u32 speed, bool autoneg_wait_to_complete)
{
	s32 err = 0;
	u16 autoneg_reg = NGBE_MII_AUTONEG_REG;

	DEBUGFUNC("ngbe_setup_phy_link");
	/*
	 * Clear autoneg_advertised and set n ew values based on input link
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

	ngbe_get_copper_link_capabilities(hw, &speed, &autoneg_wait_to_complete);

	/* Set or unset auto-negotiation 10G advertisement */
	hw->phy.read_reg(hw, NGBE_MII_10GBASE_T_AUTONEG_CTRL_REG,
			     NGBE_MD_DEV_AUTO_NEG,
			     &autoneg_reg);

	autoneg_reg &= ~NGBE_MII_10GBASE_T_ADVERTISE;
	if ((hw->phy.autoneg_advertised & NGBE_LINK_SPEED_10GB_FULL) &&
	    (speed & NGBE_LINK_SPEED_10GB_FULL))
		autoneg_reg |= NGBE_MII_10GBASE_T_ADVERTISE;

	hw->phy.write_reg(hw, NGBE_MII_10GBASE_T_AUTONEG_CTRL_REG,
			      NGBE_MD_DEV_AUTO_NEG,
			      autoneg_reg);

	hw->phy.read_reg(hw, NGBE_MII_AUTONEG_VENDOR_PROVISION_1_REG,
			     NGBE_MD_DEV_AUTO_NEG,
			     &autoneg_reg);

	/* Set or unset auto-negotiation 5G advertisement */
	autoneg_reg &= ~NGBE_MII_5GBASE_T_ADVERTISE;
	if ((hw->phy.autoneg_advertised & NGBE_LINK_SPEED_5GB_FULL) &&
	    (speed & NGBE_LINK_SPEED_5GB_FULL))
		autoneg_reg |= NGBE_MII_5GBASE_T_ADVERTISE;

	/* Set or unset auto-negotiation 2.5G advertisement */
	autoneg_reg &= ~NGBE_MII_2_5GBASE_T_ADVERTISE;
	if ((hw->phy.autoneg_advertised &
	     NGBE_LINK_SPEED_2_5GB_FULL) &&
	    (speed & NGBE_LINK_SPEED_2_5GB_FULL))
		autoneg_reg |= NGBE_MII_2_5GBASE_T_ADVERTISE;
	/* Set or unset auto-negotiation 1G advertisement */
	autoneg_reg &= ~NGBE_MII_1GBASE_T_ADVERTISE;
	if ((hw->phy.autoneg_advertised & NGBE_LINK_SPEED_1GB_FULL) &&
	    (speed & NGBE_LINK_SPEED_1GB_FULL))
		autoneg_reg |= NGBE_MII_1GBASE_T_ADVERTISE;

	hw->phy.write_reg(hw, NGBE_MII_AUTONEG_VENDOR_PROVISION_1_REG,
			      NGBE_MD_DEV_AUTO_NEG,
			      autoneg_reg);

	/* Set or unset auto-negotiation 100M advertisement */
	hw->phy.read_reg(hw, NGBE_MII_AUTONEG_ADVERTISE_REG,
			     NGBE_MD_DEV_AUTO_NEG,
			     &autoneg_reg);

	autoneg_reg &= ~(NGBE_MII_100BASE_T_ADVERTISE |
			 NGBE_MII_100BASE_T_ADVERTISE_HALF);
	if ((hw->phy.autoneg_advertised & NGBE_LINK_SPEED_100M_FULL) &&
	    (speed & NGBE_LINK_SPEED_100M_FULL))
		autoneg_reg |= NGBE_MII_100BASE_T_ADVERTISE;

	hw->phy.write_reg(hw, NGBE_MII_AUTONEG_ADVERTISE_REG,
			      NGBE_MD_DEV_AUTO_NEG,
			      autoneg_reg);

	/* Blocked by MNG FW so don't reset PHY */
	if (ngbe_check_reset_blocked(hw))
		return err;

	/* Restart PHY auto-negotiation. */
	hw->phy.read_reg(hw, NGBE_MD_AUTO_NEG_CONTROL,
			     NGBE_MD_DEV_AUTO_NEG, &autoneg_reg);

	autoneg_reg |= NGBE_MII_RESTART;

	hw->phy.write_reg(hw, NGBE_MD_AUTO_NEG_CONTROL,
			      NGBE_MD_DEV_AUTO_NEG, autoneg_reg);

	return err;
}

/**
 * ngbe_get_copper_speeds_supported - Get copper link speeds from phy
 * @hw: pointer to hardware structure
 *
 * Determines the supported link capabilities by reading the PHY auto
 * negotiation register.
 **/
static s32 ngbe_get_copper_speeds_supported(struct ngbe_hw *hw)
{
	s32 err;
	u16 speed_ability;

	err = hw->phy.read_reg(hw, NGBE_MD_PHY_SPEED_ABILITY,
				      NGBE_MD_DEV_PMA_PMD,
				      &speed_ability);
	if (err)
		return err;

	if (speed_ability & NGBE_MD_PHY_SPEED_10G)
		hw->phy.speeds_supported |= NGBE_LINK_SPEED_10GB_FULL;
	if (speed_ability & NGBE_MD_PHY_SPEED_1G)
		hw->phy.speeds_supported |= NGBE_LINK_SPEED_1GB_FULL;
	if (speed_ability & NGBE_MD_PHY_SPEED_100M)
		hw->phy.speeds_supported |= NGBE_LINK_SPEED_100M_FULL;
	if (speed_ability & NGBE_MD_PHY_SPEED_10M)
		hw->phy.speeds_supported |= NGBE_LINK_SPEED_10M_FULL;

	return err;
}

/**
 *  ngbe_get_copper_link_capabilities - Determines link capabilities
 *  @hw: pointer to hardware structure
 *  @speed: pointer to link speed
 *  @autoneg: boolean auto-negotiation value
 **/
s32 ngbe_get_copper_link_capabilities(struct ngbe_hw *hw,
					       u32 *speed,
					       bool *autoneg)
{
	s32 err = 0;

	DEBUGFUNC("ngbe_get_copper_link_capabilities");

	*autoneg = true;
	if (!hw->phy.speeds_supported)
		err = ngbe_get_copper_speeds_supported(hw);

	*speed = hw->phy.speeds_supported;
	return err;
}

/**
 *  ngbe_check_phy_link_tnx - Determine link and speed status
 *  @hw: pointer to hardware structure
 *  @speed: current link speed
 *  @link_up: true is link is up, false otherwise
 *
 *  Reads the VS1 register to determine if link is up and the current speed for
 *  the PHY.
 **/
s32 ngbe_check_phy_link_tnx(struct ngbe_hw *hw, u32 *speed,
			     bool *link_up)
{
	s32 err = 0;
	u32 time_out;
	u32 max_time_out = 10;
	u16 phy_link = 0;
	u16 phy_speed = 0;
	u16 phy_data = 0;

	DEBUGFUNC("ngbe_check_phy_link_tnx");

	/* Initialize speed and link to default case */
	*link_up = false;
	*speed = NGBE_LINK_SPEED_10GB_FULL;

	/*
	 * Check current speed and link status of the PHY register.
	 * This is a vendor specific register and may have to
	 * be changed for other copper PHYs.
	 */
	for (time_out = 0; time_out < max_time_out; time_out++) {
		usec_delay(10);
		err = hw->phy.read_reg(hw,
					NGBE_MD_VENDOR_SPECIFIC_1_STATUS,
					NGBE_MD_DEV_VENDOR_1,
					&phy_data);
		phy_link = phy_data & NGBE_MD_VENDOR_SPECIFIC_1_LINK_STATUS;
		phy_speed = phy_data &
				 NGBE_MD_VENDOR_SPECIFIC_1_SPEED_STATUS;
		if (phy_link == NGBE_MD_VENDOR_SPECIFIC_1_LINK_STATUS) {
			*link_up = true;
			if (phy_speed ==
			    NGBE_MD_VENDOR_SPECIFIC_1_SPEED_STATUS)
				*speed = NGBE_LINK_SPEED_1GB_FULL;
			break;
		}
	}

	return err;
}

/**
 *	ngbe_setup_phy_link_tnx - Set and restart auto-neg
 *	@hw: pointer to hardware structure
 *
 *	Restart auto-negotiation and PHY and waits for completion.
 **/
s32 ngbe_setup_phy_link_tnx(struct ngbe_hw *hw,
			u32 speed, bool autoneg_wait_to_complete)
{
	s32 err = 0;
	u16 autoneg_reg = NGBE_MII_AUTONEG_REG;

	DEBUGFUNC("ngbe_setup_phy_link_tnx");

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

	ngbe_get_copper_link_capabilities(hw, &speed, &autoneg_wait_to_complete);

	if (speed & NGBE_LINK_SPEED_10GB_FULL) {
		/* Set or unset auto-negotiation 10G advertisement */
		hw->phy.read_reg(hw, NGBE_MII_10GBASE_T_AUTONEG_CTRL_REG,
				     NGBE_MD_DEV_AUTO_NEG,
				     &autoneg_reg);

		autoneg_reg &= ~NGBE_MII_10GBASE_T_ADVERTISE;
		if (hw->phy.autoneg_advertised & NGBE_LINK_SPEED_10GB_FULL)
			autoneg_reg |= NGBE_MII_10GBASE_T_ADVERTISE;

		hw->phy.write_reg(hw, NGBE_MII_10GBASE_T_AUTONEG_CTRL_REG,
				      NGBE_MD_DEV_AUTO_NEG,
				      autoneg_reg);
	}

	if (speed & NGBE_LINK_SPEED_1GB_FULL) {
		/* Set or unset auto-negotiation 1G advertisement */
		hw->phy.read_reg(hw, NGBE_MII_AUTONEG_XNP_TX_REG,
				     NGBE_MD_DEV_AUTO_NEG,
				     &autoneg_reg);

		autoneg_reg &= ~NGBE_MII_1GBASE_T_ADVERTISE_XNP_TX;
		if (hw->phy.autoneg_advertised & NGBE_LINK_SPEED_1GB_FULL)
			autoneg_reg |= NGBE_MII_1GBASE_T_ADVERTISE_XNP_TX;

		hw->phy.write_reg(hw, NGBE_MII_AUTONEG_XNP_TX_REG,
				      NGBE_MD_DEV_AUTO_NEG,
				      autoneg_reg);
	}

	if (speed & NGBE_LINK_SPEED_100M_FULL) {
		/* Set or unset auto-negotiation 100M advertisement */
		hw->phy.read_reg(hw, NGBE_MII_AUTONEG_ADVERTISE_REG,
				     NGBE_MD_DEV_AUTO_NEG,
				     &autoneg_reg);

		autoneg_reg &= ~NGBE_MII_100BASE_T_ADVERTISE;
		if (hw->phy.autoneg_advertised & NGBE_LINK_SPEED_100M_FULL)
			autoneg_reg |= NGBE_MII_100BASE_T_ADVERTISE;

		hw->phy.write_reg(hw, NGBE_MII_AUTONEG_ADVERTISE_REG,
				      NGBE_MD_DEV_AUTO_NEG,
				      autoneg_reg);
	}

	/* Blocked by MNG FW so don't reset PHY */
	if (ngbe_check_reset_blocked(hw))
		return err;

	/* Restart PHY auto-negotiation. */
	hw->phy.read_reg(hw, NGBE_MD_AUTO_NEG_CONTROL,
			     NGBE_MD_DEV_AUTO_NEG, &autoneg_reg);

	autoneg_reg |= NGBE_MII_RESTART;

	hw->phy.write_reg(hw, NGBE_MD_AUTO_NEG_CONTROL,
			      NGBE_MD_DEV_AUTO_NEG, autoneg_reg);

	return err;
}

/**
 *  ngbe_get_phy_firmware_version_tnx - Gets the PHY Firmware Version
 *  @hw: pointer to hardware structure
 *  @firmware_version: pointer to the PHY Firmware Version
 **/
s32 ngbe_get_phy_firmware_version_tnx(struct ngbe_hw *hw,
				       u32 *firmware_version)
{
	u16 fw_rev_lo;
	s32 err;

	DEBUGFUNC("ngbe_get_phy_firmware_version_tnx");

	err = hw->phy.read_reg(hw, TNX_FW_REV,
				      NGBE_MD_DEV_VENDOR_1,
				      &fw_rev_lo);

	*firmware_version = fw_rev_lo;

	return err;
}

/**
 *  ngbe_get_phy_firmware_version - Gets the PHY Firmware Version
 *  @hw: pointer to hardware structure
 *  @firmware_version: pointer to the PHY Firmware Version
 **/
s32 ngbe_get_phy_firmware_version(struct ngbe_hw *hw,
					   u32 *firmware_version)
{
	u16 fw_rev_lo, fw_rev_hi;
	s32 err;

	DEBUGFUNC("ngbe_get_phy_firmware_version");

	err = hw->phy.read_reg(hw, NGBE_MD_FW_REV_LO,
				      NGBE_MD_DEV_PMA_PMD,
				      &fw_rev_lo);
	if (!err)
		return err;

	err = hw->phy.read_reg(hw, NGBE_MD_FW_REV_HI,
				      NGBE_MD_DEV_PMA_PMD,
				      &fw_rev_hi);
	if (!err)
		return err;

	*firmware_version = ((u32)fw_rev_hi << 16) | fw_rev_lo;
	return 0;
}

/**
 *  ngbe_reset_phy_nl - Performs a PHY reset
 *  @hw: pointer to hardware structure
 **/
s32 ngbe_reset_phy_nl(struct ngbe_hw *hw)
{
	u16 phy_offset, control, eword, edata, block_crc;
	bool end_data = false;
	u16 list_offset, data_offset;
	u16 phy_data = 0;
	s32 ret_val = 0;
	u32 i;

	DEBUGFUNC("ngbe_reset_phy_nl");

	/* Blocked by MNG FW so bail */
	if (ngbe_check_reset_blocked(hw))
		goto out;

	hw->phy.read_reg(hw, NGBE_MD_PHY_XS_CONTROL,
			     NGBE_MD_DEV_PHY_XS, &phy_data);

	/* reset the PHY and poll for completion */
	hw->phy.write_reg(hw, NGBE_MD_PHY_XS_CONTROL,
			      NGBE_MD_DEV_PHY_XS,
			      (phy_data | NGBE_MD_PHY_XS_RESET));

	for (i = 0; i < 100; i++) {
		hw->phy.read_reg(hw, NGBE_MD_PHY_XS_CONTROL,
				     NGBE_MD_DEV_PHY_XS, &phy_data);
		if ((phy_data & NGBE_MD_PHY_XS_RESET) == 0)
			break;
		msec_delay(10);
	}

	if ((phy_data & NGBE_MD_PHY_XS_RESET) != 0) {
		DEBUGOUT("PHY reset did not complete.\n");
		ret_val = NGBE_ERR_PHY;
		goto out;
	}

	/* Get init offsets */
	ret_val = ngbe_get_sfp_init_sequence_offsets(hw, &list_offset,
						      &data_offset);
	if (ret_val != 0)
		goto out;

	ret_val = hw->rom.read16(hw, data_offset, &block_crc);
	data_offset++;
	while (!end_data) {
		/*
		 * Read control word from PHY init contents offset
		 */
		ret_val = hw->rom.read16(hw, data_offset, &eword);
		if (ret_val)
			goto err_eeprom;
		control = (eword & NGBE_EE_CONTROL_MASK_NL) >>
			   NGBE_EE_CONTROL_SHIFT_NL;
		edata = eword & NGBE_EE_DATA_MASK_NL;
		switch (control) {
		case NGBE_EE_DELAY_NL:
			data_offset++;
			DEBUGOUT("DELAY: %d MS\n", edata);
			msec_delay(edata);
			break;
		case NGBE_EE_DATA_NL:
			DEBUGOUT("DATA:\n");
			data_offset++;
			ret_val = hw->rom.read16(hw, data_offset,
						      &phy_offset);
			if (ret_val)
				goto err_eeprom;
			data_offset++;
			for (i = 0; i < edata; i++) {
				ret_val = hw->rom.read16(hw, data_offset,
							      &eword);
				if (ret_val)
					goto err_eeprom;
				hw->phy.write_reg(hw, phy_offset,
					NGBE_MD_DEV_PMA_PMD, eword);
				DEBUGOUT("Wrote %4.4x to %4.4x\n", eword,
					  phy_offset);
				data_offset++;
				phy_offset++;
			}
			break;
		case NGBE_EE_CONTROL_NL:
			data_offset++;
			DEBUGOUT("CONTROL:\n");
			if (edata == NGBE_EE_CONTROL_EOL_NL) {
				DEBUGOUT("EOL\n");
				end_data = true;
			} else if (edata == NGBE_EE_CONTROL_SOL_NL) {
				DEBUGOUT("SOL\n");
			} else {
				DEBUGOUT("Bad control value\n");
				ret_val = NGBE_ERR_PHY;
				goto out;
			}
			break;
		default:
			DEBUGOUT("Bad control type\n");
			ret_val = NGBE_ERR_PHY;
			goto out;
		}
	}

out:
	return ret_val;

err_eeprom:
	DEBUGOUT("eeprom read at offset %d failed", data_offset);
	return NGBE_ERR_PHY;
}

/**
 *  ngbe_identify_fiber_module - Identifies module type
 *  @hw: pointer to hardware structure
 *
 *  Determines HW type and calls appropriate function.
 **/
s32 ngbe_identify_fiber_module(struct ngbe_hw *hw)
{
	s32 err = NGBE_ERR_SFP_NOT_PRESENT;

	DEBUGFUNC("ngbe_identify_fiber_module");

	switch (hw->phy.media_type) {
	case ngbe_media_type_fiber:
		err = ngbe_identify_sfp_module(hw);
		break;

	case ngbe_media_type_fiber_qsfp:
		err = ngbe_identify_qsfp_module(hw);
		break;

	default:
		hw->phy.sfp_type = ngbe_sfp_type_not_present;
		err = NGBE_ERR_SFP_NOT_PRESENT;
		break;
	}

	return err;
}

/**
 *  ngbe_identify_sfp_module - Identifies SFP modules
 *  @hw: pointer to hardware structure
 *
 *  Searches for and identifies the SFP module and assigns appropriate PHY type.
 **/
s32 ngbe_identify_sfp_module(struct ngbe_hw *hw)
{
	s32 err = NGBE_ERR_PHY_ADDR_INVALID;
	u32 vendor_oui = 0;
	enum ngbe_sfp_type stored_sfp_type = hw->phy.sfp_type;
	u8 identifier = 0;
	u8 comp_codes_1g = 0;
	u8 comp_codes_10g = 0;
	u8 oui_bytes[3] = {0, 0, 0};
	u8 cable_tech = 0;
	u8 cable_spec = 0;
	u16 enforce_sfp = 0;

	DEBUGFUNC("ngbe_identify_sfp_module");

	if (hw->phy.media_type != ngbe_media_type_fiber) {
		hw->phy.sfp_type = ngbe_sfp_type_not_present;
		return NGBE_ERR_SFP_NOT_PRESENT;
	}

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_IDENTIFIER,
					     &identifier);
	if (err != 0) {
ERR_I2C:
		hw->phy.sfp_type = ngbe_sfp_type_not_present;
		if (hw->phy.type != ngbe_phy_nl) {
			hw->phy.id = 0;
			hw->phy.type = ngbe_phy_unknown;
		}
		return NGBE_ERR_SFP_NOT_PRESENT;
	}

	if (identifier != NGBE_SFF_IDENTIFIER_SFP) {
		hw->phy.type = ngbe_phy_sfp_unsupported;
		return NGBE_ERR_SFP_NOT_SUPPORTED;
	}

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_1GBE_COMP_CODES,
					     &comp_codes_1g);
	if (err != 0)
		goto ERR_I2C;

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_10GBE_COMP_CODES,
					     &comp_codes_10g);
	if (err != 0)
		goto ERR_I2C;

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_CABLE_TECHNOLOGY,
					     &cable_tech);
	if (err != 0)
		goto ERR_I2C;

	 /* ID Module
	  * =========
	  * 0   SFP_DA_CU
	  * 1   SFP_SR
	  * 2   SFP_LR
	  * 3   SFP_DA_CORE0 - chip-specific
	  * 4   SFP_DA_CORE1 - chip-specific
	  * 5   SFP_SR/LR_CORE0 - chip-specific
	  * 6   SFP_SR/LR_CORE1 - chip-specific
	  * 7   SFP_act_lmt_DA_CORE0 - chip-specific
	  * 8   SFP_act_lmt_DA_CORE1 - chip-specific
	  * 9   SFP_1g_cu_CORE0 - chip-specific
	  * 10  SFP_1g_cu_CORE1 - chip-specific
	  * 11  SFP_1g_sx_CORE0 - chip-specific
	  * 12  SFP_1g_sx_CORE1 - chip-specific
	  */
	if (cable_tech & NGBE_SFF_CABLE_DA_PASSIVE) {
		if (hw->bus.lan_id == 0)
			hw->phy.sfp_type = ngbe_sfp_type_da_cu_core0;
		else
			hw->phy.sfp_type = ngbe_sfp_type_da_cu_core1;
	} else if (cable_tech & NGBE_SFF_CABLE_DA_ACTIVE) {
		err = hw->phy.read_i2c_eeprom(hw,
			NGBE_SFF_CABLE_SPEC_COMP, &cable_spec);
		if (err != 0)
			goto ERR_I2C;
		if (cable_spec & NGBE_SFF_DA_SPEC_ACTIVE_LIMITING) {
			hw->phy.sfp_type = (hw->bus.lan_id == 0
				? ngbe_sfp_type_da_act_lmt_core0
				: ngbe_sfp_type_da_act_lmt_core1);
		} else {
			hw->phy.sfp_type = ngbe_sfp_type_unknown;
		}
	} else if (comp_codes_10g &
		   (NGBE_SFF_10GBASESR_CAPABLE |
		    NGBE_SFF_10GBASELR_CAPABLE)) {
		hw->phy.sfp_type = (hw->bus.lan_id == 0
				? ngbe_sfp_type_srlr_core0
				: ngbe_sfp_type_srlr_core1);
	} else if (comp_codes_1g & NGBE_SFF_1GBASET_CAPABLE) {
		hw->phy.sfp_type = (hw->bus.lan_id == 0
				? ngbe_sfp_type_1g_cu_core0
				: ngbe_sfp_type_1g_cu_core1);
	} else if (comp_codes_1g & NGBE_SFF_1GBASESX_CAPABLE) {
		hw->phy.sfp_type = (hw->bus.lan_id == 0
				? ngbe_sfp_type_1g_sx_core0
				: ngbe_sfp_type_1g_sx_core1);
	} else if (comp_codes_1g & NGBE_SFF_1GBASELX_CAPABLE) {
		hw->phy.sfp_type = (hw->bus.lan_id == 0
				? ngbe_sfp_type_1g_lx_core0
				: ngbe_sfp_type_1g_lx_core1);
	} else {
		hw->phy.sfp_type = ngbe_sfp_type_unknown;
	}

	if (hw->phy.sfp_type != stored_sfp_type)
		hw->phy.sfp_setup_needed = true;

	/* Determine if the SFP+ PHY is dual speed or not. */
	hw->phy.multispeed_fiber = false;
	if (((comp_codes_1g & NGBE_SFF_1GBASESX_CAPABLE) &&
	     (comp_codes_10g & NGBE_SFF_10GBASESR_CAPABLE)) ||
	    ((comp_codes_1g & NGBE_SFF_1GBASELX_CAPABLE) &&
	     (comp_codes_10g & NGBE_SFF_10GBASELR_CAPABLE)))
		hw->phy.multispeed_fiber = true;

	/* Determine PHY vendor */
	if (hw->phy.type != ngbe_phy_nl) {
		hw->phy.id = identifier;
		err = hw->phy.read_i2c_eeprom(hw,
			NGBE_SFF_VENDOR_OUI_BYTE0, &oui_bytes[0]);
		if (err != 0)
			goto ERR_I2C;

		err = hw->phy.read_i2c_eeprom(hw,
			NGBE_SFF_VENDOR_OUI_BYTE1, &oui_bytes[1]);
		if (err != 0)
			goto ERR_I2C;

		err = hw->phy.read_i2c_eeprom(hw,
			NGBE_SFF_VENDOR_OUI_BYTE2, &oui_bytes[2]);
		if (err != 0)
			goto ERR_I2C;

		vendor_oui = ((u32)oui_bytes[0] << 24) |
			     ((u32)oui_bytes[1] << 16) |
			     ((u32)oui_bytes[2] << 8);
		switch (vendor_oui) {
		case NGBE_SFF_VENDOR_OUI_TYCO:
			if (cable_tech & NGBE_SFF_CABLE_DA_PASSIVE)
				hw->phy.type = ngbe_phy_sfp_tyco_passive;
			break;
		case NGBE_SFF_VENDOR_OUI_FTL:
			if (cable_tech & NGBE_SFF_CABLE_DA_ACTIVE)
				hw->phy.type = ngbe_phy_sfp_ftl_active;
			else
				hw->phy.type = ngbe_phy_sfp_ftl;
			break;
		case NGBE_SFF_VENDOR_OUI_AVAGO:
			hw->phy.type = ngbe_phy_sfp_avago;
			break;
		case NGBE_SFF_VENDOR_OUI_INTEL:
			hw->phy.type = ngbe_phy_sfp_intel;
			break;
		default:
			if (cable_tech & NGBE_SFF_CABLE_DA_PASSIVE)
				hw->phy.type = ngbe_phy_sfp_unknown_passive;
			else if (cable_tech & NGBE_SFF_CABLE_DA_ACTIVE)
				hw->phy.type = ngbe_phy_sfp_unknown_active;
			else
				hw->phy.type = ngbe_phy_sfp_unknown;
			break;
		}
	}

	/* Allow any DA cable vendor */
	if (cable_tech & (NGBE_SFF_CABLE_DA_PASSIVE |
			  NGBE_SFF_CABLE_DA_ACTIVE)) {
		return 0;
	}

	/* Verify supported 1G SFP modules */
	if (comp_codes_10g == 0 &&
	    !(hw->phy.sfp_type == ngbe_sfp_type_1g_cu_core1 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_cu_core0 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_lx_core0 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_lx_core1 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_sx_core0 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_sx_core1)) {
		hw->phy.type = ngbe_phy_sfp_unsupported;
		return NGBE_ERR_SFP_NOT_SUPPORTED;
	}

	hw->mac.get_device_caps(hw, &enforce_sfp);
	if (!(enforce_sfp & NGBE_DEVICE_CAPS_ALLOW_ANY_SFP) &&
	    !hw->allow_unsupported_sfp &&
	    !(hw->phy.sfp_type == ngbe_sfp_type_1g_cu_core0 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_cu_core1 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_lx_core0 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_lx_core1 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_sx_core0 ||
	      hw->phy.sfp_type == ngbe_sfp_type_1g_sx_core1)) {
		DEBUGOUT("SFP+ module not supported\n");
		hw->phy.type = ngbe_phy_sfp_unsupported;
		return NGBE_ERR_SFP_NOT_SUPPORTED;
	}

	return err;
}

/**
 *  ngbe_get_supported_phy_sfp_layer - Returns physical layer type
 *  @hw: pointer to hardware structure
 *
 *  Determines physical layer capabilities of the current SFP.
 */
u64 ngbe_get_supported_phy_sfp_layer(struct ngbe_hw *hw)
{
	u64 physical_layer = NGBE_PHYSICAL_LAYER_UNKNOWN;
	u8 comp_codes_10g = 0;
	u8 comp_codes_1g = 0;

	DEBUGFUNC("ngbe_get_supported_phy_sfp_layer");

	hw->phy.identify_sfp(hw);
	if (hw->phy.sfp_type == ngbe_sfp_type_not_present)
		return physical_layer;

	switch (hw->phy.type) {
	case ngbe_phy_sfp_tyco_passive:
	case ngbe_phy_sfp_unknown_passive:
	case ngbe_phy_qsfp_unknown_passive:
		physical_layer = NGBE_PHYSICAL_LAYER_SFP_PLUS_CU;
		break;
	case ngbe_phy_sfp_ftl_active:
	case ngbe_phy_sfp_unknown_active:
	case ngbe_phy_qsfp_unknown_active:
		physical_layer = NGBE_PHYSICAL_LAYER_SFP_ACTIVE_DA;
		break;
	case ngbe_phy_sfp_avago:
	case ngbe_phy_sfp_ftl:
	case ngbe_phy_sfp_intel:
	case ngbe_phy_sfp_unknown:
		hw->phy.read_i2c_eeprom(hw,
		      NGBE_SFF_1GBE_COMP_CODES, &comp_codes_1g);
		hw->phy.read_i2c_eeprom(hw,
		      NGBE_SFF_10GBE_COMP_CODES, &comp_codes_10g);
		if (comp_codes_10g & NGBE_SFF_10GBASESR_CAPABLE)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_SR;
		else if (comp_codes_10g & NGBE_SFF_10GBASELR_CAPABLE)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_LR;
		else if (comp_codes_1g & NGBE_SFF_1GBASET_CAPABLE)
			physical_layer = NGBE_PHYSICAL_LAYER_1000BASE_T;
		else if (comp_codes_1g & NGBE_SFF_1GBASESX_CAPABLE)
			physical_layer = NGBE_PHYSICAL_LAYER_1000BASE_SX;
		break;
	case ngbe_phy_qsfp_intel:
	case ngbe_phy_qsfp_unknown:
		hw->phy.read_i2c_eeprom(hw,
		      NGBE_SFF_QSFP_10GBE_COMP, &comp_codes_10g);
		if (comp_codes_10g & NGBE_SFF_10GBASESR_CAPABLE)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_SR;
		else if (comp_codes_10g & NGBE_SFF_10GBASELR_CAPABLE)
			physical_layer = NGBE_PHYSICAL_LAYER_10GBASE_LR;
		break;
	default:
		break;
	}

	return physical_layer;
}

/**
 *  ngbe_identify_qsfp_module - Identifies QSFP modules
 *  @hw: pointer to hardware structure
 *
 *  Searches for and identifies the QSFP module and assigns appropriate PHY type
 **/
s32 ngbe_identify_qsfp_module(struct ngbe_hw *hw)
{
	s32 err = NGBE_ERR_PHY_ADDR_INVALID;
	u32 vendor_oui = 0;
	enum ngbe_sfp_type stored_sfp_type = hw->phy.sfp_type;
	u8 identifier = 0;
	u8 comp_codes_1g = 0;
	u8 comp_codes_10g = 0;
	u8 oui_bytes[3] = {0, 0, 0};
	u16 enforce_sfp = 0;
	u8 connector = 0;
	u8 cable_length = 0;
	u8 device_tech = 0;
	bool active_cable = false;

	DEBUGFUNC("ngbe_identify_qsfp_module");

	if (hw->phy.media_type != ngbe_media_type_fiber_qsfp) {
		hw->phy.sfp_type = ngbe_sfp_type_not_present;
		err = NGBE_ERR_SFP_NOT_PRESENT;
		goto out;
	}

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_IDENTIFIER,
					     &identifier);
ERR_I2C:
	if (err != 0) {
		hw->phy.sfp_type = ngbe_sfp_type_not_present;
		hw->phy.id = 0;
		hw->phy.type = ngbe_phy_unknown;
		return NGBE_ERR_SFP_NOT_PRESENT;
	}
	if (identifier != NGBE_SFF_IDENTIFIER_QSFP_PLUS) {
		hw->phy.type = ngbe_phy_sfp_unsupported;
		err = NGBE_ERR_SFP_NOT_SUPPORTED;
		goto out;
	}

	hw->phy.id = identifier;

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_QSFP_10GBE_COMP,
					     &comp_codes_10g);

	if (err != 0)
		goto ERR_I2C;

	err = hw->phy.read_i2c_eeprom(hw, NGBE_SFF_QSFP_1GBE_COMP,
					     &comp_codes_1g);

	if (err != 0)
		goto ERR_I2C;

	if (comp_codes_10g & NGBE_SFF_QSFP_DA_PASSIVE_CABLE) {
		hw->phy.type = ngbe_phy_qsfp_unknown_passive;
		if (hw->bus.lan_id == 0)
			hw->phy.sfp_type = ngbe_sfp_type_da_cu_core0;
		else
			hw->phy.sfp_type = ngbe_sfp_type_da_cu_core1;
	} else if (comp_codes_10g & (NGBE_SFF_10GBASESR_CAPABLE |
				     NGBE_SFF_10GBASELR_CAPABLE)) {
		if (hw->bus.lan_id == 0)
			hw->phy.sfp_type = ngbe_sfp_type_srlr_core0;
		else
			hw->phy.sfp_type = ngbe_sfp_type_srlr_core1;
	} else {
		if (comp_codes_10g & NGBE_SFF_QSFP_DA_ACTIVE_CABLE)
			active_cable = true;

		if (!active_cable) {
			/* check for active DA cables that pre-date
			 * SFF-8436 v3.6 */
			hw->phy.read_i2c_eeprom(hw,
					NGBE_SFF_QSFP_CONNECTOR,
					&connector);

			hw->phy.read_i2c_eeprom(hw,
					NGBE_SFF_QSFP_CABLE_LENGTH,
					&cable_length);

			hw->phy.read_i2c_eeprom(hw,
					NGBE_SFF_QSFP_DEVICE_TECH,
					&device_tech);

			if ((connector ==
				     NGBE_SFF_QSFP_CONNECTOR_NOT_SEPARABLE) &&
			    (cable_length > 0) &&
			    ((device_tech >> 4) ==
				     NGBE_SFF_QSFP_TRANSMITER_850NM_VCSEL))
				active_cable = true;
		}

		if (active_cable) {
			hw->phy.type = ngbe_phy_qsfp_unknown_active;
			if (hw->bus.lan_id == 0)
				hw->phy.sfp_type =
					ngbe_sfp_type_da_act_lmt_core0;
			else
				hw->phy.sfp_type =
					ngbe_sfp_type_da_act_lmt_core1;
		} else {
			/* unsupported module type */
			hw->phy.type = ngbe_phy_sfp_unsupported;
			err = NGBE_ERR_SFP_NOT_SUPPORTED;
			goto out;
		}
	}

	if (hw->phy.sfp_type != stored_sfp_type)
		hw->phy.sfp_setup_needed = true;

	/* Determine if the QSFP+ PHY is dual speed or not. */
	hw->phy.multispeed_fiber = false;
	if (((comp_codes_1g & NGBE_SFF_1GBASESX_CAPABLE) &&
	   (comp_codes_10g & NGBE_SFF_10GBASESR_CAPABLE)) ||
	   ((comp_codes_1g & NGBE_SFF_1GBASELX_CAPABLE) &&
	   (comp_codes_10g & NGBE_SFF_10GBASELR_CAPABLE)))
		hw->phy.multispeed_fiber = true;

	/* Determine PHY vendor for optical modules */
	if (comp_codes_10g & (NGBE_SFF_10GBASESR_CAPABLE |
			      NGBE_SFF_10GBASELR_CAPABLE))  {
		err = hw->phy.read_i2c_eeprom(hw,
					    NGBE_SFF_QSFP_VENDOR_OUI_BYTE0,
					    &oui_bytes[0]);

		if (err != 0)
			goto ERR_I2C;

		err = hw->phy.read_i2c_eeprom(hw,
					    NGBE_SFF_QSFP_VENDOR_OUI_BYTE1,
					    &oui_bytes[1]);

		if (err != 0)
			goto ERR_I2C;

		err = hw->phy.read_i2c_eeprom(hw,
					    NGBE_SFF_QSFP_VENDOR_OUI_BYTE2,
					    &oui_bytes[2]);

		if (err != 0)
			goto ERR_I2C;

		vendor_oui =
		  ((oui_bytes[0] << 24) |
		   (oui_bytes[1] << 16) |
		   (oui_bytes[2] << 8));

		if (vendor_oui == NGBE_SFF_VENDOR_OUI_INTEL)
			hw->phy.type = ngbe_phy_qsfp_intel;
		else
			hw->phy.type = ngbe_phy_qsfp_unknown;

		hw->mac.get_device_caps(hw, &enforce_sfp);
		if (!(enforce_sfp & NGBE_DEVICE_CAPS_ALLOW_ANY_SFP)) {
			/* Make sure we're a supported PHY type */
			if (hw->phy.type == ngbe_phy_qsfp_intel) {
				err = 0;
			} else {
				if (hw->allow_unsupported_sfp == true) {
					DEBUGOUT(
						"WARNING: Intel (R) Network Connections are quality tested using Intel (R) Ethernet Optics. "
						"Using untested modules is not supported and may cause unstable operation or damage to the module or the adapter. "
						"Intel Corporation is not responsible for any harm caused by using untested modules.\n");
					err = 0;
				} else {
					DEBUGOUT("QSFP module not supported\n");
					hw->phy.type =
						ngbe_phy_sfp_unsupported;
					err = NGBE_ERR_SFP_NOT_SUPPORTED;
				}
			}
		} else {
			err = 0;
		}
	}

out:
	return err;
}

/**
 *  ngbe_get_sfp_init_sequence_offsets - Provides offset of PHY init sequence
 *  @hw: pointer to hardware structure
 *  @list_offset: offset to the SFP ID list
 *  @data_offset: offset to the SFP data block
 *
 *  Checks the MAC's EEPROM to see if it supports a given SFP+ module type, if
 *  so it returns the offsets to the phy init sequence block.
 **/
s32 ngbe_get_sfp_init_sequence_offsets(struct ngbe_hw *hw,
					u16 *list_offset,
					u16 *data_offset)
{
	u16 sfp_id;
	u16 sfp_type = hw->phy.sfp_type;

	DEBUGFUNC("ngbe_get_sfp_init_sequence_offsets");

	if (hw->phy.sfp_type == ngbe_sfp_type_unknown)
		return NGBE_ERR_SFP_NOT_SUPPORTED;

	if (hw->phy.sfp_type == ngbe_sfp_type_not_present)
		return NGBE_ERR_SFP_NOT_PRESENT;

	/*
	 * Limiting active cables and 1G Phys must be initialized as
	 * SR modules
	 */
	if (sfp_type == ngbe_sfp_type_da_act_lmt_core0 ||
	    sfp_type == ngbe_sfp_type_1g_lx_core0 ||
	    sfp_type == ngbe_sfp_type_1g_cu_core0 ||
	    sfp_type == ngbe_sfp_type_1g_sx_core0)
		sfp_type = ngbe_sfp_type_srlr_core0;
	else if (sfp_type == ngbe_sfp_type_da_act_lmt_core1 ||
		 sfp_type == ngbe_sfp_type_1g_lx_core1 ||
		 sfp_type == ngbe_sfp_type_1g_cu_core1 ||
		 sfp_type == ngbe_sfp_type_1g_sx_core1)
		sfp_type = ngbe_sfp_type_srlr_core1;

	/* Read offset to PHY init contents */
	if (hw->rom.read16(hw, NGBE_EE_PHY_INIT_OFFSET_NL, list_offset)) {
		DEBUGOUT("eeprom read at offset %d failed",
			 NGBE_EE_PHY_INIT_OFFSET_NL);
		return NGBE_ERR_SFP_NO_INIT_SEQ_PRESENT;
	}

	if ((!*list_offset) || (*list_offset == 0xFFFF))
		return NGBE_ERR_SFP_NO_INIT_SEQ_PRESENT;

	/* Shift offset to first ID word */
	(*list_offset)++;

	/*
	 * Find the matching SFP ID in the EEPROM
	 * and program the init sequence
	 */
	if (hw->rom.read16(hw, *list_offset, &sfp_id))
		goto err_phy;

	while (sfp_id != NGBE_EE_PHY_INIT_END_NL) {
		if (sfp_id == sfp_type) {
			(*list_offset)++;
			if (hw->rom.read16(hw, *list_offset, data_offset))
				goto err_phy;
			if ((!*data_offset) || (*data_offset == 0xFFFF)) {
				DEBUGOUT("SFP+ module not supported\n");
				return NGBE_ERR_SFP_NOT_SUPPORTED;
			} else {
				break;
			}
		} else {
			(*list_offset) += 2;
			if (hw->rom.read16(hw, *list_offset, &sfp_id))
				goto err_phy;
		}
	}

	if (sfp_id == NGBE_EE_PHY_INIT_END_NL) {
		DEBUGOUT("No matching SFP+ module found\n");
		return NGBE_ERR_SFP_NOT_SUPPORTED;
	}

	return 0;

err_phy:
	DEBUGOUT("eeprom read at offset %d failed", *list_offset);
	return NGBE_ERR_PHY;
}

/**
 *  ngbe_tn_check_overtemp - Checks if an overtemp occurred.
 *  @hw: pointer to hardware structure
 *
 *  Checks if the LASI temp alarm status was triggered due to overtemp
 **/
s32 ngbe_tn_check_overtemp(struct ngbe_hw *hw)
{

//	s32 err = 0;
	u16 phy_data = 0;

	DEBUGFUNC("ngbe_tn_check_overtemp");

//	if (hw->sub_device_id != NGBE_SUB_DEV_ID_EM_T3_LOM)
//		goto out;

	/* Check that the LASI temp alarm status was triggered */
	hw->phy.read_reg(hw, NGBE_TN_LASI_STATUS_REG,
			     NGBE_MD_DEV_PMA_PMD, &phy_data);

	if (!(phy_data & NGBE_TN_LASI_STATUS_TEMP_ALARM))
		goto out;

//	err = NGBE_ERR_OVERTEMP;
	DEBUGOUT("Device over temperature");
out:
//	return err;
	return 0;  /* do not check */
}

static s32
ngbe_set_sgmii_an37_ability(struct ngbe_hw *hw)
{
	u32 value;

	wr32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1, 0x3002);
	wr32_epcs(hw, SR_MII_MMD_AN_CTL, 0x0105);
	wr32_epcs(hw, SR_MII_MMD_DIGI_CTL, 0x0200);
	value = rd32_epcs(hw, SR_MII_MMD_CTL);
	value = (value & ~0x1200) | (0x1 << 12) | (0x1 << 9);
	wr32_epcs(hw, SR_MII_MMD_CTL, value);
	return 0;
}

static s32
ngbe_set_link_to_kr(struct ngbe_hw *hw, bool autoneg)
{
	u32 i;
	s32 err = 0;

	/* 1. Wait xpcs power-up good */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_STATUS) &
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_MASK) ==
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_POWER_GOOD)
			break;
		msleep(10);
	}
	if (i == 100) {
		err = NGBE_ERR_XPCS_POWER_UP_FAILED;
		goto out;
	}


	if (!autoneg) {
		/* 2. Disable xpcs AN-73 */
		wr32_epcs(hw, SR_AN_CTRL, 0x0);
		//Disable PHY MPLLA for eth mode change(after ECO)
		wr32_ephy(hw, 0x4, 0x243A);
		ngbe_flush(hw);
		msleep(1);
		//Set the eth change_mode bit first in mis_rst register
		//for corresponding LAN port
		wr32(hw, NGBE_RST, NGBE_RST_ETH(hw->bus.lan_id));

		//3. Set VR_XS_PMA_Gen5_12G_MPLLA_CTRL3 Register
		//Bit[10:0](MPLLA_BANDWIDTH) = 11'd123 (default: 11'd16)
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL3,
				NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_10GBASER_KR);

		//4. Set VR_XS_PMA_Gen5_12G_MISC_CTRL0 Register
		//Bit[12:8](RX_VREF_CTRL) = 5'hF (default: 5'h11)
		wr32_epcs(hw, NGBE_PHY_MISC_CTL0,
				0xCF00);

		//5. Set VR_XS_PMA_Gen5_12G_RX_EQ_CTRL0 Register
		//Bit[15:8](VGA1/2_GAIN_0) = 8'h77, Bit[7:5](CTLE_POLE_0) = 3'h2
		//Bit[4:0](CTLE_BOOST_0) = 4'hA
		wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0,
				0x774A);

		//6. Set VR_MII_Gen5_12G_RX_GENCTRL3 Register
		//Bit[2:0](LOS_TRSHLD_0) = 3'h4 (default: 3)
		wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL3,
				0x0004);
		//7. Initialize the mode by setting VR XS or PCS MMD Digital
		//Control1 Register Bit[15](VR_RST)
		wr32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1,
				0xA000);
		/* wait phy initialization done */
		for (i = 0; i < 100; i++) {
			if ((rd32_epcs(hw,
				VR_XS_OR_PCS_MMD_DIGI_CTL1) &
				VR_XS_OR_PCS_MMD_DIGI_CTL1_VR_RST) == 0)
				break;
			msleep(100);
		}
		if (i == 100) {
			err = NGBE_ERR_PHY_INIT_NOT_DONE;
			goto out;
		}
	} else {
		 wr32_epcs(hw, VR_AN_KR_MODE_CL,
				0x1);
	}
out:
	return err;
}

static s32
ngbe_set_link_to_kx4(struct ngbe_hw *hw, bool autoneg)
{
	u32 i;
	s32 err = 0;
	u32 value;

	//check link status, if already set, skip setting it again
	if (hw->link_status == NGBE_LINK_STATUS_KX4) {
		//printk("NGBE_HW link status = %d, skip config kx4\n", hw->link_status);
		goto out;
	}

	/* 1. Wait xpcs power-up good */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_STATUS) &
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_MASK) ==
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_POWER_GOOD)
			break;
		msleep(10);
	}
	if (i == 100) {
		err = NGBE_ERR_XPCS_POWER_UP_FAILED;
		goto out;
	}

	wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_TE,
			~NGBE_MACTXCFG_TE);

	/* 2. Disable xpcs AN-73 */
	if (!autoneg)
		wr32_epcs(hw, SR_AN_CTRL, 0x0);
	else
		wr32_epcs(hw, SR_AN_CTRL, 0x3000);

	//Disable PHY MPLLA for eth mode change(after ECO)
	wr32_ephy(hw, 0x4, 0x250A);
	ngbe_flush(hw);
	msleep(1);

	//Set the eth change_mode bit first in mis_rst register
	//for corresponding LAN port
	wr32(hw, NGBE_RST, NGBE_RST_ETH(hw->bus.lan_id));

	//Set SR PCS Control2 Register Bits[1:0] = 2'b01  //PCS_TYPE_SEL: non KR
	wr32_epcs(hw, SR_XS_PCS_CTRL2,
			SR_PCS_CTRL2_TYPE_SEL_X);
	//Set SR PMA MMD Control1 Register Bit[13] = 1'b1  //SS13: 10G speed
	wr32_epcs(hw, SR_PMA_CTRL1,
			SR_PMA_CTRL1_SS13_KX4);

	//value = rd32_epcs(hw, NGBE_PHY_TX_GENCTRL1);
	value = (0xf5f0 & ~0x7F0) |  (0x5 << 8) | (0x7 << 5) | 0x10;
	wr32_epcs(hw, NGBE_PHY_TX_GENCTRL1, value);

	wr32_epcs(hw, NGBE_PHY_MISC_CTL0, 0x4F00);

	//value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL0);
	value = (0x1804 & ~0x3F3F);
	wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL0, value);

	//value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL1);
	value = (0x50 & ~0x7F) | 40 | (1 << 6);
	wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL1, value);

	for (i = 0; i < 4; i++) {
		//value = rd32_epcs(hw, NGBE_PHY_RX_EQ_CTL0 + i);
		if (i == 0)
			value = (0x45 & ~0xFFFF) | (0x7 << 12) | (0x7 << 8) | 0x6;
		else
			value = (0xff06 & ~0xFFFF) | (0x7 << 12) | (0x7 << 8) | 0x6;
		wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0 + i, value);
	}

	//value = rd32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0);
	value = 0x0 & ~0x7777;
	wr32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0, value);

	wr32_epcs(hw, NGBE_PHY_DFE_TAP_CTL0, 0x0);

	//value = rd32_epcs(hw, NGBE_PHY_RX_GEN_CTL3);
	value = (0x6db & ~0xFFF) | (0x1 << 9) | (0x1 << 6) | (0x1 << 3) | 0x1;
	wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL3, value);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY MPLLA
	//Control 0 Register Bit[7:0] = 8'd40  //MPLLA_MULTIPLIER
	wr32_epcs(hw, NGBE_PHY_MPLLA_CTL0,
			NGBE_PHY_MPLLA_CTL0_MULTIPLIER_OTHER);
	//Set VR XS, PMA or MII Synopsys Enterprise Gen5 12G PHY MPLLA
	//Control 3 Register Bit[10:0] = 11'd86  //MPLLA_BANDWIDTH
	wr32_epcs(hw, NGBE_PHY_MPLLA_CTL3,
			NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_OTHER);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Load 0 Register  Bit[12:0] = 13'd1360  //VCO_LD_VAL_0
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD0,
			NGBE_PHY_VCO_CAL_LD0_OTHER);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Load 1 Register  Bit[12:0] = 13'd1360  //VCO_LD_VAL_1
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD1,
			NGBE_PHY_VCO_CAL_LD0_OTHER);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Load 2 Register  Bit[12:0] = 13'd1360  //VCO_LD_VAL_2
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD2,
			NGBE_PHY_VCO_CAL_LD0_OTHER);
	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Load 3 Register  Bit[12:0] = 13'd1360  //VCO_LD_VAL_3
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD3,
			NGBE_PHY_VCO_CAL_LD0_OTHER);
	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Reference 0 Register Bit[5:0] = 6'd34  //VCO_REF_LD_0/1
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_REF0,
			0x2222);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Reference 1 Register Bit[5:0] = 6'd34  //VCO_REF_LD_2/3
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_REF1,
			0x2222);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY AFE-DFE
	//Enable Register Bit[7:0] = 8'd0  //AFE_EN_0/3_1, DFE_EN_0/3_1
	wr32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE,
			0x0);


	//Set  VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx
	//Equalization Control 4 Register Bit[3:0] = 4'd0  //CONT_ADAPT_0/3_1
	wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL,
			0x00F0);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Tx Rate
	//Control Register Bit[14:12], Bit[10:8], Bit[6:4], Bit[2:0],
	//all rates to 3'b010  //TX0/1/2/3_RATE
	wr32_epcs(hw, NGBE_PHY_TX_RATE_CTL,
			0x2222);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx Rate
	//Control Register Bit[13:12], Bit[9:8], Bit[5:4], Bit[1:0],
	//all rates to 2'b10  //RX0/1/2/3_RATE
	wr32_epcs(hw, NGBE_PHY_RX_RATE_CTL,
			0x2222);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Tx General
	//Control 2 Register Bit[15:8] = 2'b01  //TX0/1/2/3_WIDTH: 10bits
	wr32_epcs(hw, NGBE_PHY_TX_GEN_CTL2,
			0x5500);
	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx General
	//Control 2 Register Bit[15:8] = 2'b01  //RX0/1/2/3_WIDTH: 10bits
	wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL2,
			0x5500);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY MPLLA Control
	//2 Register Bit[10:8] = 3'b010
	//MPLLA_DIV16P5_CLK_EN=0, MPLLA_DIV10_CLK_EN=1, MPLLA_DIV8_CLK_EN=0
	wr32_epcs(hw, NGBE_PHY_MPLLA_CTL2,
			NGBE_PHY_MPLLA_CTL2_DIV_CLK_EN_10);

	wr32_epcs(hw, 0x1f0000, 0x0);
	wr32_epcs(hw, 0x1f8001, 0x0);
	wr32_epcs(hw, SR_MII_MMD_DIGI_CTL, 0x0);

	//10. Initialize the mode by setting VR XS or PCS MMD Digital Control1
	//Register Bit[15](VR_RST)
	wr32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1, 0xA000);
	/* wait phy initialization done */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1) &
			VR_XS_OR_PCS_MMD_DIGI_CTL1_VR_RST) == 0)
			break;
		msleep(100);
	}

	//if success, set link status
	hw->link_status = NGBE_LINK_STATUS_KX4;

	if (i == 100) {
		err = NGBE_ERR_PHY_INIT_NOT_DONE;
		goto out;
	}

out:
	return err;
}

static s32
ngbe_set_link_to_kx(struct ngbe_hw *hw,
			       u32 speed,
			       bool autoneg)
{
	u32 i;
	s32 err = 0;
	u32 wdata = 0;
	u32 value;

	//check link status, if already set, skip setting it again
	if (hw->link_status == NGBE_LINK_STATUS_KX) {
		//printk("NGBE_HW link status = %d, skip configure kx\n", hw->link_status);
		goto out;
	}

	/* 1. Wait xpcs power-up good */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_STATUS) &
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_MASK) ==
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_POWER_GOOD)
			break;
		msleep(10);
	}
	if (i == 100) {
		err = NGBE_ERR_XPCS_POWER_UP_FAILED;
		goto out;
	}

	wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_TE,
				~NGBE_MACTXCFG_TE);

	/* 2. Disable xpcs AN-73 */
	if (!autoneg)
		wr32_epcs(hw, SR_AN_CTRL, 0x0);
	else
		wr32_epcs(hw, SR_AN_CTRL, 0x3000);

	//Disable PHY MPLLA for eth mode change(after ECO)
	wr32_ephy(hw, 0x4, 0x240A);
	ngbe_flush(hw);
	msleep(1);

	//Set the eth change_mode bit first in mis_rst register
	//for corresponding LAN port
	wr32(hw, NGBE_RST, NGBE_RST_ETH(hw->bus.lan_id));

	//Set SR PCS Control2 Register Bits[1:0] = 2'b01  //PCS_TYPE_SEL: non KR
	wr32_epcs(hw, SR_XS_PCS_CTRL2,
			SR_PCS_CTRL2_TYPE_SEL_X);

	//Set SR PMA MMD Control1 Register Bit[13] = 1'b0  //SS13: 1G speed
	wr32_epcs(hw, SR_PMA_CTRL1,
			SR_PMA_CTRL1_SS13_KX);

	//Set SR MII MMD Control Register to corresponding speed: {Bit[6],
	//Bit[13]}=[2'b00,2'b01,2'b10]->[10M,100M,1G]
	if (speed == NGBE_LINK_SPEED_100M_FULL)
		wdata = 0x2100;
	else if (speed == NGBE_LINK_SPEED_1GB_FULL)
		wdata = 0x0140;
	else if (speed == NGBE_LINK_SPEED_10M_FULL)
		wdata = 0x0100;
	wr32_epcs(hw, SR_MII_MMD_CTL,
			wdata);

	//value = rd32_epcs(hw, NGBE_PHY_TX_GENCTRL1);
	value = (0xf5f0 & ~0x710) |  (0x5 << 8);
	wr32_epcs(hw, NGBE_PHY_TX_GENCTRL1, value);

	wr32_epcs(hw, NGBE_PHY_MISC_CTL0, 0x4F00);

	//value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL0);
	value = (0x1804 & ~0x3F3F) | (24 << 8) | 4;
	wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL0, value);

	//value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL1);
	value = (0x50 & ~0x7F) | 16 | (1 << 6);
	wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL1, value);

	for (i = 0; i < 4; i++) {
		if (i) {
			value = 0xff06;
		} else {
			//value = rd32_epcs(hw, NGBE_PHY_RX_EQ_CTL0 + i);
			value = (0x45 & ~0xFFFF) | (0x7 << 12) | (0x7 << 8) | 0x6;
		}
		wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0 + i, value);
	}

	//value = rd32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0);
	value = 0x0 & ~0x7;
	wr32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0, value);

	wr32_epcs(hw, NGBE_PHY_DFE_TAP_CTL0, 0x0);

	//value = rd32_epcs(hw, NGBE_PHY_RX_GEN_CTL3);
	value = (0x6db & ~0x7) | 0x4;
	wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL3, value);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY MPLLA Control
	//0 Register Bit[7:0] = 8'd32  //MPLLA_MULTIPLIER
	wr32_epcs(hw, NGBE_PHY_MPLLA_CTL0,
			NGBE_PHY_MPLLA_CTL0_MULTIPLIER_1GBASEX_KX);

	//Set VR XS, PMA or MII Synopsys Enterprise Gen5 12G PHY MPLLA Control 3
	//Register Bit[10:0] = 11'd70  //MPLLA_BANDWIDTH
	wr32_epcs(hw, NGBE_PHY_MPLLA_CTL3,
			NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_1GBASEX_KX);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Load 0 Register  Bit[12:0] = 13'd1344  //VCO_LD_VAL_0
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD0,
			NGBE_PHY_VCO_CAL_LD0_1GBASEX_KX);

	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD1, 0x549);
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD2, 0x549);
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD3, 0x549);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO
	//Calibration Reference 0 Register Bit[5:0] = 6'd42  //VCO_REF_LD_0
	wr32_epcs(hw, NGBE_PHY_VCO_CAL_REF0,
			NGBE_PHY_VCO_CAL_REF0_LD0_1GBASEX_KX);

	wr32_epcs(hw, NGBE_PHY_VCO_CAL_REF1, 0x2929);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY AFE-DFE Enable
	//Register Bit[4], Bit[0] = 1'b0  //AFE_EN_0, DFE_EN_0
	wr32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE,
			0x0);
	//Set	VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx
	//Equalization Control 4 Register Bit[0] = 1'b0  //CONT_ADAPT_0
	wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL,
			0x0010);
	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Tx Rate
	//Control Register Bit[2:0] = 3'b011  //TX0_RATE
	wr32_epcs(hw, NGBE_PHY_TX_RATE_CTL,
			NGBE_PHY_TX_RATE_CTL_TX0_RATE_1GBASEX_KX);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx Rate
	//Control Register Bit[2:0] = 3'b011 //RX0_RATE
	wr32_epcs(hw, NGBE_PHY_RX_RATE_CTL,
			NGBE_PHY_RX_RATE_CTL_RX0_RATE_1GBASEX_KX);

	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Tx General
	//Control 2 Register Bit[9:8] = 2'b01  //TX0_WIDTH: 10bits
	wr32_epcs(hw, NGBE_PHY_TX_GEN_CTL2,
			NGBE_PHY_TX_GEN_CTL2_TX0_WIDTH_OTHER);
	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx General
	//Control 2 Register Bit[9:8] = 2'b01  //RX0_WIDTH: 10bits
	wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL2,
			NGBE_PHY_RX_GEN_CTL2_RX0_WIDTH_OTHER);
	//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY MPLLA Control
	//2 Register Bit[10:8] = 3'b010	//MPLLA_DIV16P5_CLK_EN=0,
	//MPLLA_DIV10_CLK_EN=1, MPLLA_DIV8_CLK_EN=0
	wr32_epcs(hw, NGBE_PHY_MPLLA_CTL2,
			NGBE_PHY_MPLLA_CTL2_DIV_CLK_EN_10);
	//VR MII MMD AN Control Register Bit[8] = 1'b1 //MII_CTRL
	wr32_epcs(hw, SR_MII_MMD_AN_CTL,
			0x0100);//Set to 8bit MII (required in 10M/100M SGMII)

	//10. Initialize the mode by setting VR XS or PCS MMD Digital Control1
	//Register Bit[15](VR_RST)
	wr32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1, 0xA000);
	/* wait phy initialization done */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1) &
			VR_XS_OR_PCS_MMD_DIGI_CTL1_VR_RST) == 0)
			break;
		msleep(100);
	}

	//if success, set link status
	hw->link_status = NGBE_LINK_STATUS_KX;

	if (i == 100) {
		err = NGBE_ERR_PHY_INIT_NOT_DONE;
		goto out;
	}

out:
	return err;
}

static s32
ngbe_set_link_to_sfi(struct ngbe_hw *hw,
			       u32 speed)
{
	u32 i;
	s32 err = 0;
	u32 value = 0;

	/* Set the module link speed */
	hw->mac.set_rate_select_speed(hw, speed);
	/* 1. Wait xpcs power-up good */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_STATUS) &
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_MASK) ==
			VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_POWER_GOOD)
			break;
		msleep(10);
	}
	if (i == 100) {
		err = NGBE_ERR_XPCS_POWER_UP_FAILED;
		goto out;
	}

	wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_TE,
			~NGBE_MACTXCFG_TE);

	/* 2. Disable xpcs AN-73 */
	wr32_epcs(hw, SR_AN_CTRL, 0x0);

	//Disable PHY MPLLA for eth mode change(after ECO)
	wr32_ephy(hw, 0x4, 0x243A);
	ngbe_flush(hw);
	msleep(1);
	//Set the eth change_mode bit first in mis_rst register
	//for corresponding LAN port
	wr32(hw, NGBE_RST, NGBE_RST_ETH(hw->bus.lan_id));

	if (speed == NGBE_LINK_SPEED_10GB_FULL) {
		//@. Set SR PCS Control2 Register Bits[1:0] = 2'b00  //PCS_TYPE_SEL: KR
		wr32_epcs(hw, SR_XS_PCS_CTRL2, 0);
		value = rd32_epcs(hw, SR_PMA_CTRL1);
		value = value | 0x2000;
		wr32_epcs(hw, SR_PMA_CTRL1, value);
		//@. Set VR_XS_PMA_Gen5_12G_MPLLA_CTRL0 Register Bit[7:0] = 8'd33
		//MPLLA_MULTIPLIER
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL0, 0x0021);
		//3. Set VR_XS_PMA_Gen5_12G_MPLLA_CTRL3 Register
		//Bit[10:0](MPLLA_BANDWIDTH) = 11'd0
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL3, 0);
		value = rd32_epcs(hw, NGBE_PHY_TX_GENCTRL1);
		value = (value & ~0x700) | 0x500;
		wr32_epcs(hw, NGBE_PHY_TX_GENCTRL1, value);
		//4. Set VR_XS_PMA_Gen5_12G_MISC_CTRL0 Register Bit[12:8](RX_VREF_CTRL)
		//= 5'hF
		wr32_epcs(hw, NGBE_PHY_MISC_CTL0, 0xCF00);
		//@. Set VR_XS_PMA_Gen5_12G_VCO_CAL_LD0 Register  Bit[12:0] = 13'd1353
		//VCO_LD_VAL_0
		wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD0, 0x0549);
		//@. Set VR_XS_PMA_Gen5_12G_VCO_CAL_REF0 Register Bit[5:0] = 6'd41
		//VCO_REF_LD_0
		wr32_epcs(hw, NGBE_PHY_VCO_CAL_REF0, 0x0029);
		//@. Set VR_XS_PMA_Gen5_12G_TX_RATE_CTRL Register Bit[2:0] = 3'b000
		//TX0_RATE
		wr32_epcs(hw, NGBE_PHY_TX_RATE_CTL, 0);
		//@. Set VR_XS_PMA_Gen5_12G_RX_RATE_CTRL Register Bit[2:0] = 3'b000
		//RX0_RATE
		wr32_epcs(hw, NGBE_PHY_RX_RATE_CTL, 0);
		//@. Set VR_XS_PMA_Gen5_12G_TX_GENCTRL2 Register Bit[9:8] = 2'b11
		//TX0_WIDTH: 20bits
		wr32_epcs(hw, NGBE_PHY_TX_GEN_CTL2, 0x0300);
		//@. Set VR_XS_PMA_Gen5_12G_RX_GENCTRL2 Register Bit[9:8] = 2'b11
		//RX0_WIDTH: 20bits
		wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL2, 0x0300);
		//@. Set VR_XS_PMA_Gen5_12G_MPLLA_CTRL2 Register Bit[10:8] = 3'b110
		//MPLLA_DIV16P5_CLK_EN=1, MPLLA_DIV10_CLK_EN=1, MPLLA_DIV8_CLK_EN=0
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL2, 0x0600);
		//5. Set VR_XS_PMA_Gen5_12G_TX_EQ_CTRL0 Register Bit[13:8](TX_EQ_MAIN)
		//= 6'd30, Bit[5:0](TX_EQ_PRE) = 6'd4
		value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL0);
		value = (value & ~0x3F3F) | (24 << 8) | 4;
		wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL0, value);
		//6. Set VR_XS_PMA_Gen5_12G_TX_EQ_CTRL1 Register Bit[6](TX_EQ_OVR_RIDE)
		//= 1'b1, Bit[5:0](TX_EQ_POST) = 6'd36
		value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL1);
		value = (value & ~0x7F) | 16 | (1 << 6);
		wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL1, value);
		if (hw->phy.sfp_type == ngbe_sfp_type_da_cu_core0 ||
			hw->phy.sfp_type == ngbe_sfp_type_da_cu_core1) {
			//7. Set VR_XS_PMA_Gen5_12G_RX_EQ_CTRL0 Register
			//Bit[15:8](VGA1/2_GAIN_0) = 8'h77, Bit[7:5]
			//(CTLE_POLE_0) = 3'h2, Bit[4:0](CTLE_BOOST_0) = 4'hF
			wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0, 0x774F);

		} else {
			//7. Set VR_XS_PMA_Gen5_12G_RX_EQ_CTRL0 Register Bit[15:8]
			//(VGA1/2_GAIN_0) = 8'h00, Bit[7:5](CTLE_POLE_0) = 3'h2,
			//Bit[4:0](CTLE_BOOST_0) = 4'hA
			value = rd32_epcs(hw, NGBE_PHY_RX_EQ_CTL0);
			value = (value & ~0xFFFF) | (2 << 5) | 0x05;
			wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0, value);
		}
		value = rd32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0);
		value = (value & ~0x7) | 0x0;
		wr32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0, value);

		if (hw->phy.sfp_type == ngbe_sfp_type_da_cu_core0 ||
			hw->phy.sfp_type == ngbe_sfp_type_da_cu_core1) {
			//8. Set VR_XS_PMA_Gen5_12G_DFE_TAP_CTRL0 Register Bit[7:0](DFE_TAP1_0)
			//= 8'd20
			wr32_epcs(hw, NGBE_PHY_DFE_TAP_CTL0, 0x0014);
			value = rd32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE);
			value = (value & ~0x11) | 0x11;
			wr32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE, value);
		} else {
			//8. Set VR_XS_PMA_Gen5_12G_DFE_TAP_CTRL0 Register Bit[7:0](DFE_TAP1_0)
			//= 8'd20
			wr32_epcs(hw, NGBE_PHY_DFE_TAP_CTL0, 0xBE);
			//9. Set VR_MII_Gen5_12G_AFE_DFE_EN_CTRL Register Bit[4](DFE_EN_0) =
			//1'b0, Bit[0](AFE_EN_0) = 1'b0
			value = rd32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE);
			value = (value & ~0x11) | 0x0;
			wr32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE, value);
		}
		value = rd32_epcs(hw, NGBE_PHY_RX_EQ_CTL);
		value = value & ~0x1;
		wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL, value);
	} else {
		//@. Set SR PCS Control2 Register Bits[1:0] = 2'b00  //PCS_TYPE_SEL: KR
		wr32_epcs(hw, SR_XS_PCS_CTRL2, 0x1);
		//Set SR PMA MMD Control1 Register Bit[13] = 1'b0  //SS13: 1G speed
		wr32_epcs(hw, SR_PMA_CTRL1, 0x0000);
		//Set SR MII MMD Control Register to corresponding speed:
		wr32_epcs(hw, SR_MII_MMD_CTL, 0x0140);

		value = rd32_epcs(hw, NGBE_PHY_TX_GENCTRL1);
		value = (value & ~0x710) | 0x500;
		wr32_epcs(hw, NGBE_PHY_TX_GENCTRL1, value);
		//4. Set VR_XS_PMA_Gen5_12G_MISC_CTRL0 Register Bit[12:8](RX_VREF_CTRL)
		//= 5'hF
		wr32_epcs(hw, NGBE_PHY_MISC_CTL0, 0xCF00);
		//5. Set VR_XS_PMA_Gen5_12G_TX_EQ_CTRL0 Register Bit[13:8](TX_EQ_MAIN)
		//= 6'd30, Bit[5:0](TX_EQ_PRE) = 6'd4
		value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL0);
		value = (value & ~0x3F3F) | (24 << 8) | 4;
		wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL0, value);
		//6. Set VR_XS_PMA_Gen5_12G_TX_EQ_CTRL1 Register Bit[6](TX_EQ_OVR_RIDE)
		//= 1'b1, Bit[5:0](TX_EQ_POST) = 6'd36
		value = rd32_epcs(hw, NGBE_PHY_TX_EQ_CTL1);
		value = (value & ~0x7F) | 16 | (1 << 6);
		wr32_epcs(hw, NGBE_PHY_TX_EQ_CTL1, value);
		if (hw->phy.sfp_type == ngbe_sfp_type_da_cu_core0 ||
			hw->phy.sfp_type == ngbe_sfp_type_da_cu_core1) {
			wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0, 0x774F);
		} else {
			//7. Set VR_XS_PMA_Gen5_12G_RX_EQ_CTRL0 Register Bit[15:8]
			//(VGA1/2_GAIN_0) = 8'h00, Bit[7:5](CTLE_POLE_0) = 3'h2,
			//Bit[4:0](CTLE_BOOST_0) = 4'hA
			value = rd32_epcs(hw, NGBE_PHY_RX_EQ_CTL0);
			value = (value & ~0xFFFF) | 0x7706;
			wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL0, value);
		}
		value = rd32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0);
		value = (value & ~0x7) | 0x0;
		wr32_epcs(hw, NGBE_PHY_RX_EQ_ATT_LVL0, value);
		//8. Set VR_XS_PMA_Gen5_12G_DFE_TAP_CTRL0 Register Bit[7:0](DFE_TAP1_0)
		//= 8'd00
		wr32_epcs(hw, NGBE_PHY_DFE_TAP_CTL0, 0x0);
		//Set VR_XS_PMA_Gen5_12G_RX_GENCTRL3 Register Bit[2:0] LOS_TRSHLD_0 = 4
		value = rd32_epcs(hw, NGBE_PHY_RX_GEN_CTL3);
		value = (value & ~0x7) | 0x4;
		wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL3, value);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY
		//MPLLA Control 0 Register Bit[7:0] = 8'd32  //MPLLA_MULTIPLIER
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL0, 0x0020);
		//Set VR XS, PMA or MII Synopsys Enterprise Gen5 12G PHY MPLLA Control 3 Register Bit[10:0] = 11'd70  //MPLLA_BANDWIDTH
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL3, 0x0046);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO Calibration Load 0 Register  Bit[12:0] = 13'd1344  //VCO_LD_VAL_0
		wr32_epcs(hw, NGBE_PHY_VCO_CAL_LD0, 0x0540);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY VCO Calibration Reference 0 Register Bit[5:0] = 6'd42  //VCO_REF_LD_0
		wr32_epcs(hw, NGBE_PHY_VCO_CAL_REF0, 0x002A);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY AFE-DFE Enable Register Bit[4], Bit[0] = 1'b0  //AFE_EN_0, DFE_EN_0
		wr32_epcs(hw, NGBE_PHY_AFE_DFE_ENABLE, 0x0);
		//Set  VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx Equalization Control 4 Register Bit[0] = 1'b0  //CONT_ADAPT_0
		wr32_epcs(hw, NGBE_PHY_RX_EQ_CTL, 0x0010);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Tx Rate Control Register Bit[2:0] = 3'b011  //TX0_RATE
		wr32_epcs(hw, NGBE_PHY_TX_RATE_CTL, 0x0003);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx Rate Control Register Bit[2:0] = 3'b011 //RX0_RATE
		wr32_epcs(hw, NGBE_PHY_RX_RATE_CTL, 0x0003);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Tx General Control 2 Register Bit[9:8] = 2'b01  //TX0_WIDTH: 10bits
		wr32_epcs(hw, NGBE_PHY_TX_GEN_CTL2, 0x0100);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY Rx General Control 2 Register Bit[9:8] = 2'b01  //RX0_WIDTH: 10bits
		wr32_epcs(hw, NGBE_PHY_RX_GEN_CTL2, 0x0100);
		//Set VR XS, PMA, or MII Synopsys Enterprise Gen5 12G PHY MPLLA Control 2 Register Bit[10:8] = 3'b010  //MPLLA_DIV16P5_CLK_EN=0, MPLLA_DIV10_CLK_EN=1, MPLLA_DIV8_CLK_EN=0
		wr32_epcs(hw, NGBE_PHY_MPLLA_CTL2, 0x0200);
		//VR MII MMD AN Control Register Bit[8] = 1'b1 //MII_CTRL
		wr32_epcs(hw, SR_MII_MMD_AN_CTL, 0x0100);
	}
	//10. Initialize the mode by setting VR XS or PCS MMD Digital Control1
	//Register Bit[15](VR_RST)
	wr32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1, 0xA000);
	/* wait phy initialization done */
	for (i = 0; i < 100; i++) {
		if ((rd32_epcs(hw, VR_XS_OR_PCS_MMD_DIGI_CTL1) &
			VR_XS_OR_PCS_MMD_DIGI_CTL1_VR_RST) == 0)
			break;
		msleep(100);
	}
	if (i == 100) {
		err = NGBE_ERR_PHY_INIT_NOT_DONE;
		goto out;
	}
	/*if (hw->phy.sfp_type == ngbe_sfp_type_da_cu_core0 ||
			hw->phy.sfp_type == ngbe_sfp_type_da_cu_core1) {
		msleep(500);
		err = ngbe_enable_rx_adapter(hw);
	}*/
out:
	return err;
}

/**
 *  ngbe_autoc_read - Hides MAC differences needed for AUTOC read
 *  @hw: pointer to hardware structure
 */
u64 ngbe_autoc_read(struct ngbe_hw *hw)
{
	u64 autoc = 0;
	u32 sr_pcs_ctl;
	u32 sr_pma_ctl1;
	u32 sr_an_ctl;
	u32 sr_an_adv_reg2;

	/* TBC: why depending on pci device id */
	if (hw->phy.multispeed_fiber) {
		autoc |= NGBE_AUTOC_LMS_10Gs;
	} else if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_SFP) {
		autoc |= NGBE_AUTOC_LMS_10Gs |
			 NGBE_AUTOC_10Gs_SFI;
	} else if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_QSFP) {
		autoc = 0; /*TBD*/
	} else if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_XAUI) {
		autoc |= NGBE_AUTOC_LMS_10G_LINK_NO_AN |
			 NGBE_AUTOC_10G_XAUI;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_10GBASE_T;
	} else if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_SGMII) {
		autoc |= NGBE_AUTOC_LMS_SGMII_1G_100M;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_1000BASE_T |
				NGBE_PHYSICAL_LAYER_100BASE_TX;
	}

	if (hw->sub_device_id != NGBE_SUB_DEV_ID_SP_SGMII) {
		return autoc;
	}

	sr_pcs_ctl = rd32_epcs(hw, SR_XS_PCS_CTRL2);
	sr_pma_ctl1 = rd32_epcs(hw, SR_PMA_CTRL1);
	sr_an_ctl = rd32_epcs(hw, SR_AN_CTRL);
	sr_an_adv_reg2 = rd32_epcs(hw, SR_AN_MMD_ADV_REG2);

	if ((sr_pcs_ctl & SR_PCS_CTRL2_TYPE_SEL) == SR_PCS_CTRL2_TYPE_SEL_X &&
	    (sr_pma_ctl1 & SR_PMA_CTRL1_SS13) == SR_PMA_CTRL1_SS13_KX &&
	    (sr_an_ctl & SR_AN_CTRL_AN_EN) == 0) {
		//1G or KX - no backplane auto-negotiation
		autoc |= NGBE_AUTOC_LMS_1G_LINK_NO_AN |
			 NGBE_AUTOC_1G_KX;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_1000BASE_KX;
	} else if ((sr_pcs_ctl & SR_PCS_CTRL2_TYPE_SEL) == SR_PCS_CTRL2_TYPE_SEL_X &&
		(sr_pma_ctl1 & SR_PMA_CTRL1_SS13) == SR_PMA_CTRL1_SS13_KX4 &&
		(sr_an_ctl & SR_AN_CTRL_AN_EN) == 0) {
		autoc |= NGBE_AUTOC_LMS_10Gs |
			 NGBE_AUTOC_10G_KX4;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_10GBASE_KX4;
	} else if ((sr_pcs_ctl & SR_PCS_CTRL2_TYPE_SEL) == SR_PCS_CTRL2_TYPE_SEL_R &&
		(sr_an_ctl & SR_AN_CTRL_AN_EN) == 0) {
		//10 GbE serial link (KR -no backplane auto-negotiation)
		autoc |= NGBE_AUTOC_LMS_10Gs |
			 NGBE_AUTOC_10Gs_KR;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_10GBASE_KR;
	} else if ((sr_an_ctl & SR_AN_CTRL_AN_EN)) {
		//KX/KX4/KR backplane auto-negotiation enable
		if (sr_an_adv_reg2 & SR_AN_MMD_ADV_REG2_BP_TYPE_KR) {
			autoc |= NGBE_AUTOC_10G_KR;
		}
		if (sr_an_adv_reg2 & SR_AN_MMD_ADV_REG2_BP_TYPE_KX4) {
			autoc |= NGBE_AUTOC_10G_KX4;
		}
		if (sr_an_adv_reg2 & SR_AN_MMD_ADV_REG2_BP_TYPE_KX) {
			autoc |= NGBE_AUTOC_1G_KX;
		}
		autoc |= NGBE_AUTOC_LMS_KX4_KX_KR;
		hw->phy.link_mode = NGBE_PHYSICAL_LAYER_10GBASE_KR |
				NGBE_PHYSICAL_LAYER_10GBASE_KX4 |
				NGBE_PHYSICAL_LAYER_1000BASE_KX;
	}

	return autoc;
}

/**
 * ngbe_autoc_write - Hides MAC differences needed for AUTOC write
 * @hw: pointer to hardware structure
 * @autoc: value to write to AUTOC
 */
void ngbe_autoc_write(struct ngbe_hw *hw, u64 autoc)
{
	bool autoneg;
	u32 speed;
	u32 mactxcfg = 0;

	speed = NGBE_AUTOC_SPEED(autoc);
	autoc &= ~NGBE_AUTOC_SPEED_MASK;
	autoneg = (autoc & NGBE_AUTOC_AUTONEG ? true : false);
	autoc &= ~NGBE_AUTOC_AUTONEG;

	if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_KR_KX_KX4) {
		if (!autoneg) {
			switch (hw->phy.link_mode) {
			case NGBE_PHYSICAL_LAYER_10GBASE_KR:
				ngbe_set_link_to_kr(hw, autoneg);
				break;
			case NGBE_PHYSICAL_LAYER_10GBASE_KX4:
				ngbe_set_link_to_kx4(hw, autoneg);
				break;
			case NGBE_PHYSICAL_LAYER_1000BASE_KX:
				ngbe_set_link_to_kx(hw, speed, autoneg);
				break;
			default:
				return;
			}
		} else {
			//ngbe_set_link_to_kr_kx4_kx(hw, speed, autoneg);
		}

	} else if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_XAUI ||
		   hw->sub_device_id == NGBE_SUB_DEV_ID_SP_SGMII) {
		if (speed == NGBE_LINK_SPEED_10GB_FULL) {
			ngbe_set_link_to_kx4(hw, autoneg);
		} else {
			ngbe_set_link_to_kx(hw, speed, 0);
			ngbe_set_sgmii_an37_ability(hw);
		}
	} else if (hw->sub_device_id == NGBE_SUB_DEV_ID_SP_SFP) {
		ngbe_set_link_to_sfi(hw, speed);
	}

	if (speed == NGBE_LINK_SPEED_10GB_FULL) {
		mactxcfg = NGBE_MACTXCFG_SPEED_10G;
	} else if (speed == NGBE_LINK_SPEED_1GB_FULL) {
		mactxcfg = NGBE_MACTXCFG_SPEED_1G;
	}
	/* enable mac transmitter */
	wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_SPEED_MASK, mactxcfg);
}

/**
 *  ngbe_init_phy - PHY/SFP specific init
 *  @hw: pointer to hardware structure
 *
 *  Initialize any function pointers that were not able to be
 *  set during init_shared_code because the PHY/SFP type was
 *  not known.  Perform the SFP init if necessary.
 *
 **/
s32 ngbe_init_phy(struct ngbe_hw *hw)
{
	struct ngbe_phy_info *phy = &hw->phy;
	s32 err = 0;

	DEBUGFUNC("ngbe_init_phy");

	switch (hw->sub_device_id) {
	case NGBE_SUB_DEV_ID_EM_RTL_SGMII:
	case NGBE_SUB_DEV_ID_EM_RTL_YT8521S_SFP:
		hw->phy.read_reg_unlocked = ngbe_read_phy_reg_rtl;
		hw->phy.write_reg_unlocked = ngbe_write_phy_reg_rtl;
		break;
	case NGBE_SUB_DEV_ID_EM_MVL_RGMII:
	case NGBE_SUB_DEV_ID_EM_MVL_SFP:
	case NGBE_SUB_DEV_ID_EM_MVL_MIX:
		hw->phy.read_reg_unlocked = ngbe_read_phy_reg_mvl;
		hw->phy.write_reg_unlocked = ngbe_write_phy_reg_mvl;
		break;
	case NGBE_SUB_DEV_ID_EM_YT8521S_SFP:
		hw->phy.read_reg_unlocked = ngbe_read_phy_reg_yt;
		hw->phy.write_reg_unlocked = ngbe_write_phy_reg_yt;
		break;
	default:
		break;
	}

	ngbe_read_mng_if_sel(hw);
	hw->phy.phy_semaphore_mask = NGBE_MNGSEM_SWPHY;

	/* Identify the PHY or SFP module */
	err = phy->identify(hw);
	if (err == NGBE_ERR_SFP_NOT_SUPPORTED)
		goto init_phy_ops_out;

	/* Set necessary function pointers based on PHY type */
	switch (hw->phy.type) {
	case ngbe_phy_tn:
		phy->setup_link = ngbe_setup_phy_link_tnx;
		phy->check_link = ngbe_check_phy_link_tnx;
		phy->get_firmware_version =
			     ngbe_get_phy_firmware_version_tnx;
		break;
	case ngbe_phy_rtl:
		hw->phy.init_hw = ngbe_init_phy_rtl;
		hw->phy.check_link = ngbe_check_phy_link_rtl;
		hw->phy.setup_link = ngbe_setup_phy_link_rtl;
		break;
	case ngbe_phy_mvl:
	case ngbe_phy_mvl_sfi:
		hw->phy.init_hw = ngbe_init_phy_mvl;
		hw->phy.check_link = ngbe_check_phy_link_mvl;
		hw->phy.setup_link = ngbe_setup_phy_link_mvl;
		break;
	case ngbe_phy_yt8521s:
	case ngbe_phy_yt8521s_sfi:
		hw->phy.init_hw = ngbe_init_phy_yt;
		hw->phy.check_link = ngbe_check_phy_link_yt;
		hw->phy.setup_link = ngbe_setup_phy_link_yt;
	default:
		break;
	}

init_phy_ops_out:
	return err;
}

