
#ifndef _RTE_PMD_COMPAT_H_
#define _RTE_PMD_COMPAT_H_

#include <rte_version.h>
#include <rte_config.h>
#include <rte_ethdev.h>

/* Driver Compat Parameter Value */
#define DCPV(major, minor) (((major) << 8) + (minor))

/******************************************************************************
 * header files
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(17, 0, 0, 0)
#include <rte_memzone.h>
#include <rte_pci.h>
#undef RTE_PMD_COMPAT_TM

#elif RTE_VERSION < RTE_VERSION_NUM(17, 5, 0, 0)
#include <rte_io.h>
#include <rte_pci.h>
#undef RTE_PMD_COMPAT_TM

#elif RTE_VERSION < RTE_VERSION_NUM(17, 8, 0, 0)
#include <rte_ethdev.h>
#include <rte_ethdev_pci.h>
#include <rte_io.h>
#include <rte_flow.h>
#undef RTE_PMD_COMPAT_TM

#elif RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#include <rte_ethdev.h>
#include <rte_ethdev_pci.h>
#include <rte_io.h>
#include <rte_flow.h>
#include <rte_tm_driver.h>
#undef RTE_PMD_COMPAT_TM

#elif RTE_VERSION < RTE_VERSION_NUM(18, 0, 0, 0)
#include <rte_net.h>
#include <rte_ethdev.h>
#include <rte_bus_pci.h>
#include <rte_io.h>
#include <rte_flow.h>
#include <rte_tm_driver.h>
#include <rte_bus_pci.h>
#include <rte_ethdev_pci.h>
#define RTE_PMD_COMPAT_TM        DCPV(1,0)

#elif RTE_VERSION < RTE_VERSION_NUM(19, 11, 0, 0)
#include <rte_net.h>
#include <rte_ethdev_driver.h>
#include <rte_bus_pci.h>
#include <rte_io.h>
#include <rte_flow.h>
#include <rte_tm_driver.h>
#include <rte_bus_pci.h>
#include <rte_ethdev_pci.h>
#define RTE_PMD_COMPAT_TM        DCPV(1,0)

#else /* RTE_VERSION < RTE_VERSION_NUM(*, 0, 0, 0) */
#include <rte_net.h>
#include <rte_ethdev_driver.h>
#include <rte_bus_pci.h>
#include <rte_io.h>
#include <rte_flow.h>
#include <rte_tm_driver.h>
#include <rte_bus_pci.h>
#include <rte_ethdev_pci.h>
#include <rte_vxlan.h>
#define RTE_PMD_COMPAT_TM        DCPV(1,0)

#endif

/******************************************************************************
 * syntax compilation
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(17, 8, 0, 0)
#define __rte_always_inline inline __attribute__((always_inline))
#define __rte_experimental
#elif RTE_VERSION < RTE_VERSION_NUM(18, 0, 0, 0)
#define __rte_experimental
#else /* RTE_VERSION < RTE_VERSION_NUM(*, 0, 0, 0) */

#endif

/**
 * Response sent back to driver from user app after callback
 */
enum rte_pmd_compat_mb_event_rsp {
	RTE_PMD_COMPAT_MB_EVENT_NOOP_ACK,  /**< skip mbox request and ACK */
	RTE_PMD_COMPAT_MB_EVENT_NOOP_NACK, /**< skip mbox request and NACK */
	RTE_PMD_COMPAT_MB_EVENT_PROCEED,  /**< proceed with mbox request  */
	RTE_PMD_COMPAT_MB_EVENT_MAX       /**< max value of this enum */
};

/**
 * Data sent to the user application when the callback is executed.
 */
struct rte_pmd_compat_mb_event_param {
	uint16_t vfid;     /**< Virtual Function number */
	uint16_t msg_type; /**< VF to PF message type */
	uint16_t retval;   /**< return value */
	void *msg;         /**< pointer to message */
};


