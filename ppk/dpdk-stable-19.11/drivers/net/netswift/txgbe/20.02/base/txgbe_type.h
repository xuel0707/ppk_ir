/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _TXGBE_TYPE_H_
#define _TXGBE_TYPE_H_

/*
 * Driver Configuration
 */
/* DCB configuration defines */
#define TXGBE_DCB_TC_MAX	TXGBE_MAX_UP
#define TXGBE_DCB_UP_MAX	TXGBE_MAX_UP
#define TXGBE_DCB_BWG_MAX	TXGBE_MAX_UP
#define TXGBE_DCB_BW_PERCENT	100

#define TXGBE_LINK_UP_TIME         90 /* 9.0 Seconds */
#define TXGBE_AUTO_NEG_TIME		45 /* 4.5 Seconds */

#define TXGBE_RX_HDR_SIZE    256
#define TXGBE_RX_BUF_SIZE    2048


#define TXGBE_FRAME_SIZE_MAX       (9728) /* Maximum frame size, +FCS */
#define TXGBE_FRAME_SIZE_DFT       (1518) /* Default frame size, +FCS */
#define TXGBE_NUM_POOL             (64)
#define TXGBE_PBRXSIZE_MAX         0x00080000 /* 512KB Packet Buffer */
#define TXGBE_TXPKTSIZE_MAX        (10)
#define TXGBE_PBTXSIZE_MAX         0x00028000 /* 160KB Packet Buffer */
#define TXGBE_FDIR_DROP_QUEUE      127
#define TXGBE_MAX_FTQF_FILTERS     128
#define TXGBE_TXPKT_SIZE_MAX    0xA /* Max Tx Packet size */
#define TXGBE_MAX_UP            8
#define TXGBE_MAX_QP (128)

#define TXGBE_MAX_UTA                   128

#define TXGBE_FDIR_INIT_DONE_POLL		10
#define TXGBE_FDIRCMD_CMD_POLL			10
#define TXGBE_MD_TIMEOUT 1000
#define TXGBE_SPI_TIMEOUT  1000
#define TXGBE_VF_INIT_TIMEOUT	200 /* Number of retries to clear RSTI */
#define TXGBE_PCI_MASTER_DISABLE_TIMEOUT	800

#define TXGBE_MAX_MSIX_VECTORS_RAPTOR	0x40

#define TXGBE_ALIGN		128 /* as intel did */

/*
 * The following is a brief description of the error categories used by the
 * ERROR_REPORT* macros.
 *
 * - TXGBE_ERROR_INVALID_STATE
 * This category is for errors which represent a serious failure state that is
 * unexpected, and could be potentially harmful to device operation. It should
 * not be used for errors relating to issues that can be worked around or
 * ignored.
 *
 * - TXGBE_ERROR_POLLING
 * This category is for errors related to polling/timeout issues and should be
 * used in any case where the timeout occured, or a failure to obtain a lock, or
 * failure to receive data within the time limit.
 *
 * - TXGBE_ERROR_CAUTION
 * This category should be used for reporting issues that may be the cause of
 * other errors, such as temperature warnings. It should indicate an event which
 * could be serious, but hasn't necessarily caused problems yet.
 *
 * - TXGBE_ERROR_SOFTWARE
 * This category is intended for errors due to software state preventing
 * something. The category is not intended for errors due to bad arguments, or
 * due to unsupported features. It should be used when a state occurs which
 * prevents action but is not a serious issue.
 *
 * - TXGBE_ERROR_ARGUMENT
 * This category is for when a bad or invalid argument is passed. It should be
 * used whenever a function is called and error checking has detected the
 * argument is wrong or incorrect.
 *
 * - TXGBE_ERROR_UNSUPPORTED
 * This category is for errors which are due to unsupported circumstances or
 * configuration issues. It should not be used when the issue is due to an
 * invalid argument, but for when something has occurred that is unsupported
 * (Ex: Flow control autonegotiation or an unsupported SFP+ module.)
 */

#include "txgbe_status.h"
#include "txgbe_osdep.h"
#include "txgbe_devids.h"

/* Override this by setting IOMEM in your txgbe_osdep.h header */

//#define TXGBE_CAT(r, m) TXGBE_##r##m

//#define TXGBE_BY_MAC(_hw, r) ((_hw)->mvals[TXGBE_CAT(r, _IDX)])

/* General Registers */
/*N*///#define TXGBE_CTRL		0x00000
/*N*///#define TXGBE_STATUS		0x00008
/*N*///#define TXGBE_CTRL_EXT		0x00018
/*N*///#define TXGBE_ESDP		0x00020
/*N*///#define TXGBE_EODSDP		0x00028
/*N*///#define TXGBE_I2CCTL_82599	0x00028
/*N*///#define TXGBE_I2CCTL		TXGBE_I2CCTL_82599
/*N*///#define TXGBE_I2CCTL_X540	TXGBE_I2CCTL_82599
/*N*///#define TXGBE_I2CCTL_X550	0x15F5C
/*N*///#define TXGBE_I2CCTL_X550EM_x	TXGBE_I2CCTL_X550
/*N*///#define TXGBE_I2CCTL_X550EM_a	TXGBE_I2CCTL_X550
/*N*///#define TXGBE_I2CCTL_BY_MAC(_hw) TXGBE_BY_MAC((_hw), I2CCTL)
/*N*///#define TXGBE_PHY_GPIO		0x00028
/*N*///#define TXGBE_MAC_GPIO		0x00030
/*N*///#define TXGBE_PHYINT_STATUS0	0x00100
/*N*///#define TXGBE_PHYINT_STATUS1	0x00104
/*N*///#define TXGBE_PHYINT_STATUS2	0x00108
/*N*///#define TXGBE_LEDCTL		0x00200
/*N*///#define TXGBE_FRTIMER		0x00048
/*N*///#define TXGBE_TCPTIMER		0x0004C
/*N*///#define TXGBE_CORESPARE		0x00600
/*N*///#define TXGBE_EXVET		0x05078

/* NVM Registers */
/*N*///#define TXGBE_EEC		0x10010
/*N*///#define TXGBE_EEC_X540		TXGBE_EEC
/*N*///#define TXGBE_EEC_X550		TXGBE_EEC
/*N*///#define TXGBE_EEC_X550EM_x	TXGBE_EEC
/*N*///#define TXGBE_EEC_X550EM_a	0x15FF8
/*N*///#define TXGBE_EEC_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), EEC)

/*N*///#define TXGBE_EERD		0x10014
/*N*///#define TXGBE_EEWR		0x10018

/*N*///#define TXGBE_FLA		0x1001C
/*N*///#define TXGBE_FLA_X540		TXGBE_FLA
/*N*///#define TXGBE_FLA_X550		TXGBE_FLA
/*N*///#define TXGBE_FLA_X550EM_x	TXGBE_FLA
/*N*///#define TXGBE_FLA_X550EM_a	0x15F68
/*N*///#define TXGBE_FLA_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), FLA)

/*N*///#define TXGBE_EEMNGCTL	0x10110
/*N*///#define TXGBE_EEMNGDATA	0x10114
/*N*///#define TXGBE_FLMNGCTL	0x10118
/*N*///#define TXGBE_FLMNGDATA	0x1011C
/*N*///#define TXGBE_FLMNGCNT	0x10120
/*N*///#define TXGBE_FLOP	0x1013C

/*N*///#define TXGBE_GRC		0x10200
/*N*///#define TXGBE_GRC_X540		TXGBE_GRC
/*N*///#define TXGBE_GRC_X550		TXGBE_GRC
/*N*///#define TXGBE_GRC_X550EM_x	TXGBE_GRC
/*N*///#define TXGBE_GRC_X550EM_a	0x15F64
/*N*///#define TXGBE_GRC_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), GRC)

/*N*///#define TXGBE_SRAMREL		0x10210
/*N*///#define TXGBE_SRAMREL_X540	TXGBE_SRAMREL
/*N*///#define TXGBE_SRAMREL_X550	TXGBE_SRAMREL
/*N*///#define TXGBE_SRAMREL_X550EM_x	TXGBE_SRAMREL
/*N*///#define TXGBE_SRAMREL_X550EM_a	0x15F6C
/*N*///#define TXGBE_SRAMREL_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), SRAMREL)

/*N*///#define TXGBE_PHYDBG	0x10218

/* General Receive Control */
/*N*///#define TXGBE_GRC_MNG	0x00000001 /* Manageability Enable */
/*N*///#define TXGBE_GRC_APME	0x00000002 /* APM enabled in EEPROM */

/*N*///#define TXGBE_VPDDIAG0	0x10204
/*N*///#define TXGBE_VPDDIAG1	0x10208

/* I2CCTL Bit Masks */
/*N*///#define TXGBE_I2C_CLK_IN		0x00000001
/*N*///#define TXGBE_I2C_CLK_IN_X540		TXGBE_I2C_CLK_IN
/*N*///#define TXGBE_I2C_CLK_IN_X550		0x00004000
/*N*///#define TXGBE_I2C_CLK_IN_X550EM_x	TXGBE_I2C_CLK_IN_X550
/*N*///#define TXGBE_I2C_CLK_IN_X550EM_a	TXGBE_I2C_CLK_IN_X550
/*N*///#define TXGBE_I2C_CLK_IN_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), I2C_CLK_IN)

/*N*///#define TXGBE_I2C_CLK_OUT		0x00000002
/*N*///#define TXGBE_I2C_CLK_OUT_X540		TXGBE_I2C_CLK_OUT
/*N*///#define TXGBE_I2C_CLK_OUT_X550		0x00000200
/*N*///#define TXGBE_I2C_CLK_OUT_X550EM_x	TXGBE_I2C_CLK_OUT_X550
/*N*///#define TXGBE_I2C_CLK_OUT_X550EM_a	TXGBE_I2C_CLK_OUT_X550
/*N*///#define TXGBE_I2C_CLK_OUT_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), I2C_CLK_OUT)

/*N*///#define TXGBE_I2C_DATA_IN		0x00000004
/*N*///#define TXGBE_I2C_DATA_IN_X540		TXGBE_I2C_DATA_IN
/*N*///#define TXGBE_I2C_DATA_IN_X550		0x00001000
/*N*///#define TXGBE_I2C_DATA_IN_X550EM_x	TXGBE_I2C_DATA_IN_X550
/*N*///#define TXGBE_I2C_DATA_IN_X550EM_a	TXGBE_I2C_DATA_IN_X550
/*N*///#define TXGBE_I2C_DATA_IN_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), I2C_DATA_IN)

/*N*///#define TXGBE_I2C_DATA_OUT		0x00000008
/*N*///#define TXGBE_I2C_DATA_OUT_X540		TXGBE_I2C_DATA_OUT
/*N*///#define TXGBE_I2C_DATA_OUT_X550		0x00000400
/*N*///#define TXGBE_I2C_DATA_OUT_X550EM_x	TXGBE_I2C_DATA_OUT_X550
/*N*///#define TXGBE_I2C_DATA_OUT_X550EM_a	TXGBE_I2C_DATA_OUT_X550
/*N*///#define TXGBE_I2C_DATA_OUT_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), I2C_DATA_OUT)

/*N*///#define TXGBE_I2C_DATA_OE_N_EN		0
/*N*///#define TXGBE_I2C_DATA_OE_N_EN_X540	TXGBE_I2C_DATA_OE_N_EN
/*N*///#define TXGBE_I2C_DATA_OE_N_EN_X550	0x00000800
/*N*///#define TXGBE_I2C_DATA_OE_N_EN_X550EM_x	TXGBE_I2C_DATA_OE_N_EN_X550
/*N*///#define TXGBE_I2C_DATA_OE_N_EN_X550EM_a	TXGBE_I2C_DATA_OE_N_EN_X550
/*N*///#define TXGBE_I2C_DATA_OE_N_EN_BY_MAC(_hw) TXGBE_BY_MAC((_hw), I2C_DATA_OE_N_EN)

/*N*///#define TXGBE_I2C_BB_EN			0
/*N*///#define TXGBE_I2C_BB_EN_X540		TXGBE_I2C_BB_EN
/*N*///#define TXGBE_I2C_BB_EN_X550		0x00000100
/*N*///#define TXGBE_I2C_BB_EN_X550EM_x	TXGBE_I2C_BB_EN_X550
/*N*///#define TXGBE_I2C_BB_EN_X550EM_a	TXGBE_I2C_BB_EN_X550
/*N*///#define TXGBE_I2C_BB_EN_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), I2C_BB_EN)

/*N*///#define TXGBE_I2C_CLK_OE_N_EN		0
/*N*///#define TXGBE_I2C_CLK_OE_N_EN_X540	TXGBE_I2C_CLK_OE_N_EN
/*N*///#define TXGBE_I2C_CLK_OE_N_EN_X550	0x00002000
/*N*///#define TXGBE_I2C_CLK_OE_N_EN_X550EM_x	TXGBE_I2C_CLK_OE_N_EN_X550
/*N*///#define TXGBE_I2C_CLK_OE_N_EN_X550EM_a	TXGBE_I2C_CLK_OE_N_EN_X550
/*N*///#define TXGBE_I2C_CLK_OE_N_EN_BY_MAC(_hw) TXGBE_BY_MAC((_hw), I2C_CLK_OE_N_EN)
/*N*///#define TXGBE_I2C_CLOCK_STRETCHING_TIMEOUT	500

/*N*///#define TXGBE_I2C_THERMAL_SENSOR_ADDR	0xF8
/*N*///#define TXGBE_EMC_INTERNAL_DATA		0x00
/*N*///#define TXGBE_EMC_INTERNAL_THERM_LIMIT	0x20
/*N*///#define TXGBE_EMC_DIODE1_DATA		0x01
/*N*///#define TXGBE_EMC_DIODE1_THERM_LIMIT	0x19
/*N*///#define TXGBE_EMC_DIODE2_DATA		0x23
/*N*///#define TXGBE_EMC_DIODE2_THERM_LIMIT	0x1A

//#define TXGBE_MAX_SENSORS		3

struct txgbe_thermal_diode_data {
	//u8 location;
	s16 temp;
	//u8 caution_thresh;
	//u8 max_op_thresh;
	s16 alarm_thresh;
	s16 dalarm_thresh;
};

struct txgbe_thermal_sensor_data {
	struct txgbe_thermal_diode_data sensor[1];
};


/*N*///#define NVM_OROM_OFFSET		0x17
/*N*///#define NVM_OROM_BLK_LOW	0x83
/*N*///#define NVM_OROM_BLK_HI		0x84
/*N*///#define NVM_OROM_PATCH_MASK	0xFF
/*N*///#define NVM_OROM_SHIFT		8

/*N*///#define NVM_VER_MASK		0x00FF /* version mask */
/*N*///#define NVM_VER_SHIFT		8     /* version bit shift */
/*N*///#define NVM_OEM_PROD_VER_PTR	0x1B  /* OEM Product version block pointer */
/*N*///#define NVM_OEM_PROD_VER_CAP_OFF 0x1  /* OEM Product version format offset */
/*N*///#define NVM_OEM_PROD_VER_OFF_L	0x2   /* OEM Product version offset low */
/*N*///#define NVM_OEM_PROD_VER_OFF_H	0x3   /* OEM Product version offset high */
/*N*///#define NVM_OEM_PROD_VER_CAP_MASK 0xF /* OEM Product version cap mask */
/*N*///#define NVM_OEM_PROD_VER_MOD_LEN 0x3  /* OEM Product version module length */
/*N*///#define NVM_ETK_OFF_LOW		0x2D  /* version low order word */
/*N*///#define NVM_ETK_OFF_HI		0x2E  /* version high order word */
/*N*///#define NVM_ETK_SHIFT		16    /* high version word shift */
/*N*///#define NVM_VER_INVALID		0xFFFF
/*N*///#define NVM_ETK_VALID		0x8000
/*N*///#define NVM_INVALID_PTR		0xFFFF
/*N*///#define NVM_VER_SIZE		32    /* version sting size */

struct txgbe_nvm_version {
	u32 etk_id;
	u8  nvm_major;
	u16 nvm_minor;
	u8  nvm_id;

	bool oem_valid;
	u8   oem_major;
	u8   oem_minor;
	u16  oem_release;

	bool or_valid;
	u8  or_major;
	u16 or_build;
	u8  or_patch;
};

/* Interrupt Registers */
/*N*///#define TXGBE_EICR		0x00800
/*N*///#define TXGBE_EICS		0x00808
/*N*///#define TXGBE_EIMS		0x00880
/*N*///#define TXGBE_EIMC		0x00888
/*N*///#define TXGBE_EIAC		0x00810
/*N*///#define TXGBE_EIAM		0x00890
/*N*///#define TXGBE_EICS_EX(_i)	(0x00A90 + (_i) * 4)
/*N*///#define TXGBE_EIMS_EX(_i)	(0x00AA0 + (_i) * 4)
/*N*///#define TXGBE_EIMC_EX(_i)	(0x00AB0 + (_i) * 4)
/*N*///#define TXGBE_EIAM_EX(_i)	(0x00AD0 + (_i) * 4)
/* 82599 EITR is only 12 bits, with the lower 3 always zero */
/*
 * 82598 EITR is 16 bits but set the limits based on the max
 * supported by all txgbe hardware
 */
/*N*///#define TXGBE_MAX_INT_RATE	488281
/*N*///#define TXGBE_MIN_INT_RATE	956
/*N*///#define TXGBE_MAX_EITR		0x00000FF8
/*N*///#define TXGBE_MIN_EITR		8
/*N*//*#define TXGBE_EITR(_i)		(((_i) <= 23) ? (0x00820 + ((_i) * 4)) : \
				 (0x012300 + (((_i) - 24) * 4)))*/
/*N*///#define TXGBE_EITR_ITR_INT_MASK	0x00000FF8
/*N*///#define TXGBE_EITR_LLI_MOD	0x00008000
/*N*///#define TXGBE_EITR_CNT_WDIS	0x80000000
/*N*///#define TXGBE_IVAR(_i)		(0x00900 + ((_i) * 4)) /* 24 at 0x900-0x960 */
/*N*///#define TXGBE_IVAR_MISC		0x00A00 /* misc MSI-X interrupt causes */
/*N*///#define TXGBE_EITRSEL		0x00894
/*N*///#define TXGBE_MSIXT		0x00000 /* MSI-X Table. 0x0000 - 0x01C */
/*N*///#define TXGBE_MSIXPBA		0x02000 /* MSI-X Pending bit array */
/*N*///#define TXGBE_PBACL(_i)	(((_i) == 0) ? (0x11068) : (0x110C0 + ((_i) * 4)))
/*N*///#define TXGBE_GPIE		0x00898

/* Flow Control Registers */
/*N*///#define TXGBE_FCADBUL		0x03210
/*N*///#define TXGBE_FCADBUH		0x03214
/*N*///#define TXGBE_FCAMACL		0x04328
/*N*///#define TXGBE_FCAMACH		0x0432C
/*N*///#define TXGBE_FCRTH_82599(_i)	(0x03260 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_FCRTL_82599(_i)	(0x03220 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_PFCTOP		0x03008
/*N*///#define TXGBE_FCTTV(_i)		(0x03200 + ((_i) * 4)) /* 4 of these (0-3) */
/*N*///#define TXGBE_FCRTL(_i)		(0x03220 + ((_i) * 8)) /* 8 of these (0-7) */
/*N*///#define TXGBE_FCRTH(_i)		(0x03260 + ((_i) * 8)) /* 8 of these (0-7) */
/*N*///#define TXGBE_FCRTV		0x032A0
/*N*///#define TXGBE_FCCFG		0x03D00
/*N*///#define TXGBE_TFCS		0x0CE00

/* Receive DMA Registers */
/*N*//*#define TXGBE_RDLEN(_i)	(((_i) < 64) ? (0x01008 + ((_i) * 0x40)) : \
			 (0x0D008 + (((_i) - 64) * 0x40)))*/
/*N*//*#define TXGBE_RSCCTL(_i)	(((_i) < 64) ? (0x0102C + ((_i) * 0x40)) : \
				 (0x0D02C + (((_i) - 64) * 0x40)))*/
/*N*///#define TXGBE_RSCDBU	0x03028
/*N*///#define TXGBE_RDDCC	0x02F20
/*N*///#define TXGBE_RXMEMWRAP	0x03190
/*N*///#define TXGBE_STARCTRL	0x03024
/*
 * Split and Replication Receive Control Registers
 * 00-15 : 0x02100 + n*4
 * 16-64 : 0x01014 + n*0x40
 * 64-127: 0x0D014 + (n-64)*0x40
 */
/*N*//*#define TXGBE_SRRCTL(_i)	(((_i) <= 15) ? (0x02100 + ((_i) * 4)) : \
				 (((_i) < 64) ? (0x01014 + ((_i) * 0x40)) : \
				 (0x0D014 + (((_i) - 64) * 0x40))))*/
/*
 * Rx DCA Control Register:
 * 00-15 : 0x02200 + n*4
 * 16-64 : 0x0100C + n*0x40
 * 64-127: 0x0D00C + (n-64)*0x40
 */
/*N*//*#define TXGBE_DCA_RXCTRL(_i)	(((_i) <= 15) ? (0x02200 + ((_i) * 4)) : \
				 (((_i) < 64) ? (0x0100C + ((_i) * 0x40)) : \
				 (0x0D00C + (((_i) - 64) * 0x40))))*/
/*N*///#define TXGBE_RDRXCTL		0x02F00
/* 8 of these 0x03C00 - 0x03C1C */
/*N*///#define TXGBE_RXPBSIZE(_i)	(0x03C00 + ((_i) * 4))
/*N*///#define TXGBE_RXCTRL		0x03000
/*N*///#define TXGBE_DROPEN		0x03D04
/*N*///#define TXGBE_RXPBSIZE_SHIFT	10
/*N*///#define TXGBE_RXPBSIZE_MASK	0x000FFC00

/* Receive Registers */
/*N*///#define TXGBE_RXCSUM		0x05000
/*N*///#define TXGBE_RFCTL		0x05008
/*N*///#define TXGBE_DRECCCTL		0x02F08
/*N*///#define TXGBE_DRECCCTL_DISABLE	0
/*N*///#define TXGBE_DRECCCTL2		0x02F8C

/* Multicast Table Array - 128 entries */
/*N*///#define TXGBE_MTA(_i)		(0x05200 + ((_i) * 4))
/*N*//*#define TXGBE_RAL(_i)		(((_i) <= 15) ? (0x05400 + ((_i) * 8)) : \
				 (0x0A200 + ((_i) * 8)))*/
/*N*//*#define TXGBE_RAH(_i)		(((_i) <= 15) ? (0x05404 + ((_i) * 8)) : \
				 (0x0A204 + ((_i) * 8)))*/
/*N*///#define TXGBE_MPSAR_LO(_i)	(0x0A600 + ((_i) * 8))
/*N*///#define TXGBE_MPSAR_HI(_i)	(0x0A604 + ((_i) * 8))
/* Packet split receive type */
/*N*//*#define TXGBE_PSRTYPE(_i)	(((_i) <= 15) ? (0x05480 + ((_i) * 4)) : \
				 (0x0EA00 + ((_i) * 4)))*/
/* array of 4096 1-bit vlan filters */
/*N*///#define TXGBE_VFTA(_i)		(0x0A000 + ((_i) * 4))
/*array of 4096 4-bit vlan vmdq indices */
/*N*///#define TXGBE_VFTAVIND(_j, _i)	(0x0A200 + ((_j) * 0x200) + ((_i) * 4))
/*N*///#define TXGBE_FCTRL		0x05080
/*N*///#define TXGBE_VLNCTRL		0x05088
/*N*///#define TXGBE_MCSTCTRL		0x05090
/*N*///#define TXGBE_MRQC		0x05818
/*N*///#define TXGBE_SAQF(_i)	(0x0E000 + ((_i) * 4)) /* Source Address Queue Filter */
/*N*///#define TXGBE_DAQF(_i)	(0x0E200 + ((_i) * 4)) /* Dest. Address Queue Filter */
/*N*///#define TXGBE_SDPQF(_i)	(0x0E400 + ((_i) * 4)) /* Src Dest. Addr Queue Filter */
/*N*///#define TXGBE_FTQF(_i)	(0x0E600 + ((_i) * 4)) /* Five Tuple Queue Filter */
/*N*///#define TXGBE_ETQF(_i)	(0x05128 + ((_i) * 4)) /* EType Queue Filter */
/*N*///#define TXGBE_ETQS(_i)	(0x0EC00 + ((_i) * 4)) /* EType Queue Select */
/*N*///#define TXGBE_SYNQF	0x0EC30 /* SYN Packet Queue Filter */
/*N*///#define TXGBE_RQTC	0x0EC70
/*N*///#define TXGBE_MTQC	0x08120
/*N*///#define TXGBE_VLVF(_i)	(0x0F100 + ((_i) * 4))  /* 64 of these (0-63) */
/*N*///#define TXGBE_VLVFB(_i)	(0x0F200 + ((_i) * 4))  /* 128 of these (0-127) */
/*N*///#define TXGBE_VMVIR(_i)	(0x08000 + ((_i) * 4))  /* 64 of these (0-63) */
/*N*///#define TXGBE_PFFLPL		0x050B0
/*N*///#define TXGBE_PFFLPH		0x050B4
/*N*///#define TXGBE_VT_CTL		0x051B0
/*N*///#define TXGBE_PFMAILBOX(_i)	(0x04B00 + (4 * (_i))) /* 64 total */
/* 64 Mailboxes, 16 DW each */
/*N*///#define TXGBE_PFMBMEM(_i)	(0x13000 + (64 * (_i)))
/*N*///#define TXGBE_PFMBICR(_i)	(0x00710 + (4 * (_i))) /* 4 total */
/*N*///#define TXGBE_PFMBIMR(_i)	(0x00720 + (4 * (_i))) /* 4 total */
/*N*///#define TXGBE_VFRE(_i)		(0x051E0 + ((_i) * 4))
/*N*///#define TXGBE_VFTE(_i)		(0x08110 + ((_i) * 4))
/*N*///#define TXGBE_VMECM(_i)		(0x08790 + ((_i) * 4))
/*N*///#define TXGBE_QDE		0x2F04
/*N*///#define TXGBE_VMTXSW(_i)	(0x05180 + ((_i) * 4)) /* 2 total */
/*N*///#define TXGBE_VMOLR(_i)		(0x0F000 + ((_i) * 4)) /* 64 total */
/*N*///#define TXGBE_UTA(_i)		(0x0F400 + ((_i) * 4))
/*N*///#define TXGBE_MRCTL(_i)		(0x0F600 + ((_i) * 4))
/*N*///#define TXGBE_VMRVLAN(_i)	(0x0F610 + ((_i) * 4))
/*N*///#define TXGBE_VMRVM(_i)		(0x0F630 + ((_i) * 4))
/*N*///#define TXGBE_LVMMC_RX		0x2FA8
/*N*///#define TXGBE_LVMMC_TX		0x8108
/*N*///#define TXGBE_LMVM_RX		0x2FA4
/*N*///#define TXGBE_LMVM_TX		0x8124
/*N*///#define TXGBE_WQBR_RX(_i)	(0x2FB0 + ((_i) * 4)) /* 4 total */
/*N*///#define TXGBE_WQBR_TX(_i)	(0x8130 + ((_i) * 4)) /* 4 total */
/*N*///#define TXGBE_L34T_IMIR(_i)	(0x0E800 + ((_i) * 4)) /*128 of these (0-127)*/
/*N*///#define TXGBE_RXFECCERR0	0x051B8
/*N*///#define TXGBE_LLITHRESH		0x0EC90
/*N*///#define TXGBE_IMIR(_i)		(0x05A80 + ((_i) * 4))  /* 8 of these (0-7) */
/*N*///#define TXGBE_IMIREXT(_i)	(0x05AA0 + ((_i) * 4))  /* 8 of these (0-7) */
/*N*///#define TXGBE_IMIRVP		0x05AC0
/*N*///#define TXGBE_VMD_CTL		0x0581C
/*N*///#define TXGBE_RETA(_i)		(0x05C00 + ((_i) * 4))  /* 32 of these (0-31) */
/*N*///#define TXGBE_ERETA(_i)		(0x0EE80 + ((_i) * 4))  /* 96 of these (0-95) */
/*N*///#define TXGBE_RSSRK(_i)		(0x05C80 + ((_i) * 4))  /* 10 of these (0-9) */

/* Registers for setting up RSS on X550 with SRIOV
 * _p - pool number (0..63)
 * _i - index (0..10 for PFVFRSSRK, 0..15 for PFVFRETA)
 */
/*N*///#define TXGBE_PFVFMRQC(_p)	(0x03400 + ((_p) * 4))
/*N*///#define TXGBE_PFVFRSSRK(_i, _p)	(0x018000 + ((_i) * 4) + ((_p) * 0x40))
/*N*///#define TXGBE_PFVFRETA(_i, _p)	(0x019000 + ((_i) * 4) + ((_p) * 0x40))

/* Flow Director registers */
/*N*///#define TXGBE_FDIRCTRL	0x0EE00
/*N*///#define TXGBE_FDIRHKEY	0x0EE68
/*N*///#define TXGBE_FDIRSKEY	0x0EE6C
/*N*///#define TXGBE_FDIRDIP4M	0x0EE3C
/*N*///#define TXGBE_FDIRSIP4M	0x0EE40
/*N*///#define TXGBE_FDIRTCPM	0x0EE44
/*N*///#define TXGBE_FDIRUDPM	0x0EE48
/*N*///#define TXGBE_FDIRSCTPM	0x0EE78
/*N*///#define TXGBE_FDIRIP6M	0x0EE74
/*N*///#define TXGBE_FDIRM	0x0EE70

/* Flow Director Stats registers */
/*N*///#define TXGBE_FDIRFREE	0x0EE38
/*N*///#define TXGBE_FDIRLEN	0x0EE4C
/*N*///#define TXGBE_FDIRUSTAT	0x0EE50
/*N*///#define TXGBE_FDIRFSTAT	0x0EE54
/*N*///#define TXGBE_FDIRMATCH	0x0EE58
/*N*///#define TXGBE_FDIRMISS	0x0EE5C

/* Flow Director Programming registers */
/*N*///#define TXGBE_FDIRSIPv6(_i) (0x0EE0C + ((_i) * 4)) /* 3 of these (0-2) */
/*N*///#define TXGBE_FDIRIPSA	0x0EE18
/*N*///#define TXGBE_FDIRIPDA	0x0EE1C
/*N*///#define TXGBE_FDIRPORT	0x0EE20
/*N*///#define TXGBE_FDIRVLAN	0x0EE24
/*N*///#define TXGBE_FDIRHASH	0x0EE28
/*N*///#define TXGBE_FDIRCMD	0x0EE2C

/* Transmit DMA registers */
/*N*///#define TXGBE_TDBAL(_i)		(0x06000 + ((_i) * 0x40)) /* 32 of them (0-31)*/
/*N*///#define TXGBE_TDBAH(_i)		(0x06004 + ((_i) * 0x40))
/*N*///#define TXGBE_TDLEN(_i)		(0x06008 + ((_i) * 0x40))
/*N*///#define TXGBE_TDH(_i)		(0x06010 + ((_i) * 0x40))
/*N*///#define TXGBE_TDT(_i)		(0x06018 + ((_i) * 0x40))
/*N*///#define TXGBE_TXDCTL(_i)	(0x06028 + ((_i) * 0x40))
/*N*///#define TXGBE_TDWBAL(_i)	(0x06038 + ((_i) * 0x40))
/*N*///#define TXGBE_TDWBAH(_i)	(0x0603C + ((_i) * 0x40))
/*N*///#define TXGBE_DTXCTL		0x07E00

/*N*///#define TXGBE_DMATXCTL		0x04A80
/*N*///#define TXGBE_PFVFSPOOF(_i)	(0x08200 + ((_i) * 4)) /* 8 of these 0 - 7 */
/*N*///#define TXGBE_PFDTXGSWC		0x08220
/*N*///#define TXGBE_DTXMXSZRQ		0x08100
/*N*///#define TXGBE_DTXTCPFLGL	0x04A88
/*N*///#define TXGBE_DTXTCPFLGH	0x04A8C
/*N*///#define TXGBE_LBDRPEN		0x0CA00
/*N*///#define TXGBE_TXPBTHRESH(_i)	(0x04950 + ((_i) * 4)) /* 8 of these 0 - 7 */

/*N*///#define TXGBE_DMATXCTL_TE	0x1 /* Transmit Enable */
/*N*///#define TXGBE_DMATXCTL_NS	0x2 /* No Snoop LSO hdr buffer */
/*N*///#define TXGBE_DMATXCTL_GDV	0x8 /* Global Double VLAN */
/*N*///#define TXGBE_DMATXCTL_MDP_EN	0x20 /* Bit 5 */
/*N*///#define TXGBE_DMATXCTL_MBINTEN	0x40 /* Bit 6 */
/*N*///#define TXGBE_DMATXCTL_VT_SHIFT	16  /* VLAN EtherType */

/*N*///#define TXGBE_PFDTXGSWC_VT_LBEN	0x1 /* Local L2 VT switch enable */

/* Anti-spoofing defines */
/*N*///#define TXGBE_SPOOF_MACAS_MASK		0xFF
/*N*///#define TXGBE_SPOOF_VLANAS_MASK		0xFF00
/*N*///#define TXGBE_SPOOF_VLANAS_SHIFT	8
/*N*///#define TXGBE_SPOOF_ETHERTYPEAS		0xFF000000
/*N*///#define TXGBE_SPOOF_ETHERTYPEAS_SHIFT	16
/*N*///#define TXGBE_PFVFSPOOF_REG_COUNT	8
/* 16 of these (0-15) */
/*N*///#define TXGBE_DCA_TXCTRL(_i)		(0x07200 + ((_i) * 4))
/* Tx DCA Control register : 128 of these (0-127) */
/*N*///#define TXGBE_DCA_TXCTRL_82599(_i)	(0x0600C + ((_i) * 0x40))
/*N*///#define TXGBE_TIPG			0x0CB00
/*N*///#define TXGBE_TXPBSIZE(_i)		(0x0CC00 + ((_i) * 4)) /* 8 of these */
/*N*///#define TXGBE_MNGTXMAP			0x0CD10
/*N*///#define TXGBE_TIPG_FIBER_DEFAULT	3
/*N*///#define TXGBE_TXPBSIZE_SHIFT		10

/* Wake up registers */
/*N*///#define TXGBE_WUC	0x05800
/*N*///#define TXGBE_WUFC	0x05808
/*N*///#define TXGBE_WUS	0x05810
/*N*///#define TXGBE_IPAV	0x05838
/*N*///#define TXGBE_IP4AT	0x05840 /* IPv4 table 0x5840-0x5858 */
/*N*///#define TXGBE_IP6AT	0x05880 /* IPv6 table 0x5880-0x588F */

/*N*///#define TXGBE_WUPL	0x05900
/*N*///#define TXGBE_WUPM	0x05A00 /* wake up pkt memory 0x5A00-0x5A7C */
/*N*///#define TXGBE_PROXYS	0x05F60 /* Proxying Status Register */
/*N*///#define TXGBE_PROXYFC	0x05F64 /* Proxying Filter Control Register */
/*N*///#define TXGBE_VXLANCTRL	0x0000507C /* Rx filter VXLAN UDPPORT Register */

/* masks for accessing VXLAN and GENEVE UDP ports */
/*N*///#define TXGBE_VXLANCTRL_VXLAN_UDPPORT_MASK	0x0000ffff /* VXLAN port */
/*N*///#define TXGBE_VXLANCTRL_GENEVE_UDPPORT_MASK	0xffff0000 /* GENEVE port */
/*N*///#define TXGBE_VXLANCTRL_ALL_UDPPORT_MASK	0xffffffff /* GENEVE/VXLAN */
/*N*///#define TXGBE_VXLANCTRL_GENEVE_UDPPORT_SHIFT	16

/*N*///#define TXGBE_FHFT(_n)	(0x09000 + ((_n) * 0x100)) /* Flex host filter table */
/* Ext Flexible Host Filter Table */
/*N*///#define TXGBE_FHFT_EXT(_n)	(0x09800 + ((_n) * 0x100))
/*N*///#define TXGBE_FHFT_EXT_X550(_n)	(0x09600 + ((_n) * 0x100))

/* Four Flexible Filters are supported */
/*N*///#define TXGBE_FLEXIBLE_FILTER_COUNT_MAX		4
/* Six Flexible Filters are supported */
/*N*///#define TXGBE_FLEXIBLE_FILTER_COUNT_MAX_6	6
/* Eight Flexible Filters are supported */
/*N*///#define TXGBE_FLEXIBLE_FILTER_COUNT_MAX_8	8
/*N*///#define TXGBE_EXT_FLEXIBLE_FILTER_COUNT_MAX	2

/* Each Flexible Filter is at most 128 (0x80) bytes in length */
/*N*///#define TXGBE_FLEXIBLE_FILTER_SIZE_MAX		128
/*N*///#define TXGBE_FHFT_LENGTH_OFFSET		0xFC  /* Length byte in FHFT */
/*N*///#define TXGBE_FHFT_LENGTH_MASK			0x0FF /* Length in lower byte */

/* Definitions for power management and wakeup registers */
/* Wake Up Control */
/*N*///#define TXGBE_WUC_PME_EN	0x00000002 /* PME Enable */
/*N*///#define TXGBE_WUC_PME_STATUS	0x00000004 /* PME Status */
/*N*///#define TXGBE_WUC_WKEN		0x00000010 /* Enable PE_WAKE_N pin assertion  */

/* Wake Up Filter Control */
/*N*///#define TXGBE_WUFC_LNKC	0x00000001 /* Link Status Change Wakeup Enable */
/*N*///#define TXGBE_WUFC_MAG	0x00000002 /* Magic Packet Wakeup Enable */
/*N*///#define TXGBE_WUFC_EX	0x00000004 /* Directed Exact Wakeup Enable */
/*N*///#define TXGBE_WUFC_MC	0x00000008 /* Directed Multicast Wakeup Enable */
/*N*///#define TXGBE_WUFC_BC	0x00000010 /* Broadcast Wakeup Enable */
/*N*///#define TXGBE_WUFC_ARP	0x00000020 /* ARP Request Packet Wakeup Enable */
/*N*///#define TXGBE_WUFC_IPV4	0x00000040 /* Directed IPv4 Packet Wakeup Enable */
/*N*///#define TXGBE_WUFC_IPV6	0x00000080 /* Directed IPv6 Packet Wakeup Enable */
/*N*///#define TXGBE_WUFC_MNG	0x00000100 /* Directed Mgmt Packet Wakeup Enable */

