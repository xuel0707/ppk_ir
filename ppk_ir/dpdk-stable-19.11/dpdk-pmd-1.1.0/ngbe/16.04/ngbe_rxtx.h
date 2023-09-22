/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _NGBE_RXTX_H_
#define _NGBE_RXTX_H_

/*****************************************************************************
 * Receive Descriptor
 *****************************************************************************/
struct ngbe_rx_desc {
	struct {
		union {
			__le32 dw0;
			struct {
				__le16 pkt;
				__le16 hdr;
			} lo;
		};
		union {
			__le32 dw1;
			struct {
				__le16 ipid;
				__le16 csum;
			} hi;
		};
	} qw0; /* also as r.pkt_addr */
	struct {
		union {
			__le32 dw2;
			struct {
				__le32 status;
			} lo;
		};
		union {
			__le32 dw3;
			struct {
				__le16 len;
				__le16 tag;
			} hi;
		};
	} qw1; /* also as r.hdr_addr */
};

/* @ngbe_rx_desc.qw0 */
#define NGBE_RXD_PKTADDR(rxd, v)  \
	(((volatile __le64 *)(rxd))[0] = cpu_to_le64(v))

/* @ngbe_rx_desc.qw1 */
#define NGBE_RXD_HDRADDR(rxd, v)  \
	(((volatile __le64 *)(rxd))[1] = cpu_to_le64(v))

/* @ngbe_rx_desc.dw0 */
#define NGBE_RXD_RSSTYPE(dw)      RS(dw, 0, 0xF)
#define   NGBE_RSSTYPE_NONE     0
#define   NGBE_RSSTYPE_IPV4TCP  1
#define   NGBE_RSSTYPE_IPV4     2
#define   NGBE_RSSTYPE_IPV6TCP  3
#define   NGBE_RSSTYPE_IPV4SCTP 4
#define   NGBE_RSSTYPE_IPV6     5
#define   NGBE_RSSTYPE_IPV6SCTP 6
#define   NGBE_RSSTYPE_IPV4UDP  7
#define   NGBE_RSSTYPE_IPV6UDP  8
#define   NGBE_RSSTYPE_FDIR     15
#define NGBE_RXD_SECTYPE(dw)      RS(dw, 4, 0x3)
#define NGBE_RXD_SECTYPE_NONE     LS(0, 4, 0x3)
//#define NGBE_RXD_SECTYPE_LINKSEC  LS(1, 4, 0x3)
#define NGBE_RXD_SECTYPE_IPSECESP LS(2, 4, 0x3)
#define NGBE_RXD_SECTYPE_IPSECAH  LS(3, 4, 0x3)
#define NGBE_RXD_TPIDSEL(dw)      RS(dw, 6, 0x7)
#define NGBE_RXD_PTID(dw)         RS(dw, 9, 0xFF)
#define   NGBE_RXD_PTID_SHIFT     9
#define NGBE_RXD_RSCCNT(dw)       RS(dw, 17, 0xF)
#define NGBE_RXD_HDRLEN(dw)       RS(dw, 21, 0x3FF)
#define NGBE_RXD_SPH              MS(31, 0x1)

/* @ngbe_rx_desc.dw1 */
/** bit 0-31, as rss hash when  **/
#define NGBE_RXD_RSSHASH(rxd)     ((rxd)->qw0.dw1)

/** bit 0-31, as ip csum when  **/
#define NGBE_RXD_IPID(rxd)        ((rxd)->qw0.hi.ipid)
#define NGBE_RXD_CSUM(rxd)        ((rxd)->qw0.hi.csum)

/** bit 0-31, as fdir id when  **/
#define NGBE_RXD_FDIRID(rxd)      ((rxd)->qw0.hi.dw1)