/******************************************************************************
 * eth_dev_ops
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(16, 7, 0, 0)
#define rte_eth_xstat    rte_eth_xstats
#endif

/******************************************************************************
 * pci interfaces
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(17, 5, 0, 0)
#define rte_pci_read_config    rte_eal_pci_read_config
#define rte_pci_write_config    rte_eal_pci_write_config
#endif

/******************************************************************************
 * ioaddr interfaces
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(17, 0, 0, 0)
#define rte_io_wmb()   rte_wmb()
#define rte_io_rmb()   rte_rmb()
static __rte_always_inline uint32_t
rte_read32_relaxed(const volatile void *addr)
{
	return (uint32_t)*((const volatile uint32_t *)addr);
}
static __rte_always_inline void
rte_write32_relaxed(uint32_t val, volatile void *addr)
{
	*((volatile uint32_t *)addr) = val;
}
static __rte_always_inline uint32_t
rte_read32(const volatile void *addr)
{
	uint32_t val;
	val = rte_read32_relaxed(addr);
	rte_io_rmb();
	return val;
}
static __rte_always_inline void
rte_write32(uint32_t value, volatile void *addr)
{
	rte_io_wmb();
	rte_write32_relaxed(value, addr);
}

#else /* RTE_VERSION < RTE_VERSION_NUM(*, 0, 0, 0) */
#endif

/******************************************************************************
 * memory interfaces
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define TMZ_PADDR(mz) rte_mem_phy2mch((mz)->memseg_id, (mz)->phys_addr)
#define TMZ_VADDR(mz) ((mz)->addr)
#else
#define TMZ_PADDR(mz)  ((mz)->iova)
#define TMZ_VADDR(mz)  ((mz)->addr)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 5, 0, 0)
#define TDEV_NAME(eth_dev)  ((eth_dev)->data->name)
#else
#define TDEV_NAME(eth_dev)  ((eth_dev)->device->name)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 5, 0, 0)
#define rte_pktmbuf_prefree_seg(m) __rte_pktmbuf_prefree_seg(m)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define rte_mbuf_data_iova(m)      rte_mbuf_data_dma_addr(m)
#endif
#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define rte_mbuf_data_iova_default rte_mbuf_data_dma_addr_default
#endif

#if RTE_VERSION < RTE_VERSION_NUM(16, 11, 0, 0)
static const void *__rte_pktmbuf_read(const struct rte_mbuf *m, uint32_t off,
	uint32_t len, void *buf)
{
	const struct rte_mbuf *seg = m;
	uint32_t buf_off = 0, copy_len;

	if (off + len > rte_pktmbuf_pkt_len(m))
		return NULL;

	while (off >= rte_pktmbuf_data_len(seg)) {
		off -= rte_pktmbuf_data_len(seg);
		seg = seg->next;
	}

	if (off + len <= rte_pktmbuf_data_len(seg))
		return rte_pktmbuf_mtod_offset(seg, char *, off);

	/* rare case: header is split among several segments */
	while (len > 0) {
		copy_len = rte_pktmbuf_data_len(seg) - off;
		if (copy_len > len)
			copy_len = len;
		rte_memcpy((char *)buf + buf_off,
			rte_pktmbuf_mtod_offset(seg, char *, off), copy_len);
		off = 0;
		buf_off += copy_len;
		len -= copy_len;
		seg = seg->next;
	}

	return buf;
}

static inline const void *rte_pktmbuf_read(const struct rte_mbuf *m,
	uint32_t off, uint32_t len, void *buf)
{
	if (likely(off + len <= rte_pktmbuf_data_len(m)))
		return rte_pktmbuf_mtod_offset(m, char *, off);
	else
		return __rte_pktmbuf_read(m, off, len, buf);
}
#endif

#if RTE_VERSION < RTE_VERSION_NUM(16, 7, 0, 0)
static inline struct rte_mbuf *
rte_mbuf_raw_alloc(struct rte_mempool *mp)
{
	struct rte_mbuf *m;

	m = __rte_mbuf_raw_alloc(mp);
	__rte_mbuf_sanity_check_raw(m, 0);
	return m;
}
#endif

