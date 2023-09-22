/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_PHY_H_
#define _NGBE_PHY_H_

#include "ngbe_type.h"
#include "ngbe_phy_mtd.h"
#include "ngbe_phy_rtl.h"
#include "ngbe_phy_mvl.h"
#include "ngbe_phy_zte.h"
#include "ngbe_phy_yt.h"

#define NGBE_SFP_DETECT_RETRIES	10
#define NGBE_MD_COMMAND_TIMEOUT	100 /* PHY Timeout for 1 GB mode */

#define NGBE_I2C_SLAVEADDR            (0x50)


/******************************************************************************
 * SFP I2C Registers:
 ******************************************************************************/
/* SFP IDs: format of OUI is 0x[byte0][byte1][byte2][00] */
#define NGBE_SFF_VENDOR_OUI_TYCO	0x00407600
#define NGBE_SFF_VENDOR_OUI_FTL	0x00906500
#define NGBE_SFF_VENDOR_OUI_AVAGO	0x00176A00
#define NGBE_SFF_VENDOR_OUI_INTEL	0x001B2100

/* EEPROM (dev_addr = 0xA0) */
#define NGBE_I2C_EEPROM_DEV_ADDR	0xA0
#define NGBE_SFF_IDENTIFIER		0x00
#define NGBE_SFF_IDENTIFIER_SFP	0x03
#define NGBE_SFF_VENDOR_OUI_BYTE0	0x25
#define NGBE_SFF_VENDOR_OUI_BYTE1	0x26
#define NGBE_SFF_VENDOR_OUI_BYTE2	0x27
#define NGBE_SFF_1GBE_COMP_CODES	0x06
#define NGBE_SFF_10GBE_COMP_CODES	0x03
#define NGBE_SFF_CABLE_TECHNOLOGY	0x08
#define   NGBE_SFF_CABLE_DA_PASSIVE    0x4
#define   NGBE_SFF_CABLE_DA_ACTIVE     0x8
#define NGBE_SFF_CABLE_SPEC_COMP	0x3C
#define NGBE_SFF_SFF_8472_SWAP		0x5C
#define NGBE_SFF_SFF_8472_COMP		0x5E
#define NGBE_SFF_SFF_8472_OSCB		0x6E
#define NGBE_SFF_SFF_8472_ESCB		0x76

#define NGBE_SFF_IDENTIFIER_QSFP_PLUS	0x0D
#define NGBE_SFF_QSFP_VENDOR_OUI_BYTE0	0xA5
#define NGBE_SFF_QSFP_VENDOR_OUI_BYTE1	0xA6
#define NGBE_SFF_QSFP_VENDOR_OUI_BYTE2	0xA7
#define NGBE_SFF_QSFP_CONNECTOR	0x82
#define NGBE_SFF_QSFP_10GBE_COMP	0x83
#define NGBE_SFF_QSFP_1GBE_COMP	0x86
#define NGBE_SFF_QSFP_CABLE_LENGTH	0x92
#define NGBE_SFF_QSFP_DEVICE_TECH	0x93

/* Bitmasks */
#define NGBE_SFF_DA_SPEC_ACTIVE_LIMITING	0x4
#define NGBE_SFF_1GBASESX_CAPABLE	0x1
#define NGBE_SFF_1GBASELX_CAPABLE	0x2
#define NGBE_SFF_1GBASET_CAPABLE	0x8
#define NGBE_SFF_10GBASESR_CAPABLE	0x10
#define NGBE_SFF_10GBASELR_CAPABLE	0x20
#define NGBE_SFF_SOFT_RS_SELECT_MASK	0x8
#define NGBE_SFF_SOFT_RS_SELECT_10G	0x8
#define NGBE_SFF_SOFT_RS_SELECT_1G	0x0
#define NGBE_SFF_ADDRESSING_MODE	0x4
#define NGBE_SFF_QSFP_DA_ACTIVE_CABLE	0x1
#define NGBE_SFF_QSFP_DA_PASSIVE_CABLE	0x8
#define NGBE_SFF_QSFP_CONNECTOR_NOT_SEPARABLE	0x23
#define NGBE_SFF_QSFP_TRANSMITER_850NM_VCSEL	0x0

