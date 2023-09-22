/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2017 Intel Corporation
 */

#include <sys/queue.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <rte_byteorder.h>
#include <rte_common.h>
#include <rte_cycles.h>

#include <rte_interrupts.h>
#include <rte_log.h>
#include <rte_debug.h>
#include <rte_pci.h>
#include <rte_branch_prediction.h>
#include <rte_memory.h>
#include <rte_eal.h>
#include <rte_alarm.h>
#include <rte_ether.h>
#include <rte_malloc.h>
#include <rte_random.h>
#include <rte_dev.h>
#include <rte_hash_crc.h>
#ifdef RTE_LIBRTE_SECURITY
#include <rte_security_driver.h>
#endif

#include "ngbe_logs.h"
#include "base/ngbe.h"
#include "ngbe_ethdev.h"
#include "ngbe_rxtx.h"
#include "ngbe_regs_group.h"

static const struct reg_info ngbe_regs_general[] = {
	{NGBE_RST, 1, 1, "NGBE_RST"},
	{NGBE_STAT, 1, 1, "NGBE_STAT"},
	{NGBE_PORTCTL, 1, 1, "NGBE_PORTCTL"},
	{NGBE_GPIODATA, 1, 1, "NGBE_GPIODATA"},
	{NGBE_GPIOCTL, 1, 1, "NGBE_GPIOCTL"},
	{NGBE_LEDCTL, 1, 1, "NGBE_LEDCTL"},
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_nvm[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_interrupt[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_fctl_others[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_rxdma[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_rx[] = {
	{0, 0, 0, ""}
};

static struct reg_info ngbe_regs_tx[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_wakeup[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_mac[] = {
	{0, 0, 0, ""}
};

static const struct reg_info ngbe_regs_diagnostic[] = {
	{0, 0, 0, ""},
};

/* PF registers */
static const struct reg_info *ngbe_regs_others[] = {
				ngbe_regs_general,
				ngbe_regs_nvm,
				ngbe_regs_interrupt,
				ngbe_regs_fctl_others,
				ngbe_regs_rxdma,
				ngbe_regs_rx,
				ngbe_regs_tx,
				ngbe_regs_wakeup,
				ngbe_regs_mac,
				ngbe_regs_diagnostic,
				NULL};

static int ngbe_l2_tn_filter_init(struct rte_eth_dev *eth_dev);
static int ngbe_l2_tn_filter_uninit(struct rte_eth_dev *eth_dev);
static int ngbe_ntuple_filter_uninit(struct rte_eth_dev *eth_dev);
static int  ngbe_dev_set_link_up(struct rte_eth_dev *dev);
static int  ngbe_dev_set_link_down(struct rte_eth_dev *dev);
static void ngbe_dev_close(struct rte_eth_dev *dev);
static int ngbe_dev_allmulticast_enable(struct rte_eth_dev *dev);
static int ngbe_dev_allmulticast_disable(struct rte_eth_dev *dev);
static int ngbe_dev_link_update(struct rte_eth_dev *dev,
				int wait_to_complete);
static int ngbe_dev_stats_reset(struct rte_eth_dev *dev);
static int ngbe_dev_xstats_reset(struct rte_eth_dev *dev);
static void ngbe_vlan_hw_strip_enable(struct rte_eth_dev *dev, uint16_t queue);
static void ngbe_vlan_hw_strip_disable(struct rte_eth_dev *dev, uint16_t queue);

static void ngbe_dev_link_status_print(struct rte_eth_dev *dev);
static int ngbe_dev_lsc_interrupt_setup(struct rte_eth_dev *dev, uint8_t on);
static int ngbe_dev_macsec_interrupt_setup(struct rte_eth_dev *dev);
static int ngbe_dev_misc_interrupt_setup(struct rte_eth_dev *dev);
static int ngbe_dev_rxq_interrupt_setup(struct rte_eth_dev *dev);
static int ngbe_dev_interrupt_get_status(struct rte_eth_dev *dev);
static int ngbe_dev_interrupt_action(struct rte_eth_dev *dev,
				      struct rte_intr_handle *handle);
static void ngbe_dev_interrupt_handler(void *param);
static int ngbe_add_rar(struct rte_eth_dev *dev, struct ether_addr *mac_addr,
			 uint32_t index, uint32_t pool);
static void ngbe_remove_rar(struct rte_eth_dev *dev, uint32_t index);

/* For Eth VMDQ APIs support */
static int ngbe_uc_hash_table_set(struct rte_eth_dev *dev, struct
		ether_addr * mac_addr, uint8_t on);
static int ngbe_uc_all_hash_table_set(struct rte_eth_dev *dev, uint8_t on);
static int ngbe_mirror_rule_set(struct rte_eth_dev *dev,
		struct rte_eth_mirror_conf *mirror_conf,
		uint8_t rule_id, uint8_t on);
static int ngbe_mirror_rule_reset(struct rte_eth_dev *dev,
		uint8_t	rule_id);
static int ngbe_dev_rx_queue_intr_enable(struct rte_eth_dev *dev,
					  uint16_t queue_id);
static int ngbe_dev_rx_queue_intr_disable(struct rte_eth_dev *dev,
					   uint16_t queue_id);
static void ngbe_configure_msix(struct rte_eth_dev *dev);

static int ngbe_syn_filter_get(struct rte_eth_dev *dev,
			struct rte_eth_syn_filter *filter);
static int ngbe_syn_filter_handle(struct rte_eth_dev *dev,
			enum rte_filter_op filter_op,
			void *arg);
static int ngbe_dev_l2_tunnel_filter_handle(struct rte_eth_dev *dev,
					     enum rte_filter_op filter_op,
					     void *arg);
static int ngbe_filter_restore(struct rte_eth_dev *dev);
static void ngbe_l2_tunnel_conf(struct rte_eth_dev *dev);

#define NGBE_SET_HWSTRIP(h, q) do {\
		uint32_t idx = (q) / (sizeof((h)->bitmap[0]) * NBBY); \
		uint32_t bit = (q) % (sizeof((h)->bitmap[0]) * NBBY); \
		(h)->bitmap[idx] |= 1 << bit;\
	} while (0)

#define NGBE_CLEAR_HWSTRIP(h, q) do {\
		uint32_t idx = (q) / (sizeof((h)->bitmap[0]) * NBBY); \
		uint32_t bit = (q) % (sizeof((h)->bitmap[0]) * NBBY); \
		(h)->bitmap[idx] &= ~(1 << bit);\
	} while (0)

#define NGBE_GET_HWSTRIP(h, q, r) do {\
		uint32_t idx = (q) / (sizeof((h)->bitmap[0]) * NBBY); \
		uint32_t bit = (q) % (sizeof((h)->bitmap[0]) * NBBY); \
		(r) = (h)->bitmap[idx] >> bit & 1;\
	} while (0)

int ngbe_logtype_init = RTE_LOGTYPE_PMD;
int ngbe_logtype_driver = RTE_LOGTYPE_PMD;

int ngbe_logtype_rx = RTE_LOGTYPE_PMD;
int ngbe_logtype_tx = RTE_LOGTYPE_PMD;
int ngbe_logtype_tx_free = RTE_LOGTYPE_PMD;

/*
 * The set of PCI devices this driver supports
 */
static const struct rte_pci_id pci_id_ngbe_map[] = {
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860A2) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860A2S) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860A4) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860A4S) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860AL2) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860AL2S) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860AL4) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860AL4S) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860NCSI) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860A1) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860A1L) },
	{ RTE_PCI_DEVICE(PCI_VENDOR_ID_WANGXUN, NGBE_DEV_ID_EM_WX1860AL_W) },
	{ .vendor_id = 0, /* sentinel */ },
};

static const struct rte_eth_desc_lim rx_desc_lim = {
	.nb_max = NGBE_RING_DESC_MAX,
	.nb_min = NGBE_RING_DESC_MIN,
	.nb_align = NGBE_RXD_ALIGN,
};

static const struct rte_eth_desc_lim tx_desc_lim = {
	.nb_max = NGBE_RING_DESC_MAX,
	.nb_min = NGBE_RING_DESC_MIN,
	.nb_align = NGBE_TXD_ALIGN,
	.nb_seg_max = NGBE_TX_MAX_SEG,
	.nb_mtu_seg_max = NGBE_TX_MAX_SEG,
};

static const struct eth_dev_ops ngbe_eth_dev_ops;

#define HW_XSTAT(m) {#m, offsetof(struct ngbe_hw_stats, m)}
#define HW_XSTAT_NAME(m, n) {n, offsetof(struct ngbe_hw_stats, m)}
static const struct rte_ngbe_xstats_name_off rte_ngbe_stats_strings[] = {
	/* MNG RxTx */
	HW_XSTAT(mng_bmc2host_packets),
	HW_XSTAT(mng_host2bmc_packets),
	/* Basic RxTx */
	HW_XSTAT(rx_packets),
	HW_XSTAT(tx_packets),
	HW_XSTAT(rx_bytes),
	HW_XSTAT(tx_bytes),
	HW_XSTAT(rx_total_bytes),
	HW_XSTAT(rx_total_packets),
	HW_XSTAT(tx_total_packets),
	HW_XSTAT(rx_total_missed_packets),
	HW_XSTAT(rx_broadcast_packets),
	HW_XSTAT(rx_multicast_packets),
	HW_XSTAT(rx_management_packets),
	HW_XSTAT(tx_management_packets),
	HW_XSTAT(rx_management_dropped),
	HW_XSTAT(rx_dma_drop),
	HW_XSTAT(tx_secdrp_packets),

	/* Basic Error */
	HW_XSTAT(rx_crc_errors),
	HW_XSTAT(rx_illegal_byte_errors),
	HW_XSTAT(rx_error_bytes),
	HW_XSTAT(rx_mac_short_packet_dropped),
	HW_XSTAT(rx_length_errors),
	HW_XSTAT(rx_undersize_errors),
	HW_XSTAT(rx_fragment_errors),
	HW_XSTAT(rx_oversize_errors),
	HW_XSTAT(rx_jabber_errors),
	HW_XSTAT(rx_l3_l4_xsum_error),
	HW_XSTAT(mac_local_errors),
	HW_XSTAT(mac_remote_errors),

	/* PB Stats */
	HW_XSTAT(rx_up_dropped),
	HW_XSTAT(rdb_pkt_cnt),
	HW_XSTAT(rdb_repli_cnt),
	HW_XSTAT(rdb_drp_cnt),

	/* MACSEC */
	HW_XSTAT(tx_macsec_pkts_untagged),
	HW_XSTAT(tx_macsec_pkts_encrypted),
	HW_XSTAT(tx_macsec_pkts_protected),
	HW_XSTAT(tx_macsec_octets_encrypted),
	HW_XSTAT(tx_macsec_octets_protected),
	HW_XSTAT(rx_macsec_pkts_untagged),
	HW_XSTAT(rx_macsec_pkts_badtag),
	HW_XSTAT(rx_macsec_pkts_nosci),
	HW_XSTAT(rx_macsec_pkts_unknownsci),
	HW_XSTAT(rx_macsec_octets_decrypted),
	HW_XSTAT(rx_macsec_octets_validated),
	HW_XSTAT(rx_macsec_sc_pkts_unchecked),
	HW_XSTAT(rx_macsec_sc_pkts_delayed),
	HW_XSTAT(rx_macsec_sc_pkts_late),
	HW_XSTAT(rx_macsec_sa_pkts_ok),
	HW_XSTAT(rx_macsec_sa_pkts_invalid),
	HW_XSTAT(rx_macsec_sa_pkts_notvalid),
	HW_XSTAT(rx_macsec_sa_pkts_unusedsa),
	HW_XSTAT(rx_macsec_sa_pkts_notusingsa),

	/* MAC RxTx */
	HW_XSTAT(rx_size_64_packets),
	HW_XSTAT(rx_size_65_to_127_packets),
	HW_XSTAT(rx_size_128_to_255_packets),
	HW_XSTAT(rx_size_256_to_511_packets),
	HW_XSTAT(rx_size_512_to_1023_packets),
	HW_XSTAT(rx_size_1024_to_max_packets),
	HW_XSTAT(tx_size_64_packets),
	HW_XSTAT(tx_size_65_to_127_packets),
	HW_XSTAT(tx_size_128_to_255_packets),
	HW_XSTAT(tx_size_256_to_511_packets),
	HW_XSTAT(tx_size_512_to_1023_packets),
	HW_XSTAT(tx_size_1024_to_max_packets),

	/* Flow Control */
	HW_XSTAT(tx_xon_packets),
	HW_XSTAT(rx_xon_packets),
	HW_XSTAT(tx_xoff_packets),
	HW_XSTAT(rx_xoff_packets),

	HW_XSTAT_NAME(tx_xon_packets, "tx_flow_control_xon_packets"),
	HW_XSTAT_NAME(rx_xon_packets, "rx_flow_control_xon_packets"),
	HW_XSTAT_NAME(tx_xoff_packets, "tx_flow_control_xoff_packets"),
	HW_XSTAT_NAME(rx_xoff_packets, "rx_flow_control_xoff_packets"),
};

#define NGBE_NB_HW_STATS (sizeof(rte_ngbe_stats_strings) / \
			   sizeof(rte_ngbe_stats_strings[0]))

/* Per-queue statistics */
#define QP_XSTAT(m) {#m, offsetof(struct ngbe_hw_stats, qp[0].m)}
static const struct rte_ngbe_xstats_name_off rte_ngbe_qp_strings[] = {
	QP_XSTAT(rx_qp_packets),
	QP_XSTAT(tx_qp_packets),
	QP_XSTAT(rx_qp_bytes),
	QP_XSTAT(tx_qp_bytes),
	QP_XSTAT(rx_qp_mc_packets),
};

#define NGBE_NB_QP_STATS (sizeof(rte_ngbe_qp_strings) / \
			   sizeof(rte_ngbe_qp_strings[0]))

/*
 * This function is the same as ngbe_is_sfp() in base/ngbe.h.
 */
static inline int
ngbe_is_sfp(struct ngbe_hw *hw)
{
	switch (hw->phy.type) {
	case ngbe_phy_sfp_avago:
	case ngbe_phy_sfp_ftl:
	case ngbe_phy_sfp_intel:
	case ngbe_phy_sfp_unknown:
	case ngbe_phy_sfp_tyco_passive:
	case ngbe_phy_sfp_unknown_passive:
		return 1;
	default:
		return 0;
	}
}

static inline int32_t
ngbe_pf_reset_hw(struct ngbe_hw *hw)
{
	uint32_t ctrl_ext;
	int32_t status;

	status = hw->mac.reset_hw(hw);

	ctrl_ext = rd32(hw, NGBE_PORTCTL);
	/* Set PF Reset Done bit so PF/VF Mail Ops can work */
	ctrl_ext |= NGBE_PORTCTL_RSTDONE;
	wr32(hw, NGBE_PORTCTL, ctrl_ext);
	ngbe_flush(hw);

	if (status == NGBE_ERR_SFP_NOT_PRESENT)
		status = 0;
	return status;
}

static inline void
ngbe_enable_intr(struct rte_eth_dev *dev)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	wr32(hw, NGBE_IENMISC, intr->mask_misc);
	wr32(hw, NGBE_IMC(0), intr->mask & BIT_MASK32);
//	wr32(hw, NGBE_IMC(1), (intr->mask >> 32) & BIT_MASK32);
	ngbe_flush(hw);
}

/*
 * This function is based on ngbe_disable_intr() in base/ngbe.h.
 */
static void
ngbe_disable_intr(struct ngbe_hw *hw)
{
	PMD_INIT_FUNC_TRACE();

	//fixme: wr32(hw, NGBE_IENMISC, ~BIT_MASK32);
	wr32(hw, NGBE_IMS(0), NGBE_IMS_MASK);
//	wr32(hw, NGBE_IMS(1), NGBE_IMS_MASK);
	ngbe_flush(hw);
}

static int
ngbe_dev_queue_stats_mapping_set(struct rte_eth_dev *eth_dev,
				  uint16_t queue_id,
				  uint8_t stat_idx,
				  uint8_t is_rx)
{
	struct ngbe_stat_mappings *stat_mappings =
		NGBE_DEV_STAT_MAPPINGS(eth_dev);
	uint32_t qsmr_mask = 0;
	uint32_t clearing_mask = QMAP_FIELD_RESERVED_BITS_MASK;
	uint32_t q_map;
	uint8_t n, offset;

	PMD_INIT_LOG(DEBUG, "Setting port %d, %s queue_id %d to stat index %d",
		     (int)(eth_dev->data->port_id), is_rx ? "RX" : "TX",
		     queue_id, stat_idx);

	n = (uint8_t)(queue_id / NB_QMAP_FIELDS_PER_QSM_REG);
	if (n >= NGBE_NB_STAT_MAPPING) {
		PMD_INIT_LOG(ERR, "Nb of stat mapping registers exceeded");
		return -EIO;
	}
	offset = (uint8_t)(queue_id % NB_QMAP_FIELDS_PER_QSM_REG);

	/* Now clear any previous stat_idx set */
	clearing_mask <<= (QSM_REG_NB_BITS_PER_QMAP_FIELD * offset);
	if (!is_rx)
		stat_mappings->tqsm[n] &= ~clearing_mask;
	else
		stat_mappings->rqsm[n] &= ~clearing_mask;

	q_map = (uint32_t)stat_idx;
	q_map &= QMAP_FIELD_RESERVED_BITS_MASK;
	qsmr_mask = q_map << (QSM_REG_NB_BITS_PER_QMAP_FIELD * offset);
	if (!is_rx)
		stat_mappings->tqsm[n] |= qsmr_mask;
	else
		stat_mappings->rqsm[n] |= qsmr_mask;

	PMD_INIT_LOG(DEBUG, "Set port %d, %s queue_id %d to stat index %d",
		     (int)(eth_dev->data->port_id), is_rx ? "RX" : "TX",
		     queue_id, stat_idx);
	PMD_INIT_LOG(DEBUG, "%s[%d] = 0x%08x", is_rx ? "RQSMR" : "TQSM", n,
		     is_rx ? stat_mappings->rqsm[n] : stat_mappings->tqsm[n]);
	return 0;
}

/*
 * Ensure that all locks are released before first NVM or PHY access
 */
static void
ngbe_swfw_lock_reset(struct ngbe_hw *hw)
{
	uint16_t mask;

	/*
	 * Phy lock should not fail in this early stage. If this is the case,
	 * it is due to an improper exit of the application.
	 * So force the release of the faulty lock. Release of common lock
	 * is done automatically by swfw_sync function.
	 */

	/*
	 * These ones are more tricky since they are common to all ports; but
	 * swfw_sync retries last long enough (1s) to be almost sure that if
	 * lock can not be taken it is due to an improper lock of the
	 * semaphore.
	 */
	mask = NGBE_MNGSEM_SWPHY |
	       NGBE_MNGSEM_SWMBX |
	       NGBE_MNGSEM_SWFLASH;
	if (hw->mac.acquire_swfw_sync(hw, mask) < 0) {
		PMD_DRV_LOG(DEBUG, "SWFW common locks released");
	}
	hw->mac.release_swfw_sync(hw, mask);
}

/*
 * This function is based on code in ngbe_attach() in base/ngbe.c.
 * It returns 0 on success.
 */