/*N*///#define TXGBE_WUFC_IGNORE_TCO	0x00008000 /* Ignore WakeOn TCO packets */
/*N*///#define TXGBE_WUFC_FLX0	0x00010000 /* Flexible Filter 0 Enable */
/*N*///#define TXGBE_WUFC_FLX1	0x00020000 /* Flexible Filter 1 Enable */
/*N*///#define TXGBE_WUFC_FLX2	0x00040000 /* Flexible Filter 2 Enable */
/*N*///#define TXGBE_WUFC_FLX3	0x00080000 /* Flexible Filter 3 Enable */
/*N*///#define TXGBE_WUFC_FLX4	0x00100000 /* Flexible Filter 4 Enable */
/*N*///#define TXGBE_WUFC_FLX5	0x00200000 /* Flexible Filter 5 Enable */
/*N*///#define TXGBE_WUFC_FLX_FILTERS		0x000F0000 /* Mask for 4 flex filters */
/*N*///#define TXGBE_WUFC_FLX_FILTERS_6	0x003F0000 /* Mask for 6 flex filters */
/*N*///#define TXGBE_WUFC_FLX_FILTERS_8	0x00FF0000 /* Mask for 8 flex filters */
/*N*///#define TXGBE_WUFC_FW_RST_WK	0x80000000 /* Ena wake on FW reset assertion */
/* Mask for Ext. flex filters */
/*N*///#define TXGBE_WUFC_EXT_FLX_FILTERS	0x00300000
/*N*///#define TXGBE_WUFC_ALL_FILTERS		0x000F00FF /* Mask all 4 flex filters */
/*N*///#define TXGBE_WUFC_ALL_FILTERS_6	0x003F00FF /* Mask all 6 flex filters */
/*N*///#define TXGBE_WUFC_ALL_FILTERS_8	0x00FF00FF /* Mask all 8 flex filters */
/*N*///#define TXGBE_WUFC_FLX_OFFSET	16 /* Offset to the Flexible Filters bits */

/* Wake Up Status */
/*N*///#define TXGBE_WUS_LNKC		TXGBE_WUFC_LNKC
/*N*///#define TXGBE_WUS_MAG		TXGBE_WUFC_MAG
/*N*///#define TXGBE_WUS_EX		TXGBE_WUFC_EX
/*N*///#define TXGBE_WUS_MC		TXGBE_WUFC_MC
/*N*///#define TXGBE_WUS_BC		TXGBE_WUFC_BC
/*N*///#define TXGBE_WUS_ARP		TXGBE_WUFC_ARP
/*N*///#define TXGBE_WUS_IPV4		TXGBE_WUFC_IPV4
/*N*///#define TXGBE_WUS_IPV6		TXGBE_WUFC_IPV6
/*N*///#define TXGBE_WUS_MNG		TXGBE_WUFC_MNG
/*N*///#define TXGBE_WUS_FLX0		TXGBE_WUFC_FLX0
/*N*///#define TXGBE_WUS_FLX1		TXGBE_WUFC_FLX1
/*N*///#define TXGBE_WUS_FLX2		TXGBE_WUFC_FLX2
/*N*///#define TXGBE_WUS_FLX3		TXGBE_WUFC_FLX3
/*N*///#define TXGBE_WUS_FLX4		TXGBE_WUFC_FLX4
/*N*///#define TXGBE_WUS_FLX5		TXGBE_WUFC_FLX5
/*N*///#define TXGBE_WUS_FLX_FILTERS	TXGBE_WUFC_FLX_FILTERS
/*N*///#define TXGBE_WUS_FW_RST_WK	TXGBE_WUFC_FW_RST_WK
/* Proxy Status */
/*N*///#define TXGBE_PROXYS_EX		0x00000004 /* Exact packet received */
/*N*///#define TXGBE_PROXYS_ARP_DIR	0x00000020 /* ARP w/filter match received */
/*N*///#define TXGBE_PROXYS_NS		0x00000200 /* IPV6 NS received */
/*N*///#define TXGBE_PROXYS_NS_DIR	0x00000400 /* IPV6 NS w/DA match received */
/*N*///#define TXGBE_PROXYS_ARP	0x00000800 /* ARP request packet received */
/*N*///#define TXGBE_PROXYS_MLD	0x00001000 /* IPv6 MLD packet received */

/* Proxying Filter Control */
/*N*///#define TXGBE_PROXYFC_ENABLE	0x00000001 /* Port Proxying Enable */
/*N*///#define TXGBE_PROXYFC_EX	0x00000004 /* Directed Exact Proxy Enable */
/*N*///#define TXGBE_PROXYFC_ARP_DIR	0x00000020 /* Directed ARP Proxy Enable */
/*N*///#define TXGBE_PROXYFC_NS	0x00000200 /* IPv6 Neighbor Solicitation */
/*N*///#define TXGBE_PROXYFC_ARP	0x00000800 /* ARP Request Proxy Enable */
/*N*///#define TXGBE_PROXYFC_MLD	0x00000800 /* IPv6 MLD Proxy Enable */
/*N*///#define TXGBE_PROXYFC_NO_TCO	0x00008000 /* Ignore TCO packets */

/*N*///#define TXGBE_WUPL_LENGTH_MASK	0xFFFF

/* DCB registers */
/*N*///#define TXGBE_MAX_TC	8
/*N*///#define TXGBE_RMCS		0x03D00
/*N*///#define TXGBE_DPMCS		0x07F40
/*N*///#define TXGBE_PDPMCS		0x0CD00
/*N*///#define TXGBE_RUPPBMR		0x050A0
/*N*///#define TXGBE_RT2CR(_i)		(0x03C20 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_RT2SR(_i)		(0x03C40 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_TDTQ2TCCR(_i)	(0x0602C + ((_i) * 0x40)) /* 8 of these (0-7) */
/*N*///#define TXGBE_TDTQ2TCSR(_i)	(0x0622C + ((_i) * 0x40)) /* 8 of these (0-7) */
/*N*///#define TXGBE_TDPT2TCCR(_i)	(0x0CD20 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_TDPT2TCSR(_i)	(0x0CD40 + ((_i) * 4)) /* 8 of these (0-7) */

/* Power Management */
/* DMA Coalescing configuration */
struct txgbe_dmac_config {
	u16	watchdog_timer; /* usec units */
	bool	fcoe_en;
	u32	link_speed;
	u8	fcoe_tc;
	u8	num_tcs;
};

/*
 * DMA Coalescing threshold Rx PB TC[n] value in Kilobyte by link speed.
 * DMACRXT = 10Gbps = 10,000 bits / usec = 1250 bytes / usec 70 * 1250 ==
 * 87500 bytes [85KB]
 */
/*N*///#define TXGBE_DMACRXT_10G		0x55
/*N*///#define TXGBE_DMACRXT_1G		0x09
/*N*///#define TXGBE_DMACRXT_100M		0x01

/* DMA Coalescing registers */
/*N*///#define TXGBE_DMCMNGTH			0x15F20 /* Management Threshold */
/*N*///#define TXGBE_DMACR			0x02400 /* Control register */
/*N*///#define TXGBE_DMCTH(_i)			(0x03300 + ((_i) * 4)) /* 8 of these */
/*N*///#define TXGBE_DMCTLX			0x02404 /* Time to Lx request */
/* DMA Coalescing register fields */
/*N*///#define TXGBE_DMCMNGTH_DMCMNGTH_MASK	0x000FFFF0 /* Mng Threshold mask */
/*N*///#define TXGBE_DMCMNGTH_DMCMNGTH_SHIFT	4 /* Management Threshold shift */
/*N*///#define TXGBE_DMACR_DMACWT_MASK		0x0000FFFF /* Watchdog Timer mask */
/*N*///#define TXGBE_DMACR_HIGH_PRI_TC_MASK	0x00FF0000
/*N*///#define TXGBE_DMACR_HIGH_PRI_TC_SHIFT	16
/*N*///#define TXGBE_DMACR_EN_MNG_IND		0x10000000 /* Enable Mng Indications */
/*N*///#define TXGBE_DMACR_LX_COAL_IND		0x40000000 /* Lx Coalescing indicate */
/*N*///#define TXGBE_DMACR_DMAC_EN		0x80000000 /* DMA Coalescing Enable */
/*N*///#define TXGBE_DMCTH_DMACRXT_MASK	0x000001FF /* Receive Threshold mask */
/*N*///#define TXGBE_DMCTLX_TTLX_MASK		0x00000FFF /* Time to Lx request mask */

/* EEE registers */
/*N*///#define TXGBE_EEER			0x043A0 /* EEE register */
/*N*///#define TXGBE_EEE_STAT			0x04398 /* EEE Status */
/*N*///#define TXGBE_EEE_SU			0x04380 /* EEE Set up */
/*N*///#define TXGBE_EEE_SU_TEEE_DLY_SHIFT	26
/*N*///#define TXGBE_TLPIC			0x041F4 /* EEE Tx LPI count */
/*N*///#define TXGBE_RLPIC			0x041F8 /* EEE Rx LPI count */

/* EEE register fields */
/*N*///#define TXGBE_EEER_TX_LPI_EN		0x00010000 /* Enable EEE LPI TX path */
/*N*///#define TXGBE_EEER_RX_LPI_EN		0x00020000 /* Enable EEE LPI RX path */
/*N*///#define TXGBE_EEE_STAT_NEG		0x20000000 /* EEE support neg on link */
/*N*///#define TXGBE_EEE_RX_LPI_STATUS		0x40000000 /* RX Link in LPI status */
/*N*///#define TXGBE_EEE_TX_LPI_STATUS		0x80000000 /* TX Link in LPI status */

/* Security Control Registers */
/*N*///#define TXGBE_SECTXCTRL		0x08800
/*N*///#define TXGBE_SECTXSTAT		0x08804
/*N*///#define TXGBE_SECTXBUFFAF	0x08808
/*N*///#define TXGBE_SECTXMINIFG	0x08810
/*N*///#define TXGBE_SECRXCTRL		0x08D00
/*N*///#define TXGBE_SECRXSTAT		0x08D04

/* Security Bit Fields and Masks */
/*N*///#define TXGBE_SECTXCTRL_SECTX_DIS	0x00000001
/*N*///#define TXGBE_SECTXCTRL_TX_DIS		0x00000002
/*N*///#define TXGBE_SECTXCTRL_STORE_FORWARD	0x00000004

/*N*///#define TXGBE_SECTXSTAT_SECTX_RDY	0x00000001
/*N*///#define TXGBE_SECTXSTAT_ECC_TXERR	0x00000002

/*N*///#define TXGBE_SECRXCTRL_SECRX_DIS	0x00000001
/*N*///#define TXGBE_SECRXCTRL_RX_DIS		0x00000002

/*N*///#define TXGBE_SECRXSTAT_SECRX_RDY	0x00000001
/*N*///#define TXGBE_SECRXSTAT_ECC_RXERR	0x00000002

/* LinkSec (MacSec) Registers */
/*N*///#define TXGBE_LSECTXCAP		0x08A00
/*N*///#define TXGBE_LSECRXCAP		0x08F00
/*N*///#define TXGBE_LSECTXCTRL	0x08A04
/*N*///#define TXGBE_LSECTXSCL		0x08A08 /* SCI Low */
/*N*///#define TXGBE_LSECTXSCH		0x08A0C /* SCI High */
/*N*///#define TXGBE_LSECTXSA		0x08A10
/*N*///#define TXGBE_LSECTXPN0		0x08A14
/*N*///#define TXGBE_LSECTXPN1		0x08A18
/*N*///#define TXGBE_LSECTXKEY0(_n)	(0x08A1C + (4 * (_n))) /* 4 of these (0-3) */
/*N*///#define TXGBE_LSECTXKEY1(_n)	(0x08A2C + (4 * (_n))) /* 4 of these (0-3) */
/*N*///#define TXGBE_LSECRXCTRL	0x08F04
/*N*///#define TXGBE_LSECRXSCL		0x08F08
/*N*///#define TXGBE_LSECRXSCH		0x08F0C
/*N*///#define TXGBE_LSECRXSA(_i)	(0x08F10 + (4 * (_i))) /* 2 of these (0-1) */
/*N*///#define TXGBE_LSECRXPN(_i)	(0x08F18 + (4 * (_i))) /* 2 of these (0-1) */
/*N*///#define TXGBE_LSECRXKEY(_n, _m)	(0x08F20 + ((0x10 * (_n)) + (4 * (_m))))
/*N*///#define TXGBE_LSECTXUT		0x08A3C /* OutPktsUntagged */
/*N*///#define TXGBE_LSECTXPKTE	0x08A40 /* OutPktsEncrypted */
/*N*///#define TXGBE_LSECTXPKTP	0x08A44 /* OutPktsProtected */
/*N*///#define TXGBE_LSECTXOCTE	0x08A48 /* OutOctetsEncrypted */
/*N*///#define TXGBE_LSECTXOCTP	0x08A4C /* OutOctetsProtected */
/*N*///#define TXGBE_LSECRXUT		0x08F40 /* InPktsUntagged/InPktsNoTag */
/*N*///#define TXGBE_LSECRXOCTD	0x08F44 /* InOctetsDecrypted */
/*N*///#define TXGBE_LSECRXOCTV	0x08F48 /* InOctetsValidated */
/*N*///#define TXGBE_LSECRXBAD		0x08F4C /* InPktsBadTag */
/*N*///#define TXGBE_LSECRXNOSCI	0x08F50 /* InPktsNoSci */
/*N*///#define TXGBE_LSECRXUNSCI	0x08F54 /* InPktsUnknownSci */
/*N*///#define TXGBE_LSECRXUNCH	0x08F58 /* InPktsUnchecked */
/*N*///#define TXGBE_LSECRXDELAY	0x08F5C /* InPktsDelayed */
/*N*///#define TXGBE_LSECRXLATE	0x08F60 /* InPktsLate */
/*N*///#define TXGBE_LSECRXOK(_n)	(0x08F64 + (0x04 * (_n))) /* InPktsOk */
/*N*///#define TXGBE_LSECRXINV(_n)	(0x08F6C + (0x04 * (_n))) /* InPktsInvalid */
/*N*///#define TXGBE_LSECRXNV(_n)	(0x08F74 + (0x04 * (_n))) /* InPktsNotValid */
/*N*///#define TXGBE_LSECRXUNSA	0x08F7C /* InPktsUnusedSa */
/*N*///#define TXGBE_LSECRXNUSA	0x08F80 /* InPktsNotUsingSa */

/* LinkSec (MacSec) Bit Fields and Masks */
/*N*///#define TXGBE_LSECTXCAP_SUM_MASK	0x00FF0000
/*N*///#define TXGBE_LSECTXCAP_SUM_SHIFT	16
/*N*///#define TXGBE_LSECRXCAP_SUM_MASK	0x00FF0000
/*N*///#define TXGBE_LSECRXCAP_SUM_SHIFT	16

/*N*///#define TXGBE_LSECTXCTRL_EN_MASK	0x00000003
/*N*///#define TXGBE_LSECTXCTRL_DISABLE	0x0
/*N*///#define TXGBE_LSECTXCTRL_AUTH		0x1
/*N*///#define TXGBE_LSECTXCTRL_AUTH_ENCRYPT	0x2
/*N*///#define TXGBE_LSECTXCTRL_AISCI		0x00000020
/*N*///#define TXGBE_LSECTXCTRL_PNTHRSH_MASK	0xFFFFFF00
/*N*///#define TXGBE_LSECTXCTRL_RSV_MASK	0x000000D8

/*N*///#define TXGBE_LSECRXCTRL_EN_MASK	0x0000000C
/*N*///#define TXGBE_LSECRXCTRL_EN_SHIFT	2
/*N*///#define TXGBE_LSECRXCTRL_DISABLE	0x0
/*N*///#define TXGBE_LSECRXCTRL_CHECK		0x1
/*N*///#define TXGBE_LSECRXCTRL_STRICT		0x2
/*N*///#define TXGBE_LSECRXCTRL_DROP		0x3
/*N*///#define TXGBE_LSECRXCTRL_PLSH		0x00000040
/*N*///#define TXGBE_LSECRXCTRL_RP		0x00000080
/*N*///#define TXGBE_LSECRXCTRL_RSV_MASK	0xFFFFFF33

/* IpSec Registers */
/*N*///#define TXGBE_IPSTXIDX		0x08900
/*N*///#define TXGBE_IPSTXSALT		0x08904
/*N*///#define TXGBE_IPSTXKEY(_i)	(0x08908 + (4 * (_i))) /* 4 of these (0-3) */
/*N*///#define TXGBE_IPSRXIDX		0x08E00
/*N*///#define TXGBE_IPSRXIPADDR(_i)	(0x08E04 + (4 * (_i))) /* 4 of these (0-3) */
/*N*///#define TXGBE_IPSRXSPI		0x08E14
/*N*///#define TXGBE_IPSRXIPIDX	0x08E18
/*N*///#define TXGBE_IPSRXKEY(_i)	(0x08E1C + (4 * (_i))) /* 4 of these (0-3) */
/*N*///#define TXGBE_IPSRXSALT		0x08E2C
/*N*///#define TXGBE_IPSRXMOD		0x08E30

/*N*///#define TXGBE_SECTXCTRL_STORE_FORWARD_ENABLE	0x4

/* DCB registers */
/*N*///#define TXGBE_RTRPCS		0x02430
/*N*///#define TXGBE_RTTDCS		0x04900
/*N*///#define TXGBE_ARBTXCTL_DSA	0x00000040 /* DCB arbiter disable */
/*N*///#define TXGBE_RTTPCS		0x0CD00
/*N*///#define TXGBE_RTRUP2TC		0x03020
/*N*///#define TXGBE_RTTUP2TC		0x0C800
/*N*///#define TXGBE_RTRPT4C(_i)	(0x02140 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_TXLLQ(_i)		(0x082E0 + ((_i) * 4)) /* 4 of these (0-3) */
/*N*///#define TXGBE_RTRPT4S(_i)	(0x02160 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_RTTDT2C(_i)	(0x04910 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_RTTDT2S(_i)	(0x04930 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_RTTPT2C(_i)	(0x0CD20 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_RTTPT2S(_i)	(0x0CD40 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_RTTDQSEL		0x04904
/*N*///#define TXGBE_RTTDT1C		0x04908
/*N*///#define TXGBE_RTTDT1S		0x0490C
/*N*///#define TXGBE_RTTDTECC		0x04990
/*N*///#define TXGBE_RTTDTECC_NO_BCN	0x00000100

/*N*///#define TXGBE_RTTBCNRC			0x04984
/*N*///#define TXGBE_RTTBCNRC_RS_ENA		0x80000000
/*N*///#define TXGBE_RTTBCNRC_RF_DEC_MASK	0x00003FFF
/*N*///#define TXGBE_RTTBCNRC_RF_INT_SHIFT	14
/*N*//*#define TXGBE_RTTBCNRC_RF_INT_MASK \
	(TXGBE_RTTBCNRC_RF_DEC_MASK << TXGBE_RTTBCNRC_RF_INT_SHIFT)*/
/*N*///#define TXGBE_RTTBCNRM	0x04980

/* BCN (for DCB) Registers */
/*N*///#define TXGBE_RTTBCNRS	0x04988
/*N*///#define TXGBE_RTTBCNCR	0x08B00
/*N*///#define TXGBE_RTTBCNACH	0x08B04
/*N*///#define TXGBE_RTTBCNACL	0x08B08
/*N*///#define TXGBE_RTTBCNTG	0x04A90
/*N*///#define TXGBE_RTTBCNIDX	0x08B0C
/*N*///#define TXGBE_RTTBCNCP	0x08B10
/*N*///#define TXGBE_RTFRTIMER	0x08B14
/*N*///#define TXGBE_RTTBCNRTT	0x05150
/*N*///#define TXGBE_RTTBCNRD	0x0498C

/* FCoE DMA Context Registers */
/* FCoE Direct DMA Context */
/*N*///#define TXGBE_FCDDC(_i, _j)	(0x20000 + ((_i) * 0x4) + ((_j) * 0x10))
/*N*///#define TXGBE_FCPTRL		0x02410 /* FC User Desc. PTR Low */
/*N*///#define TXGBE_FCPTRH		0x02414 /* FC USer Desc. PTR High */
/*N*///#define TXGBE_FCBUFF		0x02418 /* FC Buffer Control */
/*N*///#define TXGBE_FCDMARW		0x02420 /* FC Receive DMA RW */
/*N*///#define TXGBE_FCBUFF_VALID	(1 << 0)   /* DMA Context Valid */
/*N*///#define TXGBE_FCBUFF_BUFFSIZE	(3 << 3)   /* User Buffer Size */
/*N*///#define TXGBE_FCBUFF_WRCONTX	(1 << 7)   /* 0: Initiator, 1: Target */
/*N*///#define TXGBE_FCBUFF_BUFFCNT	0x0000ff00 /* Number of User Buffers */
/*N*///#define TXGBE_FCBUFF_OFFSET	0xffff0000 /* User Buffer Offset */
/*N*///#define TXGBE_FCBUFF_BUFFSIZE_SHIFT	3
/*N*///#define TXGBE_FCBUFF_BUFFCNT_SHIFT	8
/*N*///#define TXGBE_FCBUFF_OFFSET_SHIFT	16
/*N*///#define TXGBE_FCDMARW_WE		(1 << 14)   /* Write enable */
/*N*///#define TXGBE_FCDMARW_RE		(1 << 15)   /* Read enable */
/*N*///#define TXGBE_FCDMARW_FCOESEL		0x000001ff  /* FC X_ID: 11 bits */
/*N*///#define TXGBE_FCDMARW_LASTSIZE		0xffff0000  /* Last User Buffer Size */
/*N*///#define TXGBE_FCDMARW_LASTSIZE_SHIFT	16
/* FCoE SOF/EOF */
/*N*///#define TXGBE_TEOFF		0x04A94 /* Tx FC EOF */
/*N*///#define TXGBE_TSOFF		0x04A98 /* Tx FC SOF */
/*N*///#define TXGBE_REOFF		0x05158 /* Rx FC EOF */
/*N*///#define TXGBE_RSOFF		0x051F8 /* Rx FC SOF */
/* FCoE Filter Context Registers */
/*N*///#define TXGBE_FCD_ID		0x05114 /* FCoE D_ID */
/*N*///#define TXGBE_FCSMAC		0x0510C /* FCoE Source MAC */
/*N*///#define TXGBE_FCFLTRW_SMAC_HIGH_SHIFT	16
/* FCoE Direct Filter Context */
/*N*///#define TXGBE_FCDFC(_i, _j)	(0x28000 + ((_i) * 0x4) + ((_j) * 0x10))
/*N*///#define TXGBE_FCDFCD(_i)	(0x30000 + ((_i) * 0x4))
/*N*///#define TXGBE_FCFLT		0x05108 /* FC FLT Context */
/*N*///#define TXGBE_FCFLTRW		0x05110 /* FC Filter RW Control */
/*N*///#define TXGBE_FCPARAM		0x051d8 /* FC Offset Parameter */
/*N*///#define TXGBE_FCFLT_VALID	(1 << 0)   /* Filter Context Valid */
/*N*///#define TXGBE_FCFLT_FIRST	(1 << 1)   /* Filter First */
/*N*///#define TXGBE_FCFLT_SEQID	0x00ff0000 /* Sequence ID */
/*N*///#define TXGBE_FCFLT_SEQCNT	0xff000000 /* Sequence Count */
/*N*///#define TXGBE_FCFLTRW_RVALDT	(1 << 13)  /* Fast Re-Validation */
/*N*///#define TXGBE_FCFLTRW_WE	(1 << 14)  /* Write Enable */
/*N*///#define TXGBE_FCFLTRW_RE	(1 << 15)  /* Read Enable */
/* FCoE Receive Control */
/*N*///#define TXGBE_FCRXCTRL		0x05100 /* FC Receive Control */
/*N*///#define TXGBE_FCRXCTRL_FCOELLI	(1 << 0)   /* Low latency interrupt */
/*N*///#define TXGBE_FCRXCTRL_SAVBAD	(1 << 1)   /* Save Bad Frames */
/*N*///#define TXGBE_FCRXCTRL_FRSTRDH	(1 << 2)   /* EN 1st Read Header */
/*N*///#define TXGBE_FCRXCTRL_LASTSEQH	(1 << 3)   /* EN Last Header in Seq */
/*N*///#define TXGBE_FCRXCTRL_ALLH	(1 << 4)   /* EN All Headers */
/*N*///#define TXGBE_FCRXCTRL_FRSTSEQH	(1 << 5)   /* EN 1st Seq. Header */
/*N*///#define TXGBE_FCRXCTRL_ICRC	(1 << 6)   /* Ignore Bad FC CRC */
/*N*///#define TXGBE_FCRXCTRL_FCCRCBO	(1 << 7)   /* FC CRC Byte Ordering */
/*N*///#define TXGBE_FCRXCTRL_FCOEVER	0x00000f00 /* FCoE Version: 4 bits */
/*N*///#define TXGBE_FCRXCTRL_FCOEVER_SHIFT	8
/* FCoE Redirection */
/*N*///#define TXGBE_FCRECTL		0x0ED00 /* FC Redirection Control */
/*N*///#define TXGBE_FCRETA0		0x0ED10 /* FC Redirection Table 0 */
/*N*///#define TXGBE_FCRETA(_i)	(TXGBE_FCRETA0 + ((_i) * 4)) /* FCoE Redir */
/*N*///#define TXGBE_FCRECTL_ENA	0x1 /* FCoE Redir Table Enable */
/*N*///#define TXGBE_FCRETASEL_ENA	0x2 /* FCoE FCRETASEL bit */
/*N*///#define TXGBE_FCRETA_SIZE	8 /* Max entries in FCRETA */
/*N*///#define TXGBE_FCRETA_ENTRY_MASK	0x0000007f /* 7 bits for the queue index */
/*N*///#define TXGBE_FCRETA_SIZE_X550	32 /* Max entries in FCRETA */
/* Higher 7 bits for the queue index */
/*N*///#define TXGBE_FCRETA_ENTRY_HIGH_MASK	0x007F0000
/*N*///#define TXGBE_FCRETA_ENTRY_HIGH_SHIFT	16

/* Stats registers */
/*N*///#define TXGBE_CRCERRS	0x04000
/*N*///#define TXGBE_ILLERRC	0x04004
/*N*///#define TXGBE_ERRBC	0x04008
/*N*///#define TXGBE_MSPDC	0x04010
/*N*///#define TXGBE_MPC(_i)	(0x03FA0 + ((_i) * 4)) /* 8 of these 3FA0-3FBC*/
/*N*///#define TXGBE_MLFC	0x04034
/*N*///#define TXGBE_MRFC	0x04038
/*N*///#define TXGBE_RLEC	0x04040
/*N*///#define TXGBE_LXONTXC	0x03F60
/*N*///#define TXGBE_LXONRXC	0x0CF60
/*N*///#define TXGBE_LXOFFTXC	0x03F68
/*N*///#define TXGBE_LXOFFRXC	0x0CF68
/*N*///#define TXGBE_LXONRXCNT		0x041A4
/*N*///#define TXGBE_LXOFFRXCNT	0x041A8
/*N*///#define TXGBE_PXONRXCNT(_i)	(0x04140 + ((_i) * 4)) /* 8 of these */
/*N*///#define TXGBE_PXOFFRXCNT(_i)	(0x04160 + ((_i) * 4)) /* 8 of these */
/*N*///#define TXGBE_PXON2OFFCNT(_i)	(0x03240 + ((_i) * 4)) /* 8 of these */
/*N*///#define TXGBE_PXONTXC(_i)	(0x03F00 + ((_i) * 4)) /* 8 of these 3F00-3F1C*/
/*N*///#define TXGBE_PXONRXC(_i)	(0x0CF00 + ((_i) * 4)) /* 8 of these CF00-CF1C*/
/*N*///#define TXGBE_PXOFFTXC(_i)	(0x03F20 + ((_i) * 4)) /* 8 of these 3F20-3F3C*/
/*N*///#define TXGBE_PXOFFRXC(_i)	(0x0CF20 + ((_i) * 4)) /* 8 of these CF20-CF3C*/
/*N*///#define TXGBE_PRC64		0x0405C
/*N*///#define TXGBE_PRC127		0x04060
/*N*///#define TXGBE_PRC255		0x04064
/*N*///#define TXGBE_PRC511		0x04068
/*N*///#define TXGBE_PRC1023		0x0406C
/*N*///#define TXGBE_PRC1522		0x04070
/*N*///#define TXGBE_GPRC		0x04074
/*N*///#define TXGBE_BPRC		0x04078
/*N*///#define TXGBE_MPRC		0x0407C
/*N*///#define TXGBE_GPTC		0x04080
/*N*///#define TXGBE_GORCL		0x04088
/*N*///#define TXGBE_GORCH		0x0408C
/*N*///#define TXGBE_GOTCL		0x04090
/*N*///#define TXGBE_GOTCH		0x04094
/*N*///#define TXGBE_RNBC(_i)		(0x03FC0 + ((_i) * 4)) /* 8 of these 3FC0-3FDC*/
/*N*///#define TXGBE_RUC		0x040A4
/*N*///#define TXGBE_RFC		0x040A8
/*N*///#define TXGBE_ROC		0x040AC
/*N*///#define TXGBE_RJC		0x040B0
/*N*///#define TXGBE_MNGPRC		0x040B4
/*N*///#define TXGBE_MNGPDC		0x040B8
/*N*///#define TXGBE_MNGPTC		0x0CF90
/*N*///#define TXGBE_TORL		0x040C0
/*N*///#define TXGBE_TORH		0x040C4
/*N*///#define TXGBE_TPR		0x040D0
/*N*///#define TXGBE_TPT		0x040D4
/*N*///#define TXGBE_PTC64		0x040D8
/*N*///#define TXGBE_PTC127		0x040DC
/*N*///#define TXGBE_PTC255		0x040E0
/*N*///#define TXGBE_PTC511		0x040E4
/*N*///#define TXGBE_PTC1023		0x040E8
/*N*///#define TXGBE_PTC1522		0x040EC
/*N*///#define TXGBE_MPTC		0x040F0
/*N*///#define TXGBE_BPTC		0x040F4
/*N*///#define TXGBE_XEC		0x04120
/*N*///#define TXGBE_SSVPC		0x08780

/*N*///#define TXGBE_RQSMR(_i)	(0x02300 + ((_i) * 4))
/*N*//*#define TXGBE_TQSMR(_i)	(((_i) <= 7) ? (0x07300 + ((_i) * 4)) : \
			 (0x08600 + ((_i) * 4)))*/
/*N*///#define TXGBE_TQSM(_i)	(0x08600 + ((_i) * 4))

/*N*///#define TXGBE_QPRC(_i)	(0x01030 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QPTC(_i)	(0x06030 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QBRC(_i)	(0x01034 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QBTC(_i)	(0x06034 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QBRC_L(_i)	(0x01034 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QBRC_H(_i)	(0x01038 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QPRDC(_i)		(0x01430 + ((_i) * 0x40)) /* 16 of these */
/*N*///#define TXGBE_QBTC_L(_i)	(0x08700 + ((_i) * 0x8)) /* 16 of these */
/*N*///#define TXGBE_QBTC_H(_i)	(0x08704 + ((_i) * 0x8)) /* 16 of these */
/*N*///#define TXGBE_FCCRC		0x05118 /* Num of Good Eth CRC w/ Bad FC CRC */
/*N*///#define TXGBE_FCOERPDC		0x0241C /* FCoE Rx Packets Dropped Count */
/*N*///#define TXGBE_FCLAST		0x02424 /* FCoE Last Error Count */
/*N*///#define TXGBE_FCOEPRC		0x02428 /* Number of FCoE Packets Received */
/*N*///#define TXGBE_FCOEDWRC		0x0242C /* Number of FCoE DWords Received */
/*N*///#define TXGBE_FCOEPTC		0x08784 /* Number of FCoE Packets Transmitted */
/*N*///#define TXGBE_FCOEDWTC		0x08788 /* Number of FCoE DWords Transmitted */
/*N*///#define TXGBE_FCCRC_CNT_MASK	0x0000FFFF /* CRC_CNT: bit 0 - 15 */
/*N*///#define TXGBE_FCLAST_CNT_MASK	0x0000FFFF /* Last_CNT: bit 0 - 15 */
/*N*///#define TXGBE_O2BGPTC		0x041C4
/*N*///#define TXGBE_O2BSPC		0x087B0
/*N*///#define TXGBE_B2OSPC		0x041C0
/*N*///#define TXGBE_B2OGPRC		0x02F90
/*N*///#define TXGBE_BUPRC		0x04180
/*N*///#define TXGBE_BMPRC		0x04184
/*N*///#define TXGBE_BBPRC		0x04188
/*N*///#define TXGBE_BUPTC		0x0418C
/*N*///#define TXGBE_BMPTC		0x04190
/*N*///#define TXGBE_BBPTC		0x04194
/*N*///#define TXGBE_BCRCERRS		0x04198
/*N*///#define TXGBE_BXONRXC		0x0419C
/*N*///#define TXGBE_BXOFFRXC		0x041E0
/*N*///#define TXGBE_BXONTXC		0x041E4
/*N*///#define TXGBE_BXOFFTXC		0x041E8

/* Management */
/*N*///#define TXGBE_MAVTV(_i)		(0x05010 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_MFUTP(_i)		(0x05030 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_MANC		0x05820
/*N*///#define TXGBE_MFVAL		0x05824
/*N*///#define TXGBE_MANC2H		0x05860
/*N*///#define TXGBE_MDEF(_i)		(0x05890 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_MIPAF		0x058B0
/*N*///#define TXGBE_MMAL(_i)		(0x05910 + ((_i) * 8)) /* 4 of these (0-3) */
/*N*///#define TXGBE_MMAH(_i)		(0x05914 + ((_i) * 8)) /* 4 of these (0-3) */
/*N*///#define TXGBE_FTFT		0x09400 /* 0x9400-0x97FC */
/*N*///#define TXGBE_METF(_i)		(0x05190 + ((_i) * 4)) /* 4 of these (0-3) */
/*N*///#define TXGBE_MDEF_EXT(_i)	(0x05160 + ((_i) * 4)) /* 8 of these (0-7) */
/*N*///#define TXGBE_LSWFW		0x15F14
/*N*///#define TXGBE_BMCIP(_i)		(0x05050 + ((_i) * 4)) /* 0x5050-0x505C */
/*N*///#define TXGBE_BMCIPVAL		0x05060
/*N*///#define TXGBE_BMCIP_IPADDR_TYPE	0x00000001
/*N*///#define TXGBE_BMCIP_IPADDR_VALID	0x00000002

/* Management Bit Fields and Masks */
/*N*///#define TXGBE_MANC_MPROXYE	0x40000000 /* Management Proxy Enable */
/*N*///#define TXGBE_MANC_RCV_TCO_EN	0x00020000 /* Rcv TCO packet enable */
/*N*///#define TXGBE_MANC_EN_BMC2OS	0x10000000 /* Ena BMC2OS and OS2BMC traffic */
/*N*///#define TXGBE_MANC_EN_BMC2OS_SHIFT	28

/* Firmware Semaphore Register */
/*N*///#define TXGBE_FWSM_MODE_MASK	0xE
/*N*///#define TXGBE_FWSM_TS_ENABLED	0x1
/*N*///#define TXGBE_FWSM_FW_MODE_PT	0x4

/* ARC Subsystem registers */
/*N*///#define TXGBE_HICR		0x15F00
/*N*///#define TXGBE_FWSTS		0x15F0C
/*N*///#define TXGBE_HSMC0R		0x15F04
/*N*///#define TXGBE_HSMC1R		0x15F08
/*N*///#define TXGBE_SWSR		0x15F10
/*N*///#define TXGBE_HFDR		0x15FE8
/*N*///#define TXGBE_FLEX_MNG		0x15800 /* 0x15800 - 0x15EFC */

/*N*///#define TXGBE_HICR_EN		0x01  /* Enable bit - RO */
/* Driver sets this bit when done to put command in RAM */
/*N*///#define TXGBE_HICR_C		0x02
/*N*///#define TXGBE_HICR_SV		0x04  /* Status Validity */
/*N*///#define TXGBE_HICR_FW_RESET_ENABLE	0x40
/*N*///#define TXGBE_HICR_FW_RESET	0x80

/* PCI-E registers */
/*N*///#define TXGBE_GCR		0x11000
/*N*///#define TXGBE_GTV		0x11004
/*N*///#define TXGBE_FUNCTAG		0x11008
/*N*///#define TXGBE_GLT		0x1100C
/*N*///#define TXGBE_PCIEPIPEADR	0x11004
/*N*///#define TXGBE_PCIEPIPEDAT	0x11008
/*N*///#define TXGBE_GSCL_1		0x11010
/*N*///#define TXGBE_GSCL_2		0x11014
/*N*///#define TXGBE_GSCL_1_X540	TXGBE_GSCL_1
/*N*///#define TXGBE_GSCL_2_X540	TXGBE_GSCL_2
/*N*///#define TXGBE_GSCL_3		0x11018
/*N*///#define TXGBE_GSCL_4		0x1101C
/*N*///#define TXGBE_GSCN_0		0x11020
/*N*///#define TXGBE_GSCN_1		0x11024
/*N*///#define TXGBE_GSCN_2		0x11028
/*N*///#define TXGBE_GSCN_3		0x1102C
/*N*///#define TXGBE_GSCN_0_X540	TXGBE_GSCN_0
/*N*///#define TXGBE_GSCN_1_X540	TXGBE_GSCN_1
/*N*///#define TXGBE_GSCN_2_X540	TXGBE_GSCN_2
/*N*///#define TXGBE_GSCN_3_X540	TXGBE_GSCN_3
/*N*///#define TXGBE_FACTPS		0x10150
/*N*///#define TXGBE_FACTPS_X540	TXGBE_FACTPS
/*N*///#define TXGBE_GSCL_1_X550	0x11800
/*N*///#define TXGBE_GSCL_2_X550	0x11804
/*N*///#define TXGBE_GSCL_1_X550EM_x	TXGBE_GSCL_1_X550
/*N*///#define TXGBE_GSCL_2_X550EM_x	TXGBE_GSCL_2_X550
/*N*///#define TXGBE_GSCN_0_X550	0x11820
/*N*///#define TXGBE_GSCN_1_X550	0x11824
/*N*///#define TXGBE_GSCN_2_X550	0x11828
/*N*///#define TXGBE_GSCN_3_X550	0x1182C
/*N*///#define TXGBE_GSCN_0_X550EM_x	TXGBE_GSCN_0_X550
/*N*///#define TXGBE_GSCN_1_X550EM_x	TXGBE_GSCN_1_X550
/*N*///#define TXGBE_GSCN_2_X550EM_x	TXGBE_GSCN_2_X550
/*N*///#define TXGBE_GSCN_3_X550EM_x	TXGBE_GSCN_3_X550
/*N*///#define TXGBE_FACTPS_X550	TXGBE_FACTPS
/*N*///#define TXGBE_FACTPS_X550EM_x	TXGBE_FACTPS
/*N*///#define TXGBE_GSCL_1_X550EM_a	TXGBE_GSCL_1_X550
/*N*///#define TXGBE_GSCL_2_X550EM_a	TXGBE_GSCL_2_X550
/*N*///#define TXGBE_GSCN_0_X550EM_a	TXGBE_GSCN_0_X550
/*N*///#define TXGBE_GSCN_1_X550EM_a	TXGBE_GSCN_1_X550
/*N*///#define TXGBE_GSCN_2_X550EM_a	TXGBE_GSCN_2_X550
/*N*///#define TXGBE_GSCN_3_X550EM_a	TXGBE_GSCN_3_X550
/*N*///#define TXGBE_FACTPS_X550EM_a	0x15FEC
/*N*///#define TXGBE_FACTPS_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), FACTPS)

