/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_MNG_H_
#define _NGBE_MNG_H_

#include "ngbe_type.h"


#define NGBE_PMMBX_QSIZE       64 /* Num of dwords in range */
#define NGBE_PMMBX_BSIZE       (NGBE_PMMBX_QSIZE * 4)
#define NGBE_PMMBX_DATA_SIZE   (NGBE_PMMBX_BSIZE - FW_NVM_DATA_OFFSET * 4)
#define NGBE_HI_COMMAND_TIMEOUT        5000 /* Process HI command limit */
#define NGBE_HI_FLASH_ERASE_TIMEOUT    5000 /* Process Erase command limit */
#define NGBE_HI_FLASH_UPDATE_TIMEOUT   5000 /* Process Update command limit */
#define NGBE_HI_FLASH_VERIFY_TIMEOUT   60000 /* Process Apply command limit */
#define NGBE_HI_PHY_MGMT_REQ_TIMEOUT   2000 /* Wait up to 2 seconds */

/* CEM Support */
#define FW_CEM_HDR_LEN                  0x4
#define FW_CEM_CMD_DRIVER_INFO          0xDD
#define FW_CEM_CMD_DRIVER_INFO_LEN      0x5
#define FW_CEM_CMD_RESERVED             0X0
#define FW_CEM_UNUSED_VER               0x0
#define FW_CEM_MAX_RETRIES              3
#define FW_CEM_RESP_STATUS_SUCCESS      0x1
#define FW_READ_SHADOW_RAM_CMD          0x31
#define FW_READ_SHADOW_RAM_LEN          0x6
#define FW_WRITE_SHADOW_RAM_CMD         0x33
#define FW_WRITE_SHADOW_RAM_LEN         0xA /* 8 plus 1 WORD to write */
#define FW_SHADOW_RAM_DUMP_CMD          0x36
#define FW_SHADOW_RAM_DUMP_LEN          0
#define FW_PCIE_READ_CMD		0xEC
#define FW_PCIE_WRITE_CMD		0xED
#define FW_PCIE_BUSMASTER_OFFSET        2
#define FW_DEFAULT_CHECKSUM             0xFF /* checksum always 0xFF */
#define FW_NVM_DATA_OFFSET              3
#define FW_MAX_READ_BUFFER_SIZE         244
#define FW_DISABLE_RXEN_CMD             0xDE
#define FW_DISABLE_RXEN_LEN             0x1
#define FW_PHY_MGMT_REQ_CMD             0x20
#define FW_RESET_CMD                    0xDF
#define FW_RESET_LEN                    0x2
#define FW_SETUP_MAC_LINK_CMD           0xE0
#define FW_SETUP_MAC_LINK_LEN           0x2
#define FW_FLASH_UPGRADE_START_CMD      0xE3
#define FW_FLASH_UPGRADE_START_LEN      0x1
#define FW_FLASH_UPGRADE_WRITE_CMD      0xE4
#define FW_FLASH_UPGRADE_VERIFY_CMD     0xE5
#define FW_FLASH_UPGRADE_VERIFY_LEN     0x4
#define FW_EEPROM_CHECK_STATUS		0xE9
#define FW_PHY_ACT_DATA_COUNT		4
#define FW_PHY_TOKEN_DELAY		5	/* milliseconds */
#define FW_PHY_TOKEN_WAIT		5	/* seconds */
#define FW_PHY_TOKEN_RETRIES ((FW_PHY_TOKEN_WAIT * 1000) / FW_PHY_TOKEN_DELAY)
#define FW_PHY_SIGNAL			0xF0
#define FW_PHY_LED_CONF			0xF1

#define FW_CHECKSUM_CAP_ST_PASS	0x80658383
#define FW_CHECKSUM_CAP_ST_FAIL	0x70657376
#define FW_ERR_ST		0xFFFFFFFF

/* Host Interface Command Structures */
struct ngbe_hic_hdr {
	u8 cmd;
	u8 buf_len;
	union {
		u8 cmd_resv;
		u8 ret_status;
	} cmd_or_resp;
	u8 checksum;
};

