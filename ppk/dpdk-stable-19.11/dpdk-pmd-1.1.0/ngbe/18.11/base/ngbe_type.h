/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_TYPE_H_
#define _NGBE_TYPE_H_

/*
 * Driver Configuration
 */
/* DCB configuration defines */
#define NGBE_DCB_TC_MAX	NGBE_MAX_UP

#define NGBE_LINK_UP_TIME         90 /* 9.0 Seconds */
#define NGBE_AUTO_NEG_TIME		45 /* 4.5 Seconds */

#define NGBE_RX_HDR_SIZE    256
#define NGBE_RX_BUF_SIZE    2048


#define NGBE_FRAME_SIZE_MAX       (9728) /* Maximum frame size, +FCS */
#define NGBE_FRAME_SIZE_DFT       (1522) /* Default frame size, +FCS */
#define NGBE_NUM_POOL             (32)
#define NGBE_PBRXSIZE_MAX         0x00080000 /* 512KB Packet Buffer */
#define NGBE_TXPKTSIZE_MAX        (10)
#define NGBE_PBTXSIZE_MAX         0x00005000 /* 20KB Packet Buffer */
#define NGBE_FDIR_DROP_QUEUE      127
#define NGBE_MAX_FTQF_FILTERS     8
#define NGBE_TXPKT_SIZE_MAX    0xA /* Max Tx Packet size */
#define NGBE_MAX_UP            8
#define NGBE_MAX_QP (8)

#define NGBE_MAX_UTA                   128

#define NGBE_MD_TIMEOUT 1000
#define NGBE_SPI_TIMEOUT  1000
#define NGBE_VF_INIT_TIMEOUT	200 /* Number of retries to clear RSTI */
#define NGBE_PCI_MASTER_DISABLE_TIMEOUT	800

#define NGBE_MAX_MSIX_VECTORS_EMERALD	0x09

#define NGBE_ALIGN		128 /* as intel did */

/*
 * The following is a brief description of the error categories used by the
 * ERROR_REPORT* macros.
 *
 * - NGBE_ERROR_INVALID_STATE
 * This category is for errors which represent a serious failure state that is
 * unexpected, and could be potentially harmful to device operation. It should
 * not be used for errors relating to issues that can be worked around or
 * ignored.
 *
 * - NGBE_ERROR_POLLING
 * This category is for errors related to polling/timeout issues and should be
 * used in any case where the timeout occured, or a failure to obtain a lock, or
 * failure to receive data within the time limit.
 *
 * - NGBE_ERROR_CAUTION
 * This category should be used for reporting issues that may be the cause of
 * other errors, such as temperature warnings. It should indicate an event which
 * could be serious, but hasn't necessarily caused problems yet.
 *
 * - NGBE_ERROR_SOFTWARE
 * This category is intended for errors due to software state preventing
 * something. The category is not intended for errors due to bad arguments, or
 * due to unsupported features. It should be used when a state occurs which
 * prevents action but is not a serious issue.
 *
 * - NGBE_ERROR_ARGUMENT
 * This category is for when a bad or invalid argument is passed. It should be
 * used whenever a function is called and error checking has detected the
 * argument is wrong or incorrect.
 *
 * - NGBE_ERROR_UNSUPPORTED
 * This category is for errors which are due to unsupported circumstances or
 * configuration issues. It should not be used when the issue is due to an
 * invalid argument, but for when something has occurred that is unsupported
 * (Ex: Flow control autonegotiation or an unsupported SFP+ module.)
 */

#include "ngbe_status.h"
#include "ngbe_osdep.h"
#include "ngbe_devids.h"

/* Override this by setting IOMEM in your ngbe_osdep.h header */

//#define NGBE_CAT(r, m) NGBE_##r##m

//#define NGBE_BY_MAC(_hw, r) ((_hw)->mvals[NGBE_CAT(r, _IDX)])

struct ngbe_thermal_diode_data {
	//u8 location;
	s16 temp;
	//u8 caution_thresh;
	//u8 max_op_thresh;
	s16 alarm_thresh;
	s16 dalarm_thresh;
};

struct ngbe_thermal_sensor_data {
	struct ngbe_thermal_diode_data sensor[1];
};