/*N*///#define TXGBE_PCIEANACTL	0x11040
/*N*///#define TXGBE_SWSM		0x10140
/*N*///#define TXGBE_SWSM_X540		TXGBE_SWSM
/*N*///#define TXGBE_SWSM_X550		TXGBE_SWSM
/*N*///#define TXGBE_SWSM_X550EM_x	TXGBE_SWSM
/*N*///#define TXGBE_SWSM_X550EM_a	0x15F70
/*N*///#define TXGBE_SWSM_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), SWSM)

/*N*///#define TXGBE_FWSM		0x10148
/*N*///#define TXGBE_FWSM_X540		TXGBE_FWSM
/*N*///#define TXGBE_FWSM_X550		TXGBE_FWSM
/*N*///#define TXGBE_FWSM_X550EM_x	TXGBE_FWSM
/*N*///#define TXGBE_FWSM_X550EM_a	0x15F74
/*N*///#define TXGBE_FWSM_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), FWSM)

/*N*///#define TXGBE_SWFW_SYNC		TXGBE_GSSR
/*N*///#define TXGBE_SWFW_SYNC_X540	TXGBE_SWFW_SYNC
/*N*///#define TXGBE_SWFW_SYNC_X550	TXGBE_SWFW_SYNC
/*N*///#define TXGBE_SWFW_SYNC_X550EM_x	TXGBE_SWFW_SYNC
/*N*///#define TXGBE_SWFW_SYNC_X550EM_a	0x15F78
/*N*///#define TXGBE_SWFW_SYNC_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), SWFW_SYNC)

/*N*///#define TXGBE_GSSR		0x10160
/*N*///#define TXGBE_MREVID		0x11064
/*N*///#define TXGBE_DCA_ID		0x11070
/*N*///#define TXGBE_DCA_CTRL		0x11074

/* PCI-E registers 82599-Specific */
/*N*///#define TXGBE_GCR_EXT		0x11050
/*N*///#define TXGBE_GSCL_5_82599	0x11030
/*N*///#define TXGBE_GSCL_6_82599	0x11034
/*N*///#define TXGBE_GSCL_7_82599	0x11038
/*N*///#define TXGBE_GSCL_8_82599	0x1103C
/*N*///#define TXGBE_GSCL_5_X540	TXGBE_GSCL_5_82599
/*N*///#define TXGBE_GSCL_6_X540	TXGBE_GSCL_6_82599
/*N*///#define TXGBE_GSCL_7_X540	TXGBE_GSCL_7_82599
/*N*///#define TXGBE_GSCL_8_X540	TXGBE_GSCL_8_82599
/*N*///#define TXGBE_PHYADR_82599	0x11040
/*N*///#define TXGBE_PHYDAT_82599	0x11044
/*N*///#define TXGBE_PHYCTL_82599	0x11048
/*N*///#define TXGBE_PBACLR_82599	0x11068
/*N*///#define TXGBE_CIAA		0x11088
/*N*///#define TXGBE_CIAD		0x1108C
/*N*///#define TXGBE_CIAA_82599	TXGBE_CIAA
/*N*///#define TXGBE_CIAD_82599	TXGBE_CIAD
/*N*///#define TXGBE_CIAA_X540		TXGBE_CIAA
/*N*///#define TXGBE_CIAD_X540		TXGBE_CIAD
/*N*///#define TXGBE_GSCL_5_X550	0x11810
/*N*///#define TXGBE_GSCL_6_X550	0x11814
/*N*///#define TXGBE_GSCL_7_X550	0x11818
/*N*///#define TXGBE_GSCL_8_X550	0x1181C
/*N*///#define TXGBE_GSCL_5_X550EM_x	TXGBE_GSCL_5_X550
/*N*///#define TXGBE_GSCL_6_X550EM_x	TXGBE_GSCL_6_X550
/*N*///#define TXGBE_GSCL_7_X550EM_x	TXGBE_GSCL_7_X550
/*N*///#define TXGBE_GSCL_8_X550EM_x	TXGBE_GSCL_8_X550
/*N*///#define TXGBE_CIAA_X550		0x11508
/*N*///#define TXGBE_CIAD_X550		0x11510
/*N*///#define TXGBE_CIAA_X550EM_x	TXGBE_CIAA_X550
/*N*///#define TXGBE_CIAD_X550EM_x	TXGBE_CIAD_X550
/*N*///#define TXGBE_GSCL_5_X550EM_a	TXGBE_GSCL_5_X550
/*N*///#define TXGBE_GSCL_6_X550EM_a	TXGBE_GSCL_6_X550
/*N*///#define TXGBE_GSCL_7_X550EM_a	TXGBE_GSCL_7_X550
/*N*///#define TXGBE_GSCL_8_X550EM_a	TXGBE_GSCL_8_X550
/*N*///#define TXGBE_CIAA_X550EM_a	TXGBE_CIAA_X550
/*N*///#define TXGBE_CIAD_X550EM_a	TXGBE_CIAD_X550
/*N*///#define TXGBE_CIAA_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), CIAA)
/*N*///#define TXGBE_CIAD_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), CIAD)
/*N*///#define TXGBE_PICAUSE		0x110B0
/*N*///#define TXGBE_PIENA		0x110B8
/*N*///#define TXGBE_CDQ_MBR_82599	0x110B4
/*N*///#define TXGBE_PCIESPARE		0x110BC
/*N*///#define TXGBE_MISC_REG_82599	0x110F0
/*N*///#define TXGBE_ECC_CTRL_0_82599	0x11100
/*N*///#define TXGBE_ECC_CTRL_1_82599	0x11104
/*N*///#define TXGBE_ECC_STATUS_82599	0x110E0
/*N*///#define TXGBE_BAR_CTRL_82599	0x110F4

/* PCI Express Control */
/*N*///#define TXGBE_GCR_CMPL_TMOUT_MASK	0x0000F000
/*N*///#define TXGBE_GCR_CMPL_TMOUT_10ms	0x00001000
/*N*///#define TXGBE_GCR_CMPL_TMOUT_RESEND	0x00010000
/*N*///#define TXGBE_GCR_CAP_VER2		0x00040000

/*N*///#define TXGBE_GCR_EXT_MSIX_EN		0x80000000
/*N*///#define TXGBE_GCR_EXT_BUFFERS_CLEAR	0x40000000
/*N*///#define TXGBE_GCR_EXT_VT_MODE_16	0x00000001
/*N*///#define TXGBE_GCR_EXT_VT_MODE_32	0x00000002
/*N*///#define TXGBE_GCR_EXT_VT_MODE_64	0x00000003
/*N*//*#define TXGBE_GCR_EXT_SRIOV		(TXGBE_GCR_EXT_MSIX_EN | \
					 TXGBE_GCR_EXT_VT_MODE_64)*/
/*N*///#define TXGBE_GCR_EXT_VT_MODE_MASK	0x00000003
/* Time Sync Registers */
/*N*///#define TXGBE_TSYNCRXCTL	0x05188 /* Rx Time Sync Control register - RW */
/*N*///#define TXGBE_TSYNCTXCTL	0x08C00 /* Tx Time Sync Control register - RW */
/*N*///#define TXGBE_RXSTMPL	0x051E8 /* Rx timestamp Low - RO */
/*N*///#define TXGBE_RXSTMPH	0x051A4 /* Rx timestamp High - RO */
/*N*///#define TXGBE_RXSATRL	0x051A0 /* Rx timestamp attribute low - RO */
/*N*///#define TXGBE_RXSATRH	0x051A8 /* Rx timestamp attribute high - RO */
/*N*///#define TXGBE_RXMTRL	0x05120 /* RX message type register low - RW */
/*N*///#define TXGBE_TXSTMPL	0x08C04 /* Tx timestamp value Low - RO */
/*N*///#define TXGBE_TXSTMPH	0x08C08 /* Tx timestamp value High - RO */
/*N*///#define TXGBE_SYSTIML	0x08C0C /* System time register Low - RO */
/*N*///#define TXGBE_SYSTIMH	0x08C10 /* System time register High - RO */
/*N*///#define TXGBE_SYSTIMR	0x08C58 /* System time register Residue - RO */
/*N*///#define TXGBE_TIMINCA	0x08C14 /* Increment attributes register - RW */
/*N*///#define TXGBE_TIMADJL	0x08C18 /* Time Adjustment Offset register Low - RW */
/*N*///#define TXGBE_TIMADJH	0x08C1C /* Time Adjustment Offset register High - RW */
/*N*///#define TXGBE_TSAUXC	0x08C20 /* TimeSync Auxiliary Control register - RW */
/*N*///#define TXGBE_TRGTTIML0	0x08C24 /* Target Time Register 0 Low - RW */
/*N*///#define TXGBE_TRGTTIMH0	0x08C28 /* Target Time Register 0 High - RW */
/*N*///#define TXGBE_TRGTTIML1	0x08C2C /* Target Time Register 1 Low - RW */
/*N*///#define TXGBE_TRGTTIMH1	0x08C30 /* Target Time Register 1 High - RW */
/*N*///#define TXGBE_CLKTIML	0x08C34 /* Clock Out Time Register Low - RW */
/*N*///#define TXGBE_CLKTIMH	0x08C38 /* Clock Out Time Register High - RW */
/*N*///#define TXGBE_FREQOUT0	0x08C34 /* Frequency Out 0 Control register - RW */
/*N*///#define TXGBE_FREQOUT1	0x08C38 /* Frequency Out 1 Control register - RW */
/*N*///#define TXGBE_AUXSTMPL0	0x08C3C /* Auxiliary Time Stamp 0 register Low - RO */
/*N*///#define TXGBE_AUXSTMPH0	0x08C40 /* Auxiliary Time Stamp 0 register High - RO */
/*N*///#define TXGBE_AUXSTMPL1	0x08C44 /* Auxiliary Time Stamp 1 register Low - RO */
/*N*///#define TXGBE_AUXSTMPH1	0x08C48 /* Auxiliary Time Stamp 1 register High - RO */
/*N*///#define TXGBE_TSIM	0x08C68 /* TimeSync Interrupt Mask Register - RW */
/*N*///#define TXGBE_TSICR	0x08C60 /* TimeSync Interrupt Cause Register - WO */
/*N*///#define TXGBE_TSSDP	0x0003C /* TimeSync SDP Configuration Register - RW */

/* Diagnostic Registers */
/*N*///#define TXGBE_RDSTATCTL		0x02C20
/*N*///#define TXGBE_RDSTAT(_i)	(0x02C00 + ((_i) * 4)) /* 0x02C00-0x02C1C */
/*N*///#define TXGBE_RDHMPN		0x02F08
/*N*///#define TXGBE_RIC_DW(_i)	(0x02F10 + ((_i) * 4))
/*N*///#define TXGBE_RDPROBE		0x02F20
/*N*///#define TXGBE_RDMAM		0x02F30
/*N*///#define TXGBE_RDMAD		0x02F34
/*N*///#define TXGBE_TDHMPN		0x07F08
/*N*///#define TXGBE_TDHMPN2		0x082FC
/*N*///#define TXGBE_TXDESCIC		0x082CC
/*N*///#define TXGBE_TIC_DW(_i)	(0x07F10 + ((_i) * 4))
/*N*///#define TXGBE_TIC_DW2(_i)	(0x082B0 + ((_i) * 4))
/*N*///#define TXGBE_TDPROBE		0x07F20
/*N*///#define TXGBE_TXBUFCTRL		0x0C600
/*N*///#define TXGBE_TXBUFDATA0	0x0C610
/*N*///#define TXGBE_TXBUFDATA1	0x0C614
/*N*///#define TXGBE_TXBUFDATA2	0x0C618
/*N*///#define TXGBE_TXBUFDATA3	0x0C61C
/*N*///#define TXGBE_RXBUFCTRL		0x03600
/*N*///#define TXGBE_RXBUFDATA0	0x03610
/*N*///#define TXGBE_RXBUFDATA1	0x03614
/*N*///#define TXGBE_RXBUFDATA2	0x03618
/*N*///#define TXGBE_RXBUFDATA3	0x0361C
/*N*///#define TXGBE_PCIE_DIAG(_i)	(0x11090 + ((_i) * 4)) /* 8 of these */
/*N*///#define TXGBE_RFVAL		0x050A4
/*N*///#define TXGBE_MDFTC1		0x042B8
/*N*///#define TXGBE_MDFTC2		0x042C0
/*N*///#define TXGBE_MDFTFIFO1		0x042C4
/*N*///#define TXGBE_MDFTFIFO2		0x042C8
/*N*///#define TXGBE_MDFTS		0x042CC
/*N*///#define TXGBE_RXDATAWRPTR(_i)	(0x03700 + ((_i) * 4)) /* 8 of these 3700-370C*/
/*N*///#define TXGBE_RXDESCWRPTR(_i)	(0x03710 + ((_i) * 4)) /* 8 of these 3710-371C*/
/*N*///#define TXGBE_RXDATARDPTR(_i)	(0x03720 + ((_i) * 4)) /* 8 of these 3720-372C*/
/*N*///#define TXGBE_RXDESCRDPTR(_i)	(0x03730 + ((_i) * 4)) /* 8 of these 3730-373C*/
/*N*///#define TXGBE_TXDATAWRPTR(_i)	(0x0C700 + ((_i) * 4)) /* 8 of these C700-C70C*/
/*N*///#define TXGBE_TXDESCWRPTR(_i)	(0x0C710 + ((_i) * 4)) /* 8 of these C710-C71C*/
/*N*///#define TXGBE_TXDATARDPTR(_i)	(0x0C720 + ((_i) * 4)) /* 8 of these C720-C72C*/
/*N*///#define TXGBE_TXDESCRDPTR(_i)	(0x0C730 + ((_i) * 4)) /* 8 of these C730-C73C*/
/*N*///#define TXGBE_PCIEECCCTL	0x1106C
/*N*///#define TXGBE_RXWRPTR(_i)	(0x03100 + ((_i) * 4)) /* 8 of these 3100-310C*/
/*N*///#define TXGBE_RXUSED(_i)	(0x03120 + ((_i) * 4)) /* 8 of these 3120-312C*/
/*N*///#define TXGBE_RXRDPTR(_i)	(0x03140 + ((_i) * 4)) /* 8 of these 3140-314C*/
/*N*///#define TXGBE_RXRDWRPTR(_i)	(0x03160 + ((_i) * 4)) /* 8 of these 3160-310C*/
/*N*///#define TXGBE_TXWRPTR(_i)	(0x0C100 + ((_i) * 4)) /* 8 of these C100-C10C*/
/*N*///#define TXGBE_TXUSED(_i)	(0x0C120 + ((_i) * 4)) /* 8 of these C120-C12C*/
/*N*///#define TXGBE_TXRDPTR(_i)	(0x0C140 + ((_i) * 4)) /* 8 of these C140-C14C*/
/*N*///#define TXGBE_TXRDWRPTR(_i)	(0x0C160 + ((_i) * 4)) /* 8 of these C160-C10C*/
/*N*///#define TXGBE_PCIEECCCTL0	0x11100
/*N*///#define TXGBE_PCIEECCCTL1	0x11104
/*N*///#define TXGBE_RXDBUECC		0x03F70
/*N*///#define TXGBE_TXDBUECC		0x0CF70
/*N*///#define TXGBE_RXDBUEST		0x03F74
/*N*///#define TXGBE_TXDBUEST		0x0CF74
/*N*///#define TXGBE_PBTXECC		0x0C300
/*N*///#define TXGBE_PBRXECC		0x03300
/*N*///#define TXGBE_GHECCR		0x110B0

/* MAC Registers */
/*N*///#define TXGBE_PCS1GCFIG		0x04200
/*N*///#define TXGBE_PCS1GLCTL		0x04208
/*N*///#define TXGBE_PCS1GLSTA		0x0420C
/*N*///#define TXGBE_PCS1GDBG0		0x04210
/*N*///#define TXGBE_PCS1GDBG1		0x04214
/*N*///#define TXGBE_PCS1GANA		0x04218
/*N*///#define TXGBE_PCS1GANLP		0x0421C
/*N*///#define TXGBE_PCS1GANNP		0x04220
/*N*///#define TXGBE_PCS1GANLPNP	0x04224
/*N*///#define TXGBE_HLREG0		0x04240
/*N*///#define TXGBE_HLREG1		0x04244
/*N*///#define TXGBE_PAP		0x04248
/*N*///#define TXGBE_MACA		0x0424C
/*N*///#define TXGBE_APAE		0x04250
/*N*///#define TXGBE_ARD		0x04254
/*N*///#define TXGBE_AIS		0x04258
/*N*///#define TXGBE_MSCA		0x0425C
/*N*///#define TXGBE_MSRWD		0x04260
/*N*///#define TXGBE_MLADD		0x04264
/*N*///#define TXGBE_MHADD		0x04268
/*N*///#define TXGBE_MAXFRS		0x04268
/*N*///#define TXGBE_TREG		0x0426C
/*N*///#define TXGBE_PCSS1		0x04288
/*N*///#define TXGBE_PCSS2		0x0428C
/*N*///#define TXGBE_XPCSS		0x04290
/*N*///#define TXGBE_MFLCN		0x04294
/*N*///#define TXGBE_SERDESC		0x04298
/*N*///#define TXGBE_MAC_SGMII_BUSY	0x04298
/*N*///#define TXGBE_MACS		0x0429C
/*N*///#define TXGBE_AUTOC		0x042A0
/*N*///#define TXGBE_LINKS		0x042A4
/*N*///#define TXGBE_LINKS2		0x04324
/*N*///#define TXGBE_AUTOC2		0x042A8
/*N*///#define TXGBE_AUTOC3		0x042AC
/*N*///#define TXGBE_ANLP1		0x042B0
/*N*///#define TXGBE_ANLP2		0x042B4
/*N*///#define TXGBE_MACC		0x04330
/*N*///#define TXGBE_ATLASCTL		0x04800
/*N*///#define TXGBE_MMNGC		0x042D0
/*N*///#define TXGBE_ANLPNP1		0x042D4
/*N*///#define TXGBE_ANLPNP2		0x042D8
/*N*///#define TXGBE_KRPCSFC		0x042E0
/*N*///#define TXGBE_KRPCSS		0x042E4
/*N*///#define TXGBE_FECS1		0x042E8
/*N*///#define TXGBE_FECS2		0x042EC
/*N*///#define TXGBE_SMADARCTL		0x14F10
/*N*///#define TXGBE_MPVC		0x04318
/*N*///#define TXGBE_SGMIIC		0x04314

/* Statistics Registers */
/*N*///#define TXGBE_RXNFGPC		0x041B0
/*N*///#define TXGBE_RXNFGBCL		0x041B4
/*N*///#define TXGBE_RXNFGBCH		0x041B8
/*N*///#define TXGBE_RXDGPC		0x02F50
/*N*///#define TXGBE_RXDGBCL		0x02F54
/*N*///#define TXGBE_RXDGBCH		0x02F58
/*N*///#define TXGBE_RXDDGPC		0x02F5C
/*N*///#define TXGBE_RXDDGBCL		0x02F60
/*N*///#define TXGBE_RXDDGBCH		0x02F64
/*N*///#define TXGBE_RXLPBKGPC		0x02F68
/*N*///#define TXGBE_RXLPBKGBCL	0x02F6C
/*N*///#define TXGBE_RXLPBKGBCH	0x02F70
/*N*///#define TXGBE_RXDLPBKGPC	0x02F74
/*N*///#define TXGBE_RXDLPBKGBCL	0x02F78
/*N*///#define TXGBE_RXDLPBKGBCH	0x02F7C
/*N*///#define TXGBE_TXDGPC		0x087A0
/*N*///#define TXGBE_TXDGBCL		0x087A4
/*N*///#define TXGBE_TXDGBCH		0x087A8

/*N*///#define TXGBE_RXDSTATCTRL	0x02F40

/* Copper Pond 2 link timeout */
/*N*///#define TXGBE_VALIDATE_LINK_READY_TIMEOUT 50

/* Omer CORECTL */
/*N*///#define TXGBE_CORECTL			0x014F00
/* BARCTRL */
/*N*///#define TXGBE_BARCTRL			0x110F4
/*N*///#define TXGBE_BARCTRL_FLSIZE		0x0700
/*N*///#define TXGBE_BARCTRL_FLSIZE_SHIFT	8
/*N*///#define TXGBE_BARCTRL_CSRSIZE		0x2000

/* RSCCTL Bit Masks */
/*N*///#define TXGBE_RSCCTL_RSCEN	0x01
/*N*///#define TXGBE_RSCCTL_MAXDESC_1	0x00
/*N*///#define TXGBE_RSCCTL_MAXDESC_4	0x04
/*N*///#define TXGBE_RSCCTL_MAXDESC_8	0x08
/*N*///#define TXGBE_RSCCTL_MAXDESC_16	0x0C
/*N*///#define TXGBE_RSCCTL_TS_DIS	0x02

/* RSCDBU Bit Masks */
/*N*///#define TXGBE_RSCDBU_RSCSMALDIS_MASK	0x0000007F
/*N*///#define TXGBE_RSCDBU_RSCACKDIS		0x00000080

/* RDRXCTL Bit Masks */
/*N*///#define TXGBE_RDRXCTL_RDMTS_1_2		0x00000000 /* Rx Desc Min THLD Size */
/*N*///#define TXGBE_RDRXCTL_CRCSTRIP		0x00000002 /* CRC Strip */
/*N*///#define TXGBE_RDRXCTL_PSP		0x00000004 /* Pad Small Packet */
/*N*///#define TXGBE_RDRXCTL_MVMEN		0x00000020
/*N*///#define TXGBE_RDRXCTL_RSC_PUSH_DIS	0x00000020
/*N*///#define TXGBE_RDRXCTL_DMAIDONE		0x00000008 /* DMA init cycle done */
/*N*///#define TXGBE_RDRXCTL_RSC_PUSH		0x00000080
/*N*///#define TXGBE_RDRXCTL_AGGDIS		0x00010000 /* Aggregation disable */
/*N*///#define TXGBE_RDRXCTL_RSCFRSTSIZE	0x003E0000 /* RSC First packet size */
/*N*///#define TXGBE_RDRXCTL_RSCLLIDIS		0x00800000 /* Disable RSC compl on LLI*/
/*N*///#define TXGBE_RDRXCTL_RSCACKC		0x02000000 /* must set 1 when RSC ena */
/*N*///#define TXGBE_RDRXCTL_FCOE_WRFIX	0x04000000 /* must set 1 when RSC ena */
/*N*///#define TXGBE_RDRXCTL_MBINTEN		0x10000000
/*N*///#define TXGBE_RDRXCTL_MDP_EN		0x20000000

/* RQTC Bit Masks and Shifts */
/*N*///#define TXGBE_RQTC_SHIFT_TC(_i)	((_i) * 4)
/*N*///#define TXGBE_RQTC_TC0_MASK	(0x7 << 0)
/*N*///#define TXGBE_RQTC_TC1_MASK	(0x7 << 4)
/*N*///#define TXGBE_RQTC_TC2_MASK	(0x7 << 8)
/*N*///#define TXGBE_RQTC_TC3_MASK	(0x7 << 12)
/*N*///#define TXGBE_RQTC_TC4_MASK	(0x7 << 16)
/*N*///#define TXGBE_RQTC_TC5_MASK	(0x7 << 20)
/*N*///#define TXGBE_RQTC_TC6_MASK	(0x7 << 24)
/*N*///#define TXGBE_RQTC_TC7_MASK	(0x7 << 28)

/* PSRTYPE.RQPL Bit masks and shift */
/*N*///#define TXGBE_PSRTYPE_RQPL_MASK		0x7
/*N*///#define TXGBE_PSRTYPE_RQPL_SHIFT	29

/* CTRL Bit Masks */
/*N*///#define TXGBE_CTRL_GIO_DIS	0x00000004 /* Global IO Master Disable bit */
/*N*///#define TXGBE_CTRL_LNK_RST	0x00000008 /* Link Reset. Resets everything. */
/*N*///#define TXGBE_CTRL_RST		0x04000000 /* Reset (SW) */
/*N*///#define TXGBE_CTRL_RST_MASK	(TXGBE_CTRL_LNK_RST | TXGBE_CTRL_RST)

/* FACTPS */
/*N*///#define TXGBE_FACTPS_MNGCG	0x20000000 /* Manageblility Clock Gated */
/*N*///#define TXGBE_FACTPS_LFS	0x40000000 /* LAN Function Select */

/* MHADD Bit Masks */
/*N*///#define TXGBE_MHADD_MFS_MASK	0xFFFF0000
/*N*///#define TXGBE_MHADD_MFS_SHIFT	16

/* Extended Device Control */
/*N*///#define TXGBE_CTRL_EXT_PFRSTD	0x00004000 /* Physical Function Reset Done */
/*N*///#define TXGBE_CTRL_EXT_NS_DIS	0x00010000 /* No Snoop disable */
/*N*///#define TXGBE_CTRL_EXT_RO_DIS	0x00020000 /* Relaxed Ordering disable */
/*N*///#define TXGBE_CTRL_EXT_DRV_LOAD	0x10000000 /* Driver loaded bit for FW */

/* Direct Cache Access (DCA) definitions */
/*N*///#define TXGBE_DCA_CTRL_DCA_ENABLE	0x00000000 /* DCA Enable */
/*N*///#define TXGBE_DCA_CTRL_DCA_DISABLE	0x00000001 /* DCA Disable */

/*N*///#define TXGBE_DCA_CTRL_DCA_MODE_CB1	0x00 /* DCA Mode CB1 */
/*N*///#define TXGBE_DCA_CTRL_DCA_MODE_CB2	0x02 /* DCA Mode CB2 */

/*N*///#define TXGBE_DCA_RXCTRL_CPUID_MASK	0x0000001F /* Rx CPUID Mask */
/*N*///#define TXGBE_DCA_RXCTRL_CPUID_MASK_82599	0xFF000000 /* Rx CPUID Mask */
/*N*///#define TXGBE_DCA_RXCTRL_CPUID_SHIFT_82599	24 /* Rx CPUID Shift */
/*N*///#define TXGBE_DCA_RXCTRL_DESC_DCA_EN	(1 << 5) /* Rx Desc enable */
/*N*///#define TXGBE_DCA_RXCTRL_HEAD_DCA_EN	(1 << 6) /* Rx Desc header ena */
/*N*///#define TXGBE_DCA_RXCTRL_DATA_DCA_EN	(1 << 7) /* Rx Desc payload ena */
/*N*///#define TXGBE_DCA_RXCTRL_DESC_RRO_EN	(1 << 9) /* Rx rd Desc Relax Order */
/*N*///#define TXGBE_DCA_RXCTRL_DATA_WRO_EN	(1 << 13) /* Rx wr data Relax Order */
/*N*///#define TXGBE_DCA_RXCTRL_HEAD_WRO_EN	(1 << 15) /* Rx wr header RO */

/*N*///#define TXGBE_DCA_TXCTRL_CPUID_MASK	0x0000001F /* Tx CPUID Mask */
/*N*///#define TXGBE_DCA_TXCTRL_CPUID_MASK_82599	0xFF000000 /* Tx CPUID Mask */
/*N*///#define TXGBE_DCA_TXCTRL_CPUID_SHIFT_82599	24 /* Tx CPUID Shift */
/*N*///#define TXGBE_DCA_TXCTRL_DESC_DCA_EN	(1 << 5) /* DCA Tx Desc enable */
/*N*///#define TXGBE_DCA_TXCTRL_DESC_RRO_EN	(1 << 9) /* Tx rd Desc Relax Order */
/*N*///#define TXGBE_DCA_TXCTRL_DESC_WRO_EN	(1 << 11) /* Tx Desc writeback RO bit */
/*N*///#define TXGBE_DCA_TXCTRL_DATA_RRO_EN	(1 << 13) /* Tx rd data Relax Order */
/*N*///#define TXGBE_DCA_MAX_QUEUES_82598	16 /* DCA regs only on 16 queues */

/* MSCA Bit Masks */
/*N*///#define TXGBE_MSCA_NP_ADDR_MASK		0x0000FFFF /* MDI Addr (new prot) */
/*N*///#define TXGBE_MSCA_NP_ADDR_SHIFT	0
/*N*///#define TXGBE_MSCA_DEV_TYPE_MASK	0x001F0000 /* Dev Type (new prot) */
/*N*///#define TXGBE_MSCA_DEV_TYPE_SHIFT	16 /* Register Address (old prot */
/*N*///#define TXGBE_MSCA_PHY_ADDR_MASK	0x03E00000 /* PHY Address mask */
/*N*///#define TXGBE_MSCA_PHY_ADDR_SHIFT	21 /* PHY Address shift*/
/*N*///#define TXGBE_MSCA_OP_CODE_MASK		0x0C000000 /* OP CODE mask */
/*N*///#define TXGBE_MSCA_OP_CODE_SHIFT	26 /* OP CODE shift */
/*N*///#define TXGBE_MSCA_ADDR_CYCLE		0x00000000 /* OP CODE 00 (addr cycle) */
/*N*///#define TXGBE_MSCA_WRITE		0x04000000 /* OP CODE 01 (wr) */
/*N*///#define TXGBE_MSCA_READ			0x0C000000 /* OP CODE 11 (rd) */
/*N*///#define TXGBE_MSCA_READ_AUTOINC		0x08000000 /* OP CODE 10 (rd auto inc)*/
/*N*///#define TXGBE_MSCA_ST_CODE_MASK		0x30000000 /* ST Code mask */
/*N*///#define TXGBE_MSCA_ST_CODE_SHIFT	28 /* ST Code shift */
/*N*///#define TXGBE_MSCA_NEW_PROTOCOL		0x00000000 /* ST CODE 00 (new prot) */
/*N*///#define TXGBE_MSCA_OLD_PROTOCOL		0x10000000 /* ST CODE 01 (old prot) */
/*N*///#define TXGBE_MSCA_MDI_COMMAND		0x40000000 /* Initiate MDI command */
/*N*///#define TXGBE_MSCA_MDI_IN_PROG_EN	0x80000000 /* MDI in progress ena */

/* MSRWD bit masks */
/*N*///#define TXGBE_MSRWD_WRITE_DATA_MASK	0x0000FFFF
/*N*///#define TXGBE_MSRWD_WRITE_DATA_SHIFT	0
/*N*///#define TXGBE_MSRWD_READ_DATA_MASK	0xFFFF0000
/*N*///#define TXGBE_MSRWD_READ_DATA_SHIFT	16

/* Atlas registers */
/*N*///#define TXGBE_ATLAS_PDN_LPBK		0x24
/*N*///#define TXGBE_ATLAS_PDN_10G		0xB
/*N*///#define TXGBE_ATLAS_PDN_1G		0xC
/*N*///#define TXGBE_ATLAS_PDN_AN		0xD

/* Atlas bit masks */
/*N*///#define TXGBE_ATLASCTL_WRITE_CMD	0x00010000
/*N*///#define TXGBE_ATLAS_PDN_TX_REG_EN	0x10
/*N*///#define TXGBE_ATLAS_PDN_TX_10G_QL_ALL	0xF0
/*N*///#define TXGBE_ATLAS_PDN_TX_1G_QL_ALL	0xF0
/*N*///#define TXGBE_ATLAS_PDN_TX_AN_QL_ALL	0xF0

/* Omer bit masks */
/*N*///#define TXGBE_CORECTL_WRITE_CMD		0x00010000

/* Device Type definitions for new protocol MDIO commands */
/*N*///#define TXGBE_MDIO_DEV_TYPE_ZERO		0x0
/*N*///#define TXGBE_MDIO_DEV_TYPE_PMA_PMD		0x1
/*N*///#define TXGBE_MDIO_DEV_TYPE_PCS			0x3
/*N*///#define TXGBE_MDIO_DEV_TYPE_PHY_XS		0x4
/*N*///#define TXGBE_MDIO_DEV_TYPE_AUTO_NEG		0x7
/*N*///#define TXGBE_MDIO_DEV_TYPE_VENDOR_SPECIFIC_1	0x1E   /* Device 30 */
/*N*///#define TXGBE_TWINAX_DEV			1

/*N*///#define TXGBE_MDIO_COMMAND_TIMEOUT	100 /* PHY Timeout for 1 GB mode */

/*N*///#define TXGBE_MDIO_VENDOR_SPECIFIC_1_CONTROL		0x0 /* VS1 Ctrl Reg */
/*N*///#define TXGBE_MDIO_VENDOR_SPECIFIC_1_STATUS		0x1 /* VS1 Status Reg */
/*N*///#define TXGBE_MDIO_VENDOR_SPECIFIC_1_LINK_STATUS	0x0008 /* 1 = Link Up */
/*N*///#define TXGBE_MDIO_VENDOR_SPECIFIC_1_SPEED_STATUS	0x0010 /* 0-10G, 1-1G */
/*N*///#define TXGBE_MDIO_VENDOR_SPECIFIC_1_10G_SPEED		0x0018
/*N*///#define TXGBE_MDIO_VENDOR_SPECIFIC_1_1G_SPEED		0x0010

/*N*///#define TXGBE_MDIO_AUTO_NEG_CONTROL	0x0 /* AUTO_NEG Control Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_STATUS	0x1 /* AUTO_NEG Status Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STAT	0xC800 /* AUTO_NEG Vendor Status Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_TX_ALARM 0xCC00 /* AUTO_NEG Vendor TX Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_TX_ALARM2 0xCC01 /* AUTO_NEG Vendor Tx Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VEN_LSC	0x1 /* AUTO_NEG Vendor Tx LSC */
/*N*///#define TXGBE_MDIO_AUTO_NEG_ADVT	0x10 /* AUTO_NEG Advt Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_LP		0x13 /* AUTO_NEG LP Status Reg */
/*N*///#define TXGBE_MDIO_AUTO_NEG_EEE_ADVT	0x3C /* AUTO_NEG EEE Advt Reg */
/*N*///#define TXGBE_AUTO_NEG_10GBASE_EEE_ADVT	0x8  /* AUTO NEG EEE 10GBaseT Advt */
/*N*///#define TXGBE_AUTO_NEG_1000BASE_EEE_ADVT 0x4  /* AUTO NEG EEE 1000BaseT Advt */
/*N*///#define TXGBE_AUTO_NEG_100BASE_EEE_ADVT	0x2  /* AUTO NEG EEE 100BaseT Advt */
/*N*///#define TXGBE_MDIO_PHY_XS_CONTROL	0x0 /* PHY_XS Control Reg */
/*N*///#define TXGBE_MDIO_PHY_XS_RESET		0x8000 /* PHY_XS Reset */
/*N*///#define TXGBE_MDIO_PHY_ID_HIGH		0x2 /* PHY ID High Reg*/
/*N*///#define TXGBE_MDIO_PHY_ID_LOW		0x3 /* PHY ID Low Reg*/
/*N*///#define TXGBE_MDIO_PHY_SPEED_ABILITY	0x4 /* Speed Ability Reg */
/*N*///#define TXGBE_MDIO_PHY_SPEED_10G	0x0001 /* 10G capable */
/*N*///#define TXGBE_MDIO_PHY_SPEED_1G		0x0010 /* 1G capable */
/*N*///#define TXGBE_MDIO_PHY_SPEED_100M	0x0020 /* 100M capable */
/*N*///#define TXGBE_MDIO_PHY_EXT_ABILITY	0xB /* Ext Ability Reg */
/*N*///#define TXGBE_MDIO_PHY_10GBASET_ABILITY		0x0004 /* 10GBaseT capable */
/*N*///#define TXGBE_MDIO_PHY_1000BASET_ABILITY	0x0020 /* 1000BaseT capable */
/*N*///#define TXGBE_MDIO_PHY_100BASETX_ABILITY	0x0080 /* 100BaseTX capable */
/*N*///#define TXGBE_MDIO_PHY_SET_LOW_POWER_MODE	0x0800 /* Set low power mode */
/*N*///#define TXGBE_AUTO_NEG_LP_STATUS	0xE820 /* AUTO NEG Rx LP Status Reg */
/*N*///#define TXGBE_AUTO_NEG_LP_1000BASE_CAP	0x8000 /* AUTO NEG Rx LP 1000BaseT Cap */
/*N*///#define TXGBE_AUTO_NEG_LP_10GBASE_CAP	0x0800 /* AUTO NEG Rx LP 10GBaseT Cap */
/*N*///#define TXGBE_AUTO_NEG_10GBASET_STAT	0x0021 /* AUTO NEG 10G BaseT Stat */

