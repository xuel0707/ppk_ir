/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015-2020
 */

#ifndef _NGBE_ETHDEV_H_
#define _NGBE_ETHDEV_H_

#include <stdint.h>

#include "base/ngbe.h"
#ifdef RTE_LIB_SECURITY
#include "ngbe_ipsec.h"
#endif
#include <rte_flow.h>
#include <rte_time.h>
#include <rte_hash.h>
#include <rte_pci.h>

/* need update link, bit flag */
#define NGBE_FLAG_NEED_LINK_UPDATE (uint32_t)(1 << 0)
#define NGBE_FLAG_MAILBOX          (uint32_t)(1 << 1)
#define NGBE_FLAG_PHY_INTERRUPT    (uint32_t)(1 << 2)
#define NGBE_FLAG_MACSEC           (uint32_t)(1 << 3)
#define NGBE_FLAG_NEED_LINK_CONFIG (uint32_t)(1 << 4)

/*
 * Defines that were not part of ngbe_type.h as they are not used by the
 * FreeBSD driver.
 */
//#define NGBE_EXTENDED_VLAN	  (uint32_t)(1 << 26) /* EXTENDED VLAN ENABLE */
#define NGBE_VFTA_SIZE 128
#define NGBE_VLAN_TAG_SIZE 4
#define NGBE_HKEY_MAX_INDEX 10
#define NGBE_MAX_RX_QUEUE_NUM	8
#define NGBE_NONE_MODE_TX_NB_QUEUES 64

#ifndef NBBY
#define NBBY	8	/* number of bits in a byte */
#endif
#define NGBE_HWSTRIP_BITMAP_SIZE (NGBE_MAX_RX_QUEUE_NUM / (sizeof(uint32_t) * NBBY))

#define NGBE_QUEUE_ITR_INTERVAL_DEFAULT	500 /* 500us */

#define NGBE_MAX_QUEUE_NUM_PER_VF  8

#define NGBE_5TUPLE_MAX_PRI            7
#define NGBE_5TUPLE_MIN_PRI            1

/* The overhead from MTU to max frame size. */
#define NGBE_ETH_OVERHEAD (ETHER_HDR_LEN + ETHER_CRC_LEN)

#define NGBE_RSS_OFFLOAD_ALL ( \
	ETH_RSS_IPV4 | \
	ETH_RSS_NONFRAG_IPV4_TCP | \
	ETH_RSS_NONFRAG_IPV4_UDP | \
	ETH_RSS_IPV6 | \
	ETH_RSS_NONFRAG_IPV6_TCP | \
	ETH_RSS_NONFRAG_IPV6_UDP | \
	ETH_RSS_IPV6_EX | \
	ETH_RSS_IPV6_TCP_EX | \
	ETH_RSS_IPV6_UDP_EX)

//#define NGBE_VF_IRQ_ENABLE_MASK        3          /* vf irq enable mask */
//#define NGBE_VF_MAXMSIVECTOR           1

#define NGBE_MISC_VEC_ID               RTE_INTR_VEC_ZERO_OFFSET
#define NGBE_RX_VEC_START              RTE_INTR_VEC_RXTX_OFFSET

#define NGBE_MACSEC_PNTHRSH            0xFFFFFE00

#define NGBE_MAX_FDIR_FILTER_NUM       (1024 * 32)
#define NGBE_MAX_L2_TN_FILTER_NUM      128

/*
 * Information about the fdir mode.
 */
struct ngbe_hw_fdir_mask {
	uint16_t vlan_tci_mask;
	uint32_t src_ipv4_mask;
	uint32_t dst_ipv4_mask;
	uint16_t src_ipv6_mask;
	uint16_t dst_ipv6_mask;
	uint16_t src_port_mask;
	uint16_t dst_port_mask;
	uint16_t flex_bytes_mask;
	uint8_t  mac_addr_byte_mask;
	uint32_t tunnel_id_mask;
	uint8_t  tunnel_type_mask;
};

struct ngbe_rte_flow_rss_conf {
	struct rte_flow_action_rss conf; /**< RSS parameters. */
	uint8_t key[NGBE_HKEY_MAX_INDEX * sizeof(uint32_t)]; /* Hash key. */
	uint16_t queue[NGBE_MAX_RX_QUEUE_NUM]; /**< Queues indices to use. */
};

