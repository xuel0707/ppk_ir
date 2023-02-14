/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015-2020
 */

#include "txgbe_type.h"
#include "txgbe_hw.h"
#include "txgbe_dcb.h"
#include "txgbe_dcb_hw.h"

/**
 *  txgbe_pfc_enable - Enable flow control
 *  @hw: pointer to hardware structure
 *  @tc_num: traffic class number
 *  Enable flow control according to the current settings.
 */
int
txgbe_dcb_pfc_enable(struct txgbe_hw *hw, uint8_t tc_num)
{
	int ret_val = 0;
	uint32_t mflcn_reg, fccfg_reg;
	uint32_t pause_time;
	uint32_t fcrtl, fcrth;
	uint8_t i;
	uint8_t nb_rx_en;

	/* Validate the water mark configuration */
	if (!hw->fc.pause_time) {
		ret_val = TXGBE_ERR_INVALID_LINK_SETTINGS;
		goto out;
	}

	/* Low water mark of zero causes XOFF floods */
	if (hw->fc.current_mode & txgbe_fc_tx_pause) {
		 /* High/Low water can not be 0 */
		if ((!hw->fc.high_water[tc_num]) ||
		    (!hw->fc.low_water[tc_num])) {
			PMD_INIT_LOG(ERR, "Invalid water mark configuration");
			ret_val = TXGBE_ERR_INVALID_LINK_SETTINGS;
			goto out;
		}

		if (hw->fc.low_water[tc_num] >= hw->fc.high_water[tc_num]) {
			PMD_INIT_LOG(ERR, "Invalid water mark configuration");
			ret_val = TXGBE_ERR_INVALID_LINK_SETTINGS;
			goto out;
		}
	}
	/* Negotiate the fc mode to use */
	txgbe_fc_autoneg(hw);

	/* Disable any previous flow control settings */
	mflcn_reg = rd32(hw, TXGBE_RXFCCFG);
	mflcn_reg &= ~(TXGBE_RXFCCFG_FC | TXGBE_RXFCCFG_PFC);

	fccfg_reg = rd32(hw, TXGBE_TXFCCFG);
	fccfg_reg &= ~(TXGBE_TXFCCFG_FC | TXGBE_TXFCCFG_PFC);

	switch (hw->fc.current_mode) {
	case txgbe_fc_none:
		/*
		 * If the count of enabled RX Priority Flow control > 1,
		 * and the TX pause can not be disabled
		 */
		nb_rx_en = 0;
		for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
			uint32_t reg = rd32(hw, TXGBE_FCWTRHI(i));
			if (reg & TXGBE_FCWTRHI_XOFF)
				nb_rx_en++;
		}
		if (nb_rx_en > 1)
			fccfg_reg |= TXGBE_TXFCCFG_PFC;
		break;
	case txgbe_fc_rx_pause:
		/*
		 * Rx Flow control is enabled and Tx Flow control is
		 * disabled by software override. Since there really
		 * isn't a way to advertise that we are capable of RX
		 * Pause ONLY, we will advertise that we support both
		 * symmetric and asymmetric Rx PAUSE.  Later, we will
		 * disable the adapter's ability to send PAUSE frames.
		 */
		mflcn_reg |= TXGBE_RXFCCFG_PFC;
		/*
		 * If the count of enabled RX Priority Flow control > 1,
		 * and the TX pause can not be disabled
		 */
		nb_rx_en = 0;
		for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
			uint32_t reg = rd32(hw, TXGBE_FCWTRHI(i));
			if (reg & TXGBE_FCWTRHI_XOFF)
				nb_rx_en++;
		}
		if (nb_rx_en > 1)
			fccfg_reg |= TXGBE_TXFCCFG_PFC;
		break;
	case txgbe_fc_tx_pause:
		/*
		 * Tx Flow control is enabled, and Rx Flow control is
		 * disabled by software override.
		 */
		fccfg_reg |= TXGBE_TXFCCFG_PFC;
		break;
	case txgbe_fc_full:
		/* Flow control (both Rx and Tx) is enabled by SW override. */
		mflcn_reg |= TXGBE_RXFCCFG_PFC;
		fccfg_reg |= TXGBE_TXFCCFG_PFC;
		break;
	default:
		PMD_DRV_LOG(DEBUG, "Flow control param set incorrectly");
		ret_val = TXGBE_ERR_CONFIG;
		goto out;
	}

	/* Set 802.3x based flow control settings. */
	wr32(hw, TXGBE_RXFCCFG, mflcn_reg);
	wr32(hw, TXGBE_TXFCCFG, fccfg_reg);

	/* Set up and enable Rx high/low water mark thresholds, enable XON. */
	if ((hw->fc.current_mode & txgbe_fc_tx_pause) &&
		hw->fc.high_water[tc_num]) {
		fcrtl = TXGBE_FCWTRLO_TH(hw->fc.low_water[tc_num]) |
			TXGBE_FCWTRLO_XON;
		fcrth = TXGBE_FCWTRHI_TH(hw->fc.high_water[tc_num]) |
			TXGBE_FCWTRHI_XOFF;
	} else {
		/*
		 * In order to prevent Tx hangs when the internal Tx
		 * switch is enabled we must set the high water mark
		 * to the maximum FCRTH value.  This allows the Tx
		 * switch to function even under heavy Rx workloads.
		 */
		fcrtl = 0;
		fcrth = rd32(hw, TXGBE_PBRXSIZE(tc_num)) - 32;
	}
	wr32(hw, TXGBE_FCWTRLO(tc_num), fcrtl);
	wr32(hw, TXGBE_FCWTRHI(tc_num), fcrth);

	/* Configure pause time (2 TCs per register) */
	pause_time = TXGBE_RXFCFSH_TIME(hw->fc.pause_time);
	for (i = 0; i < (TXGBE_DCB_TC_MAX / 2); i++)
		wr32(hw, TXGBE_FCXOFFTM(i), pause_time * 0x00010001);

	/* Configure flow control refresh threshold value */
	wr32(hw, TXGBE_RXFCRFSH, pause_time / 2);