/******************************************************************************
 * ethernet interfaces
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(18, 5, 0, 0)
#define rte_eth_switch_domain_alloc(a)
#define rte_eth_switch_domain_free(a) 0
static inline int
rte_eth_linkstatus_get(struct rte_eth_dev *dev,
				struct rte_eth_link *cur_link)
{
	memset(cur_link, 0, sizeof(*cur_link));
	struct rte_eth_link *dev_link = &(dev->data->dev_link);
	if (!rte_atomic64_cmpset((uint64_t *)cur_link,
		*(uint64_t *)cur_link, *(uint64_t *)dev_link))
		return -1;
	return 0;
}
static inline int
rte_eth_linkstatus_set(struct rte_eth_dev *dev,
				struct rte_eth_link *new_link)
{
	struct rte_eth_link *dev_link = &(dev->data->dev_link);
	if (!rte_atomic64_cmpset((uint64_t *)dev_link,
		*(uint64_t *)dev_link, *(uint64_t *)new_link))
		return -1;
	return 0;
}
#endif

#ifndef RTE_ETH_EVENT_VF_MBOX
#define RTE_ETH_EVENT_VF_MBOX (RTE_ETH_EVENT_MAX + 1)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(16, 11, 0, 0)
static inline int rte_eth_dev_callback_process(
	struct rte_eth_dev *dev, int event,
	void *arg __rte_unused, void *ret)
{
	if (ret)
		((struct rte_pmd_compat_mb_event_param *)(ret))->retval
				= RTE_PMD_COMPAT_MB_EVENT_PROCEED;
	_rte_eth_dev_callback_process(dev, event);
	return 0;
}
#elif RTE_VERSION < RTE_VERSION_NUM(17, 8, 0, 0)
static inline int rte_eth_dev_callback_process(
	struct rte_eth_dev *dev, int event,
	void *arg, void *ret)
{
	if (ret)
		((struct rte_pmd_compat_mb_event_param *)(ret))->retval
				= RTE_PMD_COMPAT_MB_EVENT_PROCEED;
	_rte_eth_dev_callback_process(dev, event, arg);
	return 0;
}
#elif RTE_VERSION < RTE_VERSION_NUM(18, 0, 0, 0)
static inline int rte_eth_dev_callback_process(
	struct rte_eth_dev *dev, int event,
	void *arg, void *ret)
{
	return _rte_eth_dev_callback_process(dev, event, arg, ret);
}
#elif RTE_VERSION < RTE_VERSION_NUM(18, 8, 0, 0)
static inline int rte_eth_dev_callback_process(
	struct rte_eth_dev *dev, int event,
	void *arg __rte_unused, void *ret)
{
	return _rte_eth_dev_callback_process(dev, event, ret);
}
#else
static inline int rte_eth_dev_callback_process(
	struct rte_eth_dev *dev, int event,
	void *arg __rte_unused, void *ret)
{
	return _rte_eth_dev_callback_process(dev, event, ret);
}
#endif

/******************************************************************************
 * packet interfaces
 ******************************************************************************/
#ifndef ETHER_TYPE_ETAG
#define ETHER_TYPE_ETAG   0x893F
#endif
#ifndef ETHER_TYPE_QINQ
#define ETHER_TYPE_QINQ   0x88A8
#endif

#ifndef PKT_RX_L4_CKSUM_GOOD
#define PKT_RX_L4_CKSUM_GOOD   0
#define PKT_RX_IP_CKSUM_GOOD   0
#endif

#ifndef PKT_RX_VLAN_STRIPPED
#define PKT_RX_VLAN_STRIPPED (1ULL << 6)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define PKT_RX_VLAN                PKT_RX_VLAN_PKT
#endif

#if RTE_VERSION < RTE_VERSION_NUM(16, 11, 0, 0)
#define PKT_TX_TUNNEL_VXLAN   (0x1ULL << 45)
#define PKT_TX_TUNNEL_GRE     (0x2ULL << 45)
#define PKT_TX_TUNNEL_IPIP    (0x3ULL << 45)
#define PKT_TX_TUNNEL_GENEVE  (0x4ULL << 45)
#define PKT_TX_TUNNEL_MPLSINUDP (0x5ULL << 45)
#define PKT_TX_TUNNEL_MASK    (0xFULL << 45)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(18, 5, 0, 0)
#define PKT_TX_TUNNEL_IP                (0xDULL << 45)
#define PKT_TX_TUNNEL_VXLAN_GPE         (0x6ULL << 45)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(18, 5, 0, 0)
#define RTE_PTYPE_TUNNEL_VXLAN_GPE          0x0000b000
#endif