/* structure for interrupt relative data */
struct ngbe_interrupt {
	uint32_t flags;
	uint32_t mask_misc;
	uint32_t mask_misc_orig; /* save mask during delayed handler */
	uint64_t mask;
	uint64_t mask_orig; /* save mask during delayed handler */
};

#define NGBE_NB_STAT_MAPPING  32
#define QSM_REG_NB_BITS_PER_QMAP_FIELD 8
#define NB_QMAP_FIELDS_PER_QSM_REG 4
#define QMAP_FIELD_RESERVED_BITS_MASK 0x0f
struct ngbe_stat_mappings {
	uint32_t tqsm[NGBE_NB_STAT_MAPPING];
	uint32_t rqsm[NGBE_NB_STAT_MAPPING];
};

struct ngbe_vfta {
	uint32_t vfta[NGBE_VFTA_SIZE];
};

struct ngbe_hwstrip {
	uint32_t bitmap[NGBE_HWSTRIP_BITMAP_SIZE];
};

/*
 * VF data which used by PF host only
 */
#define NGBE_MAX_VF_MC_ENTRIES		30
#define NGBE_MAX_MR_RULE_ENTRIES	4 /* number of mirroring rules supported */

struct ngbe_uta_info {
	uint8_t  uc_filter_type;
	uint16_t uta_in_use;
	uint32_t uta_shadow[NGBE_MAX_UTA];
};

#define NGBE_MAX_MIRROR_RULES 4  /* Maximum nb. of mirror rules. */

struct ngbe_mirror_info {
	struct rte_eth_mirror_conf mr_conf[NGBE_MAX_MIRROR_RULES];
	/**< store PF mirror rules configuration*/
};

struct ngbe_vf_info {
	uint8_t vf_mac_addresses[ETH_ADDR_LEN];
	uint16_t vf_mc_hashes[NGBE_MAX_VF_MC_ENTRIES];
	uint16_t num_vf_mc_hashes;
	uint16_t default_vf_vlan_id;
	uint16_t vlans_enabled;
	bool clear_to_send;
	uint16_t tx_rate[NGBE_MAX_QUEUE_NUM_PER_VF];
	uint16_t vlan_count;
	uint8_t spoofchk_enabled;
	uint8_t api_version;
	uint16_t switch_domain_id;
	uint16_t xcast_mode;
	uint16_t mac_count;
};

TAILQ_HEAD(ngbe_5tuple_filter_list, ngbe_5tuple_filter);

struct ngbe_5tuple_filter_info {
	uint32_t dst_ip;
	uint32_t src_ip;
	uint16_t dst_port;
	uint16_t src_port;
	enum ngbe_5tuple_protocol proto;        /* l4 protocol. */
	uint8_t priority;        /* seven levels (001b-111b), 111b is highest,
				      used when more than one filter matches. */
	uint8_t dst_ip_mask:1,   /* if mask is 1b, do not compare dst ip. */
		src_ip_mask:1,   /* if mask is 1b, do not compare src ip. */
		dst_port_mask:1, /* if mask is 1b, do not compare dst port. */
		src_port_mask:1, /* if mask is 1b, do not compare src port. */
		proto_mask:1;    /* if mask is 1b, do not compare protocol. */
};

/* 5tuple filter structure */
struct ngbe_5tuple_filter {
	TAILQ_ENTRY(ngbe_5tuple_filter) entries;
	uint16_t index;       /* the index of 5tuple filter */
	struct ngbe_5tuple_filter_info filter_info;
	uint16_t queue;       /* rx queue assigned to */
};

#define NGBE_5TUPLE_ARRAY_SIZE \
	(RTE_ALIGN(NGBE_MAX_FTQF_FILTERS, (sizeof(uint32_t) * NBBY)) / \
	 (sizeof(uint32_t) * NBBY))

struct ngbe_ethertype_filter {
	uint16_t ethertype;
	uint32_t etqf;
	uint32_t etqs;
	/**
	 * If this filter is added by configuration,
	 * it should not be removed.
	 */
	bool     conf;
};

/*
 * Structure to store filters' info.
 */
