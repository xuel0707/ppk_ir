/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015-2020
 */

#include "txgbe_type.h"

#include "txgbe_dcb.h"

/**
 * txgbe_dcb_get_tc_stats_raptor - Returns status for each traffic class
 * @hw: pointer to hardware structure
 * @stats: pointer to statistics structure
 * @tc_count:  Number of elements in bwg_array.
 *
 * This function returns the status data for each of the Traffic Classes in use.
 */
s32 txgbe_dcb_get_tc_stats_raptor(struct txgbe_hw *hw,
				 struct txgbe_hw_stats *stats,
				 u8 tc_count)
{
	int tc;

	UNREFERENCED_PARAMETER(hw);

	DEBUGFUNC("dcb_get_tc_stats");

	if (tc_count > TXGBE_DCB_TC_MAX)
		return TXGBE_ERR_PARAM;

	/* Statistics pertaining to each traffic class */
	for (tc = 0; tc < tc_count; tc++) {
		/* Transmitted Packets */
		stats->up[tc].tx_up_packets += 0;
		/* Transmitted Bytes (read low first to prevent missed carry) */
		stats->up[tc].tx_up_bytes += 0;
		/* Received Packets */
		stats->up[tc].rx_up_packets += 0;
		/* Received Bytes (read low first to prevent missed carry) */
		stats->up[tc].rx_up_bytes += 0;
		/* Received Dropped Packet */
		stats->up[tc].rx_up_drop_packets += 0;
	}

	return 0;
}

/**
 * txgbe_dcb_get_pfc_stats_raptor - Return CBFC status data
 * @hw: pointer to hardware structure
 * @stats: pointer to statistics structure
 * @tc_count:  Number of elements in bwg_array.
 *
 * This function returns the CBFC status data for each of the Traffic Classes.
 */
s32 txgbe_dcb_get_pfc_stats_raptor(struct txgbe_hw *hw,
				  struct txgbe_hw_stats *stats,
				  u8 tc_count)
{
	int tc;

	DEBUGFUNC("dcb_get_pfc_stats");

	if (tc_count > TXGBE_DCB_TC_MAX)
		return TXGBE_ERR_PARAM;

	for (tc = 0; tc < tc_count; tc++) {
		/* Priority XOFF Transmitted */
		stats->up[tc].tx_up_xoff_packets +=
					rd32(hw, TXGBE_PBTXUPXOFF(tc));
		/* Priority XOFF Received */
		stats->up[tc].rx_up_xoff_packets +=
					rd32(hw, TXGBE_PBRXUPXOFF(tc));
	}

	return 0;
}

/**
 * txgbe_dcb_config_rx_arbiter_raptor - Config Rx Data arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @tsa: transmission selection algorithm indexed by traffic class
 * @map: priority to tc assignments indexed by priority
 *
 * Configure Rx Packet Arbiter and credits for each traffic class.
 */
s32 txgbe_dcb_config_rx_arbiter_raptor(struct txgbe_hw *hw, u16 *refill,
				      u16 *max, u8 *bwg_id, u8 *tsa,
				      u8 *map)
{
	u32 reg = 0;
	u32 credit_refill = 0;
	u32 credit_max = 0;
	u8  i = 0;

	/*
	 * Disable the arbiter before changing parameters
	 * (always enable recycle mode; WSP)
	 */
	reg = TXGBE_ARBRXCTL_RRM | TXGBE_ARBRXCTL_WSP |
	      TXGBE_ARBRXCTL_DIA;
	wr32(hw, TXGBE_ARBRXCTL, reg);

	/*
	 * map all UPs to TCs. up_to_tc_bitmap for each TC has corresponding
	 * bits sets for the UPs that needs to be mappped to that TC.
	 * e.g if priorities 6 and 7 are to be mapped to a TC then the
	 * up_to_tc_bitmap value for that TC will be 11000000 in binary.
	 */
	reg = 0;
	for (i = 0; i < TXGBE_DCB_UP_MAX; i++)
		reg |= (map[i] << (i * TXGBE_RPUP2TC_UP_SHIFT));

	wr32(hw, TXGBE_RPUP2TC, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		credit_refill = refill[i];
		credit_max = max[i];
		reg = TXGBE_QARBRXCFG_CRQ(credit_refill) |
		      TXGBE_QARBRXCFG_MCL(credit_max) |
		      TXGBE_QARBRXCFG_BWG(bwg_id[i]);

		if (tsa[i] == txgbe_dcb_tsa_strict)
			reg |= TXGBE_QARBRXCFG_LSP;

		wr32(hw, TXGBE_QARBRXCFG(i), reg);
	}

	/*
	 * Configure Rx packet plane (recycle mode; WSP) and
	 * enable arbiter
	 */
	reg = TXGBE_ARBRXCTL_RRM | TXGBE_ARBRXCTL_WSP;
	wr32(hw, TXGBE_ARBRXCTL, reg);

	return 0;
}