/* EEPROM for SFF-8472 (dev_addr = 0xA2) */
#define NGBE_I2C_EEPROM_DEV_ADDR2	0xA2
/* sff8472 registers */

/* SFP+ SFF-8472 Compliance */
/*N*/#define NGBE_SFF_SFF_8472_UNSUP	0x00

/******************************************************************************
 * PHY MDIO Registers:
 ******************************************************************************/
#define NGBE_MAX_PHY_ADDR		32

/* (dev_type = all) */
/*N*/#define NGBE_MD_GLOBAL_INT_CHIP_STD_MASK	0xFF00 /* int std mask */
/*N*/#define NGBE_MD_GLOBAL_CHIP_STD_INT_FLAG	0xFC00 /* chip std int flag */
/*N*/#define NGBE_MD_GLOBAL_INT_CHIP_VEN_MASK	0xFF01 /* int chip-wide mask */
/*N*/#define NGBE_MD_GLOBAL_INT_CHIP_VEN_FLAG	0xFC01 /* int chip-wide mask */
/*N*/#define NGBE_MD_GLOBAL_ALARM_1		0xCC00 /* Global alarm 1 */
/*N*/#define NGBE_MD_GLOBAL_ALM_1_DEV_FAULT	0x0010 /* device fault */
/*N*/#define NGBE_MD_GLOBAL_ALM_1_HI_TMP_FAIL	0x4000 /* high temp failure */
/*N*/#define NGBE_MD_GLOBAL_FAULT_MSG	0xC850 /* Global Fault Message */
/*N*/#define NGBE_MD_GLOBAL_FAULT_MSG_HI_TMP	0x8007 /* high temp failure */
/*N*/#define NGBE_MD_GLOBAL_INT_MASK		0xD400 /* Global int mask */
/*N*/#define NGBE_MD_GLOBAL_AN_VEN_ALM_INT_EN	0x1000 /* autoneg vendor alarm int enable */
/*N*/#define NGBE_MD_GLOBAL_ALARM_1_INT		0x4 /* int in Global alarm 1 */
/*N*/#define NGBE_MD_GLOBAL_VEN_ALM_INT_EN	0x1 /* vendor alarm int enable */
/*N*/#define NGBE_MD_GLOBAL_STD_ALM2_INT		0x200 /* vendor alarm2 int mask */
/*N*/#define NGBE_MD_GLOBAL_INT_HI_TEMP_EN	0x4000 /* int high temp enable */
/*N*/#define NGBE_MD_GLOBAL_INT_DEV_FAULT_EN 0x0010 /* int dev fault enable */

/* (dev_type = 0) */
#define NGBE_MD_DEV_ZERO		0x0
/*N*/#define NGBE_CS4227			0xBE	/* CS4227 address */
/*N*/#define   NGBE_CS4227_GLOBAL_ID_LSB	0
/*N*/#define   NGBE_CS4227_GLOBAL_ID_MSB	1
/*N*/#define   NGBE_CS4227_SCRATCH		2
/*N*/#define   NGBE_CS4227_GLOBAL_ID_VALUE	0x03E5
/*N*/#define   NGBE_CS4227_EFUSE_PDF_SKU	0x19F
/*N*/#define   NGBE_CS4223_SKU_ID		0x0010	/* Quad port */
/*N*/#define   NGBE_CS4227_SKU_ID		0x0014	/* Dual port */
/*N*/#define   NGBE_CS4227_RESET_PENDING	0x1357
/*N*/#define   NGBE_CS4227_RESET_COMPLETE	0x5AA5
/*N*/#define   NGBE_CS4227_RETRIES		15
/*N*/#define   NGBE_CS4227_EFUSE_STATUS	0x0181
/*N*/#define   NGBE_CS4227_LINE_SPARE22_MSB	0x12AD	/* Reg to program speed */
/*N*/#define   NGBE_CS4227_LINE_SPARE24_LSB	0x12B0	/* Reg to program EDC */
/*N*/#define   NGBE_CS4227_HOST_SPARE22_MSB	0x1AAD	/* Reg to program speed */
/*N*/#define   NGBE_CS4227_HOST_SPARE24_LSB	0x1AB0	/* Reg to program EDC */
/*N*/#define   NGBE_CS4227_EEPROM_STATUS	0x5001
/*N*/#define   NGBE_CS4227_EEPROM_LOAD_OK	0x0001
/*N*/#define   NGBE_CS4227_SPEED_1G		0x8000
/*N*/#define   NGBE_CS4227_SPEED_10G		0
/*N*/#define   NGBE_CS4227_EDC_MODE_CX1	0x0002
/*N*/#define   NGBE_CS4227_EDC_MODE_SR	0x0004
/*N*/#define   NGBE_CS4227_EDC_MODE_DIAG	0x0008
/*N*/#define   NGBE_CS4227_RESET_HOLD		500	/* microseconds */
/*N*/#define   NGBE_CS4227_RESET_DELAY	450	/* milliseconds */
/*N*/#define   NGBE_CS4227_CHECK_DELAY	30	/* milliseconds */
/*N*/#define   NGBE_PE			0xE0	/* Port expander address */
/*N*/#define   NGBE_PE_OUTPUT			1	/* Output register offset */
/*N*/#define   NGBE_PE_CONFIG			3	/* Config register offset */
/*N*/#define   NGBE_PE_BIT1			(1 << 1)