/* @ngbe_rx_desc.dw2 */
#define NGBE_RXD_STATUS(rxd)      ((rxd)->qw1.lo.status)
/** bit 0-1 **/
#define NGBE_RXD_STAT_DD          MS(0, 0x1) /* Descriptor Done */
#define NGBE_RXD_STAT_EOP         MS(1, 0x1) /* End of Packet */
/** bit 2-31, when EOP=0 **/
#define NGBE_RXD_NEXTP_RESV(v)    LS(v, 2, 0x3)
#define NGBE_RXD_NEXTP(dw)        RS(dw, 4, 0xFFFF) /* Next Descriptor */
/** bit 2-31, when EOP=1 **/
#define NGBE_RXD_PKT_CLS_MASK     MS(2, 0x7) /* Packet Class */
#define NGBE_RXD_PKT_CLS_TC_RSS   LS(0, 2, 0x7) /* RSS Hash */
#define NGBE_RXD_PKT_CLS_FLM      LS(1, 2, 0x7) /* FDir Match */
#define NGBE_RXD_PKT_CLS_SYN      LS(2, 2, 0x7) /* TCP Sync */
#define NGBE_RXD_PKT_CLS_5TUPLE   LS(3, 2, 0x7) /* 5 Tuple */
#define NGBE_RXD_PKT_CLS_ETF      LS(4, 2, 0x7) /* Ethertype Filter */
#define NGBE_RXD_STAT_VLAN        MS(5, 0x1) /* IEEE VLAN Packet */
#define NGBE_RXD_STAT_UDPCS       MS(6, 0x1) /* UDP xsum calculated */
#define NGBE_RXD_STAT_L4CS        MS(7, 0x1) /* L4 xsum calculated */
#define NGBE_RXD_STAT_IPCS        MS(8, 0x1) /* IP xsum calculated */
#define NGBE_RXD_STAT_PIF         MS(9, 0x1) /* Non-unicast address */
#define NGBE_RXD_STAT_EIPCS       MS(10, 0x1) /* Encap IP xsum calculated */
#define NGBE_RXD_STAT_VEXT        MS(11, 0x1) /* Multi-VLAN */
#define NGBE_RXD_STAT_IPV6EX      MS(12, 0x1) /* IPv6 with option header */
#define NGBE_RXD_STAT_LLINT       MS(13, 0x1) /* Pkt caused LLI */
#define NGBE_RXD_STAT_1588        MS(14, 0x1) /* IEEE1588 Time Stamp */
#define NGBE_RXD_STAT_SECP        MS(15, 0x1) /* Security Processing */
#define NGBE_RXD_STAT_LB          MS(16, 0x1) /* Loopback Status */
/*** bit 17-30, when PTYPE=IP ***/
#define NGBE_RXD_STAT_BMC         MS(17, 0x1) /* PTYPE=IP, BMC status */
#define NGBE_RXD_ERR_FDIR_LEN     MS(20, 0x1) /* FDIR Length error */
#define NGBE_RXD_ERR_FDIR_DROP    MS(21, 0x1) /* FDIR Drop error */
#define NGBE_RXD_ERR_FDIR_COLL    MS(22, 0x1) /* FDIR Collision error */
#define NGBE_RXD_ERR_HBO          MS(23, 0x1) /* Header Buffer Overflow */
#define NGBE_RXD_ERR_EIPCS        MS(26, 0x1) /* Encap IP header error */
#define NGBE_RXD_ERR_SECERR       MS(27, 0x1) /* macsec or ipsec error */
#define NGBE_RXD_ERR_RXE          MS(29, 0x1) /* Any MAC Error */
#define NGBE_RXD_ERR_L4CS         MS(30, 0x1) /* TCP/UDP xsum error */
#define NGBE_RXD_ERR_IPCS         MS(31, 0x1) /* IP xsum error */
#define NGBE_RXD_ERR_CSUM(dw)     RS(dw, 30, 0x3)
/*** bit 17-30, when PTYPE=FCOE ***/
#define NGBE_RXD_STAT_FCOEFS      MS(17, 0x1) /* PTYPE=FCOE, FCoE EOF/SOF */
#define NGBE_RXD_FCSTAT_MASK      MS(18, 0x3) /* FCoE Pkt Stat */
#define NGBE_RXD_FCSTAT_NOMTCH    LS(0, 18, 0x3) /* No Ctxt Match */
#define NGBE_RXD_FCSTAT_NODDP     LS(1, 18, 0x3) /* Ctxt w/o DDP */
#define NGBE_RXD_FCSTAT_FCPRSP    LS(2, 18, 0x3) /* Recv. FCP_RSP */
#define NGBE_RXD_FCSTAT_DDP       LS(3, 18, 0x3) /* Ctxt w/ DDP */
#define NGBE_RXD_FCERR_MASK       MS(20, 0x7) /* FCERR */
#define NGBE_RXD_FCERR_0          LS(0, 20, 0x7)
#define NGBE_RXD_FCERR_1          LS(1, 20, 0x7)
#define NGBE_RXD_FCERR_2          LS(2, 20, 0x7)
#define NGBE_RXD_FCERR_3          LS(3, 20, 0x7)
#define NGBE_RXD_FCERR_4          LS(4, 20, 0x7)
#define NGBE_RXD_FCERR_5          LS(5, 20, 0x7)
#define NGBE_RXD_FCERR_6          LS(6, 20, 0x7)
#define NGBE_RXD_FCERR_7          LS(7, 20, 0x7)

