/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _TXGBE_EEPROM_H_
#define _TXGBE_EEPROM_H_

/* Checksum and EEPROM pointers */
/*N*/#define TXGBE_PBANUM_PTR_GUARD		0xFAFA
/*N*/#define TXGBE_EEPROM_SUM		0xBABA
/*N*/#define TXGBE_EEPROM_CTRL_4		0x45
/*N*/#define TXGBE_EE_CTRL_4_INST_ID		0x10
/*N*/#define TXGBE_EE_CTRL_4_INST_ID_SHIFT	4

#define TXGBE_FW_VER_LEN	32
#define TXGBE_FW_N_TXEQ		0x0002000A

/*N*/#define TXGBE_PCIE_ANALOG_PTR	0x02
/*N*/#define TXGBE_ATLAS0_CONFIG_PTR	0x04
/*N*/#define TXGBE_PHY_PTR		0x04
/*N*/#define TXGBE_ATLAS1_CONFIG_PTR	0x05
/*N*/#define TXGBE_OPTION_ROM_PTR	0x05
/*N*/#define TXGBE_PCIE_GENERAL_PTR	0x06
/*N*/#define TXGBE_PCIE_CONFIG0_PTR	0x07
/*N*/#define TXGBE_PCIE_CONFIG1_PTR	0x08
/*N*/#define TXGBE_CORE0_PTR		0x09
/*N*/#define TXGBE_CORE1_PTR		0x0A
/*N*/#define TXGBE_MAC0_PTR		0x0B
/*N*/#define TXGBE_MAC1_PTR		0x0C
/*N*/#define TXGBE_CSR0_CONFIG_PTR	0x0D
/*N*/#define TXGBE_CSR1_CONFIG_PTR	0x0E
/*N*/#define TXGBE_FW_PTR		0x0F
/*N*/#define TXGBE_PBANUM0_PTR		0x05
/*N*/#define TXGBE_PBANUM1_PTR		0x06
#define TXGBE_SW_REGION_PTR             0x1C
/*N*///#define TXGBE_EEPROM_LAST_WORD		0x41
#define TXGBE_EE_PTR_MAX		0x1E

#define TXGBE_EE_CSUM_MAX		0x800

#define TXGBE_EEPROM_CHECKSUM		0x2F
/*N*///#define TXGBE_ALT_MAC_ADDR_PTR	0x37
/*N*///#define TXGBE_FREE_SPACE_PTR	0X3E



/*N*///#define TXGBE_TXGBE_PCIE_GENERAL_SIZE	0x24
/*N*///#define TXGBE_PCIE_CONFIG_SIZE		0x08


/*N*/#define TXGBE_SAN_MAC_ADDR_PTR		0x18
/*N*/#define TXGBE_DEVICE_CAPS		0x1C
#define TXGBE_EEPROM_VERSION_L          0x1D
#define TXGBE_EEPROM_VERSION_H          0x1E
#define TXGBE_ISCSI_BOOT_CONFIG         0x07

/* Special PHY Init Routine */
/*N*/#define TXGBE_EE_PHY_INIT_OFFSET_NL	0x002B
/*N*/#define TXGBE_EE_PHY_INIT_END_NL	0xFFFF
/*N*/#define TXGBE_EE_CONTROL_MASK_NL	0xF000
/*N*/#define TXGBE_EE_DATA_MASK_NL		0x0FFF
/*N*/#define TXGBE_EE_CONTROL_SHIFT_NL	12
/*N*/#define TXGBE_EE_DELAY_NL		0
/*N*/#define TXGBE_EE_DATA_NL		1
/*N*/#define TXGBE_EE_CONTROL_NL		0x000F
/*N*/#define TXGBE_EE_CONTROL_EOL_NL		0x0FFF
/*N*/#define TXGBE_EE_CONTROL_SOL_NL		0x0000


/*N*/#define NVM_INIT_CTRL_3		0x38
/*N*/#define NVM_INIT_CTRL_3_LPLU	0x8
/*N*/#define NVM_INIT_CTRL_3_D10GMP_PORT0 0x40
/*N*/#define NVM_INIT_CTRL_3_D10GMP_PORT1 0x100