struct ngbe_nvm_version {
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

/* Power Management */
/* DMA Coalescing configuration */
struct ngbe_dmac_config {
	u16	watchdog_timer; /* usec units */
	bool	fcoe_en;
	u32	link_speed;
	u8	fcoe_tc;
	u8	num_tcs;
};

/* Packet buffer allocation strategies */
enum {
	PBA_STRATEGY_EQUAL	= 0, /* Distribute PB space equally */
#define PBA_STRATEGY_EQUAL	PBA_STRATEGY_EQUAL
	PBA_STRATEGY_WEIGHTED	= 1, /* Weight front half of TCs */
#define PBA_STRATEGY_WEIGHTED	PBA_STRATEGY_WEIGHTED
};


/* Physical layer type */
/*N*///typedef u64 ngbe_physical_layer;
#define NGBE_PHYSICAL_LAYER_UNKNOWN		0
#define NGBE_PHYSICAL_LAYER_10GBASE_T		0x00001
#define NGBE_PHYSICAL_LAYER_1000BASE_T		0x00002
#define NGBE_PHYSICAL_LAYER_100BASE_TX		0x00004
#define NGBE_PHYSICAL_LAYER_SFP_PLUS_CU	0x00008
#define NGBE_PHYSICAL_LAYER_10GBASE_LR		0x00010
#define NGBE_PHYSICAL_LAYER_10GBASE_LRM	0x00020
#define NGBE_PHYSICAL_LAYER_10GBASE_SR		0x00040
#define NGBE_PHYSICAL_LAYER_10GBASE_KX4	0x00080
#define NGBE_PHYSICAL_LAYER_10GBASE_CX4	0x00100
#define NGBE_PHYSICAL_LAYER_1000BASE_KX	0x00200
#define NGBE_PHYSICAL_LAYER_1000BASE_BX	0x00400
#define NGBE_PHYSICAL_LAYER_10GBASE_KR		0x00800
#define NGBE_PHYSICAL_LAYER_10GBASE_XAUI	0x01000
#define NGBE_PHYSICAL_LAYER_SFP_ACTIVE_DA	0x02000
#define NGBE_PHYSICAL_LAYER_1000BASE_SX	0x04000
#define NGBE_PHYSICAL_LAYER_10BASE_T		0x08000
#define NGBE_PHYSICAL_LAYER_2500BASE_KX	0x10000

/* Software ATR hash keys */
#define NGBE_ATR_BUCKET_HASH_KEY	0x3DAD14E2
#define NGBE_ATR_SIGNATURE_HASH_KEY	0x174D3614

/* Software ATR input stream values and masks */
#define NGBE_ATR_HASH_MASK		0x7fff

enum ngbe_atr_flow_type {
	NGBE_ATR_FLOW_TYPE_IPV4	= 0x0,
	NGBE_ATR_FLOW_TYPE_UDPV4	= 0x1,
	NGBE_ATR_FLOW_TYPE_TCPV4	= 0x2,
	NGBE_ATR_FLOW_TYPE_SCTPV4	= 0x3,
	NGBE_ATR_FLOW_TYPE_IPV6	= 0x4,
	NGBE_ATR_FLOW_TYPE_UDPV6	= 0x5,
	NGBE_ATR_FLOW_TYPE_TCPV6	= 0x6,
	NGBE_ATR_FLOW_TYPE_SCTPV6	= 0x7,
	NGBE_ATR_FLOW_TYPE_TUNNELED_IPV4	= 0x10,
	NGBE_ATR_FLOW_TYPE_TUNNELED_UDPV4	= 0x11,
	NGBE_ATR_FLOW_TYPE_TUNNELED_TCPV4	= 0x12,
	NGBE_ATR_FLOW_TYPE_TUNNELED_SCTPV4	= 0x13,
	NGBE_ATR_FLOW_TYPE_TUNNELED_IPV6	= 0x14,
	NGBE_ATR_FLOW_TYPE_TUNNELED_UDPV6	= 0x15,
	NGBE_ATR_FLOW_TYPE_TUNNELED_TCPV6	= 0x16,
	NGBE_ATR_FLOW_TYPE_TUNNELED_SCTPV6	= 0x17,
};

/* Flow Director ATR input struct. */
struct ngbe_atr_input {
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
union ngbe_atr_hash_dword {
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


/*#define NGBE_MVALS_INIT(m)	\
	NGBE_CAT(EEC, m),		\
	NGBE_CAT(FLA, m),		\
	NGBE_CAT(GRC, m),		\
	NGBE_CAT(SRAMREL, m),		\
	NGBE_CAT(FACTPS, m),		\
	NGBE_CAT(SWSM, m),		\
	NGBE_CAT(SWFW_SYNC, m),	\
	NGBE_CAT(FWSM, m),		\
	NGBE_CAT(SDP0_GPIEN, m),	\
	NGBE_CAT(SDP1_GPIEN, m),	\
	NGBE_CAT(SDP2_GPIEN, m),	\
	NGBE_CAT(EICR_GPI_SDP0, m),	\
	NGBE_CAT(EICR_GPI_SDP1, m),	\
	NGBE_CAT(EICR_GPI_SDP2, m),	\
	NGBE_CAT(CIAA, m),		\
	NGBE_CAT(CIAD, m),		\
	NGBE_CAT(I2C_CLK_IN, m),	\
	NGBE_CAT(I2C_CLK_OUT, m),	\
	NGBE_CAT(I2C_DATA_IN, m),	\
	NGBE_CAT(I2C_DATA_OUT, m),	\
	NGBE_CAT(I2C_DATA_OE_N_EN, m),	\
	NGBE_CAT(I2C_BB_EN, m),	\
	NGBE_CAT(I2C_CLK_OE_N_EN, m),	\
	NGBE_CAT(I2CCTL, m)

enum ngbe_mvals {
	NGBE_MVALS_INIT(_IDX),
	NGBE_MVALS_IDX_LIMIT
};*/

/*
 * Unavailable: The FCoE Boot Option ROM is not present in the flash.
 * Disabled: Present; boot order is not set for any targets on the port.
 * Enabled: Present; boot order is set for at least one target on the port.
 */
enum ngbe_fcoe_boot_status {
	ngbe_fcoe_bootstatus_disabled = 0,
	ngbe_fcoe_bootstatus_enabled = 1,
	ngbe_fcoe_bootstatus_unavailable = 0xFFFF
};

enum ngbe_eeprom_type {
	ngbe_eeprom_unknown = 0,
	ngbe_eeprom_spi,
	ngbe_eeprom_flash,
	ngbe_eeprom_none /* No NVM support */
};

enum ngbe_link_type {
	ngbe_link_fiber = 0,
	ngbe_link_copper
};

enum ngbe_mac_type {
	ngbe_mac_unknown = 0,
	ngbe_mac_sp,
	ngbe_mac_sp_vf,
	ngbe_mac_em,
	ngbe_mac_em_vf,
	ngbe_num_macs
};

enum ngbe_phy_type {
	ngbe_phy_unknown = 0,
	ngbe_phy_none,
	ngbe_phy_rtl,
	ngbe_phy_mvl,
	ngbe_phy_mvl_sfi,
	ngbe_phy_yt8521s,
	ngbe_phy_yt8521s_sfi,
	ngbe_phy_rtl_yt8521s_sfi,
	ngbe_phy_zte,
	ngbe_phy_tn,
	ngbe_phy_aq,
	ngbe_phy_ext_1g_t,
	ngbe_phy_cu_mtd,
	ngbe_phy_cu_unknown,
	ngbe_phy_qt,
	ngbe_phy_xaui,
	ngbe_phy_nl,
	ngbe_phy_sfp_tyco_passive,
	ngbe_phy_sfp_unknown_passive,
	ngbe_phy_sfp_unknown_active,
	ngbe_phy_sfp_avago,
	ngbe_phy_sfp_ftl,
	ngbe_phy_sfp_ftl_active,
	ngbe_phy_sfp_unknown,
	ngbe_phy_sfp_intel,
	ngbe_phy_qsfp_unknown_passive,
	ngbe_phy_qsfp_unknown_active,
	ngbe_phy_qsfp_intel,
	ngbe_phy_qsfp_unknown,
	ngbe_phy_sfp_unsupported, /*Enforce bit set with unsupported module*/
	ngbe_phy_sgmii,
	ngbe_phy_fw,
	ngbe_phy_generic
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
enum ngbe_sfp_type {
	ngbe_sfp_type_unknown = 0,
	ngbe_sfp_type_da_cu,
	ngbe_sfp_type_sr,
	ngbe_sfp_type_lr,
	ngbe_sfp_type_da_cu_core0,
	ngbe_sfp_type_da_cu_core1,
	ngbe_sfp_type_srlr_core0,
	ngbe_sfp_type_srlr_core1,
	ngbe_sfp_type_da_act_lmt_core0,
	ngbe_sfp_type_da_act_lmt_core1,
	ngbe_sfp_type_1g_cu_core0,
	ngbe_sfp_type_1g_cu_core1,
	ngbe_sfp_type_1g_sx_core0,
	ngbe_sfp_type_1g_sx_core1,
	ngbe_sfp_type_1g_lx_core0,
	ngbe_sfp_type_1g_lx_core1,
	ngbe_sfp_type_not_present = 0xFFFE,
	ngbe_sfp_type_not_known = 0xFFFF
};

enum ngbe_media_type {
	ngbe_media_type_unknown = 0,
	ngbe_media_type_fiber,
	ngbe_media_type_fiber_qsfp,
	ngbe_media_type_copper,
	ngbe_media_type_backplane,
	ngbe_media_type_cx4,
	ngbe_media_type_virtual
};

/* Flow Control Settings */
enum ngbe_fc_mode {
	ngbe_fc_none = 0,
	ngbe_fc_rx_pause,
	ngbe_fc_tx_pause,
	ngbe_fc_full,
	ngbe_fc_default
};

/* Smart Speed Settings */
#define NGBE_SMARTSPEED_MAX_RETRIES	3
enum ngbe_smart_speed {
	ngbe_smart_speed_auto = 0,
	ngbe_smart_speed_on,
	ngbe_smart_speed_off
};

/* PCI bus types */
enum ngbe_bus_type {
	ngbe_bus_type_unknown = 0,
	ngbe_bus_type_pci,
	ngbe_bus_type_pcix,
	ngbe_bus_type_pci_express,
	ngbe_bus_type_internal,
	ngbe_bus_type_reserved
};

/* PCI bus speeds */
enum ngbe_bus_speed {
	ngbe_bus_speed_unknown	= 0,
	ngbe_bus_speed_33	= 33,
	ngbe_bus_speed_66	= 66,
	ngbe_bus_speed_100	= 100,
	ngbe_bus_speed_120	= 120,
	ngbe_bus_speed_133	= 133,
	ngbe_bus_speed_2500	= 2500,
	ngbe_bus_speed_5000	= 5000,
	ngbe_bus_speed_8000	= 8000,
	ngbe_bus_speed_reserved
};

/* PCI bus widths */
enum ngbe_bus_width {
	ngbe_bus_width_unknown	= 0,
	ngbe_bus_width_pcie_x1	= 1,
	ngbe_bus_width_pcie_x2	= 2,
	ngbe_bus_width_pcie_x4	= 4,
	ngbe_bus_width_pcie_x8	= 8,
	ngbe_bus_width_32	= 32,
	ngbe_bus_width_64	= 64,
	ngbe_bus_width_reserved
};

struct ngbe_hw;

struct ngbe_addr_filter_info {
	u32 num_mc_addrs;
	u32 rar_used_count;
	u32 mta_in_use;
	u32 overflow_promisc;
	bool user_set_promisc;
};

/* Bus parameters */
struct ngbe_bus_info {
	s32 (*get_bus_info)(struct ngbe_hw *);
	void (*set_lan_id)(struct ngbe_hw *);