/*N*///#define TXGBE_MDIO_TX_VENDOR_ALARMS_3		0xCC02 /* Vendor Alarms 3 Reg */
/*N*///#define TXGBE_MDIO_TX_VENDOR_ALARMS_3_RST_MASK	0x3 /* PHY Reset Complete Mask */
/*N*///#define TXGBE_MDIO_GLOBAL_RES_PR_10 0xC479 /* Global Resv Provisioning 10 Reg */
/*N*///#define TXGBE_MDIO_POWER_UP_STALL		0x8000 /* Power Up Stall */
/*N*///#define TXGBE_MDIO_GLOBAL_INT_CHIP_STD_MASK	0xFF00 /* int std mask */
/*N*///#define TXGBE_MDIO_GLOBAL_CHIP_STD_INT_FLAG	0xFC00 /* chip std int flag */
/*N*///#define TXGBE_MDIO_GLOBAL_INT_CHIP_VEN_MASK	0xFF01 /* int chip-wide mask */
/*N*///#define TXGBE_MDIO_GLOBAL_INT_CHIP_VEN_FLAG	0xFC01 /* int chip-wide mask */
/*N*///#define TXGBE_MDIO_GLOBAL_ALARM_1		0xCC00 /* Global alarm 1 */
/*N*///#define TXGBE_MDIO_GLOBAL_ALM_1_DEV_FAULT	0x0010 /* device fault */
/*N*///#define TXGBE_MDIO_GLOBAL_ALM_1_HI_TMP_FAIL	0x4000 /* high temp failure */
/*N*///#define TXGBE_MDIO_GLOBAL_FAULT_MSG	0xC850 /* Global Fault Message */
/*N*///#define TXGBE_MDIO_GLOBAL_FAULT_MSG_HI_TMP	0x8007 /* high temp failure */
/*N*///#define TXGBE_MDIO_GLOBAL_INT_MASK		0xD400 /* Global int mask */
/*N*///#define TXGBE_MDIO_GLOBAL_AN_VEN_ALM_INT_EN	0x1000 /* autoneg vendor alarm int enable */
/*N*///#define TXGBE_MDIO_GLOBAL_ALARM_1_INT		0x4 /* int in Global alarm 1 */
/*N*///#define TXGBE_MDIO_GLOBAL_VEN_ALM_INT_EN	0x1 /* vendor alarm int enable */
/*N*///#define TXGBE_MDIO_GLOBAL_STD_ALM2_INT		0x200 /* vendor alarm2 int mask */
/*N*///#define TXGBE_MDIO_GLOBAL_INT_HI_TEMP_EN	0x4000 /* int high temp enable */
/*N*///#define TXGBE_MDIO_GLOBAL_INT_DEV_FAULT_EN 0x0010 /* int dev fault enable */
/*N*///#define TXGBE_MDIO_PMA_PMD_CONTROL_ADDR	0x0000 /* PMA/PMD Control Reg */
/*N*///#define TXGBE_MDIO_PMA_PMD_SDA_SCL_ADDR	0xC30A /* PHY_XS SDA/SCL Addr Reg */
/*N*///#define TXGBE_MDIO_PMA_PMD_SDA_SCL_DATA	0xC30B /* PHY_XS SDA/SCL Data Reg */
/*N*///#define TXGBE_MDIO_PMA_PMD_SDA_SCL_STAT	0xC30C /* PHY_XS SDA/SCL Status Reg */
/*N*///#define TXGBE_MDIO_PMA_TX_VEN_LASI_INT_MASK 0xD401 /* PHY TX Vendor LASI */
/*N*///#define TXGBE_MDIO_PMA_TX_VEN_LASI_INT_EN   0x1 /* PHY TX Vendor LASI enable */
/*N*///#define TXGBE_MDIO_PMD_STD_TX_DISABLE_CNTR 0x9 /* Standard Transmit Dis Reg */
/*N*///#define TXGBE_MDIO_PMD_GLOBAL_TX_DISABLE 0x0001 /* PMD Global Transmit Dis */

/*N*///#define TXGBE_PCRC8ECL		0x0E810 /* PCR CRC-8 Error Count Lo */
/*N*///#define TXGBE_PCRC8ECH		0x0E811 /* PCR CRC-8 Error Count Hi */
/*N*///#define TXGBE_PCRC8ECH_MASK	0x1F
/*N*///#define TXGBE_LDPCECL		0x0E820 /* PCR Uncorrected Error Count Lo */
/*N*///#define TXGBE_LDPCECH		0x0E821 /* PCR Uncorrected Error Count Hi */

/* MII clause 22/28 definitions */
/*N*///#define TXGBE_MDIO_PHY_LOW_POWER_MODE	0x0800

/*N*///#define TXGBE_MDIO_XENPAK_LASI_STATUS		0x9005 /* XENPAK LASI Status register*/
/*N*///#define TXGBE_XENPAK_LASI_LINK_STATUS_ALARM	0x1 /* Link Status Alarm change */

/*N*///#define TXGBE_MDIO_AUTO_NEG_LINK_STATUS		0x4 /* Indicates if link is up */

/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_MASK	0x7 /* Speed/Duplex Mask */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VEN_STAT_SPEED_MASK		0x6 /* Speed Mask */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_10M_HALF	0x0 /* 10Mb/s Half Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_10M_FULL	0x1 /* 10Mb/s Full Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_100M_HALF	0x2 /* 100Mb/s Half Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_100M_FULL	0x3 /* 100Mb/s Full Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_1GB_HALF	0x4 /* 1Gb/s Half Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_1GB_FULL	0x5 /* 1Gb/s Full Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_10GB_HALF	0x6 /* 10Gb/s Half Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_10GB_FULL	0x7 /* 10Gb/s Full Duplex */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_1GB		0x4 /* 1Gb/s */
/*N*///#define TXGBE_MDIO_AUTO_NEG_VENDOR_STATUS_10GB		0x6 /* 10Gb/s */

/*N*///#define TXGBE_MII_10GBASE_T_AUTONEG_CTRL_REG	0x20   /* 10G Control Reg */
/*N*///#define TXGBE_MII_AUTONEG_VENDOR_PROVISION_1_REG 0xC400 /* 1G Provisioning 1 */
/*N*///#define TXGBE_MII_AUTONEG_XNP_TX_REG		0x17   /* 1G XNP Transmit */
/*N*///#define TXGBE_MII_AUTONEG_ADVERTISE_REG		0x10   /* 100M Advertisement */
/*N*///#define TXGBE_MII_10GBASE_T_ADVERTISE		0x1000 /* full duplex, bit:12*/
/*N*///#define TXGBE_MII_1GBASE_T_ADVERTISE_XNP_TX	0x4000 /* full duplex, bit:14*/
/*N*///#define TXGBE_MII_1GBASE_T_ADVERTISE		0x8000 /* full duplex, bit:15*/
/*N*///#define TXGBE_MII_2_5GBASE_T_ADVERTISE		0x0400
/*N*///#define TXGBE_MII_5GBASE_T_ADVERTISE		0x0800
/*N*///#define TXGBE_MII_100BASE_T_ADVERTISE		0x0100 /* full duplex, bit:8 */
/*N*///#define TXGBE_MII_100BASE_T_ADVERTISE_HALF	0x0080 /* half duplex, bit:7 */
/*N*///#define TXGBE_MII_RESTART			0x200
/*N*///#define TXGBE_MII_AUTONEG_COMPLETE		0x20
/*N*///#define TXGBE_MII_AUTONEG_LINK_UP		0x04
/*N*///#define TXGBE_MII_AUTONEG_REG			0x0

/*N*///#define TXGBE_PHY_REVISION_MASK		0xFFFFFFF0
/*N*///#define TXGBE_MAX_PHY_ADDR		32

/* Special PHY Init Routine */
/*N*///#define TXGBE_PHY_INIT_OFFSET_NL	0x002B
/*N*///#define TXGBE_PHY_INIT_END_NL		0xFFFF
/*N*///#define TXGBE_CONTROL_MASK_NL		0xF000
/*N*///#define TXGBE_DATA_MASK_NL		0x0FFF
/*N*///#define TXGBE_CONTROL_SHIFT_NL		12
/*N*///#define TXGBE_DELAY_NL			0
/*N*///#define TXGBE_DATA_NL			1
/*N*///#define TXGBE_CONTROL_NL		0x000F
/*N*///#define TXGBE_CONTROL_EOL_NL		0x0FFF
/*N*///#define TXGBE_CONTROL_SOL_NL		0x0000

/* General purpose Interrupt Enable */
/*N*///#define TXGBE_SDP0_GPIEN	0x00000001 /* SDP0 */
/*N*///#define TXGBE_SDP1_GPIEN	0x00000002 /* SDP1 */
/*N*///#define TXGBE_SDP2_GPIEN	0x00000004 /* SDP2 */
/*N*///#define TXGBE_SDP0_GPIEN_X540	0x00000002 /* SDP0 on X540 and X550 */
/*N*///#define TXGBE_SDP1_GPIEN_X540	0x00000004 /* SDP1 on X540 and X550 */
/*N*///#define TXGBE_SDP2_GPIEN_X540	0x00000008 /* SDP2 on X540 and X550 */
/*N*///#define TXGBE_SDP0_GPIEN_X550	TXGBE_SDP0_GPIEN_X540
/*N*///#define TXGBE_SDP1_GPIEN_X550	TXGBE_SDP1_GPIEN_X540
/*N*///#define TXGBE_SDP2_GPIEN_X550	TXGBE_SDP2_GPIEN_X540
/*N*///#define TXGBE_SDP0_GPIEN_X550EM_x	TXGBE_SDP0_GPIEN_X540
/*N*///#define TXGBE_SDP1_GPIEN_X550EM_x	TXGBE_SDP1_GPIEN_X540
/*N*///#define TXGBE_SDP2_GPIEN_X550EM_x	TXGBE_SDP2_GPIEN_X540
/*N*///#define TXGBE_SDP0_GPIEN_X550EM_a	TXGBE_SDP0_GPIEN_X540
/*N*///#define TXGBE_SDP1_GPIEN_X550EM_a	TXGBE_SDP1_GPIEN_X540
/*N*///#define TXGBE_SDP2_GPIEN_X550EM_a	TXGBE_SDP2_GPIEN_X540
/*N*///#define TXGBE_SDP0_GPIEN_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), SDP0_GPIEN)
/*N*///#define TXGBE_SDP1_GPIEN_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), SDP1_GPIEN)
/*N*///#define TXGBE_SDP2_GPIEN_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), SDP2_GPIEN)

/*N*///#define TXGBE_GPIE_MSIX_MODE	0x00000010 /* MSI-X mode */
/*N*///#define TXGBE_GPIE_OCD		0x00000020 /* Other Clear Disable */
/*N*///#define TXGBE_GPIE_EIMEN	0x00000040 /* Immediate Interrupt Enable */
/*N*///#define TXGBE_GPIE_EIAME	0x40000000
/*N*///#define TXGBE_GPIE_PBA_SUPPORT	0x80000000
/*N*///#define TXGBE_GPIE_RSC_DELAY_SHIFT	11
/*N*///#define TXGBE_GPIE_VTMODE_MASK	0x0000C000 /* VT Mode Mask */
/*N*///#define TXGBE_GPIE_VTMODE_16	0x00004000 /* 16 VFs 8 queues per VF */
/*N*///#define TXGBE_GPIE_VTMODE_32	0x00008000 /* 32 VFs 4 queues per VF */
/*N*///#define TXGBE_GPIE_VTMODE_64	0x0000C000 /* 64 VFs 2 queues per VF */

/* Packet Buffer Initialization */
/*N*///#define TXGBE_MAX_PACKET_BUFFERS	8

/*N*///#define TXGBE_TXPBSIZE_20KB	0x00005000 /* 20KB Packet Buffer */
/*N*///#define TXGBE_TXPBSIZE_40KB	0x0000A000 /* 40KB Packet Buffer */
/*N*///#define TXGBE_RXPBSIZE_48KB	0x0000C000 /* 48KB Packet Buffer */
/*N*///#define TXGBE_RXPBSIZE_64KB	0x00010000 /* 64KB Packet Buffer */
/*N*///#define TXGBE_RXPBSIZE_80KB	0x00014000 /* 80KB Packet Buffer */
/*N*///#define TXGBE_RXPBSIZE_128KB	0x00020000 /* 128KB Packet Buffer */
/*N*///#define TXGBE_RXPBSIZE_MAX	0x00080000 /* 512KB Packet Buffer */
/*N*///#define TXGBE_TXPBSIZE_MAX	0x00028000 /* 160KB Packet Buffer */

/*N*///#define TXGBE_TXPKT_SIZE_MAX	0xA /* Max Tx Packet size */
/*N*///#define TXGBE_MAX_PB		8

/* Packet buffer allocation strategies */
enum {
	PBA_STRATEGY_EQUAL	= 0, /* Distribute PB space equally */
#define PBA_STRATEGY_EQUAL	PBA_STRATEGY_EQUAL
	PBA_STRATEGY_WEIGHTED	= 1, /* Weight front half of TCs */
#define PBA_STRATEGY_WEIGHTED	PBA_STRATEGY_WEIGHTED
};

/* Transmit Flow Control status */
/*N*///#define TXGBE_TFCS_TXOFF	0x00000001
/*N*///#define TXGBE_TFCS_TXOFF0	0x00000100
/*N*///#define TXGBE_TFCS_TXOFF1	0x00000200
/*N*///#define TXGBE_TFCS_TXOFF2	0x00000400
/*N*///#define TXGBE_TFCS_TXOFF3	0x00000800
/*N*///#define TXGBE_TFCS_TXOFF4	0x00001000
/*N*///#define TXGBE_TFCS_TXOFF5	0x00002000
/*N*///#define TXGBE_TFCS_TXOFF6	0x00004000
/*N*///#define TXGBE_TFCS_TXOFF7	0x00008000

/* TCP Timer */
/*N*///#define TXGBE_TCPTIMER_KS		0x00000100
/*N*///#define TXGBE_TCPTIMER_COUNT_ENABLE	0x00000200
/*N*///#define TXGBE_TCPTIMER_COUNT_FINISH	0x00000400
/*N*///#define TXGBE_TCPTIMER_LOOP		0x00000800
/*N*///#define TXGBE_TCPTIMER_DURATION_MASK	0x000000FF

/* HLREG0 Bit Masks */
/*N*///#define TXGBE_HLREG0_TXCRCEN		0x00000001 /* bit  0 */
/*N*///#define TXGBE_HLREG0_RXCRCSTRP		0x00000002 /* bit  1 */
/*N*///#define TXGBE_HLREG0_JUMBOEN		0x00000004 /* bit  2 */
/*N*///#define TXGBE_HLREG0_TXPADEN		0x00000400 /* bit 10 */
/*N*///#define TXGBE_HLREG0_TXPAUSEEN		0x00001000 /* bit 12 */
/*N*///#define TXGBE_HLREG0_RXPAUSEEN		0x00004000 /* bit 14 */
/*N*///#define TXGBE_HLREG0_LPBK		0x00008000 /* bit 15 */
/*N*///#define TXGBE_HLREG0_MDCSPD		0x00010000 /* bit 16 */
/*N*///#define TXGBE_HLREG0_CONTMDC		0x00020000 /* bit 17 */
/*N*///#define TXGBE_HLREG0_CTRLFLTR		0x00040000 /* bit 18 */
/*N*///#define TXGBE_HLREG0_PREPEND		0x00F00000 /* bits 20-23 */
/*N*///#define TXGBE_HLREG0_PRIPAUSEEN		0x01000000 /* bit 24 */
/*N*///#define TXGBE_HLREG0_RXPAUSERECDA	0x06000000 /* bits 25-26 */
/*N*///#define TXGBE_HLREG0_RXLNGTHERREN	0x08000000 /* bit 27 */
/*N*///#define TXGBE_HLREG0_RXPADSTRIPEN	0x10000000 /* bit 28 */

/* VMD_CTL bitmasks */
/*N*///#define TXGBE_VMD_CTL_VMDQ_EN		0x00000001
/*N*///#define TXGBE_VMD_CTL_VMDQ_FILTER	0x00000002

/* VT_CTL bitmasks */
/*N*///#define TXGBE_VT_CTL_DIS_DEFPL		0x20000000 /* disable default pool */
/*N*///#define TXGBE_VT_CTL_REPLEN		0x40000000 /* replication enabled */
/*N*///#define TXGBE_VT_CTL_VT_ENABLE		0x00000001  /* Enable VT Mode */
/*N*///#define TXGBE_VT_CTL_POOL_SHIFT		7
/*N*///#define TXGBE_VT_CTL_POOL_MASK		(0x3F << TXGBE_VT_CTL_POOL_SHIFT)

/* VMOLR bitmasks */
/*N*///#define TXGBE_VMOLR_UPE		0x00400000 /* unicast promiscuous */
/*N*///#define TXGBE_VMOLR_VPE		0x00800000 /* VLAN promiscuous */
/*N*///#define TXGBE_VMOLR_AUPE	0x01000000 /* accept untagged packets */
/*N*///#define TXGBE_VMOLR_ROMPE	0x02000000 /* accept packets in MTA tbl */
/*N*///#define TXGBE_VMOLR_ROPE	0x04000000 /* accept packets in UC tbl */
/*N*///#define TXGBE_VMOLR_BAM		0x08000000 /* accept broadcast packets */
/*N*///#define TXGBE_VMOLR_MPE		0x10000000 /* multicast promiscuous */

/* VFRE bitmask */
/*N*///#define TXGBE_VFRE_ENABLE_ALL	0xFFFFFFFF

/*N*///#define TXGBE_VF_INIT_TIMEOUT	200 /* Number of retries to clear RSTI */

/* RDHMPN and TDHMPN bitmasks */
/*N*///#define TXGBE_RDHMPN_RDICADDR		0x007FF800
/*N*///#define TXGBE_RDHMPN_RDICRDREQ		0x00800000
/*N*///#define TXGBE_RDHMPN_RDICADDR_SHIFT	11
/*N*///#define TXGBE_TDHMPN_TDICADDR		0x003FF800
/*N*///#define TXGBE_TDHMPN_TDICRDREQ		0x00800000
/*N*///#define TXGBE_TDHMPN_TDICADDR_SHIFT	11

/*N*///#define TXGBE_RDMAM_MEM_SEL_SHIFT		13
/*N*///#define TXGBE_RDMAM_DWORD_SHIFT			9
/*N*///#define TXGBE_RDMAM_DESC_COMP_FIFO		1
/*N*///#define TXGBE_RDMAM_DFC_CMD_FIFO		2
/*N*///#define TXGBE_RDMAM_RSC_HEADER_ADDR		3
/*N*///#define TXGBE_RDMAM_TCN_STATUS_RAM		4
/*N*///#define TXGBE_RDMAM_WB_COLL_FIFO		5
/*N*///#define TXGBE_RDMAM_QSC_CNT_RAM			6
/*N*///#define TXGBE_RDMAM_QSC_FCOE_RAM		7
/*N*///#define TXGBE_RDMAM_QSC_QUEUE_CNT		8
/*N*///#define TXGBE_RDMAM_QSC_QUEUE_RAM		0xA
/*N*///#define TXGBE_RDMAM_QSC_RSC_RAM			0xB
/*N*///#define TXGBE_RDMAM_DESC_COM_FIFO_RANGE		135
/*N*///#define TXGBE_RDMAM_DESC_COM_FIFO_COUNT		4
/*N*///#define TXGBE_RDMAM_DFC_CMD_FIFO_RANGE		48
/*N*///#define TXGBE_RDMAM_DFC_CMD_FIFO_COUNT		7
/*N*///#define TXGBE_RDMAM_RSC_HEADER_ADDR_RANGE	32
/*N*///#define TXGBE_RDMAM_RSC_HEADER_ADDR_COUNT	4
/*N*///#define TXGBE_RDMAM_TCN_STATUS_RAM_RANGE	256
/*N*///#define TXGBE_RDMAM_TCN_STATUS_RAM_COUNT	9
/*N*///#define TXGBE_RDMAM_WB_COLL_FIFO_RANGE		8
/*N*///#define TXGBE_RDMAM_WB_COLL_FIFO_COUNT		4
/*N*///#define TXGBE_RDMAM_QSC_CNT_RAM_RANGE		64
/*N*///#define TXGBE_RDMAM_QSC_CNT_RAM_COUNT		4
/*N*///#define TXGBE_RDMAM_QSC_FCOE_RAM_RANGE		512
/*N*///#define TXGBE_RDMAM_QSC_FCOE_RAM_COUNT		5
/*N*///#define TXGBE_RDMAM_QSC_QUEUE_CNT_RANGE		32
/*N*///#define TXGBE_RDMAM_QSC_QUEUE_CNT_COUNT		4
/*N*///#define TXGBE_RDMAM_QSC_QUEUE_RAM_RANGE		128
/*N*///#define TXGBE_RDMAM_QSC_QUEUE_RAM_COUNT		8
/*N*///#define TXGBE_RDMAM_QSC_RSC_RAM_RANGE		32
/*N*///#define TXGBE_RDMAM_QSC_RSC_RAM_COUNT		8

/*N*///#define TXGBE_TXDESCIC_READY	0x80000000

/* Receive Checksum Control */
/*N*///#define TXGBE_RXCSUM_IPPCSE	0x00001000 /* IP payload checksum enable */
/*N*///#define TXGBE_RXCSUM_PCSD	0x00002000 /* packet checksum disabled */

/* FCRTL Bit Masks */
/*N*///#define TXGBE_FCRTL_XONE	0x80000000 /* XON enable */
/*N*///#define TXGBE_FCRTH_FCEN	0x80000000 /* Packet buffer fc enable */

/* PAP bit masks*/
/*N*///#define TXGBE_PAP_TXPAUSECNT_MASK	0x0000FFFF /* Pause counter mask */

/* RMCS Bit Masks */
/*N*///#define TXGBE_RMCS_RRM			0x00000002 /* Rx Recycle Mode enable */
/* Receive Arbitration Control: 0 Round Robin, 1 DFP */
/*N*///#define TXGBE_RMCS_RAC			0x00000004
/* Deficit Fixed Prio ena */
/*N*///#define TXGBE_RMCS_DFP			TXGBE_RMCS_RAC
/*N*///#define TXGBE_RMCS_TFCE_802_3X		0x00000008 /* Tx Priority FC ena */
/*N*///#define TXGBE_RMCS_TFCE_PRIORITY	0x00000010 /* Tx Priority FC ena */
/*N*///#define TXGBE_RMCS_ARBDIS		0x00000040 /* Arbitration disable bit */

/* FCCFG Bit Masks */
/*N*///#define TXGBE_FCCFG_TFCE_802_3X		0x00000008 /* Tx link FC enable */
/*N*///#define TXGBE_FCCFG_TFCE_PRIORITY	0x00000010 /* Tx priority FC enable */

/* Interrupt register bitmasks */

/* Extended Interrupt Cause Read */
/*N*///#define TXGBE_EICR_RTX_QUEUE	0x0000FFFF /* RTx Queue Interrupt */
/*N*///#define TXGBE_EICR_FLOW_DIR	0x00010000 /* FDir Exception */
/*N*///#define TXGBE_EICR_RX_MISS	0x00020000 /* Packet Buffer Overrun */
/*N*///#define TXGBE_EICR_PCI		0x00040000 /* PCI Exception */
/*N*///#define TXGBE_EICR_MAILBOX	0x00080000 /* VF to PF Mailbox Interrupt */
/*N*///#define TXGBE_EICR_LSC		0x00100000 /* Link Status Change */
/*N*///#define TXGBE_EICR_LINKSEC	0x00200000 /* PN Threshold */
/*N*///#define TXGBE_EICR_MNG		0x00400000 /* Manageability Event Interrupt */
/*N*///#define TXGBE_EICR_TS		0x00800000 /* Thermal Sensor Event */
/*N*///#define TXGBE_EICR_TIMESYNC	0x01000000 /* Timesync Event */
/*N*///#define TXGBE_EICR_GPI_SDP0	0x01000000 /* Gen Purpose Interrupt on SDP0 */
/*N*///#define TXGBE_EICR_GPI_SDP1	0x02000000 /* Gen Purpose Interrupt on SDP1 */
/*N*///#define TXGBE_EICR_GPI_SDP2	0x04000000 /* Gen Purpose Interrupt on SDP2 */
/*N*///#define TXGBE_EICR_ECC		0x10000000 /* ECC Error */
/*N*///#define TXGBE_EICR_GPI_SDP0_X540 0x02000000 /* Gen Purpose Interrupt on SDP0 */
/*N*///#define TXGBE_EICR_GPI_SDP1_X540 0x04000000 /* Gen Purpose Interrupt on SDP1 */
/*N*///#define TXGBE_EICR_GPI_SDP2_X540 0x08000000 /* Gen Purpose Interrupt on SDP2 */
/*N*///#define TXGBE_EICR_GPI_SDP0_X550	TXGBE_EICR_GPI_SDP0_X540
/*N*///#define TXGBE_EICR_GPI_SDP1_X550	TXGBE_EICR_GPI_SDP1_X540
/*N*///#define TXGBE_EICR_GPI_SDP2_X550	TXGBE_EICR_GPI_SDP2_X540
/*N*///#define TXGBE_EICR_GPI_SDP0_X550EM_x	TXGBE_EICR_GPI_SDP0_X540
/*N*///#define TXGBE_EICR_GPI_SDP1_X550EM_x	TXGBE_EICR_GPI_SDP1_X540
/*N*///#define TXGBE_EICR_GPI_SDP2_X550EM_x	TXGBE_EICR_GPI_SDP2_X540
/*N*///#define TXGBE_EICR_GPI_SDP0_X550EM_a	TXGBE_EICR_GPI_SDP0_X540
/*N*///#define TXGBE_EICR_GPI_SDP1_X550EM_a	TXGBE_EICR_GPI_SDP1_X540
/*N*///#define TXGBE_EICR_GPI_SDP2_X550EM_a	TXGBE_EICR_GPI_SDP2_X540
/*N*///#define TXGBE_EICR_GPI_SDP0_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), EICR_GPI_SDP0)
/*N*///#define TXGBE_EICR_GPI_SDP1_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), EICR_GPI_SDP1)
/*N*///#define TXGBE_EICR_GPI_SDP2_BY_MAC(_hw)	TXGBE_BY_MAC((_hw), EICR_GPI_SDP2)

/*N*///#define TXGBE_EICR_PBUR		0x10000000 /* Packet Buffer Handler Error */
/*N*///#define TXGBE_EICR_DHER		0x20000000 /* Descriptor Handler Error */
/*N*///#define TXGBE_EICR_TCP_TIMER	0x40000000 /* TCP Timer */
/*N*///#define TXGBE_EICR_OTHER	0x80000000 /* Interrupt Cause Active */

/* Extended Interrupt Cause Set */
/*N*///#define TXGBE_EICS_RTX_QUEUE	TXGBE_EICR_RTX_QUEUE /* RTx Queue Interrupt */
/*N*///#define TXGBE_EICS_FLOW_DIR	TXGBE_EICR_FLOW_DIR  /* FDir Exception */
/*N*///#define TXGBE_EICS_RX_MISS	TXGBE_EICR_RX_MISS   /* Pkt Buffer Overrun */
/*N*///#define TXGBE_EICS_PCI		TXGBE_EICR_PCI /* PCI Exception */
/*N*///#define TXGBE_EICS_MAILBOX	TXGBE_EICR_MAILBOX   /* VF to PF Mailbox Int */
/*N*///#define TXGBE_EICS_LSC		TXGBE_EICR_LSC /* Link Status Change */
/*N*///#define TXGBE_EICS_MNG		TXGBE_EICR_MNG /* MNG Event Interrupt */
/*N*///#define TXGBE_EICS_TIMESYNC	TXGBE_EICR_TIMESYNC /* Timesync Event */
/*N*///#define TXGBE_EICS_GPI_SDP0	TXGBE_EICR_GPI_SDP0 /* SDP0 Gen Purpose Int */
/*N*///#define TXGBE_EICS_GPI_SDP1	TXGBE_EICR_GPI_SDP1 /* SDP1 Gen Purpose Int */
/*N*///#define TXGBE_EICS_GPI_SDP2	TXGBE_EICR_GPI_SDP2 /* SDP2 Gen Purpose Int */
/*N*///#define TXGBE_EICS_ECC		TXGBE_EICR_ECC /* ECC Error */
/*N*///#define TXGBE_EICS_GPI_SDP0_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP0_BY_MAC(_hw)
/*N*///#define TXGBE_EICS_GPI_SDP1_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP1_BY_MAC(_hw)
/*N*///#define TXGBE_EICS_GPI_SDP2_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP2_BY_MAC(_hw)
/*N*///#define TXGBE_EICS_PBUR		TXGBE_EICR_PBUR /* Pkt Buf Handler Err */
/*N*///#define TXGBE_EICS_DHER		TXGBE_EICR_DHER /* Desc Handler Error */
/*N*///#define TXGBE_EICS_TCP_TIMER	TXGBE_EICR_TCP_TIMER /* TCP Timer */
/*N*///#define TXGBE_EICS_OTHER	TXGBE_EICR_OTHER /* INT Cause Active */

/* Extended Interrupt Mask Set */
/*N*///#define TXGBE_EIMS_RTX_QUEUE	TXGBE_EICR_RTX_QUEUE /* RTx Queue Interrupt */
/*N*///#define TXGBE_EIMS_FLOW_DIR	TXGBE_EICR_FLOW_DIR /* FDir Exception */
/*N*///#define TXGBE_EIMS_RX_MISS	TXGBE_EICR_RX_MISS /* Packet Buffer Overrun */
/*N*///#define TXGBE_EIMS_PCI		TXGBE_EICR_PCI /* PCI Exception */
/*N*///#define TXGBE_EIMS_MAILBOX	TXGBE_EICR_MAILBOX   /* VF to PF Mailbox Int */
/*N*///#define TXGBE_EIMS_LSC		TXGBE_EICR_LSC /* Link Status Change */
/*N*///#define TXGBE_EIMS_MNG		TXGBE_EICR_MNG /* MNG Event Interrupt */
/*N*///#define TXGBE_EIMS_TS		TXGBE_EICR_TS /* Thermal Sensor Event */
/*N*///#define TXGBE_EIMS_TIMESYNC	TXGBE_EICR_TIMESYNC /* Timesync Event */
/*N*///#define TXGBE_EIMS_GPI_SDP0	TXGBE_EICR_GPI_SDP0 /* SDP0 Gen Purpose Int */
/*N*///#define TXGBE_EIMS_GPI_SDP1	TXGBE_EICR_GPI_SDP1 /* SDP1 Gen Purpose Int */
/*N*///#define TXGBE_EIMS_GPI_SDP2	TXGBE_EICR_GPI_SDP2 /* SDP2 Gen Purpose Int */
/*N*///#define TXGBE_EIMS_ECC		TXGBE_EICR_ECC /* ECC Error */
/*N*///#define TXGBE_EIMS_GPI_SDP0_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP0_BY_MAC(_hw)
/*N*///#define TXGBE_EIMS_GPI_SDP1_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP1_BY_MAC(_hw)
/*N*///#define TXGBE_EIMS_GPI_SDP2_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP2_BY_MAC(_hw)
/*N*///#define TXGBE_EIMS_PBUR		TXGBE_EICR_PBUR /* Pkt Buf Handler Err */
/*N*///#define TXGBE_EIMS_DHER		TXGBE_EICR_DHER /* Descr Handler Error */
/*N*///#define TXGBE_EIMS_TCP_TIMER	TXGBE_EICR_TCP_TIMER /* TCP Timer */
/*N*///#define TXGBE_EIMS_OTHER	TXGBE_EICR_OTHER /* INT Cause Active */

/* Extended Interrupt Mask Clear */
/*N*///#define TXGBE_EIMC_RTX_QUEUE	TXGBE_EICR_RTX_QUEUE /* RTx Queue Interrupt */
/*N*///#define TXGBE_EIMC_FLOW_DIR	TXGBE_EICR_FLOW_DIR /* FDir Exception */
/*N*///#define TXGBE_EIMC_RX_MISS	TXGBE_EICR_RX_MISS /* Packet Buffer Overrun */
/*N*///#define TXGBE_EIMC_PCI		TXGBE_EICR_PCI /* PCI Exception */
/*N*///#define TXGBE_EIMC_MAILBOX	TXGBE_EICR_MAILBOX /* VF to PF Mailbox Int */
/*N*///#define TXGBE_EIMC_LSC		TXGBE_EICR_LSC /* Link Status Change */
/*N*///#define TXGBE_EIMC_MNG		TXGBE_EICR_MNG /* MNG Event Interrupt */
/*N*///#define TXGBE_EIMC_TIMESYNC	TXGBE_EICR_TIMESYNC /* Timesync Event */
/*N*///#define TXGBE_EIMC_GPI_SDP0	TXGBE_EICR_GPI_SDP0 /* SDP0 Gen Purpose Int */
/*N*///#define TXGBE_EIMC_GPI_SDP1	TXGBE_EICR_GPI_SDP1 /* SDP1 Gen Purpose Int */
/*N*///#define TXGBE_EIMC_GPI_SDP2	TXGBE_EICR_GPI_SDP2  /* SDP2 Gen Purpose Int */
/*N*///#define TXGBE_EIMC_ECC		TXGBE_EICR_ECC /* ECC Error */
/*N*///#define TXGBE_EIMC_GPI_SDP0_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP0_BY_MAC(_hw)
/*N*///#define TXGBE_EIMC_GPI_SDP1_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP1_BY_MAC(_hw)
/*N*///#define TXGBE_EIMC_GPI_SDP2_BY_MAC(_hw)	TXGBE_EICR_GPI_SDP2_BY_MAC(_hw)
/*N*///#define TXGBE_EIMC_PBUR		TXGBE_EICR_PBUR /* Pkt Buf Handler Err */
/*N*///#define TXGBE_EIMC_DHER		TXGBE_EICR_DHER /* Desc Handler Err */
/*N*///#define TXGBE_EIMC_TCP_TIMER	TXGBE_EICR_TCP_TIMER /* TCP Timer */
/*N*///#define TXGBE_EIMC_OTHER	TXGBE_EICR_OTHER /* INT Cause Active */

/*N*//*#define TXGBE_EIMS_ENABLE_MASK ( \
				TXGBE_EIMS_RTX_QUEUE	| \
				TXGBE_EIMS_LSC		| \
				TXGBE_EIMS_TCP_TIMER	| \
				TXGBE_EIMS_OTHER)*/

/* Immediate Interrupt Rx (A.K.A. Low Latency Interrupt) */
/*N*///#define TXGBE_IMIR_PORT_IM_EN	0x00010000  /* TCP port enable */
/*N*///#define TXGBE_IMIR_PORT_BP	0x00020000  /* TCP port check bypass */
/*N*///#define TXGBE_IMIREXT_SIZE_BP	0x00001000  /* Packet size bypass */
/*N*///#define TXGBE_IMIREXT_CTRL_URG	0x00002000  /* Check URG bit in header */
/*N*///#define TXGBE_IMIREXT_CTRL_ACK	0x00004000  /* Check ACK bit in header */
/*N*///#define TXGBE_IMIREXT_CTRL_PSH	0x00008000  /* Check PSH bit in header */
/*N*///#define TXGBE_IMIREXT_CTRL_RST	0x00010000  /* Check RST bit in header */
/*N*///#define TXGBE_IMIREXT_CTRL_SYN	0x00020000  /* Check SYN bit in header */
/*N*///#define TXGBE_IMIREXT_CTRL_FIN	0x00040000  /* Check FIN bit in header */
/*N*///#define TXGBE_IMIREXT_CTRL_BP	0x00080000  /* Bypass check of control bits */
/*N*///#define TXGBE_IMIR_SIZE_BP_82599	0x00001000 /* Packet size bypass */
/*N*///#define TXGBE_IMIR_CTRL_URG_82599	0x00002000 /* Check URG bit in header */
/*N*///#define TXGBE_IMIR_CTRL_ACK_82599	0x00004000 /* Check ACK bit in header */
/*N*///#define TXGBE_IMIR_CTRL_PSH_82599	0x00008000 /* Check PSH bit in header */
/*N*///#define TXGBE_IMIR_CTRL_RST_82599	0x00010000 /* Check RST bit in header */
/*N*///#define TXGBE_IMIR_CTRL_SYN_82599	0x00020000 /* Check SYN bit in header */
/*N*///#define TXGBE_IMIR_CTRL_FIN_82599	0x00040000 /* Check FIN bit in header */
/*N*///#define TXGBE_IMIR_CTRL_BP_82599	0x00080000 /* Bypass chk of ctrl bits */
/*N*///#define TXGBE_IMIR_LLI_EN_82599		0x00100000 /* Enables low latency Int */
/*N*///#define TXGBE_IMIR_RX_QUEUE_MASK_82599	0x0000007F /* Rx Queue Mask */
/*N*///#define TXGBE_IMIR_RX_QUEUE_SHIFT_82599	21 /* Rx Queue Shift */
/*N*///#define TXGBE_IMIRVP_PRIORITY_MASK	0x00000007 /* VLAN priority mask */
/*N*///#define TXGBE_IMIRVP_PRIORITY_EN	0x00000008 /* VLAN priority enable */

/*N*///#define TXGBE_MAX_FTQF_FILTERS		128
/*N*///#define TXGBE_FTQF_PROTOCOL_MASK	0x00000003
/*N*///#define TXGBE_FTQF_PROTOCOL_TCP		0x00000000
/*N*///#define TXGBE_FTQF_PROTOCOL_UDP		0x00000001
/*N*///#define TXGBE_FTQF_PROTOCOL_SCTP	2
/*N*///#define TXGBE_FTQF_PRIORITY_MASK	0x00000007
/*N*///#define TXGBE_FTQF_PRIORITY_SHIFT	2
/*N*///#define TXGBE_FTQF_POOL_MASK		0x0000003F
/*N*///#define TXGBE_FTQF_POOL_SHIFT		8
/*N*///#define TXGBE_FTQF_5TUPLE_MASK_MASK	0x0000001F
/*N*///#define TXGBE_FTQF_5TUPLE_MASK_SHIFT	25
/*N*///#define TXGBE_FTQF_SOURCE_ADDR_MASK	0x1E
/*N*///#define TXGBE_FTQF_DEST_ADDR_MASK	0x1D
/*N*///#define TXGBE_FTQF_SOURCE_PORT_MASK	0x1B
/*N*///#define TXGBE_FTQF_DEST_PORT_MASK	0x17
/*N*///#define TXGBE_FTQF_PROTOCOL_COMP_MASK	0x0F
/*N*///#define TXGBE_FTQF_POOL_MASK_EN		0x40000000
/*N*///#define TXGBE_FTQF_QUEUE_ENABLE		0x80000000

/* Interrupt clear mask */
/*N*///#define TXGBE_IRQ_CLEAR_MASK	0xFFFFFFFF

/* Interrupt Vector Allocation Registers */
/*N*///#define TXGBE_IVAR_REG_NUM		25
/*N*///#define TXGBE_IVAR_REG_NUM_82599	64
/*N*///#define TXGBE_IVAR_TXRX_ENTRY		96
/*N*///#define TXGBE_IVAR_RX_ENTRY		64
/*N*///#define TXGBE_IVAR_RX_QUEUE(_i)		(0 + (_i))
/*N*///#define TXGBE_IVAR_TX_QUEUE(_i)		(64 + (_i))
/*N*///#define TXGBE_IVAR_TX_ENTRY		32

/*N*///#define TXGBE_IVAR_TCP_TIMER_INDEX	96 /* 0 based index */
/*N*///#define TXGBE_IVAR_OTHER_CAUSES_INDEX	97 /* 0 based index */

/*N*///#define TXGBE_MSIX_VECTOR(_i)		(0 + (_i))

/*N*///#define TXGBE_IVAR_ALLOC_VAL		0x80 /* Interrupt Allocation valid */

/* ETYPE Queue Filter/Select Bit Masks */
/*N*///#define TXGBE_MAX_ETQF_FILTERS		8
/*N*///#define TXGBE_ETQF_FCOE			0x08000000 /* bit 27 */
/*N*///#define TXGBE_ETQF_BCN			0x10000000 /* bit 28 */
/*N*///#define TXGBE_ETQF_TX_ANTISPOOF		0x20000000 /* bit 29 */
/*N*///#define TXGBE_ETQF_1588			0x40000000 /* bit 30 */
/*N*///#define TXGBE_ETQF_FILTER_EN		0x80000000 /* bit 31 */
/*N*///#define TXGBE_ETQF_POOL_ENABLE		(1 << 26) /* bit 26 */
/*N*///#define TXGBE_ETQF_POOL_SHIFT		20