out:
	return ret_val;
}

/**
 * txgbe_dcb_calculate_tc_credits - This calculates the ieee traffic class
 * credits from the configured bandwidth percentages. Credits
 * are the smallest unit programmable into the underlying
 * hardware. The IEEE 802.1Qaz specification do not use bandwidth
 * groups so this is much simplified from the CEE case.
 * @bw: bandwidth index by traffic class
 * @refill: refill credits index by traffic class
 * @max: max credits by traffic class
 * @max_frame_size: maximum frame size
 */
s32 txgbe_dcb_calculate_tc_credits(u8 *bw, u16 *refill, u16 *max,
				   int max_frame_size)
{
	int min_percent = 100;
	int min_credit, multiplier;
	int i;

	min_credit = ((max_frame_size / 2) + TXGBE_DCB_CREDIT_QUANTUM - 1) /
			TXGBE_DCB_CREDIT_QUANTUM;

	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		if (bw[i] < min_percent && bw[i])
			min_percent = bw[i];
	}

	multiplier = (min_credit / min_percent) + 1;

	/* Find out the hw credits for each TC */
	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		int val = min(bw[i] * multiplier, TXGBE_DCB_MAX_CREDIT_REFILL);

		if (val < min_credit)
			val = min_credit;
		refill[i] = (u16)val;

		max[i] = bw[i] ? (bw[i]*TXGBE_DCB_MAX_CREDIT)/100 : min_credit;
	}

	return 0;
}

/**
 * txgbe_dcb_calculate_tc_credits_cee - Calculates traffic class credits
 * @hw: pointer to hardware structure
 * @dcb_config: Struct containing DCB settings
 * @max_frame_size: Maximum frame size
 * @direction: Configuring either Tx or Rx
 *
 * This function calculates the credits allocated to each traffic class.
 * It should be called only after the rules are checked by
 * txgbe_dcb_check_config_cee().
 */