	enum ngbe_bus_speed speed;
	enum ngbe_bus_width width;
	enum ngbe_bus_type type;

	u16 func;
	u8 lan_id;
	u16 instance_id;
};

/* Flow control parameters */
struct ngbe_fc_info {
	u32 high_water; /* Flow Ctrl High-water */
	u32 low_water; /* Flow Ctrl Low-water */
	u16 pause_time; /* Flow Control Pause timer */
	bool send_xon; /* Flow control send XON */
	bool strict_ieee; /* Strict IEEE mode */
	bool disable_fc_autoneg; /* Do not autonegotiate FC */
	bool fc_was_autonegged; /* Is current_mode the result of autonegging? */
	enum ngbe_fc_mode current_mode; /* FC mode in effect */
	enum ngbe_fc_mode requested_mode; /* FC mode requested by caller */
};

/* Statistics counters collected by the MAC */
/* PB[] RxTx */
struct ngbe_pb_stats {
	u64 tx_pb_xon_packets; //pxontxc;
	u64 rx_pb_xon_packets; //pxonrxc;
	u64 tx_pb_xoff_packets; //pxofftxc;
	u64 rx_pb_xoff_packets; //pxoffrxc;
	u64 rx_pb_dropped; //mpc;
	u64 rx_pb_mbuf_alloc_errors; //rnbc;
	u64 tx_pb_xon2off_packets; //pxon2offc;
};

/* QP[] RxTx */
struct ngbe_qp_stats {
	u64 rx_qp_packets; //qprc;
	u64 tx_qp_packets; //qptc;
	u64 rx_qp_bytes; //qbrc;
	u64 tx_qp_bytes; //qbtc;
	u64 rx_qp_mc_packets; //qprdc;
};

struct ngbe_hw_stats {
	/* MNG RxTx */
	u64 mng_bmc2host_packets; //b2ospc
	u64 mng_host2bmc_packets; //o2bspc
	/* Basix RxTx */
	u64 rx_drop_packets;
	u64 tx_drop_packets;
	u64 rx_dma_drop;
	u64 tx_secdrp_packets;
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