static int
eth_ngbe_dev_init(struct rte_eth_dev *eth_dev, void *init_params __rte_unused)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(eth_dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vfta *shadow_vfta = NGBE_DEV_VFTA(eth_dev);
	struct ngbe_hwstrip *hwstrip = NGBE_DEV_HWSTRIP(eth_dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(eth_dev);
	struct ngbe_bw_conf *bw_conf = NGBE_DEV_BW_CONF(eth_dev);
	struct rte_intr_handle *intr_handle = &pci_dev->intr_handle;
	const struct rte_memzone *mz;
	uint32_t ctrl_ext;
	u32 led_conf = 0;
	int err;

	PMD_INIT_FUNC_TRACE();

	eth_dev->dev_ops = &ngbe_eth_dev_ops;
	eth_dev->rx_pkt_burst = &ngbe_recv_pkts;
	eth_dev->tx_pkt_burst = &ngbe_xmit_pkts;
	eth_dev->tx_pkt_prepare = &ngbe_prep_pkts;

	/*
	 * For secondary processes, we don't initialise any further as primary
	 * has already done this work. Only check we don't need a different
	 * RX and TX function.
	 */
	if (rte_eal_process_type() != RTE_PROC_PRIMARY) {
		struct ngbe_tx_queue *txq;
		/* TX queue function in primary, set by last queue initialized
		 * Tx queue may not initialized by primary process
		 */
		if (eth_dev->data->tx_queues) {
			txq = eth_dev->data->tx_queues[eth_dev->data->nb_tx_queues-1];
			ngbe_set_tx_function(eth_dev, txq);
		} else {
			/* Use default TX function if we get here */
			PMD_INIT_LOG(NOTICE, "No TX queues configured yet. "
				     "Using default TX function.");
		}

		ngbe_set_rx_function(eth_dev);

		return 0;
	}

	rte_eth_copy_pci_info(eth_dev, pci_dev);

	/* Vendor and Device ID need to be set before init of shared code */
	hw->back = pci_dev;
	hw->device_id = pci_dev->id.device_id;
	hw->vendor_id = pci_dev->id.vendor_id;
	hw->sub_system_id = pci_dev->id.subsystem_device_id;
	hw->sub_vendor_id = pci_dev->id.subsystem_vendor_id;
	ngbe_map_device_id(hw);
	hw->hw_addr = (void *)pci_dev->mem_resource[0].addr;
	hw->allow_unsupported_sfp = 1;

	/* Reserve memory for interrupt status block */
	mz = rte_eth_dma_zone_reserve(eth_dev, "ngbe_driver", -1,
		16, NGBE_ALIGN, SOCKET_ID_ANY);
	if (mz == NULL) {
		return -ENOMEM;
	}
	hw->isb_dma = TMZ_PADDR(mz);
	hw->isb_mem = TMZ_VADDR(mz);

	/* Initialize the shared code (base driver) */
	err = ngbe_init_shared_code(hw);
	if (err != 0) {
		PMD_INIT_LOG(ERR, "Shared code init failed: %d", err);
		return -EIO;
	}

	if (hw->mac.fw_recovery_mode && hw->mac.fw_recovery_mode(hw)) {
		PMD_INIT_LOG(ERR, "\nERROR: "
			"Firmware recovery mode detected. Limiting functionality.\n"
			"Refer to the WangXun(R) Ethernet Adapters and Devices "
			"User Guide for details on firmware recovery mode.");
		return -EIO;
	}

	/* pick up the PCI bus settings for reporting later */

	/* Unlock any pending hardware semaphore */
	ngbe_swfw_lock_reset(hw);

#ifdef RTE_LIBRTE_SECURITY
	/* Initialize security_ctx only for primary process*/
	if (ngbe_ipsec_ctx_create(eth_dev))
		return -ENOMEM;
#endif

	/* Get Hardware Flow Control setting */
	hw->fc.requested_mode = ngbe_fc_full;
	hw->fc.current_mode = ngbe_fc_full;
	hw->fc.pause_time = NGBE_FC_PAUSE_TIME;
	hw->fc.low_water = NGBE_FC_XON_LOTH;
	hw->fc.high_water = NGBE_FC_XOFF_HITH;
	hw->fc.send_xon = 1;

	err = hw->rom.init_params(hw);
	if (err != 0) {
		PMD_INIT_LOG(ERR, "The EEPROM init failed: %d", err);
		return -EIO;
	}

	/* Make sure we have a good EEPROM before we read from it */
	err = hw->rom.validate_checksum(hw, NULL);
	if (err != 0) {
		PMD_INIT_LOG(ERR, "The EEPROM checksum is not valid: %d", err);
		return -EIO;
	}

	err = hw->phy.led_oem_chk(hw, &led_conf);
	if (err == 0)
		hw->led_conf = led_conf;
	else
		hw->led_conf = 0xFFFF;

	err = hw->mac.init_hw(hw);

	/*
	 * Devices with copper phys will fail to initialise if ngbe_init_hw()
	 * is called too soon after the kernel driver unbinding/binding occurs.
	 * The failure occurs in ngbe_identify_phy() for all devices,
	 * but for non-copper devies, ngbe_identify_sfp_module() is
	 * also called. See ngbe_identify_phy(). The reason for the
	 * failure is not known, and only occuts when virtualisation features
	 * are disabled in the bios. A delay of 100ms  was found to be enough by
	 * trial-and-error, and is doubled to be safe.
	 */
	if (err && (hw->phy.media_type == ngbe_media_type_copper)) {
		rte_delay_ms(200);
		err = hw->mac.init_hw(hw);
	}

	if (err == NGBE_ERR_SFP_NOT_PRESENT)
		err = 0;

	if (err == NGBE_ERR_EEPROM_VERSION) {
		PMD_INIT_LOG(ERR, "This device is a pre-production adapter/"
			     "LOM.  Please be aware there may be issues associated "
			     "with your hardware.");
		PMD_INIT_LOG(ERR, "If you are experiencing problems "
			     "please contact your Intel or hardware representative "
			     "who provided you with this hardware.");
	} else if (err == NGBE_ERR_SFP_NOT_SUPPORTED)
		PMD_INIT_LOG(ERR, "Unsupported SFP+ Module");
	if (err) {
		PMD_INIT_LOG(ERR, "Hardware Initialization Failure: %d", err);
		return -EIO;
	}

	/* Reset the hw statistics */
	ngbe_dev_stats_reset(eth_dev);

	/* disable interrupt */
	ngbe_disable_intr(hw);

	/* Allocate memory for storing MAC addresses */
	eth_dev->data->mac_addrs = rte_zmalloc("ngbe", ETHER_ADDR_LEN *
					       hw->mac.num_rar_entries, 0);
	if (eth_dev->data->mac_addrs == NULL) {
		PMD_INIT_LOG(ERR,
			     "Failed to allocate %u bytes needed to store "
			     "MAC addresses",
			     ETHER_ADDR_LEN * hw->mac.num_rar_entries);
		return -ENOMEM;
	}

	/* Copy the permanent MAC address */
	ether_addr_copy((struct ether_addr *) hw->mac.perm_addr,
			&eth_dev->data->mac_addrs[0]);

	/* Allocate memory for storing hash filter MAC addresses */
	eth_dev->data->hash_mac_addrs = rte_zmalloc("ngbe", ETHER_ADDR_LEN *
						    NGBE_VMDQ_NUM_UC_MAC, 0);
	if (eth_dev->data->hash_mac_addrs == NULL) {
		PMD_INIT_LOG(ERR,
			     "Failed to allocate %d bytes needed to store MAC addresses",
			     ETHER_ADDR_LEN * NGBE_VMDQ_NUM_UC_MAC);
		return -ENOMEM;
	}

	/* initialize the vfta */
	memset(shadow_vfta, 0, sizeof(*shadow_vfta));

	/* initialize the hw strip bitmap*/
	memset(hwstrip, 0, sizeof(*hwstrip));

	/* initialize PF if max_vfs not zero */
	ngbe_pf_host_init(eth_dev);

	ctrl_ext = rd32(hw, NGBE_PORTCTL);
	/* let hardware know driver is loaded */
	ctrl_ext |= NGBE_PORTCTL_DRVLOAD;
	/* Set PF Reset Done bit so PF/VF Mail Ops can work */
	ctrl_ext |= NGBE_PORTCTL_RSTDONE;
	wr32(hw, NGBE_PORTCTL, ctrl_ext);
	ngbe_flush(hw);

	if (ngbe_is_sfp(hw) && hw->phy.sfp_type != ngbe_sfp_type_not_present)
		PMD_INIT_LOG(DEBUG, "MAC: %d, PHY: %d, SFP+: %d",
			     (int) hw->mac.type, (int) hw->phy.type,
			     (int) hw->phy.sfp_type);
	else
		PMD_INIT_LOG(DEBUG, "MAC: %d, PHY: %d",
			     (int) hw->mac.type, (int) hw->phy.type);

	PMD_INIT_LOG(DEBUG, "port %d vendorID=0x%x deviceID=0x%x",
		     eth_dev->data->port_id, pci_dev->id.vendor_id,
		     pci_dev->id.device_id);

	rte_intr_callback_register(intr_handle,
				   ngbe_dev_interrupt_handler, eth_dev);

	/* enable uio/vfio intr/eventfd mapping */
	rte_intr_enable(intr_handle);

	/* enable support intr */
	ngbe_enable_intr(eth_dev);

	/* initialize filter info */
	memset(filter_info, 0,
	       sizeof(struct ngbe_filter_info));

	/* initialize 5tuple filter list */
	TAILQ_INIT(&filter_info->fivetuple_list);

	/* initialize l2 tunnel filter list & hash */
	ngbe_l2_tn_filter_init(eth_dev);

	/* initialize flow filter lists */
	ngbe_filterlist_init();

	/* initialize bandwidth configuration info */
	memset(bw_conf, 0, sizeof(struct ngbe_bw_conf));

	/* initialize Traffic Manager configuration */
	ngbe_tm_conf_init(eth_dev);

	return 0;
}

static int
eth_ngbe_dev_uninit(struct rte_eth_dev *eth_dev)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(eth_dev);
	struct rte_intr_handle *intr_handle = &pci_dev->intr_handle;
	struct ngbe_hw *hw;
	int retries = 0;
	int ret;

	PMD_INIT_FUNC_TRACE();

	if (rte_eal_process_type() != RTE_PROC_PRIMARY)
		return 0;

	hw = NGBE_DEV_HW(eth_dev);

	if (hw->adapter_stopped == 0)
		ngbe_dev_close(eth_dev);

	eth_dev->dev_ops = NULL;
	eth_dev->rx_pkt_burst = NULL;
	eth_dev->tx_pkt_burst = NULL;

	/* Unlock any pending hardware semaphore */
	ngbe_swfw_lock_reset(hw);

	/* disable uio intr before callback unregister */
	rte_intr_disable(intr_handle);

	do {
		ret = rte_intr_callback_unregister(intr_handle,
				ngbe_dev_interrupt_handler, eth_dev);
		if (ret >= 0) {
			break;
		} else if (ret != -EAGAIN) {
			PMD_INIT_LOG(ERR,
				"intr callback unregister failed: %d",
				ret);
			return ret;
		}
		rte_delay_ms(100);
	} while (retries++ < (10 + NGBE_LINK_UP_TIME));

	/* uninitialize PF if max_vfs not zero */
	ngbe_pf_host_uninit(eth_dev);

	rte_free(eth_dev->data->mac_addrs);
	eth_dev->data->mac_addrs = NULL;

	rte_free(eth_dev->data->hash_mac_addrs);
	eth_dev->data->hash_mac_addrs = NULL;

	/* remove all the L2 tunnel filters & hash */
	ngbe_l2_tn_filter_uninit(eth_dev);

	/* Remove all ntuple filters of the device */
	ngbe_ntuple_filter_uninit(eth_dev);

	/* clear all the filters list */
	ngbe_filterlist_flush();

	/* Remove all Traffic Manager configuration */
	ngbe_tm_conf_uninit(eth_dev);

#ifdef RTE_LIBRTE_SECURITY
	rte_free(eth_dev->security_ctx);
#endif

	return 0;
}

static int ngbe_ntuple_filter_uninit(struct rte_eth_dev *eth_dev)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(eth_dev);
	struct ngbe_5tuple_filter *p_5tuple;

	while ((p_5tuple = TAILQ_FIRST(&filter_info->fivetuple_list))) {
		TAILQ_REMOVE(&filter_info->fivetuple_list,
			     p_5tuple,
			     entries);
		rte_free(p_5tuple);
	}
	memset(filter_info->fivetuple_mask, 0,
	       sizeof(uint32_t) * NGBE_5TUPLE_ARRAY_SIZE);

	return 0;
}

static int ngbe_l2_tn_filter_uninit(struct rte_eth_dev *eth_dev)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(eth_dev);
	struct ngbe_l2_tn_filter *l2_tn_filter;

	if (l2_tn_info->hash_map)
		rte_free(l2_tn_info->hash_map);
	if (l2_tn_info->hash_handle)
		rte_hash_free(l2_tn_info->hash_handle);

	while ((l2_tn_filter = TAILQ_FIRST(&l2_tn_info->l2_tn_list))) {
		TAILQ_REMOVE(&l2_tn_info->l2_tn_list,
			     l2_tn_filter,
			     entries);
		rte_free(l2_tn_filter);
	}

	return 0;
}

static int ngbe_l2_tn_filter_init(struct rte_eth_dev *eth_dev)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(eth_dev);
	char l2_tn_hash_name[RTE_HASH_NAMESIZE];
	struct rte_hash_parameters l2_tn_hash_params = {
		.name = l2_tn_hash_name,
		.entries = NGBE_MAX_L2_TN_FILTER_NUM,
		.key_len = sizeof(struct ngbe_l2_tn_key),
		.hash_func = rte_hash_crc,
		.hash_func_init_val = 0,
		.socket_id = rte_socket_id(),
	};

	TAILQ_INIT(&l2_tn_info->l2_tn_list);
	snprintf(l2_tn_hash_name, RTE_HASH_NAMESIZE,
		 "l2_tn_%s", TDEV_NAME(eth_dev));
	l2_tn_info->hash_handle = rte_hash_create(&l2_tn_hash_params);
	if (!l2_tn_info->hash_handle) {
		PMD_INIT_LOG(ERR, "Failed to create L2 TN hash table!");
		return -EINVAL;
	}
	l2_tn_info->hash_map = rte_zmalloc("ngbe",
				   sizeof(struct ngbe_l2_tn_filter *) *
				   NGBE_MAX_L2_TN_FILTER_NUM,
				   0);
	if (!l2_tn_info->hash_map) {
		PMD_INIT_LOG(ERR,
			"Failed to allocate memory for L2 TN hash map!");
		return -ENOMEM;
	}
	l2_tn_info->e_tag_en = FALSE;
	l2_tn_info->e_tag_fwd_en = FALSE;
	l2_tn_info->e_tag_ether_type = ETHER_TYPE_ETAG;

	return 0;
}

static int
eth_ngbe_pci_probe(struct rte_pci_driver *pci_drv __rte_unused,
		struct rte_pci_device *pci_dev)
{
	char name[RTE_ETH_NAME_MAX_LEN];
	struct rte_eth_dev *pf_ethdev;
	struct rte_eth_devargs eth_da;
	int i, retval;

	if (pci_dev->device.devargs) {
		retval = rte_eth_devargs_parse(pci_dev->device.devargs->args,
				&eth_da);
		if (retval)
			return retval;
	} else
		memset(&eth_da, 0, sizeof(eth_da));

	retval = rte_eth_dev_create(&pci_dev->device, pci_dev->device.name,
		sizeof(struct ngbe_adapter),
		eth_dev_pci_specific_init, pci_dev,
		eth_ngbe_dev_init, NULL);

	if (retval || eth_da.nb_representor_ports < 1)
		return retval;

	pf_ethdev = rte_eth_dev_allocated(pci_dev->device.name);
	if (pf_ethdev == NULL)
		return -ENODEV;

	/* probe VF representor ports */
	for (i = 0; i < eth_da.nb_representor_ports; i++) {
		struct ngbe_vf_info *vfinfo;
		struct ngbe_vf_representor representor;

		vfinfo = *NGBE_DEV_VFDATA(pf_ethdev);
		if (vfinfo == NULL) {
			PMD_DRV_LOG(ERR,
				"no virtual functions supported by PF");
			break;
		}

		representor.vf_id = eth_da.representor_ports[i];
		representor.switch_domain_id = vfinfo->switch_domain_id;
		representor.pf_ethdev = pf_ethdev;

		/* representor port net_bdf_port */
		snprintf(name, sizeof(name), "net_%s_representor_%d",
			pci_dev->device.name,
			eth_da.representor_ports[i]);

		retval = rte_eth_dev_create(&pci_dev->device, name,
			sizeof(struct ngbe_vf_representor), NULL, NULL,
			ngbe_vf_representor_init, &representor);

		if (retval)
			PMD_DRV_LOG(ERR, "failed to create ngbe vf "
				"representor %s.", name);
	}

	return 0;
}

static int eth_ngbe_pci_remove(struct rte_pci_device *pci_dev)
{
	struct rte_eth_dev *ethdev;

	ethdev = rte_eth_dev_allocated(pci_dev->device.name);
	if (!ethdev)
		return -ENODEV;

	if (ethdev->data->dev_flags & RTE_ETH_DEV_REPRESENTOR)
		return rte_eth_dev_destroy(ethdev, ngbe_vf_representor_uninit);
	else
		return rte_eth_dev_destroy(ethdev, eth_ngbe_dev_uninit);
}

static struct rte_pci_driver rte_ngbe_pmd = {
	.id_table = pci_id_ngbe_map,
	.drv_flags = RTE_PCI_DRV_NEED_MAPPING |
		     RTE_PCI_DRV_INTR_LSC,
	.probe = eth_ngbe_pci_probe,
	.remove = eth_ngbe_pci_remove,
};

static int
ngbe_vlan_filter_set(struct rte_eth_dev *dev, uint16_t vlan_id, int on)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_vfta *shadow_vfta = NGBE_DEV_VFTA(dev);
	uint32_t vfta;
	uint32_t vid_idx;
	uint32_t vid_bit;

	vid_idx = (uint32_t) ((vlan_id >> 5) & 0x7F);
	vid_bit = (uint32_t) (1 << (vlan_id & 0x1F));
	vfta = rd32(hw, NGBE_VLANTBL(vid_idx));
	if (on)
		vfta |= vid_bit;
	else
		vfta &= ~vid_bit;
	wr32(hw, NGBE_VLANTBL(vid_idx), vfta);

	/* update local VFTA copy */
	shadow_vfta->vfta[vid_idx] = vfta;

	return 0;
}

static void
ngbe_vlan_strip_queue_set(struct rte_eth_dev *dev, uint16_t queue, int on)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_rx_queue *rxq;
	bool restart;
	uint32_t rxcfg, rxbal, rxbah;

	if (on)
		ngbe_vlan_hw_strip_enable(dev, queue);
	else
		ngbe_vlan_hw_strip_disable(dev, queue);

	rxq = dev->data->rx_queues[queue];
	rxbal = rd32(hw, NGBE_RXBAL(rxq->reg_idx));
	rxbah = rd32(hw, NGBE_RXBAH(rxq->reg_idx));
	rxcfg = rd32(hw, NGBE_RXCFG(rxq->reg_idx));
	if (rxq->offloads & DEV_RX_OFFLOAD_VLAN_STRIP) {
		restart = (rxcfg & NGBE_RXCFG_ENA) &&
			!(rxcfg & NGBE_RXCFG_VLAN);
		rxcfg |= NGBE_RXCFG_VLAN;
	} else {
		restart = (rxcfg & NGBE_RXCFG_ENA) &&
			(rxcfg & NGBE_RXCFG_VLAN);
		rxcfg &= ~NGBE_RXCFG_VLAN;
	}
	rxcfg &= ~NGBE_RXCFG_ENA;

	if (restart) {
		/* set vlan strip for ring */
		ngbe_dev_rx_queue_stop(dev, queue);
		wr32(hw, NGBE_RXBAL(rxq->reg_idx), rxbal);
		wr32(hw, NGBE_RXBAH(rxq->reg_idx), rxbah);
		wr32(hw, NGBE_RXCFG(rxq->reg_idx), rxcfg);
		ngbe_dev_rx_queue_start(dev, queue);
	}
}

static int
ngbe_vlan_tpid_set(struct rte_eth_dev *dev,
		    enum rte_vlan_type vlan_type,
		    uint16_t tpid)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	int ret = 0;
	uint32_t portctrl, vlan_ext, qinq;

	portctrl = rd32(hw, NGBE_PORTCTL);

	vlan_ext = (portctrl & NGBE_PORTCTL_VLANEXT);
	qinq = vlan_ext && (portctrl & NGBE_PORTCTL_QINQ);
	switch (vlan_type) {
	case ETH_VLAN_TYPE_INNER:
		if (vlan_ext) {
			wr32m(hw, NGBE_VLANCTL,
				NGBE_VLANCTL_TPID_MASK,
				NGBE_VLANCTL_TPID(tpid));
			wr32m(hw, NGBE_DMATXCTRL,
				NGBE_DMATXCTRL_TPID_MASK,
				NGBE_DMATXCTRL_TPID(tpid));
		} else {
			ret = -ENOTSUP;
			PMD_DRV_LOG(ERR, "Inner type is not supported"
				    " by single VLAN");
		}

		if (qinq) {
			wr32m(hw, NGBE_TAGTPID(0),
				NGBE_TAGTPID_LSB_MASK,
				NGBE_TAGTPID_LSB(tpid));
		}
		break;
	case ETH_VLAN_TYPE_OUTER:
		if (vlan_ext) {
			/* Only the high 16-bits is valid */
			wr32m(hw, NGBE_EXTAG,
				NGBE_EXTAG_VLAN_MASK,
				NGBE_EXTAG_VLAN(tpid));
		} else {
			wr32m(hw, NGBE_VLANCTL,
				NGBE_VLANCTL_TPID_MASK,
				NGBE_VLANCTL_TPID(tpid));
			wr32m(hw, NGBE_DMATXCTRL,
				NGBE_DMATXCTRL_TPID_MASK,
				NGBE_DMATXCTRL_TPID(tpid));
		}

		if (qinq) {
			wr32m(hw, NGBE_TAGTPID(0),
				NGBE_TAGTPID_MSB_MASK,
				NGBE_TAGTPID_MSB(tpid));
		}
		break;
	default:
		PMD_DRV_LOG(ERR, "Unsupported VLAN type %d", vlan_type);
		return -EINVAL;
	}

	return ret;
}

void
ngbe_vlan_hw_filter_disable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t vlnctrl;

	PMD_INIT_FUNC_TRACE();

	/* Filter Table Disable */
	vlnctrl = rd32(hw, NGBE_VLANCTL);
	vlnctrl &= ~NGBE_VLANCTL_VFE;
	wr32(hw, NGBE_VLANCTL, vlnctrl);
}

void
ngbe_vlan_hw_filter_enable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_vfta *shadow_vfta = NGBE_DEV_VFTA(dev);
	uint32_t vlnctrl;
	uint16_t i;

	PMD_INIT_FUNC_TRACE();

	/* Filter Table Enable */
	vlnctrl = rd32(hw, NGBE_VLANCTL);
	vlnctrl &= ~NGBE_VLANCTL_CFIENA;
	vlnctrl |= NGBE_VLANCTL_VFE;
	wr32(hw, NGBE_VLANCTL, vlnctrl);

	/* write whatever is in local vfta copy */
	for (i = 0; i < NGBE_VFTA_SIZE; i++)
		wr32(hw, NGBE_VLANTBL(i), shadow_vfta->vfta[i]);
}

void
ngbe_vlan_hw_strip_bitmap_set(struct rte_eth_dev *dev, uint16_t queue, bool on)
{
	struct ngbe_hwstrip *hwstrip = NGBE_DEV_HWSTRIP(dev);
	struct ngbe_rx_queue *rxq;

	if (queue >= NGBE_MAX_RX_QUEUE_NUM)
		return;

	if (on)
		NGBE_SET_HWSTRIP(hwstrip, queue);
	else
		NGBE_CLEAR_HWSTRIP(hwstrip, queue);

	if (queue >= dev->data->nb_rx_queues)
		return;

	rxq = dev->data->rx_queues[queue];

	if (on) {
		rxq->vlan_flags = PKT_RX_VLAN | PKT_RX_VLAN_STRIPPED;
		rxq->offloads |= DEV_RX_OFFLOAD_VLAN_STRIP;
	} else {
		rxq->vlan_flags = PKT_RX_VLAN;
		rxq->offloads &= ~DEV_RX_OFFLOAD_VLAN_STRIP;
	}
}

static void
ngbe_vlan_hw_strip_disable(struct rte_eth_dev *dev, uint16_t queue)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t ctrl;

	PMD_INIT_FUNC_TRACE();

	/* Other 10G NIC, the VLAN strip can be setup per queue in RXDCTL */
	ctrl = rd32(hw, NGBE_RXCFG(queue));
	ctrl &= ~NGBE_RXCFG_VLAN;
	wr32(hw, NGBE_RXCFG(queue), ctrl);

	/* record those setting for HW strip per queue */
	ngbe_vlan_hw_strip_bitmap_set(dev, queue, 0);
}

static void
ngbe_vlan_hw_strip_enable(struct rte_eth_dev *dev, uint16_t queue)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t ctrl;

	PMD_INIT_FUNC_TRACE();

	/* Other 10G NIC, the VLAN strip can be setup per queue in RXDCTL */
	ctrl = rd32(hw, NGBE_RXCFG(queue));
	ctrl |= NGBE_RXCFG_VLAN;
	wr32(hw, NGBE_RXCFG(queue), ctrl);

	/* record those setting for HW strip per queue */
	ngbe_vlan_hw_strip_bitmap_set(dev, queue, 1);
}

static void
ngbe_vlan_hw_extend_disable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t ctrl;

	PMD_INIT_FUNC_TRACE();

	/* DMATXCTRL: Geric Double VLAN Disable */
	ctrl = rd32(hw, NGBE_PORTCTL);
	ctrl &= ~NGBE_PORTCTL_VLANEXT;
	ctrl &= ~NGBE_PORTCTL_QINQ;
	wr32(hw, NGBE_PORTCTL, ctrl);

	/* CTRL_EXT: Global Double VLAN Disable */
}

static void
ngbe_vlan_hw_extend_enable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
//	struct rte_eth_rxmode *rxmode = &dev->data->dev_conf.rxmode;
//	struct rte_eth_txmode *txmode = &dev->data->dev_conf.txmode;
	uint32_t ctrl;

	PMD_INIT_FUNC_TRACE();

	/* DMATXCTRL: Geric Double VLAN Enable */
	ctrl  = rd32(hw, NGBE_PORTCTL);
	ctrl |= NGBE_PORTCTL_VLANEXT | NGBE_PORTCTL_QINQ;
//	if (rxmode->offloads & DEV_RX_OFFLOAD_QINQ_STRIP ||
//	    txmode->offloads & DEV_TX_OFFLOAD_QINQ_INSERT)
//		ctrl |= NGBE_PORTCTL_QINQ;
	wr32(hw, NGBE_PORTCTL, ctrl);

	/* CTRL_EXT: Global Double VLAN Enable */

	/* Clear pooling mode of PFVTCTL. */

	/*
	 * VET EXT field in the EXVET register = 0x8100 by default
	 * So no need to change. Same to VT field of DMATXCTL register
	 */
}

void
ngbe_vlan_hw_strip_config(struct rte_eth_dev *dev)
{
	struct ngbe_rx_queue *rxq;
	uint16_t i;

	PMD_INIT_FUNC_TRACE();

	/*
	 * Other 10G NIC, the VLAN strip can be setup
	 * per queue in RXDCTL
	 */
	for (i = 0; i < dev->data->nb_rx_queues; i++) {
		rxq = dev->data->rx_queues[i];

		if (rxq->offloads & DEV_RX_OFFLOAD_VLAN_STRIP) {
			ngbe_vlan_hw_strip_enable(dev, i);
		} else {
			ngbe_vlan_hw_strip_disable(dev, i);
		}
	}
}

void
ngbe_config_vlan_strip_on_all_queues(struct rte_eth_dev *dev, int mask)
{
	uint16_t i;
	struct rte_eth_rxmode *rxmode;
	struct ngbe_rx_queue *rxq;

	if (mask & ETH_VLAN_STRIP_MASK) {
		rxmode = &dev->data->dev_conf.rxmode;
		if (rxmode->offloads & DEV_RX_OFFLOAD_VLAN_STRIP)
			for (i = 0; i < dev->data->nb_rx_queues; i++) {
				rxq = dev->data->rx_queues[i];
				rxq->offloads |= DEV_RX_OFFLOAD_VLAN_STRIP;
			}
		else
			for (i = 0; i < dev->data->nb_rx_queues; i++) {
				rxq = dev->data->rx_queues[i];
				rxq->offloads &= ~DEV_RX_OFFLOAD_VLAN_STRIP;
			}
	}
}

static int
ngbe_vlan_offload_config(struct rte_eth_dev *dev, int mask)
{
	struct rte_eth_rxmode *rxmode;
	rxmode = &dev->data->dev_conf.rxmode;

	if (mask & ETH_VLAN_STRIP_MASK) {
		ngbe_vlan_hw_strip_config(dev);
	}

	if (mask & ETH_VLAN_FILTER_MASK) {
		if (rxmode->offloads & DEV_RX_OFFLOAD_VLAN_FILTER)
			ngbe_vlan_hw_filter_enable(dev);
		else
			ngbe_vlan_hw_filter_disable(dev);
	}

	if (mask & (ETH_VLAN_EXTEND_MASK | ETH_QINQ_STRIP_MASK)) {
		if (rxmode->offloads & DEV_RX_OFFLOAD_VLAN_EXTEND ||
			rxmode->offloads & DEV_RX_OFFLOAD_QINQ_STRIP)
			ngbe_vlan_hw_extend_enable(dev);
		else
			ngbe_vlan_hw_extend_disable(dev);
	}

	return 0;
}

static int
ngbe_vlan_offload_set(struct rte_eth_dev *dev, int mask)
{
	ngbe_config_vlan_strip_on_all_queues(dev, mask);

	ngbe_vlan_offload_config(dev, mask);

	return 0;
}

static void
ngbe_vmdq_vlan_hw_filter_enable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	/* VLNCTRL: enable vlan filtering and allow all vlan tags through */
	uint32_t vlanctrl = rd32(hw, NGBE_VLANCTL);

	vlanctrl |= NGBE_VLANCTL_VFE; /* enable vlan filters */
	wr32(hw, NGBE_VLANCTL, vlanctrl);
}