/* @ngbe_rx_desc.dw3 */
#define NGBE_RXD_LENGTH(rxd)           ((rxd)->qw1.hi.len)
#define NGBE_RXD_VLAN(rxd)             ((rxd)->qw1.hi.tag)

/******************************************************************************
 * Transmit Descriptor
******************************************************************************/
/**
 * Transmit Context Descriptor (NGBE_TXD_TYP=CTXT)
 **/
struct ngbe_tx_ctx_desc {
	__le32 dw0; /* w.vlan_macip_lens  */
	__le32 dw1; /* w.seqnum_seed      */
	__le32 dw2; /* w.type_tucmd_mlhl  */
	__le32 dw3; /* w.mss_l4len_idx    */
};

/* @ngbe_tx_ctx_desc.dw0 */
#define NGBE_TXD_IPLEN(v)         LS(v, 0, 0x1FF) /* ip/fcoe header end */
#define NGBE_TXD_MACLEN(v)        LS(v, 9, 0x7F) /* desc mac len */
#define NGBE_TXD_VLAN(v)          LS(v, 16, 0xFFFF) /* vlan tag */

/* @ngbe_tx_ctx_desc.dw1 */
/*** bit 0-31, when NGBE_TXD_DTYP_FCOE=0 ***/
#define NGBE_TXD_IPSEC_SAIDX(v)   LS(v, 0, 0x3FF) /* ipsec SA index */
#define NGBE_TXD_ETYPE(v)         LS(v, 11, 0x1) /* tunnel type */
#define NGBE_TXD_ETYPE_UDP        LS(0, 11, 0x1)
#define NGBE_TXD_ETYPE_GRE        LS(1, 11, 0x1)
#define NGBE_TXD_EIPLEN(v)        LS(v, 12, 0x7F) /* tunnel ip header */
#define NGBE_TXD_DTYP_FCOE        MS(16, 0x1) /* FCoE/IP descriptor */
#define NGBE_TXD_ETUNLEN(v)       LS(v, 21, 0xFF) /* tunnel header */
#define NGBE_TXD_DECTTL(v)        LS(v, 29, 0xF) /* decrease ip TTL */
/*** bit 0-31, when NGBE_TXD_DTYP_FCOE=1 ***/
#define NGBE_TXD_FCOEF_EOF_MASK   MS(10, 0x3) /* FC EOF index */
#define NGBE_TXD_FCOEF_EOF_N      LS(0, 10, 0x3) /* EOFn */
#define NGBE_TXD_FCOEF_EOF_T      LS(1, 10, 0x3) /* EOFt */
#define NGBE_TXD_FCOEF_EOF_NI     LS(2, 10, 0x3) /* EOFni */
#define NGBE_TXD_FCOEF_EOF_A      LS(3, 10, 0x3) /* EOFa */
#define NGBE_TXD_FCOEF_SOF        MS(12, 0x1) /* FC SOF index */
#define NGBE_TXD_FCOEF_PARINC     MS(13, 0x1) /* Rel_Off in F_CTL */
#define NGBE_TXD_FCOEF_ORIE       MS(14, 0x1) /* orientation end */
#define NGBE_TXD_FCOEF_ORIS       MS(15, 0x1) /* orientation start */