/*N*///#define TXGBE_ETQS_RX_QUEUE		0x007F0000 /* bits 22:16 */
/*N*///#define TXGBE_ETQS_RX_QUEUE_SHIFT	16
/*N*///#define TXGBE_ETQS_LLI			0x20000000 /* bit 29 */
/*N*///#define TXGBE_ETQS_QUEUE_EN		0x80000000 /* bit 31 */

/*
 * ETQF filter list: one static filter per filter consumer. This is
 *		   to avoid filter collisions later. Add new filters
 *		   here!!
 *
 * Current filters:
 *	EAPOL 802.1x (0x888e): Filter 0
 *	FCoE (0x8906):	 Filter 2
 *	1588 (0x88f7):	 Filter 3
 *	FIP  (0x8914):	 Filter 4
 *	LLDP (0x88CC):	 Filter 5
 *	LACP (0x8809):	 Filter 6
 *	FC   (0x8808):	 Filter 7
 */
/*N*///#define TXGBE_ETQF_FILTER_EAPOL		0
/*N*///#define TXGBE_ETQF_FILTER_FCOE		2
/*N*///#define TXGBE_ETQF_FILTER_1588		3
/*N*///#define TXGBE_ETQF_FILTER_FIP		4
/*N*///#define TXGBE_ETQF_FILTER_LLDP		5
/*N*///#define TXGBE_ETQF_FILTER_LACP		6
/*N*///#define TXGBE_ETQF_FILTER_FC		7
/* VLAN Control Bit Masks */
/*N*///#define TXGBE_VLNCTRL_VET		0x0000FFFF  /* bits 0-15 */
/*N*///#define TXGBE_VLNCTRL_CFI		0x10000000  /* bit 28 */
/*N*///#define TXGBE_VLNCTRL_CFIEN		0x20000000  /* bit 29 */
/*N*///#define TXGBE_VLNCTRL_VFE		0x40000000  /* bit 30 */
/*N*///#define TXGBE_VLNCTRL_VME		0x80000000  /* bit 31 */

/* VLAN pool filtering masks */
/*N*///#define TXGBE_VLVF_VIEN			0x80000000  /* filter is valid */
/*N*///#define TXGBE_VLVF_ENTRIES		64
/*N*///#define TXGBE_VLVF_VLANID_MASK		0x00000FFF
/* Per VF Port VLAN insertion rules */
/*N*///#define TXGBE_VMVIR_VLANA_DEFAULT	0x40000000 /* Always use default VLAN */
/*N*///#define TXGBE_VMVIR_VLANA_NEVER		0x80000000 /* Never insert VLAN tag */

/*N*///#define TXGBE_ETHERNET_IEEE_VLAN_TYPE	0x8100  /* 802.1q protocol */

/* STATUS Bit Masks */
/*N*///#define TXGBE_STATUS_LAN_ID		0x0000000C /* LAN ID */
/*N*///#define TXGBE_STATUS_LAN_ID_SHIFT	2 /* LAN ID Shift*/
/*N*///#define TXGBE_STATUS_GIO		0x00080000 /* GIO Master Ena Status */

/*N*///#define TXGBE_STATUS_LAN_ID_0	0x00000000 /* LAN ID 0 */
/*N*///#define TXGBE_STATUS_LAN_ID_1	0x00000004 /* LAN ID 1 */

/* ESDP Bit Masks */
/*N*///#define TXGBE_ESDP_SDP0		0x00000001 /* SDP0 Data Value */
/*N*///#define TXGBE_ESDP_SDP1		0x00000002 /* SDP1 Data Value */
/*N*///#define TXGBE_ESDP_SDP2		0x00000004 /* SDP2 Data Value */
/*N*///#define TXGBE_ESDP_SDP3		0x00000008 /* SDP3 Data Value */
/*N*///#define TXGBE_ESDP_SDP4		0x00000010 /* SDP4 Data Value */
/*N*///#define TXGBE_ESDP_SDP5		0x00000020 /* SDP5 Data Value */
/*N*///#define TXGBE_ESDP_SDP6		0x00000040 /* SDP6 Data Value */
/*N*///#define TXGBE_ESDP_SDP7		0x00000080 /* SDP7 Data Value */
/*N*///#define TXGBE_ESDP_SDP0_DIR	0x00000100 /* SDP0 IO direction */
/*N*///#define TXGBE_ESDP_SDP1_DIR	0x00000200 /* SDP1 IO direction */
/*N*///#define TXGBE_ESDP_SDP2_DIR	0x00000400 /* SDP1 IO direction */
/*N*///#define TXGBE_ESDP_SDP3_DIR	0x00000800 /* SDP3 IO direction */
/*N*///#define TXGBE_ESDP_SDP4_DIR	0x00001000 /* SDP4 IO direction */
/*N*///#define TXGBE_ESDP_SDP5_DIR	0x00002000 /* SDP5 IO direction */
/*N*///#define TXGBE_ESDP_SDP6_DIR	0x00004000 /* SDP6 IO direction */
/*N*///#define TXGBE_ESDP_SDP7_DIR	0x00008000 /* SDP7 IO direction */
/*N*///#define TXGBE_ESDP_SDP0_NATIVE	0x00010000 /* SDP0 IO mode */
/*N*///#define TXGBE_ESDP_SDP1_NATIVE	0x00020000 /* SDP1 IO mode */


/* LEDCTL Bit Masks */
/*N*///#define TXGBE_LED_IVRT_BASE		0x00000040
/*N*///#define TXGBE_LED_BLINK_BASE		0x00000080
/*N*///#define TXGBE_LED_MODE_MASK_BASE	0x0000000F
/*N*///#define TXGBE_LED_OFFSET(_base, _i)	(_base << (8 * (_i)))
/*N*///#define TXGBE_LED_MODE_SHIFT(_i)	(8*(_i))
/*N*///#define TXGBE_LED_IVRT(_i)	TXGBE_LED_OFFSET(TXGBE_LED_IVRT_BASE, _i)
/*N*///#define TXGBE_LED_BLINK(_i)	TXGBE_LED_OFFSET(TXGBE_LED_BLINK_BASE, _i)
/*N*///#define TXGBE_LED_MODE_MASK(_i)	TXGBE_LED_OFFSET(TXGBE_LED_MODE_MASK_BASE, _i)
/*N*///#define TXGBE_X557_LED_MANUAL_SET_MASK	(1 << 8)
/*N*///#define TXGBE_X557_MAX_LED_INDEX	3
/*N*///#define TXGBE_X557_LED_PROVISIONING	0xC430

/* LED modes */
/*N*///#define TXGBE_LED_LINK_UP	0x0
/*N*///#define TXGBE_LED_LINK_10G	0x1
/*N*///#define TXGBE_LED_MAC		0x2
/*N*///#define TXGBE_LED_FILTER	0x3
/*N*///#define TXGBE_LED_LINK_ACTIVE	0x4
/*N*///#define TXGBE_LED_LINK_1G	0x5
/*N*///#define TXGBE_LED_ON		0xE
/*N*///#define TXGBE_LED_OFF		0xF

/* AUTOC Bit Masks */
/*N*///#define TXGBE_AUTOC_KX4_KX_SUPP_MASK 0xC0000000
/*N*///#define TXGBE_AUTOC_KX4_SUPP	0x80000000
/*N*///#define TXGBE_AUTOC_KX_SUPP	0x40000000
/*N*///#define TXGBE_AUTOC_PAUSE	0x30000000
/*N*///#define TXGBE_AUTOC_ASM_PAUSE	0x20000000
/*N*///#define TXGBE_AUTOC_SYM_PAUSE	0x10000000
/*N*///#define TXGBE_AUTOC_RF		0x08000000
/*N*///#define TXGBE_AUTOC_PD_TMR	0x06000000
/*N*///#define TXGBE_AUTOC_AN_RX_LOOSE	0x01000000
/*N*///#define TXGBE_AUTOC_AN_RX_DRIFT	0x00800000
/*N*///#define TXGBE_AUTOC_AN_RX_ALIGN	0x007C0000
/*N*///#define TXGBE_AUTOC_FECA	0x00040000
/*N*///#define TXGBE_AUTOC_FECR	0x00020000
/*N*///#define TXGBE_AUTOC_KR_SUPP	0x00010000
/*N*///#define TXGBE_AUTOC_AN_RESTART	0x00001000
/*N*///#define TXGBE_AUTOC_FLU		0x00000001
/*N*///#define TXGBE_AUTOC_LMS_SHIFT	13
/*N*///#define TXGBE_AUTOC_LMS_10G_SERIAL	(0x3 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_KX4_KX_KR	(0x4 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_SGMII_1G_100M	(0x5 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_KX4_KX_KR_1G_AN	(0x6 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_KX4_KX_KR_SGMII	(0x7 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_MASK		(0x7 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_1G_LINK_NO_AN	(0x0 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_10G_LINK_NO_AN	(0x1 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_1G_AN		(0x2 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_KX4_AN		(0x4 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_KX4_AN_1G_AN	(0x6 << TXGBE_AUTOC_LMS_SHIFT)
/*N*///#define TXGBE_AUTOC_LMS_ATTACH_TYPE	(0x7 << TXGBE_AUTOC_10G_PMA_PMD_SHIFT)

/*N*///#define TXGBE_AUTOC_1G_PMA_PMD_MASK	0x00000200
/*N*///#define TXGBE_AUTOC_1G_PMA_PMD_SHIFT	9
/*N*///#define TXGBE_AUTOC_10G_PMA_PMD_MASK	0x00000180
/*N*///#define TXGBE_AUTOC_10G_PMA_PMD_SHIFT	7
/*N*///#define TXGBE_AUTOC_10G_XAUI	(0x0 << TXGBE_AUTOC_10G_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC_10G_KX4	(0x1 << TXGBE_AUTOC_10G_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC_10G_CX4	(0x2 << TXGBE_AUTOC_10G_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC_1G_BX	(0x0 << TXGBE_AUTOC_1G_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC_1G_KX	(0x1 << TXGBE_AUTOC_1G_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC_1G_SFI	(0x0 << TXGBE_AUTOC_1G_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC_1G_KX_BX	(0x1 << TXGBE_AUTOC_1G_PMA_PMD_SHIFT)

/*N*///#define TXGBE_AUTOC2_UPPER_MASK	0xFFFF0000
/*N*///#define TXGBE_AUTOC2_10G_SERIAL_PMA_PMD_MASK	0x00030000
/*N*///#define TXGBE_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT	16
/*N*///#define TXGBE_AUTOC2_10G_KR	(0x0 << TXGBE_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC2_10G_XFI	(0x1 << TXGBE_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC2_10G_SFI	(0x2 << TXGBE_AUTOC2_10G_SERIAL_PMA_PMD_SHIFT)
/*N*///#define TXGBE_AUTOC2_LINK_DISABLE_ON_D3_MASK	0x50000000
/*N*///#define TXGBE_AUTOC2_LINK_DISABLE_MASK		0x70000000

/*N*///#define TXGBE_MACC_FLU		0x00000001
/*N*///#define TXGBE_MACC_FSV_10G	0x00030000
/*N*///#define TXGBE_MACC_FS		0x00040000
/*N*///#define TXGBE_MAC_RX2TX_LPBK	0x00000002

/* Veto Bit definiton */
/*N*///#define TXGBE_MMNGC_MNG_VETO	0x00000001

/* LINKS Bit Masks */
/*N*///#define TXGBE_LINKS_KX_AN_COMP	0x80000000
/*N*///#define TXGBE_LINKS_UP		0x40000000
/*N*///#define TXGBE_LINKS_SPEED	0x20000000
/*N*///#define TXGBE_LINKS_MODE	0x18000000
/*N*///#define TXGBE_LINKS_RX_MODE	0x06000000
/*N*///#define TXGBE_LINKS_TX_MODE	0x01800000
/*N*///#define TXGBE_LINKS_XGXS_EN	0x00400000
/*N*///#define TXGBE_LINKS_SGMII_EN	0x02000000
/*N*///#define TXGBE_LINKS_PCS_1G_EN	0x00200000
/*N*///#define TXGBE_LINKS_1G_AN_EN	0x00100000
/*N*///#define TXGBE_LINKS_KX_AN_IDLE	0x00080000
/*N*///#define TXGBE_LINKS_1G_SYNC	0x00040000
/*N*///#define TXGBE_LINKS_10G_ALIGN	0x00020000
/*N*///#define TXGBE_LINKS_10G_LANE_SYNC	0x00017000
/*N*///#define TXGBE_LINKS_TL_FAULT		0x00001000
/*N*///#define TXGBE_LINKS_SIGNAL		0x00000F00

/*N*///#define TXGBE_LINKS_SPEED_NON_STD	0x08000000
/*N*///#define TXGBE_LINKS_SPEED_82599		0x30000000
/*N*///#define TXGBE_LINKS_SPEED_10G_82599	0x30000000
/*N*///#define TXGBE_LINKS_SPEED_1G_82599	0x20000000
/*N*///#define TXGBE_LINKS_SPEED_100_82599	0x10000000
/*N*///#define TXGBE_LINKS_SPEED_10_X550EM_A	0x00000000
/*N*///#define TXGBE_LINK_UP_TIME		90 /* 9.0 Seconds */
/*N*///#define TXGBE_AUTO_NEG_TIME		45 /* 4.5 Seconds */

/*N*///#define TXGBE_LINKS2_AN_SUPPORTED	0x00000040

/* PCS1GLSTA Bit Masks */
/*N*///#define TXGBE_PCS1GLSTA_LINK_OK		1
/*N*///#define TXGBE_PCS1GLSTA_SYNK_OK		0x10
/*N*///#define TXGBE_PCS1GLSTA_AN_COMPLETE	0x10000
/*N*///#define TXGBE_PCS1GLSTA_AN_PAGE_RX	0x20000
/*N*///#define TXGBE_PCS1GLSTA_AN_TIMED_OUT	0x40000
/*N*///#define TXGBE_PCS1GLSTA_AN_REMOTE_FAULT	0x80000
/*N*///#define TXGBE_PCS1GLSTA_AN_ERROR_RWS	0x100000

/*N*///#define TXGBE_PCS1GANA_SYM_PAUSE	0x80
/*N*///#define TXGBE_PCS1GANA_ASM_PAUSE	0x100

/* PCS1GLCTL Bit Masks */
/*N*///#define TXGBE_PCS1GLCTL_AN_1G_TIMEOUT_EN 0x00040000 /* PCS 1G autoneg to en */
/*N*///#define TXGBE_PCS1GLCTL_FLV_LINK_UP	1
/*N*///#define TXGBE_PCS1GLCTL_FORCE_LINK	0x20
/*N*///#define TXGBE_PCS1GLCTL_LOW_LINK_LATCH	0x40
/*N*///#define TXGBE_PCS1GLCTL_AN_ENABLE	0x10000
/*N*///#define TXGBE_PCS1GLCTL_AN_RESTART	0x20000

/* ANLP1 Bit Masks */
/*N*///#define TXGBE_ANLP1_PAUSE		0x0C00
/*N*///#define TXGBE_ANLP1_SYM_PAUSE		0x0400
/*N*///#define TXGBE_ANLP1_ASM_PAUSE		0x0800
/*N*///#define TXGBE_ANLP1_AN_STATE_MASK	0x000f0000

/* SW Semaphore Register bitmasks */
/*N*///#define TXGBE_SWSM_SMBI		0x00000001 /* Driver Semaphore bit */
/*N*///#define TXGBE_SWSM_SWESMBI	0x00000002 /* FW Semaphore bit */
/*N*///#define TXGBE_SWSM_WMNG		0x00000004 /* Wake MNG Clock */
/*N*///#define TXGBE_SWFW_REGSMP	0x80000000 /* Register Semaphore bit 31 */

/* SW_FW_SYNC/GSSR definitions */
/*N*///#define TXGBE_GSSR_EEP_SM		0x0001
/*N*///#define TXGBE_GSSR_PHY0_SM		0x0002
/*N*///#define TXGBE_GSSR_PHY1_SM		0x0004
/*N*///#define TXGBE_GSSR_MAC_CSR_SM		0x0008
/*N*///#define TXGBE_GSSR_FLASH_SM		0x0010
/*N*///#define TXGBE_GSSR_NVM_UPDATE_SM	0x0200
/*N*///#define TXGBE_GSSR_SW_MNG_SM		0x0400
/*N*///#define TXGBE_GSSR_TOKEN_SM	0x40000000 /* SW bit for shared access */
/*N*///#define TXGBE_GSSR_SHARED_I2C_SM 0x1806 /* Wait for both phys and both I2Cs */
/*N*///#define TXGBE_GSSR_I2C_MASK	0x1800
/*N*///#define TXGBE_GSSR_NVM_PHY_MASK	0xF

/* FW Status register bitmask */
/*N*///#define TXGBE_FWSTS_FWRI	0x00000200 /* Firmware Reset Indication */

/* EEC Register */
/*N*///#define TXGBE_EEC_SK		0x00000001 /* EEPROM Clock */
/*N*///#define TXGBE_EEC_CS		0x00000002 /* EEPROM Chip Select */
/*N*///#define TXGBE_EEC_DI		0x00000004 /* EEPROM Data In */
/*N*///#define TXGBE_EEC_DO		0x00000008 /* EEPROM Data Out */
/*N*///#define TXGBE_EEC_FWE_MASK	0x00000030 /* FLASH Write Enable */
/*N*///#define TXGBE_EEC_FWE_DIS	0x00000010 /* Disable FLASH writes */
/*N*///#define TXGBE_EEC_FWE_EN	0x00000020 /* Enable FLASH writes */
/*N*///#define TXGBE_EEC_FWE_SHIFT	4
/*N*///#define TXGBE_EEC_REQ		0x00000040 /* EEPROM Access Request */
/*N*///#define TXGBE_EEC_GNT		0x00000080 /* EEPROM Access Grant */
/*N*///#define TXGBE_EEC_PRES		0x00000100 /* EEPROM Present */
/*N*///#define TXGBE_EEC_ARD		0x00000200 /* EEPROM Auto Read Done */
/*N*///#define TXGBE_EEC_FLUP		0x00800000 /* Flash update command */
/*N*///#define TXGBE_EEC_SEC1VAL	0x02000000 /* Sector 1 Valid */
/*N*///#define TXGBE_EEC_FLUDONE	0x04000000 /* Flash update done */
/* EEPROM Addressing bits based on type (0-small, 1-large) */
/*N*///#define TXGBE_EEC_ADDR_SIZE	0x00000400
/*N*///#define TXGBE_EEC_SIZE		0x00007800 /* EEPROM Size */
/*N*///#define TXGBE_EERD_MAX_ADDR	0x00003FFF /* EERD alows 14 bits for addr. */

/*N*///#define TXGBE_EEC_SIZE_SHIFT		11
/*N*///#define TXGBE_EEPROM_WORD_SIZE_SHIFT	6
/*N*///#define TXGBE_EEPROM_OPCODE_BITS	8

/* FLA Register */
/*N*///#define TXGBE_FLA_LOCKED	0x00000040

/* Part Number String Length */
/*N*///#define TXGBE_PBANUM_LENGTH	11

/* Checksum and EEPROM pointers */
/*N*///#define TXGBE_PBANUM_PTR_GUARD		0xFAFA
/*N*///#define TXGBE_EEPROM_CHECKSUM		0x3F
/*N*///#define TXGBE_EEPROM_SUM		0xBABA
/*N*///#define TXGBE_EEPROM_CTRL_4		0x45
/*N*///#define TXGBE_EE_CTRL_4_INST_ID		0x10
/*N*///#define TXGBE_EE_CTRL_4_INST_ID_SHIFT	4
/*N*///#define TXGBE_PCIE_ANALOG_PTR		0x03
/*N*///#define TXGBE_ATLAS0_CONFIG_PTR		0x04
/*N*///#define TXGBE_PHY_PTR			0x04
/*N*///#define TXGBE_ATLAS1_CONFIG_PTR		0x05
/*N*///#define TXGBE_OPTION_ROM_PTR		0x05
/*N*///#define TXGBE_PCIE_GENERAL_PTR		0x06
/*N*///#define TXGBE_PCIE_CONFIG0_PTR		0x07
/*N*///#define TXGBE_PCIE_CONFIG1_PTR		0x08
/*N*///#define TXGBE_CORE0_PTR			0x09
/*N*///#define TXGBE_CORE1_PTR			0x0A
/*N*///#define TXGBE_MAC0_PTR			0x0B
/*N*///#define TXGBE_MAC1_PTR			0x0C
/*N*///#define TXGBE_CSR0_CONFIG_PTR		0x0D
/*N*///#define TXGBE_CSR1_CONFIG_PTR		0x0E
/*N*///#define TXGBE_PCIE_ANALOG_PTR_X550	0x02
/*N*///#define TXGBE_SHADOW_RAM_SIZE_X550	0x4000
/*N*///#define TXGBE_TXGBE_PCIE_GENERAL_SIZE	0x24
/*N*///#define TXGBE_PCIE_CONFIG_SIZE		0x08
/*N*///#define TXGBE_EEPROM_LAST_WORD		0x41
/*N*///#define TXGBE_FW_PTR			0x0F
/*N*///#define TXGBE_PBANUM0_PTR		0x15
/*N*///#define TXGBE_PBANUM1_PTR		0x16
/*N*///#define TXGBE_ALT_MAC_ADDR_PTR		0x37
/*N*///#define TXGBE_FREE_SPACE_PTR		0X3E

/* External Thermal Sensor Config */
/*N*///#define TXGBE_ETS_CFG			0x26
/*N*///#define TXGBE_ETS_LTHRES_DELTA_MASK	0x07C0
/*N*///#define TXGBE_ETS_LTHRES_DELTA_SHIFT	6
/*N*///#define TXGBE_ETS_TYPE_MASK		0x0038
/*N*///#define TXGBE_ETS_TYPE_SHIFT		3
/*N*///#define TXGBE_ETS_TYPE_EMC		0x000
/*N*///#define TXGBE_ETS_NUM_SENSORS_MASK	0x0007
/*N*///#define TXGBE_ETS_DATA_LOC_MASK		0x3C00
/*N*///#define TXGBE_ETS_DATA_LOC_SHIFT	10
/*N*///#define TXGBE_ETS_DATA_INDEX_MASK	0x0300
/*N*///#define TXGBE_ETS_DATA_INDEX_SHIFT	8
/*N*///#define TXGBE_ETS_DATA_HTHRESH_MASK	0x00FF

/*N*///#define TXGBE_SAN_MAC_ADDR_PTR		0x28
/*N*///#define TXGBE_DEVICE_CAPS		0x2C
/*N*///#define TXGBE_82599_SERIAL_NUMBER_MAC_ADDR	0x11
/*N*///#define TXGBE_X550_SERIAL_NUMBER_MAC_ADDR	0x04

/*N*///#define TXGBE_PCIE_MSIX_82599_CAPS	0x72
/*N*///#define TXGBE_MAX_MSIX_VECTORS_82599	0x40
/*N*///#define TXGBE_PCIE_MSIX_82598_CAPS	0x62
/*N*///#define TXGBE_MAX_MSIX_VECTORS_82598	0x13

/* MSI-X capability fields masks */
/*N*///#define TXGBE_PCIE_MSIX_TBL_SZ_MASK	0x7FF

/* Legacy EEPROM word offsets */
/*N*///#define TXGBE_ISCSI_BOOT_CAPS		0x0033
/*N*///#define TXGBE_ISCSI_SETUP_PORT_0	0x0030
/*N*///#define TXGBE_ISCSI_SETUP_PORT_1	0x0034

/* EEPROM Commands - SPI */
/*N*///#define TXGBE_EEPROM_MAX_RETRY_SPI	5000 /* Max wait 5ms for RDY signal */
/*N*///#define TXGBE_EEPROM_STATUS_RDY_SPI	0x01
/*N*///#define TXGBE_EEPROM_READ_OPCODE_SPI	0x03  /* EEPROM read opcode */
/*N*///#define TXGBE_EEPROM_WRITE_OPCODE_SPI	0x02  /* EEPROM write opcode */
/*N*///#define TXGBE_EEPROM_A8_OPCODE_SPI	0x08  /* opcode bit-3 = addr bit-8 */
/*N*///#define TXGBE_EEPROM_WREN_OPCODE_SPI	0x06  /* EEPROM set Write Ena latch */
/* EEPROM reset Write Enable latch */
/*N*///#define TXGBE_EEPROM_WRDI_OPCODE_SPI	0x04
/*N*///#define TXGBE_EEPROM_RDSR_OPCODE_SPI	0x05  /* EEPROM read Status reg */
/*N*///#define TXGBE_EEPROM_WRSR_OPCODE_SPI	0x01  /* EEPROM write Status reg */
/*N*///#define TXGBE_EEPROM_ERASE4K_OPCODE_SPI	0x20  /* EEPROM ERASE 4KB */
/*N*///#define TXGBE_EEPROM_ERASE64K_OPCODE_SPI	0xD8  /* EEPROM ERASE 64KB */
/*N*///#define TXGBE_EEPROM_ERASE256_OPCODE_SPI	0xDB  /* EEPROM ERASE 256B */

/* EEPROM Read Register */
/*N*///#define TXGBE_EEPROM_RW_REG_DATA	16 /* data offset in EEPROM read reg */
/*N*///#define TXGBE_EEPROM_RW_REG_DONE	2 /* Offset to READ done bit */
/*N*///#define TXGBE_EEPROM_RW_REG_START	1 /* First bit to start operation */
/*N*///#define TXGBE_EEPROM_RW_ADDR_SHIFT	2 /* Shift to the address bits */
/*N*///#define TXGBE_NVM_POLL_WRITE		1 /* Flag for polling for wr complete */
/*N*///#define TXGBE_NVM_POLL_READ		0 /* Flag for polling for rd complete */

/*N*///#define NVM_INIT_CTRL_3		0x38
/*N*///#define NVM_INIT_CTRL_3_LPLU	0x8
/*N*///#define NVM_INIT_CTRL_3_D10GMP_PORT0 0x40
/*N*///#define NVM_INIT_CTRL_3_D10GMP_PORT1 0x100

/*N*///#define TXGBE_ETH_LENGTH_OF_ADDRESS	6

/*N*///#define TXGBE_EEPROM_PAGE_SIZE_MAX	128
/*N*///#define TXGBE_EEPROM_RD_BUFFER_MAX_COUNT	256 /* words rd in burst */
/*N*///#define TXGBE_EEPROM_WR_BUFFER_MAX_COUNT	256 /* words wr in burst */
/*N*///#define TXGBE_EEPROM_CTRL_2		1 /* EEPROM CTRL word 2 */
/*N*///#define TXGBE_EEPROM_CCD_BIT		2

/*N*///#ifndef TXGBE_EEPROM_GRANT_ATTEMPTS
/*N*///#define TXGBE_EEPROM_GRANT_ATTEMPTS	1000 /* EEPROM attempts to gain grant */
/*N*///#endif

/* Number of 5 microseconds we wait for EERD read and
 * EERW write to complete */
/*N*///#define TXGBE_EERD_EEWR_ATTEMPTS	100000

/* # attempts we wait for flush update to complete */
/*N*///#define TXGBE_FLUDONE_ATTEMPTS		20000

/*N*///#define TXGBE_PCIE_CTRL2		0x5   /* PCIe Control 2 Offset */
/*N*///#define TXGBE_PCIE_CTRL2_DUMMY_ENABLE	0x8   /* Dummy Function Enable */
/*N*///#define TXGBE_PCIE_CTRL2_LAN_DISABLE	0x2   /* LAN PCI Disable */
/*N*///#define TXGBE_PCIE_CTRL2_DISABLE_SELECT	0x1   /* LAN Disable Select */

/*N*///#define TXGBE_SAN_MAC_ADDR_PORT0_OFFSET		0x0
/*N*///#define TXGBE_SAN_MAC_ADDR_PORT1_OFFSET		0x3
/*N*///#define TXGBE_DEVICE_CAPS_ALLOW_ANY_SFP		0x1
/*N*///#define TXGBE_DEVICE_CAPS_FCOE_OFFLOADS		0x2
/*N*///#define TXGBE_DEVICE_CAPS_NO_CROSSTALK_WR	(1 << 7)
/*N*///#define TXGBE_FW_LESM_PARAMETERS_PTR		0x2
/*N*///#define TXGBE_FW_LESM_STATE_1			0x1
/*N*///#define TXGBE_FW_LESM_STATE_ENABLED		0x8000 /* LESM Enable bit */
/*N*///#define TXGBE_FW_LESM_2_STATES_ENABLED_MASK	0x1F
/*N*///#define TXGBE_FW_LESM_2_STATES_ENABLED		0x12
/*N*///#define TXGBE_FW_LESM_STATE0_10G_ENABLED	0x6FFF
/*N*///#define TXGBE_FW_LESM_STATE1_10G_ENABLED	0x4FFF
/*N*///#define TXGBE_FW_LESM_STATE0_10G_DISABLED	0x0FFF
/*N*///#define TXGBE_FW_LESM_STATE1_10G_DISABLED	0x2FFF
/*N*///#define TXGBE_FW_LESM_PORT0_STATE0_OFFSET	0x2
/*N*///#define TXGBE_FW_LESM_PORT0_STATE1_OFFSET	0x3
/*N*///#define TXGBE_FW_LESM_PORT1_STATE0_OFFSET	0x6
/*N*///#define TXGBE_FW_LESM_PORT1_STATE1_OFFSET	0x7
/*N*///#define TXGBE_FW_PASSTHROUGH_PATCH_CONFIG_PTR	0x4
/*N*///#define TXGBE_FW_PATCH_VERSION_4		0x7
/*N*///#define TXGBE_FCOE_IBA_CAPS_BLK_PTR		0x33 /* iSCSI/FCOE block */
/*N*///#define TXGBE_FCOE_IBA_CAPS_FCOE		0x20 /* FCOE flags */
/*N*///#define TXGBE_ISCSI_FCOE_BLK_PTR		0x17 /* iSCSI/FCOE block */
/*N*///#define TXGBE_ISCSI_FCOE_FLAGS_OFFSET		0x0 /* FCOE flags */
/*N*///#define TXGBE_ISCSI_FCOE_FLAGS_ENABLE		0x1 /* FCOE flags enable bit */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_BLK_PTR		0x27 /* Alt. SAN MAC block */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_CAPS_OFFSET	0x0 /* Alt SAN MAC capability */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_PORT0_OFFSET	0x1 /* Alt SAN MAC 0 offset */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_PORT1_OFFSET	0x4 /* Alt SAN MAC 1 offset */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_WWNN_OFFSET	0x7 /* Alt WWNN prefix offset */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_WWPN_OFFSET	0x8 /* Alt WWPN prefix offset */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_CAPS_SANMAC	0x0 /* Alt SAN MAC exists */
/*N*///#define TXGBE_ALT_SAN_MAC_ADDR_CAPS_ALTWWN	0x1 /* Alt WWN base exists */

/* FW header offset */
/*N*///#define TXGBE_X540_FW_PASSTHROUGH_PATCH_CONFIG_PTR	0x4
/*N*///#define TXGBE_X540_FW_MODULE_MASK			0x7FFF
/* 4KB multiplier */
/*N*///#define TXGBE_X540_FW_MODULE_LENGTH			0x1000
/* version word 2 (month & day) */
/*N*///#define TXGBE_X540_FW_PATCH_VERSION_2		0x5
/* version word 3 (silicon compatibility & year) */
/*N*///#define TXGBE_X540_FW_PATCH_VERSION_3		0x6
/* version word 4 (major & minor numbers) */
/*N*///#define TXGBE_X540_FW_PATCH_VERSION_4		0x7

/*N*///#define TXGBE_DEVICE_CAPS_WOL_PORT0_1	0x4 /* WoL supported on ports 0 & 1 */
/*N*///#define TXGBE_DEVICE_CAPS_WOL_PORT0	0x8 /* WoL supported on port 0 */
/*N*///#define TXGBE_DEVICE_CAPS_WOL_MASK	0xC /* Mask for WoL capabilities */

/* PCI Bus Info */
/*N*///#define TXGBE_PCI_DEVICE_STATUS		0xAA
/*N*///#define TXGBE_PCI_DEVICE_STATUS_TRANSACTION_PENDING	0x0020
/*N*///#define TXGBE_PCI_LINK_STATUS		0xB2
/*N*///#define TXGBE_PCI_DEVICE_CONTROL2	0xC8
/*N*///#define TXGBE_PCI_LINK_WIDTH		0x3F0
/*N*///#define TXGBE_PCI_LINK_WIDTH_1		0x10
/*N*///#define TXGBE_PCI_LINK_WIDTH_2		0x20
/*N*///#define TXGBE_PCI_LINK_WIDTH_4		0x40
/*N*///#define TXGBE_PCI_LINK_WIDTH_8		0x80
/*N*///#define TXGBE_PCI_LINK_SPEED		0xF
/*N*///#define TXGBE_PCI_LINK_SPEED_2500	0x1
/*N*///#define TXGBE_PCI_LINK_SPEED_5000	0x2
/*N*///#define TXGBE_PCI_LINK_SPEED_8000	0x3
/*N*///#define TXGBE_PCI_HEADER_TYPE_REGISTER	0x0E
/*N*///#define TXGBE_PCI_HEADER_TYPE_MULTIFUNC	0x80
/*N*///#define TXGBE_PCI_DEVICE_CONTROL2_16ms	0x0005

/*N*///#define TXGBE_PCIDEVCTRL2_TIMEO_MASK	0xf
/*N*///#define TXGBE_PCIDEVCTRL2_16_32ms_def	0x0
/*N*///#define TXGBE_PCIDEVCTRL2_50_100us	0x1
/*N*///#define TXGBE_PCIDEVCTRL2_1_2ms		0x2
/*N*///#define TXGBE_PCIDEVCTRL2_16_32ms	0x5
/*N*///#define TXGBE_PCIDEVCTRL2_65_130ms	0x6
/*N*///#define TXGBE_PCIDEVCTRL2_260_520ms	0x9
/*N*///#define TXGBE_PCIDEVCTRL2_1_2s		0xa
/*N*///#define TXGBE_PCIDEVCTRL2_4_8s		0xd
/*N*///#define TXGBE_PCIDEVCTRL2_17_34s	0xe

/* Number of 100 microseconds we wait for PCI Express master disable */
/*N*///#define TXGBE_PCI_MASTER_DISABLE_TIMEOUT	800

/* Check whether address is multicast. This is little-endian specific check.*/
/*N*//*#define TXGBE_IS_MULTICAST(Address) \
		(bool)(((u8 *)(Address))[0] & ((u8)0x01))*/

/* Check whether an address is broadcast. */
/*N*//*#define TXGBE_IS_BROADCAST(Address) \
		((((u8 *)(Address))[0] == ((u8)0xff)) && \
		(((u8 *)(Address))[1] == ((u8)0xff)))*/

/* RAH */
/*N*///#define TXGBE_RAH_VIND_MASK	0x003C0000
/*N*///#define TXGBE_RAH_VIND_SHIFT	18
/*N*///#define TXGBE_RAH_AV		0x80000000
/*N*///#define TXGBE_CLEAR_VMDQ_ALL	0xFFFFFFFF

/* Header split receive */
/*N*///#define TXGBE_RFCTL_ISCSI_DIS		0x00000001
/*N*///#define TXGBE_RFCTL_ISCSI_DWC_MASK	0x0000003E
/*N*///#define TXGBE_RFCTL_ISCSI_DWC_SHIFT	1
/*N*///#define TXGBE_RFCTL_RSC_DIS		0x00000020
/*N*///#define TXGBE_RFCTL_NFSW_DIS		0x00000040
/*N*///#define TXGBE_RFCTL_NFSR_DIS		0x00000080
/*N*///#define TXGBE_RFCTL_NFS_VER_MASK	0x00000300
/*N*///#define TXGBE_RFCTL_NFS_VER_SHIFT	8
/*N*///#define TXGBE_RFCTL_NFS_VER_2		0
/*N*///#define TXGBE_RFCTL_NFS_VER_3		1
/*N*///#define TXGBE_RFCTL_NFS_VER_4		2
/*N*///#define TXGBE_RFCTL_IPV6_DIS		0x00000400
/*N*///#define TXGBE_RFCTL_IPV6_XSUM_DIS	0x00000800
/*N*///#define TXGBE_RFCTL_IPFRSP_DIS		0x00004000
/*N*///#define TXGBE_RFCTL_IPV6_EX_DIS		0x00010000
/*N*///#define TXGBE_RFCTL_NEW_IPV6_EXT_DIS	0x00020000

/* Transmit Config masks */
/*N*///#define TXGBE_TXDCTL_ENABLE		0x02000000 /* Ena specific Tx Queue */
/*N*///#define TXGBE_TXDCTL_SWFLSH		0x04000000 /* Tx Desc. wr-bk flushing */
/*N*///#define TXGBE_TXDCTL_WTHRESH_SHIFT	16 /* shift to WTHRESH bits */
/* Enable short packet padding to 64 bytes */
/*N*///#define TXGBE_TX_PAD_ENABLE		0x00000400
/*N*///#define TXGBE_JUMBO_FRAME_ENABLE	0x00000004  /* Allow jumbo frames */
/* This allows for 16K packets + 4k for vlan */
/*N*///#define TXGBE_MAX_FRAME_SZ		0x40040000

/*N*///#define TXGBE_TDWBAL_HEAD_WB_ENABLE	0x1 /* Tx head write-back enable */
/*N*///#define TXGBE_TDWBAL_SEQNUM_WB_ENABLE	0x2 /* Tx seq# write-back enable */

/* Receive Config masks */
/*N*///#define TXGBE_RXCTRL_RXEN		0x00000001 /* Enable Receiver */
/*N*///#define TXGBE_RXCTRL_DMBYPS		0x00000002 /* Desc Monitor Bypass */
/*N*///#define TXGBE_RXDCTL_ENABLE		0x02000000 /* Ena specific Rx Queue */
/*N*///#define TXGBE_RXDCTL_SWFLSH		0x04000000 /* Rx Desc wr-bk flushing */
/*N*///#define TXGBE_RXDCTL_RLPMLMASK		0x00003FFF /* X540 supported only */
/*N*///#define TXGBE_RXDCTL_RLPML_EN		0x00008000
/*N*///#define TXGBE_RXDCTL_VME		0x40000000 /* VLAN mode enable */

/*N*///#define TXGBE_TSAUXC_EN_CLK		0x00000004
/*N*///#define TXGBE_TSAUXC_SYNCLK		0x00000008
/*N*///#define TXGBE_TSAUXC_SDP0_INT		0x00000040
/*N*///#define TXGBE_TSAUXC_EN_TT0		0x00000001
/*N*///#define TXGBE_TSAUXC_EN_TT1		0x00000002
/*N*///#define TXGBE_TSAUXC_ST0		0x00000010
/*N*///#define TXGBE_TSAUXC_DISABLE_SYSTIME	0x80000000