/**
 * txgbe_dcb_config_tx_desc_arbiter_raptor - Config Tx Desc. arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @tsa: transmission selection algorithm indexed by traffic class
 *
 * Configure Tx Descriptor Arbiter and credits for each traffic class.
 */
s32 txgbe_dcb_config_tx_desc_arbiter_raptor(struct txgbe_hw *hw, u16 *refill,
					   u16 *max, u8 *bwg_id, u8 *tsa)
{
	u32 reg, max_credits;
	u8  i;

	/* Clear the per-Tx queue credits; we use per-TC instead */
	for (i = 0; i < 128; i++) {
		wr32(hw, TXGBE_QARBTXCRED(i), 0);
	}

	/* Configure traffic class credits and priority */
	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		max_credits = max[i];
		reg = TXGBE_QARBTXCFG_MCL(max_credits) |
		      TXGBE_QARBTXCFG_CRQ(refill[i]) |
		      TXGBE_QARBTXCFG_BWG(bwg_id[i]);

		if (tsa[i] == txgbe_dcb_tsa_group_strict_cee)
			reg |= TXGBE_QARBTXCFG_GSP;

		if (tsa[i] == txgbe_dcb_tsa_strict)
			reg |= TXGBE_QARBTXCFG_LSP;

		wr32(hw, TXGBE_QARBTXCFG(i), reg);
	}

	/*
	 * Configure Tx descriptor plane (recycle mode; WSP) and
	 * enable arbiter
	 */
	reg = TXGBE_ARBTXCTL_WSP | TXGBE_ARBTXCTL_RRM;
	wr32(hw, TXGBE_ARBTXCTL, reg);

	return 0;
}

/**
 * txgbe_dcb_config_tx_data_arbiter_raptor - Config Tx Data arbiter
 * @hw: pointer to hardware structure
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @tsa: transmission selection algorithm indexed by traffic class
 * @map: priority to tc assignments indexed by priority
 *
 * Configure Tx Packet Arbiter and credits for each traffic class.
 */