/* @ngbe_tx_ctx_desc.dw2 */
#define NGBE_TXD_IPSEC_ESPLEN(v)  LS(v, 1, 0x1FF) /* ipsec ESP length */
#define NGBE_TXD_SNAP             MS(10, 0x1) /* SNAP indication */
#define NGBE_TXD_TPID_SEL(v)      LS(v, 11, 0x7) /* vlan tag index */
#define NGBE_TXD_IPSEC_ESP        MS(14, 0x1) /* ipsec type: esp=1 ah=0 */
#define NGBE_TXD_IPSEC_ESPENC     MS(15, 0x1) /* ESP encrypt */
#define NGBE_TXD_CTXT             MS(20, 0x1) /* context descriptor */
#define NGBE_TXD_PTID(v)          LS(v, 24, 0xFF) /* packet type */
/* @ngbe_tx_ctx_desc.dw3 */
#define NGBE_TXD_DD               MS(0, 0x1) /* descriptor done */
#define NGBE_TXD_IDX(v)           LS(v, 4, 0x1) /* ctxt desc index */
#define NGBE_TXD_L4LEN(v)         LS(v, 8, 0xFF) /* l4 header length */
#define NGBE_TXD_MSS(v)           LS(v, 16, 0xFFFF) /* l4 MSS */

/**
 * Transmit Data Descriptor (NGBE_TXD_TYP=DATA)
 **/
struct ngbe_tx_desc {
	__le64 qw0; /* r.buffer_addr ,  w.reserved    */
	__le32 dw2; /* r.cmd_type_len,  w.nxtseq_seed */
	__le32 dw3; /* r.olinfo_status, w.status      */
};
/* @ngbe_tx_desc.qw0 */

/* @ngbe_tx_desc.dw2 */
#define NGBE_TXD_DATLEN(v)        ((0xFFFF & (v))) /* data buffer length */
#define NGBE_TXD_1588             ((0x1) << 19) /* IEEE1588 time stamp */
#define NGBE_TXD_DATA             ((0x0) << 20) /* data descriptor */
#define NGBE_TXD_EOP              ((0x1) << 24) /* End of Packet */
#define NGBE_TXD_FCS              ((0x1) << 25) /* Insert FCS */
#define NGBE_TXD_LINKSEC          ((0x1) << 26) /* Insert LinkSec */
//#define NGBE_TXD_RS               ((0x1) << 27) /* Report Status */
#define NGBE_TXD_ECU              ((0x1) << 28) /* forward to ECU */
#define NGBE_TXD_CNTAG            ((0x1) << 29) /* insert CN tag */
#define NGBE_TXD_VLE              ((0x1) << 30) /* insert VLAN tag */
#define NGBE_TXD_TSE              ((0x1) << 31) /* transmit segmentation */

#define NGBE_TXD_FLAGS (NGBE_TXD_FCS | NGBE_TXD_EOP)

/* @ngbe_tx_desc.dw3 */
#define NGBE_TXD_DD_UNUSED        NGBE_TXD_DD
#define NGBE_TXD_IDX_UNUSED(v)    NGBE_TXD_IDX(v)
#define NGBE_TXD_CC               ((0x1) << 7) /* check context */
#define NGBE_TXD_IPSEC            ((0x1) << 8) /* request ipsec offload */
#define NGBE_TXD_L4CS             ((0x1) << 9) /* insert TCP/UDP/SCTP csum */
#define NGBE_TXD_IPCS             ((0x1) << 10) /* insert IPv4 csum */
#define NGBE_TXD_EIPCS            ((0x1) << 11) /* insert outer IP csum */
#define NGBE_TXD_MNGFLT           ((0x1) << 12) /* enable management filter */
#define NGBE_TXD_PAYLEN(v)        ((0x7FFFF & (v)) << 13) /* payload length */

/*
 * Rings setup and release.
 *
 * TDBA/RDBA should be aligned on 16 byte boundary. But TDLEN/RDLEN should be
 * multiple of 128 bytes. So we align TDBA/RDBA on 128 byte boundary. This will
 * also optimize cache line size effect. H/W supports up to cache line size 128.
 */

/*
 * Maximum number of Ring Descriptors.
 *
 * Since RDLEN/TDLEN should be multiple of 128 bytes, the number of ring
 * descriptors should meet the following condition:
 *      (num_ring_desc * sizeof(rx/tx descriptor)) % 128 == 0
 */