/* (dev_type = 1) */
#define NGBE_MD_DEV_PMA_PMD		0x1
/*N*/#define NGBE_AUTO_NEG_10GBASE_EEE_ADVT	0x8  /* AUTO NEG EEE 10GBaseT Advt */
/*N*/#define NGBE_AUTO_NEG_1000BASE_EEE_ADVT 0x4  /* AUTO NEG EEE 1000BaseT Advt */
/*N*/#define NGBE_AUTO_NEG_100BASE_EEE_ADVT	0x2  /* AUTO NEG EEE 100BaseT Advt */
#define NGBE_MD_PHY_ID_HIGH		0x2 /* PHY ID High Reg*/
#define NGBE_MD_PHY_ID_LOW		0x3 /* PHY ID Low Reg*/
#define   NGBE_PHY_REVISION_MASK		0xFFFFFFF0
#define NGBE_MD_PHY_SPEED_ABILITY	0x4 /* Speed Ability Reg */
/*N*/#define NGBE_MD_PHY_SPEED_10G	0x0001 /* 10G capable */
/*N*/#define NGBE_MD_PHY_SPEED_1G	0x0010 /* 1G capable */
/*N*/#define NGBE_MD_PHY_SPEED_100M	0x0020 /* 100M capable */
/*N*/#define NGBE_MD_PHY_SPEED_10M	0x0040 /* 10M capable */

#define NGBE_MD_PHY_EXT_ABILITY	0xB /* Ext Ability Reg */
/*N*/#define NGBE_MD_PHY_10GBASET_ABILITY		0x0004 /* 10GBaseT capable */
/*N*/#define NGBE_MD_PHY_1000BASET_ABILITY	0x0020 /* 1000BaseT capable */
/*N*/#define NGBE_MD_PHY_100BASETX_ABILITY	0x0080 /* 100BaseTX capable */
/*N*/#define NGBE_MD_PHY_SET_LOW_POWER_MODE	0x0800 /* Set low power mode */
/*N*/#define NGBE_AUTO_NEG_LP_STATUS	0xE820 /* AUTO NEG Rx LP Status Reg */
/*N*/#define NGBE_AUTO_NEG_LP_1000BASE_CAP	0x8000 /* AUTO NEG Rx LP 1000BaseT Cap */
/*N*/#define NGBE_AUTO_NEG_LP_10GBASE_CAP	0x0800 /* AUTO NEG Rx LP 10GBaseT Cap */
/*N*/#define NGBE_AUTO_NEG_10GBASET_STAT	0x0021 /* AUTO NEG 10G BaseT Stat */