static int
ngbe_check_vf_rss_rxq_num(struct rte_eth_dev *dev, uint16_t nb_rx_q)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	switch (nb_rx_q) {
	case 1:
	case 2:
		RTE_ETH_DEV_SRIOV(dev).active = ETH_64_POOLS;
		break;
	case 4:
		RTE_ETH_DEV_SRIOV(dev).active = ETH_32_POOLS;
		break;
	default:
		return -EINVAL;
	}

	RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool =
		NGBE_MAX_RX_QUEUE_NUM / RTE_ETH_DEV_SRIOV(dev).active;
	RTE_ETH_DEV_SRIOV(dev).def_pool_q_idx =
		pci_dev->max_vfs * RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool;
	return 0;
}

static int
ngbe_check_mq_mode(struct rte_eth_dev *dev)
{
	struct rte_eth_conf *dev_conf = &dev->data->dev_conf;
	uint16_t nb_rx_q = dev->data->nb_rx_queues;
	uint16_t nb_tx_q = dev->data->nb_tx_queues;

	if (RTE_ETH_DEV_SRIOV(dev).active != 0) {
		/* check multi-queue mode */
		switch (dev_conf->rxmode.mq_mode) {
		case ETH_MQ_RX_RSS:
		case ETH_MQ_RX_VMDQ_RSS:
			dev->data->dev_conf.rxmode.mq_mode = ETH_MQ_RX_VMDQ_RSS;
			if (nb_rx_q <= RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool)
				if (ngbe_check_vf_rss_rxq_num(dev, nb_rx_q)) {
					PMD_INIT_LOG(ERR, "SRIOV is active,"
						" invalid queue number"
						" for VMDQ RSS, allowed"
						" value are 1, 2 or 4.");
					return -EINVAL;
				}
			break;
		case ETH_MQ_RX_VMDQ_ONLY:
		case ETH_MQ_RX_NONE:
			/* if nothing mq mode configure, use default scheme */
			dev->data->dev_conf.rxmode.mq_mode = ETH_MQ_RX_VMDQ_ONLY;
			break;
		default:
			/* SRIOV only works in VMDq enable mode */
			PMD_INIT_LOG(ERR, "SRIOV is active,"
					" wrong mq_mode rx %d.",
					dev_conf->rxmode.mq_mode);
			return -EINVAL;
		}

		switch (dev_conf->txmode.mq_mode) {
		default: /* ETH_MQ_TX_VMDQ_ONLY or ETH_MQ_TX_NONE */
			dev->data->dev_conf.txmode.mq_mode = ETH_MQ_TX_VMDQ_ONLY;
			break;
		}

		/* check valid queue number */
		if ((nb_rx_q > RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool) ||
		    (nb_tx_q > RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool)) {
			PMD_INIT_LOG(ERR, "SRIOV is active,"
					" nb_rx_q=%d nb_tx_q=%d queue number"
					" must be less than or equal to %d.",
					nb_rx_q, nb_tx_q,
					RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool);
			return -EINVAL;
		}
	}

	return 0;
}

static int
ngbe_dev_configure(struct rte_eth_dev *dev)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);
	int ret;

	PMD_INIT_FUNC_TRACE();
	/* multipe queue mode checking */
	ret  = ngbe_check_mq_mode(dev);
	if (ret != 0) {
		PMD_DRV_LOG(ERR, "ngbe_check_mq_mode fails with %d.",
			    ret);
		return ret;
	}

	/* set flag to update link status after init */
	intr->flags |= NGBE_FLAG_NEED_LINK_UPDATE;

	/*
	 * Initialize to TRUE. If any of Rx queues doesn't meet the bulk
	 * allocation or vector Rx preconditions we will reset it.
	 */
	adapter->rx_bulk_alloc_allowed = true;
	adapter->rx_vec_allowed = true;

	return 0;
}

static void
ngbe_dev_phy_intr_setup(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);

	wr32(hw, NGBE_GPIODIR, NGBE_GPIODIR_DDR(1));
	wr32(hw, NGBE_GPIOINTEN, NGBE_GPIOINTEN_INT(3));
	wr32(hw, NGBE_GPIOINTTYPE, NGBE_GPIOINTTYPE_LEVEL(0));
	if (hw->phy.type == ngbe_phy_yt8521s_sfi)
		wr32(hw, NGBE_GPIOINTPOL, NGBE_GPIOINTPOL_ACT(0));
	else
		wr32(hw, NGBE_GPIOINTPOL, NGBE_GPIOINTPOL_ACT(3));

	intr->mask_misc |= NGBE_ICRMISC_GPIO;
}

int
ngbe_set_vf_rate_limit(struct rte_eth_dev *dev, uint16_t vf,
			uint16_t tx_rate, uint64_t q_msk)
{
	struct ngbe_hw *hw;
	struct ngbe_vf_info *vfinfo;
	struct rte_eth_link link;
	uint8_t  nb_q_per_pool;
	uint32_t queue_stride;
	uint32_t queue_idx, idx = 0, vf_idx;
	uint32_t queue_end;
	uint16_t total_rate = 0;
	struct rte_pci_device *pci_dev;

	pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	rte_eth_link_get_nowait(dev->data->port_id, &link);

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (tx_rate > link.link_speed)
		return -EINVAL;

	if (q_msk == 0)
		return 0;

	hw = NGBE_DEV_HW(dev);
	vfinfo = *(NGBE_DEV_VFDATA(dev));
	nb_q_per_pool = RTE_ETH_DEV_SRIOV(dev).nb_q_per_pool;
	queue_stride = NGBE_MAX_RX_QUEUE_NUM / RTE_ETH_DEV_SRIOV(dev).active;
	queue_idx = vf * queue_stride;
	queue_end = queue_idx + nb_q_per_pool - 1;
	if (queue_end >= hw->mac.max_tx_queues)
		return -EINVAL;

	if (vfinfo) {
		for (vf_idx = 0; vf_idx < pci_dev->max_vfs; vf_idx++) {
			if (vf_idx == vf)
				continue;
			for (idx = 0; idx < RTE_DIM(vfinfo[vf_idx].tx_rate);
				idx++)
				total_rate += vfinfo[vf_idx].tx_rate[idx];
		}
	} else {
		return -EINVAL;
	}

	/* Store tx_rate for this vf. */
	for (idx = 0; idx < nb_q_per_pool; idx++) {
		if (((uint64_t)0x1 << idx) & q_msk) {
			if (vfinfo[vf].tx_rate[idx] != tx_rate)
				vfinfo[vf].tx_rate[idx] = tx_rate;
			total_rate += tx_rate;
		}
	}

	if (total_rate > dev->data->dev_link.link_speed) {
		/* Reset stored TX rate of the VF if it causes exceed
		 * link speed.
		 */
		memset(vfinfo[vf].tx_rate, 0, sizeof(vfinfo[vf].tx_rate));
		return -EINVAL;
	}

	/* Set RTTBCNRC of each queue/pool for vf X  */
	for (; queue_idx <= queue_end; queue_idx++) {
		if (0x1 & q_msk)
			ngbe_set_queue_rate_limit(dev, queue_idx, tx_rate);
		q_msk = q_msk >> 1;
	}

	return 0;
}

/*
 * Configure device link speed and setup link.
 * It returns 0 on success.
 */
static int
ngbe_dev_start(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(dev);
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	struct rte_intr_handle *intr_handle = &pci_dev->intr_handle;
	uint32_t intr_vector = 0;
	int err, link_up = 0;
	bool negotiate = false;
	uint32_t speed = 0;
	uint32_t allowed_speeds;
	int mask = 0;
	int status;
	uint16_t vf, idx;
	uint32_t *link_speeds;
	struct ngbe_tm_conf *tm_conf = NGBE_DEV_TM_CONF(dev);

	PMD_INIT_FUNC_TRACE();

	/* Stop the link setup handler before resetting the HW. */
	rte_eal_alarm_cancel(ngbe_dev_setup_link_alarm_handler, dev);

	/* disable uio/vfio intr/eventfd mapping */
	rte_intr_disable(intr_handle);

	/* stop adapter */
	hw->adapter_stopped = 0;

	/* reinitialize adapter
	 * this calls reset and start
	 */
	hw->nb_rx_queues = dev->data->nb_rx_queues;
	hw->nb_tx_queues = dev->data->nb_tx_queues;
	status = ngbe_pf_reset_hw(hw);
	if (status != 0)
		return -1;
	hw->mac.start_hw(hw);
	hw->mac.get_link_status = true;

	ngbe_set_pcie_master(hw, 1);

	/* configure PF module if SRIOV enabled */
	ngbe_pf_host_configure(dev);

	ngbe_dev_phy_intr_setup(dev);

	/* check and configure queue intr-vector mapping */
	if ((rte_intr_cap_multiple(intr_handle) ||
	     !RTE_ETH_DEV_SRIOV(dev).active) &&
	    dev->data->dev_conf.intr_conf.rxq != 0) {
		intr_vector = dev->data->nb_rx_queues;
		if (rte_intr_efd_enable(intr_handle, intr_vector))
			return -1;
	}

	if (rte_intr_dp_is_en(intr_handle) && !intr_handle->intr_vec) {
		intr_handle->intr_vec =
			rte_zmalloc("intr_vec",
				    dev->data->nb_rx_queues * sizeof(int), 0);
		if (intr_handle->intr_vec == NULL) {
			PMD_INIT_LOG(ERR, "Failed to allocate %d rx_queues"
				     " intr_vec", dev->data->nb_rx_queues);
			return -ENOMEM;
		}
	}

	/* confiugre msix for sleep until rx interrupt */
	ngbe_configure_msix(dev);

	/* initialize transmission unit */
	ngbe_dev_tx_init(dev);

	/* This can fail when allocating mbufs for descriptor rings */
	err = ngbe_dev_rx_init(dev);
	if (err) {
		PMD_INIT_LOG(ERR, "Unable to initialize RX hardware");
		goto error;
	}

	mask = ETH_VLAN_STRIP_MASK | ETH_VLAN_FILTER_MASK |
		ETH_VLAN_EXTEND_MASK;
	err = ngbe_vlan_offload_config(dev, mask);
	if (err) {
		PMD_INIT_LOG(ERR, "Unable to set VLAN offload");
		goto error;
	}

	if (dev->data->dev_conf.rxmode.mq_mode == ETH_MQ_RX_VMDQ_ONLY) {
		/* Enable vlan filtering for VMDq */
		ngbe_vmdq_vlan_hw_filter_enable(dev);
	}

	/* Configure DCB hw */
	ngbe_configure_pb(dev);
	ngbe_configure_port(dev);

	/* Restore vf rate limit */
	if (vfinfo != NULL) {
		for (vf = 0; vf < pci_dev->max_vfs; vf++)
			for (idx = 0; idx < NGBE_MAX_QUEUE_NUM_PER_VF; idx++)
				if (vfinfo[vf].tx_rate[idx] != 0)
					ngbe_set_vf_rate_limit(
						dev, vf,
						vfinfo[vf].tx_rate[idx],
						1 << idx);
	}

	err = ngbe_dev_rxtx_start(dev);
	if (err < 0) {
		PMD_INIT_LOG(ERR, "Unable to start rxtx queues");
		goto error;
	}

	/* Skip link setup if loopback mode is enabled. */
	if (hw->is_pf && dev->data->dev_conf.lpbk_mode)
		goto skip_link_setup;

	if (ngbe_is_sfp(hw) && hw->phy.multispeed_fiber) {
		err = hw->mac.setup_sfp(hw);
		if (err)
			goto error;
	}

	if (hw->phy.media_type == ngbe_media_type_copper) {
		/* Turn on the copper */
		hw->phy.set_phy_power(hw, true);
	} else {
		/* Turn on the laser */
		hw->mac.enable_tx_laser(hw);
	}

	err = hw->mac.check_link(hw, &speed, &link_up, 0);
	if (err)
		goto error;
	dev->data->dev_link.link_status = link_up;

	link_speeds = &dev->data->dev_conf.link_speeds;
	if (*link_speeds == ETH_LINK_SPEED_AUTONEG)
		negotiate = true;

	err = hw->mac.get_link_capabilities(hw, &speed, &negotiate);
	if (err)
		goto error;

	allowed_speeds = 0;
	if (hw->mac.default_speeds & NGBE_LINK_SPEED_10GB_FULL)
		allowed_speeds |= ETH_LINK_SPEED_10G;
	if (hw->mac.default_speeds & NGBE_LINK_SPEED_5GB_FULL)
		allowed_speeds |= ETH_LINK_SPEED_5G;
	if (hw->mac.default_speeds & NGBE_LINK_SPEED_2_5GB_FULL)
		allowed_speeds |= ETH_LINK_SPEED_2_5G;
	if (hw->mac.default_speeds & NGBE_LINK_SPEED_1GB_FULL)
		allowed_speeds |= ETH_LINK_SPEED_1G;
	if (hw->mac.default_speeds & NGBE_LINK_SPEED_100M_FULL)
		allowed_speeds |= ETH_LINK_SPEED_100M;
	if (hw->mac.default_speeds & NGBE_LINK_SPEED_10M_FULL)
		allowed_speeds |= ETH_LINK_SPEED_10M;

	if (*link_speeds & ~allowed_speeds) {
		PMD_INIT_LOG(ERR, "Invalid link setting");
		goto error;
	}

	speed = 0x0;
	if (*link_speeds == ETH_LINK_SPEED_AUTONEG) {
		speed = hw->mac.default_speeds;
	} else {
		if (*link_speeds & ETH_LINK_SPEED_10G)
			speed |= NGBE_LINK_SPEED_10GB_FULL;
		if (*link_speeds & ETH_LINK_SPEED_5G)
			speed |= NGBE_LINK_SPEED_5GB_FULL;
		if (*link_speeds & ETH_LINK_SPEED_2_5G)
			speed |= NGBE_LINK_SPEED_2_5GB_FULL;
		if (*link_speeds & ETH_LINK_SPEED_1G)
			speed |= NGBE_LINK_SPEED_1GB_FULL;
		if (*link_speeds & ETH_LINK_SPEED_100M)
			speed |= NGBE_LINK_SPEED_100M_FULL;
		if (*link_speeds & ETH_LINK_SPEED_10M)
			speed |= NGBE_LINK_SPEED_10M_FULL;
	}

	err = hw->phy.init_hw(hw);
	if (err) {
		PMD_INIT_LOG(ERR, "PHY init failed");
		goto error;
	}
	err = hw->mac.setup_link(hw, speed, link_up);
	if (err)
		goto error;

skip_link_setup:

	if (rte_intr_allow_others(intr_handle)) {
		ngbe_dev_misc_interrupt_setup(dev);
		/* check if lsc interrupt is enabled */
		if (dev->data->dev_conf.intr_conf.lsc != 0)
			ngbe_dev_lsc_interrupt_setup(dev, TRUE);
		else
			ngbe_dev_lsc_interrupt_setup(dev, FALSE);
		ngbe_dev_macsec_interrupt_setup(dev);
		ngbe_set_ivar_map(hw, -1, 1, NGBE_MISC_VEC_ID);
	} else {
		rte_intr_callback_unregister(intr_handle,
					     ngbe_dev_interrupt_handler, dev);
		if (dev->data->dev_conf.intr_conf.lsc != 0)
			PMD_INIT_LOG(INFO, "lsc won't enable because of"
				     " no intr multiplex");
	}

	/* check if rxq interrupt is enabled */
	if (dev->data->dev_conf.intr_conf.rxq != 0 &&
	    rte_intr_dp_is_en(intr_handle))
		ngbe_dev_rxq_interrupt_setup(dev);

	/* enable uio/vfio intr/eventfd mapping */
	rte_intr_enable(intr_handle);

	/* resume enabled intr since hw reset */
	ngbe_enable_intr(dev);
	ngbe_l2_tunnel_conf(dev);
	ngbe_filter_restore(dev);

	if (hw->gpio_ctl == 1) {
		/* gpio0 is used to power on/off control*/
		wr32(hw, NGBE_GPIODATA, 0);
	}

	if (tm_conf->root && !tm_conf->committed)
		PMD_DRV_LOG(WARNING,
			    "please call hierarchy_commit() "
			    "before starting the port");

	/*
	 * Update link status right before return, because it may
	 * start link configuration process in a separate thread.
	 */
	ngbe_dev_link_update(dev, 0);
#ifdef RTE_NGBE_POLL
	rte_eal_alarm_set(100, ngbe_check_link_alarm, dev);
#endif

	ngbe_read_stats_registers(hw, hw_stats);
	hw->offset_loaded = 1;

	return 0;

error:
	PMD_INIT_LOG(ERR, "failure in ngbe_dev_start(): %d", err);
	ngbe_dev_clear_queues(dev);
	return -EIO;
}

/*
 * Stop device: disable rx and tx functions to allow for reconfiguring.
 */
static void
ngbe_dev_stop(struct rte_eth_dev *dev)
{
	struct rte_eth_link link;
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(dev);
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	struct rte_intr_handle *intr_handle = &pci_dev->intr_handle;
	int vf;
	struct ngbe_tm_conf *tm_conf = NGBE_DEV_TM_CONF(dev);

	PMD_INIT_FUNC_TRACE();

	rte_eal_alarm_cancel(ngbe_dev_setup_link_alarm_handler, dev);
#ifdef RTE_NGBE_POLL
	rte_eal_alarm_cancel(ngbe_check_link_alarm, dev);
#endif

	if (hw->gpio_ctl == 1) {
		/* gpio0 is used to power on/off control*/
		wr32(hw, NGBE_GPIODATA, NGBE_GPIOBIT_0);
	}

	/* disable interrupts */
	ngbe_disable_intr(hw);

	/* reset the NIC */
	ngbe_pf_reset_hw(hw);
	hw->adapter_stopped = 0;

	/* stop adapter */
	ngbe_stop_hw(hw);

	for (vf = 0; vfinfo != NULL && vf < pci_dev->max_vfs; vf++)
		vfinfo[vf].clear_to_send = false;

	if (hw->phy.media_type == ngbe_media_type_copper) {
		/* Turn off the copper */
		hw->phy.set_phy_power(hw, false);
	} else {
		/* Turn off the laser */
		hw->mac.disable_tx_laser(hw);
	}

	ngbe_dev_clear_queues(dev);

	/* Clear stored conf */
	dev->data->scattered_rx = 0;
	dev->data->lro = 0;

	/* Clear recorded link status */
	memset(&link, 0, sizeof(link));
	rte_eth_linkstatus_set(dev, &link);

	if (!rte_intr_allow_others(intr_handle))
		/* resume to the default handler */
		rte_intr_callback_register(intr_handle,
					   ngbe_dev_interrupt_handler,
					   (void *)dev);

	/* Clean datapath event and queue/vec mapping */
	rte_intr_efd_disable(intr_handle);
	if (intr_handle->intr_vec != NULL) {
		rte_free(intr_handle->intr_vec);
		intr_handle->intr_vec = NULL;
	}

	ngbe_set_pcie_master(hw, 1);

	/* reset hierarchy commit */
	tm_conf->committed = false;

	adapter->rss_reta_updated = 0;
}

/*
 * Set device link up: enable tx.
 */
static int
ngbe_dev_set_link_up(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	if (hw->phy.media_type == ngbe_media_type_copper) {
		/* Turn on the copper */
		hw->phy.set_phy_power(hw, true);
	} else {
		/* Turn on the laser */
		hw->mac.enable_tx_laser(hw);
	}

	return 0;
}

/*
 * Set device link down: disable tx.
 */
static int
ngbe_dev_set_link_down(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	if (hw->phy.media_type == ngbe_media_type_copper) {
		/* Turn off the copper */
		hw->phy.set_phy_power(hw, false);
	} else {
		/* Turn off the laser */
		hw->mac.disable_tx_laser(hw);
	}

	return 0;
}

/*
 * Reset and stop device.
 */
static void
ngbe_dev_close(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	PMD_INIT_FUNC_TRACE();

	ngbe_pf_reset_hw(hw);

	ngbe_dev_stop(dev);
	hw->adapter_stopped = 1;

	ngbe_dev_free_queues(dev);

	ngbe_set_pcie_master(hw, 0);

	/* reprogram the RAR[0] in case user changed it. */
	ngbe_set_rar(hw, 0, hw->mac.addr, 0, true);
}

#if RTE_VERSION >= RTE_VERSION_NUM(17, 11, 0, 0)
/*
 * Reset PF device.
 */
static int
ngbe_dev_reset(struct rte_eth_dev *dev)
{
	int ret;

	/* When a DPDK PMD PF begin to reset PF port, it should notify all
	 * its VF to make them align with it. The detailed notification
	 * mechanism is PMD specific. As to ngbe PF, it is rather complex.
	 * To avoid unexpected behavior in VF, currently reset of PF with
	 * SR-IOV activation is not supported. It might be supported later.
	 */
	if (dev->data->sriov.active)
		return -ENOTSUP;

	ret = eth_ngbe_dev_uninit(dev);
	if (ret)
		return ret;

	ret = eth_ngbe_dev_init(dev, NULL);

	return ret;
}
#endif /* RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0) */

#define UPDATE_QP_COUNTER_32bit(reg, last_counter, counter)     \
	{                                                       \
		uint32_t current_counter = rd32(hw, reg);  \
		if (current_counter < last_counter)             \
			current_counter += 0x100000000LL;               \
		if (!hw->offset_loaded)                       \
			last_counter = current_counter;                          \
		counter = current_counter - last_counter;                              \
		counter &= 0xFFFFFFFFLL;                              \
	}

#define UPDATE_QP_COUNTER_36bit(reg_lsb, reg_msb, last_counter, counter) \
	{                                                                \
		uint64_t current_counter_lsb = rd32(hw, reg_lsb);   \
		uint64_t current_counter_msb = rd32(hw, reg_msb);   \
		uint64_t current_counter = (current_counter_msb << 32) |      \
			current_counter_lsb;                             \
		if (current_counter < last_counter)                      \
			current_counter += 0x1000000000LL;                       \
		if (!hw->offset_loaded)                       \
			last_counter = current_counter;                          \
		counter = current_counter - last_counter;                              \
		counter &= 0xFFFFFFFFFLL;                              \
	}

void
ngbe_read_stats_registers(struct ngbe_hw *hw,
			   struct ngbe_hw_stats *hw_stats)
{
	unsigned i;

	/* QP Stats */
	for (i = 0; i < hw->nb_rx_queues; i++) {
		UPDATE_QP_COUNTER_32bit(NGBE_QPRXPKT(i),
			hw->qp_last[i].rx_qp_packets,
			hw_stats->qp[i].rx_qp_packets);
		UPDATE_QP_COUNTER_36bit(NGBE_QPRXOCTL(i), NGBE_QPRXOCTH(i),
			hw->qp_last[i].rx_qp_bytes,
			hw_stats->qp[i].rx_qp_bytes);
		UPDATE_QP_COUNTER_32bit(NGBE_QPRXMPKT(i),
			hw->qp_last[i].rx_qp_mc_packets,
			hw_stats->qp[i].rx_qp_mc_packets);
		UPDATE_QP_COUNTER_32bit(NGBE_QPRXBPKT(i),
			hw->qp_last[i].rx_qp_bc_packets,
			hw_stats->qp[i].rx_qp_bc_packets);
	}
	for (i = 0; i < hw->nb_tx_queues; i++) {
		UPDATE_QP_COUNTER_32bit(NGBE_QPTXPKT(i),
			hw->qp_last[i].tx_qp_packets,
			hw_stats->qp[i].tx_qp_packets);
		UPDATE_QP_COUNTER_36bit(NGBE_QPTXOCTL(i), NGBE_QPTXOCTH(i),
			hw->qp_last[i].tx_qp_bytes,
			hw_stats->qp[i].tx_qp_bytes);
		UPDATE_QP_COUNTER_32bit(NGBE_QPTXMPKT(i),
			hw->qp_last[i].tx_qp_mc_packets,
			hw_stats->qp[i].tx_qp_mc_packets);
		UPDATE_QP_COUNTER_32bit(NGBE_QPTXBPKT(i),
			hw->qp_last[i].tx_qp_bc_packets,
			hw_stats->qp[i].tx_qp_bc_packets);
	}

	/* PB Stats */
	hw_stats->rx_up_dropped += rd32(hw, NGBE_PBRXMISS);	
	hw_stats->rdb_pkt_cnt += rd32(hw, NGBE_PBRXPKT);
	hw_stats->rdb_repli_cnt += rd32(hw, NGBE_PBRXREP);
	hw_stats->rdb_drp_cnt += rd32(hw, NGBE_PBRXDROP);
	hw_stats->tx_xoff_packets += rd32(hw, NGBE_PBTXLNKXOFF);
	hw_stats->tx_xon_packets += rd32(hw, NGBE_PBTXLNKXON);

	hw_stats->rx_xon_packets += rd32(hw, NGBE_PBRXLNKXON);
	hw_stats->rx_xoff_packets += rd32(hw, NGBE_PBRXLNKXOFF);