s32 txgbe_dcb_calculate_tc_credits_cee(struct txgbe_hw *hw,
				   struct txgbe_dcb_config *dcb_config,
				   u32 max_frame_size, u8 direction)
{
	struct txgbe_dcb_tc_path *p;
	u32 min_multiplier	= 0;
	u16 min_percent		= 100;
	s32 ret_val =		0;
	/* Initialization values default for Tx settings */
	u32 min_credit		= 0;
	u32 credit_refill	= 0;
	u32 credit_max		= 0;
	u16 link_percentage	= 0;
	u8  bw_percent		= 0;
	u8  i;

	UNREFERENCED_PARAMETER(hw);

	if (dcb_config == NULL) {
		ret_val = TXGBE_ERR_CONFIG;
		goto out;
	}

	min_credit = ((max_frame_size / 2) + TXGBE_DCB_CREDIT_QUANTUM - 1) /
		     TXGBE_DCB_CREDIT_QUANTUM;

	/* Find smallest link percentage */
	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		p = &dcb_config->tc_config[i].path[direction];
		bw_percent = dcb_config->bw_percentage[p->bwg_id][direction];
		link_percentage = p->bwg_percent;

		link_percentage = (link_percentage * bw_percent) / 100;

		if (link_percentage && link_percentage < min_percent)
			min_percent = link_percentage;
	}

	/*
	 * The ratio between traffic classes will control the bandwidth
	 * percentages seen on the wire. To calculate this ratio we use
	 * a multiplier. It is required that the refill credits must be
	 * larger than the max frame size so here we find the smallest
	 * multiplier that will allow all bandwidth percentages to be
	 * greater than the max frame size.
	 */
	min_multiplier = (min_credit / min_percent) + 1;

	/* Find out the link percentage for each TC first */
	for (i = 0; i < TXGBE_DCB_TC_MAX; i++) {
		p = &dcb_config->tc_config[i].path[direction];
		bw_percent = dcb_config->bw_percentage[p->bwg_id][direction];

		link_percentage = p->bwg_percent;
		/* Must be careful of integer division for very small nums */
		link_percentage = (link_percentage * bw_percent) / 100;
		if (p->bwg_percent > 0 && link_percentage == 0)
			link_percentage = 1;

		/* Save link_percentage for reference */
		p->link_percent = (u8)link_percentage;

		/* Calculate credit refill ratio using multiplier */
		credit_refill = min(link_percentage * min_multiplier,
				    (u32)TXGBE_DCB_MAX_CREDIT_REFILL);

		/* Refill at least minimum credit */
		if (credit_refill < min_credit)
			credit_refill = min_credit;

		p->data_credits_refill = (u16)credit_refill;

		/* Calculate maximum credit for the TC */
		credit_max = (link_percentage * TXGBE_DCB_MAX_CREDIT) / 100;

		/*
		 * Adjustment based on rule checking, if the percentage
		 * of a TC is too small, the maximum credit may not be
		 * enough to send out a jumbo frame in data plane arbitration.
		 */
		if (credit_max < min_credit)
			credit_max = min_credit;

		if (direction == TXGBE_DCB_TX_CONFIG) {
			dcb_config->tc_config[i].desc_credits_max =
								(u16)credit_max;
		}

		p->data_credits_max = (u16)credit_max;
	}

out:
	return ret_val;
}

/**
 * txgbe_dcb_unpack_pfc_cee - Unpack dcb_config PFC info
 * @cfg: dcb configuration to unpack into hardware consumable fields
 * @map: user priority to traffic class map
 * @pfc_up: u8 to store user priority PFC bitmask
 *
 * This unpacks the dcb configuration PFC info which is stored per
 * traffic class into a 8bit user priority bitmask that can be
 * consumed by hardware routines. The priority to tc map must be
 * updated before calling this routine to use current up-to maps.
 */