struct ngbe_filter_info {
	uint8_t ethertype_mask;  /* Bit mask for every used ethertype filter */
	/* store used ethertype filters*/
	struct ngbe_ethertype_filter ethertype_filters[NGBE_ETF_ID_MAX];
	/* Bit mask for every used 5tuple filter */
	uint32_t fivetuple_mask[NGBE_5TUPLE_ARRAY_SIZE];
	struct ngbe_5tuple_filter_list fivetuple_list;
	/* store the SYN filter info */
	uint32_t syn_info;
	/* store the rss filter info */
	struct ngbe_rte_flow_rss_conf rss_info;
};

struct ngbe_l2_tn_key {
	enum rte_eth_tunnel_type          l2_tn_type;
	uint32_t                          tn_id;
};

struct ngbe_l2_tn_filter {
	TAILQ_ENTRY(ngbe_l2_tn_filter)    entries;
	struct ngbe_l2_tn_key             key;
	uint32_t                           pool;
};

TAILQ_HEAD(ngbe_l2_tn_filter_list, ngbe_l2_tn_filter);

struct ngbe_l2_tn_info {
	struct ngbe_l2_tn_filter_list      l2_tn_list;
	struct ngbe_l2_tn_filter         **hash_map;
	struct rte_hash                    *hash_handle;
	bool e_tag_en; /* e-tag enabled */
	bool e_tag_fwd_en; /* e-tag based forwarding enabled */
	uint16_t e_tag_ether_type; /* ether type for e-tag */
};

struct rte_flow {
	enum rte_filter_type filter_type;
	void *rule;
};

/* The configuration of bandwidth */
struct ngbe_bw_conf {
	uint8_t tc_num; /* Number of TCs. */
};

#if RTE_VERSION >= RTE_VERSION_NUM(17, 8, 0, 0)
/* Struct to store Traffic Manager shaper profile. */
struct ngbe_tm_shaper_profile {
	TAILQ_ENTRY(ngbe_tm_shaper_profile) node;
	uint32_t shaper_profile_id;
	uint32_t reference_count;
	struct rte_tm_shaper_params profile;
};

TAILQ_HEAD(ngbe_shaper_profile_list, ngbe_tm_shaper_profile);

/* node type of Traffic Manager */
enum ngbe_tm_node_type {
	NGBE_TM_NODE_TYPE_PORT,
	NGBE_TM_NODE_TYPE_TC,
	NGBE_TM_NODE_TYPE_QUEUE,
	NGBE_TM_NODE_TYPE_MAX,
};

/* Struct to store Traffic Manager node configuration. */
struct ngbe_tm_node {
	TAILQ_ENTRY(ngbe_tm_node) node;
	uint32_t id;
	uint32_t priority;
	uint32_t weight;
	uint32_t reference_count;
	uint16_t no;
	struct ngbe_tm_node *parent;
	struct ngbe_tm_shaper_profile *shaper_profile;
	struct rte_tm_node_params params;
};

TAILQ_HEAD(ngbe_tm_node_list, ngbe_tm_node);

/* The configuration of Traffic Manager */
struct ngbe_tm_conf {
	struct ngbe_shaper_profile_list shaper_profile_list;
	struct ngbe_tm_node *root; /* root node - port */
	struct ngbe_tm_node_list tc_list; /* node list for all the TCs */
	struct ngbe_tm_node_list queue_list; /* node list for all the queues */
	/**
	 * The number of added TC nodes.
	 * It should be no more than the TC number of this port.
	 */
	uint32_t nb_tc_node;
	/**
	 * The number of added queue nodes.
	 * It should be no more than the queue number of this port.
	 */
	uint32_t nb_queue_node;
	/**
	 * This flag is used to check if APP can change the TM node
	 * configuration.
	 * When it's true, means the configuration is applied to HW,
	 * APP should not change the configuration.
	 * As we don't support on-the-fly configuration, when starting
	 * the port, APP should call the hierarchy_commit API to set this
	 * flag to true. When stopping the port, this flag should be set
	 * to false.
	 */
	bool committed;
};
#endif /* RTE_VERSION >= RTE_VERSION_NUM(17, 8, 0, 0) */

/*
 * Structure to store private data for each driver instance (for each port).
 */