	/* DMA Stats */
	hw_stats->rx_drop_packets += rd32(hw, NGBE_DMARXDROP);
	hw_stats->tx_drop_packets += rd32(hw, NGBE_DMATXDROP);
	hw_stats->rx_dma_drop += rd32(hw, NGBE_DMARXDROP);
	hw_stats->tx_secdrp_packets += rd32(hw, NGBE_DMATXSECDROP);
	hw_stats->rx_packets += rd32(hw, NGBE_DMARXPKT);
	hw_stats->tx_packets += rd32(hw, NGBE_DMATXPKT);
	hw_stats->rx_bytes += rd64(hw, NGBE_DMARXOCTL);
	hw_stats->tx_bytes += rd64(hw, NGBE_DMATXOCTL);

	/* MAC Stats */
	hw_stats->rx_crc_errors += rd64(hw, NGBE_MACRXERRCRCL);
	hw_stats->rx_multicast_packets += rd64(hw, NGBE_MACRXMPKTL);
	hw_stats->tx_multicast_packets += rd64(hw, NGBE_MACTXMPKTL);

	hw_stats->rx_total_packets += rd64(hw, NGBE_MACRXPKTL);
	hw_stats->tx_total_packets += rd64(hw, NGBE_MACTXPKTL);
	//hw_stats->rx_length_errors += rd32(hw, NGBE_MACRXOVERSIZE);
	hw_stats->rx_total_bytes += rd64(hw, NGBE_MACRXGBOCTL);

	hw_stats->rx_broadcast_packets += rd64(hw, NGBE_MACRXOCTL);
	hw_stats->tx_broadcast_packets += rd32(hw, NGBE_MACTXOCTL);
	
	hw_stats->rx_size_64_packets += rd64(hw, NGBE_MACRX1to64L);
	hw_stats->rx_size_65_to_127_packets += rd64(hw, NGBE_MACRX65to127L);
	hw_stats->rx_size_128_to_255_packets += rd64(hw, NGBE_MACRX128to255L);
	hw_stats->rx_size_256_to_511_packets += rd64(hw, NGBE_MACRX256to511L);
	hw_stats->rx_size_512_to_1023_packets += rd64(hw, NGBE_MACRX512to1023L);
	hw_stats->rx_size_1024_to_max_packets += rd64(hw, NGBE_MACRX1024toMAXL);
	hw_stats->tx_size_64_packets += rd64(hw, NGBE_MACTX1to64L);
	hw_stats->tx_size_65_to_127_packets += rd64(hw, NGBE_MACTX65to127L);
	hw_stats->tx_size_128_to_255_packets += rd64(hw, NGBE_MACTX128to255L);
	hw_stats->tx_size_256_to_511_packets += rd64(hw, NGBE_MACTX256to511L);
	hw_stats->tx_size_512_to_1023_packets += rd64(hw, NGBE_MACTX512to1023L);
	hw_stats->tx_size_1024_to_max_packets += rd64(hw, NGBE_MACTX1024toMAXL);

	hw_stats->rx_undersize_errors += rd64(hw, NGBE_MACRXERRLENL);
	//hw_stats->rfc += rd32(hw, NGBE_RFC);
	hw_stats->rx_oversize_errors += rd32(hw, NGBE_MACRXOVERSIZE);
	hw_stats->rx_jabber_errors += rd32(hw, NGBE_MACRXJABBER);

	/* MNG Stats */
	hw_stats->mng_bmc2host_packets = rd32(hw, NGBE_MNGBMC2OS);
	hw_stats->mng_host2bmc_packets = rd32(hw, NGBE_MNGOS2BMC);
	hw_stats->rx_management_packets = rd32(hw, NGBE_DMARXMNG);
	hw_stats->tx_management_packets = rd32(hw, NGBE_DMATXMNG);

	/* FCoE Stats */
//	hw_stats->rx_fcoe_crc_errors += rd32(hw, NGBE_FCOECRC);
//	hw_stats->rx_fcoe_mbuf_allocation_errors += rd32(hw, NGBE_FCOELAST);
//	hw_stats->rx_fcoe_dropped += rd32(hw, NGBE_FCOERPDC);
//	hw_stats->rx_fcoe_packets += rd32(hw, NGBE_FCOEPRC);
//	hw_stats->tx_fcoe_packets += rd32(hw, NGBE_FCOEPTC);
//	hw_stats->rx_fcoe_bytes += rd32(hw, NGBE_FCOEDWRC);
//	hw_stats->tx_fcoe_bytes += rd32(hw, NGBE_FCOEDWTC);

	/* MACsec Stats */
	hw_stats->tx_macsec_pkts_untagged += rd32(hw, NGBE_LSECTX_UTPKT);
	hw_stats->tx_macsec_pkts_encrypted +=
			rd32(hw, NGBE_LSECTX_ENCPKT);
	hw_stats->tx_macsec_pkts_protected +=
			rd32(hw, NGBE_LSECTX_PROTPKT);
	hw_stats->tx_macsec_octets_encrypted +=
			rd32(hw, NGBE_LSECTX_ENCOCT);
	hw_stats->tx_macsec_octets_protected +=
			rd32(hw, NGBE_LSECTX_PROTOCT);
	hw_stats->rx_macsec_pkts_untagged += rd32(hw, NGBE_LSECRX_UTPKT);
	hw_stats->rx_macsec_pkts_badtag += rd32(hw, NGBE_LSECRX_BTPKT);
	hw_stats->rx_macsec_pkts_nosci += rd32(hw, NGBE_LSECRX_NOSCIPKT);
	hw_stats->rx_macsec_pkts_unknownsci += rd32(hw, NGBE_LSECRX_UNSCIPKT);
	hw_stats->rx_macsec_octets_decrypted += rd32(hw, NGBE_LSECRX_DECOCT);
	hw_stats->rx_macsec_octets_validated += rd32(hw, NGBE_LSECRX_VLDOCT);
	hw_stats->rx_macsec_sc_pkts_unchecked += rd32(hw, NGBE_LSECRX_UNCHKPKT);
	hw_stats->rx_macsec_sc_pkts_delayed += rd32(hw, NGBE_LSECRX_DLYPKT);
	hw_stats->rx_macsec_sc_pkts_late += rd32(hw, NGBE_LSECRX_LATEPKT);
	for (i = 0; i < 2; i++) {
		hw_stats->rx_macsec_sa_pkts_ok +=
			rd32(hw, NGBE_LSECRX_OKPKT(i));
		hw_stats->rx_macsec_sa_pkts_invalid +=
			rd32(hw, NGBE_LSECRX_INVPKT(i));
		hw_stats->rx_macsec_sa_pkts_notvalid +=
			rd32(hw, NGBE_LSECRX_BADPKT(i));
	}
	for (i = 0; i < 4; i++) {
		hw_stats->rx_macsec_sa_pkts_unusedsa +=
			rd32(hw, NGBE_LSECRX_INVSAPKT(i));
		hw_stats->rx_macsec_sa_pkts_notusingsa +=
			rd32(hw, NGBE_LSECRX_BADSAPKT(i));
	}
	hw_stats->rx_total_missed_packets =
			hw_stats->rx_up_dropped;

}

/*
 * This function is based on ngbe_update_stats_counters() in ngbe/ngbe.c
 */
static int
ngbe_dev_stats_get(struct rte_eth_dev *dev, struct rte_eth_stats *stats)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);
	struct ngbe_stat_mappings *stat_mappings =
			NGBE_DEV_STAT_MAPPINGS(dev);
	uint32_t i, j;

	ngbe_read_stats_registers(hw, hw_stats);

	if (stats == NULL)
		return -EINVAL;

	/* Fill out the rte_eth_stats statistics structure */
	stats->ipackets = hw_stats->rx_packets;
	stats->ibytes = hw_stats->rx_bytes;
	stats->opackets = hw_stats->tx_packets;
	stats->obytes = hw_stats->tx_bytes;

	memset(&stats->q_ipackets, 0, sizeof(stats->q_ipackets));
	memset(&stats->q_opackets, 0, sizeof(stats->q_opackets));
	memset(&stats->q_ibytes, 0, sizeof(stats->q_ibytes));
	memset(&stats->q_obytes, 0, sizeof(stats->q_obytes));
	memset(&stats->q_errors, 0, sizeof(stats->q_errors));
	for (i = 0; i < NGBE_MAX_QP; i++) {
		uint32_t n = i / NB_QMAP_FIELDS_PER_QSM_REG;
		uint32_t offset = (i % NB_QMAP_FIELDS_PER_QSM_REG) * 8;
		uint32_t q_map;

		q_map = (stat_mappings->rqsm[n] >> offset)
				& QMAP_FIELD_RESERVED_BITS_MASK;
		j = (q_map < RTE_ETHDEV_QUEUE_STAT_CNTRS
		     ? q_map : q_map % RTE_ETHDEV_QUEUE_STAT_CNTRS);
		stats->q_ipackets[j] += hw_stats->qp[i].rx_qp_packets;
		stats->q_ibytes[j] += hw_stats->qp[i].rx_qp_bytes;
		//stats->q_errors[j] += hw_stats->qp[i].rx_qp_drop_packets;

		q_map = (stat_mappings->tqsm[n] >> offset)
				& QMAP_FIELD_RESERVED_BITS_MASK;
		j = (q_map < RTE_ETHDEV_QUEUE_STAT_CNTRS
		     ? q_map : q_map % RTE_ETHDEV_QUEUE_STAT_CNTRS);
		stats->q_opackets[j] += hw_stats->qp[i].tx_qp_packets;
		stats->q_obytes[j] += hw_stats->qp[i].tx_qp_bytes;
		
	}

	/* Rx Errors */
	stats->imissed  = hw_stats->rx_total_missed_packets +
			  hw_stats->rx_dma_drop;
	stats->ierrors  = hw_stats->rx_crc_errors +
			  hw_stats->rx_mac_short_packet_dropped +
			  hw_stats->rx_length_errors +
			  hw_stats->rx_undersize_errors +
			  hw_stats->rx_oversize_errors +
			  hw_stats->rx_illegal_byte_errors +
			  hw_stats->rx_error_bytes +
			  hw_stats->rx_fragment_errors;

	/* Tx Errors */
	stats->oerrors  = 0;
	return 0;
}

static int
ngbe_dev_stats_reset(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);

	/* HW registers are cleared on read */
	hw->offset_loaded = 0;
	ngbe_dev_stats_get(dev, NULL);
	hw->offset_loaded = 1;

	/* Reset software totals */
	memset(hw_stats, 0, sizeof(*hw_stats));

	return 0;
}

/* This function calculates the number of xstats based on the current config */
static unsigned
ngbe_xstats_calc_num(struct rte_eth_dev *dev) {
	int nb_queues = max(dev->data->nb_rx_queues, dev->data->nb_tx_queues);
	return NGBE_NB_HW_STATS +
	       NGBE_NB_QP_STATS * nb_queues;
}

static inline int
ngbe_get_name_by_id(uint32_t id, char *name, uint32_t size)
{
	int nb, st;

	/* Extended stats from ngbe_hw_stats */
	if (id < NGBE_NB_HW_STATS) {
		snprintf(name, size, "[hw]%s",
			rte_ngbe_stats_strings[id].name);
		return 0;
	}
	id -= NGBE_NB_HW_STATS;

	/* Queue Stats */
	if (id < NGBE_NB_QP_STATS * NGBE_MAX_QP) {
		nb = id / NGBE_NB_QP_STATS;
		st = id % NGBE_NB_QP_STATS;
		snprintf(name, size, "[q%u]%s", nb,
			rte_ngbe_qp_strings[st].name);
		return 0;
	}
	id -= NGBE_NB_QP_STATS * NGBE_MAX_QP;

	return -(int)(id + 1);
}

static inline int
ngbe_get_offset_by_id(uint32_t id, uint32_t *offset)
{
	int nb, st;

	/* Extended stats from ngbe_hw_stats */
	if (id < NGBE_NB_HW_STATS) {
		*offset = rte_ngbe_stats_strings[id].offset;
		return 0;
	}
	id -= NGBE_NB_HW_STATS;

	/* Queue Stats */
	if (id < NGBE_NB_QP_STATS * NGBE_MAX_QP) {
		nb = id / NGBE_NB_QP_STATS;
		st = id % NGBE_NB_QP_STATS;
		*offset = rte_ngbe_qp_strings[st].offset +
			nb * (NGBE_NB_QP_STATS * sizeof(uint64_t));
		return 0;
	}

	return -1;
}

static int ngbe_dev_xstats_get_names(__rte_unused struct rte_eth_dev *dev,
	struct rte_eth_xstat_name *xstats_names, unsigned int limit)
{
	unsigned i, count;

	count = ngbe_xstats_calc_num(dev);
	if (xstats_names == NULL) {
		return count;
	}

	/* Note: limit >= cnt_stats checked upstream
	 * in rte_eth_xstats_names()
	 */
	limit = min(limit, count);

	/* Extended stats from ngbe_hw_stats */
	for (i = 0; i < limit; i++) {
		if (ngbe_get_name_by_id(i, xstats_names[i].name,
			sizeof(xstats_names[i].name))) {
			PMD_INIT_LOG(WARNING, "id value %d isn't valid", i);
			break;
		}
	}

	return i;
}

#if RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0)
static int ngbe_dev_xstats_get_names_by_id(
	struct rte_eth_dev *dev,
	struct rte_eth_xstat_name *xstats_names,
	const uint64_t *ids,
	unsigned int limit)
{
	unsigned i;

	if (ids == NULL) {
		return ngbe_dev_xstats_get_names(dev, xstats_names, limit);
	}

	for (i = 0; i < limit; i++) {
		if (ngbe_get_name_by_id(ids[i], xstats_names[i].name,
				sizeof(xstats_names[i].name))) {
			PMD_INIT_LOG(WARNING, "id value %d isn't valid", i);
			return -1;
		}
	}

	return i;
}
#endif /* RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0) */

static int
ngbe_dev_xstats_get(struct rte_eth_dev *dev, struct rte_eth_xstat *xstats,
					 unsigned limit)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);
	unsigned i, count;

	ngbe_read_stats_registers(hw, hw_stats);

	/* If this is a reset xstats is NULL, and we have cleared the
	 * registers by reading them.
	 */
	count = ngbe_xstats_calc_num(dev);
	if (xstats == NULL) {
		return count;
	}

	limit = min(limit, ngbe_xstats_calc_num(dev));

	/* Extended stats from ngbe_hw_stats */
	for (i = 0; i < limit; i++) {
		uint32_t offset = 0;

		if (ngbe_get_offset_by_id(i, &offset)) {
			PMD_INIT_LOG(WARNING, "id value %d isn't valid", i);
			break;
		}
		xstats[i].value = *(uint64_t *)(((char *)hw_stats) + offset);
		xstats[i].id = i;
	}

	return i;
}

#if RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0)
static int
ngbe_dev_xstats_get_(struct rte_eth_dev *dev, uint64_t *values,
					 unsigned limit)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);
	unsigned i, count;

	ngbe_read_stats_registers(hw, hw_stats);

	/* If this is a reset xstats is NULL, and we have cleared the
	 * registers by reading them.
	 */
	count = ngbe_xstats_calc_num(dev);
	if (values == NULL) {
		return count;
	}

	limit = min(limit, ngbe_xstats_calc_num(dev));

	/* Extended stats from ngbe_hw_stats */
	for (i = 0; i < limit; i++) {
		uint32_t offset = 0;

		if (ngbe_get_offset_by_id(i, &offset)) {
			PMD_INIT_LOG(WARNING, "id value %d isn't valid", i);
			break;
		}
		values[i] = *(uint64_t *)(((char *)hw_stats) + offset);
	}

	return i;
}

static int
ngbe_dev_xstats_get_by_id(struct rte_eth_dev *dev, const uint64_t *ids,
		uint64_t *values, unsigned int limit)
{
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);
	unsigned i;

	if (ids == NULL) {
		return ngbe_dev_xstats_get_(dev, values, limit);
	}

	for (i = 0; i < limit; i++) {
		uint32_t offset;

		if (ngbe_get_offset_by_id(ids[i], &offset)) {
			PMD_INIT_LOG(WARNING, "id value %d isn't valid", i);
			break;
		}
		values[i] = *(uint64_t *)(((char *)hw_stats) + offset);
	}

	return i;
}
#endif /* RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0) */

static int
ngbe_dev_xstats_reset(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_hw_stats *hw_stats = NGBE_DEV_STATS(dev);

	/* HW registers are cleared on read */
	hw->offset_loaded = 0;
	ngbe_read_stats_registers(hw, hw_stats);
	hw->offset_loaded = 1;

	/* Reset software totals */
	memset(hw_stats, 0, sizeof(*hw_stats));

	return 0;
}

static int
ngbe_fw_version_get(struct rte_eth_dev *dev, char *fw_version, size_t fw_size)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	int ret;

	ret = snprintf(fw_version, fw_size, "0x%08x", hw->eeprom_id);

	if (ret < 0)
		return -EINVAL;

	ret += 1; /* add the size of '\0' */
	if (fw_size < (size_t)ret)
		return ret;

	return 0;
}

static int
ngbe_dev_info_get(struct rte_eth_dev *dev, struct rte_eth_dev_info *dev_info)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	RTE_ETH_DEV_INFO_INIT(dev, dev_info);
	
	dev_info->max_rx_queues = (uint16_t)hw->mac.max_rx_queues;
	dev_info->max_tx_queues = (uint16_t)hw->mac.max_tx_queues;
	dev_info->min_rx_bufsize = 1024; /* cf BSIZEPACKET in SRRCTL register */
	dev_info->max_rx_pktlen = 15872; /* includes CRC, cf MAXFRS register */
	dev_info->max_mac_addrs = hw->mac.num_rar_entries;
	dev_info->max_hash_mac_addrs = NGBE_VMDQ_NUM_UC_MAC;
	dev_info->max_vfs = pci_dev->max_vfs;
	dev_info->max_vmdq_pools = ETH_64_POOLS;
	dev_info->vmdq_queue_num = dev_info->max_rx_queues;
	dev_info->rx_queue_offload_capa = ngbe_get_rx_queue_offloads(dev);
	dev_info->rx_offload_capa = (ngbe_get_rx_port_offloads(dev) |
				     dev_info->rx_queue_offload_capa);
	dev_info->tx_queue_offload_capa = ngbe_get_tx_queue_offloads(dev);
	dev_info->tx_offload_capa = ngbe_get_tx_port_offloads(dev);

	dev_info->default_rxconf = (struct rte_eth_rxconf) {
		.rx_thresh = {
			.pthresh = NGBE_DEFAULT_RX_PTHRESH,
			.hthresh = NGBE_DEFAULT_RX_HTHRESH,
			.wthresh = NGBE_DEFAULT_RX_WTHRESH,
		},
		.rx_free_thresh = NGBE_DEFAULT_RX_FREE_THRESH,
		.rx_drop_en = 0,
		.offloads = 0,
	};

	dev_info->default_txconf = (struct rte_eth_txconf) {
		.tx_thresh = {
			.pthresh = NGBE_DEFAULT_TX_PTHRESH,
			.hthresh = NGBE_DEFAULT_TX_HTHRESH,
			.wthresh = NGBE_DEFAULT_TX_WTHRESH,
		},
		.tx_free_thresh = NGBE_DEFAULT_TX_FREE_THRESH,
		.offloads = 0,
	};

	dev_info->rx_desc_lim = rx_desc_lim;
	dev_info->tx_desc_lim = tx_desc_lim;

	dev_info->hash_key_size = NGBE_HKEY_MAX_INDEX * sizeof(uint32_t);
	dev_info->reta_size = ETH_RSS_RETA_SIZE_128;
	dev_info->flow_type_rss_offloads = NGBE_RSS_OFFLOAD_ALL;

	dev_info->speed_capa = ETH_LINK_SPEED_1G | ETH_LINK_SPEED_100M |
				ETH_LINK_SPEED_10M;

	/* Driver-preferred Rx/Tx parameters */
	dev_info->default_rxportconf.burst_size = 32;
	dev_info->default_txportconf.burst_size = 32;
	dev_info->default_rxportconf.nb_queues = 1;
	dev_info->default_txportconf.nb_queues = 1;
	dev_info->default_rxportconf.ring_size = 256;
	dev_info->default_txportconf.ring_size = 256;

	return 0;
}

const uint32_t *
ngbe_dev_supported_ptypes_get(struct rte_eth_dev *dev)
{
	if (dev->rx_pkt_burst == ngbe_recv_pkts ||
	    dev->rx_pkt_burst == ngbe_recv_pkts_lro_single_alloc ||
	    dev->rx_pkt_burst == ngbe_recv_pkts_lro_bulk_alloc ||
	    dev->rx_pkt_burst == ngbe_recv_pkts_bulk_alloc)
		return ngbe_get_supported_ptypes();

#if defined(RTE_ARCH_X86)
	if (dev->rx_pkt_burst == ngbe_recv_pkts_vec ||
	    dev->rx_pkt_burst == ngbe_recv_scattered_pkts_vec)
		return ngbe_get_supported_ptypes();
#endif
	return NULL;
}

void
ngbe_dev_setup_link_alarm_handler(void *param)
{
	struct rte_eth_dev *dev = (struct rte_eth_dev *)param;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	u32 speed;
	bool autoneg = false;

	speed = hw->phy.autoneg_advertised;
	if (!speed)
		hw->mac.get_link_capabilities(hw, &speed, &autoneg);

	hw->mac.setup_link(hw, speed, true);

	intr->flags &= ~NGBE_FLAG_NEED_LINK_CONFIG;
}

#ifndef RTE_NGBE_POLL
void
ngbe_check_link_alarm(void *param)
{
	struct rte_eth_dev *dev = (struct rte_eth_dev *)param;
	struct rte_eth_link *link = &(dev->data->dev_link);

	ngbe_dev_link_update(dev, 0);
	if (!link->link_status)
		rte_eal_alarm_set(100, ngbe_check_link_alarm, dev);

	return;
}
#endif

/* return 0 means link status changed, -1 means not changed */
int
ngbe_dev_link_update_share(struct rte_eth_dev *dev,
			    int wait_to_complete)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct rte_eth_link link;
	u32 link_speed = NGBE_LINK_SPEED_UNKNOWN;
	u32 lan_speed = 0;
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	int link_up;
	int err;
	int wait = 1;

	memset(&link, 0, sizeof(link));
	link.link_status = ETH_LINK_DOWN;
	link.link_speed = ETH_SPEED_NUM_NONE;
	link.link_duplex = ETH_LINK_HALF_DUPLEX;
	link.link_autoneg = !(dev->data->dev_conf.link_speeds &
			~ETH_LINK_SPEED_AUTONEG);

	hw->mac.get_link_status = true;

	if (intr->flags & NGBE_FLAG_NEED_LINK_CONFIG)
		return rte_eth_linkstatus_set(dev, &link);

	/* check if it needs to wait to complete, if lsc interrupt is enabled */
	if (wait_to_complete == 0 || dev->data->dev_conf.intr_conf.lsc != 0)
		wait = 0;

	err = hw->mac.check_link(hw, &link_speed, &link_up, wait);

	if (err != 0) {
		link.link_speed = ETH_LINK_DOWN;
		link.link_duplex = ETH_LINK_HALF_DUPLEX;
		return rte_eth_linkstatus_set(dev, &link);
	}

	if (link_up == 0) {
#ifndef RTE_NGBE_POLL
		if ((hw->phy.media_type == ngbe_media_type_fiber) &&
			(hw->phy.type != ngbe_phy_mvl_sfi)) {
			intr->flags |= NGBE_FLAG_NEED_LINK_CONFIG;
			rte_eal_alarm_set(10,
				ngbe_dev_setup_link_alarm_handler, dev);
		}
#endif
		return rte_eth_linkstatus_set(dev, &link);
	}

	intr->flags &= ~NGBE_FLAG_NEED_LINK_CONFIG;
	link.link_status = ETH_LINK_UP;
	link.link_duplex = ETH_LINK_FULL_DUPLEX;

	switch (link_speed) {
	default:
	case NGBE_LINK_SPEED_UNKNOWN:
		link.link_duplex = ETH_LINK_HALF_DUPLEX;
		link.link_speed = ETH_LINK_DOWN;
		break;

	case NGBE_LINK_SPEED_10M_FULL:
		link.link_speed = ETH_SPEED_NUM_10M;
		lan_speed = 0;
		break;

	case NGBE_LINK_SPEED_100M_FULL:
		link.link_speed = ETH_SPEED_NUM_100M;
		lan_speed = 1;
		break;

	case NGBE_LINK_SPEED_1GB_FULL:
		link.link_speed = ETH_SPEED_NUM_1G;
		lan_speed = 2;
		break;

	case NGBE_LINK_SPEED_2_5GB_FULL:
		link.link_speed = ETH_SPEED_NUM_2_5G;
		break;

	case NGBE_LINK_SPEED_5GB_FULL:
		link.link_speed = ETH_SPEED_NUM_5G;
		break;

	case NGBE_LINK_SPEED_10GB_FULL:
		link.link_speed = ETH_SPEED_NUM_10G;
		break;
	}

	if (hw->is_pf) {
		wr32m(hw, NGBE_LAN_SPEED, NGBE_LAN_SPEED_MASK, lan_speed);
		if (link_speed & (NGBE_LINK_SPEED_1GB_FULL |
			NGBE_LINK_SPEED_100M_FULL | NGBE_LINK_SPEED_10M_FULL)) {
			wr32m(hw, NGBE_MACTXCFG, NGBE_MACTXCFG_SPEED_MASK,
				NGBE_MACTXCFG_SPEED_1G | NGBE_MACTXCFG_TE);
		}
	}

	return rte_eth_linkstatus_set(dev, &link);
}