/*N*///#define TXGBE_TSSDP_TS_SDP0_SEL_MASK	0x000000C0
/*N*///#define TXGBE_TSSDP_TS_SDP0_CLK0	0x00000080
/*N*///#define TXGBE_TSSDP_TS_SDP0_EN		0x00000100

/*N*///#define TXGBE_TSYNCTXCTL_VALID		0x00000001 /* Tx timestamp valid */
/*N*///#define TXGBE_TSYNCTXCTL_ENABLED	0x00000010 /* Tx timestamping enabled */

/*N*///#define TXGBE_TSYNCRXCTL_VALID		0x00000001 /* Rx timestamp valid */
/*N*///#define TXGBE_TSYNCRXCTL_TYPE_MASK	0x0000000E /* Rx type mask */
/*N*///#define TXGBE_TSYNCRXCTL_TYPE_L2_V2	0x00
/*N*///#define TXGBE_TSYNCRXCTL_TYPE_L4_V1	0x02
/*N*///#define TXGBE_TSYNCRXCTL_TYPE_L2_L4_V2	0x04
/*N*///#define TXGBE_TSYNCRXCTL_TYPE_ALL	0x08
/*N*///#define TXGBE_TSYNCRXCTL_TYPE_EVENT_V2	0x0A
/*N*///#define TXGBE_TSYNCRXCTL_ENABLED	0x00000010 /* Rx Timestamping enabled */
/*N*///#define TXGBE_TSYNCRXCTL_TSIP_UT_EN	0x00800000 /* Rx Timestamp in Packet */
/*N*///#define TXGBE_TSYNCRXCTL_TSIP_UP_MASK	0xFF000000 /* Rx Timestamp UP Mask */

/*N*///#define TXGBE_TSIM_SYS_WRAP		0x00000001
/*N*///#define TXGBE_TSIM_TXTS			0x00000002
/*N*///#define TXGBE_TSIM_TADJ			0x00000080

/*N*///#define TXGBE_TSICR_SYS_WRAP		TXGBE_TSIM_SYS_WRAP
/*N*///#define TXGBE_TSICR_TXTS		TXGBE_TSIM_TXTS
/*N*///#define TXGBE_TSICR_TADJ		TXGBE_TSIM_TADJ

/*N*///#define TXGBE_RXMTRL_V1_CTRLT_MASK	0x000000FF
/*N*///#define TXGBE_RXMTRL_V1_SYNC_MSG	0x00
/*N*///#define TXGBE_RXMTRL_V1_DELAY_REQ_MSG	0x01
/*N*///#define TXGBE_RXMTRL_V1_FOLLOWUP_MSG	0x02
/*N*///#define TXGBE_RXMTRL_V1_DELAY_RESP_MSG	0x03
/*N*///#define TXGBE_RXMTRL_V1_MGMT_MSG	0x04

/*N*///#define TXGBE_RXMTRL_V2_MSGID_MASK	0x0000FF00
/*N*///#define TXGBE_RXMTRL_V2_SYNC_MSG	0x0000
/*N*///#define TXGBE_RXMTRL_V2_DELAY_REQ_MSG	0x0100
/*N*///#define TXGBE_RXMTRL_V2_PDELAY_REQ_MSG	0x0200
/*N*///#define TXGBE_RXMTRL_V2_PDELAY_RESP_MSG	0x0300
/*N*///#define TXGBE_RXMTRL_V2_FOLLOWUP_MSG	0x0800
/*N*///#define TXGBE_RXMTRL_V2_DELAY_RESP_MSG	0x0900
/*N*///#define TXGBE_RXMTRL_V2_PDELAY_FOLLOWUP_MSG 0x0A00
/*N*///#define TXGBE_RXMTRL_V2_ANNOUNCE_MSG	0x0B00
/*N*///#define TXGBE_RXMTRL_V2_SIGNALLING_MSG	0x0C00
/*N*///#define TXGBE_RXMTRL_V2_MGMT_MSG	0x0D00

/*N*///#define TXGBE_FCTRL_SBP		0x00000002 /* Store Bad Packet */
/*N*///#define TXGBE_FCTRL_MPE		0x00000100 /* Multicast Promiscuous Ena*/
/*N*///#define TXGBE_FCTRL_UPE		0x00000200 /* Unicast Promiscuous Ena */
/*N*///#define TXGBE_FCTRL_BAM		0x00000400 /* Broadcast Accept Mode */
/*N*///#define TXGBE_FCTRL_PMCF	0x00001000 /* Pass MAC Control Frames */
/*N*///#define TXGBE_FCTRL_DPF		0x00002000 /* Discard Pause Frame */
/* Receive Priority Flow Control Enable */
/*N*///#define TXGBE_FCTRL_RPFCE	0x00004000
/*N*///#define TXGBE_FCTRL_RFCE	0x00008000 /* Receive Flow Control Ena */
/*N*///#define TXGBE_MFLCN_PMCF	0x00000001 /* Pass MAC Control Frames */
/*N*///#define TXGBE_MFLCN_DPF		0x00000002 /* Discard Pause Frame */
/*N*///#define TXGBE_MFLCN_RPFCE	0x00000004 /* Receive Priority FC Enable */
/*N*///#define TXGBE_MFLCN_RFCE	0x00000008 /* Receive FC Enable */
/*N*///#define TXGBE_MFLCN_RPFCE_MASK	0x00000FF4 /* Rx Priority FC bitmap mask */
/*N*///#define TXGBE_MFLCN_RPFCE_SHIFT	4 /* Rx Priority FC bitmap shift */

/* Multiple Receive Queue Control */
/*N*///#define TXGBE_MRQC_RSSEN	0x00000001  /* RSS Enable */
/*N*///#define TXGBE_MRQC_MRQE_MASK	0xF /* Bits 3:0 */
/*N*///#define TXGBE_MRQC_RT8TCEN	0x00000002 /* 8 TC no RSS */
/*N*///#define TXGBE_MRQC_RT4TCEN	0x00000003 /* 4 TC no RSS */
/*N*///#define TXGBE_MRQC_RTRSS8TCEN	0x00000004 /* 8 TC w/ RSS */
/*N*///#define TXGBE_MRQC_RTRSS4TCEN	0x00000005 /* 4 TC w/ RSS */
/*N*///#define TXGBE_MRQC_VMDQEN	0x00000008 /* VMDq2 64 pools no RSS */
/*N*///#define TXGBE_MRQC_VMDQRSS32EN	0x0000000A /* VMDq2 32 pools w/ RSS */
/*N*///#define TXGBE_MRQC_VMDQRSS64EN	0x0000000B /* VMDq2 64 pools w/ RSS */
/*N*///#define TXGBE_MRQC_VMDQRT8TCEN	0x0000000C /* VMDq2/RT 16 pool 8 TC */
/*N*///#define TXGBE_MRQC_VMDQRT4TCEN	0x0000000D /* VMDq2/RT 32 pool 4 TC */
/*N*///#define TXGBE_MRQC_L3L4TXSWEN	0x00008000 /* Enable L3/L4 Tx switch */
/*N*///#define TXGBE_MRQC_RSS_FIELD_MASK	0xFFFF0000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV4_TCP	0x00010000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV4	0x00020000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV6_EX_TCP 0x00040000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV6_EX	0x00080000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV6	0x00100000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV6_TCP	0x00200000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV4_UDP	0x00400000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV6_UDP	0x00800000
/*N*///#define TXGBE_MRQC_RSS_FIELD_IPV6_EX_UDP 0x01000000
/*N*///#define TXGBE_MRQC_MULTIPLE_RSS		0x00002000
/*N*///#define TXGBE_MRQC_L3L4TXSWEN		0x00008000

/* Queue Drop Enable */
/*N*///#define TXGBE_QDE_ENABLE	0x00000001
/*N*///#define TXGBE_QDE_HIDE_VLAN	0x00000002
/*N*///#define TXGBE_QDE_IDX_MASK	0x00007F00
/*N*///#define TXGBE_QDE_IDX_SHIFT	8
/*N*///#define TXGBE_QDE_WRITE		0x00010000
/*N*///#define TXGBE_QDE_READ		0x00020000

/*N*///#define TXGBE_TXD_POPTS_IXSM	0x01 /* Insert IP checksum */
/*N*///#define TXGBE_TXD_POPTS_TXSM	0x02 /* Insert TCP/UDP checksum */
/*N*///#define TXGBE_TXD_CMD_EOP	0x01000000 /* End of Packet */
/*N*///#define TXGBE_TXD_CMD_IFCS	0x02000000 /* Insert FCS (Ethernet CRC) */
/*N*///#define TXGBE_TXD_CMD_IC	0x04000000 /* Insert Checksum */
/*N*///#define TXGBE_TXD_CMD_RS	0x08000000 /* Report Status */
/*N*///#define TXGBE_TXD_CMD_DEXT	0x20000000 /* Desc extension (0 = legacy) */
/*N*///#define TXGBE_TXD_CMD_VLE	0x40000000 /* Add VLAN tag */
/*N*///#define TXGBE_TXD_STAT_DD	0x00000001 /* Descriptor Done */

/*N*///#define TXGBE_RXDADV_IPSEC_STATUS_SECP		0x00020000
/*N*///#define TXGBE_RXDADV_IPSEC_ERROR_INVALID_PROTOCOL 0x08000000
/*N*///#define TXGBE_RXDADV_IPSEC_ERROR_INVALID_LENGTH	0x10000000
/*N*///#define TXGBE_RXDADV_IPSEC_ERROR_AUTH_FAILED	0x18000000
/*N*///#define TXGBE_RXDADV_IPSEC_ERROR_BIT_MASK	0x18000000
/* Multiple Transmit Queue Command Register */
/*N*///#define TXGBE_MTQC_RT_ENA	0x1 /* DCB Enable */
/*N*///#define TXGBE_MTQC_VT_ENA	0x2 /* VMDQ2 Enable */
/*N*///#define TXGBE_MTQC_64Q_1PB	0x0 /* 64 queues 1 pack buffer */
/*N*///#define TXGBE_MTQC_32VF		0x8 /* 4 TX Queues per pool w/32VF's */
/*N*///#define TXGBE_MTQC_64VF		0x4 /* 2 TX Queues per pool w/64VF's */
/*N*///#define TXGBE_MTQC_4TC_4TQ	0x8 /* 4 TC if RT_ENA and VT_ENA */
/*N*///#define TXGBE_MTQC_8TC_8TQ	0xC /* 8 TC if RT_ENA or 8 TQ if VT_ENA */

/* Receive Descriptor bit definitions */
/*N*///#define TXGBE_RXD_STAT_DD	0x01 /* Descriptor Done */
/*N*///#define TXGBE_RXD_STAT_EOP	0x02 /* End of Packet */
/*N*///#define TXGBE_RXD_STAT_FLM	0x04 /* FDir Match */
/*N*///#define TXGBE_RXD_STAT_VP	0x08 /* IEEE VLAN Packet */
/*N*///#define TXGBE_RXDADV_NEXTP_MASK	0x000FFFF0 /* Next Descriptor Index */
/*N*///#define TXGBE_RXDADV_NEXTP_SHIFT	0x00000004
/*N*///#define TXGBE_RXD_STAT_UDPCS	0x10 /* UDP xsum calculated */
/*N*///#define TXGBE_RXD_STAT_L4CS	0x20 /* L4 xsum calculated */
/*N*///#define TXGBE_RXD_STAT_IPCS	0x40 /* IP xsum calculated */
/*N*///#define TXGBE_RXD_STAT_PIF	0x80 /* passed in-exact filter */
/*N*///#define TXGBE_RXD_STAT_CRCV	0x100 /* Speculative CRC Valid */
/*N*///#define TXGBE_RXD_STAT_OUTERIPCS	0x100 /* Cloud IP xsum calculated */
/*N*///#define TXGBE_RXD_STAT_VEXT	0x200 /* 1st VLAN found */
/*N*///#define TXGBE_RXD_STAT_UDPV	0x400 /* Valid UDP checksum */
/*N*///#define TXGBE_RXD_STAT_DYNINT	0x800 /* Pkt caused INT via DYNINT */
/*N*///#define TXGBE_RXD_STAT_LLINT	0x800 /* Pkt caused Low Latency Interrupt */
/*N*///#define TXGBE_RXD_STAT_TSIP	0x08000 /* Time Stamp in packet buffer */
/*N*///#define TXGBE_RXD_STAT_TS	0x10000 /* Time Stamp */
/*N*///#define TXGBE_RXD_STAT_SECP	0x20000 /* Security Processing */
/*N*///#define TXGBE_RXD_STAT_LB	0x40000 /* Loopback Status */
/*N*///#define TXGBE_RXD_STAT_ACK	0x8000 /* ACK Packet indication */
/*N*///#define TXGBE_RXD_ERR_CE	0x01 /* CRC Error */
/*N*///#define TXGBE_RXD_ERR_LE	0x02 /* Length Error */
/*N*///#define TXGBE_RXD_ERR_PE	0x08 /* Packet Error */
/*N*///#define TXGBE_RXD_ERR_OSE	0x10 /* Oversize Error */
/*N*///#define TXGBE_RXD_ERR_USE	0x20 /* Undersize Error */
/*N*///#define TXGBE_RXD_ERR_TCPE	0x40 /* TCP/UDP Checksum Error */
/*N*///#define TXGBE_RXD_ERR_IPE	0x80 /* IP Checksum Error */
/*N*///#define TXGBE_RXDADV_ERR_MASK		0xfff00000 /* RDESC.ERRORS mask */
/*N*///#define TXGBE_RXDADV_ERR_SHIFT		20 /* RDESC.ERRORS shift */
/*N*///#define TXGBE_RXDADV_ERR_OUTERIPER	0x04000000 /* CRC IP Header error */
/*N*///#define TXGBE_RXDADV_ERR_RXE		0x20000000 /* Any MAC Error */
/*N*///#define TXGBE_RXDADV_ERR_FCEOFE		0x80000000 /* FCEOFe/IPE */
/*N*///#define TXGBE_RXDADV_ERR_FCERR		0x00700000 /* FCERR/FDIRERR */
/*N*///#define TXGBE_RXDADV_ERR_FDIR_LEN	0x00100000 /* FDIR Length error */
/*N*///#define TXGBE_RXDADV_ERR_FDIR_DROP	0x00200000 /* FDIR Drop error */
/*N*///#define TXGBE_RXDADV_ERR_FDIR_COLL	0x00400000 /* FDIR Collision error */
/*N*///#define TXGBE_RXDADV_ERR_HBO	0x00800000 /*Header Buffer Overflow */
/*N*///#define TXGBE_RXDADV_ERR_CE	0x01000000 /* CRC Error */
/*N*///#define TXGBE_RXDADV_ERR_LE	0x02000000 /* Length Error */
/*N*///#define TXGBE_RXDADV_ERR_PE	0x08000000 /* Packet Error */
/*N*///#define TXGBE_RXDADV_ERR_OSE	0x10000000 /* Oversize Error */
/*N*///#define TXGBE_RXDADV_ERR_USE	0x20000000 /* Undersize Error */
/*N*///#define TXGBE_RXDADV_ERR_TCPE	0x40000000 /* TCP/UDP Checksum Error */
/*N*///#define TXGBE_RXDADV_ERR_IPE	0x80000000 /* IP Checksum Error */
/*N*///#define TXGBE_RXD_VLAN_ID_MASK	0x0FFF  /* VLAN ID is in lower 12 bits */
/*N*///#define TXGBE_RXD_PRI_MASK	0xE000  /* Priority is in upper 3 bits */
/*N*///#define TXGBE_RXD_PRI_SHIFT	13
/*N*///#define TXGBE_RXD_CFI_MASK	0x1000  /* CFI is bit 12 */
/*N*///#define TXGBE_RXD_CFI_SHIFT	12

/*N*///#define TXGBE_RXDADV_STAT_DD		TXGBE_RXD_STAT_DD  /* Done */
/*N*///#define TXGBE_RXDADV_STAT_EOP		TXGBE_RXD_STAT_EOP /* End of Packet */
/*N*///#define TXGBE_RXDADV_STAT_FLM		TXGBE_RXD_STAT_FLM /* FDir Match */
/*N*///#define TXGBE_RXDADV_STAT_VP		TXGBE_RXD_STAT_VP  /* IEEE VLAN Pkt */
/*N*///#define TXGBE_RXDADV_STAT_MASK		0x000fffff /* Stat/NEXTP: bit 0-19 */
/*N*///#define TXGBE_RXDADV_STAT_FCEOFS	0x00000040 /* FCoE EOF/SOF Stat */
/*N*///#define TXGBE_RXDADV_STAT_FCSTAT	0x00000030 /* FCoE Pkt Stat */
/*N*///#define TXGBE_RXDADV_STAT_FCSTAT_NOMTCH	0x00000000 /* 00: No Ctxt Match */
/*N*///#define TXGBE_RXDADV_STAT_FCSTAT_NODDP	0x00000010 /* 01: Ctxt w/o DDP */
/*N*///#define TXGBE_RXDADV_STAT_FCSTAT_FCPRSP	0x00000020 /* 10: Recv. FCP_RSP */
/*N*///#define TXGBE_RXDADV_STAT_FCSTAT_DDP	0x00000030 /* 11: Ctxt w/ DDP */
/*N*///#define TXGBE_RXDADV_STAT_TS		0x00010000 /* IEEE1588 Time Stamp */
/*N*///#define TXGBE_RXDADV_STAT_TSIP		0x00008000 /* Time Stamp in packet buffer */

/* PSRTYPE bit definitions */
/*N*///#define TXGBE_PSRTYPE_TCPHDR	0x00000010
/*N*///#define TXGBE_PSRTYPE_UDPHDR	0x00000020
/*N*///#define TXGBE_PSRTYPE_IPV4HDR	0x00000100
/*N*///#define TXGBE_PSRTYPE_IPV6HDR	0x00000200
/*N*///#define TXGBE_PSRTYPE_L2HDR	0x00001000

/* SRRCTL bit definitions */
/*N*///#define TXGBE_SRRCTL_BSIZEPKT_SHIFT	10 /* so many KBs */
/*N*///#define TXGBE_SRRCTL_BSIZEHDRSIZE_SHIFT	2 /* 64byte resolution (>> 6)
/*N*///					   * + at bit 8 offset (<< 8)
/*N*///					   *  = (<< 2)
/*N*///					   */
/*N*///#define TXGBE_SRRCTL_RDMTS_SHIFT	22
/*N*///#define TXGBE_SRRCTL_RDMTS_MASK		0x01C00000
/*N*///#define TXGBE_SRRCTL_DROP_EN		0x10000000
/*N*///#define TXGBE_SRRCTL_BSIZEPKT_MASK	0x0000007F
/*N*///#define TXGBE_SRRCTL_BSIZEHDR_MASK	0x00003F00
/*N*///#define TXGBE_SRRCTL_DESCTYPE_LEGACY	0x00000000
/*N*///#define TXGBE_SRRCTL_DESCTYPE_ADV_ONEBUF 0x02000000
/*N*///#define TXGBE_SRRCTL_DESCTYPE_HDR_SPLIT	0x04000000
/*N*///#define TXGBE_SRRCTL_DESCTYPE_HDR_REPLICATION_LARGE_PKT 0x08000000
/*N*///#define TXGBE_SRRCTL_DESCTYPE_HDR_SPLIT_ALWAYS 0x0A000000
/*N*///#define TXGBE_SRRCTL_DESCTYPE_MASK	0x0E000000

/*N*///#define TXGBE_RXDPS_HDRSTAT_HDRSP	0x00008000
/*N*///#define TXGBE_RXDPS_HDRSTAT_HDRLEN_MASK	0x000003FF

/*N*///#define TXGBE_RXDADV_RSSTYPE_MASK	0x0000000F
/*N*///#define TXGBE_RXDADV_PKTTYPE_MASK	0x0000FFF0
/*N*///#define TXGBE_RXDADV_PKTTYPE_MASK_EX	0x0001FFF0
/*N*///#define TXGBE_RXDADV_HDRBUFLEN_MASK	0x00007FE0
/*N*///#define TXGBE_RXDADV_RSCCNT_MASK	0x001E0000
/*N*///#define TXGBE_RXDADV_RSCCNT_SHIFT	17
/*N*///#define TXGBE_RXDADV_HDRBUFLEN_SHIFT	5
/*N*///#define TXGBE_RXDADV_SPLITHEADER_EN	0x00001000
/*N*///#define TXGBE_RXDADV_SPH		0x8000

/* RSS Hash results */
/*N*///#define TXGBE_RXDADV_RSSTYPE_NONE	0x00000000
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV4_TCP	0x00000001
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV4	0x00000002
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV6_TCP	0x00000003
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV6_EX	0x00000004
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV6	0x00000005
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV6_TCP_EX 0x00000006
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV4_UDP	0x00000007
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV6_UDP	0x00000008
/*N*///#define TXGBE_RXDADV_RSSTYPE_IPV6_UDP_EX 0x00000009

/* RSS Packet Types as indicated in the receive descriptor. */
/*N*///#define TXGBE_RXDADV_PKTTYPE_NONE	0x00000000
/*N*///#define TXGBE_RXDADV_PKTTYPE_IPV4	0x00000010 /* IPv4 hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_IPV4_EX	0x00000020 /* IPv4 hdr + extensions */
/*N*///#define TXGBE_RXDADV_PKTTYPE_IPV6	0x00000040 /* IPv6 hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_IPV6_EX	0x00000080 /* IPv6 hdr + extensions */
/*N*///#define TXGBE_RXDADV_PKTTYPE_TCP	0x00000100 /* TCP hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_UDP	0x00000200 /* UDP hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_SCTP	0x00000400 /* SCTP hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_NFS	0x00000800 /* NFS hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_GENEVE	0x00000800 /* GENEVE hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_VXLAN	0x00000800 /* VXLAN hdr present */
/*N*///#define TXGBE_RXDADV_PKTTYPE_TUNNEL	0x00010000 /* Tunnel type */
/*N*///#define TXGBE_RXDADV_PKTTYPE_IPSEC_ESP	0x00001000 /* IPSec ESP */
/*N*///#define TXGBE_RXDADV_PKTTYPE_IPSEC_AH	0x00002000 /* IPSec AH */
/*N*///#define TXGBE_RXDADV_PKTTYPE_LINKSEC	0x00004000 /* LinkSec Encap */
/*N*///#define TXGBE_RXDADV_PKTTYPE_ETQF	0x00008000 /* PKTTYPE is ETQF index */
/*N*///#define TXGBE_RXDADV_PKTTYPE_ETQF_MASK	0x00000070 /* ETQF has 8 indices */
/*N*///#define TXGBE_RXDADV_PKTTYPE_ETQF_SHIFT	4 /* Right-shift 4 bits */

/* Security Processing bit Indication */
/*N*///#define TXGBE_RXDADV_LNKSEC_STATUS_SECP		0x00020000
/*N*///#define TXGBE_RXDADV_LNKSEC_ERROR_NO_SA_MATCH	0x08000000
/*N*///#define TXGBE_RXDADV_LNKSEC_ERROR_REPLAY_ERROR	0x10000000
/*N*///#define TXGBE_RXDADV_LNKSEC_ERROR_BIT_MASK	0x18000000
/*N*///#define TXGBE_RXDADV_LNKSEC_ERROR_BAD_SIG	0x18000000

/* Masks to determine if packets should be dropped due to frame errors */
/*N*//*#define TXGBE_RXD_ERR_FRAME_ERR_MASK ( \
				TXGBE_RXD_ERR_CE | \
				TXGBE_RXD_ERR_LE | \
				TXGBE_RXD_ERR_PE | \
				TXGBE_RXD_ERR_OSE | \
				TXGBE_RXD_ERR_USE)*/

/*N*//*#define TXGBE_RXDADV_ERR_FRAME_ERR_MASK ( \
				TXGBE_RXDADV_ERR_CE | \
				TXGBE_RXDADV_ERR_LE | \
				TXGBE_RXDADV_ERR_PE | \
				TXGBE_RXDADV_ERR_OSE | \
				TXGBE_RXDADV_ERR_USE)*/

/*N*///#define TXGBE_RXDADV_ERR_FRAME_ERR_MASK_82599	TXGBE_RXDADV_ERR_RXE

/* Multicast bit mask */
/*N*///#define TXGBE_MCSTCTRL_MFE	0x4

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
/*N*///#define TXGBE_REQ_TX_DESCRIPTOR_MULTIPLE	8
/*N*///#define TXGBE_REQ_RX_DESCRIPTOR_MULTIPLE	8
/*N*///#define TXGBE_REQ_TX_BUFFER_GRANULARITY		1024

/* Vlan-specific macros */
/*N*///#define TXGBE_RX_DESC_SPECIAL_VLAN_MASK	0x0FFF /* VLAN ID in lower 12 bits */
/*N*///#define TXGBE_RX_DESC_SPECIAL_PRI_MASK	0xE000 /* Priority in upper 3 bits */
/*N*///#define TXGBE_RX_DESC_SPECIAL_PRI_SHIFT	0x000D /* Priority in upper 3 of 16 */
/*N*///#define TXGBE_TX_DESC_SPECIAL_PRI_SHIFT	TXGBE_RX_DESC_SPECIAL_PRI_SHIFT

/* SR-IOV specific macros */
/*N*///#define TXGBE_MBVFICR_INDEX(vf_number)	(vf_number >> 4)
/*N*///#define TXGBE_MBVFICR(_i)		(0x00710 + ((_i) * 4))
/*N*///#define TXGBE_VFLRE(_i)			(((_i & 1) ? 0x001C0 : 0x00600))
/*N*///#define TXGBE_VFLREC(_i)		 (0x00700 + ((_i) * 4))
/* Translated register #defines */
/*N*///#define TXGBE_PVFCTRL(P)	(0x00300 + (4 * (P)))
/*N*///#define TXGBE_PVFSTATUS(P)	(0x00008 + (0 * (P)))
/*N*///#define TXGBE_PVFLINKS(P)	(0x042A4 + (0 * (P)))
/*N*///#define TXGBE_PVFRTIMER(P)	(0x00048 + (0 * (P)))
/*N*///#define TXGBE_PVFMAILBOX(P)	(0x04C00 + (4 * (P)))
/*N*///#define TXGBE_PVFRXMEMWRAP(P)	(0x03190 + (0 * (P)))
/*N*///#define TXGBE_PVTEICR(P)	(0x00B00 + (4 * (P)))
/*N*///#define TXGBE_PVTEICS(P)	(0x00C00 + (4 * (P)))
/*N*///#define TXGBE_PVTEIMS(P)	(0x00D00 + (4 * (P)))
/*N*///#define TXGBE_PVTEIMC(P)	(0x00E00 + (4 * (P)))
/*N*///#define TXGBE_PVTEIAC(P)	(0x00F00 + (4 * (P)))
/*N*///#define TXGBE_PVTEIAM(P)	(0x04D00 + (4 * (P)))
/*N*//*#define TXGBE_PVTEITR(P)	(((P) < 24) ? (0x00820 + ((P) * 4)) : \
				 (0x012300 + (((P) - 24) * 4)))*/
/*N*///#define TXGBE_PVTIVAR(P)	(0x12500 + (4 * (P)))
/*N*///#define TXGBE_PVTIVAR_MISC(P)	(0x04E00 + (4 * (P)))
/*N*///#define TXGBE_PVTRSCINT(P)	(0x12000 + (4 * (P)))
/*N*///#define TXGBE_VFPBACL(P)	(0x110C8 + (4 * (P)))
/*N*//*#define TXGBE_PVFRDBAL(P)	((P < 64) ? (0x01000 + (0x40 * (P))) \
				 : (0x0D000 + (0x40 * ((P) - 64))))*/
/*N*//*#define TXGBE_PVFRDBAH(P)	((P < 64) ? (0x01004 + (0x40 * (P))) \
				 : (0x0D004 + (0x40 * ((P) - 64))))*/
/*N*//*#define TXGBE_PVFRDLEN(P)	((P < 64) ? (0x01008 + (0x40 * (P))) \
				 : (0x0D008 + (0x40 * ((P) - 64))))*/
/*N*//*#define TXGBE_PVFRDH(P)		((P < 64) ? (0x01010 + (0x40 * (P))) \
				 : (0x0D010 + (0x40 * ((P) - 64))))*/
/*N*//*#define TXGBE_PVFRDT(P)		((P < 64) ? (0x01018 + (0x40 * (P))) \
				 : (0x0D018 + (0x40 * ((P) - 64))))*/
/*N*//*#define TXGBE_PVFRXDCTL(P)	((P < 64) ? (0x01028 + (0x40 * (P))) \
				 : (0x0D028 + (0x40 * ((P) - 64))))*/
/*N*//*#define TXGBE_PVFSRRCTL(P)	((P < 64) ? (0x01014 + (0x40 * (P))) \
				 : (0x0D014 + (0x40 * ((P) - 64))))*/
/*N*///#define TXGBE_PVFPSRTYPE(P)	(0x0EA00 + (4 * (P)))
/*N*///#define TXGBE_PVFTDBAL(P)	(0x06000 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTDBAH(P)	(0x06004 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTDLEN(P)	(0x06008 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTDH(P)		(0x06010 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTDT(P)		(0x06018 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTXDCTL(P)	(0x06028 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTDWBAL(P)	(0x06038 + (0x40 * (P)))
/*N*///#define TXGBE_PVFTDWBAH(P)	(0x0603C + (0x40 * (P)))
/*N*//*#define TXGBE_PVFDCA_RXCTRL(P)	(((P) < 64) ? (0x0100C + (0x40 * (P))) \
				 : (0x0D00C + (0x40 * ((P) - 64))))*/
/*N*///#define TXGBE_PVFDCA_TXCTRL(P)	(0x0600C + (0x40 * (P)))
/*N*///#define TXGBE_PVFGPRC(x)	(0x0101C + (0x40 * (x)))
/*N*///#define TXGBE_PVFGPTC(x)	(0x08300 + (0x04 * (x)))
/*N*///#define TXGBE_PVFGORC_LSB(x)	(0x01020 + (0x40 * (x)))
/*N*///#define TXGBE_PVFGORC_MSB(x)	(0x0D020 + (0x40 * (x)))
/*N*///#define TXGBE_PVFGOTC_LSB(x)	(0x08400 + (0x08 * (x)))
/*N*///#define TXGBE_PVFGOTC_MSB(x)	(0x08404 + (0x08 * (x)))
/*N*///#define TXGBE_PVFMPRC(x)	(0x0D01C + (0x40 * (x)))

/*N*//*#define TXGBE_PVFTDWBALn(q_per_pool, vf_number, vf_q_index) \
		(TXGBE_PVFTDWBAL((q_per_pool)*(vf_number) + (vf_q_index)))*/
/*N*//*#define TXGBE_PVFTDWBAHn(q_per_pool, vf_number, vf_q_index) \
		(TXGBE_PVFTDWBAH((q_per_pool)*(vf_number) + (vf_q_index)))*/

/*N*//*#define TXGBE_PVFTDHn(q_per_pool, vf_number, vf_q_index) \
		(TXGBE_PVFTDH((q_per_pool)*(vf_number) + (vf_q_index)))*/
/*N*//*#define TXGBE_PVFTDTn(q_per_pool, vf_number, vf_q_index) \
		(TXGBE_PVFTDT((q_per_pool)*(vf_number) + (vf_q_index)))*/

/* Little Endian defines */
//#ifndef __le16
//#define __le16  u16
//#endif
//#ifndef __le32
//#define __le32  u32
//#endif
//#ifndef __le64
//#define __le64  u64

//#endif
//#ifndef __be16
/* Big Endian defines */
//#define __be16  u16
//#define __be32  u32
//#define __be64  u64

//#endif
enum txgbe_fdir_pballoc_type {
	TXGBE_FDIR_PBALLOC_NONE = 0,
	TXGBE_FDIR_PBALLOC_64K  = 1,
	TXGBE_FDIR_PBALLOC_128K = 2,
	TXGBE_FDIR_PBALLOC_256K = 3,
};

/* Flow Director register values */
/*N*///#define TXGBE_FDIRCTRL_PBALLOC_64K		0x00000001
/*N*///#define TXGBE_FDIRCTRL_PBALLOC_128K		0x00000002
/*N*///#define TXGBE_FDIRCTRL_PBALLOC_256K		0x00000003
/*N*///#define TXGBE_FDIRCTRL_INIT_DONE		0x00000008
/*N*///#define TXGBE_FDIRCTRL_PERFECT_MATCH		0x00000010
/*N*///#define TXGBE_FDIRCTRL_REPORT_STATUS		0x00000020
/*N*///#define TXGBE_FDIRCTRL_REPORT_STATUS_ALWAYS	0x00000080
/*N*///#define TXGBE_FDIRCTRL_DROP_Q_SHIFT		8
/*N*///#define TXGBE_FDIRCTRL_DROP_Q_MASK		0x00007F00
/*N*///#define TXGBE_FDIRCTRL_FLEX_SHIFT		16
/*N*///#define TXGBE_FDIRCTRL_DROP_NO_MATCH		0x00008000
/*N*///#define TXGBE_FDIRCTRL_FILTERMODE_SHIFT		21
/*N*///#define TXGBE_FDIRCTRL_FILTERMODE_MACVLAN	0x0001 /* bit 23:21, 001b */
/*N*///#define TXGBE_FDIRCTRL_FILTERMODE_CLOUD		0x0002 /* bit 23:21, 010b */
/*N*///#define TXGBE_FDIRCTRL_SEARCHLIM		0x00800000
/*N*///#define TXGBE_FDIRCTRL_FILTERMODE_MASK		0x00E00000
/*N*///#define TXGBE_FDIRCTRL_MAX_LENGTH_SHIFT		24
/*N*///#define TXGBE_FDIRCTRL_FULL_THRESH_MASK		0xF0000000
/*N*///#define TXGBE_FDIRCTRL_FULL_THRESH_SHIFT	28

/*N*///#define TXGBE_FDIRTCPM_DPORTM_SHIFT		16
/*N*///#define TXGBE_FDIRUDPM_DPORTM_SHIFT		16
/*N*///#define TXGBE_FDIRIP6M_DIPM_SHIFT		16
/*N*///#define TXGBE_FDIRM_VLANID			0x00000001
/*N*///#define TXGBE_FDIRM_VLANP			0x00000002
/*N*///#define TXGBE_FDIRM_POOL			0x00000004
/*N*///#define TXGBE_FDIRM_L4P				0x00000008
/*N*///#define TXGBE_FDIRM_FLEX			0x00000010
/*N*///#define TXGBE_FDIRM_DIPv6			0x00000020
/*N*///#define TXGBE_FDIRM_L3P				0x00000040

/*N*///#define TXGBE_FDIRIP6M_INNER_MAC	0x03F0 /* bit 9:4 */
/*N*///#define TXGBE_FDIRIP6M_TUNNEL_TYPE	0x0800 /* bit 11 */
/*N*///#define TXGBE_FDIRIP6M_TNI_VNI		0xF000 /* bit 15:12 */
/*N*///#define TXGBE_FDIRIP6M_TNI_VNI_24	0x1000 /* bit 12 */
/*N*///#define TXGBE_FDIRIP6M_ALWAYS_MASK	0x040F /* bit 10, 3:0 */

/*N*///#define TXGBE_FDIRFREE_FREE_MASK		0xFFFF
/*N*///#define TXGBE_FDIRFREE_FREE_SHIFT		0
/*N*///#define TXGBE_FDIRFREE_COLL_MASK		0x7FFF0000
/*N*///#define TXGBE_FDIRFREE_COLL_SHIFT		16
/*N*///#define TXGBE_FDIRLEN_MAXLEN_MASK		0x3F
/*N*///#define TXGBE_FDIRLEN_MAXLEN_SHIFT		0
/*N*///#define TXGBE_FDIRLEN_MAXHASH_MASK		0x7FFF0000
/*N*///#define TXGBE_FDIRLEN_MAXHASH_SHIFT		16
/*N*///#define TXGBE_FDIRUSTAT_ADD_MASK		0xFFFF
/*N*///#define TXGBE_FDIRUSTAT_ADD_SHIFT		0
/*N*///#define TXGBE_FDIRUSTAT_REMOVE_MASK		0xFFFF0000
/*N*///#define TXGBE_FDIRUSTAT_REMOVE_SHIFT		16
/*N*///#define TXGBE_FDIRFSTAT_FADD_MASK		0x00FF
/*N*///#define TXGBE_FDIRFSTAT_FADD_SHIFT		0
/*N*///#define TXGBE_FDIRFSTAT_FREMOVE_MASK		0xFF00
/*N*///#define TXGBE_FDIRFSTAT_FREMOVE_SHIFT		8
/*N*///#define TXGBE_FDIRPORT_DESTINATION_SHIFT	16
/*N*///#define TXGBE_FDIRVLAN_FLEX_SHIFT		16
/*N*///#define TXGBE_FDIRHASH_BUCKET_VALID_SHIFT	15
/*N*///#define TXGBE_FDIRHASH_SIG_SW_INDEX_SHIFT	16

/*N*///#define TXGBE_FDIRCMD_CMD_MASK			0x00000003
/*N*///#define TXGBE_FDIRCMD_CMD_ADD_FLOW		0x00000001
/*N*///#define TXGBE_FDIRCMD_CMD_REMOVE_FLOW		0x00000002
/*N*///#define TXGBE_FDIRCMD_CMD_QUERY_REM_FILT	0x00000003
/*N*///#define TXGBE_FDIRCMD_FILTER_VALID		0x00000004
/*N*///#define TXGBE_FDIRCMD_FILTER_UPDATE		0x00000008
/*N*///#define TXGBE_FDIRCMD_IPv6DMATCH		0x00000010
/*N*///#define TXGBE_FDIRCMD_L4TYPE_UDP		0x00000020
/*N*///#define TXGBE_FDIRCMD_L4TYPE_TCP		0x00000040
/*N*///#define TXGBE_FDIRCMD_L4TYPE_SCTP		0x00000060
/*N*///#define TXGBE_FDIRCMD_IPV6			0x00000080
/*N*///#define TXGBE_FDIRCMD_CLEARHT			0x00000100
/*N*///#define TXGBE_FDIRCMD_DROP			0x00000200
/*N*///#define TXGBE_FDIRCMD_INT			0x00000400
/*N*///#define TXGBE_FDIRCMD_LAST			0x00000800
/*N*///#define TXGBE_FDIRCMD_COLLISION			0x00001000
/*N*///#define TXGBE_FDIRCMD_QUEUE_EN			0x00008000
/*N*///#define TXGBE_FDIRCMD_FLOW_TYPE_SHIFT		5
/*N*///#define TXGBE_FDIRCMD_RX_QUEUE_SHIFT		16
/*N*///#define TXGBE_FDIRCMD_TUNNEL_FILTER_SHIFT	23
/*N*///#define TXGBE_FDIRCMD_VT_POOL_SHIFT		24
/*N*///#define TXGBE_FDIR_INIT_DONE_POLL		10
/*N*///#define TXGBE_FDIRCMD_CMD_POLL			10
/*N*///#define TXGBE_FDIRCMD_TUNNEL_FILTER		0x00800000
/*N*///#define TXGBE_FDIR_DROP_QUEUE			127


