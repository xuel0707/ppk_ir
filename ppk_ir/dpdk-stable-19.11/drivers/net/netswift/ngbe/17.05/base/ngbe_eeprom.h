/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_EEPROM_H_
#define _NGBE_EEPROM_H_

/* Checksum and EEPROM pointers */
/*N*/#define NGBE_PBANUM_PTR_GUARD		0xFAFA
/*N*/#define NGBE_EEPROM_SUM		0xBABA
/*N*/#define NGBE_EEPROM_CTRL_4		0x45
/*N*/#define NGBE_EE_CTRL_4_INST_ID		0x10
/*N*/#define NGBE_EE_CTRL_4_INST_ID_SHIFT	4

/*N*/#define NGBE_PCIE_ANALOG_PTR	0x02
/*N*/#define NGBE_ATLAS0_CONFIG_PTR	0x04
/*N*/#define NGBE_PHY_PTR		0x04
/*N*/#define NGBE_ATLAS1_CONFIG_PTR	0x05
/*N*/#define NGBE_OPTION_ROM_PTR	0x05
/*N*/#define NGBE_PCIE_GENERAL_PTR	0x06
/*N*/#define NGBE_PCIE_CONFIG0_PTR	0x07
/*N*/#define NGBE_PCIE_CONFIG1_PTR	0x08
/*N*/#define NGBE_CORE0_PTR		0x09
/*N*/#define NGBE_CORE1_PTR		0x0A
/*N*/#define NGBE_MAC0_PTR		0x0B
/*N*/#define NGBE_MAC1_PTR		0x0C
/*N*/#define NGBE_CSR0_CONFIG_PTR	0x0D
/*N*/#define NGBE_CSR1_CONFIG_PTR	0x0E
/*N*/#define NGBE_FW_PTR		0x0F
/*N*/#define NGBE_PBANUM0_PTR		0x05
/*N*/#define NGBE_PBANUM1_PTR		0x06
/*N*///#define NGBE_EEPROM_LAST_WORD		0x41
#define NGBE_EE_PTR_MAX		0x1E

#define NGBE_EE_CSUM_MAX		0x800

#define NGBE_EEPROM_CHECKSUM		0x2F

/*N*/#define NGBE_SAN_MAC_ADDR_PTR		0x18
/*N*/#define NGBE_DEVICE_CAPS		0x1C
#define NGBE_EEPROM_VERSION_L          0x1D
#define NGBE_EEPROM_VERSION_H          0x1E
#define NGBE_ISCSI_BOOT_CONFIG         0x07
#define NGBE_CALSUM_CAP_STATUS         0x10224
#define NGBE_EEPROM_VERSION_STORE_REG  0x1022C

/* Special PHY Init Routine */
/*N*/#define NGBE_EE_PHY_INIT_OFFSET_NL	0x002B
/*N*/#define NGBE_EE_PHY_INIT_END_NL	0xFFFF
/*N*/#define NGBE_EE_CONTROL_MASK_NL	0xF000
/*N*/#define NGBE_EE_DATA_MASK_NL		0x0FFF
/*N*/#define NGBE_EE_CONTROL_SHIFT_NL	12
/*N*/#define NGBE_EE_DELAY_NL		0
/*N*/#define NGBE_EE_DATA_NL		1
/*N*/#define NGBE_EE_CONTROL_NL		0x000F
/*N*/#define NGBE_EE_CONTROL_EOL_NL		0x0FFF
/*N*/#define NGBE_EE_CONTROL_SOL_NL		0x0000


/*N*/#define NVM_INIT_CTRL_3		0x38
/*N*/#define NVM_INIT_CTRL_3_LPLU	0x8
/*N*/#define NVM_INIT_CTRL_3_D10GMP_PORT0 0x40
/*N*/#define NVM_INIT_CTRL_3_D10GMP_PORT1 0x100