struct ngbe_adapter {
	struct ngbe_hw             hw;
	struct ngbe_hw_stats       stats;
	struct ngbe_interrupt      intr;
	struct ngbe_stat_mappings  stat_mappings;
	struct ngbe_vfta           shadow_vfta;
	struct ngbe_hwstrip	   hwstrip;
	struct ngbe_mirror_info    mr_data;
	struct ngbe_vf_info        *vfdata;
	struct ngbe_uta_info       uta_info;
	struct ngbe_filter_info    filter;
	struct ngbe_l2_tn_info     l2_tn;
	struct ngbe_bw_conf        bw_conf;
#ifdef RTE_LIB_SECURITY
	struct ngbe_ipsec          ipsec;
#endif
	bool rx_bulk_alloc_allowed;
	struct rte_timecounter      systime_tc;
	struct rte_timecounter      rx_tstamp_tc;
	struct rte_timecounter      tx_tstamp_tc;
#if RTE_VERSION >= RTE_VERSION_NUM(17, 8, 0, 0)
	struct ngbe_tm_conf        tm_conf;
#endif
	/* For RSS reta table update */
	uint8_t rss_reta_updated;

	/* Used for VF link sync with PF's physical and logical (by checking
	 * mailbox status) link status.
	 */
	uint8_t pflink_fullchk;
	uint8_t mac_ctrl_frame_fwd;
	rte_atomic32_t link_thread_running;
	pthread_t link_thread_tid;
};

struct ngbe_vf_representor {
	uint16_t vf_id;
	uint16_t switch_domain_id;
	struct rte_eth_dev *pf_ethdev;
};

int ngbe_vf_representor_init(struct rte_eth_dev *ethdev, void *init_params);
int ngbe_vf_representor_uninit(struct rte_eth_dev *ethdev);

#define NGBE_DEV_REPRESENTOR(dev) \
	((struct ngbe_vf_representor *)(dev)->data->dev_private) 

#define NGBE_DEV_ADAPTER(dev) \
	((struct ngbe_adapter *)(dev)->data->dev_private)

#define NGBE_DEV_HW(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->hw)

#define NGBE_DEV_STATS(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->stats)

#define NGBE_DEV_INTR(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->intr)

#define NGBE_DEV_FDIR(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->fdir)

#define NGBE_DEV_STAT_MAPPINGS(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->stat_mappings)

#define NGBE_DEV_VFTA(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->shadow_vfta)

#define NGBE_DEV_HWSTRIP(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->hwstrip)

#define NGBE_DEV_VFDATA(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->vfdata)

#define NGBE_DEV_MR_INFO(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->mr_data)

#define NGBE_DEV_UTA_INFO(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->uta_info)

#define NGBE_DEV_FILTER(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->filter)

#define NGBE_DEV_L2_TN(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->l2_tn)

#define NGBE_DEV_BW_CONF(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->bw_conf)

#define NGBE_DEV_TM_CONF(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->tm_conf)

#define NGBE_DEV_IPSEC(dev) \
	(&((struct ngbe_adapter *)(dev)->data->dev_private)->ipsec)

/*
 * RX/TX function prototypes
 */
void ngbe_dev_clear_queues(struct rte_eth_dev *dev);

void ngbe_dev_free_queues(struct rte_eth_dev *dev);

void ngbe_dev_rx_queue_release(void *rxq);

void ngbe_dev_tx_queue_release(void *txq);

int  ngbe_dev_rx_queue_setup(struct rte_eth_dev *dev, uint16_t rx_queue_id,
		uint16_t nb_rx_desc, unsigned int socket_id,
		const struct rte_eth_rxconf *rx_conf,
		struct rte_mempool *mb_pool);

int  ngbe_dev_tx_queue_setup(struct rte_eth_dev *dev, uint16_t tx_queue_id,
		uint16_t nb_tx_desc, unsigned int socket_id,
		const struct rte_eth_txconf *tx_conf);

uint32_t ngbe_dev_rx_queue_count(struct rte_eth_dev *dev,
		uint16_t rx_queue_id);

int ngbe_dev_rx_descriptor_done(void *rx_queue, uint16_t offset);