/* Manageablility Host Interface defines */
/*N*///#define TXGBE_HI_MAX_BLOCK_BYTE_LENGTH	1792 /* Num of bytes in range */
/*N*///#define TXGBE_HI_MAX_BLOCK_DWORD_LENGTH	448 /* Num of dwords in range */
/*N*///#define TXGBE_HI_COMMAND_TIMEOUT	500 /* Process HI command limit */
/*N*///#define TXGBE_HI_FLASH_ERASE_TIMEOUT	1000 /* Process Erase command limit */
/*N*///#define TXGBE_HI_FLASH_UPDATE_TIMEOUT	5000 /* Process Update command limit */
/*N*///#define TXGBE_HI_FLASH_APPLY_TIMEOUT	0 /* Process Apply command limit */
/*N*///#define TXGBE_HI_PHY_MGMT_REQ_TIMEOUT	2000 /* Wait up to 2 seconds */

/* CEM Support */
//#define FW_CEM_HDR_LEN			0x4
//#define FW_CEM_CMD_DRIVER_INFO		0xDD
//#define FW_CEM_CMD_DRIVER_INFO_LEN	0x5
//#define FW_CEM_CMD_RESERVED		0X0
//#define FW_CEM_UNUSED_VER		0x0
//#define FW_CEM_MAX_RETRIES		3
//#define FW_CEM_RESP_STATUS_SUCCESS	0x1
//#define FW_CEM_DRIVER_VERSION_SIZE	39 /* +9 would send 48 bytes to fw */
//#define FW_READ_SHADOW_RAM_CMD		0x31
//#define FW_READ_SHADOW_RAM_LEN		0x6
//#define FW_WRITE_SHADOW_RAM_CMD		0x33
//#define FW_WRITE_SHADOW_RAM_LEN		0xA /* 8 plus 1 WORD to write */
//#define FW_SHADOW_RAM_DUMP_CMD		0x36
//#define FW_SHADOW_RAM_DUMP_LEN		0
//#define FW_DEFAULT_CHECKSUM		0xFF /* checksum always 0xFF */
//#define FW_NVM_DATA_OFFSET		3
//#define FW_MAX_READ_BUFFER_SIZE		1024
//#define FW_DISABLE_RXEN_CMD		0xDE
//#define FW_DISABLE_RXEN_LEN		0x1
//#define FW_PHY_MGMT_REQ_CMD		0x20
//#define FW_PHY_TOKEN_REQ_CMD		0xA
//#define FW_PHY_TOKEN_REQ_LEN		2
//#define FW_PHY_TOKEN_REQ		0
//#define FW_PHY_TOKEN_REL		1
//#define FW_PHY_TOKEN_OK			1
//#define FW_PHY_TOKEN_RETRY		0x80
//#define FW_PHY_TOKEN_DELAY		5	/* milliseconds */
//#define FW_PHY_TOKEN_WAIT		5	/* seconds */
//#define FW_PHY_TOKEN_RETRIES ((FW_PHY_TOKEN_WAIT * 1000) / FW_PHY_TOKEN_DELAY)
//#define FW_INT_PHY_REQ_CMD		0xB
//#define FW_INT_PHY_REQ_LEN		10
//#define FW_INT_PHY_REQ_READ		0
//#define FW_INT_PHY_REQ_WRITE		1
//#define FW_PHY_ACT_REQ_CMD		5
//#define FW_PHY_ACT_DATA_COUNT		4
//#define FW_PHY_ACT_REQ_LEN		(4 + 4 * FW_PHY_ACT_DATA_COUNT)
//#define FW_PHY_ACT_INIT_PHY		1
//#define FW_PHY_ACT_SETUP_LINK		2
//#define FW_PHY_ACT_LINK_SPEED_10	(1u << 0)
//#define FW_PHY_ACT_LINK_SPEED_100	(1u << 1)
//#define FW_PHY_ACT_LINK_SPEED_1G	(1u << 2)
//#define FW_PHY_ACT_LINK_SPEED_2_5G	(1u << 3)
//#define FW_PHY_ACT_LINK_SPEED_5G	(1u << 4)
//#define FW_PHY_ACT_LINK_SPEED_10G	(1u << 5)
//#define FW_PHY_ACT_LINK_SPEED_20G	(1u << 6)
//#define FW_PHY_ACT_LINK_SPEED_25G	(1u << 7)
//#define FW_PHY_ACT_LINK_SPEED_40G	(1u << 8)
//#define FW_PHY_ACT_LINK_SPEED_50G	(1u << 9)
//#define FW_PHY_ACT_LINK_SPEED_100G	(1u << 10)
//#define FW_PHY_ACT_SETUP_LINK_PAUSE_SHIFT 16
//#define FW_PHY_ACT_SETUP_LINK_PAUSE_MASK (3u << FW_PHY_ACT_SETUP_LINK_PAUSE_SHIFT)
//#define FW_PHY_ACT_SETUP_LINK_PAUSE_NONE 0u
//#define FW_PHY_ACT_SETUP_LINK_PAUSE_TX	1u
//#define FW_PHY_ACT_SETUP_LINK_PAUSE_RX	2u
//#define FW_PHY_ACT_SETUP_LINK_PAUSE_RXTX 3u
//#define FW_PHY_ACT_SETUP_LINK_LP	(1u << 18)
//#define FW_PHY_ACT_SETUP_LINK_HP	(1u << 19)
//#define FW_PHY_ACT_SETUP_LINK_EEE	(1u << 20)
//#define FW_PHY_ACT_SETUP_LINK_AN	(1u << 22)
//#define FW_PHY_ACT_SETUP_LINK_RSP_DOWN	(1u << 0)
//#define FW_PHY_ACT_GET_LINK_INFO	3
//#define FW_PHY_ACT_GET_LINK_INFO_EEE	(1u << 19)
//#define FW_PHY_ACT_GET_LINK_INFO_FC_TX	(1u << 20)
//#define FW_PHY_ACT_GET_LINK_INFO_FC_RX	(1u << 21)
//#define FW_PHY_ACT_GET_LINK_INFO_POWER	(1u << 22)
//#define FW_PHY_ACT_GET_LINK_INFO_AN_COMPLETE	(1u << 24)
//#define FW_PHY_ACT_GET_LINK_INFO_TEMP	(1u << 25)
//#define FW_PHY_ACT_GET_LINK_INFO_LP_FC_TX	(1u << 28)
//#define FW_PHY_ACT_GET_LINK_INFO_LP_FC_RX	(1u << 29)
//#define FW_PHY_ACT_FORCE_LINK_DOWN	4
//#define FW_PHY_ACT_FORCE_LINK_DOWN_OFF	(1u << 0)
//#define FW_PHY_ACT_PHY_SW_RESET		5
//#define FW_PHY_ACT_PHY_HW_RESET		6
//#define FW_PHY_ACT_GET_PHY_INFO		7
//#define FW_PHY_ACT_UD_2			0x1002
//#define FW_PHY_ACT_UD_2_10G_KR_EEE	(1u << 6)
//#define FW_PHY_ACT_UD_2_10G_KX4_EEE	(1u << 5)
//#define FW_PHY_ACT_UD_2_1G_KX_EEE	(1u << 4)
//#define FW_PHY_ACT_UD_2_10G_T_EEE	(1u << 3)
//#define FW_PHY_ACT_UD_2_1G_T_EEE	(1u << 2)
//#define FW_PHY_ACT_UD_2_100M_TX_EEE	(1u << 1)
//#define FW_PHY_ACT_RETRIES		50
//#define FW_PHY_INFO_SPEED_MASK		0xFFFu
//#define FW_PHY_INFO_ID_HI_MASK		0xFFFF0000u
//#define FW_PHY_INFO_ID_LO_MASK		0x0000FFFFu


/* 
 * VF Registers
 */
/*N*///#define TXGBE_VFCTRL		0x00000
/*N*///#define TXGBE_VFLINKS		0x00010
/*N*///#define TXGBE_VFFRTIMER		0x00048
/*N*///#define TXGBE_VFRXMEMWRAP	0x03190
/*N*///#define TXGBE_VTEICR		0x00100
/*N*///#define TXGBE_VTEICS		0x00104
/*N*///#define TXGBE_VTEIMS		0x00108
/*N*///#define TXGBE_VTEIMC		0x0010C
/*N*///#define TXGBE_VTEIAC		0x00110
/*N*///#define TXGBE_VTEIAM		0x00114
/*N*///#define TXGBE_VTEITR(x)		(0x00820 + (4 * (x)))
/*N*///#define TXGBE_VTIVAR(x)		(0x00120 + (4 * (x)))
/*N*///#define TXGBE_VTIVAR_MISC	0x00140
/*N*///#define TXGBE_VTRSCINT(x)	(0x00180 + (4 * (x)))
/* define TXGBE_VFPBACL  still says TBD in EAS */
/*N*///#define TXGBE_VFRDBAL(x)	(0x01000 + (0x40 * (x)))
/*N*///#define TXGBE_VFRDBAH(x)	(0x01004 + (0x40 * (x)))
/*N*///#define TXGBE_VFRDLEN(x)	(0x01008 + (0x40 * (x)))
/*N*///#define TXGBE_VFRDH(x)		(0x01010 + (0x40 * (x)))
/*N*///#define TXGBE_VFRDT(x)		(0x01018 + (0x40 * (x)))
/*N*///#define TXGBE_VFRXDCTL(x)	(0x01028 + (0x40 * (x)))
/*N*///#define TXGBE_VFSRRCTL(x)	(0x01014 + (0x40 * (x)))
/*N*///#define TXGBE_VFRSCCTL(x)	(0x0102C + (0x40 * (x)))
/*N*///#define TXGBE_VFPSRTYPE		0x00300
/*N*///#define TXGBE_VFTDBAL(x)	(0x02000 + (0x40 * (x)))
/*N*///#define TXGBE_VFTDBAH(x)	(0x02004 + (0x40 * (x)))
/*N*///#define TXGBE_VFTDLEN(x)	(0x02008 + (0x40 * (x)))
/*N*///#define TXGBE_VFTDH(x)		(0x02010 + (0x40 * (x)))
/*N*///#define TXGBE_VFTDT(x)		(0x02018 + (0x40 * (x)))
/*N*///#define TXGBE_VFTXDCTL(x)	(0x02028 + (0x40 * (x)))
/*N*///#define TXGBE_VFTDWBAL(x)	(0x02038 + (0x40 * (x)))
/*N*///#define TXGBE_VFTDWBAH(x)	(0x0203C + (0x40 * (x)))
/*N*///#define TXGBE_VFDCA_RXCTRL(x)	(0x0100C + (0x40 * (x)))
/*N*///#define TXGBE_VFDCA_TXCTRL(x)	(0x0200c + (0x40 * (x)))
/*N*///#define TXGBE_VFGPRC		0x0101C
/*N*///#define TXGBE_VFGPTC		0x0201C
/*N*///#define TXGBE_VFGORC_LSB	0x01020
/*N*///#define TXGBE_VFGORC_MSB	0x01024
/*N*///#define TXGBE_VFGOTC_LSB	0x02020
/*N*///#define TXGBE_VFGOTC_MSB	0x02024
/*N*///#define TXGBE_VFMPRC		0x01034
/*N*///#define TXGBE_VFRSSRK(x)	(0x3100 + ((x) * 4))
/*N*///#define TXGBE_VFRETA(x)	(0x3200 + ((x) * 4))


/* Host Interface Command Structures */
//struct txgbe_hic_hdr {
//	u8 cmd;
//	u8 buf_len;
//	union {
//		u8 cmd_resv;
//		u8 ret_status;
//	} cmd_or_resp;
//	u8 checksum;
//};
//
//struct txgbe_hic_hdr2_req {
//	u8 cmd;
//	u8 buf_lenh;
//	u8 buf_lenl;
//	u8 checksum;
//};
//
//struct txgbe_hic_hdr2_rsp {
//	u8 cmd;
//	u8 buf_lenl;
//	u8 buf_lenh_status;	/* 7-5: high bits of buf_len, 4-0: status */
//	u8 checksum;
//};
//
//union txgbe_hic_hdr2 {
//	struct txgbe_hic_hdr2_req req;
//	struct txgbe_hic_hdr2_rsp rsp;
//};
//
//struct txgbe_hic_drv_info {
//	struct txgbe_hic_hdr hdr;
//	u8 port_num;
//	u8 ver_sub;
//	u8 ver_build;
//	u8 ver_min;
//	u8 ver_maj;
//	u8 pad; /* end spacing to ensure length is mult. of dword */
//	u16 pad2; /* end spacing to ensure length is mult. of dword2 */
//};
//
//struct txgbe_hic_drv_info2 {
//	struct txgbe_hic_hdr hdr;
//	u8 port_num;
//	u8 ver_sub;
//	u8 ver_build;
//	u8 ver_min;
//	u8 ver_maj;
//	char driver_string[FW_CEM_DRIVER_VERSION_SIZE];
//};
//
///* These need to be dword aligned */
//struct txgbe_hic_read_shadow_ram {
//	union txgbe_hic_hdr2 hdr;
//	u32 address;
//	u16 length;
//	u16 pad2;
//	u16 data;
//	u16 pad3;
//};
//
//struct txgbe_hic_write_shadow_ram {
//	union txgbe_hic_hdr2 hdr;
//	u32 address;
//	u16 length;
//	u16 pad2;
//	u16 data;
//	u16 pad3;
//};
//
//struct txgbe_hic_disable_rxen {
//	struct txgbe_hic_hdr hdr;
//	u8  port_number;
//	u8  pad2;
//	u16 pad3;
//};
//
//struct txgbe_hic_phy_token_req {
//	struct txgbe_hic_hdr hdr;
//	u8 port_number;
//	u8 command_type;
//	u16 pad;
//};
//
//struct txgbe_hic_internal_phy_req {
//	struct txgbe_hic_hdr hdr;
//	u8 port_number;
//	u8 command_type;
//	__be16 address;
//	u16 rsv1;
//	__be32 write_data;
//	u16 pad;
//};
//
//struct txgbe_hic_internal_phy_resp {
//	struct txgbe_hic_hdr hdr;
//	__be32 read_data;
//};
//
//struct txgbe_hic_phy_activity_req {
//	struct txgbe_hic_hdr hdr;
//	u8 port_number;
//	u8 pad;
//	__le16 activity_id;
//	__be32 data[FW_PHY_ACT_DATA_COUNT];
//};
//
//struct txgbe_hic_phy_activity_resp {
//	struct txgbe_hic_hdr hdr;
//	__be32 data[FW_PHY_ACT_DATA_COUNT];
//};

///* Transmit Descriptor - Legacy */
//struct txgbe_legacy_tx_desc {
//	u64 buffer_addr; /* Address of the descriptor's data buffer */
//	union {
//		__le32 data;
//		struct {
//			__le16 length; /* Data buffer length */
//			u8 cso; /* Checksum offset */
//			u8 cmd; /* Descriptor control */
//		} flags;
//	} lower;
//	union {
//		__le32 data;
//		struct {
//			u8 status; /* Descriptor status */
//			u8 css; /* Checksum start */
//			__le16 vlan;
//		} fields;
//	} upper;
//};
//
///* Transmit Descriptor - Advanced */
//union txgbe_adv_tx_desc {
//	struct {
//		__le64 buffer_addr; /* Address of descriptor's data buf */
//		__le32 cmd_type_len;
//		__le32 olinfo_status;
//	} read;
//	struct {
//		__le64 rsvd; /* Reserved */
//		__le32 nxtseq_seed;
//		__le32 status;
//	} wb;
//};
//
///* Receive Descriptor - Legacy */
//struct txgbe_legacy_rx_desc {
//	__le64 buffer_addr; /* Address of the descriptor's data buffer */
//	__le16 length; /* Length of data DMAed into data buffer */
//	__le16 csum; /* Packet checksum */
//	u8 status;   /* Descriptor status */
//	u8 errors;   /* Descriptor Errors */
//	__le16 vlan;
//};
//
///* Receive Descriptor - Advanced */
//union txgbe_adv_rx_desc {
//	struct {
//		__le64 pkt_addr; /* Packet buffer address */
//		__le64 hdr_addr; /* Header buffer address */
//	} read;
//	struct {
//		struct {
//			union {
//				__le32 data;
//				struct {
//					__le16 pkt_info; /* RSS, Pkt type */
//					__le16 hdr_info; /* Splithdr, hdrlen */
//				} hs_rss;
//			} lo_dword;
//			union {
//				__le32 rss; /* RSS Hash */
//				struct {
//					__le16 ip_id; /* IP id */
//					__le16 csum; /* Packet Checksum */
//				} csum_ip;
//			} hi_dword;
//		} lower;
//		struct {
//			__le32 status_error; /* ext status/error */
//			__le16 length; /* Packet length */
//			__le16 vlan; /* VLAN tag */
//		} upper;
//	} wb;  /* writeback */
//};
//
///* Context descriptors */
//struct txgbe_adv_tx_context_desc {
//	__le32 vlan_macip_lens;
//	__le32 seqnum_seed;
//	__le32 type_tucmd_mlhl;
//	__le32 mss_l4len_idx;
//};

/* Adv Transmit Descriptor Config Masks */
/*N*///#define TXGBE_ADVTXD_DTALEN_MASK	0x0000FFFF /* Data buf length(bytes) */
/*N*///#define TXGBE_ADVTXD_MAC_LINKSEC	0x00040000 /* Insert LinkSec */
/*N*///#define TXGBE_ADVTXD_MAC_TSTAMP		0x00080000 /* IEEE1588 time stamp */
/*N*///#define TXGBE_ADVTXD_IPSEC_SA_INDEX_MASK 0x000003FF /* IPSec SA index */
/*N*///#define TXGBE_ADVTXD_IPSEC_ESP_LEN_MASK	0x000001FF /* IPSec ESP length */
/*N*///#define TXGBE_ADVTXD_DTYP_MASK		0x00F00000 /* DTYP mask */
/*N*///#define TXGBE_ADVTXD_DTYP_CTXT		0x00200000 /* Adv Context Desc */
/*N*///#define TXGBE_ADVTXD_DTYP_DATA		0x00300000 /* Adv Data Descriptor */
/*N*///#define TXGBE_ADVTXD_DCMD_EOP		TXGBE_TXD_CMD_EOP  /* End of Packet */
/*N*///#define TXGBE_ADVTXD_DCMD_IFCS		TXGBE_TXD_CMD_IFCS /* Insert FCS */
/*N*///#define TXGBE_ADVTXD_DCMD_RS		TXGBE_TXD_CMD_RS /* Report Status */
/*N*///#define TXGBE_ADVTXD_DCMD_DDTYP_ISCSI	0x10000000 /* DDP hdr type or iSCSI */
/*N*///#define TXGBE_ADVTXD_DCMD_DEXT		TXGBE_TXD_CMD_DEXT /* Desc ext 1=Adv */
/*N*///#define TXGBE_ADVTXD_DCMD_VLE		TXGBE_TXD_CMD_VLE  /* VLAN pkt enable */
/*N*///#define TXGBE_ADVTXD_DCMD_TSE		0x80000000 /* TCP Seg enable */
/*N*///#define TXGBE_ADVTXD_STAT_DD		TXGBE_TXD_STAT_DD  /* Descriptor Done */
/*N*///#define TXGBE_ADVTXD_STAT_SN_CRC	0x00000002 /* NXTSEQ/SEED pres in WB */
/*N*///#define TXGBE_ADVTXD_STAT_RSV		0x0000000C /* STA Reserved */
/*N*///#define TXGBE_ADVTXD_IDX_SHIFT		4 /* Adv desc Index shift */
/*N*///#define TXGBE_ADVTXD_CC			0x00000080 /* Check Context */
/*N*///#define TXGBE_ADVTXD_POPTS_SHIFT	8  /* Adv desc POPTS shift */
/*N*///#define TXGBE_ADVTXD_POPTS_IXSM		(TXGBE_TXD_POPTS_IXSM << TXGBE_ADVTXD_POPTS_SHIFT)
/*N*///#define TXGBE_ADVTXD_POPTS_TXSM		(TXGBE_TXD_POPTS_TXSM << TXGBE_ADVTXD_POPTS_SHIFT)
/*N*///#define TXGBE_ADVTXD_POPTS_ISCO_1ST	0x00000000 /* 1st TSO of iSCSI PDU */
/*N*///#define TXGBE_ADVTXD_POPTS_ISCO_MDL	0x00000800 /* Middle TSO of iSCSI PDU */
/*N*///#define TXGBE_ADVTXD_POPTS_ISCO_LAST	0x00001000 /* Last TSO of iSCSI PDU */
/* 1st&Last TSO-full iSCSI PDU */
/*N*///#define TXGBE_ADVTXD_POPTS_ISCO_FULL	0x00001800
/*N*///#define TXGBE_ADVTXD_POPTS_RSV		0x00002000 /* POPTS Reserved */
/*N*///#define TXGBE_ADVTXD_PAYLEN_SHIFT	14 /* Adv desc PAYLEN shift */
/*N*///#define TXGBE_ADVTXD_MACLEN_SHIFT	9  /* Adv ctxt desc mac len shift */
/*N*///#define TXGBE_ADVTXD_VLAN_SHIFT		16  /* Adv ctxt vlan tag shift */
/*N*///#define TXGBE_ADVTXD_TUCMD_IPV4		0x00000400 /* IP Packet Type: 1=IPv4 */
/*N*///#define TXGBE_ADVTXD_TUCMD_IPV6		0x00000000 /* IP Packet Type: 0=IPv6 */
/*N*///#define TXGBE_ADVTXD_TUCMD_L4T_UDP	0x00000000 /* L4 Packet TYPE of UDP */
/*N*///#define TXGBE_ADVTXD_TUCMD_L4T_TCP	0x00000800 /* L4 Packet TYPE of TCP */
/*N*///#define TXGBE_ADVTXD_TUCMD_L4T_SCTP	0x00001000 /* L4 Packet TYPE of SCTP */
/*N*///#define TXGBE_ADVTXD_TUCMD_L4T_RSV	0x00001800 /* RSV L4 Packet TYPE */
/*N*///#define TXGBE_ADVTXD_TUCMD_MKRREQ	0x00002000 /* req Markers and CRC */
/*N*///#define TXGBE_ADVTXD_POPTS_IPSEC	0x00000400 /* IPSec offload request */
/*N*///#define TXGBE_ADVTXD_TUCMD_IPSEC_TYPE_ESP 0x00002000 /* IPSec Type ESP */
/*N*///#define TXGBE_ADVTXD_TUCMD_IPSEC_ENCRYPT_EN 0x00004000/* ESP Encrypt Enable */
/*N*///#define TXGBE_ADVTXT_TUCMD_FCOE		0x00008000 /* FCoE Frame Type */
/*N*///#define TXGBE_ADVTXD_FCOEF_EOF_MASK	(0x3 << 10) /* FC EOF index */
/*N*///#define TXGBE_ADVTXD_FCOEF_SOF		((1 << 2) << 10) /* FC SOF index */
/*N*///#define TXGBE_ADVTXD_FCOEF_PARINC	((1 << 3) << 10) /* Rel_Off in F_CTL */
/*N*///#define TXGBE_ADVTXD_FCOEF_ORIE		((1 << 4) << 10) /* Orientation End */
/*N*///#define TXGBE_ADVTXD_FCOEF_ORIS		((1 << 5) << 10) /* Orientation Start */
/*N*///#define TXGBE_ADVTXD_FCOEF_EOF_N	(0x0 << 10) /* 00: EOFn */
/*N*///#define TXGBE_ADVTXD_FCOEF_EOF_T	(0x1 << 10) /* 01: EOFt */
/*N*///#define TXGBE_ADVTXD_FCOEF_EOF_NI	(0x2 << 10) /* 10: EOFni */
/*N*///#define TXGBE_ADVTXD_FCOEF_EOF_A	(0x3 << 10) /* 11: EOFa */
/*N*///#define TXGBE_ADVTXD_L4LEN_SHIFT	8  /* Adv ctxt L4LEN shift */
/*N*///#define TXGBE_ADVTXD_MSS_SHIFT		16  /* Adv ctxt MSS shift */

/*N*///#define TXGBE_ADVTXD_OUTER_IPLEN	16 /* Adv ctxt OUTERIPLEN shift */
/*N*///#define TXGBE_ADVTXD_TUNNEL_LEN 	24 /* Adv ctxt TUNNELLEN shift */
/*N*///#define TXGBE_ADVTXD_TUNNEL_TYPE_SHIFT	16 /* Adv Tx Desc Tunnel Type shift */
/*N*///#define TXGBE_ADVTXD_OUTERIPCS_SHIFT	17 /* Adv Tx Desc OUTERIPCS Shift */
/*N*///#define TXGBE_ADVTXD_TUNNEL_TYPE_NVGRE	1  /* Adv Tx Desc Tunnel Type NVGRE */
/* Adv Tx Desc OUTERIPCS Shift for X550EM_a */
/*N*///#define TXGBE_ADVTXD_OUTERIPCS_SHIFT_X550EM_a	26
/* Autonegotiation advertised speeds */
/*N*///typedef u32 txgbe_autoneg_advertised;
/* Link speed */
/*N*///typedef u32 txgbe_link_speed;
/*N*///#define TXGBE_LINK_SPEED_UNKNOWN	0
/*N*///#define TXGBE_LINK_SPEED_10M_FULL	0x0002
/*N*///#define TXGBE_LINK_SPEED_100M_FULL	0x0008
/*N*///#define TXGBE_LINK_SPEED_1GB_FULL	0x0020
/*N*///#define TXGBE_LINK_SPEED_2_5GB_FULL	0x0400
/*N*///#define TXGBE_LINK_SPEED_5GB_FULL	0x0800
/*N*///#define TXGBE_LINK_SPEED_10GB_FULL	0x0080
/*N*//*#define TXGBE_LINK_SPEED_82598_AUTONEG	(TXGBE_LINK_SPEED_1GB_FULL | \
					 TXGBE_LINK_SPEED_10GB_FULL)*/
/*N*//*#define TXGBE_LINK_SPEED_82599_AUTONEG	(TXGBE_LINK_SPEED_100M_FULL | \
					 TXGBE_LINK_SPEED_1GB_FULL | \
					 TXGBE_LINK_SPEED_10GB_FULL)*/
/* Physical layer type */
/*N*///typedef u64 txgbe_physical_layer;
#define TXGBE_PHYSICAL_LAYER_UNKNOWN		0
#define TXGBE_PHYSICAL_LAYER_10GBASE_T		0x00001
#define TXGBE_PHYSICAL_LAYER_1000BASE_T		0x00002
#define TXGBE_PHYSICAL_LAYER_100BASE_TX		0x00004
#define TXGBE_PHYSICAL_LAYER_SFP_PLUS_CU	0x00008
#define TXGBE_PHYSICAL_LAYER_10GBASE_LR		0x00010
#define TXGBE_PHYSICAL_LAYER_10GBASE_LRM	0x00020
#define TXGBE_PHYSICAL_LAYER_10GBASE_SR		0x00040
#define TXGBE_PHYSICAL_LAYER_10GBASE_KX4	0x00080
#define TXGBE_PHYSICAL_LAYER_10GBASE_CX4	0x00100
#define TXGBE_PHYSICAL_LAYER_1000BASE_KX	0x00200
#define TXGBE_PHYSICAL_LAYER_1000BASE_BX	0x00400
#define TXGBE_PHYSICAL_LAYER_10GBASE_KR		0x00800
#define TXGBE_PHYSICAL_LAYER_10GBASE_XAUI	0x01000
#define TXGBE_PHYSICAL_LAYER_SFP_ACTIVE_DA	0x02000
#define TXGBE_PHYSICAL_LAYER_1000BASE_SX	0x04000
#define TXGBE_PHYSICAL_LAYER_10BASE_T		0x08000
#define TXGBE_PHYSICAL_LAYER_2500BASE_KX	0x10000

/* Flow Control Data Sheet defined values
 * Calculation and defines taken from 802.1bb Annex O
 */

/* BitTimes (BT) conversion */
/*N*///#define TXGBE_BT2KB(BT)		((BT + (8 * 1024 - 1)) / (8 * 1024))
/*N*///#define TXGBE_B2BT(BT)		(BT * 8)

/* Calculate Delay to respond to PFC */
/*N*///#define TXGBE_PFC_D	672

/* Calculate Cable Delay */
/*N*///#define TXGBE_CABLE_DC	5556 /* Delay Copper */
/*N*///#define TXGBE_CABLE_DO	5000 /* Delay Optical */

/* Calculate Interface Delay X540 */
/*N*///#define TXGBE_PHY_DC	25600 /* Delay 10G BASET */
/*N*///#define TXGBE_MAC_DC	8192  /* Delay Copper XAUI interface */
/*N*///#define TXGBE_XAUI_DC	(2 * 2048) /* Delay Copper Phy */

/*N*///#define TXGBE_ID_X540	(TXGBE_MAC_DC + TXGBE_XAUI_DC + TXGBE_PHY_DC)

/* Calculate Interface Delay 82598, 82599 */
/*N*///#define TXGBE_PHY_D	12800
/*N*///#define TXGBE_MAC_D	4096
/*N*///#define TXGBE_XAUI_D	(2 * 1024)

/*N*///#define TXGBE_ID	(TXGBE_MAC_D + TXGBE_XAUI_D + TXGBE_PHY_D)

/* Calculate Delay incurred from higher layer */
/*N*///#define TXGBE_HD	6144

/* Calculate PCI Bus delay for low thresholds */
/*N*///#define TXGBE_PCI_DELAY	10000

/* Calculate X540 delay value in bit times */
/*N*//*#define TXGBE_DV_X540(_max_frame_link, _max_frame_tc) \
			((36 * \
			  (TXGBE_B2BT(_max_frame_link) + \
			   TXGBE_PFC_D + \
			   (2 * TXGBE_CABLE_DC) + \
			   (2 * TXGBE_ID_X540) + \
			   TXGBE_HD) / 25 + 1) + \
			 2 * TXGBE_B2BT(_max_frame_tc))*/

/* Calculate 82599, 82598 delay value in bit times */
/*N*//*#define TXGBE_DV(_max_frame_link, _max_frame_tc) \
			((36 * \
			  (TXGBE_B2BT(_max_frame_link) + \
			   TXGBE_PFC_D + \
			   (2 * TXGBE_CABLE_DC) + \
			   (2 * TXGBE_ID) + \
			   TXGBE_HD) / 25 + 1) + \
			 2 * TXGBE_B2BT(_max_frame_tc))*/

/* Calculate low threshold delay values */
/*N*//*#define TXGBE_LOW_DV_X540(_max_frame_tc) \
			(2 * TXGBE_B2BT(_max_frame_tc) + \
			(36 * TXGBE_PCI_DELAY / 25) + 1)*/
/*N*//*#define TXGBE_LOW_DV(_max_frame_tc) \
			(2 * TXGBE_LOW_DV_X540(_max_frame_tc))*/

/* Software ATR hash keys */
#define TXGBE_ATR_BUCKET_HASH_KEY	0x3DAD14E2
#define TXGBE_ATR_SIGNATURE_HASH_KEY	0x174D3614

/* Software ATR input stream values and masks */
#define TXGBE_ATR_HASH_MASK		0x7fff
#define TXGBE_ATR_L3TYPE_MASK		0x4
#define TXGBE_ATR_L3TYPE_IPV4		0x0
#define TXGBE_ATR_L3TYPE_IPV6		0x4
#define TXGBE_ATR_L4TYPE_MASK		0x3
#define TXGBE_ATR_L4TYPE_UDP		0x1
#define TXGBE_ATR_L4TYPE_TCP		0x2
#define TXGBE_ATR_L4TYPE_SCTP		0x3
#define TXGBE_ATR_TUNNEL_MASK		0x10
#define TXGBE_ATR_TUNNEL_ANY		0x10
enum txgbe_atr_flow_type {
	TXGBE_ATR_FLOW_TYPE_IPV4	= 0x0,
	TXGBE_ATR_FLOW_TYPE_UDPV4	= 0x1,
	TXGBE_ATR_FLOW_TYPE_TCPV4	= 0x2,
	TXGBE_ATR_FLOW_TYPE_SCTPV4	= 0x3,
	TXGBE_ATR_FLOW_TYPE_IPV6	= 0x4,
	TXGBE_ATR_FLOW_TYPE_UDPV6	= 0x5,
	TXGBE_ATR_FLOW_TYPE_TCPV6	= 0x6,
	TXGBE_ATR_FLOW_TYPE_SCTPV6	= 0x7,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_IPV4	= 0x10,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_UDPV4	= 0x11,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_TCPV4	= 0x12,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_SCTPV4	= 0x13,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_IPV6	= 0x14,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_UDPV6	= 0x15,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_TCPV6	= 0x16,
	TXGBE_ATR_FLOW_TYPE_TUNNELED_SCTPV6	= 0x17,
};

/* Flow Director ATR input struct. */
struct txgbe_atr_input {
	/*
	 * Byte layout in order, all values with MSB first:
	 *
	 * vm_pool	- 1 byte
	 * flow_type	- 1 byte
	 * vlan_id	- 2 bytes
	 * src_ip	- 16 bytes
	 * inner_mac	- 6 bytes
	 * cloud_mode	- 2 bytes
	 * tni_vni	- 4 bytes
	 * dst_ip	- 16 bytes
	 * src_port	- 2 bytes
	 * dst_port	- 2 bytes
	 * flex_bytes	- 2 bytes
	 * bkt_hash	- 2 bytes
	 */
	u8 vm_pool;
	u8 flow_type;
	__be16 pkt_type;
	__be32 dst_ip[4];
	__be32 src_ip[4];
	__be16 src_port;
	__be16 dst_port;
	__be16 flex_bytes;
	__be16 bkt_hash;
};

/* Flow Director compressed ATR hash input struct */
union txgbe_atr_hash_dword {
	struct {
		u8 vm_pool;
		u8 flow_type;
		__be16 vlan_id;
	} formatted;
	__be32 ip;
	struct {
		__be16 src;
		__be16 dst;
	} port;
	__be16 flex_bytes;
	__be32 dword;
};


/*#define TXGBE_MVALS_INIT(m)	\
	TXGBE_CAT(EEC, m),		\
	TXGBE_CAT(FLA, m),		\
	TXGBE_CAT(GRC, m),		\
	TXGBE_CAT(SRAMREL, m),		\
	TXGBE_CAT(FACTPS, m),		\
	TXGBE_CAT(SWSM, m),		\
	TXGBE_CAT(SWFW_SYNC, m),	\
	TXGBE_CAT(FWSM, m),		\
	TXGBE_CAT(SDP0_GPIEN, m),	\
	TXGBE_CAT(SDP1_GPIEN, m),	\
	TXGBE_CAT(SDP2_GPIEN, m),	\
	TXGBE_CAT(EICR_GPI_SDP0, m),	\
	TXGBE_CAT(EICR_GPI_SDP1, m),	\
	TXGBE_CAT(EICR_GPI_SDP2, m),	\
	TXGBE_CAT(CIAA, m),		\
	TXGBE_CAT(CIAD, m),		\
	TXGBE_CAT(I2C_CLK_IN, m),	\
	TXGBE_CAT(I2C_CLK_OUT, m),	\
	TXGBE_CAT(I2C_DATA_IN, m),	\
	TXGBE_CAT(I2C_DATA_OUT, m),	\
	TXGBE_CAT(I2C_DATA_OE_N_EN, m),	\
	TXGBE_CAT(I2C_BB_EN, m),	\
	TXGBE_CAT(I2C_CLK_OE_N_EN, m),	\
	TXGBE_CAT(I2CCTL, m)

enum txgbe_mvals {
	TXGBE_MVALS_INIT(_IDX),
	TXGBE_MVALS_IDX_LIMIT
};*/

/*
 * Unavailable: The FCoE Boot Option ROM is not present in the flash.
 * Disabled: Present; boot order is not set for any targets on the port.
 * Enabled: Present; boot order is set for at least one target on the port.
 */
enum txgbe_fcoe_boot_status {
	txgbe_fcoe_bootstatus_disabled = 0,
	txgbe_fcoe_bootstatus_enabled = 1,
	txgbe_fcoe_bootstatus_unavailable = 0xFFFF
};

enum txgbe_eeprom_type {
	txgbe_eeprom_unknown = 0,
	txgbe_eeprom_spi,
	txgbe_eeprom_flash,
	txgbe_eeprom_none /* No NVM support */
};

enum txgbe_mac_type {
	txgbe_mac_unknown = 0,
	txgbe_mac_raptor,
	txgbe_mac_raptor_vf,
	txgbe_num_macs
};

enum txgbe_phy_type {
	txgbe_phy_unknown = 0,
	txgbe_phy_none,
	txgbe_phy_tn,
	txgbe_phy_aq,
	txgbe_phy_ext_1g_t,
	txgbe_phy_cu_mtd,
	txgbe_phy_cu_unknown,
	txgbe_phy_qt,
	txgbe_phy_xaui,
	txgbe_phy_nl,
	txgbe_phy_sfp_tyco_passive,
	txgbe_phy_sfp_unknown_passive,
	txgbe_phy_sfp_unknown_active,
	txgbe_phy_sfp_avago,
	txgbe_phy_sfp_ftl,
	txgbe_phy_sfp_ftl_active,
	txgbe_phy_sfp_unknown,
	txgbe_phy_sfp_intel,
	txgbe_phy_qsfp_unknown_passive,
	txgbe_phy_qsfp_unknown_active,
	txgbe_phy_qsfp_intel,
	txgbe_phy_qsfp_unknown,
	txgbe_phy_sfp_unsupported, /*Enforce bit set with unsupported module*/
	txgbe_phy_sgmii,
	txgbe_phy_fw,
	txgbe_phy_generic
};

/*
 * SFP+ module type IDs:
 *
 * ID	Module Type
 * =============
 * 0	SFP_DA_CU
 * 1	SFP_SR
 * 2	SFP_LR
 * 3	SFP_DA_CU_CORE0 - chip-specific
 * 4	SFP_DA_CU_CORE1 - chip-specific
 * 5	SFP_SR/LR_CORE0 - chip-specific
 * 6	SFP_SR/LR_CORE1 - chip-specific
 */
enum txgbe_sfp_type {
	txgbe_sfp_type_unknown = 0,
	txgbe_sfp_type_da_cu,
	txgbe_sfp_type_sr,
	txgbe_sfp_type_lr,
	txgbe_sfp_type_da_cu_core0,
	txgbe_sfp_type_da_cu_core1,
	txgbe_sfp_type_srlr_core0,
	txgbe_sfp_type_srlr_core1,
	txgbe_sfp_type_da_act_lmt_core0,
	txgbe_sfp_type_da_act_lmt_core1,
	txgbe_sfp_type_1g_cu_core0,
	txgbe_sfp_type_1g_cu_core1,
	txgbe_sfp_type_1g_sx_core0,
	txgbe_sfp_type_1g_sx_core1,
	txgbe_sfp_type_1g_lx_core0,
	txgbe_sfp_type_1g_lx_core1,
	txgbe_sfp_type_not_present = 0xFFFE,
	txgbe_sfp_type_not_known = 0xFFFF
};