struct ngbe_hic_hdr2_req {
	u8 cmd;
	u8 buf_lenh;
	u8 buf_lenl;
	u8 checksum;
};

struct ngbe_hic_hdr2_rsp {
	u8 cmd;
	u8 buf_lenl;
	u8 ret_status;     /* 7-5: high bits of buf_len, 4-0: status */
	u8 checksum;
};

union ngbe_hic_hdr2 {
	struct ngbe_hic_hdr2_req req;
	struct ngbe_hic_hdr2_rsp rsp;
};

struct ngbe_hic_drv_info {
	struct ngbe_hic_hdr hdr;
	u8 port_num;
	u8 ver_sub;
	u8 ver_build;
	u8 ver_min;
	u8 ver_maj;
	u8 pad; /* end spacing to ensure length is mult. of dword */
	u16 pad2; /* end spacing to ensure length is mult. of dword2 */
};

/* These need to be dword aligned */
struct ngbe_hic_read_shadow_ram {
	union ngbe_hic_hdr2 hdr;
	u32 address;
	u16 length;
	u16 pad2;
	u16 data;
	u16 pad3;
};

struct ngbe_hic_write_shadow_ram {
	union ngbe_hic_hdr2 hdr;
	u32 address;
	u16 length;
	u16 pad2;
	u16 data;
	u16 pad3;
};

struct ngbe_hic_read_pcie {
	struct ngbe_hic_hdr hdr;
	u8 lan_id;
	u8 rsvd;
	u16 addr;
	u32 data;
};

struct ngbe_hic_write_pcie {
	struct ngbe_hic_hdr hdr;
	u8 lan_id;
	u8 rsvd;
	u16 addr;
	u32 data;
};

struct ngbe_hic_disable_rxen {
	struct ngbe_hic_hdr hdr;
	u8  port_number;
	u8  pad2;
	u16 pad3;
};

struct ngbe_hic_reset {
	struct ngbe_hic_hdr hdr;
	u16 lan_id;
	u16 reset_type;
};

struct ngbe_hic_phy_cfg {
	struct ngbe_hic_hdr hdr;
	u8 lan_id;
	u8 phy_mode;
	u16 phy_speed;
};

enum ngbe_module_id {
	NGBE_MODULE_EEPROM = 0,
	NGBE_MODULE_FIRMWARE,
	NGBE_MODULE_HARDWARE,
	NGBE_MODULE_PCIE
};

struct ngbe_hic_upg_start {
	struct ngbe_hic_hdr hdr;
	u8 module_id;
	u8  pad2;
	u16 pad3;
};

struct ngbe_hic_upg_write {
	struct ngbe_hic_hdr hdr;
	u8 data_len;
	u8 eof_flag;
	u16 check_sum;
	u32 data[62];
};

enum ngbe_upg_flag {
	NGBE_RESET_NONE = 0,
	NGBE_RESET_FIRMWARE,
	NGBE_RELOAD_EEPROM,
	NGBE_RESET_LAN
};

struct ngbe_hic_upg_verify {
	struct ngbe_hic_hdr hdr;
	u32 action_flag;
};

s32 ngbe_hic_sr_read(struct ngbe_hw *hw, u32 addr, u8 *buf, int len);
s32 ngbe_hic_sr_write(struct ngbe_hw *hw, u32 addr, u8 *buf, int len);
s32 ngbe_hic_pcie_read(struct ngbe_hw *hw, u16 addr, u32 *buf, int len);
s32 ngbe_hic_pcie_write(struct ngbe_hw * hw, u16 addr, u32 *buf, int len);

s32 ngbe_hic_set_drv_ver(struct ngbe_hw *hw, u8 maj, u8 min, u8 build, u8 ver, u16 len, const char *str);
s32 ngbe_hic_reset(struct ngbe_hw *hw);
bool ngbe_mng_present(struct ngbe_hw *hw);
s32 ngbe_hic_check_cap(struct ngbe_hw *hw);
s32 ngbe_phy_signal_set(struct ngbe_hw *hw);
s32 ngbe_phy_led_oem_chk(struct ngbe_hw *hw, u32 *data);

#endif /* _NGBE_MNG_H_ */