static int
ngbe_dev_link_update(struct rte_eth_dev *dev, int wait_to_complete)
{
	return ngbe_dev_link_update_share(dev, wait_to_complete);
}

static int
ngbe_dev_promiscuous_enable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t fctrl;

	fctrl = rd32(hw, NGBE_PSRCTL);
	fctrl |= (NGBE_PSRCTL_UCP | NGBE_PSRCTL_MCP);
	wr32(hw, NGBE_PSRCTL, fctrl);

	return 0;
}

static int
ngbe_dev_promiscuous_disable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t fctrl;

	fctrl = rd32(hw, NGBE_PSRCTL);
	fctrl &= (~NGBE_PSRCTL_UCP);
	if (dev->data->all_multicast == 1)
		fctrl |= NGBE_PSRCTL_MCP;
	else
		fctrl &= (~NGBE_PSRCTL_MCP);
	wr32(hw, NGBE_PSRCTL, fctrl);

	return 0;
}

static int
ngbe_dev_allmulticast_enable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t fctrl;

	fctrl = rd32(hw, NGBE_PSRCTL);
	fctrl |= NGBE_PSRCTL_MCP;
	wr32(hw, NGBE_PSRCTL, fctrl);

	return 0;
}

static int
ngbe_dev_allmulticast_disable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t fctrl;

	if (dev->data->promiscuous == 1)
		return 0; /* must remain in all_multicast mode */

	fctrl = rd32(hw, NGBE_PSRCTL);
	fctrl &= (~NGBE_PSRCTL_MCP);
	wr32(hw, NGBE_PSRCTL, fctrl);

	return 0;
}

/**
 * It clears the interrupt causes and enables the interrupt.
 * It will be called once only during nic initialized.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 * @param on
 *  Enable or Disable.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static int
ngbe_dev_lsc_interrupt_setup(struct rte_eth_dev *dev, uint8_t on)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);

	ngbe_dev_link_status_print(dev);
	if (on) {
		intr->mask_misc |= NGBE_ICRMISC_PHY;
		intr->mask_misc |= NGBE_ICRMISC_GPIO;
	} else {
		intr->mask_misc &= ~NGBE_ICRMISC_PHY;
		intr->mask_misc &= ~NGBE_ICRMISC_GPIO;
	}

	return 0;
}

/**
 * It clears the interrupt causes and enables the interrupt.
 * It will be called once only during nic initialized.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static int
ngbe_dev_misc_interrupt_setup(struct rte_eth_dev *dev)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	u64 mask;

	mask = NGBE_ICR_MASK;
	mask &= (1ULL << NGBE_MISC_VEC_ID);
	intr->mask |= mask;
	intr->mask_misc |= NGBE_ICRMISC_GPIO;

	return 0;
}

/**
 * It clears the interrupt causes and enables the interrupt.
 * It will be called once only during nic initialized.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static int
ngbe_dev_rxq_interrupt_setup(struct rte_eth_dev *dev)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	u64 mask;

	mask = NGBE_ICR_MASK;
	mask &= ~((1ULL << NGBE_RX_VEC_START) - 1);
	intr->mask |= mask;

	return 0;
}

/**
 * It clears the interrupt causes and enables the interrupt.
 * It will be called once only during nic initialized.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static int
ngbe_dev_macsec_interrupt_setup(struct rte_eth_dev *dev)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);

	intr->mask_misc |= NGBE_ICRMISC_LNKSEC;

	return 0;
}

/*
 * It reads ICR and sets flag (NGBE_EICR_LSC) for the link_update.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static int
ngbe_dev_interrupt_get_status(struct rte_eth_dev *dev)
{
	uint32_t eicr;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);

	/* read-on-clear nic registers here */
	eicr = ((u32 *)hw->isb_mem)[NGBE_ISB_MISC];
	PMD_DRV_LOG(DEBUG, "eicr %x", eicr);

	intr->flags = 0;

	/* set flag for async link update */
	if (eicr & NGBE_ICRMISC_PHY)
		intr->flags |= NGBE_FLAG_NEED_LINK_UPDATE;

	if (eicr & NGBE_ICRMISC_VFMBX)
		intr->flags |= NGBE_FLAG_MAILBOX;

	if (eicr & NGBE_ICRMISC_LNKSEC)
		intr->flags |= NGBE_FLAG_MACSEC;

	if (eicr & NGBE_ICRMISC_GPIO) {
		intr->flags |= NGBE_FLAG_PHY_INTERRUPT;	
		intr->flags |= NGBE_FLAG_NEED_LINK_UPDATE;
	}

	((u32 *)hw->isb_mem)[NGBE_ISB_MISC] = 0;

	return 0;
}

/**
 * It gets and then prints the link status.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static void
ngbe_dev_link_status_print(struct rte_eth_dev *dev)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	struct rte_eth_link link;

	rte_eth_linkstatus_get(dev, &link);

	if (link.link_status) {
		PMD_INIT_LOG(INFO, "Port %d: Link Up - speed %u Mbps - %s",
					(int)(dev->data->port_id),
					(unsigned)link.link_speed,
			link.link_duplex == ETH_LINK_FULL_DUPLEX ?
					"full-duplex" : "half-duplex");
	} else {
		PMD_INIT_LOG(INFO, " Port %d: Link Down",
				(int)(dev->data->port_id));
	}
	PMD_INIT_LOG(DEBUG, "PCI Address: " PCI_PRI_FMT,
				pci_dev->addr.domain,
				pci_dev->addr.bus,
				pci_dev->addr.devid,
				pci_dev->addr.function);
}

/*
 * It executes link_update after knowing an interrupt occurred.
 *
 * @param dev
 *  Pointer to struct rte_eth_dev.
 *
 * @return
 *  - On success, zero.
 *  - On failure, a negative value.
 */
static int
ngbe_dev_interrupt_action(struct rte_eth_dev *dev,
			   struct rte_intr_handle *intr_handle)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	UNREFERENCED_PARAMETER(intr_handle);

	PMD_DRV_LOG(DEBUG, "intr action type %d", intr->flags);

	if (intr->flags & NGBE_FLAG_MAILBOX) {
		ngbe_pf_mbx_process(dev);
		intr->flags &= ~NGBE_FLAG_MAILBOX;
	}

	if (intr->flags & NGBE_FLAG_PHY_INTERRUPT) {
		hw->phy.handle_lasi(hw);
		intr->flags &= ~NGBE_FLAG_PHY_INTERRUPT;
	}

	if (intr->flags & NGBE_FLAG_NEED_LINK_UPDATE) {
		struct rte_eth_link link;

		/* get the link status before link update, for predicting later */
		rte_eth_linkstatus_get(dev, &link);

		ngbe_dev_link_update(dev, 0);
		intr->flags &= ~NGBE_FLAG_NEED_LINK_UPDATE;
		ngbe_dev_link_status_print(dev);
		if (dev->data->dev_link.link_speed != link.link_speed)
			rte_eth_dev_callback_process(dev,
				RTE_ETH_EVENT_INTR_LSC, NULL, NULL);
	}

	PMD_DRV_LOG(DEBUG, "enable intr immediately");
	ngbe_enable_intr(dev);

	return 0;
}

/**
 * Interrupt handler triggered by NIC  for handling
 * specific interrupt.
 *
 * @param handle
 *  Pointer to interrupt handle.
 * @param param
 *  The address of parameter (struct rte_eth_dev *) regsitered before.
 *
 * @return
 *  void
 */
static void
ngbe_dev_interrupt_handler(void *param)
{
	struct rte_eth_dev *dev = (struct rte_eth_dev *)param;

	ngbe_dev_interrupt_get_status(dev);
	ngbe_dev_interrupt_action(dev, dev->intr_handle);
}

static int
ngbe_dev_led_on(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw;

	hw = NGBE_DEV_HW(dev);
	return ngbe_led_on(hw, 0) == 0 ? 0 : -ENOTSUP;
}

static int
ngbe_dev_led_off(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw;

	hw = NGBE_DEV_HW(dev);
	return ngbe_led_off(hw, 0) == 0 ? 0 : -ENOTSUP;
}

static int
ngbe_flow_ctrl_get(struct rte_eth_dev *dev, struct rte_eth_fc_conf *fc_conf)
{
	struct ngbe_hw *hw;
	uint32_t mflcn_reg;
	uint32_t fccfg_reg;
	int rx_pause;
	int tx_pause;

	hw = NGBE_DEV_HW(dev);

	fc_conf->pause_time = hw->fc.pause_time;
	fc_conf->high_water = hw->fc.high_water;
	fc_conf->low_water = hw->fc.low_water;
	fc_conf->send_xon = hw->fc.send_xon;
	fc_conf->autoneg = !hw->fc.disable_fc_autoneg;

	/*
	 * Return rx_pause status according to actual setting of
	 * MFLCN register.
	 */
	mflcn_reg = rd32(hw, NGBE_RXFCCFG);
	if (mflcn_reg & NGBE_RXFCCFG_FC)
		rx_pause = 1;
	else
		rx_pause = 0;

	/*
	 * Return tx_pause status according to actual setting of
	 * FCCFG register.
	 */
	fccfg_reg = rd32(hw, NGBE_TXFCCFG);
	if (fccfg_reg & NGBE_TXFCCFG_FC)
		tx_pause = 1;
	else
		tx_pause = 0;

	if (rx_pause && tx_pause)
		fc_conf->mode = RTE_FC_FULL;
	else if (rx_pause)
		fc_conf->mode = RTE_FC_RX_PAUSE;
	else if (tx_pause)
		fc_conf->mode = RTE_FC_TX_PAUSE;
	else
		fc_conf->mode = RTE_FC_NONE;

	return 0;
}

static int
ngbe_flow_ctrl_set(struct rte_eth_dev *dev, struct rte_eth_fc_conf *fc_conf)
{
	struct ngbe_hw *hw;
	int err;
	uint32_t rx_buf_size;
	uint32_t max_high_water;
	enum ngbe_fc_mode rte_fcmode_2_ngbe_fcmode[] = {
		ngbe_fc_none,
		ngbe_fc_rx_pause,
		ngbe_fc_tx_pause,
		ngbe_fc_full
	};

	PMD_INIT_FUNC_TRACE();

	hw = NGBE_DEV_HW(dev);
	rx_buf_size = rd32(hw, NGBE_PBRXSIZE);
	PMD_INIT_LOG(DEBUG, "Rx packet buffer size = 0x%x", rx_buf_size);

	/*
	 * At least reserve one Ethernet frame for watermark
	 * high_water/low_water in kilo bytes for ngbe
	 */
	max_high_water = (rx_buf_size - ETHER_MAX_LEN) >> 10;
	if ((fc_conf->high_water > max_high_water) ||
	    (fc_conf->high_water < fc_conf->low_water)) {
		PMD_INIT_LOG(ERR, "Invalid high/low water setup value in KB");
		PMD_INIT_LOG(ERR, "High_water must <= 0x%x", max_high_water);
		return -EINVAL;
	}

	hw->fc.requested_mode = rte_fcmode_2_ngbe_fcmode[fc_conf->mode];
	hw->fc.pause_time     = fc_conf->pause_time;
	hw->fc.high_water  	  = fc_conf->high_water;
	hw->fc.low_water      = fc_conf->low_water;
	hw->fc.send_xon       = fc_conf->send_xon;
	hw->fc.disable_fc_autoneg = !fc_conf->autoneg;

	err = ngbe_fc_enable(hw);

	/* Not negotiated is not an error case */
	if ((err == 0) || (err == NGBE_ERR_FC_NOT_NEGOTIATED)) {
		/* check if we want to forward MAC frames - driver doesn't have native
		 * capability to do that, so we'll write the registers ourselves */

		/* set or clear MFLCN.PMCF bit depending on configuration */
		wr32m(hw, NGBE_MACRXFLT, NGBE_MACRXFLT_CTL_MASK,
		      (fc_conf->mac_ctrl_frame_fwd
		       ? NGBE_MACRXFLT_CTL_NOPS : NGBE_MACRXFLT_CTL_DROP));
		ngbe_flush(hw);

		return 0;
	}

	PMD_INIT_LOG(ERR, "ngbe_fc_enable = 0x%x", err);
	return -EIO;
}

int
ngbe_dev_rss_reta_update(struct rte_eth_dev *dev,
			  struct rte_eth_rss_reta_entry64 *reta_conf,
			  uint16_t reta_size)
{
	uint8_t i, j, mask;
	uint32_t reta;
	uint16_t idx, shift;
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	PMD_INIT_FUNC_TRACE();

	if (!ngbe_rss_update_sp(hw->mac.type)) {
		PMD_DRV_LOG(ERR, "RSS reta update is not supported on this "
			"NIC.");
		return -ENOTSUP;
	}

	if (reta_size != ETH_RSS_RETA_SIZE_128) {
		PMD_DRV_LOG(ERR, "The size of hash lookup table configured "
			"(%d) doesn't match the number hardware can supported "
			"(%d)", reta_size, ETH_RSS_RETA_SIZE_128);
		return -EINVAL;
	}

	for (i = 0; i < reta_size; i += 4) {
		idx = i / RTE_RETA_GROUP_SIZE;
		shift = i % RTE_RETA_GROUP_SIZE;
		mask = (uint8_t)RS64(reta_conf[idx].mask, shift, 0xF);
		if (!mask)
			continue;

		reta = rd32a(hw, NGBE_REG_RSSTBL, i >> 2);
		for (j = 0; j < 4; j++) {
			if (RS8(mask, j , 0x1)) {
				reta  &= ~ MS32(8 * j, 0xFF);
				reta |= LS32(reta_conf[idx].reta[shift + j],
						8 * j, 0xFF);
			}
		}
		wr32a(hw, NGBE_REG_RSSTBL, i >> 2, reta);
	}
	adapter->rss_reta_updated = 1;

	return 0;
}

int
ngbe_dev_rss_reta_query(struct rte_eth_dev *dev,
			 struct rte_eth_rss_reta_entry64 *reta_conf,
			 uint16_t reta_size)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint8_t i, j, mask;
	uint32_t reta;
	uint16_t idx, shift;

	PMD_INIT_FUNC_TRACE();

	if (reta_size != ETH_RSS_RETA_SIZE_128) {
		PMD_DRV_LOG(ERR, "The size of hash lookup table configured "
			"(%d) doesn't match the number hardware can supported "
			"(%d)", reta_size, ETH_RSS_RETA_SIZE_128);
		return -EINVAL;
	}

	for (i = 0; i < reta_size; i += 4) {
		idx = i / RTE_RETA_GROUP_SIZE;
		shift = i % RTE_RETA_GROUP_SIZE;
		mask = (uint8_t)RS64(reta_conf[idx].mask, shift, 0xF);
		if (!mask)
			continue;

		reta = rd32a(hw, NGBE_REG_RSSTBL, i >> 2);
		for (j = 0; j < 4; j++) {
			if (RS8(mask, j , 0x1))
				reta_conf[idx].reta[shift + j] =
					(uint16_t)RS32(reta, 8 * j, 0xFF);
		}
	}

	return 0;
}

static int
ngbe_add_rar(struct rte_eth_dev *dev, struct ether_addr *mac_addr,
				uint32_t index, uint32_t pool)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t enable_addr = 1;

	return ngbe_set_rar(hw, index, mac_addr->addr_bytes,
			     pool, enable_addr);
}

static void
ngbe_remove_rar(struct rte_eth_dev *dev, uint32_t index)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	ngbe_clear_rar(hw, index);
}

static int
ngbe_set_default_mac_addr(struct rte_eth_dev *dev, struct ether_addr *addr)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	ngbe_remove_rar(dev, 0);
	ngbe_add_rar(dev, addr, 0, pci_dev->max_vfs);

	return 0;
}

static int
ngbe_dev_mtu_set(struct rte_eth_dev *dev, uint16_t mtu)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct rte_eth_dev_info dev_info;
	uint32_t frame_size = mtu + ETHER_HDR_LEN + ETHER_CRC_LEN + 4;
	struct rte_eth_dev_data *dev_data = dev->data;

	ngbe_dev_info_get(dev, &dev_info);

	/* check that mtu is within the allowed range */
	if ((mtu < ETHER_MIN_MTU) || (frame_size > dev_info.max_rx_pktlen))
		return -EINVAL;

	/* If device is started, refuse mtu that requires the support of
	 * scattered packets when this feature has not been enabled before.
	 */
	if (dev_data->dev_started && !dev_data->scattered_rx &&
	    (frame_size + 2 * NGBE_VLAN_TAG_SIZE >
	     dev->data->min_rx_buf_size - RTE_PKTMBUF_HEADROOM)) {
		PMD_INIT_LOG(ERR, "Stop port first.");
		return -EINVAL;
	}

	/* switch to jumbo mode if needed */
	if (frame_size > ETHER_MAX_LEN)
		dev->data->dev_conf.rxmode.offloads |=
			DEV_RX_OFFLOAD_JUMBO_FRAME;
	else
		dev->data->dev_conf.rxmode.offloads &=
			~DEV_RX_OFFLOAD_JUMBO_FRAME;

	/* update max frame size */
	dev->data->dev_conf.rxmode.max_rx_pkt_len = frame_size;

	if(hw->mode)
		wr32m(hw, NGBE_FRMSZ, NGBE_FRMSZ_MAX_MASK, NGBE_FRAME_SIZE_MAX);
	else
		wr32m(hw, NGBE_FRMSZ, NGBE_FRMSZ_MAX_MASK,
			NGBE_FRMSZ_MAX(dev->data->dev_conf.rxmode.max_rx_pkt_len));

	return 0;
}

int
ngbe_vt_check(struct ngbe_hw *hw)
{
	uint32_t reg_val;

	/* if Virtualization Technology is enabled */
	reg_val = rd32(hw, NGBE_PORTCTL);
	if (!(reg_val & NGBE_PORTCTL_NUMVT_MASK)) {
		PMD_INIT_LOG(ERR, "VT must be enabled for this setting");
		return -1;
	}

	return 0;
}

static uint32_t
ngbe_uta_vector(struct ngbe_hw *hw, struct ether_addr *uc_addr)
{
	uint32_t vector = 0;

	switch (hw->mac.mc_filter_type) {
	case 0:   /* use bits [47:36] of the address */
		vector = ((uc_addr->addr_bytes[4] >> 4) |
			(((uint16_t)uc_addr->addr_bytes[5]) << 4));
		break;
	case 1:   /* use bits [46:35] of the address */
		vector = ((uc_addr->addr_bytes[4] >> 3) |
			(((uint16_t)uc_addr->addr_bytes[5]) << 5));
		break;
	case 2:   /* use bits [45:34] of the address */
		vector = ((uc_addr->addr_bytes[4] >> 2) |
			(((uint16_t)uc_addr->addr_bytes[5]) << 6));
		break;
	case 3:   /* use bits [43:32] of the address */
		vector = ((uc_addr->addr_bytes[4]) |
			(((uint16_t)uc_addr->addr_bytes[5]) << 8));
		break;
	default:  /* Invalid mc_filter_type */
		break;
	}

	/* vector can only be 12-bits or boundary will be exceeded */
	vector &= 0xFFF;
	return vector;
}

static int
ngbe_uc_hash_table_set(struct rte_eth_dev *dev, struct ether_addr *mac_addr,
			uint8_t on)
{
	uint32_t vector;
	uint32_t uta_idx;
	uint32_t reg_val;
	uint32_t uta_mask;
	uint32_t psrctl;

	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_uta_info *uta_info = NGBE_DEV_UTA_INFO(dev);

	/* The UTA table only exists on pf hardware */
	if (hw->mac.type < ngbe_mac_sp)
		return -ENOTSUP;

	vector = ngbe_uta_vector(hw, mac_addr);
	uta_idx = (vector >> 5) & 0x7F;
	uta_mask = 0x1UL << (vector & 0x1F);

	if (!!on == !!(uta_info->uta_shadow[uta_idx] & uta_mask))
		return 0;

	reg_val = rd32(hw, NGBE_UCADDRTBL(uta_idx));
	if (on) {
		uta_info->uta_in_use++;
		reg_val |= uta_mask;
		uta_info->uta_shadow[uta_idx] |= uta_mask;
	} else {
		uta_info->uta_in_use--;
		reg_val &= ~uta_mask;
		uta_info->uta_shadow[uta_idx] &= ~uta_mask;
	}

	wr32(hw, NGBE_UCADDRTBL(uta_idx), reg_val);

	psrctl = rd32(hw, NGBE_PSRCTL);
	if (uta_info->uta_in_use > 0) {
		psrctl |= NGBE_PSRCTL_UCHFENA;

	} else {
		psrctl &= ~NGBE_PSRCTL_UCHFENA;
	}
	psrctl &= ~NGBE_PSRCTL_ADHF12_MASK;
	psrctl |= NGBE_PSRCTL_ADHF12(hw->mac.mc_filter_type);
	wr32(hw, NGBE_PSRCTL, psrctl);

	return 0;
}

static int
ngbe_uc_all_hash_table_set(struct rte_eth_dev *dev, uint8_t on)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_uta_info *uta_info = NGBE_DEV_UTA_INFO(dev);
	uint32_t psrctl;
	int i;

	/* The UTA table only exists on pf hardware */
	if (hw->mac.type < ngbe_mac_sp)
		return -ENOTSUP;

	if (on) {
		for (i = 0; i < ETH_VMDQ_NUM_UC_HASH_ARRAY; i++) {
			uta_info->uta_shadow[i] = ~0;
			wr32(hw, NGBE_UCADDRTBL(i), ~0);
		}
	} else {
		for (i = 0; i < ETH_VMDQ_NUM_UC_HASH_ARRAY; i++) {
			uta_info->uta_shadow[i] = 0;
			wr32(hw, NGBE_UCADDRTBL(i), 0);
		}
	}

	psrctl = rd32(hw, NGBE_PSRCTL);
	if (on) {
		psrctl |= NGBE_PSRCTL_UCHFENA;

	} else {
		psrctl &= ~NGBE_PSRCTL_UCHFENA;
	}
	psrctl &= ~NGBE_PSRCTL_ADHF12_MASK;
	psrctl |= NGBE_PSRCTL_ADHF12(hw->mac.mc_filter_type);
	wr32(hw, NGBE_PSRCTL, psrctl);

	return 0;

}

uint32_t
ngbe_convert_vm_rx_mask_to_val(uint16_t rx_mask, uint32_t orig_val)
{
	uint32_t new_val = orig_val;

	if (rx_mask & ETH_VMDQ_ACCEPT_UNTAG)
		new_val |= NGBE_POOLETHCTL_UTA;
	if (rx_mask & ETH_VMDQ_ACCEPT_HASH_MC)
		new_val |= NGBE_POOLETHCTL_MCHA;
	if (rx_mask & ETH_VMDQ_ACCEPT_HASH_UC)
		new_val |= NGBE_POOLETHCTL_UCHA;
	if (rx_mask & ETH_VMDQ_ACCEPT_BROADCAST)
		new_val |= NGBE_POOLETHCTL_BCA;
	if (rx_mask & ETH_VMDQ_ACCEPT_MULTICAST)
		new_val |= NGBE_POOLETHCTL_MCP;

	return new_val;
}

#define NGBE_INVALID_MIRROR_TYPE(mirror_type) \
	((mirror_type) & ~(uint8_t)(ETH_MIRROR_VIRTUAL_POOL_UP | \
	ETH_MIRROR_UPLINK_PORT | ETH_MIRROR_DOWNLINK_PORT | ETH_MIRROR_VLAN))