s32 txgbe_dcb_config_tx_data_arbiter_raptor(struct txgbe_hw *hw, u16 *refill,
					   u16 *max, u8 *bwg_id, u8 *tsa,
					   u8 *map)
{
	u32 reg;
	u8 i;

	/*
	 * Disable the arbiter before changing parameters
	 * (always enable recycle mode; SP; arb delay)
	 */
	reg = TXGBE_PARBTXCTL_SP |
	      TXGBE_PARBTXCTL_RECYC |
	      TXGBE_PARBTXCTL_DA;
	wr32(hw, TXGBE_PARBTXCTL, reg);

	/*
	 * map all UPs to TCs. up_to_tc_bitmap for each TC has corresponding
	 * bits sets for the UPs that needs to be mappped to that TC.
	 * e.g if priorities 6 and 7 are to be mapped to a TC then the
	 * up_to_tc_bitmap value for that TC will be 11000000 in binary.
	 */
	reg = 0;
	for (i = 0; i < TXGBE_DCB_UP_MAX; i++)
		reg |= TXGBE_DCBUP2TC_MAP(i, map[i]);

	wr32(hw, TXGBE_PBRXUP2TC, reg);

	/* Configure traffic class credits and priority */
	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		reg = TXGBE_PARBTXCFG_CRQ(refill[i]) |
		      TXGBE_PARBTXCFG_MCL(max[i]) |
		      TXGBE_PARBTXCFG_BWG(bwg_id[i]);

		if (tsa[i] == txgbe_dcb_tsa_group_strict_cee)
			reg |= TXGBE_PARBTXCFG_GSP;

		if (tsa[i] == txgbe_dcb_tsa_strict)
			reg |= TXGBE_PARBTXCFG_LSP;

		wr32(hw, TXGBE_PARBTXCFG(i), reg);
	}

	/*
	 * Configure Tx packet plane (recycle mode; SP; arb delay) and
	 * enable arbiter
	 */
	reg = TXGBE_PARBTXCTL_SP | TXGBE_PARBTXCTL_RECYC;
	wr32(hw, TXGBE_PARBTXCTL, reg);

	return 0;
}

/**
 * txgbe_dcb_config_pfc_raptor - Configure priority flow control
 * @hw: pointer to hardware structure
 * @pfc_en: enabled pfc bitmask
 * @map: priority to tc assignments indexed by priority
 *
 * Configure Priority Flow Control (PFC) for each traffic class.
 */
s32 txgbe_dcb_config_pfc_raptor(struct txgbe_hw *hw, u8 pfc_en, u8 *map)
{
	u32 i, j, fcrtl, reg;
	u8 max_tc = 0;

	/* Enable Transmit Priority Flow Control */
	wr32(hw, TXGBE_TXFCCFG, TXGBE_TXFCCFG_PFC);

	/* Enable Receive Priority Flow Control */
	wr32m(hw, TXGBE_RXFCCFG, TXGBE_RXFCCFG_PFC,
		pfc_en ? TXGBE_RXFCCFG_PFC : 0);

	for (i = 0; i < TXGBE_DCB_UP_MAX; i++) {
		if (map[i] > max_tc)
			max_tc = map[i];
	}

	/* Configure PFC Tx thresholds per TC */
	for (i = 0; i <= max_tc; i++) {
		int enabled = 0;

		for (j = 0; j < TXGBE_DCB_UP_MAX; j++) {
			if ((map[j] == i) && (pfc_en & (1 << j))) {
				enabled = 1;
				break;
			}
		}

		if (enabled) {
			reg = TXGBE_FCWTRHI_TH(hw->fc.high_water[i]) |
			      TXGBE_FCWTRHI_XOFF;
			fcrtl = TXGBE_FCWTRLO_TH(hw->fc.low_water[i]) |
				TXGBE_FCWTRLO_XON;
			wr32(hw, TXGBE_FCWTRLO(i), fcrtl);
		} else {
			/*
			 * In order to prevent Tx hangs when the internal Tx
			 * switch is enabled we must set the high water mark
			 * to the Rx packet buffer size - 24KB.  This allows
			 * the Tx switch to function even under heavy Rx
			 * workloads.
			 */
			reg = rd32(hw, TXGBE_PBRXSIZE(i)) - 24576;
			wr32(hw, TXGBE_FCWTRLO(i), 0);
		}

		wr32(hw, TXGBE_FCWTRHI(i), reg);
	}

	for (; i < TXGBE_DCB_TC_MAX; i++) {
		wr32(hw, TXGBE_FCWTRLO(i), 0);
		wr32(hw, TXGBE_FCWTRHI(i), 0);
	}

	/* Configure pause time (2 TCs per register) */
	reg = hw->fc.pause_time | (hw->fc.pause_time << 16);
	for (i = 0; i < (TXGBE_DCB_TC_MAX / 2); i++)
		wr32(hw, TXGBE_FCXOFFTM(i), reg);

	/* Configure flow control refresh threshold value */
	wr32(hw, TXGBE_RXFCRFSH, hw->fc.pause_time / 2);

	return 0;
}