#define RTE_PMD_NGBE_TX_MAX_BURST 32
#define RTE_PMD_NGBE_RX_MAX_BURST 32
#define RTE_NGBE_TX_MAX_FREE_BUF_SZ 64

#define RTE_NGBE_DESCS_PER_LOOP    4

#ifdef RTE_NGBE_INC_VECTOR
#define RTE_NGBE_RXQ_REARM_THRESH      32
#define RTE_NGBE_MAX_RX_BURST          RTE_NGBE_RXQ_REARM_THRESH
#endif

#define RX_RING_SZ ((NGBE_RING_DESC_MAX + RTE_PMD_NGBE_RX_MAX_BURST) * \
		    sizeof(struct ngbe_rx_desc))

#ifdef RTE_PMD_PACKET_PREFETCH
#define rte_packet_prefetch(p)  rte_prefetch1(p)
#else
#define rte_packet_prefetch(p)  do {} while(0)
#endif

#define RTE_NGBE_REGISTER_POLL_WAIT_10_MS  10
#define RTE_NGBE_WAIT_100_US               100

#define NGBE_TX_MAX_SEG                    40

/**
 * Structure associated with each descriptor of the RX ring of a RX queue.
 */
struct ngbe_rx_entry {
	struct rte_mbuf *mbuf; /**< mbuf associated with RX descriptor. */
};

struct ngbe_scattered_rx_entry {
	struct rte_mbuf *fbuf; /**< First segment of the fragmented packet. */
};

/**
 * Structure associated with each descriptor of the TX ring of a TX queue.
 */
struct ngbe_tx_entry {
	struct rte_mbuf *mbuf; /**< mbuf associated with TX desc, if any. */
	uint16_t next_id; /**< Index of next descriptor in ring. */
	uint16_t last_id; /**< Index of last scattered descriptor. */
};

/**
 * Structure associated with each descriptor of the TX ring of a TX queue.
 */
struct ngbe_tx_entry_v {
	struct rte_mbuf *mbuf; /**< mbuf associated with TX desc, if any. */
};

/**
 * Structure associated with each RX queue.
 */
struct ngbe_rx_queue {
	struct rte_mempool  *mb_pool; /**< mbuf pool to populate RX ring. */
	volatile struct ngbe_rx_desc *rx_ring; /**< RX ring virtual address. */
	uint64_t            rx_ring_phys_addr; /**< RX ring DMA address. */
	volatile uint32_t   *rdt_reg_addr; /**< RDT register address. */
	volatile uint32_t   *rdh_reg_addr; /**< RDH register address. */
	struct ngbe_rx_entry *sw_ring; /**< address of RX software ring. */
	struct ngbe_scattered_rx_entry *sw_sc_ring; /**< address of scattered Rx software ring. */
	struct rte_mbuf *pkt_first_seg; /**< First segment of current packet. */
	struct rte_mbuf *pkt_last_seg; /**< Last segment of current packet. */
	uint64_t            mbuf_initializer; /**< value to init mbufs */
	uint16_t            nb_rx_desc; /**< number of RX descriptors. */
	uint16_t            rx_tail;  /**< current value of RDT register. */
	uint16_t            nb_rx_hold; /**< number of held free RX desc. */
	uint16_t rx_nb_avail; /**< nr of staged pkts ready to ret to app */
	uint16_t rx_next_avail; /**< idx of next staged pkt to ret to app */
	uint16_t rx_free_trigger; /**< triggers rx buffer allocation */
	uint8_t            rx_using_sse;
	/**< indicates that vector RX is in use */
#ifdef RTE_LIBRTE_SECURITY
	uint8_t            using_ipsec;
	/**< indicates that IPsec RX feature is in use */
#endif
#ifdef RTE_NGBE_INC_VECTOR
	uint16_t            rxrearm_nb;     /**< number of remaining to be re-armed */
	uint16_t            rxrearm_start;  /**< the idx we start the re-arming from */
#endif
	uint16_t            rx_free_thresh; /**< max free RX desc to hold. */
	uint16_t            queue_id; /**< RX queue index. */
	uint16_t            reg_idx;  /**< RX queue register index. */
	uint16_t            pkt_type_mask;  /**< Packet type mask for different NICs. */
	uint16_t            port_id;  /**< Device port identifier. */
	uint8_t             crc_len;  /**< 0 if CRC stripped, 4 otherwise. */
	uint8_t             drop_en;  /**< If not 0, set SRRCTL.Drop_En. */
	uint8_t             rx_deferred_start; /**< not in global dev start. */
	/** flags to set in mbuf when a vlan is detected. */
	uint64_t            vlan_flags;
	uint64_t	    offloads; /**< Rx offloads with DEV_RX_OFFLOAD_* */
	/** need to alloc dummy mbuf, for wraparound when scanning hw ring */
	struct rte_mbuf fake_mbuf;
	/** hold packets to return to application */
	struct rte_mbuf *rx_stage[RTE_PMD_NGBE_RX_MAX_BURST*2];
};