static int
ngbe_mirror_rule_set(struct rte_eth_dev *dev,
		      struct rte_eth_mirror_conf *mirror_conf,
		      uint8_t rule_id, uint8_t on)
{
	uint32_t mr_ctl, vlvf;
	uint32_t mp_lsb = 0;
//	uint32_t mv_msb = 0;
	uint32_t mv_lsb = 0;
//	uint32_t mp_msb = 0;
	uint8_t i = 0;
	int reg_index = 0;
	uint64_t vlan_mask = 0;

//	const uint8_t pool_mask_offset = 32;
//	const uint8_t vlan_mask_offset = 32;
	const uint8_t dst_pool_offset = 8;
	const uint8_t mirror_rule_mask = 0x0F;

	struct ngbe_mirror_info *mr_info = NGBE_DEV_MR_INFO(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint8_t mirror_type = 0;

	if (ngbe_vt_check(hw) < 0)
		return -ENOTSUP;

	if (rule_id >= NGBE_MAX_MIRROR_RULES)
		return -EINVAL;

	if (NGBE_INVALID_MIRROR_TYPE(mirror_conf->rule_type)) {
		PMD_DRV_LOG(ERR, "unsupported mirror type 0x%x.",
			    mirror_conf->rule_type);
		return -EINVAL;
	}

	if (mirror_conf->rule_type & ETH_MIRROR_VLAN) {
		mirror_type |= NGBE_MIRRCTL_VLAN;
		/* Check if vlan id is valid and find conresponding VLAN ID
		 * index in VLVF
		 */
		for (i = 0; i < NGBE_NUM_POOL; i++) {
			if (mirror_conf->vlan.vlan_mask & (1ULL << i)) {
				/* search vlan id related pool vlan filter
				 * index
				 */
				reg_index = ngbe_find_vlvf_slot(
						hw,
						mirror_conf->vlan.vlan_id[i],
						false);
				if (reg_index < 0)
					return -EINVAL;
				wr32(hw, NGBE_PSRVLANIDX, reg_index);
				vlvf = rd32(hw, NGBE_PSRVLAN);
				if ((NGBE_PSRVLAN_VID(vlvf) ==
				      mirror_conf->vlan.vlan_id[i]))
					vlan_mask |= (1ULL << reg_index);
				else
					return -EINVAL;
			}
		}

		if (on) {
			mv_lsb = vlan_mask & BIT_MASK32;
//			mv_msb = vlan_mask >> vlan_mask_offset;

			mr_info->mr_conf[rule_id].vlan.vlan_mask =
						mirror_conf->vlan.vlan_mask;
			for (i = 0; i < ETH_VMDQ_MAX_VLAN_FILTERS; i++) {
				if (mirror_conf->vlan.vlan_mask & (1ULL << i))
					mr_info->mr_conf[rule_id].vlan.vlan_id[i] =
						mirror_conf->vlan.vlan_id[i];
			}
		} else {
			mv_lsb = 0;
//			mv_msb = 0;
			mr_info->mr_conf[rule_id].vlan.vlan_mask = 0;
			for (i = 0; i < ETH_VMDQ_MAX_VLAN_FILTERS; i++)
				mr_info->mr_conf[rule_id].vlan.vlan_id[i] = 0;
		}
	}

	/**
	 * if enable pool mirror, write related pool mask register,if disable
	 * pool mirror, clear PFMRVM register
	 */
	if (mirror_conf->rule_type & ETH_MIRROR_VIRTUAL_POOL_UP) {
		mirror_type |= NGBE_MIRRCTL_POOL;
		if (on) {
			mp_lsb = mirror_conf->pool_mask & BIT_MASK32;
//			mp_msb = mirror_conf->pool_mask >> pool_mask_offset;
			mr_info->mr_conf[rule_id].pool_mask =
					mirror_conf->pool_mask;

		} else {
			mp_lsb = 0;
//			mp_msb = 0;
			mr_info->mr_conf[rule_id].pool_mask = 0;
		}
	}
	if (mirror_conf->rule_type & ETH_MIRROR_UPLINK_PORT)
		mirror_type |= NGBE_MIRRCTL_UPLINK;
	if (mirror_conf->rule_type & ETH_MIRROR_DOWNLINK_PORT)
		mirror_type |= NGBE_MIRRCTL_DNLINK;

	/* read  mirror control register and recalculate it */
	mr_ctl = rd32(hw, NGBE_MIRRCTL(rule_id));

	if (on) {
		mr_ctl |= mirror_type;
		mr_ctl &= mirror_rule_mask;
		mr_ctl |= mirror_conf->dst_pool << dst_pool_offset;
	} else {
		mr_ctl &= ~(mirror_conf->rule_type & mirror_rule_mask);
	}

	mr_info->mr_conf[rule_id].rule_type = mirror_conf->rule_type;
	mr_info->mr_conf[rule_id].dst_pool = mirror_conf->dst_pool;

	/* write mirrror control  register */
	wr32(hw, NGBE_MIRRCTL(rule_id), mr_ctl);

	/* write pool mirrror control  register */
	if (mirror_conf->rule_type & ETH_MIRROR_VIRTUAL_POOL_UP) {
		wr32(hw, NGBE_MIRRPOOLL(rule_id), mp_lsb);
//		wr32(hw, NGBE_MIRRPOOLH(rule_id + rule_mr_offset),
//				mp_msb);
	}
	/* write VLAN mirrror control  register */
	if (mirror_conf->rule_type & ETH_MIRROR_VLAN) {
		wr32(hw, NGBE_MIRRVLANL(rule_id), mv_lsb);
//		wr32(hw, NGBE_MIRRVLANH(rule_id + rule_mr_offset),
//				mv_msb);
	}

	return 0;
}

static int
ngbe_mirror_rule_reset(struct rte_eth_dev *dev, uint8_t rule_id)
{
	int mr_ctl = 0;
	uint32_t lsb_val = 0;
//	uint32_t msb_val = 0;

	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_mirror_info *mr_info = NGBE_DEV_MR_INFO(dev);

	if (ngbe_vt_check(hw) < 0)
		return -ENOTSUP;

	if (rule_id >= NGBE_MAX_MIRROR_RULES)
		return -EINVAL;

	memset(&mr_info->mr_conf[rule_id], 0,
	       sizeof(struct rte_eth_mirror_conf));

	/* clear PFVMCTL register */
	wr32(hw, NGBE_MIRRCTL(rule_id), mr_ctl);

	/* clear pool mask register */
	wr32(hw, NGBE_MIRRPOOLL(rule_id), lsb_val);
//	wr32(hw, NGBE_MIRRPOOLH(rule_id + rule_mr_offset), msb_val);

	/* clear vlan mask register */
	wr32(hw, NGBE_MIRRVLANL(rule_id), lsb_val);
//	wr32(hw, NGBE_MIRRVLANH(rule_id + rule_mr_offset), msb_val);

	return 0;
}

static int
ngbe_dev_rx_queue_intr_enable(struct rte_eth_dev *dev, uint16_t queue_id)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	struct rte_intr_handle *intr_handle = &pci_dev->intr_handle;
	uint32_t mask;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	if (queue_id < 32) {
		mask = rd32(hw, NGBE_IMS(0));
		mask &= (1 << queue_id);
		wr32(hw, NGBE_IMS(0), mask);
//	} else if (queue_id < 64) {
//		mask = rd32(hw, NGBE_IMS(1));
//		mask &= (1 << (queue_id - 32));
//		wr32(hw, NGBE_IMS(1), mask);
	}
	rte_intr_enable(intr_handle);

	return 0;
}

static int
ngbe_dev_rx_queue_intr_disable(struct rte_eth_dev *dev, uint16_t queue_id)
{
	uint32_t mask;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	if (queue_id < 32) {
		mask = rd32(hw, NGBE_IMS(0));
		mask &= ~(1 << queue_id);
		wr32(hw, NGBE_IMS(0), mask);
//	} else if (queue_id < 64) {
//		mask = rd32(hw, NGBE_IMS(1));
//		mask &= ~(1 << (queue_id - 32));
//		wr32(hw, NGBE_IMS(1), mask);
	}

	return 0;
}

/**
 * set the IVAR registers, mapping interrupt causes to vectors
 * @param hw
 *  pointer to ngbe_hw struct
 * @direction
 *  0 for Rx, 1 for Tx, -1 for other causes
 * @queue
 *  queue to map the corresponding interrupt to
 * @msix_vector
 *  the vector to map to the corresponding queue
 */
void
ngbe_set_ivar_map(struct ngbe_hw *hw, int8_t direction,
		   uint8_t queue, uint8_t msix_vector)
{
	uint32_t tmp, idx;

	if (direction == -1) {
		/* other causes */
		msix_vector |= NGBE_IVARMISC_VLD;
		idx = 0; //((queue & 1) * 8);
		tmp = rd32(hw, NGBE_IVARMISC);
		tmp &= ~(0xFF << idx);
		tmp |= (msix_vector << idx);
		wr32(hw, NGBE_IVARMISC, tmp);
	} else {
		/* rx or tx causes */
		//msix_vector |= NGBE_IVAR_VALID; /* Workround for ICR lost */
		idx = ((16 * (queue & 1)) + (8 * direction));
		tmp = rd32(hw, NGBE_IVAR(queue >> 1));
		tmp &= ~(0xFF << idx);
		tmp |= (msix_vector << idx);
		wr32(hw, NGBE_IVAR(queue >> 1), tmp);
	}
}

/**
 * Sets up the hardware to properly generate MSI-X interrupts
 * @hw
 *  board private structure
 */
static void
ngbe_configure_msix(struct rte_eth_dev *dev)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	struct rte_intr_handle *intr_handle = &pci_dev->intr_handle;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t queue_id, base = NGBE_MISC_VEC_ID;
	uint32_t vec = NGBE_MISC_VEC_ID;
	uint32_t gpie;

	/* won't configure msix register if no mapping is done
	 * between intr vector and event fd
	 * but if misx has been enabled already, need to configure
	 * auto clean, auto mask and throttling.
	 */
	gpie = rd32(hw, NGBE_GPIE);
	if (!rte_intr_dp_is_en(intr_handle) &&
	    !(gpie & NGBE_GPIE_MSIX))
		return;

	if (rte_intr_allow_others(intr_handle))
		vec = base = NGBE_RX_VEC_START;

	/* setup GPIE for MSI-x mode */
	gpie = rd32(hw, NGBE_GPIE);
	gpie |= NGBE_GPIE_MSIX;
	/* auto clearing and auto setting corresponding bits in EIMS
	 * when MSI-X interrupt is triggered
	 */
	wr32(hw, NGBE_GPIE, gpie);

	/* Populate the IVAR table and set the ITR values to the
	 * corresponding register.
	 */
	if (rte_intr_dp_is_en(intr_handle)) {
		for (queue_id = 0; queue_id < dev->data->nb_rx_queues;
			queue_id++) {
			/* by default, 1:1 mapping */
			ngbe_set_ivar_map(hw, 0, queue_id, vec);
			intr_handle->intr_vec[queue_id] = vec;
			if (vec < base + intr_handle->nb_efd - 1)
				vec++;
		}

		ngbe_set_ivar_map(hw, -1, 1, NGBE_MISC_VEC_ID);
	}
	wr32(hw, NGBE_ITR(NGBE_MISC_VEC_ID),
			NGBE_ITR_IVAL_1G(NGBE_QUEUE_ITR_INTERVAL_DEFAULT)
			| NGBE_ITR_WRDSA);
}

int
ngbe_set_queue_rate_limit(struct rte_eth_dev *dev,
			   uint16_t queue_idx, uint16_t tx_rate)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t bcnrc_val;

	if (queue_idx >= hw->mac.max_tx_queues)
		return -EINVAL;

	if (tx_rate != 0) {
		bcnrc_val = NGBE_ARBTXRATE_MAX(tx_rate);
		bcnrc_val |= NGBE_ARBTXRATE_MIN(tx_rate / 2);
	} else {
		bcnrc_val = 0;
	}

	/*
	 * Set global transmit compensation time to the MMW_SIZE in RTTBCNRM
	 * register. MMW_SIZE=0x014 if 9728-byte jumbo is supported, otherwise
	 * set as 0x4.
	 */
	wr32(hw, NGBE_ARBTXMMW, 0x14);

	/* Set RTTBCNRC of queue X */
	wr32(hw, NGBE_ARBPOOLIDX, queue_idx);
	wr32(hw, NGBE_ARBTXRATE, bcnrc_val);
	ngbe_flush(hw);

	return 0;
}

int
ngbe_syn_filter_set(struct rte_eth_dev *dev,
			struct rte_eth_syn_filter *filter,
			bool add)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	uint32_t syn_info;
	uint32_t synqf;

	if (filter->queue >= NGBE_MAX_RX_QUEUE_NUM)
		return -EINVAL;

	syn_info = filter_info->syn_info;

	if (add) {
		if (syn_info & NGBE_SYNCLS_ENA)
			return -EINVAL;
		synqf = (uint32_t)NGBE_SYNCLS_QPID(filter->queue);
		synqf |= NGBE_SYNCLS_ENA;

		if (filter->hig_pri)
			synqf |= NGBE_SYNCLS_HIPRIO;
		else
			synqf &= ~NGBE_SYNCLS_HIPRIO;
	} else {
		synqf = rd32(hw, NGBE_SYNCLS);
		if (!(syn_info & NGBE_SYNCLS_ENA))
			return -ENOENT;
		synqf &= ~(NGBE_SYNCLS_QPID_MASK | NGBE_SYNCLS_ENA);
	}

	filter_info->syn_info = synqf;
	wr32(hw, NGBE_SYNCLS, synqf);
	ngbe_flush(hw);
	return 0;
}

static int
ngbe_syn_filter_get(struct rte_eth_dev *dev,
			struct rte_eth_syn_filter *filter)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t synqf = rd32(hw, NGBE_SYNCLS);

	if (synqf & NGBE_SYNCLS_ENA) {
		filter->hig_pri = (synqf & NGBE_SYNCLS_HIPRIO) ? 1 : 0;
		filter->queue = (uint16_t)NGBD_SYNCLS_QPID(synqf);
		return 0;
	}
	return -ENOENT;
}

static int
ngbe_syn_filter_handle(struct rte_eth_dev *dev,
			enum rte_filter_op filter_op,
			void *arg)
{
	int ret;

	if (filter_op == RTE_ETH_FILTER_NOP)
		return 0;

	if (arg == NULL) {
		PMD_DRV_LOG(ERR, "arg shouldn't be NULL for operation %u",
			    filter_op);
		return -EINVAL;
	}

	switch (filter_op) {
	case RTE_ETH_FILTER_ADD:
		ret = ngbe_syn_filter_set(dev,
				(struct rte_eth_syn_filter *)arg,
				TRUE);
		break;
	case RTE_ETH_FILTER_DELETE:
		ret = ngbe_syn_filter_set(dev,
				(struct rte_eth_syn_filter *)arg,
				FALSE);
		break;
	case RTE_ETH_FILTER_GET:
		ret = ngbe_syn_filter_get(dev,
				(struct rte_eth_syn_filter *)arg);
		break;
	default:
		PMD_DRV_LOG(ERR, "unsupported operation %u", filter_op);
		ret = -EINVAL;
		break;
	}

	return ret;
}


static inline enum ngbe_5tuple_protocol
convert_protocol_type(uint8_t protocol_value)
{
	if (protocol_value == IPPROTO_TCP)
		return NGBE_5TF_PROT_TCP;
	else if (protocol_value == IPPROTO_UDP)
		return NGBE_5TF_PROT_UDP;
	else if (protocol_value == IPPROTO_SCTP)
		return NGBE_5TF_PROT_SCTP;
	else
		return NGBE_5TF_PROT_NONE;
}

/* inject a 5-tuple filter to HW */
static inline void
ngbe_inject_5tuple_filter(struct rte_eth_dev *dev,
			   struct ngbe_5tuple_filter *filter)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	int i;
	uint32_t ftqf, sdpqf;
	uint32_t l34timir = 0;
	uint32_t mask = NGBE_5TFCTL0_MASK;

	i = filter->index;
	sdpqf = NGBE_5TFPORT_DST(be_to_le16(filter->filter_info.dst_port));
	sdpqf |= NGBE_5TFPORT_SRC(be_to_le16(filter->filter_info.src_port));

	ftqf = NGBE_5TFCTL0_PROTO(filter->filter_info.proto);
	ftqf |= NGBE_5TFCTL0_PRI(filter->filter_info.priority);
//	if (filter->filter_info.src_ip_mask == 0) /* 0 means compare. */
//		mask &= ~NGBE_5TFCTL0_MSADDR;
//	if (filter->filter_info.dst_ip_mask == 0)
//		mask &= ~NGBE_5TFCTL0_MDADDR;
	if (filter->filter_info.src_port_mask == 0)
		mask &= ~NGBE_5TFCTL0_MSPORT;
	if (filter->filter_info.dst_port_mask == 0)
		mask &= ~NGBE_5TFCTL0_MDPORT;
	if (filter->filter_info.proto_mask == 0)
		mask &= ~NGBE_5TFCTL0_MPROTO;
	ftqf |= mask;
	ftqf |= NGBE_5TFCTL0_MPOOL;
	ftqf |= NGBE_5TFCTL0_ENA;

//	wr32(hw, NGBE_5TFDADDR(i), be_to_le32(filter->filter_info.dst_ip));
//	wr32(hw, NGBE_5TFSADDR(i), be_to_le32(filter->filter_info.src_ip));
	wr32(hw, NGBE_5TFPORT(i), sdpqf);
	wr32(hw, NGBE_5TFCTL0(i), ftqf);

	l34timir |= NGBE_5TFCTL1_QP(filter->queue);
	wr32(hw, NGBE_5TFCTL1(i), l34timir);
}

/*
 * add a 5tuple filter
 *
 * @param
 * dev: Pointer to struct rte_eth_dev.
 * index: the index the filter allocates.
 * filter: ponter to the filter that will be added.
 * rx_queue: the queue id the filter assigned to.
 *
 * @return
 *    - On success, zero.
 *    - On failure, a negative value.
 */
static int
ngbe_add_5tuple_filter(struct rte_eth_dev *dev,
			struct ngbe_5tuple_filter *filter)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	int i, idx, shift;

	/*
	 * look for an unused 5tuple filter index,
	 * and insert the filter to list.
	 */
	for (i = 0; i < NGBE_MAX_FTQF_FILTERS; i++) {
		idx = i / (sizeof(uint32_t) * NBBY);
		shift = i % (sizeof(uint32_t) * NBBY);
		if (!(filter_info->fivetuple_mask[idx] & (1 << shift))) {
			filter_info->fivetuple_mask[idx] |= 1 << shift;
			filter->index = i;
			TAILQ_INSERT_TAIL(&filter_info->fivetuple_list,
					  filter,
					  entries);
			break;
		}
	}
	if (i >= NGBE_MAX_FTQF_FILTERS) {
		PMD_DRV_LOG(ERR, "5tuple filters are full.");
		return -ENOSYS;
	}

	ngbe_inject_5tuple_filter(dev, filter);

	return 0;
}

/*
 * remove a 5tuple filter
 *
 * @param
 * dev: Pointer to struct rte_eth_dev.
 * filter: the pointer of the filter will be removed.
 */
static void
ngbe_remove_5tuple_filter(struct rte_eth_dev *dev,
			struct ngbe_5tuple_filter *filter)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	uint16_t index = filter->index;

	filter_info->fivetuple_mask[index / (sizeof(uint32_t) * NBBY)] &=
				~(1 << (index % (sizeof(uint32_t) * NBBY)));
	TAILQ_REMOVE(&filter_info->fivetuple_list, filter, entries);
	rte_free(filter);

//	wr32(hw, NGBE_5TFDADDR(index), 0);
//	wr32(hw, NGBE_5TFSADDR(index), 0);
	wr32(hw, NGBE_5TFPORT(index), 0);
	wr32(hw, NGBE_5TFCTL0(index), 0);
	wr32(hw, NGBE_5TFCTL1(index), 0);
}



static inline struct ngbe_5tuple_filter *
ngbe_5tuple_filter_lookup(struct ngbe_5tuple_filter_list *filter_list,
			struct ngbe_5tuple_filter_info *key)
{
	struct ngbe_5tuple_filter *it;

	TAILQ_FOREACH(it, filter_list, entries) {
		if (memcmp(key, &it->filter_info,
			sizeof(struct ngbe_5tuple_filter_info)) == 0) {
			return it;
		}
	}
	return NULL;
}

/* translate elements in struct rte_eth_ntuple_filter to struct ngbe_5tuple_filter_info*/
static inline int
ntuple_filter_to_5tuple(struct rte_eth_ntuple_filter *filter,
			struct ngbe_5tuple_filter_info *filter_info)
{
	if (filter->queue >= NGBE_MAX_RX_QUEUE_NUM ||
		filter->priority > NGBE_5TUPLE_MAX_PRI ||
		filter->priority < NGBE_5TUPLE_MIN_PRI)
		return -EINVAL;

	switch (filter->dst_ip_mask) {
	case UINT32_MAX:
		filter_info->dst_ip_mask = 0;
		filter_info->dst_ip = filter->dst_ip;
		break;
	case 0:
		filter_info->dst_ip_mask = 1;
		break;
	default:
		PMD_DRV_LOG(ERR, "invalid dst_ip mask.");
		return -EINVAL;
	}

	switch (filter->src_ip_mask) {
	case UINT32_MAX:
		filter_info->src_ip_mask = 0;
		filter_info->src_ip = filter->src_ip;
		break;
	case 0:
		filter_info->src_ip_mask = 1;
		break;
	default:
		PMD_DRV_LOG(ERR, "invalid src_ip mask.");
		return -EINVAL;
	}

	switch (filter->dst_port_mask) {
	case UINT16_MAX:
		filter_info->dst_port_mask = 0;
		filter_info->dst_port = filter->dst_port;
		break;
	case 0:
		filter_info->dst_port_mask = 1;
		break;
	default:
		PMD_DRV_LOG(ERR, "invalid dst_port mask.");
		return -EINVAL;
	}

	switch (filter->src_port_mask) {
	case UINT16_MAX:
		filter_info->src_port_mask = 0;
		filter_info->src_port = filter->src_port;
		break;
	case 0:
		filter_info->src_port_mask = 1;
		break;
	default:
		PMD_DRV_LOG(ERR, "invalid src_port mask.");
		return -EINVAL;
	}

	switch (filter->proto_mask) {
	case UINT8_MAX:
		filter_info->proto_mask = 0;
		filter_info->proto =
			convert_protocol_type(filter->proto);
		break;
	case 0:
		filter_info->proto_mask = 1;
		break;
	default:
		PMD_DRV_LOG(ERR, "invalid protocol mask.");
		return -EINVAL;
	}

	filter_info->priority = (uint8_t)filter->priority;
	return 0;
}

/*
 * add or delete a ntuple filter
 *
 * @param
 * dev: Pointer to struct rte_eth_dev.
 * ntuple_filter: Pointer to struct rte_eth_ntuple_filter
 * add: if true, add filter, if false, remove filter
 *
 * @return
 *    - On success, zero.
 *    - On failure, a negative value.
 */
int
ngbe_add_del_ntuple_filter(struct rte_eth_dev *dev,
			struct rte_eth_ntuple_filter *ntuple_filter,
			bool add)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	struct ngbe_5tuple_filter_info filter_5tuple;
	struct ngbe_5tuple_filter *filter;
	int ret;

	if (ntuple_filter->flags != RTE_5TUPLE_FLAGS) {
		PMD_DRV_LOG(ERR, "only 5tuple is supported.");
		return -EINVAL;
	}

	memset(&filter_5tuple, 0, sizeof(struct ngbe_5tuple_filter_info));
	ret = ntuple_filter_to_5tuple(ntuple_filter, &filter_5tuple);
	if (ret < 0)
		return ret;

	filter = ngbe_5tuple_filter_lookup(&filter_info->fivetuple_list,
					 &filter_5tuple);
	if (filter != NULL && add) {
		PMD_DRV_LOG(ERR, "filter exists.");
		return -EEXIST;
	}
	if (filter == NULL && !add) {
		PMD_DRV_LOG(ERR, "filter doesn't exist.");
		return -ENOENT;
	}

	if (add) {
		filter = rte_zmalloc("ngbe_5tuple_filter",
				sizeof(struct ngbe_5tuple_filter), 0);
		if (filter == NULL)
			return -ENOMEM;
		rte_memcpy(&filter->filter_info,
				 &filter_5tuple,
				 sizeof(struct ngbe_5tuple_filter_info));
		filter->queue = ntuple_filter->queue;
		ret = ngbe_add_5tuple_filter(dev, filter);
		if (ret < 0) {
			rte_free(filter);
			return ret;
		}
	} else {
		ngbe_remove_5tuple_filter(dev, filter);
	}

	return 0;
}

/*
 * get a ntuple filter
 *
 * @param
 * dev: Pointer to struct rte_eth_dev.
 * ntuple_filter: Pointer to struct rte_eth_ntuple_filter
 *
 * @return
 *    - On success, zero.
 *    - On failure, a negative value.
 */
static int
ngbe_get_ntuple_filter(struct rte_eth_dev *dev,
			struct rte_eth_ntuple_filter *ntuple_filter)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	struct ngbe_5tuple_filter_info filter_5tuple;
	struct ngbe_5tuple_filter *filter;
	int ret;

	if (ntuple_filter->flags != RTE_5TUPLE_FLAGS) {
		PMD_DRV_LOG(ERR, "only 5tuple is supported.");
		return -EINVAL;
	}

	memset(&filter_5tuple, 0, sizeof(struct ngbe_5tuple_filter_info));
	ret = ntuple_filter_to_5tuple(ntuple_filter, &filter_5tuple);
	if (ret < 0)
		return ret;

	filter = ngbe_5tuple_filter_lookup(&filter_info->fivetuple_list,
					 &filter_5tuple);
	if (filter == NULL) {
		PMD_DRV_LOG(ERR, "filter doesn't exist.");
		return -ENOENT;
	}
	ntuple_filter->queue = filter->queue;
	return 0;
}