void txgbe_dcb_unpack_pfc_cee(struct txgbe_dcb_config *cfg, u8 *map, u8 *pfc_up)
{
	struct txgbe_dcb_tc_config *tc_config = &cfg->tc_config[0];
	int up;

	/*
	 * If the TC for this user priority has PFC enabled then set the
	 * matching bit in 'pfc_up' to reflect that PFC is enabled.
	 */
	for (*pfc_up = 0, up = 0; up < TXGBE_DCB_UP_MAX; up++) {
		if (tc_config[map[up]].pfc != txgbe_dcb_pfc_disabled)
			*pfc_up |= 1 << up;
	}
}

void txgbe_dcb_unpack_refill_cee(struct txgbe_dcb_config *cfg, int direction,
			     u16 *refill)
{
	struct txgbe_dcb_tc_config *tc_config = &cfg->tc_config[0];
	int tc;

	for (tc = 0; tc < TXGBE_DCB_TC_MAX; tc++)
		refill[tc] = tc_config[tc].path[direction].data_credits_refill;
}

void txgbe_dcb_unpack_max_cee(struct txgbe_dcb_config *cfg, u16 *max)
{
	struct txgbe_dcb_tc_config *tc_config = &cfg->tc_config[0];
	int tc;

	for (tc = 0; tc < TXGBE_DCB_TC_MAX; tc++)
		max[tc] = tc_config[tc].desc_credits_max;
}

void txgbe_dcb_unpack_bwgid_cee(struct txgbe_dcb_config *cfg, int direction,
			    u8 *bwgid)
{
	struct txgbe_dcb_tc_config *tc_config = &cfg->tc_config[0];
	int tc;

	for (tc = 0; tc < TXGBE_DCB_TC_MAX; tc++)
		bwgid[tc] = tc_config[tc].path[direction].bwg_id;
}

void txgbe_dcb_unpack_tsa_cee(struct txgbe_dcb_config *cfg, int direction,
			   u8 *tsa)
{
	struct txgbe_dcb_tc_config *tc_config = &cfg->tc_config[0];
	int tc;

	for (tc = 0; tc < TXGBE_DCB_TC_MAX; tc++)
		tsa[tc] = tc_config[tc].path[direction].tsa;
}

u8 txgbe_dcb_get_tc_from_up(struct txgbe_dcb_config *cfg, int direction, u8 up)
{
	struct txgbe_dcb_tc_config *tc_config = &cfg->tc_config[0];
	u8 prio_mask = 1 << up;
	u8 tc = cfg->num_tcs.pg_tcs;

	/* If tc is 0 then DCB is likely not enabled or supported */
	if (!tc)
		goto out;

	/*
	 * Test from maximum TC to 1 and report the first match we find.  If
	 * we find no match we can assume that the TC is 0 since the TC must
	 * be set for all user priorities
	 */
	for (tc--; tc; tc--) {
		if (prio_mask & tc_config[tc].path[direction].up_to_tc_bitmap)
			break;
	}
out:
	return tc;
}

void txgbe_dcb_unpack_map_cee(struct txgbe_dcb_config *cfg, int direction,
			      u8 *map)
{
	u8 up;

	for (up = 0; up < TXGBE_DCB_UP_MAX; up++)
		map[up] = txgbe_dcb_get_tc_from_up(cfg, direction, up);
}

/**
 * txgbe_dcb_config - Struct containing DCB settings.
 * @dcb_config: Pointer to DCB config structure
 *
 * This function checks DCB rules for DCB settings.
 * The following rules are checked:
 * 1. The sum of bandwidth percentages of all Bandwidth Groups must total 100%.
 * 2. The sum of bandwidth percentages of all Traffic Classes within a Bandwidth
 *    Group must total 100.
 * 3. A Traffic Class should not be set to both Link Strict Priority
 *    and Group Strict Priority.
 * 4. Link strict Bandwidth Groups can only have link strict traffic classes
 *    with zero bandwidth.
 */