	u64 rx_up_dropped;

	u64 rdb_pkt_cnt;
	u64 rdb_repli_cnt;
	u64 rdb_drp_cnt;

	/* QP[] RxTx */
	struct {
		u64 rx_qp_packets;
		u64 tx_qp_packets;
		u64 rx_qp_bytes;
		u64 tx_qp_bytes;
		u64 rx_qp_mc_packets;
		u64 tx_qp_mc_packets;
		u64 rx_qp_bc_packets;
		u64 tx_qp_bc_packets;
	} qp[NGBE_MAX_QP];

};

/* iterator type for walking multicast address lists */
typedef u8* (*ngbe_mc_addr_itr) (struct ngbe_hw *hw, u8 **mc_addr_ptr,
				  u32 *vmdq);

struct ngbe_link_info {
	s32 (*read_link)(struct ngbe_hw *, u8 addr, u16 reg, u16 *val);
	s32 (*read_link_unlocked)(struct ngbe_hw *, u8 addr, u16 reg,
				  u16 *val);
	s32 (*write_link)(struct ngbe_hw *, u8 addr, u16 reg, u16 val);
	s32 (*write_link_unlocked)(struct ngbe_hw *, u8 addr, u16 reg,
				   u16 val);

	u8 addr;
};

struct ngbe_rom_info {
	s32 (*init_params)(struct ngbe_hw *);
	s32 (*read16)(struct ngbe_hw *, u32, u16 *);
	s32 (*readw_sw)(struct ngbe_hw *, u32, u16 *);
	s32 (*readw_buffer)(struct ngbe_hw *, u32, u32, void *);
	s32 (*read32)(struct ngbe_hw *, u32, u32 *);
	s32 (*read_buffer)(struct ngbe_hw *, u32, u32, void *);
	s32 (*write16)(struct ngbe_hw *, u32, u16);
	s32 (*writew_sw)(struct ngbe_hw *, u32, u16);
	s32 (*writew_buffer)(struct ngbe_hw *, u32, u32, void *);
	s32 (*write32)(struct ngbe_hw *, u32, u32);
	s32 (*write_buffer)(struct ngbe_hw *, u32, u32, void *);
	s32 (*validate_checksum)(struct ngbe_hw *, u16 *);
	s32 (*update_checksum)(struct ngbe_hw *);
	s32 (*calc_checksum)(struct ngbe_hw *);