#if RTE_VERSION_NUM(17, 2, 0, 0) <= RTE_VERSION && RTE_VERSION < RTE_VERSION_NUM(18, 11, 0, 0)
#undef PKT_TX_OFFLOAD_MASK
#define PKT_TX_OFFLOAD_MASK (    \
		PKT_TX_OUTER_IPV6 |	 \
		PKT_TX_OUTER_IPV4 |	 \
		PKT_TX_OUTER_IP_CKSUM |  \
		PKT_TX_VLAN_PKT |        \
		PKT_TX_IPV6 |		 \
		PKT_TX_IPV4 |		 \
		PKT_TX_IP_CKSUM |        \
		PKT_TX_L4_MASK |         \
		PKT_TX_IEEE1588_TMST |	 \
		PKT_TX_TCP_SEG |         \
		PKT_TX_QINQ_PKT |        \
		PKT_TX_TUNNEL_MASK |	 \
		PKT_TX_MACSEC |		 \
		PKT_TX_SEC_OFFLOAD)
#endif

#ifndef ETH_QINQ_STRIP_MASK
#define ETH_QINQ_STRIP_MASK 0x0008
#endif

#ifndef PKT_TX_SEC_OFFLOAD
#define PKT_TX_SEC_OFFLOAD 0
#endif

#ifndef PKT_TX_IEEE1588_TMST
#define PKT_TX_IEEE1588_TMST  0
#endif

#ifndef RTE_VXLAN_DEFAULT_PORT
#define RTE_VXLAN_DEFAULT_PORT 4789
#endif

#ifndef DEV_TX_OFFLOAD_UDP_TNL_TSO
#define DEV_TX_OFFLOAD_UDP_TNL_TSO	0
#endif

#ifndef DEV_TX_OFFLOAD_IP_TNL_TSO
#define DEV_TX_OFFLOAD_IP_TNL_TSO	0
#endif

#ifndef DEV_TX_OFFLOAD_VXLAN_TNL_TSO
#define DEV_TX_OFFLOAD_VXLAN_TNL_TSO	0
#endif

#ifndef DEV_TX_OFFLOAD_GRE_TNL_TSO
#define DEV_TX_OFFLOAD_GRE_TNL_TSO	0
#endif

#ifndef DEV_TX_OFFLOAD_IPIP_TNL_TSO
#define DEV_TX_OFFLOAD_IPIP_TNL_TSO	0
#endif

#ifndef DEV_TX_OFFLOAD_GENEVE_TNL_TSO
#define DEV_TX_OFFLOAD_GENEVE_TNL_TSO	0
#endif

/******************************************************************************
 * device interfaces
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(17, 0, 0, 0)
#define RTE_ETH_DEV_TO_PCI(eth_dev)     (eth_dev->pci_dev)
#elif RTE_VERSION < RTE_VERSION_NUM(17, 8, 0, 0)
#define RTE_ETH_DEV_TO_PCI(eth_dev)     RTE_DEV_TO_PCI((eth_dev)->device)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(18, 2, 255, 255)
#define RTE_ETH_DEV_INFO_INIT(dev, info)	\
	(info->pci_dev = RTE_ETH_DEV_TO_PCI(dev))
#else
#define RTE_ETH_DEV_INFO_INIT(dev, info)	\
	do {} while(false)
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define DEV_RX_OFFLOAD_SCATTER    0
#define DEV_RX_OFFLOAD_VLAN_FILTER    0
#define DEV_RX_OFFLOAD_VLAN_EXTEND    0
#define DEV_RX_OFFLOAD_JUMBO_FRAME    0
#define DEV_RX_OFFLOAD_CRC_STRIP    0
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define DEV_RX_OFFLOAD_KEEP_CRC    0
#endif

#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define DEV_TX_OFFLOAD_MULTI_SEGS    0
#endif

#if RTE_VERSION < RTE_VERSION_NUM(19, 11, 0, 0)
#define DEV_RX_OFFLOAD_RSS_HASH    0
#endif

#if RTE_VERSION >= RTE_VERSION_NUM(19, 8, 0, 0)
#define ETHER_MIN_MTU                RTE_ETHER_MIN_MTU
#define ETHER_ADDR_LEN               RTE_ETHER_ADDR_LEN
#define ETHER_CRC_LEN                RTE_ETHER_CRC_LEN
#define ETHER_HDR_LEN                RTE_ETHER_HDR_LEN
#define ETHER_MIN_LEN                RTE_ETHER_MIN_LEN
#define ETHER_MAX_LEN                RTE_ETHER_MAX_LEN
#define ETHER_TYPE_1588              RTE_ETHER_TYPE_1588
#define ETHER_TYPE_VLAN              RTE_ETHER_TYPE_VLAN
#define ETHER_TYPE_IPv4              RTE_ETHER_TYPE_IPV4
#define ETHER_TYPE_IPv6              RTE_ETHER_TYPE_IPV6
#define ETHER_MAX_JUMBO_FRAME_LEN    RTE_ETHER_MAX_JUMBO_FRAME_LEN
#define ETHER_LOCAL_ADMIN_ADDR       RTE_ETHER_LOCAL_ADMIN_ADDR
#define IPV4_MAX_PKT_LEN             RTE_IPV4_MAX_PKT_LEN
#define TCP_SYN_FLAG                 RTE_TCP_SYN_FLAG
#define ETHER_MAX_VLAN_ID            RTE_ETHER_MAX_VLAN_ID

#define ether_addr                   rte_ether_addr
#define tcp_hdr                      rte_tcp_hdr
#define udp_hdr                      rte_udp_hdr
#define sctp_hdr                     rte_sctp_hdr

#define eth_random_addr              rte_eth_random_addr
#define ether_addr_copy              rte_ether_addr_copy
#define is_zero_ether_addr           rte_is_zero_ether_addr
#define is_broadcast_ether_addr      rte_is_broadcast_ether_addr
#define is_valid_assigned_ether_addr rte_is_valid_assigned_ether_addr
#endif

/******************************************************************************
 * misc definitions
 ******************************************************************************/