/*
 * ngbe_ntuple_filter_handle - Handle operations for ntuple filter.
 * @dev: pointer to rte_eth_dev structure
 * @filter_op:operation will be taken.
 * @arg: a pointer to specific structure corresponding to the filter_op
 *
 * @return
 *    - On success, zero.
 *    - On failure, a negative value.
 */
static int
ngbe_ntuple_filter_handle(struct rte_eth_dev *dev,
				enum rte_filter_op filter_op,
				void *arg)
{
	int ret;

	if (filter_op == RTE_ETH_FILTER_NOP)
		return 0;

	if (arg == NULL) {
		PMD_DRV_LOG(ERR, "arg shouldn't be NULL for operation %u.",
			    filter_op);
		return -EINVAL;
	}

	switch (filter_op) {
	case RTE_ETH_FILTER_ADD:
		ret = ngbe_add_del_ntuple_filter(dev,
			(struct rte_eth_ntuple_filter *)arg,
			TRUE);
		break;
	case RTE_ETH_FILTER_DELETE:
		ret = ngbe_add_del_ntuple_filter(dev,
			(struct rte_eth_ntuple_filter *)arg,
			FALSE);
		break;
	case RTE_ETH_FILTER_GET:
		ret = ngbe_get_ntuple_filter(dev,
			(struct rte_eth_ntuple_filter *)arg);
		break;
	default:
		PMD_DRV_LOG(ERR, "unsupported operation %u.", filter_op);
		ret = -EINVAL;
		break;
	}
	return ret;
}

int
ngbe_add_del_ethertype_filter(struct rte_eth_dev *dev,
			struct rte_eth_ethertype_filter *filter,
			bool add)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	uint32_t etqf = 0;
	uint32_t etqs = 0;
	int ret;
	struct ngbe_ethertype_filter ethertype_filter;

	if (filter->queue >= NGBE_MAX_RX_QUEUE_NUM)
		return -EINVAL;

	if (filter->ether_type == ETHER_TYPE_IPv4 ||
	    filter->ether_type == ETHER_TYPE_IPv6) {
		PMD_DRV_LOG(ERR, "unsupported ether_type(0x%04x) in"
			" ethertype filter.", filter->ether_type);
		return -EINVAL;
	}

	if (filter->flags & RTE_ETHTYPE_FLAGS_MAC) {
		PMD_DRV_LOG(ERR, "mac compare is unsupported.");
		return -EINVAL;
	}
	if (filter->flags & RTE_ETHTYPE_FLAGS_DROP) {
		PMD_DRV_LOG(ERR, "drop option is unsupported.");
		return -EINVAL;
	}

	ret = ngbe_ethertype_filter_lookup(filter_info, filter->ether_type);
	if (ret >= 0 && add) {
		PMD_DRV_LOG(ERR, "ethertype (0x%04x) filter exists.",
			    filter->ether_type);
		return -EEXIST;
	}
	if (ret < 0 && !add) {
		PMD_DRV_LOG(ERR, "ethertype (0x%04x) filter doesn't exist.",
			    filter->ether_type);
		return -ENOENT;
	}

	if (add) {
		etqf = NGBE_ETFLT_ENA;
		etqf |= NGBE_ETFLT_ETID(filter->ether_type);
		etqs |= NGBE_ETCLS_QPID(filter->queue);
		etqs |= NGBE_ETCLS_QENA;

		ethertype_filter.ethertype = filter->ether_type;
		ethertype_filter.etqf = etqf;
		ethertype_filter.etqs = etqs;
		ethertype_filter.conf = FALSE;
		ret = ngbe_ethertype_filter_insert(filter_info,
						    &ethertype_filter);
		if (ret < 0) {
			PMD_DRV_LOG(ERR, "ethertype filters are full.");
			return -ENOSPC;
		}
	} else {
		ret = ngbe_ethertype_filter_remove(filter_info, (uint8_t)ret);
		if (ret < 0)
			return -ENOSYS;
	}
	wr32(hw, NGBE_ETFLT(ret), etqf);
	wr32(hw, NGBE_ETCLS(ret), etqs);
	ngbe_flush(hw);

	return 0;
}

static int
ngbe_get_ethertype_filter(struct rte_eth_dev *dev,
			struct rte_eth_ethertype_filter *filter)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	uint32_t etqf, etqs;
	int ret;

	ret = ngbe_ethertype_filter_lookup(filter_info, filter->ether_type);
	if (ret < 0) {
		PMD_DRV_LOG(ERR, "ethertype (0x%04x) filter doesn't exist.",
			    filter->ether_type);
		return -ENOENT;
	}

	etqf = rd32(hw, NGBE_ETFLT(ret));
	if (etqf & NGBE_ETFLT_ENA) {
		etqs = rd32(hw, NGBE_ETCLS(ret));
		filter->ether_type = etqf & NGBE_ETFLT_ETID_MASK;
		filter->flags = 0;
		filter->queue = NGBD_ETCLS_QPID(etqs);
		return 0;
	}
	return -ENOENT;
}

/*
 * ngbe_ethertype_filter_handle - Handle operations for ethertype filter.
 * @dev: pointer to rte_eth_dev structure
 * @filter_op:operation will be taken.
 * @arg: a pointer to specific structure corresponding to the filter_op
 */
static int
ngbe_ethertype_filter_handle(struct rte_eth_dev *dev,
				enum rte_filter_op filter_op,
				void *arg)
{
	int ret;

	if (filter_op == RTE_ETH_FILTER_NOP)
		return 0;

	if (arg == NULL) {
		PMD_DRV_LOG(ERR, "arg shouldn't be NULL for operation %u.",
			    filter_op);
		return -EINVAL;
	}

	switch (filter_op) {
	case RTE_ETH_FILTER_ADD:
		ret = ngbe_add_del_ethertype_filter(dev,
			(struct rte_eth_ethertype_filter *)arg,
			TRUE);
		break;
	case RTE_ETH_FILTER_DELETE:
		ret = ngbe_add_del_ethertype_filter(dev,
			(struct rte_eth_ethertype_filter *)arg,
			FALSE);
		break;
	case RTE_ETH_FILTER_GET:
		ret = ngbe_get_ethertype_filter(dev,
			(struct rte_eth_ethertype_filter *)arg);
		break;
	default:
		PMD_DRV_LOG(ERR, "unsupported operation %u.", filter_op);
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int
ngbe_dev_filter_ctrl(struct rte_eth_dev *dev,
		     enum rte_filter_type filter_type,
		     enum rte_filter_op filter_op,
		     void *arg)
{
	int ret = 0;

	switch (filter_type) {
	case RTE_ETH_FILTER_NTUPLE:
		ret = ngbe_ntuple_filter_handle(dev, filter_op, arg);
		break;
	case RTE_ETH_FILTER_ETHERTYPE:
		ret = ngbe_ethertype_filter_handle(dev, filter_op, arg);
		break;
	case RTE_ETH_FILTER_SYN:
		ret = ngbe_syn_filter_handle(dev, filter_op, arg);
		break;
	case RTE_ETH_FILTER_L2_TUNNEL:
		ret = ngbe_dev_l2_tunnel_filter_handle(dev, filter_op, arg);
		break;
	case RTE_ETH_FILTER_GENERIC:
		if (filter_op != RTE_ETH_FILTER_GET)
			return -EINVAL;
		*(const void **)arg = &ngbe_flow_ops;
		break;
	default:
		PMD_DRV_LOG(WARNING, "Filter type (%d) not supported",
							filter_type);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static u8 *
ngbe_dev_addr_list_itr(__attribute__((unused)) struct ngbe_hw *hw,
			u8 **mc_addr_ptr, u32 *vmdq)
{
	u8 *mc_addr;

	*vmdq = 0;
	mc_addr = *mc_addr_ptr;
	*mc_addr_ptr = (mc_addr + sizeof(struct ether_addr));
	return mc_addr;
}

int
ngbe_dev_set_mc_addr_list(struct rte_eth_dev *dev,
			  struct ether_addr *mc_addr_set,
			  uint32_t nb_mc_addr)
{
	struct ngbe_hw *hw;
	u8 *mc_addr_list;

	hw = NGBE_DEV_HW(dev);
	mc_addr_list = (u8 *)mc_addr_set;
	return hw->mac.update_mc_addr_list(hw, mc_addr_list, nb_mc_addr,
					 ngbe_dev_addr_list_itr, TRUE);
}

static uint64_t
ngbe_read_systime_cyclecounter(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint64_t systime_cycles;

	systime_cycles = (uint64_t)rd32(hw, NGBE_TSTIMEL);
	systime_cycles |= (uint64_t)rd32(hw, NGBE_TSTIMEH) << 32;

	return systime_cycles;
}

static uint64_t
ngbe_read_rx_tstamp_cyclecounter(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint64_t rx_tstamp_cycles;

	/* RXSTMPL stores ns and RXSTMPH stores seconds. */
	rx_tstamp_cycles = (uint64_t)rd32(hw, NGBE_TSRXSTMPL);
	rx_tstamp_cycles |= (uint64_t)rd32(hw, NGBE_TSRXSTMPH) << 32;

	return rx_tstamp_cycles;
}

static uint64_t
ngbe_read_tx_tstamp_cyclecounter(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint64_t tx_tstamp_cycles;

	/* TXSTMPL stores ns and TXSTMPH stores seconds. */
	tx_tstamp_cycles = (uint64_t)rd32(hw, NGBE_TSTXSTMPL);
	tx_tstamp_cycles |= (uint64_t)rd32(hw, NGBE_TSTXSTMPH) << 32;

	return tx_tstamp_cycles;
}

static void
ngbe_start_timecounters(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);
	struct rte_eth_link link;
	uint32_t incval = 0;
	uint32_t shift = 0;

	/* Get current link speed. */
	ngbe_dev_link_update(dev, 1);
	rte_eth_linkstatus_get(dev, &link);

	switch (link.link_speed) {
	case ETH_SPEED_NUM_100M:
		incval = NGBE_INCVAL_100;
		shift = NGBE_INCVAL_SHIFT_100;
		break;
	case ETH_SPEED_NUM_1G:
		incval = NGBE_INCVAL_1GB;
		shift = NGBE_INCVAL_SHIFT_1GB;
		break;
	case ETH_SPEED_NUM_10G:
	default:
		incval = NGBE_INCVAL_10GB;
		shift = NGBE_INCVAL_SHIFT_10GB;
		break;
	}

	wr32(hw, NGBE_TSTIMEINC, NGBE_TSTIMEINC_IV(incval));

	memset(&adapter->systime_tc, 0, sizeof(struct rte_timecounter));
	memset(&adapter->rx_tstamp_tc, 0, sizeof(struct rte_timecounter));
	memset(&adapter->tx_tstamp_tc, 0, sizeof(struct rte_timecounter));

	adapter->systime_tc.cc_mask = NGBE_CYCLECOUNTER_MASK;
	adapter->systime_tc.cc_shift = shift;
	adapter->systime_tc.nsec_mask = (1ULL << shift) - 1;

	adapter->rx_tstamp_tc.cc_mask = NGBE_CYCLECOUNTER_MASK;
	adapter->rx_tstamp_tc.cc_shift = shift;
	adapter->rx_tstamp_tc.nsec_mask = (1ULL << shift) - 1;

	adapter->tx_tstamp_tc.cc_mask = NGBE_CYCLECOUNTER_MASK;
	adapter->tx_tstamp_tc.cc_shift = shift;
	adapter->tx_tstamp_tc.nsec_mask = (1ULL << shift) - 1;
}

static int
ngbe_timesync_adjust_time(struct rte_eth_dev *dev, int64_t delta)
{
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);

	adapter->systime_tc.nsec += delta;
	adapter->rx_tstamp_tc.nsec += delta;
	adapter->tx_tstamp_tc.nsec += delta;

	return 0;
}

static int
ngbe_timesync_write_time(struct rte_eth_dev *dev, const struct timespec *ts)
{
	uint64_t ns;
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);

	ns = rte_timespec_to_ns(ts);
	/* Set the timecounters to a new value. */
	adapter->systime_tc.nsec = ns;
	adapter->rx_tstamp_tc.nsec = ns;
	adapter->tx_tstamp_tc.nsec = ns;

	return 0;
}

static int
ngbe_timesync_read_time(struct rte_eth_dev *dev, struct timespec *ts)
{
	uint64_t ns, systime_cycles;
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);

	systime_cycles = ngbe_read_systime_cyclecounter(dev);
	ns = rte_timecounter_update(&adapter->systime_tc, systime_cycles);
	*ts = rte_ns_to_timespec(ns);

	return 0;
}

static int
ngbe_timesync_enable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t tsync_ctl;

	/* Stop the timesync system time. */
	wr32(hw, NGBE_TSTIMEINC, 0x0);
	/* Reset the timesync system time value. */
	wr32(hw, NGBE_TSTIMEL, 0x0);
	wr32(hw, NGBE_TSTIMEH, 0x0);

	/* Enable system time for platforms where it isn't on by default. */

	ngbe_start_timecounters(dev);

	/* Enable L2 filtering of IEEE1588/802.1AS Ethernet frame types. */
	wr32(hw, NGBE_ETFLT(NGBE_ETF_ID_1588),
		ETHER_TYPE_1588 | NGBE_ETFLT_ENA | NGBE_ETFLT_1588);

	/* Enable timestamping of received PTP packets. */
	tsync_ctl = rd32(hw, NGBE_TSRXCTL);
	tsync_ctl |= NGBE_TSRXCTL_ENA;
	wr32(hw, NGBE_TSRXCTL, tsync_ctl);

	/* Enable timestamping of transmitted PTP packets. */
	tsync_ctl = rd32(hw, NGBE_TSTXCTL);
	tsync_ctl |= NGBE_TSTXCTL_ENA;
	wr32(hw, NGBE_TSTXCTL, tsync_ctl);

	ngbe_flush(hw);

	return 0;
}

static int
ngbe_timesync_disable(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t tsync_ctl;

	/* Disable timestamping of transmitted PTP packets. */
	tsync_ctl = rd32(hw, NGBE_TSTXCTL);
	tsync_ctl &= ~NGBE_TSTXCTL_ENA;
	wr32(hw, NGBE_TSTXCTL, tsync_ctl);

	/* Disable timestamping of received PTP packets. */
	tsync_ctl = rd32(hw, NGBE_TSRXCTL);
	tsync_ctl &= ~NGBE_TSRXCTL_ENA;
	wr32(hw, NGBE_TSRXCTL, tsync_ctl);

	/* Disable L2 filtering of IEEE1588/802.1AS Ethernet frame types. */
	wr32(hw, NGBE_ETFLT(NGBE_ETF_ID_1588), 0);

	/* Stop incrementating the System Time registers. */
	wr32(hw, NGBE_TSTIMEINC, 0);

	return 0;
}

static int
ngbe_timesync_read_rx_timestamp(struct rte_eth_dev *dev,
				 struct timespec *timestamp,
				 uint32_t flags __rte_unused)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);
	uint32_t tsync_rxctl;
	uint64_t rx_tstamp_cycles;
	uint64_t ns;

	tsync_rxctl = rd32(hw, NGBE_TSRXCTL);
	if ((tsync_rxctl & NGBE_TSRXCTL_VLD) == 0)
		return -EINVAL;

	rx_tstamp_cycles = ngbe_read_rx_tstamp_cyclecounter(dev);
	ns = rte_timecounter_update(&adapter->rx_tstamp_tc, rx_tstamp_cycles);
	*timestamp = rte_ns_to_timespec(ns);

	return  0;
}

static int
ngbe_timesync_read_tx_timestamp(struct rte_eth_dev *dev,
				 struct timespec *timestamp)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_adapter *adapter = NGBE_DEV_ADAPTER(dev);
	uint32_t tsync_txctl;
	uint64_t tx_tstamp_cycles;
	uint64_t ns;

	tsync_txctl = rd32(hw, NGBE_TSTXCTL);
	if ((tsync_txctl & NGBE_TSTXCTL_VLD) == 0)
		return -EINVAL;

	tx_tstamp_cycles = ngbe_read_tx_tstamp_cyclecounter(dev);
	ns = rte_timecounter_update(&adapter->tx_tstamp_tc, tx_tstamp_cycles);
	*timestamp = rte_ns_to_timespec(ns);

	return 0;
}

static int
ngbe_get_reg_length(struct rte_eth_dev *dev __rte_unused)
{
	int count = 0;
	int g_ind = 0;
	const struct reg_info *reg_group;
	const struct reg_info **reg_set = ngbe_regs_others;

	while ((reg_group = reg_set[g_ind++]))
		count += ngbe_regs_group_count(reg_group);

	return count;
}

static int
ngbe_get_regs(struct rte_eth_dev *dev,
	      struct rte_dev_reg_info *regs)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t *data = regs->data;
	int g_ind = 0;
	int count = 0;
	const struct reg_info *reg_group;
	const struct reg_info **reg_set = ngbe_regs_others;

	if (data == NULL) {
		regs->length = ngbe_get_reg_length(dev);
		regs->width = sizeof(uint32_t);
		return 0;
	}

	/* Support only full register dump */
	if ((regs->length == 0) ||
	    (regs->length == (uint32_t)ngbe_get_reg_length(dev))) {
		regs->version = hw->mac.type << 24 |
				hw->revision_id << 16 |
				hw->device_id;
		while ((reg_group = reg_set[g_ind++]))
			count += ngbe_read_regs_group(dev, &data[count],
						      reg_group);
		return 0;
	}

	return -ENOTSUP;
}

static int
ngbe_get_eeprom_length(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	/* Return unit is byte count */
	return hw->rom.word_size * 2;
}

static int
ngbe_get_eeprom(struct rte_eth_dev *dev,
		struct rte_dev_eeprom_info *in_eeprom)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_rom_info *eeprom = &hw->rom;
	uint16_t *data = in_eeprom->data;
	int first, length;

	first = in_eeprom->offset >> 1;
	length = in_eeprom->length >> 1;
	if ((first > hw->rom.word_size) ||
	    ((first + length) > hw->rom.word_size))
		return -EINVAL;

	in_eeprom->magic = hw->vendor_id | (hw->device_id << 16);

	return eeprom->readw_buffer(hw, first, length, data);
}

static int
ngbe_set_eeprom(struct rte_eth_dev *dev,
		struct rte_dev_eeprom_info *in_eeprom)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_rom_info *eeprom = &hw->rom;
	uint16_t *data = in_eeprom->data;
	int first, length;

	first = in_eeprom->offset >> 1;
	length = in_eeprom->length >> 1;
	if ((first > hw->rom.word_size) ||
	    ((first + length) > hw->rom.word_size))
		return -EINVAL;

	in_eeprom->magic = hw->vendor_id | (hw->device_id << 16);

	return eeprom->writew_buffer(hw,  first, length, data);
}

#if RTE_VERSION >= RTE_VERSION_NUM(18, 5, 0, 0)
static int
ngbe_get_module_info(struct rte_eth_dev *dev,
		      struct rte_eth_dev_module_info *modinfo)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t status;
	uint8_t sff8472_rev, addr_mode;
	bool page_swap = false;

	/* Check whether we support SFF-8472 or not */
	status = hw->phy.read_i2c_eeprom(hw,
					     NGBE_SFF_SFF_8472_COMP,
					     &sff8472_rev);
	if (status != 0)
		return -EIO;

	/* addressing mode is not supported */
	status = hw->phy.read_i2c_eeprom(hw,
					     NGBE_SFF_SFF_8472_SWAP,
					     &addr_mode);
	if (status != 0)
		return -EIO;

	if (addr_mode & NGBE_SFF_ADDRESSING_MODE) {
		PMD_DRV_LOG(ERR,
			    "Address change required to access page 0xA2, "
			    "but not supported. Please report the module "
			    "type to the driver maintainers.");
		page_swap = true;
	}

	if (sff8472_rev == NGBE_SFF_SFF_8472_UNSUP || page_swap) {
		/* We have a SFP, but it does not support SFF-8472 */
		modinfo->type = RTE_ETH_MODULE_SFF_8079;
		modinfo->eeprom_len = RTE_ETH_MODULE_SFF_8079_LEN;
	} else {
		/* We have a SFP which supports a revision of SFF-8472. */
		modinfo->type = RTE_ETH_MODULE_SFF_8472;
		modinfo->eeprom_len = RTE_ETH_MODULE_SFF_8472_LEN;
	}

	return 0;
}

static int
ngbe_get_module_eeprom(struct rte_eth_dev *dev,
			struct rte_dev_eeprom_info *info)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t status = NGBE_ERR_PHY_ADDR_INVALID;
	uint8_t databyte = 0xFF;
	uint8_t *data = info->data;
	uint32_t i = 0;

	if (info->length == 0)
		return -EINVAL;

	for (i = info->offset; i < info->offset + info->length; i++) {
		if (i < RTE_ETH_MODULE_SFF_8079_LEN)
			status = hw->phy.read_i2c_eeprom(hw, i, &databyte);
		else
			status = hw->phy.read_i2c_sff8472(hw, i, &databyte);

		if (status != 0)
			return -EIO;

		data[i - info->offset] = databyte;
	}

	return 0;
}
#endif /* RTE_VERSION >= RTE_VERSION_NUM(18, 5, 0, 0) */

bool
ngbe_rss_update_sp(enum ngbe_mac_type mac_type)
{
	switch (mac_type) {
	case ngbe_mac_em:
		return 1;
	default:
		return 0;
	}
}

/* Update e-tag ether type */
static int
ngbe_update_e_tag_eth_type(struct ngbe_hw *hw,
			    uint16_t ether_type)
{
	uint32_t etag_etype;

	etag_etype = rd32(hw, NGBE_EXTAG);
	etag_etype &= ~NGBE_EXTAG_ETAG_MASK;
	etag_etype |= ether_type;
	wr32(hw, NGBE_EXTAG, etag_etype);
	ngbe_flush(hw);

	return 0;
}