int ngbe_dev_rx_descriptor_status(void *rx_queue, uint16_t offset);
int ngbe_dev_tx_descriptor_status(void *tx_queue, uint16_t offset);

int ngbe_dev_rx_init(struct rte_eth_dev *dev);

void ngbe_dev_tx_init(struct rte_eth_dev *dev);

int ngbe_dev_rxtx_start(struct rte_eth_dev *dev);

void ngbe_dev_save_rx_queue(struct ngbe_hw *hw, uint16_t rx_queue_id);
void ngbe_dev_store_rx_queue(struct ngbe_hw *hw, uint16_t rx_queue_id);
void ngbe_dev_save_tx_queue(struct ngbe_hw *hw, uint16_t tx_queue_id);
void ngbe_dev_store_tx_queue(struct ngbe_hw *hw, uint16_t tx_queue_id);

int ngbe_dev_rx_queue_start(struct rte_eth_dev *dev, uint16_t rx_queue_id);

int ngbe_dev_rx_queue_stop(struct rte_eth_dev *dev, uint16_t rx_queue_id);

int ngbe_rx_queue_start(struct rte_eth_dev *dev, uint16_t rx_queue_id);
int ngbe_rx_queue_stop(struct rte_eth_dev *dev, uint16_t rx_queue_id);

int ngbe_dev_tx_queue_start(struct rte_eth_dev *dev, uint16_t tx_queue_id);

int ngbe_dev_tx_queue_stop(struct rte_eth_dev *dev, uint16_t tx_queue_id);

void ngbe_rxq_info_get(struct rte_eth_dev *dev, uint16_t queue_id,
	struct rte_eth_rxq_info *qinfo);

void ngbe_txq_info_get(struct rte_eth_dev *dev, uint16_t queue_id,
	struct rte_eth_txq_info *qinfo);

int ngbevf_dev_rx_init(struct rte_eth_dev *dev);

void ngbevf_dev_tx_init(struct rte_eth_dev *dev);

void ngbevf_dev_rxtx_start(struct rte_eth_dev *dev);

uint16_t ngbe_recv_pkts(void *rx_queue, struct rte_mbuf **rx_pkts,
		uint16_t nb_pkts);

uint16_t ngbe_recv_pkts_bulk_alloc(void *rx_queue, struct rte_mbuf **rx_pkts,
				    uint16_t nb_pkts);

uint16_t ngbe_recv_pkts_lro_single_alloc(void *rx_queue,
		struct rte_mbuf **rx_pkts, uint16_t nb_pkts);
uint16_t ngbe_recv_pkts_lro_bulk_alloc(void *rx_queue,
		struct rte_mbuf **rx_pkts, uint16_t nb_pkts);

uint16_t ngbe_xmit_pkts(void *tx_queue, struct rte_mbuf **tx_pkts,
		uint16_t nb_pkts);

uint16_t ngbe_xmit_pkts_simple(void *tx_queue, struct rte_mbuf **tx_pkts,
		uint16_t nb_pkts);

uint16_t ngbe_prep_pkts(void *tx_queue, struct rte_mbuf **tx_pkts,
		uint16_t nb_pkts);

int ngbe_dev_rss_hash_update(struct rte_eth_dev *dev,
			      struct rte_eth_rss_conf *rss_conf);

int ngbe_dev_rss_hash_conf_get(struct rte_eth_dev *dev,
				struct rte_eth_rss_conf *rss_conf);

bool ngbe_rss_update_sp(enum ngbe_mac_type mac_type);

int ngbe_add_del_ntuple_filter(struct rte_eth_dev *dev,
			struct rte_eth_ntuple_filter *filter,
			bool add);
int ngbe_add_del_ethertype_filter(struct rte_eth_dev *dev,
			struct rte_eth_ethertype_filter *filter,
			bool add);
int ngbe_syn_filter_set(struct rte_eth_dev *dev,
			struct rte_eth_syn_filter *filter,
			bool add);

/**
 * l2 tunnel configuration.
 */
struct ngbe_l2_tunnel_conf {
	enum rte_eth_tunnel_type l2_tunnel_type;
	uint16_t ether_type; /* ether type in l2 header */
	uint32_t tunnel_id; /* port tag id for e-tag */
	uint16_t vf_id; /* VF id for tag insertion */
	uint32_t pool; /* destination pool for tag based forwarding */
};

