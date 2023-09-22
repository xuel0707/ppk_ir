/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _TXGBE_DCB_H_
#define _TXGBE_DCB_H_

#include "txgbe_type.h"

/* DCB defines */
/* DCB credit calculation defines */
#define TXGBE_DCB_CREDIT_QUANTUM	64
#define TXGBE_DCB_MAX_CREDIT_REFILL	200   /* 200 * 64B = 12800B */
#define TXGBE_DCB_MAX_TSO_SIZE		(32 * 1024) /* Max TSO pkt size in DCB*/
#define TXGBE_DCB_MAX_CREDIT		(2 * TXGBE_DCB_MAX_CREDIT_REFILL)

/* 513 for 32KB TSO packet */
#define TXGBE_DCB_MIN_TSO_CREDIT	\
	((TXGBE_DCB_MAX_TSO_SIZE / TXGBE_DCB_CREDIT_QUANTUM) + 1)

#define TXGBE_DCB_TX_CONFIG		0
#define TXGBE_DCB_RX_CONFIG		1

/* DCB capability defines */
#define TXGBE_DCB_PG_SUPPORT	0x00000001
#define TXGBE_DCB_PFC_SUPPORT	0x00000002
#define TXGBE_DCB_BCN_SUPPORT	0x00000004
#define TXGBE_DCB_UP2TC_SUPPORT	0x00000008
#define TXGBE_DCB_GSP_SUPPORT	0x00000010

struct txgbe_dcb_support {
	u32 capabilities; /* DCB capabilities */

	/* Each bit represents a number of TCs configurable in the hw.
	 * If 8 traffic classes can be configured, the value is 0x80. */
	u8 traffic_classes;
	u8 pfc_traffic_classes;
};

enum txgbe_dcb_tsa {
	txgbe_dcb_tsa_ets = 0,
	txgbe_dcb_tsa_group_strict_cee,
	txgbe_dcb_tsa_strict
};

/* Traffic class bandwidth allocation per direction */
struct txgbe_dcb_tc_path {
	u8 bwg_id; /* Bandwidth Group (BWG) ID */
	u8 bwg_percent; /* % of BWG's bandwidth */
	u8 link_percent; /* % of link bandwidth */
	u8 up_to_tc_bitmap; /* User Priority to Traffic Class mapping */
	u16 data_credits_refill; /* Credit refill amount in 64B granularity */
	u16 data_credits_max; /* Max credits for a configured packet buffer
			       * in 64B granularity.*/
	enum txgbe_dcb_tsa tsa; /* Link or Group Strict Priority */
};

enum txgbe_dcb_pfc {
	txgbe_dcb_pfc_disabled = 0,
	txgbe_dcb_pfc_enabled,
	txgbe_dcb_pfc_enabled_txonly,
	txgbe_dcb_pfc_enabled_rxonly
};

/* Traffic class configuration */
struct txgbe_dcb_tc_config {
	struct txgbe_dcb_tc_path path[2]; /* One each for Tx/Rx */
	enum txgbe_dcb_pfc pfc; /* Class based flow control setting */

	u16 desc_credits_max; /* For Tx Descriptor arbitration */
	u8 tc; /* Traffic class (TC) */
};

enum txgbe_dcb_pba {
	/* PBA[0-7] each use 64KB FIFO */
	txgbe_dcb_pba_equal = PBA_STRATEGY_EQUAL,
	/* PBA[0-3] each use 80KB, PBA[4-7] each use 48KB */
	txgbe_dcb_pba_80_48 = PBA_STRATEGY_WEIGHTED
};

struct txgbe_dcb_num_tcs {
	u8 pg_tcs;
	u8 pfc_tcs;
};

struct txgbe_dcb_config {
	struct txgbe_dcb_tc_config tc_config[TXGBE_DCB_TC_MAX];
	struct txgbe_dcb_support support;
	struct txgbe_dcb_num_tcs num_tcs;
	u8 bw_percentage[TXGBE_DCB_BWG_MAX][2]; /* One each for Tx/Rx */
	bool pfc_mode_enable;
	bool round_robin_enable;

	enum txgbe_dcb_pba rx_pba_cfg;

	u32 dcb_cfg_version; /* Not used...OS-specific? */
	u32 link_speed; /* For bandwidth allocation validation purpose */
	bool vt_mode;
};

int txgbe_dcb_pfc_enable(struct txgbe_hw *hw, u8 tc_num);
/* DCB rule checking */
s32 txgbe_dcb_check_config_cee(struct txgbe_dcb_config *);

/* DCB credits calculation */
s32 txgbe_dcb_calculate_tc_credits(u8 *, u16 *, u16 *, int);
s32 txgbe_dcb_calculate_tc_credits_cee(struct txgbe_hw *,
				       struct txgbe_dcb_config *, u32, u8);

/* DCB PFC */
s32 txgbe_dcb_config_pfc(struct txgbe_hw *, u8, u8 *);
s32 txgbe_dcb_config_pfc_cee(struct txgbe_hw *, struct txgbe_dcb_config *);

/* DCB stats */
s32 txgbe_dcb_config_tc_stats(struct txgbe_hw *);
s32 txgbe_dcb_get_tc_stats(struct txgbe_hw *, struct txgbe_hw_stats *, u8);
s32 txgbe_dcb_get_pfc_stats(struct txgbe_hw *, struct txgbe_hw_stats *, u8);

/* DCB config arbiters */
s32 txgbe_dcb_config_tx_desc_arbiter_cee(struct txgbe_hw *,
					 struct txgbe_dcb_config *);
s32 txgbe_dcb_config_tx_data_arbiter_cee(struct txgbe_hw *,
					 struct txgbe_dcb_config *);
s32 txgbe_dcb_config_rx_arbiter_cee(struct txgbe_hw *,
				    struct txgbe_dcb_config *);

/* DCB unpack routines */
void txgbe_dcb_unpack_pfc_cee(struct txgbe_dcb_config *, u8 *, u8 *);
void txgbe_dcb_unpack_refill_cee(struct txgbe_dcb_config *, int, u16 *);
void txgbe_dcb_unpack_max_cee(struct txgbe_dcb_config *, u16 *);
void txgbe_dcb_unpack_bwgid_cee(struct txgbe_dcb_config *, int, u8 *);
void txgbe_dcb_unpack_tsa_cee(struct txgbe_dcb_config *, int, u8 *);
void txgbe_dcb_unpack_map_cee(struct txgbe_dcb_config *, int, u8 *);
u8 txgbe_dcb_get_tc_from_up(struct txgbe_dcb_config *, int, u8);

/* DCB initialization */
s32 txgbe_dcb_hw_config(struct txgbe_hw *, u16 *, u16 *, u8 *, u8 *, u8 *);
s32 txgbe_dcb_hw_config_cee(struct txgbe_hw *, struct txgbe_dcb_config *);


#include "txgbe_dcb_hw.h"

#endif /* _TXGBE_DCB_H_ */