	enum ngbe_eeprom_type type;
	u32 semaphore_delay;
	u16 word_size;
	u16 address_bits;
	u16 word_page_size;
	u16 ctrl_word_3;

	u32 sw_addr; //fixme
	u32 saved_version;
	u16 cksum_devcap;
};


struct ngbe_flash_info {
	s32 (*init_params)(struct ngbe_hw *);
	s32 (*read_buffer)(struct ngbe_hw *, u32, u32, u32 *);
	s32 (*write_buffer)(struct ngbe_hw *, u32, u32, u32 *);
	u32 semaphore_delay;
	u32 dword_size;
	u16 address_bits;
};

#define NGBE_FLAGS_DOUBLE_RESET_REQUIRED	0x01
struct ngbe_mac_info {
	s32 (*init_params)(struct ngbe_hw *);
	s32 (*init_hw)(struct ngbe_hw *);
	s32 (*reset_hw)(struct ngbe_hw *);
	s32 (*start_hw)(struct ngbe_hw *);
	s32 (*stop_hw)(struct ngbe_hw *);
	s32 (*clear_hw_cntrs)(struct ngbe_hw *);
	void (*enable_relaxed_ordering)(struct ngbe_hw *);
	u64 (*get_supported_physical_layer)(struct ngbe_hw *);
	s32 (*get_mac_addr)(struct ngbe_hw *, u8 *);
	s32 (*get_san_mac_addr)(struct ngbe_hw *, u8 *);
	s32 (*set_san_mac_addr)(struct ngbe_hw *, u8 *);
	s32 (*get_device_caps)(struct ngbe_hw *, u16 *);
	s32 (*get_wwn_prefix)(struct ngbe_hw *, u16 *, u16 *);
	s32 (*get_fcoe_boot_status)(struct ngbe_hw *, u16 *);
	s32 (*read_analog_reg8)(struct ngbe_hw*, u32, u8*);
	s32 (*write_analog_reg8)(struct ngbe_hw*, u32, u8);
	s32 (*setup_sfp)(struct ngbe_hw *);
	s32 (*enable_rx_dma)(struct ngbe_hw *, u32);
	s32 (*disable_sec_rx_path)(struct ngbe_hw *);
	s32 (*enable_sec_rx_path)(struct ngbe_hw *);
	s32 (*disable_sec_tx_path)(struct ngbe_hw *);
	s32 (*enable_sec_tx_path)(struct ngbe_hw *);
	s32 (*acquire_swfw_sync)(struct ngbe_hw *, u32);
	void (*release_swfw_sync)(struct ngbe_hw *, u32);
	void (*init_swfw_sync)(struct ngbe_hw *);
	u64 (*autoc_read)(struct ngbe_hw *);
	void (*autoc_write)(struct ngbe_hw *, u64);
	s32 (*prot_autoc_read)(struct ngbe_hw *, bool *, u64 *);
	s32 (*prot_autoc_write)(struct ngbe_hw *, bool, u64);
	s32 (*negotiate_api_version)(struct ngbe_hw *hw, int api);