/**
 * NGBE CTX Constants
 */
enum ngbe_ctx_num {
	NGBE_CTX_0    = 0, /**< CTX0 */
	NGBE_CTX_1    = 1, /**< CTX1  */
	NGBE_CTX_NUM  = 2, /**< CTX NUMBER  */
};

/** Offload features */
union ngbe_tx_offload {
	uint64_t data[2];
	struct {
		uint64_t ptid:8; /**< Packet Type Identifier. */
		uint64_t l2_len:7; /**< L2 (MAC) Header Length. */
		uint64_t l3_len:9; /**< L3 (IP) Header Length. */
		uint64_t l4_len:8; /**< L4 (TCP/UDP) Header Length. */
		uint64_t tso_segsz:16; /**< TCP TSO segment size */
		uint64_t vlan_tci:16;
		/**< VLAN Tag Control Identifier (CPU order). */

		/* fields for TX offloading of tunnels */
		uint64_t outer_tun_len:8; /**< Outer TUN (Tunnel) Hdr Length. */
		uint64_t outer_l2_len:8; /**< Outer L2 (MAC) Hdr Length. */
		uint64_t outer_l3_len:16; /**< Outer L3 (IP) Hdr Length. */
#ifdef RTE_LIBRTE_SECURITY
		/* inline ipsec related*/
		uint64_t sa_idx:8;	/**< TX SA database entry index */
		uint64_t sec_pad_len:4;	/**< padding length */
#endif
	};
};

/*
 * Compare mask for vlan_macip_len.data,
 * should be in sync with ngbe_vlan_macip.f layout.
 * */
#define TX_VLAN_CMP_MASK        0xFFFF0000  /**< VLAN length - 16-bits. */
#define TX_MAC_LEN_CMP_MASK     0x0000FE00  /**< MAC length - 7-bits. */
#define TX_IP_LEN_CMP_MASK      0x000001FF  /**< IP  length - 9-bits. */
/** MAC+IP  length. */
#define TX_MACIP_LEN_CMP_MASK   (TX_MAC_LEN_CMP_MASK | TX_IP_LEN_CMP_MASK)

/**
 * Structure to check if new context need be built
 */

struct ngbe_ctx_info {
	uint64_t flags;           /**< ol_flags for context build. */
	/**< tx offload: vlan, tso, l2-l3-l4 lengths. */
	union ngbe_tx_offload tx_offload;
	/** compare mask for tx offload. */
	union ngbe_tx_offload tx_offload_mask;
};

/**
 * Structure associated with each TX queue.
 */