#define NGBE_MD_TX_VENDOR_ALARMS_3		0xCC02 /* Vendor Alarms 3 Reg */
/*N*/#define NGBE_MD_TX_VENDOR_ALARMS_3_RST_MASK	0x3 /* PHY Reset Complete Mask */
/*N*/#define NGBE_MD_GLOBAL_RES_PR_10 0xC479 /* Global Resv Provisioning 10 Reg */
/*N*/#define NGBE_MD_POWER_UP_STALL		0x8000 /* Power Up Stall */
/*N*/#define NGBE_MD_PMA_PMD_CONTROL_ADDR	0x0000 /* PMA/PMD Control Reg */
#define NGBE_MD_PMA_PMD_SDA_SCL_ADDR	0xC30A /* PHY_XS SDA/SCL Addr Reg */
#define NGBE_MD_PMA_PMD_SDA_SCL_DATA	0xC30B /* PHY_XS SDA/SCL Data Reg */
#define NGBE_MD_PMA_PMD_SDA_SCL_STAT	0xC30C /* PHY_XS SDA/SCL Status Reg */

#define NGBE_MD_FW_REV_LO            0xC011
#define NGBE_MD_FW_REV_HI            0xC012

/*N*/#define NGBE_TN_LASI_STATUS_REG	0x9005
/*N*/#define NGBE_TN_LASI_STATUS_TEMP_ALARM	0x0008

/* (dev_type = 3) */
#define NGBE_MD_DEV_PCS         0x3
#define NGBE_PCRC8ECL		0x0E810 /* PCR CRC-8 Error Count Lo */
#define NGBE_PCRC8ECH		0x0E811 /* PCR CRC-8 Error Count Hi */
#define   NGBE_PCRC8ECH_MASK	0x1F
#define NGBE_LDPCECL		0x0E820 /* PCR Uncorrected Error Count Lo */
#define NGBE_LDPCECH		0x0E821 /* PCR Uncorrected Error Count Hi */

/* (dev_type = 0x4) */
#define NGBE_MD_DEV_PHY_XS		0x4
/*N*/#define NGBE_MD_PHY_XS_CONTROL	0x0 /* PHY_XS Control Reg */
/*N*/#define NGBE_MD_PHY_XS_RESET		0x8000 /* PHY_XS Reset */

/* (dev_type = 7) */
#define NGBE_MD_DEV_AUTO_NEG		0x7

/*N*/#define NGBE_MD_AUTO_NEG_CONTROL	0x0 /* AUTO_NEG Control Reg */
/*N*/#define NGBE_MD_AUTO_NEG_STATUS	0x1 /* AUTO_NEG Status Reg */
/*N*/#define NGBE_MD_AUTO_NEG_VENDOR_STAT	0xC800 /* AUTO_NEG Vendor Status Reg */
/*N*/#define NGBE_MD_AUTO_NEG_VENDOR_TX_ALARM 0xCC00 /* AUTO_NEG Vendor TX Reg */
/*N*/#define NGBE_MD_AUTO_NEG_VENDOR_TX_ALARM2 0xCC01 /* AUTO_NEG Vendor Tx Reg */
/*N*/#define NGBE_MD_AUTO_NEG_VEN_LSC	0x1 /* AUTO_NEG Vendor Tx LSC */
#define NGBE_MD_AUTO_NEG_ADVT	0x10 /* AUTO_NEG Advt Reg */
#define   NGBE_TAF_SYM_PAUSE		MS16(10, 0x3)
#define   NGBE_TAF_ASM_PAUSE		MS16(11, 0x3)