/*N*/#define NGBE_SAN_MAC_ADDR_PORT0_OFFSET		0x0
/*N*/#define NGBE_SAN_MAC_ADDR_PORT1_OFFSET		0x3
/*N*/#define NGBE_DEVICE_CAPS_ALLOW_ANY_SFP		0x1
/*N*/#define NGBE_DEVICE_CAPS_FCOE_OFFLOADS		0x2
/*N*/#define NGBE_DEVICE_CAPS_NO_CROSSTALK_WR	(1 << 7)
/*N*/#define NGBE_FW_LESM_PARAMETERS_PTR		0x2
/*N*/#define NGBE_FW_LESM_STATE_1			0x1
/*N*/#define NGBE_FW_LESM_STATE_ENABLED		0x8000 /* LESM Enable bit */
/*N*/#define NGBE_FW_LESM_2_STATES_ENABLED_MASK	0x1F
/*N*/#define NGBE_FW_LESM_2_STATES_ENABLED		0x12
/*N*/#define NGBE_FW_LESM_STATE0_10G_ENABLED	0x6FFF
/*N*/#define NGBE_FW_LESM_STATE1_10G_ENABLED	0x4FFF
/*N*/#define NGBE_FW_LESM_STATE0_10G_DISABLED	0x0FFF
/*N*/#define NGBE_FW_LESM_STATE1_10G_DISABLED	0x2FFF
/*N*/#define NGBE_FW_LESM_PORT0_STATE0_OFFSET	0x2
/*N*/#define NGBE_FW_LESM_PORT0_STATE1_OFFSET	0x3
/*N*/#define NGBE_FW_LESM_PORT1_STATE0_OFFSET	0x6
/*N*/#define NGBE_FW_LESM_PORT1_STATE1_OFFSET	0x7
/*N*/#define NGBE_FW_PASSTHROUGH_PATCH_CONFIG_PTR	0x4
/*N*/#define NGBE_FW_PATCH_VERSION_4		0x7
/*N*/#define NGBE_FCOE_IBA_CAPS_BLK_PTR		0x33 /* iSCSI/FCOE block */
/*N*/#define NGBE_FCOE_IBA_CAPS_FCOE		0x20 /* FCOE flags */
/*N*/#define NGBE_ISCSI_FCOE_BLK_PTR		0x17 /* iSCSI/FCOE block */
/*N*/#define NGBE_ISCSI_FCOE_FLAGS_OFFSET		0x0 /* FCOE flags */
/*N*/#define NGBE_ISCSI_FCOE_FLAGS_ENABLE		0x1 /* FCOE flags enable bit */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_BLK_PTR		0x27 /* Alt. SAN MAC block */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_CAPS_OFFSET	0x0 /* Alt SAN MAC capability */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_PORT0_OFFSET	0x1 /* Alt SAN MAC 0 offset */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_PORT1_OFFSET	0x4 /* Alt SAN MAC 1 offset */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_WWNN_OFFSET	0x7 /* Alt WWNN prefix offset */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_WWPN_OFFSET	0x8 /* Alt WWPN prefix offset */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_CAPS_SANMAC	0x0 /* Alt SAN MAC exists */
/*N*/#define NGBE_ALT_SAN_MAC_ADDR_CAPS_ALTWWN	0x1 /* Alt WWN base exists */

/*N*/#define NGBE_ISCSI_BOOT_CAPS		0x0033
/*N*/#define NGBE_ISCSI_SETUP_PORT_0	0x0030
/*N*/#define NGBE_ISCSI_SETUP_PORT_1	0x0034

s32 ngbe_init_eeprom_params(struct ngbe_hw *hw);
s32 ngbe_calc_eeprom_checksum(struct ngbe_hw *hw);
s32 ngbe_validate_eeprom_checksum(struct ngbe_hw *hw, u16 *checksum_val);
s32 ngbe_update_eeprom_checksum(struct ngbe_hw *hw);
s32 ngbe_save_eeprom_version(struct ngbe_hw *hw);
s32 ngbe_get_eeprom_semaphore(struct ngbe_hw *hw);
void ngbe_release_eeprom_semaphore(struct ngbe_hw *hw);

s32 ngbe_ee_read16(struct ngbe_hw *hw, u32 offset, u16 *data);
s32 ngbe_ee_readw_sw(struct ngbe_hw *hw, u32 offset, u16 *data);
s32 ngbe_ee_readw_buffer(struct ngbe_hw *hw, u32 offset, u32 words, void *data);
s32 ngbe_ee_read32(struct ngbe_hw *hw, u32 addr, u32 *data);
s32 ngbe_ee_read_buffer(struct ngbe_hw *hw, u32 addr, u32 len, void *data);

s32 ngbe_ee_write16(struct ngbe_hw *hw, u32 offset, u16 data);
s32 ngbe_ee_writew_sw(struct ngbe_hw *hw, u32 offset, u16 data);
s32 ngbe_ee_writew_buffer(struct ngbe_hw *hw, u32 offset, u32 words, void *data);
s32 ngbe_ee_write32(struct ngbe_hw *hw, u32 addr, u32 data);
s32 ngbe_ee_write_buffer(struct ngbe_hw *hw, u32 addr, u32 len, void *data);

#endif /* _NGBE_EEPROM_H_ */