/**
 * txgbe_dcb_config_tc_stats_raptor - Config traffic class statistics
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure queue statistics registers, all queues belonging to same traffic
 * class uses a single set of queue statistics counters.
 */
s32 txgbe_dcb_config_tc_stats_raptor(struct txgbe_hw *hw,
				    struct txgbe_dcb_config *dcb_config)
{
	u8 tc_count = 8;
	bool vt_mode = false;

	UNREFERENCED_PARAMETER(hw);

	if (dcb_config != NULL) {
		tc_count = dcb_config->num_tcs.pg_tcs;
		vt_mode = dcb_config->vt_mode;
	}

	if (!((tc_count == 8 && vt_mode == false) || tc_count == 4))
		return TXGBE_ERR_PARAM;

	return 0;
}

/**
 * txgbe_dcb_config_raptor - Configure general DCB parameters
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure general DCB parameters.
 */
s32 txgbe_dcb_config_raptor(struct txgbe_hw *hw,
			   struct txgbe_dcb_config *dcb_config)
{
	u32 reg;
	u32 q;

	/* Disable the Tx desc arbiter so that MTQC can be changed */
	reg = rd32(hw, TXGBE_ARBTXCTL);
	reg |= TXGBE_ARBTXCTL_DIA;
	wr32(hw, TXGBE_ARBTXCTL, reg);

	reg = rd32(hw, TXGBE_PORTCTL);
	reg &= ~TXGBE_PORTCTL_NUMTC_MASK;
	reg &= ~TXGBE_PORTCTL_NUMVT_MASK;
	reg |= TXGBE_PORTCTL_DCB;
	if (dcb_config->num_tcs.pg_tcs == 8) {
		/* Enable DCB for Rx with 8 TCs */
		reg |= TXGBE_PORTCTL_NUMTC_8;
		if (dcb_config->vt_mode)
			reg |= TXGBE_PORTCTL_NUMVT_16;
	}
	if (dcb_config->num_tcs.pg_tcs == 4) {
		/* We support both VT-on and VT-off with 4 TCs. */
		reg |= TXGBE_PORTCTL_NUMTC_4;
		if (dcb_config->vt_mode)
			reg |= TXGBE_PORTCTL_NUMVT_32;
	}
	wr32(hw, TXGBE_PORTCTL, reg);

	/* Disable drop for all queues */
	for (q = 0; q < 128; q++) {
		u32 val = 1 << (q % 32);
		wr32m(hw, TXGBE_QPRXDROP(q / 32), val, val);
	}

	/* Enable the Tx desc arbiter */
	reg = rd32(hw, TXGBE_ARBTXCTL);
	reg &= ~TXGBE_ARBTXCTL_DIA;
	wr32(hw, TXGBE_ARBTXCTL, reg);

	return 0;
}

/**
 * txgbe_dcb_hw_config_raptor - Configure and enable DCB
 * @hw: pointer to hardware structure
 * @link_speed: unused
 * @refill: refill credits index by traffic class
 * @max: max credits index by traffic class
 * @bwg_id: bandwidth grouping indexed by traffic class
 * @tsa: transmission selection algorithm indexed by traffic class
 * @map: priority to tc assignments indexed by priority
 *
 * Configure dcb settings and enable dcb mode.
 */
s32 txgbe_dcb_hw_config_raptor(struct txgbe_hw *hw, int link_speed,
			      u16 *refill, u16 *max, u8 *bwg_id, u8 *tsa,
			      u8 *map)
{
	UNREFERENCED_PARAMETER(link_speed);

	txgbe_dcb_config_rx_arbiter_raptor(hw, refill, max, bwg_id, tsa,
					  map);
	txgbe_dcb_config_tx_desc_arbiter_raptor(hw, refill, max, bwg_id,
					       tsa);
	txgbe_dcb_config_tx_data_arbiter_raptor(hw, refill, max, bwg_id,
					       tsa, map);

	return 0;
}