/* Config l2 tunnel ether type */
static int
ngbe_dev_l2_tunnel_eth_type_conf(struct rte_eth_dev *dev,
				  struct rte_eth_l2_tunnel_conf *l2_tunnel)
{
	int ret = 0;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);

	if (l2_tunnel == NULL)
		return -EINVAL;

	switch (l2_tunnel->l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		l2_tn_info->e_tag_ether_type = l2_tunnel->ether_type;
		ret = ngbe_update_e_tag_eth_type(hw, l2_tunnel->ether_type);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Enable e-tag tunnel */
static int
ngbe_e_tag_enable(struct ngbe_hw *hw)
{
	uint32_t etag_etype;

	etag_etype = rd32(hw, NGBE_PORTCTL);
	etag_etype |= NGBE_PORTCTL_ETAG;
	wr32(hw, NGBE_PORTCTL, etag_etype);
	ngbe_flush(hw);

	return 0;
}

/* Enable l2 tunnel */
static int
ngbe_dev_l2_tunnel_enable(struct rte_eth_dev *dev,
			   enum rte_eth_tunnel_type l2_tunnel_type)
{
	int ret = 0;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);

	switch (l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		l2_tn_info->e_tag_en = TRUE;
		ret = ngbe_e_tag_enable(hw);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Disable e-tag tunnel */
static int
ngbe_e_tag_disable(struct ngbe_hw *hw)
{
	uint32_t etag_etype;

	etag_etype = rd32(hw, NGBE_PORTCTL);
	etag_etype &= ~NGBE_PORTCTL_ETAG;
	wr32(hw, NGBE_PORTCTL, etag_etype);
	ngbe_flush(hw);

	return 0;
}

/* Disable l2 tunnel */
static int
ngbe_dev_l2_tunnel_disable(struct rte_eth_dev *dev,
			    enum rte_eth_tunnel_type l2_tunnel_type)
{
	int ret = 0;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);

	switch (l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		l2_tn_info->e_tag_en = FALSE;
		ret = ngbe_e_tag_disable(hw);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int
ngbe_e_tag_filter_del(struct rte_eth_dev *dev,
		       struct rte_eth_l2_tunnel_conf *l2_tunnel)
{
	int ret = 0;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t i, rar_entries;
	uint32_t rar_low, rar_high;

	rar_entries = hw->mac.num_rar_entries;

	for (i = 1; i < rar_entries; i++) {
		wr32(hw, NGBE_ETHADDRIDX, i);
		rar_high = rd32(hw, NGBE_ETHADDRH);
		rar_low  = rd32(hw, NGBE_ETHADDRL);
		if ((rar_high & NGBE_ETHADDRH_VLD) &&
		    (rar_high & NGBE_ETHADDRH_ETAG) &&
		    (NGBE_ETHADDRL_ETAG(rar_low) ==
		     l2_tunnel->tunnel_id)) {
			wr32(hw, NGBE_ETHADDRL, 0);
			wr32(hw, NGBE_ETHADDRH, 0);

			ngbe_clear_vmdq(hw, i, BIT_MASK32);

			return ret;
		}
	}

	return ret;
}

static int
ngbe_e_tag_filter_add(struct rte_eth_dev *dev,
		       struct rte_eth_l2_tunnel_conf *l2_tunnel)
{
	int ret = 0;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	uint32_t i, rar_entries;
	uint32_t rar_low, rar_high;

	/* One entry for one tunnel. Try to remove potential existing entry. */
	ngbe_e_tag_filter_del(dev, l2_tunnel);

	rar_entries = hw->mac.num_rar_entries;

	for (i = 1; i < rar_entries; i++) {
		wr32(hw, NGBE_ETHADDRIDX, i);
		rar_high = rd32(hw, NGBE_ETHADDRH);
		if (rar_high & NGBE_ETHADDRH_VLD) {
			continue;
		} else {
			ngbe_set_vmdq(hw, i, l2_tunnel->pool);
			rar_high = NGBE_ETHADDRH_VLD | NGBE_ETHADDRH_ETAG;
			rar_low = l2_tunnel->tunnel_id;

			wr32(hw, NGBE_ETHADDRL, rar_low);
			wr32(hw, NGBE_ETHADDRH, rar_high);

			return ret;
		}
	}

	PMD_INIT_LOG(NOTICE, "The table of E-tag forwarding rule is full."
		     " Please remove a rule before adding a new one.");
	return -EINVAL;
}

static inline struct ngbe_l2_tn_filter *
ngbe_l2_tn_filter_lookup(struct ngbe_l2_tn_info *l2_tn_info,
			  struct ngbe_l2_tn_key *key)
{
	int ret;

	ret = rte_hash_lookup(l2_tn_info->hash_handle, (const void *)key);
	if (ret < 0)
		return NULL;

	return l2_tn_info->hash_map[ret];
}

static inline int
ngbe_insert_l2_tn_filter(struct ngbe_l2_tn_info *l2_tn_info,
			  struct ngbe_l2_tn_filter *l2_tn_filter)
{
	int ret;

	ret = rte_hash_add_key(l2_tn_info->hash_handle,
			       &l2_tn_filter->key);

	if (ret < 0) {
		PMD_DRV_LOG(ERR,
			    "Failed to insert L2 tunnel filter"
			    " to hash table %d!",
			    ret);
		return ret;
	}

	l2_tn_info->hash_map[ret] = l2_tn_filter;

	TAILQ_INSERT_TAIL(&l2_tn_info->l2_tn_list, l2_tn_filter, entries);

	return 0;
}

static inline int
ngbe_remove_l2_tn_filter(struct ngbe_l2_tn_info *l2_tn_info,
			  struct ngbe_l2_tn_key *key)
{
	int ret;
	struct ngbe_l2_tn_filter *l2_tn_filter;

	ret = rte_hash_del_key(l2_tn_info->hash_handle, key);

	if (ret < 0) {
		PMD_DRV_LOG(ERR,
			    "No such L2 tunnel filter to delete %d!",
			    ret);
		return ret;
	}

	l2_tn_filter = l2_tn_info->hash_map[ret];
	l2_tn_info->hash_map[ret] = NULL;

	TAILQ_REMOVE(&l2_tn_info->l2_tn_list, l2_tn_filter, entries);
	rte_free(l2_tn_filter);

	return 0;
}

/* Add l2 tunnel filter */
int
ngbe_dev_l2_tunnel_filter_add(struct rte_eth_dev *dev,
			       struct rte_eth_l2_tunnel_conf *l2_tunnel,
			       bool restore)
{
	int ret;
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	struct ngbe_l2_tn_key key;
	struct ngbe_l2_tn_filter *node;

	if (!restore) {
		key.l2_tn_type = l2_tunnel->l2_tunnel_type;
		key.tn_id = l2_tunnel->tunnel_id;

		node = ngbe_l2_tn_filter_lookup(l2_tn_info, &key);

		if (node) {
			PMD_DRV_LOG(ERR,
				    "The L2 tunnel filter already exists!");
			return -EINVAL;
		}

		node = rte_zmalloc("ngbe_l2_tn",
				   sizeof(struct ngbe_l2_tn_filter),
				   0);
		if (!node)
			return -ENOMEM;

		rte_memcpy(&node->key,
				 &key,
				 sizeof(struct ngbe_l2_tn_key));
		node->pool = l2_tunnel->pool;
		ret = ngbe_insert_l2_tn_filter(l2_tn_info, node);
		if (ret < 0) {
			rte_free(node);
			return ret;
		}
	}

	switch (l2_tunnel->l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		ret = ngbe_e_tag_filter_add(dev, l2_tunnel);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	if ((!restore) && (ret < 0))
		(void)ngbe_remove_l2_tn_filter(l2_tn_info, &key);

	return ret;
}

/* Delete l2 tunnel filter */
int
ngbe_dev_l2_tunnel_filter_del(struct rte_eth_dev *dev,
			       struct rte_eth_l2_tunnel_conf *l2_tunnel)
{
	int ret;
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	struct ngbe_l2_tn_key key;

	key.l2_tn_type = l2_tunnel->l2_tunnel_type;
	key.tn_id = l2_tunnel->tunnel_id;
	ret = ngbe_remove_l2_tn_filter(l2_tn_info, &key);
	if (ret < 0)
		return ret;

	switch (l2_tunnel->l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		ret = ngbe_e_tag_filter_del(dev, l2_tunnel);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 * ngbe_dev_l2_tunnel_filter_handle - Handle operations for l2 tunnel filter.
 * @dev: pointer to rte_eth_dev structure
 * @filter_op:operation will be taken.
 * @arg: a pointer to specific structure corresponding to the filter_op
 */
static int
ngbe_dev_l2_tunnel_filter_handle(struct rte_eth_dev *dev,
				  enum rte_filter_op filter_op,
				  void *arg)
{
	int ret;

	if (filter_op == RTE_ETH_FILTER_NOP)
		return 0;

	if (arg == NULL) {
		PMD_DRV_LOG(ERR, "arg shouldn't be NULL for operation %u.",
			    filter_op);
		return -EINVAL;
	}

	switch (filter_op) {
	case RTE_ETH_FILTER_ADD:
		ret = ngbe_dev_l2_tunnel_filter_add
			(dev,
			 (struct rte_eth_l2_tunnel_conf *)arg,
			 FALSE);
		break;
	case RTE_ETH_FILTER_DELETE:
		ret = ngbe_dev_l2_tunnel_filter_del
			(dev,
			 (struct rte_eth_l2_tunnel_conf *)arg);
		break;
	default:
		PMD_DRV_LOG(ERR, "unsupported operation %u.", filter_op);
		ret = -EINVAL;
		break;
	}
	return ret;
}

static int
ngbe_e_tag_forwarding_en_dis(struct rte_eth_dev *dev, bool en)
{
	int ret = 0;
	uint32_t ctrl;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	ctrl = rd32(hw, NGBE_POOLCTL);
	ctrl &= ~NGBE_POOLCTL_MODE_MASK;
	if (en)
		ctrl |= NGBE_PSRPOOL_MODE_ETAG;
	wr32(hw, NGBE_POOLCTL, ctrl);

	return ret;
}

/* Enable l2 tunnel forwarding */
static int
ngbe_dev_l2_tunnel_forwarding_enable
	(struct rte_eth_dev *dev,
	 enum rte_eth_tunnel_type l2_tunnel_type)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	int ret = 0;

	switch (l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		l2_tn_info->e_tag_fwd_en = TRUE;
		ret = ngbe_e_tag_forwarding_en_dis(dev, 1);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Disable l2 tunnel forwarding */
static int
ngbe_dev_l2_tunnel_forwarding_disable
	(struct rte_eth_dev *dev,
	 enum rte_eth_tunnel_type l2_tunnel_type)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	int ret = 0;

	switch (l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		l2_tn_info->e_tag_fwd_en = FALSE;
		ret = ngbe_e_tag_forwarding_en_dis(dev, 0);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int
ngbe_e_tag_insertion_en_dis(struct rte_eth_dev *dev,
			     struct rte_eth_l2_tunnel_conf *l2_tunnel,
			     bool en)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	int ret = 0;
	uint32_t vmtir, vmvir;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	if (l2_tunnel->vf_id >= pci_dev->max_vfs) {
		PMD_DRV_LOG(ERR,
			    "VF id %u should be less than %u",
			    l2_tunnel->vf_id,
			    pci_dev->max_vfs);
		return -EINVAL;
	}

	if (en)
		vmtir = l2_tunnel->tunnel_id;
	else
		vmtir = 0;

	wr32(hw, NGBE_POOLETAG(l2_tunnel->vf_id), vmtir);

	vmvir = rd32(hw, NGBE_POOLTAG(l2_tunnel->vf_id));
	vmvir &= ~NGBE_POOLTAG_ETAG_MASK;
	if (en)
		vmvir |= NGBE_POOLTAG_ETAG;
	wr32(hw, NGBE_POOLTAG(l2_tunnel->vf_id), vmvir);

	return ret;
}

/* Enable l2 tunnel tag insertion */
static int
ngbe_dev_l2_tunnel_insertion_enable(struct rte_eth_dev *dev,
				     struct rte_eth_l2_tunnel_conf *l2_tunnel)
{
	int ret = 0;

	switch (l2_tunnel->l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		ret = ngbe_e_tag_insertion_en_dis(dev, l2_tunnel, 1);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Disable l2 tunnel tag insertion */
static int
ngbe_dev_l2_tunnel_insertion_disable
	(struct rte_eth_dev *dev,
	 struct rte_eth_l2_tunnel_conf *l2_tunnel)
{
	int ret = 0;

	switch (l2_tunnel->l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		ret = ngbe_e_tag_insertion_en_dis(dev, l2_tunnel, 0);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int
ngbe_e_tag_stripping_en_dis(struct rte_eth_dev *dev,
			     bool en)
{
	int ret = 0;
	int i;
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	for (i = 0; i < 128; ++i) {
		wr32m(hw, NGBE_RXCFG(i), NGBE_RXCFG_ETAG,
		     en ? NGBE_RXCFG_ETAG : 0);
	}

	return ret;
}

/* Enable l2 tunnel tag stripping */
static int
ngbe_dev_l2_tunnel_stripping_enable
	(struct rte_eth_dev *dev,
	 enum rte_eth_tunnel_type l2_tunnel_type)
{
	int ret = 0;

	switch (l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		ret = ngbe_e_tag_stripping_en_dis(dev, 1);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Disable l2 tunnel tag stripping */
static int
ngbe_dev_l2_tunnel_stripping_disable
	(struct rte_eth_dev *dev,
	 enum rte_eth_tunnel_type l2_tunnel_type)
{
	int ret = 0;

	switch (l2_tunnel_type) {
	case RTE_L2_TUNNEL_TYPE_E_TAG:
		ret = ngbe_e_tag_stripping_en_dis(dev, 0);
		break;
	default:
		PMD_DRV_LOG(ERR, "Invalid tunnel type");
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Enable/disable l2 tunnel offload functions */
static int
ngbe_dev_l2_tunnel_offload_set
	(struct rte_eth_dev *dev,
	 struct rte_eth_l2_tunnel_conf *l2_tunnel,
	 uint32_t mask,
	 uint8_t en)
{
	int ret = 0;

	if (l2_tunnel == NULL)
		return -EINVAL;

	ret = -EINVAL;
	if (mask & ETH_L2_TUNNEL_ENABLE_MASK) {
		if (en)
			ret = ngbe_dev_l2_tunnel_enable(
				dev,
				l2_tunnel->l2_tunnel_type);
		else
			ret = ngbe_dev_l2_tunnel_disable(
				dev,
				l2_tunnel->l2_tunnel_type);
	}

	if (mask & ETH_L2_TUNNEL_INSERTION_MASK) {
		if (en)
			ret = ngbe_dev_l2_tunnel_insertion_enable(
				dev,
				l2_tunnel);
		else
			ret = ngbe_dev_l2_tunnel_insertion_disable(
				dev,
				l2_tunnel);
	}

	if (mask & ETH_L2_TUNNEL_STRIPPING_MASK) {
		if (en)
			ret = ngbe_dev_l2_tunnel_stripping_enable(
				dev,
				l2_tunnel->l2_tunnel_type);
		else
			ret = ngbe_dev_l2_tunnel_stripping_disable(
				dev,
				l2_tunnel->l2_tunnel_type);
	}

	if (mask & ETH_L2_TUNNEL_FORWARDING_MASK) {
		if (en)
			ret = ngbe_dev_l2_tunnel_forwarding_enable(
				dev,
				l2_tunnel->l2_tunnel_type);
		else
			ret = ngbe_dev_l2_tunnel_forwarding_disable(
				dev,
				l2_tunnel->l2_tunnel_type);
	}

	return ret;
}

/* restore n-tuple filter */
static inline void
ngbe_ntuple_filter_restore(struct rte_eth_dev *dev)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	struct ngbe_5tuple_filter *node;

	TAILQ_FOREACH(node, &filter_info->fivetuple_list, entries) {
		ngbe_inject_5tuple_filter(dev, node);
	}
}

/* restore ethernet type filter */
static inline void
ngbe_ethertype_filter_restore(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	int i;

	for (i = 0; i < NGBE_ETF_ID_MAX; i++) {
		if (filter_info->ethertype_mask & (1 << i)) {
			wr32(hw, NGBE_ETFLT(i),
					filter_info->ethertype_filters[i].etqf);
			wr32(hw, NGBE_ETCLS(i),
					filter_info->ethertype_filters[i].etqs);
			ngbe_flush(hw);
		}
	}
}

/* restore SYN filter */
static inline void
ngbe_syn_filter_restore(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	uint32_t synqf;

	synqf = filter_info->syn_info;

	if (synqf & NGBE_SYNCLS_ENA) {
		wr32(hw, NGBE_SYNCLS, synqf);
		ngbe_flush(hw);
	}
}

/* restore L2 tunnel filter */
static inline void
ngbe_l2_tn_filter_restore(struct rte_eth_dev *dev)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	struct ngbe_l2_tn_filter *node;
	struct rte_eth_l2_tunnel_conf l2_tn_conf;

	TAILQ_FOREACH(node, &l2_tn_info->l2_tn_list, entries) {
		l2_tn_conf.l2_tunnel_type = node->key.l2_tn_type;
		l2_tn_conf.tunnel_id      = node->key.tn_id;
		l2_tn_conf.pool           = node->pool;
		(void)ngbe_dev_l2_tunnel_filter_add(dev, &l2_tn_conf, TRUE);
	}
}

/* restore rss filter */
static inline void
ngbe_rss_filter_restore(struct rte_eth_dev *dev)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);

	if (filter_info->rss_info.conf.queue_num)
		ngbe_config_rss_filter(dev,
			&filter_info->rss_info, TRUE);
}

static int
ngbe_filter_restore(struct rte_eth_dev *dev)
{
	ngbe_ntuple_filter_restore(dev);
	ngbe_ethertype_filter_restore(dev);
	ngbe_syn_filter_restore(dev);
	ngbe_l2_tn_filter_restore(dev);
	ngbe_rss_filter_restore(dev);

	return 0;
}

static void
ngbe_l2_tunnel_conf(struct rte_eth_dev *dev)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);

	if (l2_tn_info->e_tag_en)
		(void)ngbe_e_tag_enable(hw);

	if (l2_tn_info->e_tag_fwd_en)
		(void)ngbe_e_tag_forwarding_en_dis(dev, 1);

	(void)ngbe_update_e_tag_eth_type(hw, l2_tn_info->e_tag_ether_type);
}

/* remove all the n-tuple filters */
void
ngbe_clear_all_ntuple_filter(struct rte_eth_dev *dev)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	struct ngbe_5tuple_filter *p_5tuple;

	while ((p_5tuple = TAILQ_FIRST(&filter_info->fivetuple_list)))
		ngbe_remove_5tuple_filter(dev, p_5tuple);
}

/* remove all the ether type filters */
void
ngbe_clear_all_ethertype_filter(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);
	int i;

	for (i = 0; i < NGBE_ETF_ID_MAX; i++) {
		if (filter_info->ethertype_mask & (1 << i) &&
		    !filter_info->ethertype_filters[i].conf) {
			(void)ngbe_ethertype_filter_remove(filter_info,
							    (uint8_t)i);
			wr32(hw, NGBE_ETFLT(i), 0);
			wr32(hw, NGBE_ETCLS(i), 0);
			ngbe_flush(hw);
		}
	}
}

/* remove the SYN filter */
void
ngbe_clear_syn_filter(struct rte_eth_dev *dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);

	if (filter_info->syn_info & NGBE_SYNCLS_ENA) {
		filter_info->syn_info = 0;

		wr32(hw, NGBE_SYNCLS, 0);
		ngbe_flush(hw);
	}
}

/* remove all the L2 tunnel filters */
int
ngbe_clear_all_l2_tn_filter(struct rte_eth_dev *dev)
{
	struct ngbe_l2_tn_info *l2_tn_info = NGBE_DEV_L2_TN(dev);
	struct ngbe_l2_tn_filter *l2_tn_filter;
	struct rte_eth_l2_tunnel_conf l2_tn_conf;
	int ret = 0;

	while ((l2_tn_filter = TAILQ_FIRST(&l2_tn_info->l2_tn_list))) {
		l2_tn_conf.l2_tunnel_type = l2_tn_filter->key.l2_tn_type;
		l2_tn_conf.tunnel_id      = l2_tn_filter->key.tn_id;
		l2_tn_conf.pool           = l2_tn_filter->pool;
		ret = ngbe_dev_l2_tunnel_filter_del(dev, &l2_tn_conf);
		if (ret < 0)
			return ret;
	}

	return 0;
}

bool
is_ngbe_supported(struct rte_eth_dev *dev)
{
#if RTE_VERSION < RTE_VERSION_NUM(17, 5, 0, 0)
	UNREFERENCED_PARAMETER(dev);
	return 1;
#else
	if (strcmp(dev->device->driver->name, rte_ngbe_pmd.driver.name))
		return false;
	else
		return true;
#endif
}

static const struct eth_dev_ops ngbe_eth_dev_ops = {
	.dev_configure              = ngbe_dev_configure,
	.dev_start                  = ngbe_dev_start,
	.dev_stop                   = ngbe_dev_stop,
	.dev_set_link_up            = ngbe_dev_set_link_up,
	.dev_set_link_down          = ngbe_dev_set_link_down,
	.dev_close                  = ngbe_dev_close,
#if RTE_VERSION >= RTE_VERSION_NUM(17, 11, 0, 0)
	.dev_reset                  = ngbe_dev_reset,
#endif
	.promiscuous_enable         = ngbe_dev_promiscuous_enable,
	.promiscuous_disable        = ngbe_dev_promiscuous_disable,
	.allmulticast_enable        = ngbe_dev_allmulticast_enable,
	.allmulticast_disable       = ngbe_dev_allmulticast_disable,
	.link_update                = ngbe_dev_link_update,
	.stats_get                  = ngbe_dev_stats_get,
	.xstats_get                 = ngbe_dev_xstats_get,
#if RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0)
	.xstats_get_by_id           = ngbe_dev_xstats_get_by_id,
#endif
	.stats_reset                = ngbe_dev_stats_reset,
	.xstats_reset               = ngbe_dev_xstats_reset,
#if RTE_VERSION >= RTE_VERSION_NUM(16, 7, 0, 0)
	.xstats_get_names           = ngbe_dev_xstats_get_names,
#endif
#if RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0)
	.xstats_get_names_by_id     = ngbe_dev_xstats_get_names_by_id,
#endif
	.queue_stats_mapping_set    = ngbe_dev_queue_stats_mapping_set,
	.fw_version_get             = ngbe_fw_version_get,
	.dev_infos_get              = ngbe_dev_info_get,
	.dev_supported_ptypes_get   = ngbe_dev_supported_ptypes_get,
	.mtu_set                    = ngbe_dev_mtu_set,
	.vlan_filter_set            = ngbe_vlan_filter_set,
	.vlan_tpid_set              = ngbe_vlan_tpid_set,
	.vlan_offload_set           = ngbe_vlan_offload_set,
	.vlan_strip_queue_set       = ngbe_vlan_strip_queue_set,
	.rx_queue_start	            = ngbe_dev_rx_queue_start,
	.rx_queue_stop              = ngbe_dev_rx_queue_stop,
	.tx_queue_start	            = ngbe_dev_tx_queue_start,
	.tx_queue_stop              = ngbe_dev_tx_queue_stop,
	.rx_queue_setup             = ngbe_dev_rx_queue_setup,
	.rx_queue_intr_enable       = ngbe_dev_rx_queue_intr_enable,
	.rx_queue_intr_disable      = ngbe_dev_rx_queue_intr_disable,
	.rx_queue_release           = ngbe_dev_rx_queue_release,
	.rx_queue_count             = ngbe_dev_rx_queue_count,
	.rx_descriptor_done         = ngbe_dev_rx_descriptor_done,
#if RTE_VERSION >= RTE_VERSION_NUM(17, 5, 0, 0)
	.rx_descriptor_status       = ngbe_dev_rx_descriptor_status,
	.tx_descriptor_status       = ngbe_dev_tx_descriptor_status,
#endif
	.tx_queue_setup             = ngbe_dev_tx_queue_setup,
	.tx_queue_release           = ngbe_dev_tx_queue_release,
	.dev_led_on                 = ngbe_dev_led_on,
	.dev_led_off                = ngbe_dev_led_off,
	.flow_ctrl_get              = ngbe_flow_ctrl_get,
	.flow_ctrl_set              = ngbe_flow_ctrl_set,
	.mac_addr_add               = ngbe_add_rar,
	.mac_addr_remove            = ngbe_remove_rar,
	.mac_addr_set               = ngbe_set_default_mac_addr,
	.uc_hash_table_set          = ngbe_uc_hash_table_set,
	.uc_all_hash_table_set      = ngbe_uc_all_hash_table_set,
	.mirror_rule_set            = ngbe_mirror_rule_set,
	.mirror_rule_reset          = ngbe_mirror_rule_reset,
	.set_queue_rate_limit       = ngbe_set_queue_rate_limit,
	.reta_update                = ngbe_dev_rss_reta_update,
	.reta_query                 = ngbe_dev_rss_reta_query,
	.rss_hash_update            = ngbe_dev_rss_hash_update,
	.rss_hash_conf_get          = ngbe_dev_rss_hash_conf_get,
	.filter_ctrl                = ngbe_dev_filter_ctrl,
	.set_mc_addr_list           = ngbe_dev_set_mc_addr_list,
	.rxq_info_get               = ngbe_rxq_info_get,
	.txq_info_get               = ngbe_txq_info_get,
	.timesync_enable            = ngbe_timesync_enable,
	.timesync_disable           = ngbe_timesync_disable,
	.timesync_read_rx_timestamp = ngbe_timesync_read_rx_timestamp,
	.timesync_read_tx_timestamp = ngbe_timesync_read_tx_timestamp,
	.get_reg                    = ngbe_get_regs,
	.get_eeprom_length          = ngbe_get_eeprom_length,
	.get_eeprom                 = ngbe_get_eeprom,
	.set_eeprom                 = ngbe_set_eeprom,
#if RTE_VERSION >= RTE_VERSION_NUM(18, 5, 0, 0)
	.get_module_info            = ngbe_get_module_info,
	.get_module_eeprom          = ngbe_get_module_eeprom,
#endif
	.timesync_adjust_time       = ngbe_timesync_adjust_time,
	.timesync_read_time         = ngbe_timesync_read_time,
	.timesync_write_time        = ngbe_timesync_write_time,
	.l2_tunnel_eth_type_conf    = ngbe_dev_l2_tunnel_eth_type_conf,
	.l2_tunnel_offload_set      = ngbe_dev_l2_tunnel_offload_set,
#if RTE_VERSION >= RTE_VERSION_NUM(17, 11, 0, 0)
	.tm_ops_get                 = ngbe_tm_ops_get,
#endif
};

RTE_PMD_REGISTER_PCI(net_ngbe, rte_ngbe_pmd);
RTE_PMD_REGISTER_PCI_TABLE(net_ngbe, pci_id_ngbe_map);
RTE_PMD_REGISTER_KMOD_DEP(net_ngbe, "* igb_uio | uio_pci_generic | vfio-pci");


RTE_INIT(ngbe_init_log)
{
	ngbe_logtype_init = rte_log_register("pmd.net.ngbe.init");
	if (ngbe_logtype_init >= 0)
		rte_log_set_level(ngbe_logtype_init, RTE_LOG_NOTICE);
	ngbe_logtype_driver = rte_log_register("pmd.net.ngbe.driver");
	if (ngbe_logtype_driver >= 0)
		rte_log_set_level(ngbe_logtype_driver, RTE_LOG_NOTICE);
}