#if RTE_VERSION < RTE_VERSION_NUM(16, 7, 0, 0)
#define RTE_PTYPE_L2_ETHER_NSH              0x00000005
#endif
#if RTE_VERSION < RTE_VERSION_NUM(17, 0, 0, 0)
#define RTE_PTYPE_L2_ETHER_VLAN             0x00000006
#define RTE_PTYPE_L2_ETHER_QINQ             0x00000007
#define RTE_PTYPE_INNER_L2_ETHER_QINQ       0x00030000
#endif
#if RTE_VERSION < RTE_VERSION_NUM(17, 11, 0, 0)
#define RTE_PTYPE_TUNNEL_GTPC               0x00007000
#define RTE_PTYPE_TUNNEL_GTPU               0x00008000
#define RTE_PTYPE_TUNNEL_ESP                0x00009000
#endif
#if RTE_VERSION < RTE_VERSION_NUM(18, 5, 0, 0)
#define RTE_PTYPE_L2_ETHER_PPPOE            0x00000008
#define RTE_PTYPE_TUNNEL_L2TP               0x0000a000
#define RTE_PTYPE_TUNNEL_MPLS_IN_GRE        0x0000c000
#define RTE_PTYPE_TUNNEL_MPLS_IN_UDP        0x0000d000
#endif

/******************************************************************************
 * local definitions
 ******************************************************************************/
#ifndef RTE_PTYPE_L2_ETHER_FCOE
#define RTE_PTYPE_L2_ETHER_FCOE             RTE_PTYPE_L2_ETHER
#endif
#ifndef RTE_PTYPE_L2_ETHER_FIP
#define RTE_PTYPE_L2_ETHER_FIP              RTE_PTYPE_L2_ETHER
#endif
#ifndef RTE_PTYPE_L2_ETHER_CNM
#define RTE_PTYPE_L2_ETHER_CNM              RTE_PTYPE_L2_ETHER
#endif
#ifndef RTE_PTYPE_L2_ETHER_EAPOL
#define RTE_PTYPE_L2_ETHER_EAPOL            RTE_PTYPE_L2_ETHER
#endif
#ifndef RTE_PTYPE_L2_ETHER_FILTER
#define RTE_PTYPE_L2_ETHER_FILTER           RTE_PTYPE_L2_ETHER
#endif
#ifndef RTE_PTYPE_L2_ETHER_FIP
#define RTE_PTYPE_L2_ETHER_FIP              RTE_PTYPE_L2_ETHER
#endif

#endif /* _RTE_PMD_COMPAT_H_ */