/*N*/#define NGBE_MD_AUTO_NEG_LP		0x13 /* AUTO_NEG LP Status Reg */
/*N*/#define NGBE_MD_AUTO_NEG_EEE_ADVT	0x3C /* AUTO_NEG EEE Advt Reg */
/* PHY address definitions for new protocol MDIO commands */
/*N*/#define NGBE_MII_10GBASE_T_AUTONEG_CTRL_REG	0x20   /* 10G Control Reg */
/*N*/#define NGBE_MII_AUTONEG_VENDOR_PROVISION_1_REG 0xC400 /* 1G Provisioning 1 */
/*N*/#define NGBE_MII_AUTONEG_XNP_TX_REG		0x17   /* 1G XNP Transmit */
/*N*/#define NGBE_MII_AUTONEG_ADVERTISE_REG		0x10   /* 100M Advertisement */
/*N*/#define NGBE_MII_10GBASE_T_ADVERTISE		0x1000 /* full duplex, bit:12*/
/*N*/#define NGBE_MII_1GBASE_T_ADVERTISE_XNP_TX	0x4000 /* full duplex, bit:14*/
/*N*/#define NGBE_MII_1GBASE_T_ADVERTISE		0x8000 /* full duplex, bit:15*/
/*N*/#define NGBE_MII_2_5GBASE_T_ADVERTISE		0x0400
/*N*/#define NGBE_MII_5GBASE_T_ADVERTISE		0x0800
/*N*/#define NGBE_MII_100BASE_T_ADVERTISE		0x0100 /* full duplex, bit:8 */
/*N*/#define NGBE_MII_100BASE_T_ADVERTISE_HALF	0x0080 /* half duplex, bit:7 */
/*N*/#define NGBE_MII_RESTART			0x200
/*N*/#define NGBE_MII_AUTONEG_COMPLETE		0x20
/*N*/#define NGBE_MII_AUTONEG_LINK_UP		0x04
/*N*/#define NGBE_MII_AUTONEG_REG			0x0
/*N*/#define NGBE_MD_PMA_TX_VEN_LASI_INT_MASK 0xD401 /* PHY TX Vendor LASI */
/*N*/#define NGBE_MD_PMA_TX_VEN_LASI_INT_EN   0x1 /* PHY TX Vendor LASI enable */
/*N*/#define NGBE_MD_PMD_STD_TX_DISABLE_CNTR 0x9 /* Standard Transmit Dis Reg */
/*N*/#define NGBE_MD_PMD_GLOBAL_TX_DISABLE 0x0001 /* PMD Global Transmit Dis */

/* (dev_type = 30) */
#define NGBE_MD_DEV_VENDOR_1	30
#define NGBE_MD_DEV_XFI_DSP     30
/*N*/#define TNX_FW_REV	0xB
/*N*///#define AQ_FW_REV                               0x20
/*N*/#define NGBE_MD_VENDOR_SPECIFIC_1_CONTROL		0x0 /* VS1 Ctrl Reg */
/*N*/#define NGBE_MD_VENDOR_SPECIFIC_1_STATUS		0x1 /* VS1 Status Reg */
/*N*/#define NGBE_MD_VENDOR_SPECIFIC_1_LINK_STATUS	0x0008 /* 1 = Link Up */
/*N*/#define NGBE_MD_VENDOR_SPECIFIC_1_SPEED_STATUS	0x0010 /* 0-10G, 1-1G */
/*N*/#define NGBE_MD_VENDOR_SPECIFIC_1_10G_SPEED		0x0018
/*N*/#define NGBE_MD_VENDOR_SPECIFIC_1_1G_SPEED		0x0010

/* (dev_type = 31) */
#define NGBE_MD_DEV_GENERAL          31
#define NGBE_MD_PORT_CTRL            0xF001
#define   NGBE_MD_PORT_CTRL_RESET    MS16(14, 0x1)

/* IEEE 802.3 Clause 22 */
struct mdi_reg_22
{
	u16 page;
	u16 addr;
	u16 device_type;
};
typedef struct mdi_reg_22 mdi_reg_22_t;

/* IEEE 802.3ae Clause 45 */
struct mdi_reg
{
	u16 device_type;
	u16 addr;
};
typedef struct mdi_reg mdi_reg_t;

#define NGBE_MD22_PHY_ID_HIGH		0x2 /* PHY ID High Reg*/
#define NGBE_MD22_PHY_ID_LOW		0x3 /* PHY ID Low Reg*/

s32 ngbe_mdi_map_register(mdi_reg_t *reg, mdi_reg_22_t *reg22);

bool ngbe_validate_phy_addr(struct ngbe_hw *hw, u32 phy_addr);
s32 ngbe_get_phy_type_from_id(struct ngbe_hw *hw);
s32 ngbe_get_phy_id(struct ngbe_hw *hw);
s32 ngbe_identify_phy(struct ngbe_hw *hw);
s32 ngbe_reset_phy(struct ngbe_hw *hw);
s32 ngbe_read_phy_reg_mdi(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			   u16 *phy_data);
s32 ngbe_write_phy_reg_mdi(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			    u16 phy_data);