s32 txgbe_dcb_check_config_cee(struct txgbe_dcb_config *dcb_config)
{
	struct txgbe_dcb_tc_path *p;
	s32 ret_val = 0;
	u8 i, j, bw = 0, bw_id;
	u8 bw_sum[2][TXGBE_DCB_BWG_MAX];
	bool link_strict[2][TXGBE_DCB_BWG_MAX];

	memset(bw_sum, 0, sizeof(bw_sum));
	memset(link_strict, 0, sizeof(link_strict));

	/* First Tx, then Rx */
	for (i = 0; i < 2; i++) {
		/* Check each traffic class for rule violation */
		for (j = 0; j < TXGBE_DCB_TC_MAX; j++) {
			p = &dcb_config->tc_config[j].path[i];

			bw = p->bwg_percent;
			bw_id = p->bwg_id;

			if (bw_id >= TXGBE_DCB_BWG_MAX) {
				ret_val = TXGBE_ERR_CONFIG;
				goto err_config;
			}
			if (p->tsa == txgbe_dcb_tsa_strict) {
				link_strict[i][bw_id] = true;
				/* Link strict should have zero bandwidth */
				if (bw) {
					ret_val = TXGBE_ERR_CONFIG;
					goto err_config;
				}
			} else if (!bw) {
				/*
				 * Traffic classes without link strict
				 * should have non-zero bandwidth.
				 */
				ret_val = TXGBE_ERR_CONFIG;
				goto err_config;
			}
			bw_sum[i][bw_id] += bw;
		}

		bw = 0;

		/* Check each bandwidth group for rule violation */
		for (j = 0; j < TXGBE_DCB_BWG_MAX; j++) {
			bw += dcb_config->bw_percentage[j][i];
			/*
			 * Sum of bandwidth percentages of all traffic classes
			 * within a Bandwidth Group must total 100 except for
			 * link strict group (zero bandwidth).
			 */
			if (link_strict[i][j]) {
				if (bw_sum[i][j]) {
					/*
					 * Link strict group should have zero
					 * bandwidth.
					 */
					ret_val = TXGBE_ERR_CONFIG;
					goto err_config;
				}
			} else if (bw_sum[i][j] != TXGBE_DCB_BW_PERCENT &&
				   bw_sum[i][j] != 0) {
				ret_val = TXGBE_ERR_CONFIG;
				goto err_config;
			}
		}

		if (bw != TXGBE_DCB_BW_PERCENT) {
			ret_val = TXGBE_ERR_CONFIG;
			goto err_config;
		}
	}

err_config:

	return ret_val;
}

/**
 * txgbe_dcb_get_tc_stats - Returns status of each traffic class
 * @hw: pointer to hardware structure
 * @stats: pointer to statistics structure
 * @tc_count:  Number of elements in bwg_array.
 *
 * This function returns the status data for each of the Traffic Classes in use.
 */
s32 txgbe_dcb_get_tc_stats(struct txgbe_hw *hw, struct txgbe_hw_stats *stats,
			   u8 tc_count)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	ret = txgbe_dcb_get_tc_stats_raptor(hw, stats, tc_count);
	return ret;
}

/**
 * txgbe_dcb_get_pfc_stats - Returns CBFC status of each traffic class
 * @hw: pointer to hardware structure
 * @stats: pointer to statistics structure
 * @tc_count:  Number of elements in bwg_array.
 *
 * This function returns the CBFC status data for each of the Traffic Classes.
 */
s32 txgbe_dcb_get_pfc_stats(struct txgbe_hw *hw, struct txgbe_hw_stats *stats,
			    u8 tc_count)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	ret = txgbe_dcb_get_pfc_stats_raptor(hw, stats, tc_count);
	return ret;
}

/**
 * txgbe_dcb_config_rx_arbiter_cee - Config Rx arbiter
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure Rx Data Arbiter and credits for each traffic class.
 */