	/* Link */
	void (*disable_tx_laser)(struct ngbe_hw *);
	void (*enable_tx_laser)(struct ngbe_hw *);
	void (*flap_tx_laser)(struct ngbe_hw *);
	s32 (*setup_link)(struct ngbe_hw *, u32, bool);
	s32 (*setup_mac_link)(struct ngbe_hw *, u32, bool);
	s32 (*check_link)(struct ngbe_hw *, u32 *, bool *, bool);
	s32 (*get_link_capabilities)(struct ngbe_hw *, u32 *,
				     bool *);
	void (*set_rate_select_speed)(struct ngbe_hw *, u32);

	/* Packet Buffer manipulation */
	void (*setup_pba)(struct ngbe_hw *, int, u32, int);

	/* LED */
	s32 (*led_on)(struct ngbe_hw *, u32);
	s32 (*led_off)(struct ngbe_hw *, u32);
	s32 (*blink_led_start)(struct ngbe_hw *, u32);
	s32 (*blink_led_stop)(struct ngbe_hw *, u32);
	s32 (*init_led_link_act)(struct ngbe_hw *);

	/* RAR, Multicast, VLAN */
	s32 (*set_rar)(struct ngbe_hw *, u32, u8 *, u32, u32);
	s32 (*set_uc_addr)(struct ngbe_hw *, u32, u8 *);
	s32 (*clear_rar)(struct ngbe_hw *, u32);
	s32 (*insert_mac_addr)(struct ngbe_hw *, u8 *, u32);
	s32 (*set_vmdq)(struct ngbe_hw *, u32, u32);
	s32 (*set_vmdq_san_mac)(struct ngbe_hw *, u32);
	s32 (*clear_vmdq)(struct ngbe_hw *, u32, u32);
	s32 (*init_rx_addrs)(struct ngbe_hw *);
	s32 (*update_uc_addr_list)(struct ngbe_hw *, u8 *, u32,
				   ngbe_mc_addr_itr);
	s32 (*update_mc_addr_list)(struct ngbe_hw *, u8 *, u32,
				   ngbe_mc_addr_itr, bool clear);
	s32 (*enable_mc)(struct ngbe_hw *);
	s32 (*disable_mc)(struct ngbe_hw *);
	s32 (*clear_vfta)(struct ngbe_hw *);
	s32 (*set_vfta)(struct ngbe_hw *, u32, u32, bool, bool);
	s32 (*set_vlvf)(struct ngbe_hw *, u32, u32, bool, u32 *, u32,
			bool);
	s32 (*init_uta_tables)(struct ngbe_hw *);
	void (*set_mac_anti_spoofing)(struct ngbe_hw *, bool, int);
	void (*set_vlan_anti_spoofing)(struct ngbe_hw *, bool, int);
	s32 (*update_xcast_mode)(struct ngbe_hw *, int);
	s32 (*set_rlpml)(struct ngbe_hw *, u16);

	/* Flow Control */
	s32 (*fc_enable)(struct ngbe_hw *);
	s32 (*setup_fc)(struct ngbe_hw *);
	void (*fc_autoneg)(struct ngbe_hw *);