enum txgbe_media_type {
	txgbe_media_type_unknown = 0,
	txgbe_media_type_fiber,
	txgbe_media_type_fiber_qsfp,
	txgbe_media_type_copper,
	txgbe_media_type_backplane,
	txgbe_media_type_cx4,
	txgbe_media_type_virtual
};

/* Flow Control Settings */
enum txgbe_fc_mode {
	txgbe_fc_none = 0,
	txgbe_fc_rx_pause,
	txgbe_fc_tx_pause,
	txgbe_fc_full,
	txgbe_fc_default
};

/* Smart Speed Settings */
#define TXGBE_SMARTSPEED_MAX_RETRIES	3
enum txgbe_smart_speed {
	txgbe_smart_speed_auto = 0,
	txgbe_smart_speed_on,
	txgbe_smart_speed_off
};

/* PCI bus types */
enum txgbe_bus_type {
	txgbe_bus_type_unknown = 0,
	txgbe_bus_type_pci,
	txgbe_bus_type_pcix,
	txgbe_bus_type_pci_express,
	txgbe_bus_type_internal,
	txgbe_bus_type_reserved
};

/* PCI bus speeds */
enum txgbe_bus_speed {
	txgbe_bus_speed_unknown	= 0,
	txgbe_bus_speed_33	= 33,
	txgbe_bus_speed_66	= 66,
	txgbe_bus_speed_100	= 100,
	txgbe_bus_speed_120	= 120,
	txgbe_bus_speed_133	= 133,
	txgbe_bus_speed_2500	= 2500,
	txgbe_bus_speed_5000	= 5000,
	txgbe_bus_speed_8000	= 8000,
	txgbe_bus_speed_reserved
};

/* PCI bus widths */
enum txgbe_bus_width {
	txgbe_bus_width_unknown	= 0,
	txgbe_bus_width_pcie_x1	= 1,
	txgbe_bus_width_pcie_x2	= 2,
	txgbe_bus_width_pcie_x4	= 4,
	txgbe_bus_width_pcie_x8	= 8,
	txgbe_bus_width_32	= 32,
	txgbe_bus_width_64	= 64,
	txgbe_bus_width_reserved
};

struct txgbe_hw;

struct txgbe_addr_filter_info {
	u32 num_mc_addrs;
	u32 rar_used_count;
	u32 mta_in_use;
	u32 overflow_promisc;
	bool user_set_promisc;
};

/* Bus parameters */
struct txgbe_bus_info {
	s32 (*get_bus_info)(struct txgbe_hw *);
	void (*set_lan_id)(struct txgbe_hw *);

	enum txgbe_bus_speed speed;
	enum txgbe_bus_width width;
	enum txgbe_bus_type type;

	u16 func;
	u8 lan_id;
	u16 instance_id;
};

/* Flow control parameters */
struct txgbe_fc_info {
	u32 high_water[TXGBE_DCB_TC_MAX]; /* Flow Ctrl High-water */
	u32 low_water[TXGBE_DCB_TC_MAX]; /* Flow Ctrl Low-water */
	u16 pause_time; /* Flow Control Pause timer */
	bool send_xon; /* Flow control send XON */
	bool strict_ieee; /* Strict IEEE mode */
	bool disable_fc_autoneg; /* Do not autonegotiate FC */
	bool fc_was_autonegged; /* Is current_mode the result of autonegging? */
	enum txgbe_fc_mode current_mode; /* FC mode in effect */
	enum txgbe_fc_mode requested_mode; /* FC mode requested by caller */
};

/* Statistics counters collected by the MAC */
/* PB[] RxTx */
struct txgbe_pb_stats {
	u64 tx_pb_xon_packets; //pxontxc;
	u64 rx_pb_xon_packets; //pxonrxc;
	u64 tx_pb_xoff_packets; //pxofftxc;
	u64 rx_pb_xoff_packets; //pxoffrxc;
	u64 rx_pb_dropped; //mpc;
	u64 rx_pb_mbuf_alloc_errors; //rnbc;
	u64 tx_pb_xon2off_packets; //pxon2offc;
};

/* QP[] RxTx */
struct txgbe_qp_stats {
	u64 rx_qp_packets; //qprc;
	u64 tx_qp_packets; //qptc;
	u64 rx_qp_bytes; //qbrc;
	u64 tx_qp_bytes; //qbtc;
	u64 rx_qp_mc_packets; //qprdc;
};

struct txgbe_hw_stats {
	/* MNG RxTx */
	u64 mng_bmc2host_packets; //b2ospc
	u64 mng_host2bmc_packets; //o2bspc
	/* Basix RxTx */
	u64 rx_packets; //gprc;
	u64 tx_packets; //gptc;
	u64 rx_bytes; //gorc;
	u64 tx_bytes; //gotc;
	u64 rx_total_bytes; //tor;
	u64 rx_total_packets; //tpr;
	u64 tx_total_packets; //tpt;
	u64 rx_total_missed_packets; //mpctotal;
	u64 rx_broadcast_packets; //bprc;
	u64 tx_broadcast_packets; //bptc;
	u64 rx_multicast_packets; //mprc;
	u64 tx_multicast_packets; //mptc;
	u64 rx_management_packets; //b2ogprc;
	u64 tx_management_packets; //o2bgptc;
	u64 rx_management_dropped; //mngpdc;
	u64 rx_dma_drop;
	u64 rx_drop_packets;

	/* Basic Error */
	u64 rx_crc_errors; //crcerrs;
	u64 rx_illegal_byte_errors; //illerrc;
	u64 rx_error_bytes; //errbc;
	u64 rx_mac_short_packet_dropped; //mspdc;
	u64 rx_length_errors; //rlec;
	u64 rx_undersize_errors; //ruc;
	u64 rx_fragment_errors; //rfc;
	u64 rx_oversize_errors; //roc;
	u64 rx_jabber_errors; //rjc;
	u64 rx_l3_l4_xsum_error; //xec;
	u64 mac_local_errors; //mlfc;
	u64 mac_remote_errors; //mrfc;

	/* Flow Director */
	u64 flow_director_added_filters; //fdirustat_add;
	u64 flow_director_removed_filters; //fdirustat_remove;
	u64 flow_director_filter_add_errors; //fdirfstat_fadd;
	u64 flow_director_filter_remove_errors; //fdirfstat_fremove;
	u64 flow_director_matched_filters; //fdirmatch;
	u64 flow_director_missed_filters; //fdirmiss;

	/* FCoE */
	u64 rx_fcoe_crc_errors; //fccrc;
	u64 rx_fcoe_mbuf_allocation_errors; //fclast;
	u64 rx_fcoe_dropped; //fcoerpdc;
	u64 rx_fcoe_packets; //fcoeprc;
	u64 tx_fcoe_packets; //fcoeptc;
	u64 rx_fcoe_bytes; //fcoedwrc;
	u64 tx_fcoe_bytes; //fcoedwtc;
	u64 rx_fcoe_no_ddp; //fcoe_noddp;
	u64 rx_fcoe_no_ddp_ext_buff; //fcoe_noddp_ext_buff;

	/* MACSEC */
	u64 tx_macsec_pkts_untagged;
	u64 tx_macsec_pkts_encrypted;
	u64 tx_macsec_pkts_protected;
	u64 tx_macsec_octets_encrypted;
	u64 tx_macsec_octets_protected;
	u64 rx_macsec_pkts_untagged;
	u64 rx_macsec_pkts_badtag;
	u64 rx_macsec_pkts_nosci;
	u64 rx_macsec_pkts_unknownsci;
	u64 rx_macsec_octets_decrypted;
	u64 rx_macsec_octets_validated;
	u64 rx_macsec_sc_pkts_unchecked;
	u64 rx_macsec_sc_pkts_delayed;
	u64 rx_macsec_sc_pkts_late;
	u64 rx_macsec_sa_pkts_ok;
	u64 rx_macsec_sa_pkts_invalid;
	u64 rx_macsec_sa_pkts_notvalid;
	u64 rx_macsec_sa_pkts_unusedsa;
	u64 rx_macsec_sa_pkts_notusingsa;

	/* MAC RxTx */
	u64 rx_size_64_packets; //prc64;
	u64 rx_size_65_to_127_packets; //prc127;
	u64 rx_size_128_to_255_packets; //prc255;
	u64 rx_size_256_to_511_packets; //prc511;
	u64 rx_size_512_to_1023_packets; //prc1023;
	u64 rx_size_1024_to_max_packets; //prc1522;
	u64 tx_size_64_packets; //ptc64;
	u64 tx_size_65_to_127_packets; //ptc127;
	u64 tx_size_128_to_255_packets; //ptc255;
	u64 tx_size_256_to_511_packets; //ptc511;
	u64 tx_size_512_to_1023_packets; //ptc1023;
	u64 tx_size_1024_to_max_packets; //ptc1522;

	/* Flow Control */
	u64 tx_xon_packets; //lxontxc;
	u64 rx_xon_packets; //lxonrxc;
	u64 tx_xoff_packets; //lxofftxc;
	u64 rx_xoff_packets; //lxoffrxc;

	/* PB[] RxTx */
	struct {
		u64 rx_up_packets; //qprc;
		u64 tx_up_packets; //qptc;
		u64 rx_up_bytes; //qbrc;
		u64 tx_up_bytes; //qbtc;
		u64 rx_up_drop_packets; //qprdc;

		u64 tx_up_xon_packets; //pxontxc;
		u64 rx_up_xon_packets; //pxonrxc;
		u64 tx_up_xoff_packets; //pxofftxc;
		u64 rx_up_xoff_packets; //pxoffrxc;
		u64 rx_up_dropped; //mpc;
		u64 rx_up_mbuf_alloc_errors; //rnbc;
		u64 tx_up_xon2off_packets; //pxon2offc;
	} up[TXGBE_MAX_UP];

	/* QP[] RxTx */
	struct {
		u64 rx_qp_packets;
		u64 tx_qp_packets;
		u64 rx_qp_bytes;
		u64 tx_qp_bytes;
		u64 rx_qp_mc_packets;
	} qp[TXGBE_MAX_QP];

};

/* iterator type for walking multicast address lists */
typedef u8* (*txgbe_mc_addr_itr) (struct txgbe_hw *hw, u8 **mc_addr_ptr,
				  u32 *vmdq);

struct txgbe_link_info {
	s32 (*read_link)(struct txgbe_hw *, u8 addr, u16 reg, u16 *val);
	s32 (*read_link_unlocked)(struct txgbe_hw *, u8 addr, u16 reg,
				  u16 *val);
	s32 (*write_link)(struct txgbe_hw *, u8 addr, u16 reg, u16 val);
	s32 (*write_link_unlocked)(struct txgbe_hw *, u8 addr, u16 reg,
				   u16 val);

	u8 addr;
};

struct txgbe_rom_info {
	s32 (*init_params)(struct txgbe_hw *);
	s32 (*read16)(struct txgbe_hw *, u32, u16 *);
	s32 (*readw_sw)(struct txgbe_hw *, u32, u16 *);
	s32 (*readw_buffer)(struct txgbe_hw *, u32, u32, void *);
	s32 (*read32)(struct txgbe_hw *, u32, u32 *);
	s32 (*read_buffer)(struct txgbe_hw *, u32, u32, void *);
	s32 (*write16)(struct txgbe_hw *, u32, u16);
	s32 (*writew_sw)(struct txgbe_hw *, u32, u16);
	s32 (*writew_buffer)(struct txgbe_hw *, u32, u32, void *);
	s32 (*write32)(struct txgbe_hw *, u32, u32);
	s32 (*write_buffer)(struct txgbe_hw *, u32, u32, void *);
	s32 (*validate_checksum)(struct txgbe_hw *, u16 *);
	s32 (*update_checksum)(struct txgbe_hw *);
	s32 (*calc_checksum)(struct txgbe_hw *);

	enum txgbe_eeprom_type type;
	u32 semaphore_delay;
	u16 word_size;
	u16 address_bits;
	u16 word_page_size;
	u16 ctrl_word_3;

	u32 sw_addr; //fixme
};


struct txgbe_flash_info {
	s32 (*init_params)(struct txgbe_hw *);
	s32 (*read_buffer)(struct txgbe_hw *, u32, u32, u32 *);
	s32 (*write_buffer)(struct txgbe_hw *, u32, u32, u32 *);
	u32 semaphore_delay;
	u32 dword_size;
	u16 address_bits;
};

#define TXGBE_FLAGS_DOUBLE_RESET_REQUIRED	0x01
struct txgbe_mac_info {
	s32 (*init_hw)(struct txgbe_hw *);
	s32 (*reset_hw)(struct txgbe_hw *);
	s32 (*start_hw)(struct txgbe_hw *);
	s32 (*stop_hw)(struct txgbe_hw *);
	s32 (*clear_hw_cntrs)(struct txgbe_hw *);
	void (*enable_relaxed_ordering)(struct txgbe_hw *);
	u64 (*get_supported_physical_layer)(struct txgbe_hw *);
	s32 (*get_mac_addr)(struct txgbe_hw *, u8 *);
	s32 (*get_san_mac_addr)(struct txgbe_hw *, u8 *);
	s32 (*set_san_mac_addr)(struct txgbe_hw *, u8 *);
	s32 (*get_device_caps)(struct txgbe_hw *, u16 *);
	s32 (*get_wwn_prefix)(struct txgbe_hw *, u16 *, u16 *);
	s32 (*get_fcoe_boot_status)(struct txgbe_hw *, u16 *);
	s32 (*read_analog_reg8)(struct txgbe_hw*, u32, u8*);
	s32 (*write_analog_reg8)(struct txgbe_hw*, u32, u8);
	s32 (*setup_sfp)(struct txgbe_hw *);
	s32 (*enable_rx_dma)(struct txgbe_hw *, u32);
	s32 (*disable_sec_rx_path)(struct txgbe_hw *);
	s32 (*enable_sec_rx_path)(struct txgbe_hw *);
	s32 (*disable_sec_tx_path)(struct txgbe_hw *);
	s32 (*enable_sec_tx_path)(struct txgbe_hw *);
	s32 (*acquire_swfw_sync)(struct txgbe_hw *, u32);
	void (*release_swfw_sync)(struct txgbe_hw *, u32);
	void (*init_swfw_sync)(struct txgbe_hw *);
	u64 (*autoc_read)(struct txgbe_hw *);
	void (*autoc_write)(struct txgbe_hw *, u64);
	s32 (*prot_autoc_read)(struct txgbe_hw *, bool *, u64 *);
	s32 (*prot_autoc_write)(struct txgbe_hw *, bool, u64);
	s32 (*negotiate_api_version)(struct txgbe_hw *hw, int api);

	/* Link */
	void (*disable_tx_laser)(struct txgbe_hw *);
	void (*enable_tx_laser)(struct txgbe_hw *);
	void (*flap_tx_laser)(struct txgbe_hw *);
	s32 (*setup_link)(struct txgbe_hw *, u32, bool);
	s32 (*setup_mac_link)(struct txgbe_hw *, u32, bool);
	s32 (*check_link)(struct txgbe_hw *, u32 *, bool *, bool);
	s32 (*get_link_capabilities)(struct txgbe_hw *, u32 *,
				     bool *);
	void (*set_rate_select_speed)(struct txgbe_hw *, u32);

	/* Packet Buffer manipulation */
	void (*setup_pba)(struct txgbe_hw *, int, u32, int);

	/* LED */
	s32 (*led_on)(struct txgbe_hw *, u32);
	s32 (*led_off)(struct txgbe_hw *, u32);
	s32 (*blink_led_start)(struct txgbe_hw *, u32);
	s32 (*blink_led_stop)(struct txgbe_hw *, u32);
	s32 (*init_led_link_act)(struct txgbe_hw *);

	/* RAR, Multicast, VLAN */
	s32 (*set_rar)(struct txgbe_hw *, u32, u8 *, u32, u32);
	s32 (*set_uc_addr)(struct txgbe_hw *, u32, u8 *);
	s32 (*clear_rar)(struct txgbe_hw *, u32);
	s32 (*insert_mac_addr)(struct txgbe_hw *, u8 *, u32);
	s32 (*set_vmdq)(struct txgbe_hw *, u32, u32);
	s32 (*set_vmdq_san_mac)(struct txgbe_hw *, u32);
	s32 (*clear_vmdq)(struct txgbe_hw *, u32, u32);
	s32 (*init_rx_addrs)(struct txgbe_hw *);
	s32 (*update_uc_addr_list)(struct txgbe_hw *, u8 *, u32,
				   txgbe_mc_addr_itr);
	s32 (*update_mc_addr_list)(struct txgbe_hw *, u8 *, u32,
				   txgbe_mc_addr_itr, bool clear);
	s32 (*enable_mc)(struct txgbe_hw *);
	s32 (*disable_mc)(struct txgbe_hw *);
	s32 (*clear_vfta)(struct txgbe_hw *);
	s32 (*set_vfta)(struct txgbe_hw *, u32, u32, bool, bool);
	s32 (*set_vlvf)(struct txgbe_hw *, u32, u32, bool, u32 *, u32,
			bool);
	s32 (*init_uta_tables)(struct txgbe_hw *);
	void (*set_mac_anti_spoofing)(struct txgbe_hw *, bool, int);
	void (*set_vlan_anti_spoofing)(struct txgbe_hw *, bool, int);
	s32 (*update_xcast_mode)(struct txgbe_hw *, int);
	s32 (*set_rlpml)(struct txgbe_hw *, u16);

	/* Flow Control */
	s32 (*fc_enable)(struct txgbe_hw *);
	s32 (*setup_fc)(struct txgbe_hw *);
	void (*fc_autoneg)(struct txgbe_hw *);

	/* Manageability interface */
	s32 (*set_fw_drv_ver)(struct txgbe_hw *, u8, u8, u8, u8, u16,
			      const char *);
	s32 (*get_thermal_sensor_data)(struct txgbe_hw *);
	s32 (*init_thermal_sensor_thresh)(struct txgbe_hw *hw);
	void (*get_rtrup2tc)(struct txgbe_hw *hw, u8 *map);
	void (*disable_rx)(struct txgbe_hw *hw);
	void (*enable_rx)(struct txgbe_hw *hw);
	void (*set_source_address_pruning)(struct txgbe_hw *, bool,
					   unsigned int);
	void (*set_ethertype_anti_spoofing)(struct txgbe_hw *, bool, int);
	s32 (*dmac_update_tcs)(struct txgbe_hw *hw);
	s32 (*dmac_config_tcs)(struct txgbe_hw *hw);
	s32 (*dmac_config)(struct txgbe_hw *hw);
	s32 (*setup_eee)(struct txgbe_hw *hw, bool enable_eee);
	s32 (*read_iosf_sb_reg)(struct txgbe_hw *, u32, u32, u32 *);
	s32 (*write_iosf_sb_reg)(struct txgbe_hw *, u32, u32, u32);
	void (*disable_mdd)(struct txgbe_hw *hw);
	void (*enable_mdd)(struct txgbe_hw *hw);
	void (*mdd_event)(struct txgbe_hw *hw, u32 *vf_bitmap);
	void (*restore_mdd_vf)(struct txgbe_hw *hw, u32 vf);
	bool (*fw_recovery_mode)(struct txgbe_hw *hw);

	s32 (*kr_handle)(struct txgbe_hw *hw);
	void (*bp_down_event)(struct txgbe_hw *hw);

	enum txgbe_mac_type type;
	u8 addr[ETH_ADDR_LEN];
	u8 perm_addr[ETH_ADDR_LEN];
	u8 san_addr[ETH_ADDR_LEN];
	/* prefix for World Wide Node Name (WWNN) */
	u16 wwnn_prefix;
	/* prefix for World Wide Port Name (WWPN) */
	u16 wwpn_prefix;
#define TXGBE_MAX_MTA			128
	u32 mta_shadow[TXGBE_MAX_MTA];
	s32 mc_filter_type;
	u32 mcft_size;
	u32 vft_size;
	u32 num_rar_entries;
	u32 rar_highwater;
	u32 rx_pb_size;
	u32 max_tx_queues;
	u32 max_rx_queues;
	u64 autoc;
	u64 orig_autoc;  /* cached value of AUTOC */
	u8  san_mac_rar_index;
	bool get_link_status;
	//u16 max_msix_vectors;
	//bool arc_subsystem_valid;
	bool orig_link_settings_stored;
	bool autotry_restart;
	u8 flags;
	struct txgbe_thermal_sensor_data  thermal_sensor_data;
	bool thermal_sensor_enabled;
	struct txgbe_dmac_config dmac_cfg;
	bool set_lben;
	u32  max_link_up_time;
};

struct txgbe_phy_info {
	u32 (*get_media_type)(struct txgbe_hw *);
	s32 (*identify)(struct txgbe_hw *);
	s32 (*identify_sfp)(struct txgbe_hw *);
	s32 (*init)(struct txgbe_hw *);
	s32 (*reset)(struct txgbe_hw *);
	s32 (*read_reg)(struct txgbe_hw *, u32, u32, u16 *);
	s32 (*write_reg)(struct txgbe_hw *, u32, u32, u16);
	s32 (*read_reg_mdi)(struct txgbe_hw *, u32, u32, u16 *);
	s32 (*write_reg_mdi)(struct txgbe_hw *, u32, u32, u16);
	s32 (*setup_link)(struct txgbe_hw *);
	s32 (*setup_internal_link)(struct txgbe_hw *);
	s32 (*setup_link_speed)(struct txgbe_hw *, u32, bool);
	s32 (*check_link)(struct txgbe_hw *, u32 *, bool *);
	s32 (*get_firmware_version)(struct txgbe_hw *, u32 *);
	s32 (*read_i2c_byte)(struct txgbe_hw *, u8, u8, u8 *);
	s32 (*write_i2c_byte)(struct txgbe_hw *, u8, u8, u8);
	s32 (*read_i2c_sff8472)(struct txgbe_hw *, u8 , u8 *);
	s32 (*read_i2c_eeprom)(struct txgbe_hw *, u8 , u8 *);
	s32 (*write_i2c_eeprom)(struct txgbe_hw *, u8, u8);
	void (*i2c_bus_clear)(struct txgbe_hw *);
	s32 (*check_overtemp)(struct txgbe_hw *);
	s32 (*set_phy_power)(struct txgbe_hw *, bool on);
	s32 (*enter_lplu)(struct txgbe_hw *);
	s32 (*handle_lasi)(struct txgbe_hw *hw);
	s32 (*read_i2c_byte_unlocked)(struct txgbe_hw *, u8 offset, u8 addr,
				      u8 *value);
	s32 (*write_i2c_byte_unlocked)(struct txgbe_hw *, u8 offset, u8 addr,
				       u8 value);

	enum txgbe_phy_type type;
	u32 addr;
	u32 id;
	enum txgbe_sfp_type sfp_type;
	bool sfp_setup_needed;
	u32 revision;
	u32 media_type;
	u32 phy_semaphore_mask;
	bool reset_disable;
	u32 autoneg_advertised;
	u32 speeds_supported;
	u32 eee_speeds_supported;
	u32 eee_speeds_advertised;
	enum txgbe_smart_speed smart_speed;
	bool smart_speed_active;
	bool multispeed_fiber;
	bool reset_if_overtemp;
	bool qsfp_shared_i2c_bus;
	u32 nw_mng_if_sel;

	u32 link_mode; //fixme

	/* Some features need tri-state capability */
	u8 ffe_set;
	u8 ffe_main;
	u8 ffe_pre;
	u8 ffe_post;
};

#ifdef RTE_TXGBE_FFE_SET
#define TXGBE_FFE_SET	RTE_TXGBE_FFE_SET
#else
#define TXGBE_FFE_SET	TXGBE_BP_M_NULL
#endif
#ifdef RTE_TXGBE_FFE_MAIN
#define TXGBE_FFE_MAIN	RTE_TXGBE_FFE_MAIN
#endif
#ifdef RTE_TXGBE_FFE_PRE
#define TXGBE_FFE_PRE	RTE_TXGBE_FFE_PRE
#endif
#ifdef RTE_TXGBE_FFE_POST
#define TXGBE_FFE_POST	RTE_TXGBE_FFE_POST
#endif
#ifdef RTE_TXGBE_BP_AUTO
#define TXGBE_BP_AN	1
#else
#define TXGBE_BP_AN	0
#endif
#ifdef RTE_TXGBE_KR_POLL
#define TXGBE_KR_POLL	1
#else
#define TXGBE_KR_POLL	0
#endif
#ifdef RTE_TXGBE_KR_PRESENT
#define TXGBE_KR_PRESENT	1
#else
#define TXGBE_KR_PRESENT	0
#endif
#ifdef RTE_TXGBE_KX_TXRX_PIN
#define TXGBE_KX_TXRX_PIN	1
#else
#define TXGBE_KX_TXRX_PIN	0
#endif
#ifdef RTE_TXGBE_KX_SGMII
#define TXGBE_KX_SGMII	1
#else
#define TXGBE_KX_SGMII	0
#endif

struct txgbe_mbx_stats {
	u32 msgs_tx;
	u32 msgs_rx;

	u32 acks;
	u32 reqs;
	u32 rsts;
};

struct txgbe_mbx_info {
	void (*init_params)(struct txgbe_hw *hw);
	s32  (*read)(struct txgbe_hw *, u32 *, u16,  u16);
	s32  (*write)(struct txgbe_hw *, u32 *, u16, u16);
	s32  (*read_posted)(struct txgbe_hw *, u32 *, u16,  u16);
	s32  (*write_posted)(struct txgbe_hw *, u32 *, u16, u16);
	s32  (*check_for_msg)(struct txgbe_hw *, u16);
	s32  (*check_for_ack)(struct txgbe_hw *, u16);
	s32  (*check_for_rst)(struct txgbe_hw *, u16);

	struct txgbe_mbx_stats stats;
	u32 timeout;
	u32 usec_delay;
	u32 v2p_mailbox;
	u16 size;
};

enum txgbe_isb_idx {
	TXGBE_ISB_HEADER,
	TXGBE_ISB_MISC,
	TXGBE_ISB_VEC0,
	TXGBE_ISB_VEC1,
	TXGBE_ISB_MAX
};

struct txgbe_hw {
	void IOMEM *hw_addr;
	void *back;
	struct txgbe_mac_info mac;
	struct txgbe_addr_filter_info addr_ctrl;
	struct txgbe_fc_info fc;
	struct txgbe_phy_info phy;
	struct txgbe_link_info link;
	struct txgbe_rom_info rom;
	struct txgbe_flash_info flash;
	struct txgbe_bus_info bus;
	struct txgbe_mbx_info mbx;
	//const u32 *mvals;
	u16 device_id;
	u16 vendor_id;
	u16 subsystem_device_id;
	u16 subsystem_vendor_id;
	u8 port_id;
	u8 revision_id;
	bool adapter_stopped;
	int api_version;
	bool force_full_reset;
	bool allow_unsupported_sfp;
	bool wol_enabled;
	bool need_crosstalk_fix;
	bool dev_start;

	u32 b4_buf[16];
	uint64_t isb_dma;
	void IOMEM *isb_mem;
	u16 nb_rx_queues;
	u16 nb_tx_queues;
#if 1 /* test by wangjian <begin> */
	u32 fw_version;
	u32 mode;
	enum txgbe_link_status {
		TXGBE_LINK_STATUS_NONE = 0,
		TXGBE_LINK_STATUS_KX,
		TXGBE_LINK_STATUS_KX4
	} link_status;
	enum txgbe_reset_type {
		TXGBE_LAN_RESET = 0,
		TXGBE_SW_RESET,
		TXGBE_GLOBAL_RESET
	} reset_type;

	/* For vector PMD */
	bool rx_bulk_alloc_allowed;
	bool rx_vec_allowed;
	bool tx_vec_allowed;
#endif /* test by wangjian <end> */
	u32 q_rx_regs[128*4];
	u32 q_tx_regs[128*4];
	bool offset_loaded;
	bool rx_loaded;
	struct {
		u64 rx_qp_packets;
		u64 tx_qp_packets;
		u64 rx_qp_bytes;
		u64 tx_qp_bytes;
		u64 rx_qp_mc_packets;
	} qp_last[TXGBE_MAX_QP];
};

struct txgbe_backplane_ability {
	u32 next_page;	  //Next Page (bit0)
	u32 link_ability; //Link Ability (bit[7:0])
	u32 fec_ability;  //FEC Request (bit1), FEC Enable  (bit0)
	u32 current_link_mode; //current link mode for local device
};

/*N*//*#define txgbe_call_func(hw, func, params, error) \
		(func != NULL) ? func params : error*/

/*N*///#define TXGBE_FUSES0_GROUP(_i)		(0x11158 + ((_i) * 4))
/*N*///#define TXGBE_FUSES0_300MHZ		(1 << 5)
/*N*///#define TXGBE_FUSES0_REV_MASK		(3 << 6)

/*N*///#define TXGBE_KRM_PORT_CAR_GEN_CTRL(P)	((P) ? 0x8010 : 0x4010)
/*N*///#define TXGBE_KRM_LINK_S1(P)		((P) ? 0x8200 : 0x4200)
/*N*///#define TXGBE_KRM_LINK_CTRL_1(P)	((P) ? 0x820C : 0x420C)
/*N*///#define TXGBE_KRM_AN_CNTL_1(P)		((P) ? 0x822C : 0x422C)
/*N*///#define TXGBE_KRM_AN_CNTL_4(P)		((P) ? 0x8238 : 0x4238)
/*N*///#define TXGBE_KRM_AN_CNTL_8(P)		((P) ? 0x8248 : 0x4248)
/*N*///#define TXGBE_KRM_PCS_KX_AN(P)		((P) ? 0x9918 : 0x5918)
/*N*///#define TXGBE_KRM_PCS_KX_AN_LP(P)	((P) ? 0x991C : 0x591C)
/*N*///#define TXGBE_KRM_SGMII_CTRL(P)		((P) ? 0x82A0 : 0x42A0)
/*N*///#define TXGBE_KRM_LP_BASE_PAGE_HIGH(P)	((P) ? 0x836C : 0x436C)
/*N*///#define TXGBE_KRM_DSP_TXFFE_STATE_4(P)	((P) ? 0x8634 : 0x4634)
/*N*///#define TXGBE_KRM_DSP_TXFFE_STATE_5(P)	((P) ? 0x8638 : 0x4638)
/*N*///#define TXGBE_KRM_RX_TRN_LINKUP_CTRL(P)	((P) ? 0x8B00 : 0x4B00)
/*N*///#define TXGBE_KRM_PMD_DFX_BURNIN(P)	((P) ? 0x8E00 : 0x4E00)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20(P)	((P) ? 0x9054 : 0x5054)
/*N*///#define TXGBE_KRM_TX_COEFF_CTRL_1(P)	((P) ? 0x9520 : 0x5520)
/*N*///#define TXGBE_KRM_RX_ANA_CTL(P)		((P) ? 0x9A00 : 0x5A00)

/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SFI_10G_DA		~(0x3 << 20)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SFI_10G_SR		(1u << 20)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SFI_10G_LR		(0x2 << 20)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SGMII_EN		(1u << 25)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_AN37_EN		(1u << 26)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_AN_EN		(1u << 27)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_10M		~(0x7 << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_100M		(1u << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_1G		(0x2 << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_10G		(0x3 << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_AN		(0x4 << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_2_5G		(0x7 << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_SPEED_MASK		(0x7 << 28)
/*N*///#define TXGBE_KRM_PMD_FLX_MASK_ST20_FW_AN_RESTART	(1u << 31)

/*N*///#define TXGBE_KRM_PORT_CAR_GEN_CTRL_NELB_32B		(1 << 9)
/*N*///#define TXGBE_KRM_PORT_CAR_GEN_CTRL_NELB_KRPCS		(1 << 11)

/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_FORCE_SPEED_MASK	(0x7 << 8)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_FORCE_SPEED_1G	(2 << 8)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_FORCE_SPEED_10G	(4 << 8)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_SGMII_EN		(1 << 12)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_CLAUSE_37_EN	(1 << 13)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_FEC_REQ		(1 << 14)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_CAP_FEC		(1 << 15)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_CAP_KX		(1 << 16)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_CAP_KR		(1 << 18)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_EEE_CAP_KX		(1 << 24)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_EEE_CAP_KR		(1 << 26)
/*N*///#define TXGBE_KRM_LINK_S1_MAC_AN_COMPLETE		(1 << 28)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_ENABLE		(1 << 29)
/*N*///#define TXGBE_KRM_LINK_CTRL_1_TETH_AN_RESTART		(1 << 31)

/*N*///#define TXGBE_KRM_AN_CNTL_1_SYM_PAUSE			(1 << 28)
/*N*///#define TXGBE_KRM_AN_CNTL_1_ASM_PAUSE			(1 << 29)
/*N*///#define TXGBE_KRM_PCS_KX_AN_SYM_PAUSE			(1 << 1)
/*N*///#define TXGBE_KRM_PCS_KX_AN_ASM_PAUSE			(1 << 2)
/*N*///#define TXGBE_KRM_PCS_KX_AN_LP_SYM_PAUSE		(1 << 2)
/*N*///#define TXGBE_KRM_PCS_KX_AN_LP_ASM_PAUSE		(1 << 3)
/*N*///#define TXGBE_KRM_AN_CNTL_4_ECSR_AN37_OVER_73		(1 << 29)
/*N*///#define TXGBE_KRM_AN_CNTL_8_LINEAR			(1 << 0)
/*N*///#define TXGBE_KRM_AN_CNTL_8_LIMITING			(1 << 1)

/*N*///#define TXGBE_KRM_LP_BASE_PAGE_HIGH_SYM_PAUSE		(1 << 10)
/*N*///#define TXGBE_KRM_LP_BASE_PAGE_HIGH_ASM_PAUSE		(1 << 11)

/*N*///#define TXGBE_KRM_SGMII_CTRL_MAC_TAR_FORCE_100_D	(1 << 12)
/*N*///#define TXGBE_KRM_SGMII_CTRL_MAC_TAR_FORCE_10_D		(1 << 19)

/*N*///#define TXGBE_KRM_DSP_TXFFE_STATE_C0_EN			(1 << 6)
/*N*///#define TXGBE_KRM_DSP_TXFFE_STATE_CP1_CN1_EN		(1 << 15)
/*N*///#define TXGBE_KRM_DSP_TXFFE_STATE_CO_ADAPT_EN		(1 << 16)

/*N*///#define TXGBE_KRM_RX_TRN_LINKUP_CTRL_CONV_WO_PROTOCOL	(1 << 4)
/*N*///#define TXGBE_KRM_RX_TRN_LINKUP_CTRL_PROTOCOL_BYPASS	(1 << 2)

/*N*///#define TXGBE_KRM_PMD_DFX_BURNIN_TX_RX_KR_LB_MASK	(0x3 << 16)

/*N*///#define TXGBE_KRM_TX_COEFF_CTRL_1_CMINUS1_OVRRD_EN	(1 << 1)
/*N*///#define TXGBE_KRM_TX_COEFF_CTRL_1_CPLUS1_OVRRD_EN	(1 << 2)
/*N*///#define TXGBE_KRM_TX_COEFF_CTRL_1_CZERO_EN		(1 << 3)
/*N*///#define TXGBE_KRM_TX_COEFF_CTRL_1_OVRRD_EN		(1 << 31)

/*N*///#define TXGBE_SB_IOSF_INDIRECT_CTRL	0x00011144
/*N*///#define TXGBE_SB_IOSF_INDIRECT_DATA	0x00011148

/*N*///#define TXGBE_SB_IOSF_CTRL_ADDR_SHIFT		0
/*N*///#define TXGBE_SB_IOSF_CTRL_ADDR_MASK		0xFF
/*N*///#define TXGBE_SB_IOSF_CTRL_RESP_STAT_SHIFT	18
/*N*//*#define TXGBE_SB_IOSF_CTRL_RESP_STAT_MASK	\
				(0x3 << TXGBE_SB_IOSF_CTRL_RESP_STAT_SHIFT)*/
/*N*///#define TXGBE_SB_IOSF_CTRL_CMPL_ERR_SHIFT	20
/*N*//*#define TXGBE_SB_IOSF_CTRL_CMPL_ERR_MASK	\
				(0xFF << TXGBE_SB_IOSF_CTRL_CMPL_ERR_SHIFT)*/
/*N*///#define TXGBE_SB_IOSF_CTRL_TARGET_SELECT_SHIFT	28
/*N*///#define TXGBE_SB_IOSF_CTRL_TARGET_SELECT_MASK	0x7
/*N*///#define TXGBE_SB_IOSF_CTRL_BUSY_SHIFT		31
/*N*///#define TXGBE_SB_IOSF_CTRL_BUSY		(1 << TXGBE_SB_IOSF_CTRL_BUSY_SHIFT)
/*N*///#define TXGBE_SB_IOSF_TARGET_KR_PHY	0

/*N*///#define TXGBE_NW_MNG_IF_SEL		0x00011178
/*N*///#define TXGBE_NW_MNG_IF_SEL_MDIO_ACT	(1u << 1)
/*N*///#define TXGBE_NW_MNG_IF_SEL_MDIO_IF_MODE	(1u << 2)
/*N*///#define TXGBE_NW_MNG_IF_SEL_EN_SHARED_MDIO	(1u << 13)
/*N*///#define TXGBE_NW_MNG_IF_SEL_PHY_SPEED_10M	(1u << 17)
/*N*///#define TXGBE_NW_MNG_IF_SEL_PHY_SPEED_100M	(1u << 18)
/*N*///#define TXGBE_NW_MNG_IF_SEL_PHY_SPEED_1G	(1u << 19)
/*N*///#define TXGBE_NW_MNG_IF_SEL_PHY_SPEED_2_5G	(1u << 20)
/*N*///#define TXGBE_NW_MNG_IF_SEL_PHY_SPEED_10G	(1u << 21)
/*N*///#define TXGBE_NW_MNG_IF_SEL_SGMII_ENABLE	(1u << 25)
/*N*///#define TXGBE_NW_MNG_IF_SEL_INT_PHY_MODE (1 << 24) /* X552 reg field only */
/*N*///#define TXGBE_NW_MNG_IF_SEL_MDIO_PHY_ADD_SHIFT 3
/*N*//*#define TXGBE_NW_MNG_IF_SEL_MDIO_PHY_ADD	\
				(0x1F << TXGBE_NW_MNG_IF_SEL_MDIO_PHY_ADD_SHIFT)*/
#include "txgbe_regs.h"
#include "txgbe_dummy.h"

#endif /* _TXGBE_TYPE_H_ */