s32 ngbe_read_phy_reg(struct ngbe_hw *hw, u32 reg_addr,
			       u32 device_type, u16 *phy_data);
s32 ngbe_write_phy_reg(struct ngbe_hw *hw, u32 reg_addr,
				u32 device_type, u16 phy_data);
s32 ngbe_setup_phy_link(struct ngbe_hw *hw,
			u32 speed, bool autoneg_wait_to_complete);
s32 ngbe_get_copper_link_capabilities(struct ngbe_hw *hw,
					       u32 *speed,
					       bool *autoneg);
s32 ngbe_check_reset_blocked(struct ngbe_hw *hw);

/* PHY specific */
#if 0
u32 ngbe_setup_phy_link_rl(struct ngbe_hw *hw,
						u32 speed,
						bool autoneg_wait_to_complete);
u32 ngbe_setup_phy_link_me(struct ngbe_hw *hw,
								u32 speed,
								bool autoneg_wait_to_complete);
u32 ngbe_setup_phy_link_zte(struct ngbe_hw *hw,
							u32 speed,
							bool autoneg_wait_to_complete);
s32 ngbe_setup_phy_link_tnx(struct ngbe_hw *hw);

s32 ngbe_init_phy_rl(struct ngbe_hw *hw);
s32 ngbe_reset_phy_rl(struct ngbe_hw *hw);
s32 ngbe_reset_phy_me(struct ngbe_hw *hw);
s32 ngbe_reset_phy_zte(struct ngbe_hw *hw);
s32 ngbe_reset_phy_nl(struct ngbe_hw *hw);

s32 ngbe_check_phy_link_tnx(struct ngbe_hw *hw,
			     u32 *speed,
			     bool *link_up);
s32 ngbe_check_phy_link_rl(struct ngbe_hw *hw, 
				 u32 *speed, 
				 bool *link_up);
s32 ngbe_check_phy_link_me(struct ngbe_hw *hw, 
						   u32 *speed, 
						   bool *link_up);


s32 ngbe_get_phy_advertised_pause_rl(struct ngbe_hw *hw, u8 *pause_bit);
s32 ngbe_get_phy_advertised_pause_me(struct ngbe_hw *hw, 
												u8 *pause_bit);
s32 ngbe_get_phy_lp_advertised_pause_rl(struct ngbe_hw *hw, 
												u8 *pause_bit);
s32 ngbe_get_phy_lp_advertised_pause_me(struct ngbe_hw *hw,
												u8 *pause_bit);
s32 ngbe_set_phy_pause_adv_rl(struct ngbe_hw *hw, u16 pause_bit);
s32 ngbe_set_phy_pause_adv_me(struct ngbe_hw *hw, u16 pause_bit);
#endif
s32 ngbe_get_phy_firmware_version_tnx(struct ngbe_hw *hw,
				       u32 *firmware_version);
s32 ngbe_get_phy_firmware_version(struct ngbe_hw *hw,
					   u32 *firmware_version);

s32 ngbe_identify_fiber_module(struct ngbe_hw *hw);
s32 ngbe_identify_sfp_module(struct ngbe_hw *hw);
u64 ngbe_get_supported_phy_sfp_layer(struct ngbe_hw *hw);
s32 ngbe_identify_qsfp_module(struct ngbe_hw *hw);
s32 ngbe_get_sfp_init_sequence_offsets(struct ngbe_hw *hw,
					u16 *list_offset,
					u16 *data_offset);
s32 ngbe_tn_check_overtemp(struct ngbe_hw *hw);
u64 ngbe_autoc_read(struct ngbe_hw *hw);
void ngbe_autoc_write(struct ngbe_hw *hw, u64 value);


s32 ngbe_check_phy_link_tnx(struct ngbe_hw *hw, u32 *speed,
			     bool *link_up);
s32 ngbe_reset_phy_nl(struct ngbe_hw *hw);
s32 ngbe_setup_phy_link_tnx(struct ngbe_hw *hw,
			u32 speed, bool autoneg_wait_to_complete);

#endif /* _NGBE_PHY_H_ */