	/* Manageability interface */
	s32 (*set_fw_drv_ver)(struct ngbe_hw *, u8, u8, u8, u8, u16,
			      const char *);
	s32 (*get_thermal_sensor_data)(struct ngbe_hw *);
	s32 (*init_thermal_sensor_thresh)(struct ngbe_hw *hw);
	s32 (*check_overtemp)(struct ngbe_hw *hw);
	void (*get_rtrup2tc)(struct ngbe_hw *hw, u8 *map);
	void (*disable_rx)(struct ngbe_hw *hw);
	void (*enable_rx)(struct ngbe_hw *hw);
	void (*set_source_address_pruning)(struct ngbe_hw *, bool,
					   unsigned int);
	void (*set_ethertype_anti_spoofing)(struct ngbe_hw *, bool, int);
	s32 (*dmac_update_tcs)(struct ngbe_hw *hw);
	s32 (*dmac_config_tcs)(struct ngbe_hw *hw);
	s32 (*dmac_config)(struct ngbe_hw *hw);
	s32 (*setup_eee)(struct ngbe_hw *hw, bool enable_eee);
	s32 (*read_iosf_sb_reg)(struct ngbe_hw *, u32, u32, u32 *);
	s32 (*write_iosf_sb_reg)(struct ngbe_hw *, u32, u32, u32);
	void (*disable_mdd)(struct ngbe_hw *hw);
	void (*enable_mdd)(struct ngbe_hw *hw);
	void (*mdd_event)(struct ngbe_hw *hw, u32 *vf_bitmap);
	void (*restore_mdd_vf)(struct ngbe_hw *hw, u32 vf);
	bool (*fw_recovery_mode)(struct ngbe_hw *hw);

	enum ngbe_mac_type type;
	enum ngbe_link_type link_type;
	u8 addr[ETH_ADDR_LEN];
	u8 perm_addr[ETH_ADDR_LEN];
	u8 san_addr[ETH_ADDR_LEN];
	/* prefix for World Wide Node Name (WWNN) */
	u16 wwnn_prefix;
	/* prefix for World Wide Port Name (WWPN) */
	u16 wwpn_prefix;
#define NGBE_MAX_MTA			128
	u32 mta_shadow[NGBE_MAX_MTA];
	s32 mc_filter_type;
	u32 mcft_size;
	u32 vft_size;
	u32 num_rar_entries;
	u32 rar_highwater;
	u32 rx_pb_size;
	u32 max_tx_queues;
	u32 max_rx_queues;
	u64 orig_autoc;  /* cached value of AUTOC */
	u8  san_mac_rar_index;
	bool get_link_status;
	//u16 max_msix_vectors;
	//bool arc_subsystem_valid;
	bool orig_link_settings_stored;
	bool autotry_restart;
	u8 flags;
	struct ngbe_thermal_sensor_data  thermal_sensor_data;
	bool thermal_sensor_enabled;
	struct ngbe_dmac_config dmac_cfg;
	bool set_lben;
	u32  max_link_up_time;

	u32 default_speeds;
	bool autoneg;
};

struct ngbe_phy_info {
	s32 (*identify)(struct ngbe_hw *);
	s32 (*identify_sfp)(struct ngbe_hw *);
	s32 (*init_hw)(struct ngbe_hw *);
	s32 (*reset_hw)(struct ngbe_hw *);
	s32 (*read_reg)(struct ngbe_hw *, u32, u32, u16 *);
	s32 (*write_reg)(struct ngbe_hw *, u32, u32, u16);
	s32 (*read_reg_unlocked)(struct ngbe_hw *, u32, u32, u16 *);
	s32 (*write_reg_unlocked)(struct ngbe_hw *, u32, u32, u16);
	s32 (*setup_link)(struct ngbe_hw *, u32, bool);
	s32 (*check_link)(struct ngbe_hw *, u32 *, bool *);
	s32 (*get_firmware_version)(struct ngbe_hw *, u32 *);
	s32 (*read_i2c_byte)(struct ngbe_hw *, u8, u8, u8 *);
	s32 (*write_i2c_byte)(struct ngbe_hw *, u8, u8, u8);
	s32 (*read_i2c_sff8472)(struct ngbe_hw *, u8 , u8 *);
	s32 (*read_i2c_eeprom)(struct ngbe_hw *, u8 , u8 *);
	s32 (*write_i2c_eeprom)(struct ngbe_hw *, u8, u8);
	void (*i2c_bus_clear)(struct ngbe_hw *);
	s32 (*check_overtemp)(struct ngbe_hw *);
	s32 (*set_phy_power)(struct ngbe_hw *, bool on);
	s32 (*signal_set)(struct ngbe_hw *hw);
	s32 (*led_oem_chk)(struct ngbe_hw *hw, u32 *data);
	s32 (*enter_lplu)(struct ngbe_hw *);
	s32 (*handle_lasi)(struct ngbe_hw *hw);
	s32 (*read_i2c_byte_unlocked)(struct ngbe_hw *, u8 offset, u8 addr,
				      u8 *value);
	s32 (*write_i2c_byte_unlocked)(struct ngbe_hw *, u8 offset, u8 addr,
				       u8 value);