struct ngbe_tx_queue {
	/** TX ring virtual address. */
	volatile struct ngbe_tx_desc *tx_ring;
	uint64_t            tx_ring_phys_addr; /**< TX ring DMA address. */
	union {
		struct ngbe_tx_entry *sw_ring; /**< address of SW ring for scalar PMD. */
		struct ngbe_tx_entry_v *sw_ring_v; /**< address of SW ring for vector PMD */
	};
	volatile uint32_t   *tdt_reg_addr; /**< Address of TDT register. */
	volatile uint32_t   *tdc_reg_addr; /**< Address of TDC register. */
	uint16_t            nb_tx_desc;    /**< number of TX descriptors. */
	uint16_t            tx_tail;       /**< current value of TDT reg. */
	/**< Start freeing TX buffers if there are less free descriptors than
	     this value. */
	uint16_t            tx_free_thresh;
	/** Index to last TX descriptor to have been cleaned. */
	uint16_t            last_desc_cleaned;
	/** Total number of TX descriptors ready to be allocated. */
	uint16_t            nb_tx_free;
	uint16_t tx_next_dd; /**< next desc to scan for DD bit */
	uint16_t            queue_id;      /**< TX queue index. */
	uint16_t            reg_idx;       /**< TX queue register index. */
	uint16_t            port_id;       /**< Device port identifier. */
	uint8_t             pthresh;       /**< Prefetch threshold register. */
	uint8_t             hthresh;       /**< Host threshold register. */
	uint8_t             wthresh;       /**< Write-back threshold reg. */
	uint64_t offloads; /**< Tx offload flags of DEV_TX_OFFLOAD_* */
	uint32_t            ctx_curr;      /**< Hardware context states. */
	/** Hardware context0 history. */
	struct ngbe_ctx_info ctx_cache[NGBE_CTX_NUM];
	const struct ngbe_txq_ops *ops;       /**< txq ops */
	uint8_t             tx_deferred_start; /**< not in global dev start. */
#ifdef RTE_LIBRTE_SECURITY
	uint8_t		    using_ipsec;
	/**< indicates that IPsec TX feature is in use */
#endif
};

struct ngbe_txq_ops {
	void (*release_mbufs)(struct ngbe_tx_queue *txq);
	void (*free_swring)(struct ngbe_tx_queue *txq);
	void (*reset)(struct ngbe_tx_queue *txq);
};

/*
 * Populate descriptors with the following info:
 * 1.) buffer_addr = phys_addr + headroom
 * 2.) cmd_type_len = NGBE_TXD_FLAGS | pkt_len
 * 3.) olinfo_status = pkt_len << PAYLEN_SHIFT
 */


/* Takes an ethdev and a queue and sets up the tx function to be used based on
 * the queue parameters. Used in tx_queue_setup by primary process and then
 * in dev_init by secondary process when attaching to an existing ethdev.
 */
void ngbe_set_tx_function(struct rte_eth_dev *dev, struct ngbe_tx_queue *txq);

/**
 * Sets the rx_pkt_burst callback in the ngbe rte_eth_dev instance.
 *
 * Sets the callback based on the device parameters:
 *  - ngbe_hw.rx_bulk_alloc_allowed
 *  - rte_eth_dev_data.scattered_rx
 *  - rte_eth_dev_data.lro
 *  - conditions checked in ngbe_rx_vec_condition_check()
 *
 *  This means that the parameters above have to be configured prior to calling
 *  to this function.
 *
 * @dev rte_eth_dev handle
 */
void ngbe_set_rx_function(struct rte_eth_dev *dev);

uint16_t ngbe_recv_pkts_vec(void *rx_queue, struct rte_mbuf **rx_pkts,
		uint16_t nb_pkts);
uint16_t ngbe_recv_scattered_pkts_vec(void *rx_queue,
		struct rte_mbuf **rx_pkts, uint16_t nb_pkts);
int ngbe_rx_vec_dev_conf_condition_check(struct rte_eth_dev *dev);
int ngbe_rxq_vec_setup(struct ngbe_rx_queue *rxq);
void ngbe_rx_queue_release_mbufs_vec(struct ngbe_rx_queue *rxq);

#ifdef RTE_NGBE_INC_VECTOR

uint16_t ngbe_xmit_fixed_burst_vec(void *tx_queue, struct rte_mbuf **tx_pkts,
				    uint16_t nb_pkts);
int ngbe_txq_vec_setup(struct ngbe_tx_queue *txq);
#endif /* RTE_NGBE_INC_VECTOR */

uint64_t ngbe_get_tx_port_offloads(struct rte_eth_dev *dev);
uint64_t ngbe_get_rx_queue_offloads(struct rte_eth_dev *dev);
uint64_t ngbe_get_rx_port_offloads(struct rte_eth_dev *dev);
uint64_t ngbe_get_tx_queue_offloads(struct rte_eth_dev *dev);

#endif /* _NGBE_RXTX_H_ */