s32 txgbe_dcb_config_rx_arbiter_cee(struct txgbe_hw *hw,
				struct txgbe_dcb_config *dcb_config)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	u8 tsa[TXGBE_DCB_TC_MAX]	= { 0 };
	u8 bwgid[TXGBE_DCB_TC_MAX]	= { 0 };
	u8 map[TXGBE_DCB_UP_MAX]	= { 0 };
	u16 refill[TXGBE_DCB_TC_MAX]	= { 0 };
	u16 max[TXGBE_DCB_TC_MAX]	= { 0 };

	txgbe_dcb_unpack_refill_cee(dcb_config, TXGBE_DCB_TX_CONFIG, refill);
	txgbe_dcb_unpack_max_cee(dcb_config, max);
	txgbe_dcb_unpack_bwgid_cee(dcb_config, TXGBE_DCB_TX_CONFIG, bwgid);
	txgbe_dcb_unpack_tsa_cee(dcb_config, TXGBE_DCB_TX_CONFIG, tsa);
	txgbe_dcb_unpack_map_cee(dcb_config, TXGBE_DCB_TX_CONFIG, map);

	ret = txgbe_dcb_config_rx_arbiter_raptor(hw, refill, max, bwgid,
						tsa, map);
	return ret;
}

/**
 * txgbe_dcb_config_tx_desc_arbiter_cee - Config Tx Desc arbiter
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure Tx Descriptor Arbiter and credits for each traffic class.
 */
s32 txgbe_dcb_config_tx_desc_arbiter_cee(struct txgbe_hw *hw,
				     struct txgbe_dcb_config *dcb_config)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	u8 tsa[TXGBE_DCB_TC_MAX];
	u8 bwgid[TXGBE_DCB_TC_MAX];
	u16 refill[TXGBE_DCB_TC_MAX];
	u16 max[TXGBE_DCB_TC_MAX];

	txgbe_dcb_unpack_refill_cee(dcb_config, TXGBE_DCB_TX_CONFIG, refill);
	txgbe_dcb_unpack_max_cee(dcb_config, max);
	txgbe_dcb_unpack_bwgid_cee(dcb_config, TXGBE_DCB_TX_CONFIG, bwgid);
	txgbe_dcb_unpack_tsa_cee(dcb_config, TXGBE_DCB_TX_CONFIG, tsa);

	ret = txgbe_dcb_config_tx_desc_arbiter_raptor(hw, refill, max,
						     bwgid, tsa);

	return ret;
}

/**
 * txgbe_dcb_config_tx_data_arbiter_cee - Config Tx data arbiter
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure Tx Data Arbiter and credits for each traffic class.
 */
s32 txgbe_dcb_config_tx_data_arbiter_cee(struct txgbe_hw *hw,
				     struct txgbe_dcb_config *dcb_config)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	u8 tsa[TXGBE_DCB_TC_MAX];
	u8 bwgid[TXGBE_DCB_TC_MAX];
	u8 map[TXGBE_DCB_UP_MAX] = { 0 };
	u16 refill[TXGBE_DCB_TC_MAX];
	u16 max[TXGBE_DCB_TC_MAX];

	txgbe_dcb_unpack_refill_cee(dcb_config, TXGBE_DCB_TX_CONFIG, refill);
	txgbe_dcb_unpack_max_cee(dcb_config, max);
	txgbe_dcb_unpack_bwgid_cee(dcb_config, TXGBE_DCB_TX_CONFIG, bwgid);
	txgbe_dcb_unpack_tsa_cee(dcb_config, TXGBE_DCB_TX_CONFIG, tsa);
	txgbe_dcb_unpack_map_cee(dcb_config, TXGBE_DCB_TX_CONFIG, map);

	ret = txgbe_dcb_config_tx_data_arbiter_raptor(hw, refill, max,
						     bwgid, tsa,
						     map);

	return ret;
}

/**
 * txgbe_dcb_config_pfc_cee - Config priority flow control
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure Priority Flow Control for each traffic class.
 */