int
ngbe_dev_l2_tunnel_filter_add(struct rte_eth_dev *dev,
			       struct ngbe_l2_tunnel_conf *l2_tunnel,
			       bool restore);
int
ngbe_dev_l2_tunnel_filter_del(struct rte_eth_dev *dev,
			       struct ngbe_l2_tunnel_conf *l2_tunnel);
void ngbe_filterlist_init(void);
void ngbe_filterlist_flush(void);

void ngbe_set_ivar_map(struct ngbe_hw *hw, int8_t direction,
			       uint8_t queue, uint8_t msix_vector);

void ngbe_configure_pb(struct rte_eth_dev *dev);
void ngbe_configure_port(struct rte_eth_dev *dev);

int
ngbe_dev_link_update_share(struct rte_eth_dev *dev,
		int wait_to_complete);

/*
 * misc function prototypes
 */
void ngbe_vlan_hw_filter_enable(struct rte_eth_dev *dev);

void ngbe_vlan_hw_filter_disable(struct rte_eth_dev *dev);

void ngbe_vlan_hw_strip_config(struct rte_eth_dev *dev);

int ngbe_pf_host_init(struct rte_eth_dev *eth_dev);

void ngbe_pf_host_uninit(struct rte_eth_dev *eth_dev);

void ngbe_pf_mbx_process(struct rte_eth_dev *eth_dev);

int ngbe_pf_host_configure(struct rte_eth_dev *eth_dev);

uint32_t ngbe_convert_vm_rx_mask_to_val(uint16_t rx_mask, uint32_t orig_val);

extern const struct rte_flow_ops ngbe_flow_ops;

void ngbe_clear_all_ethertype_filter(struct rte_eth_dev *dev);
void ngbe_clear_all_ntuple_filter(struct rte_eth_dev *dev);
void ngbe_clear_syn_filter(struct rte_eth_dev *dev);
int ngbe_clear_all_l2_tn_filter(struct rte_eth_dev *dev);

int ngbe_vt_check(struct ngbe_hw *hw);
int ngbe_set_vf_rate_limit(struct rte_eth_dev *dev, uint16_t vf,
			    uint16_t tx_rate, uint64_t q_msk);
int ngbe_tm_ops_get(struct rte_eth_dev *dev, void *ops);
void ngbe_tm_conf_init(struct rte_eth_dev *dev);
void ngbe_tm_conf_uninit(struct rte_eth_dev *dev);
int ngbe_set_queue_rate_limit(struct rte_eth_dev *dev, uint16_t queue_idx,
			       uint16_t tx_rate);
int ngbe_rss_conf_init(struct ngbe_rte_flow_rss_conf *out,
			const struct rte_flow_action_rss *in);
int ngbe_action_rss_same(const struct rte_flow_action_rss *comp,
			  const struct rte_flow_action_rss *with);
int ngbe_config_rss_filter(struct rte_eth_dev *dev,
		struct ngbe_rte_flow_rss_conf *conf, bool add);

static inline int
ngbe_ethertype_filter_lookup(struct ngbe_filter_info *filter_info,
			      uint16_t ethertype)
{
	int i;

	for (i = 0; i < NGBE_ETF_ID_MAX; i++) {
		if (filter_info->ethertype_filters[i].ethertype == ethertype &&
		    (filter_info->ethertype_mask & (1 << i)))
			return i;
	}
	return -1;
}

static inline int
ngbe_ethertype_filter_insert(struct ngbe_filter_info *filter_info,
			      struct ngbe_ethertype_filter *ethertype_filter)
{
	int i;

	for (i = 0; i < NGBE_ETF_ID_MAX; i++) {
		if (filter_info->ethertype_mask & (1 << i)) {
			continue;
		}
		filter_info->ethertype_mask |= 1 << i;
		filter_info->ethertype_filters[i].ethertype =
				ethertype_filter->ethertype;
		filter_info->ethertype_filters[i].etqf =
				ethertype_filter->etqf;
		filter_info->ethertype_filters[i].etqs =
				ethertype_filter->etqs;
		filter_info->ethertype_filters[i].conf =
				ethertype_filter->conf;
		break;
	}
	return (i < NGBE_ETF_ID_MAX ? i : -1);
}