	enum ngbe_media_type media_type;
	enum ngbe_phy_type type;
	u32 addr;
	u32 id;
	enum ngbe_sfp_type sfp_type;
	bool sfp_setup_needed;
	u32 revision;
	u32 phy_semaphore_mask;
	bool reset_disable;
	u32 autoneg_advertised;
	u32 speeds_supported;
	u32 eee_speeds_supported;
	u32 eee_speeds_advertised;
	enum ngbe_smart_speed smart_speed;
	bool smart_speed_active;
	bool multispeed_fiber;
	bool reset_if_overtemp;
	bool qsfp_shared_i2c_bus;
	u32 nw_mng_if_sel;

	u32 link_mode; //fixme
};

struct ngbe_mbx_stats {
	u32 msgs_tx;
	u32 msgs_rx;

	u32 acks;
	u32 reqs;
	u32 rsts;
};

struct ngbe_mbx_info {
	void (*init_params)(struct ngbe_hw *hw);
	s32  (*read)(struct ngbe_hw *, u32 *, u16,  u16);
	s32  (*write)(struct ngbe_hw *, u32 *, u16, u16);
	s32  (*read_posted)(struct ngbe_hw *, u32 *, u16,  u16);
	s32  (*write_posted)(struct ngbe_hw *, u32 *, u16, u16);
	s32  (*check_for_msg)(struct ngbe_hw *, u16);
	s32  (*check_for_ack)(struct ngbe_hw *, u16);
	s32  (*check_for_rst)(struct ngbe_hw *, u16);

	struct ngbe_mbx_stats stats;
	u32 timeout;
	u32 usec_delay;
	u32 v2p_mailbox;
	u16 size;
};

enum ngbe_isb_idx {
	NGBE_ISB_HEADER,
	NGBE_ISB_MISC,
	NGBE_ISB_VEC0,
	NGBE_ISB_VEC1,
	NGBE_ISB_MAX
};

struct ngbe_hw {
	void IOMEM *hw_addr;
	void *back;
	struct ngbe_mac_info mac;
	struct ngbe_addr_filter_info addr_ctrl;
	struct ngbe_fc_info fc;
	struct ngbe_phy_info phy;
	struct ngbe_link_info link;
	struct ngbe_rom_info rom;
	struct ngbe_flash_info flash;
	struct ngbe_bus_info bus;
	struct ngbe_mbx_info mbx;
	//const u32 *mvals;
	u16 device_id;
	u16 vendor_id;
	u16 sub_device_id;
	u16 sub_system_id;
	u16 sub_vendor_id;
	u32 eeprom_id;
	u8 revision_id;
	bool adapter_stopped;
	int api_version;
	bool force_full_reset;
	bool allow_unsupported_sfp;
	bool wol_enabled;
	bool need_crosstalk_fix;

	u32 b4_buf[16];
	uint64_t isb_dma;
	void IOMEM *isb_mem;
	u16 nb_rx_queues;
	u16 nb_tx_queues;
#if 1 /* test by wangjian <begin> */
	u32 mode;
	enum ngbe_link_status {
		NGBE_LINK_STATUS_NONE = 0,
		NGBE_LINK_STATUS_KX,
		NGBE_LINK_STATUS_KX4
	} link_status;
	enum ngbe_reset_type {
		NGBE_LAN_RESET = 0,
		NGBE_SW_RESET,
		NGBE_GLOBAL_RESET
	} reset_type;

	/* For vector PMD */
	bool rx_bulk_alloc_allowed;
	bool rx_vec_allowed;
	bool tx_vec_allowed;
#endif /* test by wangjian <end> */
	u32 q_rx_regs[8*4];
	u32 q_tx_regs[8*4];
	u32 gphy_efuse[2];
	bool offset_loaded;
	bool rx_loaded;
	bool is_pf;
	bool gpio_ctl;
	u32 led_conf;
	u32 an_errcnt;
	bool an_retry;
	bool init_phy;
	struct {
		u64 rx_qp_packets;
		u64 tx_qp_packets;
		u64 rx_qp_bytes;
		u64 tx_qp_bytes;
		u64 rx_qp_mc_packets;
		u64 tx_qp_mc_packets;
		u64 rx_qp_bc_packets;
		u64 tx_qp_bc_packets;
	} qp_last[NGBE_MAX_QP];
};

#ifndef NGBE_HIGH_LV_EFFECT_GPHY_RGMII
#define NGBE_HIGH_LV_EFFECT_GPHY_RGMII 1
#endif

#ifndef NGBE_HIGH_LV_EFFECT_88E1512_SFP
#define NGBE_HIGH_LV_EFFECT_88E1512_SFP 1
#endif

#include "ngbe_regs.h"
#include "ngbe_dummy.h"

#endif /* _NGBE_TYPE_H_ */