s32 txgbe_dcb_config_pfc_cee(struct txgbe_hw *hw,
			 struct txgbe_dcb_config *dcb_config)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	u8 pfc_en;
	u8 map[TXGBE_DCB_UP_MAX] = { 0 };

	txgbe_dcb_unpack_map_cee(dcb_config, TXGBE_DCB_TX_CONFIG, map);
	txgbe_dcb_unpack_pfc_cee(dcb_config, map, &pfc_en);

	ret = txgbe_dcb_config_pfc_raptor(hw, pfc_en, map);

	return ret;
}

/**
 * txgbe_dcb_config_tc_stats - Config traffic class statistics
 * @hw: pointer to hardware structure
 *
 * Configure queue statistics registers, all queues belonging to same traffic
 * class uses a single set of queue statistics counters.
 */
s32 txgbe_dcb_config_tc_stats(struct txgbe_hw *hw)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	ret = txgbe_dcb_config_tc_stats_raptor(hw, NULL);

	return ret;
}

/**
 * txgbe_dcb_hw_config_cee - Config and enable DCB
 * @hw: pointer to hardware structure
 * @dcb_config: pointer to txgbe_dcb_config structure
 *
 * Configure dcb settings and enable dcb mode.
 */
s32 txgbe_dcb_hw_config_cee(struct txgbe_hw *hw,
			struct txgbe_dcb_config *dcb_config)
{
	s32 ret = TXGBE_NOT_IMPLEMENTED;
	u8 pfc_en;
	u8 tsa[TXGBE_DCB_TC_MAX];
	u8 bwgid[TXGBE_DCB_TC_MAX];
	u8 map[TXGBE_DCB_UP_MAX] = { 0 };
	u16 refill[TXGBE_DCB_TC_MAX];
	u16 max[TXGBE_DCB_TC_MAX];

	/* Unpack CEE standard containers */
	txgbe_dcb_unpack_refill_cee(dcb_config, TXGBE_DCB_TX_CONFIG, refill);
	txgbe_dcb_unpack_max_cee(dcb_config, max);
	txgbe_dcb_unpack_bwgid_cee(dcb_config, TXGBE_DCB_TX_CONFIG, bwgid);
	txgbe_dcb_unpack_tsa_cee(dcb_config, TXGBE_DCB_TX_CONFIG, tsa);
	txgbe_dcb_unpack_map_cee(dcb_config, TXGBE_DCB_TX_CONFIG, map);

	txgbe_dcb_config_raptor(hw, dcb_config);
	ret = txgbe_dcb_hw_config_raptor(hw, dcb_config->link_speed,
					refill, max, bwgid,
					tsa, map);

	txgbe_dcb_config_tc_stats_raptor(hw, dcb_config);

	if (!ret && dcb_config->pfc_mode_enable) {
		txgbe_dcb_unpack_pfc_cee(dcb_config, map, &pfc_en);
		ret = txgbe_dcb_config_pfc(hw, pfc_en, map);
	}

	return ret;
}

/* Helper routines to abstract HW specifics from DCB netlink ops */
s32 txgbe_dcb_config_pfc(struct txgbe_hw *hw, u8 pfc_en, u8 *map)
{
	int ret = TXGBE_ERR_PARAM;
	ret = txgbe_dcb_config_pfc_raptor(hw, pfc_en, map);
	return ret;
}

s32 txgbe_dcb_hw_config(struct txgbe_hw *hw, u16 *refill, u16 *max,
			    u8 *bwg_id, u8 *tsa, u8 *map)
{
	txgbe_dcb_config_rx_arbiter_raptor(hw, refill, max, bwg_id,
					  tsa, map);
	txgbe_dcb_config_tx_desc_arbiter_raptor(hw, refill, max, bwg_id,
					       tsa);
	txgbe_dcb_config_tx_data_arbiter_raptor(hw, refill, max, bwg_id,
					       tsa, map);
	return 0;
}