/*N*/#define TXGBE_SAN_MAC_ADDR_PORT0_OFFSET		0x0
/*N*/#define TXGBE_SAN_MAC_ADDR_PORT1_OFFSET		0x3
/*N*/#define TXGBE_DEVICE_CAPS_ALLOW_ANY_SFP		0x1
/*N*/#define TXGBE_DEVICE_CAPS_FCOE_OFFLOADS		0x2
/*N*/#define TXGBE_DEVICE_CAPS_NO_CROSSTALK_WR	(1 << 7)
/*N*/#define TXGBE_FW_LESM_PARAMETERS_PTR		0x2
/*N*/#define TXGBE_FW_LESM_STATE_1			0x1
/*N*/#define TXGBE_FW_LESM_STATE_ENABLED		0x8000 /* LESM Enable bit */
/*N*/#define TXGBE_FW_LESM_2_STATES_ENABLED_MASK	0x1F
/*N*/#define TXGBE_FW_LESM_2_STATES_ENABLED		0x12
/*N*/#define TXGBE_FW_LESM_STATE0_10G_ENABLED	0x6FFF
/*N*/#define TXGBE_FW_LESM_STATE1_10G_ENABLED	0x4FFF
/*N*/#define TXGBE_FW_LESM_STATE0_10G_DISABLED	0x0FFF
/*N*/#define TXGBE_FW_LESM_STATE1_10G_DISABLED	0x2FFF
/*N*/#define TXGBE_FW_LESM_PORT0_STATE0_OFFSET	0x2
/*N*/#define TXGBE_FW_LESM_PORT0_STATE1_OFFSET	0x3
/*N*/#define TXGBE_FW_LESM_PORT1_STATE0_OFFSET	0x6
/*N*/#define TXGBE_FW_LESM_PORT1_STATE1_OFFSET	0x7
/*N*/#define TXGBE_FW_PASSTHROUGH_PATCH_CONFIG_PTR	0x4
/*N*/#define TXGBE_FW_PATCH_VERSION_4		0x7
/*N*/#define TXGBE_FCOE_IBA_CAPS_BLK_PTR		0x33 /* iSCSI/FCOE block */
/*N*/#define TXGBE_FCOE_IBA_CAPS_FCOE		0x20 /* FCOE flags */
/*N*/#define TXGBE_ISCSI_FCOE_BLK_PTR		0x17 /* iSCSI/FCOE block */
/*N*/#define TXGBE_ISCSI_FCOE_FLAGS_OFFSET		0x0 /* FCOE flags */
/*N*/#define TXGBE_ISCSI_FCOE_FLAGS_ENABLE		0x1 /* FCOE flags enable bit */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_BLK_PTR		0x27 /* Alt. SAN MAC block */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_CAPS_OFFSET	0x0 /* Alt SAN MAC capability */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_PORT0_OFFSET	0x1 /* Alt SAN MAC 0 offset */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_PORT1_OFFSET	0x4 /* Alt SAN MAC 1 offset */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_WWNN_OFFSET	0x7 /* Alt WWNN prefix offset */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_WWPN_OFFSET	0x8 /* Alt WWPN prefix offset */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_CAPS_SANMAC	0x0 /* Alt SAN MAC exists */
/*N*/#define TXGBE_ALT_SAN_MAC_ADDR_CAPS_ALTWWN	0x1 /* Alt WWN base exists */

/*N*/#define TXGBE_ISCSI_BOOT_CAPS		0x0033
/*N*/#define TXGBE_ISCSI_SETUP_PORT_0	0x0030
/*N*/#define TXGBE_ISCSI_SETUP_PORT_1	0x0034

s32 txgbe_init_eeprom_params(struct txgbe_hw *hw);
s32 txgbe_calc_eeprom_checksum(struct txgbe_hw *hw);
s32 txgbe_validate_eeprom_checksum(struct txgbe_hw *hw, u16 *checksum_val);
s32 txgbe_update_eeprom_checksum(struct txgbe_hw *hw);
s32 txgbe_get_eeprom_semaphore(struct txgbe_hw *hw);
void txgbe_release_eeprom_semaphore(struct txgbe_hw *hw);

s32 txgbe_ee_read16(struct txgbe_hw *hw, u32 offset, u16 *data);
s32 txgbe_ee_readw_sw(struct txgbe_hw *hw, u32 offset, u16 *data);
s32 txgbe_ee_readw_buffer(struct txgbe_hw *hw, u32 offset, u32 words, void *data);
s32 txgbe_ee_read32(struct txgbe_hw *hw, u32 addr, u32 *data);
s32 txgbe_ee_read_buffer(struct txgbe_hw *hw, u32 addr, u32 len, void *data);

s32 txgbe_ee_write16(struct txgbe_hw *hw, u32 offset, u16 data);
s32 txgbe_ee_writew_sw(struct txgbe_hw *hw, u32 offset, u16 data);
s32 txgbe_ee_writew_buffer(struct txgbe_hw *hw, u32 offset, u32 words, void *data);
s32 txgbe_ee_write32(struct txgbe_hw *hw, u32 addr, u32 data);
s32 txgbe_ee_write_buffer(struct txgbe_hw *hw, u32 addr, u32 len, void *data);

#endif /* _TXGBE_EEPROM_H_ */