static inline int
ngbe_ethertype_filter_remove(struct ngbe_filter_info *filter_info,
			      uint8_t idx)
{
	if (idx >= NGBE_ETF_ID_MAX)
		return -1;
	filter_info->ethertype_mask &= ~(1 << idx);
	filter_info->ethertype_filters[idx].ethertype = 0;
	filter_info->ethertype_filters[idx].etqf = 0;
	filter_info->ethertype_filters[idx].etqs = 0;
	filter_info->ethertype_filters[idx].etqs = FALSE;
	return idx;
}

/* High threshold controlling when to start sending XOFF frames. */
#define NGBE_FC_XOFF_HITH              128 /*KB*/
/* Low threshold controlling when to start sending XON frames. */
#define NGBE_FC_XON_LOTH               64 /*KB*/

/* Timer value included in XOFF frames. */
#define NGBE_FC_PAUSE_TIME 0x680

#define NGBE_LINK_DOWN_CHECK_TIMEOUT 4000 /* ms */
#define NGBE_LINK_UP_CHECK_TIMEOUT   1000 /* ms */
#define NGBE_VMDQ_NUM_UC_MAC         4096 /* Maximum nb. of UC MAC addr. */

#define NGBE_MMW_SIZE_DEFAULT        0x4
#define NGBE_MMW_SIZE_JUMBO_FRAME    0x14 

/*
 *  Default values for RX/TX configuration
 */
#define NGBE_DEFAULT_RX_FREE_THRESH  32
#define NGBE_DEFAULT_RX_PTHRESH      8
#define NGBE_DEFAULT_RX_HTHRESH      8
#define NGBE_DEFAULT_RX_WTHRESH      0

#define NGBE_DEFAULT_TX_FREE_THRESH  32
#define NGBE_DEFAULT_TX_PTHRESH      32
#define NGBE_DEFAULT_TX_HTHRESH      0
#define NGBE_DEFAULT_TX_WTHRESH      0

/* Additional timesync values. */
#define NSEC_PER_SEC             1000000000L
#define NGBE_INCVAL_10GB        0xCCCCCC
#define NGBE_INCVAL_1GB         0x800000
#define NGBE_INCVAL_100         0xA00000
#define NGBE_INCVAL_10          0xC7F380
#define NGBE_INCVAL_FPGA        0x800000
#define NGBE_INCVAL_SHIFT_10GB  20
#define NGBE_INCVAL_SHIFT_1GB   18
#define NGBE_INCVAL_SHIFT_100   15
#define NGBE_INCVAL_SHIFT_10    12
#define NGBE_INCVAL_SHIFT_FPGA  17

#define NGBE_CYCLECOUNTER_MASK   0xffffffffffffffffULL

/* store statistics names and its offset in stats structure */
struct rte_ngbe_xstats_name_off {
	char name[RTE_ETH_XSTATS_NAME_SIZE];
	unsigned offset;
};

const uint32_t *ngbe_dev_supported_ptypes_get(struct rte_eth_dev *dev);
int ngbe_dev_set_mc_addr_list(struct rte_eth_dev *dev,
				      struct ether_addr *mc_addr_set,
				      uint32_t nb_mc_addr);
int ngbe_dev_rss_reta_update(struct rte_eth_dev *dev,
			struct rte_eth_rss_reta_entry64 *reta_conf,
			uint16_t reta_size);
int ngbe_dev_rss_reta_query(struct rte_eth_dev *dev,
			struct rte_eth_rss_reta_entry64 *reta_conf,
			uint16_t reta_size);
void ngbe_vlan_hw_strip_bitmap_set(struct rte_eth_dev *dev,
		uint16_t queue, bool on);
void ngbe_config_vlan_strip_on_all_queues(struct rte_eth_dev *dev,
						  int mask);
void ngbe_dev_setup_link_alarm_handler(void *param);
void ngbe_check_link_alarm(void *param);
void ngbe_read_stats_registers(struct ngbe_hw *hw,
			   struct ngbe_hw_stats *hw_stats);

bool is_ngbe_supported(struct rte_eth_dev *dev);

#endif /* _NGBE_ETHDEV_H_ */
