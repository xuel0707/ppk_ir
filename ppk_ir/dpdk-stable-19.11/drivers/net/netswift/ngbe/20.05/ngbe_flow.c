/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2016 Intel Corporation
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
#include <rte_atomic.h>
#include <rte_branch_prediction.h>
#include <rte_memory.h>
#include <rte_eal.h>
#include <rte_alarm.h>
#include <rte_ether.h>
#include <rte_malloc.h>
#include <rte_random.h>
#include <rte_dev.h>
#include <rte_hash_crc.h>
#include <rte_flow.h>
#include <rte_flow_driver.h>

#include "ngbe_logs.h"
#include "base/ngbe.h"
#include "ngbe_ethdev.h"
#include "ngbe_rxtx.h"
#include "rte_pmd_ngbe.h"


#define NGBE_MIN_N_TUPLE_PRIO 1
#define NGBE_MAX_N_TUPLE_PRIO 7
#define NGBE_MAX_FLX_SOURCE_OFF 62

/* ntuple filter list structure */
struct ngbe_ntuple_filter_ele {
	TAILQ_ENTRY(ngbe_ntuple_filter_ele) entries;
	struct rte_eth_ntuple_filter filter_info;
};
/* ethertype filter list structure */
struct ngbe_ethertype_filter_ele {
	TAILQ_ENTRY(ngbe_ethertype_filter_ele) entries;
	struct rte_eth_ethertype_filter filter_info;
};
/* syn filter list structure */
struct ngbe_eth_syn_filter_ele {
	TAILQ_ENTRY(ngbe_eth_syn_filter_ele) entries;
	struct rte_eth_syn_filter filter_info;
};
/* l2_tunnel filter list structure */
struct ngbe_eth_l2_tunnel_conf_ele {
	TAILQ_ENTRY(ngbe_eth_l2_tunnel_conf_ele) entries;
	struct rte_eth_l2_tunnel_conf filter_info;
};
/* rss filter list structure */
struct ngbe_rss_conf_ele {
	TAILQ_ENTRY(ngbe_rss_conf_ele) entries;
	struct ngbe_rte_flow_rss_conf filter_info;
};
/* ngbe_flow memory list structure */
struct ngbe_flow_mem {
	TAILQ_ENTRY(ngbe_flow_mem) entries;
	struct rte_flow *flow;
};

TAILQ_HEAD(ngbe_ntuple_filter_list, ngbe_ntuple_filter_ele);
TAILQ_HEAD(ngbe_ethertype_filter_list, ngbe_ethertype_filter_ele);
TAILQ_HEAD(ngbe_syn_filter_list, ngbe_eth_syn_filter_ele);
TAILQ_HEAD(ngbe_l2_tunnel_filter_list, ngbe_eth_l2_tunnel_conf_ele);
TAILQ_HEAD(ngbe_rss_filter_list, ngbe_rss_conf_ele);
TAILQ_HEAD(ngbe_flow_mem_list, ngbe_flow_mem);

static struct ngbe_ntuple_filter_list filter_ntuple_list;
static struct ngbe_ethertype_filter_list filter_ethertype_list;
static struct ngbe_syn_filter_list filter_syn_list;
static struct ngbe_l2_tunnel_filter_list filter_l2_tunnel_list;
static struct ngbe_rss_filter_list filter_rss_list;
static struct ngbe_flow_mem_list ngbe_flow_list;

/**
 * Endless loop will never happen with below assumption
 * 1. there is at least one no-void item(END)
 * 2. cur is before END.
 */
static inline
const struct rte_flow_item *next_no_void_pattern(
		const struct rte_flow_item pattern[],
		const struct rte_flow_item *cur)
{
	const struct rte_flow_item *next =
		cur ? cur + 1 : &pattern[0];
	while (1) {
		if (next->type != RTE_FLOW_ITEM_TYPE_VOID)
			return next;
		next++;
	}
}

static inline
const struct rte_flow_action *next_no_void_action(
		const struct rte_flow_action actions[],
		const struct rte_flow_action *cur)
{
	const struct rte_flow_action *next =
		cur ? cur + 1 : &actions[0];
	while (1) {
		if (next->type != RTE_FLOW_ACTION_TYPE_VOID)
			return next;
		next++;
	}
}

/**
 * Please aware there's an asumption for all the parsers.
 * rte_flow_item is using big endian, rte_flow_attr and
 * rte_flow_action are using CPU order.
 * Because the pattern is used to describe the packets,
 * normally the packets should use network order.
 */

/**
 * Parse the rule to see if it is a n-tuple rule.
 * And get the n-tuple filter info BTW.
 * pattern:
 * The first not void item can be ETH or IPV4.
 * The second not void item must be IPV4 if the first one is ETH.
 * The third not void item must be UDP or TCP.
 * The next not void item must be END.
 * action:
 * The first not void action should be QUEUE.
 * The next not void action should be END.
 * pattern example:
 * ITEM		Spec			Mask
 * ETH		NULL			NULL
 * IPV4		src_addr 192.168.1.20	0xFFFFFFFF
 *		dst_addr 192.167.3.50	0xFFFFFFFF
 *		next_proto_id	17	0xFF
 * UDP/TCP/	src_port	80	0xFFFF
 * SCTP		dst_port	80	0xFFFF
 * END
 * other members in mask and spec should set to 0x00.
 * item->last should be NULL.
 *
 * Special case for flow action type RTE_FLOW_ACTION_TYPE_SECURITY.
 *
 */
static int
cons_parse_ntuple_filter(const struct rte_flow_attr *attr,
			 const struct rte_flow_item pattern[],
			 const struct rte_flow_action actions[],
			 struct rte_eth_ntuple_filter *filter,
			 struct rte_flow_error *error)
{
	const struct rte_flow_item *item;
	const struct rte_flow_action *act;
	const struct rte_flow_item_ipv4 *ipv4_spec;
	const struct rte_flow_item_ipv4 *ipv4_mask;
	const struct rte_flow_item_tcp *tcp_spec;
	const struct rte_flow_item_tcp *tcp_mask;
	const struct rte_flow_item_udp *udp_spec;
	const struct rte_flow_item_udp *udp_mask;
	const struct rte_flow_item_sctp *sctp_spec;
	const struct rte_flow_item_sctp *sctp_mask;
	const struct rte_flow_item_eth *eth_spec;
	const struct rte_flow_item_eth *eth_mask;
	const struct rte_flow_item_vlan *vlan_spec;
	const struct rte_flow_item_vlan *vlan_mask;
	struct rte_flow_item_eth eth_null;
	struct rte_flow_item_vlan vlan_null;

	if (!pattern) {
		rte_flow_error_set(error,
			EINVAL, RTE_FLOW_ERROR_TYPE_ITEM_NUM,
			NULL, "NULL pattern.");
		return -rte_errno;
	}

	if (!actions) {
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ACTION_NUM,
				   NULL, "NULL action.");
		return -rte_errno;
	}
	if (!attr) {
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR,
				   NULL, "NULL attribute.");
		return -rte_errno;
	}

	memset(&eth_null, 0, sizeof(struct rte_flow_item_eth));
	memset(&vlan_null, 0, sizeof(struct rte_flow_item_vlan));

#ifdef RTE_LIBRTE_SECURITY
	/**
	 *  Special case for flow action type RTE_FLOW_ACTION_TYPE_SECURITY
	 */
	act = next_no_void_action(actions, NULL);
	if (act->type == RTE_FLOW_ACTION_TYPE_SECURITY) {
		const void *conf = act->conf;
		/* check if the next not void item is END */
		act = next_no_void_action(actions, act);
		if (act->type != RTE_FLOW_ACTION_TYPE_END) {
			memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act, "Not supported action.");
			return -rte_errno;
		}

		/* get the IP pattern*/
		item = next_no_void_pattern(pattern, NULL);
		while (item->type != RTE_FLOW_ITEM_TYPE_IPV4 &&
				item->type != RTE_FLOW_ITEM_TYPE_IPV6) {
			if (item->last ||
					item->type == RTE_FLOW_ITEM_TYPE_END) {
				rte_flow_error_set(error, EINVAL,
					RTE_FLOW_ERROR_TYPE_ITEM,
					item, "IP pattern missing.");
				return -rte_errno;
			}
			item = next_no_void_pattern(pattern, item);
		}

		filter->proto = IPPROTO_ESP;
		return ngbe_crypto_add_ingress_sa_from_flow(conf, item->spec,
					item->type == RTE_FLOW_ITEM_TYPE_IPV6);
	}
#endif

	/* the first not void item can be MAC or IPv4 */
	item = next_no_void_pattern(pattern, NULL);

	if (item->type != RTE_FLOW_ITEM_TYPE_ETH &&
	    item->type != RTE_FLOW_ITEM_TYPE_IPV4) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by ntuple filter");
		return -rte_errno;
	}
	/* Skip Ethernet */
	if (item->type == RTE_FLOW_ITEM_TYPE_ETH) {
		eth_spec = item->spec;
		eth_mask = item->mask;
		/*Not supported last point for range*/
		if (item->last) {
			rte_flow_error_set(error,
			  EINVAL,
			  RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			  item, "Not supported last point for range");
			return -rte_errno;

		}
		/* if the first item is MAC, the content should be NULL */
		if ((item->spec || item->mask) &&
			(memcmp(eth_spec, &eth_null,
				sizeof(struct rte_flow_item_eth)) ||
			 memcmp(eth_mask, &eth_null,
				sizeof(struct rte_flow_item_eth)))) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}
		/* check if the next not void item is IPv4 or Vlan */
		item = next_no_void_pattern(pattern, item);
		if (item->type != RTE_FLOW_ITEM_TYPE_IPV4 &&
			item->type != RTE_FLOW_ITEM_TYPE_VLAN) {
			rte_flow_error_set(error,
			  EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
			  item, "Not supported by ntuple filter");
			  return -rte_errno;
		}
	}

	if (item->type == RTE_FLOW_ITEM_TYPE_VLAN) {
		vlan_spec = item->spec;
		vlan_mask = item->mask;
		/*Not supported last point for range*/
		if (item->last) {
			rte_flow_error_set(error,
			  EINVAL,
			  RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			  item, "Not supported last point for range");
			return -rte_errno;
		}
		/* the content should be NULL */
		if ((item->spec || item->mask) &&
			(memcmp(vlan_spec, &vlan_null,
				sizeof(struct rte_flow_item_vlan)) ||
			 memcmp(vlan_mask, &vlan_null,
				sizeof(struct rte_flow_item_vlan)))) {

			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}
		/* check if the next not void item is IPv4 */
		item = next_no_void_pattern(pattern, item);
		if (item->type != RTE_FLOW_ITEM_TYPE_IPV4) {
			rte_flow_error_set(error,
			  EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
			  item, "Not supported by ntuple filter");
			return -rte_errno;
		}
	}

	if (item->mask) {
		/* get the IPv4 info */
		if (!item->spec || !item->mask) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Invalid ntuple mask");
			return -rte_errno;
		}
		/*Not supported last point for range*/
		if (item->last) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
				item, "Not supported last point for range");
			return -rte_errno;
		}

		ipv4_mask = item->mask;
		/**
		 * Only support src & dst addresses, protocol,
		 * others should be masked.
		 */
		if (ipv4_mask->hdr.version_ihl ||
		    ipv4_mask->hdr.type_of_service ||
		    ipv4_mask->hdr.total_length ||
		    ipv4_mask->hdr.packet_id ||
		    ipv4_mask->hdr.fragment_offset ||
		    ipv4_mask->hdr.time_to_live ||
		    ipv4_mask->hdr.hdr_checksum) {
			rte_flow_error_set(error,
				EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}
		if ((ipv4_mask->hdr.src_addr != 0 &&
			ipv4_mask->hdr.src_addr != UINT32_MAX) ||
			(ipv4_mask->hdr.dst_addr != 0 &&
			ipv4_mask->hdr.dst_addr != UINT32_MAX) ||
			(ipv4_mask->hdr.next_proto_id != UINT8_MAX &&
			ipv4_mask->hdr.next_proto_id != 0)) {
			rte_flow_error_set(error,
				EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}

		filter->dst_ip_mask = ipv4_mask->hdr.dst_addr;
		filter->src_ip_mask = ipv4_mask->hdr.src_addr;
		filter->proto_mask  = ipv4_mask->hdr.next_proto_id;

		ipv4_spec = item->spec;
		filter->dst_ip = ipv4_spec->hdr.dst_addr;
		filter->src_ip = ipv4_spec->hdr.src_addr;
		filter->proto  = ipv4_spec->hdr.next_proto_id;
	}

	/* check if the next not void item is TCP or UDP */
	item = next_no_void_pattern(pattern, item);
	if (item->type != RTE_FLOW_ITEM_TYPE_TCP &&
	    item->type != RTE_FLOW_ITEM_TYPE_UDP &&
	    item->type != RTE_FLOW_ITEM_TYPE_SCTP &&
	    item->type != RTE_FLOW_ITEM_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by ntuple filter");
		return -rte_errno;
	}

	if ((item->type != RTE_FLOW_ITEM_TYPE_END) &&
		(!item->spec && !item->mask)) {
		goto action;
	}

	/* get the TCP/UDP/SCTP info */
	if (item->type != RTE_FLOW_ITEM_TYPE_END &&
		(!item->spec || !item->mask)) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Invalid ntuple mask");
		return -rte_errno;
	}

	/*Not supported last point for range*/
	if (item->last) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			item, "Not supported last point for range");
		return -rte_errno;

	}

	if (item->type == RTE_FLOW_ITEM_TYPE_TCP) {
		tcp_mask = item->mask;

		/**
		 * Only support src & dst ports, tcp flags,
		 * others should be masked.
		 */
		if (tcp_mask->hdr.sent_seq ||
		    tcp_mask->hdr.recv_ack ||
		    tcp_mask->hdr.data_off ||
		    tcp_mask->hdr.rx_win ||
		    tcp_mask->hdr.cksum ||
		    tcp_mask->hdr.tcp_urp) {
			memset(filter, 0,
				sizeof(struct rte_eth_ntuple_filter));
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}
		if ((tcp_mask->hdr.src_port != 0 &&
			tcp_mask->hdr.src_port != UINT16_MAX) ||
			(tcp_mask->hdr.dst_port != 0 &&
			tcp_mask->hdr.dst_port != UINT16_MAX)) {
			rte_flow_error_set(error,
				EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}

		filter->dst_port_mask  = tcp_mask->hdr.dst_port;
		filter->src_port_mask  = tcp_mask->hdr.src_port;
		if (tcp_mask->hdr.tcp_flags == 0xFF) {
			filter->flags |= RTE_NTUPLE_FLAGS_TCP_FLAG;
		} else if (!tcp_mask->hdr.tcp_flags) {
			filter->flags &= ~RTE_NTUPLE_FLAGS_TCP_FLAG;
		} else {
			memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}

		tcp_spec = item->spec;
		filter->dst_port  = tcp_spec->hdr.dst_port;
		filter->src_port  = tcp_spec->hdr.src_port;
		filter->tcp_flags = tcp_spec->hdr.tcp_flags;
	} else if (item->type == RTE_FLOW_ITEM_TYPE_UDP) {
		udp_mask = item->mask;

		/**
		 * Only support src & dst ports,
		 * others should be masked.
		 */
		if (udp_mask->hdr.dgram_len ||
		    udp_mask->hdr.dgram_cksum) {
			memset(filter, 0,
				sizeof(struct rte_eth_ntuple_filter));
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}
		if ((udp_mask->hdr.src_port != 0 &&
			udp_mask->hdr.src_port != UINT16_MAX) ||
			(udp_mask->hdr.dst_port != 0 &&
			udp_mask->hdr.dst_port != UINT16_MAX)) {
			rte_flow_error_set(error,
				EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}

		filter->dst_port_mask = udp_mask->hdr.dst_port;
		filter->src_port_mask = udp_mask->hdr.src_port;

		udp_spec = item->spec;
		filter->dst_port = udp_spec->hdr.dst_port;
		filter->src_port = udp_spec->hdr.src_port;
	} else if (item->type == RTE_FLOW_ITEM_TYPE_SCTP) {
		sctp_mask = item->mask;

		/**
		 * Only support src & dst ports,
		 * others should be masked.
		 */
		if (sctp_mask->hdr.tag ||
		    sctp_mask->hdr.cksum) {
			memset(filter, 0,
				sizeof(struct rte_eth_ntuple_filter));
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ntuple filter");
			return -rte_errno;
		}

		filter->dst_port_mask = sctp_mask->hdr.dst_port;
		filter->src_port_mask = sctp_mask->hdr.src_port;

		sctp_spec = item->spec;
		filter->dst_port = sctp_spec->hdr.dst_port;
		filter->src_port = sctp_spec->hdr.src_port;
	} else {
		goto action;
	}

	/* check if the next not void item is END */
	item = next_no_void_pattern(pattern, item);
	if (item->type != RTE_FLOW_ITEM_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by ntuple filter");
		return -rte_errno;
	}

action:

	/**
	 * n-tuple only supports forwarding,
	 * check if the first not void action is QUEUE.
	 */
	act = next_no_void_action(actions, NULL);
	if (act->type != RTE_FLOW_ACTION_TYPE_QUEUE) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ACTION,
			item, "Not supported action.");
		return -rte_errno;
	}
	filter->queue =
		((const struct rte_flow_action_queue *)act->conf)->index;

	/* check if the next not void item is END */
	act = next_no_void_action(actions, act);
	if (act->type != RTE_FLOW_ACTION_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ACTION,
			act, "Not supported action.");
		return -rte_errno;
	}

	/* parse attr */
	/* must be input direction */
	if (!attr->ingress) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_INGRESS,
				   attr, "Only support ingress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->egress) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_EGRESS,
				   attr, "Not support egress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->transfer) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_TRANSFER,
				   attr, "No support for transfer.");
		return -rte_errno;
	}

	if (attr->priority > 0xFFFF) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_PRIORITY,
				   attr, "Error priority.");
		return -rte_errno;
	}
	filter->priority = (uint16_t)attr->priority;
	if (attr->priority < NGBE_MIN_N_TUPLE_PRIO ||
	    attr->priority > NGBE_MAX_N_TUPLE_PRIO)
	    filter->priority = 1;

	return 0;
}

/* a specific function for ngbe because the flags is specific */
static int
ngbe_parse_ntuple_filter(struct rte_eth_dev *dev,
			  const struct rte_flow_attr *attr,
			  const struct rte_flow_item pattern[],
			  const struct rte_flow_action actions[],
			  struct rte_eth_ntuple_filter *filter,
			  struct rte_flow_error *error)
{
	int ret;

	ret = cons_parse_ntuple_filter(attr, pattern, actions, filter, error);

	if (ret)
		return ret;

#ifdef RTE_LIBRTE_SECURITY
	/* ESP flow not really a flow*/
	if (filter->proto == IPPROTO_ESP)
		return 0;
#endif

	/* doesn't support tcp flags. */
	if (filter->flags & RTE_NTUPLE_FLAGS_TCP_FLAG) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ITEM,
				   NULL, "Not supported by ntuple filter");
		return -rte_errno;
	}

	/* doesn't support many priorities. */
	if (filter->priority < NGBE_MIN_N_TUPLE_PRIO ||
	    filter->priority > NGBE_MAX_N_TUPLE_PRIO) {
		memset(filter, 0, sizeof(struct rte_eth_ntuple_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			NULL, "Priority not supported by ntuple filter");
		return -rte_errno;
	}

	if (filter->queue >= dev->data->nb_rx_queues)
		return -rte_errno;

	/* fixed value for ngbe */
	filter->flags = RTE_5TUPLE_FLAGS;
	return 0;
}

/**
 * Parse the rule to see if it is a ethertype rule.
 * And get the ethertype filter info BTW.
 * pattern:
 * The first not void item can be ETH.
 * The next not void item must be END.
 * action:
 * The first not void action should be QUEUE.
 * The next not void action should be END.
 * pattern example:
 * ITEM		Spec			Mask
 * ETH		type	0x0807		0xFFFF
 * END
 * other members in mask and spec should set to 0x00.
 * item->last should be NULL.
 */
static int
cons_parse_ethertype_filter(const struct rte_flow_attr *attr,
			    const struct rte_flow_item *pattern,
			    const struct rte_flow_action *actions,
			    struct rte_eth_ethertype_filter *filter,
			    struct rte_flow_error *error)
{
	const struct rte_flow_item *item;
	const struct rte_flow_action *act;
	const struct rte_flow_item_eth *eth_spec;
	const struct rte_flow_item_eth *eth_mask;
	const struct rte_flow_action_queue *act_q;

	if (!pattern) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM_NUM,
				NULL, "NULL pattern.");
		return -rte_errno;
	}

	if (!actions) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION_NUM,
				NULL, "NULL action.");
		return -rte_errno;
	}

	if (!attr) {
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR,
				   NULL, "NULL attribute.");
		return -rte_errno;
	}

	item = next_no_void_pattern(pattern, NULL);
	/* The first non-void item should be MAC. */
	if (item->type != RTE_FLOW_ITEM_TYPE_ETH) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by ethertype filter");
		return -rte_errno;
	}

	/*Not supported last point for range*/
	if (item->last) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			item, "Not supported last point for range");
		return -rte_errno;
	}

	/* Get the MAC info. */
	if (!item->spec || !item->mask) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ethertype filter");
		return -rte_errno;
	}

	eth_spec = item->spec;
	eth_mask = item->mask;

	/* Mask bits of source MAC address must be full of 0.
	 * Mask bits of destination MAC address must be full
	 * of 1 or full of 0.
	 */
	if (!is_zero_ether_addr(&eth_mask->src) ||
	    (!is_zero_ether_addr(&eth_mask->dst) &&
	     !is_broadcast_ether_addr(&eth_mask->dst))) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Invalid ether address mask");
		return -rte_errno;
	}

	if ((eth_mask->type & UINT16_MAX) != UINT16_MAX) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Invalid ethertype mask");
		return -rte_errno;
	}

	/* If mask bits of destination MAC address
	 * are full of 1, set RTE_ETHTYPE_FLAGS_MAC.
	 */
	if (is_broadcast_ether_addr(&eth_mask->dst)) {
		filter->mac_addr = eth_spec->dst;
		filter->flags |= RTE_ETHTYPE_FLAGS_MAC;
	} else {
		filter->flags &= ~RTE_ETHTYPE_FLAGS_MAC;
	}
	filter->ether_type = rte_be_to_cpu_16(eth_spec->type);

	/* Check if the next non-void item is END. */
	item = next_no_void_pattern(pattern, item);
	if (item->type != RTE_FLOW_ITEM_TYPE_END) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by ethertype filter.");
		return -rte_errno;
	}

	/* Parse action */

	act = next_no_void_action(actions, NULL);
	if (act->type != RTE_FLOW_ACTION_TYPE_QUEUE &&
	    act->type != RTE_FLOW_ACTION_TYPE_DROP) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act, "Not supported action.");
		return -rte_errno;
	}

	if (act->type == RTE_FLOW_ACTION_TYPE_QUEUE) {
		act_q = (const struct rte_flow_action_queue *)act->conf;
		filter->queue = act_q->index;
	} else {
		filter->flags |= RTE_ETHTYPE_FLAGS_DROP;
	}

	/* Check if the next non-void item is END */
	act = next_no_void_action(actions, act);
	if (act->type != RTE_FLOW_ACTION_TYPE_END) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act, "Not supported action.");
		return -rte_errno;
	}

	/* Parse attr */
	/* Must be input direction */
	if (!attr->ingress) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ATTR_INGRESS,
				attr, "Only support ingress.");
		return -rte_errno;
	}

	/* Not supported */
	if (attr->egress) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ATTR_EGRESS,
				attr, "Not support egress.");
		return -rte_errno;
	}

	/* Not supported */
	if (attr->transfer) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ATTR_TRANSFER,
				attr, "No support for transfer.");
		return -rte_errno;
	}

	/* Not supported */
	if (attr->priority) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ATTR_PRIORITY,
				attr, "Not support priority.");
		return -rte_errno;
	}

	/* Not supported */
	if (attr->group) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ATTR_GROUP,
				attr, "Not support group.");
		return -rte_errno;
	}

	return 0;
}

static int
ngbe_parse_ethertype_filter(struct rte_eth_dev *dev,
				 const struct rte_flow_attr *attr,
			     const struct rte_flow_item pattern[],
			     const struct rte_flow_action actions[],
			     struct rte_eth_ethertype_filter *filter,
			     struct rte_flow_error *error)
{
	int ret;

	ret = cons_parse_ethertype_filter(attr, pattern,
					actions, filter, error);

	if (ret)
		return ret;

	/* doesn't support MAC address. */
	if (filter->flags & RTE_ETHTYPE_FLAGS_MAC) {
		memset(filter, 0, sizeof(struct rte_eth_ethertype_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			NULL, "Not supported by ethertype filter");
		return -rte_errno;
	}

	if (filter->queue >= dev->data->nb_rx_queues) {
		memset(filter, 0, sizeof(struct rte_eth_ethertype_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			NULL, "queue index much too big");
		return -rte_errno;
	}

	if (filter->ether_type == ETHER_TYPE_IPv4 ||
		filter->ether_type == ETHER_TYPE_IPv6) {
		memset(filter, 0, sizeof(struct rte_eth_ethertype_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			NULL, "IPv4/IPv6 not supported by ethertype filter");
		return -rte_errno;
	}

	if (filter->flags & RTE_ETHTYPE_FLAGS_MAC) {
		memset(filter, 0, sizeof(struct rte_eth_ethertype_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			NULL, "mac compare is unsupported");
		return -rte_errno;
	}

	if (filter->flags & RTE_ETHTYPE_FLAGS_DROP) {
		memset(filter, 0, sizeof(struct rte_eth_ethertype_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			NULL, "drop option is unsupported");
		return -rte_errno;
	}

	return 0;
}

/**
 * Parse the rule to see if it is a TCP SYN rule.
 * And get the TCP SYN filter info BTW.
 * pattern:
 * The first not void item must be ETH.
 * The second not void item must be IPV4 or IPV6.
 * The third not void item must be TCP.
 * The next not void item must be END.
 * action:
 * The first not void action should be QUEUE.
 * The next not void action should be END.
 * pattern example:
 * ITEM		Spec			Mask
 * ETH		NULL			NULL
 * IPV4/IPV6	NULL			NULL
 * TCP		tcp_flags	0x02	0xFF
 * END
 * other members in mask and spec should set to 0x00.
 * item->last should be NULL.
 */
static int
cons_parse_syn_filter(const struct rte_flow_attr *attr,
				const struct rte_flow_item pattern[],
				const struct rte_flow_action actions[],
				struct rte_eth_syn_filter *filter,
				struct rte_flow_error *error)
{
	const struct rte_flow_item *item;
	const struct rte_flow_action *act;
	const struct rte_flow_item_tcp *tcp_spec;
	const struct rte_flow_item_tcp *tcp_mask;
	const struct rte_flow_action_queue *act_q;

	if (!pattern) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM_NUM,
				NULL, "NULL pattern.");
		return -rte_errno;
	}

	if (!actions) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION_NUM,
				NULL, "NULL action.");
		return -rte_errno;
	}

	if (!attr) {
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR,
				   NULL, "NULL attribute.");
		return -rte_errno;
	}


	/* the first not void item should be MAC or IPv4 or IPv6 or TCP */
	item = next_no_void_pattern(pattern, NULL);
	if (item->type != RTE_FLOW_ITEM_TYPE_ETH &&
	    item->type != RTE_FLOW_ITEM_TYPE_IPV4 &&
	    item->type != RTE_FLOW_ITEM_TYPE_IPV6 &&
	    item->type != RTE_FLOW_ITEM_TYPE_TCP) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by syn filter");
		return -rte_errno;
	}
		/*Not supported last point for range*/
	if (item->last) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			item, "Not supported last point for range");
		return -rte_errno;
	}

	/* Skip Ethernet */
	if (item->type == RTE_FLOW_ITEM_TYPE_ETH) {
		/* if the item is MAC, the content should be NULL */
		if (item->spec || item->mask) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Invalid SYN address mask");
			return -rte_errno;
		}

		/* check if the next not void item is IPv4 or IPv6 */
		item = next_no_void_pattern(pattern, item);
		if (item->type != RTE_FLOW_ITEM_TYPE_IPV4 &&
		    item->type != RTE_FLOW_ITEM_TYPE_IPV6) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by syn filter");
			return -rte_errno;
		}
	}

	/* Skip IP */
	if (item->type == RTE_FLOW_ITEM_TYPE_IPV4 ||
	    item->type == RTE_FLOW_ITEM_TYPE_IPV6) {
		/* if the item is IP, the content should be NULL */
		if (item->spec || item->mask) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Invalid SYN mask");
			return -rte_errno;
		}

		/* check if the next not void item is TCP */
		item = next_no_void_pattern(pattern, item);
		if (item->type != RTE_FLOW_ITEM_TYPE_TCP) {
			rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by syn filter");
			return -rte_errno;
		}
	}

	/* Get the TCP info. Only support SYN. */
	if (!item->spec || !item->mask) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Invalid SYN mask");
		return -rte_errno;
	}
	/*Not supported last point for range*/
	if (item->last) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			item, "Not supported last point for range");
		return -rte_errno;
	}

	tcp_spec = item->spec;
	tcp_mask = item->mask;
	if (!(tcp_spec->hdr.tcp_flags & TCP_SYN_FLAG) ||
	    tcp_mask->hdr.src_port ||
	    tcp_mask->hdr.dst_port ||
	    tcp_mask->hdr.sent_seq ||
	    tcp_mask->hdr.recv_ack ||
	    tcp_mask->hdr.data_off ||
	    tcp_mask->hdr.tcp_flags != TCP_SYN_FLAG ||
	    tcp_mask->hdr.rx_win ||
	    tcp_mask->hdr.cksum ||
	    tcp_mask->hdr.tcp_urp) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by syn filter");
		return -rte_errno;
	}

	/* check if the next not void item is END */
	item = next_no_void_pattern(pattern, item);
	if (item->type != RTE_FLOW_ITEM_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ITEM,
				item, "Not supported by syn filter");
		return -rte_errno;
	}

	/* check if the first not void action is QUEUE. */
	act = next_no_void_action(actions, NULL);
	if (act->type != RTE_FLOW_ACTION_TYPE_QUEUE) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act, "Not supported action.");
		return -rte_errno;
	}

	act_q = (const struct rte_flow_action_queue *)act->conf;
	filter->queue = act_q->index;
	if (filter->queue >= NGBE_MAX_RX_QUEUE_NUM) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act, "Not supported action.");
		return -rte_errno;
	}

	/* check if the next not void item is END */
	act = next_no_void_action(actions, act);
	if (act->type != RTE_FLOW_ACTION_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act, "Not supported action.");
		return -rte_errno;
	}

	/* parse attr */
	/* must be input direction */
	if (!attr->ingress) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_INGRESS,
			attr, "Only support ingress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->egress) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_EGRESS,
			attr, "Not support egress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->transfer) {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_TRANSFER,
			attr, "No support for transfer.");
		return -rte_errno;
	}

	/* Support 2 priorities, the lowest or highest. */
	if (!attr->priority) {
		filter->hig_pri = 0;
	} else if (attr->priority == (uint32_t)~0U) {
		filter->hig_pri = 1;
	} else {
		memset(filter, 0, sizeof(struct rte_eth_syn_filter));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_PRIORITY,
			attr, "Not support priority.");
		return -rte_errno;
	}

	return 0;
}

static int
ngbe_parse_syn_filter(struct rte_eth_dev *dev,
				 const struct rte_flow_attr *attr,
			     const struct rte_flow_item pattern[],
			     const struct rte_flow_action actions[],
			     struct rte_eth_syn_filter *filter,
			     struct rte_flow_error *error)
{
	int ret;

	ret = cons_parse_syn_filter(attr, pattern,
					actions, filter, error);

	if (filter->queue >= dev->data->nb_rx_queues)
		return -rte_errno;

	if (ret)
		return ret;

	return 0;
}

/**
 * Parse the rule to see if it is a L2 tunnel rule.
 * And get the L2 tunnel filter info BTW.
 * Only support E-tag now.
 * pattern:
 * The first not void item can be E_TAG.
 * The next not void item must be END.
 * action:
 * The first not void action should be VF or PF.
 * The next not void action should be END.
 * pattern example:
 * ITEM		Spec			Mask
 * E_TAG	grp		0x1	0x3
		e_cid_base	0x309	0xFFF
 * END
 * other members in mask and spec should set to 0x00.
 * item->last should be NULL.
 */
static int
cons_parse_l2_tn_filter(struct rte_eth_dev *dev,
			const struct rte_flow_attr *attr,
			const struct rte_flow_item pattern[],
			const struct rte_flow_action actions[],
			struct rte_eth_l2_tunnel_conf *filter,
			struct rte_flow_error *error)
{
	const struct rte_flow_item *item;
	const struct rte_flow_item_e_tag *e_tag_spec;
	const struct rte_flow_item_e_tag *e_tag_mask;
	const struct rte_flow_action *act;
	const struct rte_flow_action_vf *act_vf;
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!pattern) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM_NUM,
			NULL, "NULL pattern.");
		return -rte_errno;
	}

	if (!actions) {
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ACTION_NUM,
				   NULL, "NULL action.");
		return -rte_errno;
	}

	if (!attr) {
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR,
				   NULL, "NULL attribute.");
		return -rte_errno;
	}

	/* The first not void item should be e-tag. */
	item = next_no_void_pattern(pattern, NULL);
	if (item->type != RTE_FLOW_ITEM_TYPE_E_TAG) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by L2 tunnel filter");
		return -rte_errno;
	}

	if (!item->spec || !item->mask) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL, RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by L2 tunnel filter");
		return -rte_errno;
	}

	/*Not supported last point for range*/
	if (item->last) {
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_UNSPECIFIED,
			item, "Not supported last point for range");
		return -rte_errno;
	}

	e_tag_spec = item->spec;
	e_tag_mask = item->mask;

	/* Only care about GRP and E cid base. */
	if (e_tag_mask->epcp_edei_in_ecid_b ||
	    e_tag_mask->in_ecid_e ||
	    e_tag_mask->ecid_e ||
	    e_tag_mask->rsvd_grp_ecid_b != rte_cpu_to_be_16(0x3FFF)) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by L2 tunnel filter");
		return -rte_errno;
	}

	filter->l2_tunnel_type = RTE_L2_TUNNEL_TYPE_E_TAG;
	/**
	 * grp and e_cid_base are bit fields and only use 14 bits.
	 * e-tag id is taken as little endian by HW.
	 */
	filter->tunnel_id = rte_be_to_cpu_16(e_tag_spec->rsvd_grp_ecid_b);

	/* check if the next not void item is END */
	item = next_no_void_pattern(pattern, item);
	if (item->type != RTE_FLOW_ITEM_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ITEM,
			item, "Not supported by L2 tunnel filter");
		return -rte_errno;
	}

	/* parse attr */
	/* must be input direction */
	if (!attr->ingress) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_INGRESS,
			attr, "Only support ingress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->egress) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_EGRESS,
			attr, "Not support egress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->transfer) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_TRANSFER,
			attr, "No support for transfer.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->priority) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ATTR_PRIORITY,
			attr, "Not support priority.");
		return -rte_errno;
	}

	/* check if the first not void action is VF or PF. */
	act = next_no_void_action(actions, NULL);
	if (act->type != RTE_FLOW_ACTION_TYPE_VF &&
			act->type != RTE_FLOW_ACTION_TYPE_PF) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ACTION,
			act, "Not supported action.");
		return -rte_errno;
	}

	if (act->type == RTE_FLOW_ACTION_TYPE_VF) {
		act_vf = (const struct rte_flow_action_vf *)act->conf;
		filter->pool = act_vf->id;
	} else {
		filter->pool = pci_dev->max_vfs;
	}

	/* check if the next not void item is END */
	act = next_no_void_action(actions, act);
	if (act->type != RTE_FLOW_ACTION_TYPE_END) {
		memset(filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ACTION,
			act, "Not supported action.");
		return -rte_errno;
	}

	return 0;
}

static int
ngbe_parse_l2_tn_filter(struct rte_eth_dev *dev,
			const struct rte_flow_attr *attr,
			const struct rte_flow_item pattern[],
			const struct rte_flow_action actions[],
			struct rte_eth_l2_tunnel_conf *l2_tn_filter,
			struct rte_flow_error *error)
{
	int ret = 0;

	ret = cons_parse_l2_tn_filter(dev, attr, pattern,
				actions, l2_tn_filter, error);

	memset(l2_tn_filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
	rte_flow_error_set(error, EINVAL,
		RTE_FLOW_ERROR_TYPE_ITEM,
		NULL, "Not supported by L2 tunnel filter");
	ret = -rte_errno;
	return ret;

//x550: vf_num = pci_dev->max_vfs;
//	if (l2_tn_filter->pool > vf_num)
//		return -rte_errno;
//	return ret;
}

/* search next no void pattern and skip fuzzy */
static inline
const struct rte_flow_item *next_no_fuzzy_pattern(
		const struct rte_flow_item pattern[],
		const struct rte_flow_item *cur)
{
	const struct rte_flow_item *next =
		next_no_void_pattern(pattern, cur);
	while (1) {
		if (next->type != RTE_FLOW_ITEM_TYPE_FUZZY)
			return next;
		next = next_no_void_pattern(pattern, next);
	}
}

static inline uint8_t signature_match(const struct rte_flow_item pattern[])
{
	const struct rte_flow_item_fuzzy *spec, *last, *mask;
	const struct rte_flow_item *item;
	uint32_t sh, lh, mh;
	int i = 0;

	while (1) {
		item = pattern + i;
		if (item->type == RTE_FLOW_ITEM_TYPE_END)
			break;

		if (item->type == RTE_FLOW_ITEM_TYPE_FUZZY) {
			spec = item->spec;
			last = item->last;
			mask = item->mask;

			if (!spec || !mask)
				return 0;

			sh = spec->thresh;

			if (!last)
				lh = sh;
			else
				lh = last->thresh;

			mh = mask->thresh;
			sh = sh & mh;
			lh = lh & mh;

			if (!sh || sh > lh)
				return 0;

			return 1;
		}

		i++;
	}

	return 0;
}

static int
ngbe_parse_rss_filter(struct rte_eth_dev *dev,
			const struct rte_flow_attr *attr,
			const struct rte_flow_action actions[],
			struct ngbe_rte_flow_rss_conf *rss_conf,
			struct rte_flow_error *error)
{
	const struct rte_flow_action *act;
	const struct rte_flow_action_rss *rss;
	uint16_t n;

	/**
	 * rss only supports forwarding,
	 * check if the first not void action is RSS.
	 */
	act = next_no_void_action(actions, NULL);
	if (act->type != RTE_FLOW_ACTION_TYPE_RSS) {
		memset(rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ACTION,
			act, "Not supported action.");
		return -rte_errno;
	}

	rss = (const struct rte_flow_action_rss *)act->conf;

	if (!rss || !rss->queue_num) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_ACTION,
				act,
			   "no valid queues");
		return -rte_errno;
	}

	for (n = 0; n < rss->queue_num; n++) {
		if (rss->queue[n] >= dev->data->nb_rx_queues) {
			rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ACTION,
				   act,
				   "queue id > max number of queues");
			return -rte_errno;
		}
	}

	if (rss->func != RTE_ETH_HASH_FUNCTION_DEFAULT)
		return rte_flow_error_set
			(error, ENOTSUP, RTE_FLOW_ERROR_TYPE_ACTION, act,
			 "non-default RSS hash functions are not supported");
	if (rss->level)
		return rte_flow_error_set
			(error, ENOTSUP, RTE_FLOW_ERROR_TYPE_ACTION, act,
			 "a nonzero RSS encapsulation level is not supported");
	if (rss->key_len && rss->key_len != RTE_DIM(rss_conf->key))
		return rte_flow_error_set
			(error, ENOTSUP, RTE_FLOW_ERROR_TYPE_ACTION, act,
			 "RSS hash key must be exactly 40 bytes");
	if (rss->queue_num > RTE_DIM(rss_conf->queue))
		return rte_flow_error_set
			(error, ENOTSUP, RTE_FLOW_ERROR_TYPE_ACTION, act,
			 "too many queues for RSS context");
	if (ngbe_rss_conf_init(rss_conf, rss))
		return rte_flow_error_set
			(error, EINVAL, RTE_FLOW_ERROR_TYPE_ACTION, act,
			 "RSS context initialization failure");

	/* check if the next not void item is END */
	act = next_no_void_action(actions, act);
	if (act->type != RTE_FLOW_ACTION_TYPE_END) {
		memset(rss_conf, 0, sizeof(struct rte_eth_rss_conf));
		rte_flow_error_set(error, EINVAL,
			RTE_FLOW_ERROR_TYPE_ACTION,
			act, "Not supported action.");
		return -rte_errno;
	}

	/* parse attr */
	/* must be input direction */
	if (!attr->ingress) {
		memset(rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_INGRESS,
				   attr, "Only support ingress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->egress) {
		memset(rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_EGRESS,
				   attr, "Not support egress.");
		return -rte_errno;
	}

	/* not supported */
	if (attr->transfer) {
		memset(rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_TRANSFER,
				   attr, "No support for transfer.");
		return -rte_errno;
	}

	if (attr->priority > 0xFFFF) {
		memset(rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
		rte_flow_error_set(error, EINVAL,
				   RTE_FLOW_ERROR_TYPE_ATTR_PRIORITY,
				   attr, "Error priority.");
		return -rte_errno;
	}

	return 0;
}

/* remove the rss filter */
static void
ngbe_clear_rss_filter(struct rte_eth_dev *dev)
{
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(dev);

	if (filter_info->rss_info.conf.queue_num)
		ngbe_config_rss_filter(dev, &filter_info->rss_info, FALSE);
}

void
ngbe_filterlist_init(void)
{
	TAILQ_INIT(&filter_ntuple_list);
	TAILQ_INIT(&filter_ethertype_list);
	TAILQ_INIT(&filter_syn_list);
	TAILQ_INIT(&filter_l2_tunnel_list);
	TAILQ_INIT(&filter_rss_list);
	TAILQ_INIT(&ngbe_flow_list);
}

void
ngbe_filterlist_flush(void)
{
	struct ngbe_ntuple_filter_ele *ntuple_filter_ptr;
	struct ngbe_ethertype_filter_ele *ethertype_filter_ptr;
	struct ngbe_eth_syn_filter_ele *syn_filter_ptr;
	struct ngbe_eth_l2_tunnel_conf_ele *l2_tn_filter_ptr;
	struct ngbe_flow_mem *ngbe_flow_mem_ptr;
	struct ngbe_rss_conf_ele *rss_filter_ptr;

	while ((ntuple_filter_ptr = TAILQ_FIRST(&filter_ntuple_list))) {
		TAILQ_REMOVE(&filter_ntuple_list,
				 ntuple_filter_ptr,
				 entries);
		rte_free(ntuple_filter_ptr);
	}

	while ((ethertype_filter_ptr = TAILQ_FIRST(&filter_ethertype_list))) {
		TAILQ_REMOVE(&filter_ethertype_list,
				 ethertype_filter_ptr,
				 entries);
		rte_free(ethertype_filter_ptr);
	}

	while ((syn_filter_ptr = TAILQ_FIRST(&filter_syn_list))) {
		TAILQ_REMOVE(&filter_syn_list,
				 syn_filter_ptr,
				 entries);
		rte_free(syn_filter_ptr);
	}

	while ((l2_tn_filter_ptr = TAILQ_FIRST(&filter_l2_tunnel_list))) {
		TAILQ_REMOVE(&filter_l2_tunnel_list,
				 l2_tn_filter_ptr,
				 entries);
		rte_free(l2_tn_filter_ptr);
	}

	while ((rss_filter_ptr = TAILQ_FIRST(&filter_rss_list))) {
		TAILQ_REMOVE(&filter_rss_list,
				 rss_filter_ptr,
				 entries);
		rte_free(rss_filter_ptr);
	}

	while ((ngbe_flow_mem_ptr = TAILQ_FIRST(&ngbe_flow_list))) {
		TAILQ_REMOVE(&ngbe_flow_list,
				 ngbe_flow_mem_ptr,
				 entries);
		rte_free(ngbe_flow_mem_ptr->flow);
		rte_free(ngbe_flow_mem_ptr);
	}
}

/**
 * Create or destroy a flow rule.
 * Theorically one rule can match more than one filters.
 * We will let it use the filter which it hitt first.
 * So, the sequence matters.
 */
static struct rte_flow *
ngbe_flow_create(struct rte_eth_dev *dev,
		  const struct rte_flow_attr *attr,
		  const struct rte_flow_item pattern[],
		  const struct rte_flow_action actions[],
		  struct rte_flow_error *error)
{
	int ret;
	struct rte_eth_ntuple_filter ntuple_filter;
	struct rte_eth_ethertype_filter ethertype_filter;
	struct rte_eth_syn_filter syn_filter;
	struct rte_eth_l2_tunnel_conf l2_tn_filter;
	struct ngbe_rte_flow_rss_conf rss_conf;
	struct rte_flow *flow = NULL;
	struct ngbe_ntuple_filter_ele *ntuple_filter_ptr;
	struct ngbe_ethertype_filter_ele *ethertype_filter_ptr;
	struct ngbe_eth_syn_filter_ele *syn_filter_ptr;
	struct ngbe_eth_l2_tunnel_conf_ele *l2_tn_filter_ptr;
	struct ngbe_rss_conf_ele *rss_filter_ptr;
	struct ngbe_flow_mem *ngbe_flow_mem_ptr;

	flow = rte_zmalloc("ngbe_rte_flow", sizeof(struct rte_flow), 0);
	if (!flow) {
		PMD_DRV_LOG(ERR, "failed to allocate memory");
		return (struct rte_flow *)flow;
	}
	ngbe_flow_mem_ptr = rte_zmalloc("ngbe_flow_mem",
			sizeof(struct ngbe_flow_mem), 0);
	if (!ngbe_flow_mem_ptr) {
		PMD_DRV_LOG(ERR, "failed to allocate memory");
		rte_free(flow);
		return NULL;
	}
	ngbe_flow_mem_ptr->flow = flow;
	TAILQ_INSERT_TAIL(&ngbe_flow_list,
				ngbe_flow_mem_ptr, entries);

	memset(&ntuple_filter, 0, sizeof(struct rte_eth_ntuple_filter));
	ret = ngbe_parse_ntuple_filter(dev, attr, pattern,
			actions, &ntuple_filter, error);

#ifdef RTE_LIBRTE_SECURITY
	/* ESP flow not really a flow*/
	if (ntuple_filter.proto == IPPROTO_ESP)
		return flow;
#endif

	if (!ret) {
		ret = ngbe_add_del_ntuple_filter(dev, &ntuple_filter, TRUE);
		if (!ret) {
			ntuple_filter_ptr = rte_zmalloc("ngbe_ntuple_filter",
				sizeof(struct ngbe_ntuple_filter_ele), 0);
			if (!ntuple_filter_ptr) {
				PMD_DRV_LOG(ERR, "failed to allocate memory");
				goto out;
			}
			rte_memcpy(&ntuple_filter_ptr->filter_info,
				&ntuple_filter,
				sizeof(struct rte_eth_ntuple_filter));
			TAILQ_INSERT_TAIL(&filter_ntuple_list,
				ntuple_filter_ptr, entries);
			flow->rule = ntuple_filter_ptr;
			flow->filter_type = RTE_ETH_FILTER_NTUPLE;
			return flow;
		}
		goto out;
	}

	memset(&ethertype_filter, 0, sizeof(struct rte_eth_ethertype_filter));
	ret = ngbe_parse_ethertype_filter(dev, attr, pattern,
				actions, &ethertype_filter, error);
	if (!ret) {
		ret = ngbe_add_del_ethertype_filter(dev,
				&ethertype_filter, TRUE);
		if (!ret) {
			ethertype_filter_ptr = rte_zmalloc(
				"ngbe_ethertype_filter",
				sizeof(struct ngbe_ethertype_filter_ele), 0);
			if (!ethertype_filter_ptr) {
				PMD_DRV_LOG(ERR, "failed to allocate memory");
				goto out;
			}
			rte_memcpy(&ethertype_filter_ptr->filter_info,
				&ethertype_filter,
				sizeof(struct rte_eth_ethertype_filter));
			TAILQ_INSERT_TAIL(&filter_ethertype_list,
				ethertype_filter_ptr, entries);
			flow->rule = ethertype_filter_ptr;
			flow->filter_type = RTE_ETH_FILTER_ETHERTYPE;
			return flow;
		}
		goto out;
	}

	memset(&syn_filter, 0, sizeof(struct rte_eth_syn_filter));
	ret = ngbe_parse_syn_filter(dev, attr, pattern,
				actions, &syn_filter, error);
	if (!ret) {
		ret = ngbe_syn_filter_set(dev, &syn_filter, TRUE);
		if (!ret) {
			syn_filter_ptr = rte_zmalloc("ngbe_syn_filter",
				sizeof(struct ngbe_eth_syn_filter_ele), 0);
			if (!syn_filter_ptr) {
				PMD_DRV_LOG(ERR, "failed to allocate memory");
				goto out;
			}
			rte_memcpy(&syn_filter_ptr->filter_info,
				&syn_filter,
				sizeof(struct rte_eth_syn_filter));
			TAILQ_INSERT_TAIL(&filter_syn_list,
				syn_filter_ptr,
				entries);
			flow->rule = syn_filter_ptr;
			flow->filter_type = RTE_ETH_FILTER_SYN;
			return flow;
		}
		goto out;
	}

	memset(&l2_tn_filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
	ret = ngbe_parse_l2_tn_filter(dev, attr, pattern,
					actions, &l2_tn_filter, error);
	if (!ret) {
		ret = ngbe_dev_l2_tunnel_filter_add(dev, &l2_tn_filter, FALSE);
		if (!ret) {
			l2_tn_filter_ptr = rte_zmalloc("ngbe_l2_tn_filter",
				sizeof(struct ngbe_eth_l2_tunnel_conf_ele), 0);
			if (!l2_tn_filter_ptr) {
				PMD_DRV_LOG(ERR, "failed to allocate memory");
				goto out;
			}
			rte_memcpy(&l2_tn_filter_ptr->filter_info,
				&l2_tn_filter,
				sizeof(struct rte_eth_l2_tunnel_conf));
			TAILQ_INSERT_TAIL(&filter_l2_tunnel_list,
				l2_tn_filter_ptr, entries);
			flow->rule = l2_tn_filter_ptr;
			flow->filter_type = RTE_ETH_FILTER_L2_TUNNEL;
			return flow;
		}
	}

	memset(&rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
	ret = ngbe_parse_rss_filter(dev, attr,
					actions, &rss_conf, error);
	if (!ret) {
		ret = ngbe_config_rss_filter(dev, &rss_conf, TRUE);
		if (!ret) {
			rss_filter_ptr = rte_zmalloc("ngbe_rss_filter",
				sizeof(struct ngbe_rss_conf_ele), 0);
			if (!rss_filter_ptr) {
				PMD_DRV_LOG(ERR, "failed to allocate memory");
				goto out;
			}
			ngbe_rss_conf_init(&rss_filter_ptr->filter_info,
					    &rss_conf.conf);
			TAILQ_INSERT_TAIL(&filter_rss_list,
				rss_filter_ptr, entries);
			flow->rule = rss_filter_ptr;
			flow->filter_type = RTE_ETH_FILTER_HASH;
			return flow;
		}
	}

out:
	TAILQ_REMOVE(&ngbe_flow_list,
		ngbe_flow_mem_ptr, entries);
	rte_flow_error_set(error, -ret,
			   RTE_FLOW_ERROR_TYPE_HANDLE, NULL,
			   "Failed to create flow.");
	rte_free(ngbe_flow_mem_ptr);
	rte_free(flow);
	return NULL;
}

/**
 * Check if the flow rule is supported by ngbe.
 * It only checkes the format. Don't guarantee the rule can be programmed into
 * the HW. Because there can be no enough room for the rule.
 */
static int
ngbe_flow_validate(struct rte_eth_dev *dev,
		const struct rte_flow_attr *attr,
		const struct rte_flow_item pattern[],
		const struct rte_flow_action actions[],
		struct rte_flow_error *error)
{
	struct rte_eth_ntuple_filter ntuple_filter;
	struct rte_eth_ethertype_filter ethertype_filter;
	struct rte_eth_syn_filter syn_filter;
	struct rte_eth_l2_tunnel_conf l2_tn_filter;
	struct ngbe_rte_flow_rss_conf rss_conf;
	int ret;

	memset(&ntuple_filter, 0, sizeof(struct rte_eth_ntuple_filter));
	ret = ngbe_parse_ntuple_filter(dev, attr, pattern,
				actions, &ntuple_filter, error);
	if (!ret)
		return 0;

	memset(&ethertype_filter, 0, sizeof(struct rte_eth_ethertype_filter));
	ret = ngbe_parse_ethertype_filter(dev, attr, pattern,
				actions, &ethertype_filter, error);
	if (!ret)
		return 0;

	memset(&syn_filter, 0, sizeof(struct rte_eth_syn_filter));
	ret = ngbe_parse_syn_filter(dev, attr, pattern,
				actions, &syn_filter, error);
	if (!ret)
		return 0;
	memset(&l2_tn_filter, 0, sizeof(struct rte_eth_l2_tunnel_conf));
	ret = ngbe_parse_l2_tn_filter(dev, attr, pattern,
				actions, &l2_tn_filter, error);
	if (!ret)
		return 0;

	memset(&rss_conf, 0, sizeof(struct ngbe_rte_flow_rss_conf));
	ret = ngbe_parse_rss_filter(dev, attr,
					actions, &rss_conf, error);

	return ret;
}

/* Destroy a flow rule on ngbe. */
static int
ngbe_flow_destroy(struct rte_eth_dev *dev,
		struct rte_flow *flow,
		struct rte_flow_error *error)
{
	int ret;
	struct rte_flow *pmd_flow = flow;
	enum rte_filter_type filter_type = pmd_flow->filter_type;
	struct rte_eth_ntuple_filter ntuple_filter;
	struct rte_eth_ethertype_filter ethertype_filter;
	struct rte_eth_syn_filter syn_filter;
	struct rte_eth_l2_tunnel_conf l2_tn_filter;
	struct ngbe_ntuple_filter_ele *ntuple_filter_ptr;
	struct ngbe_ethertype_filter_ele *ethertype_filter_ptr;
	struct ngbe_eth_syn_filter_ele *syn_filter_ptr;
	struct ngbe_eth_l2_tunnel_conf_ele *l2_tn_filter_ptr;
	struct ngbe_flow_mem *ngbe_flow_mem_ptr;
	struct ngbe_rss_conf_ele *rss_filter_ptr;

	switch (filter_type) {
	case RTE_ETH_FILTER_NTUPLE:
		ntuple_filter_ptr = (struct ngbe_ntuple_filter_ele *)
					pmd_flow->rule;
		rte_memcpy(&ntuple_filter,
			&ntuple_filter_ptr->filter_info,
			sizeof(struct rte_eth_ntuple_filter));
		ret = ngbe_add_del_ntuple_filter(dev, &ntuple_filter, FALSE);
		if (!ret) {
			TAILQ_REMOVE(&filter_ntuple_list,
			ntuple_filter_ptr, entries);
			rte_free(ntuple_filter_ptr);
		}
		break;
	case RTE_ETH_FILTER_ETHERTYPE:
		ethertype_filter_ptr = (struct ngbe_ethertype_filter_ele *)
					pmd_flow->rule;
		rte_memcpy(&ethertype_filter,
			&ethertype_filter_ptr->filter_info,
			sizeof(struct rte_eth_ethertype_filter));
		ret = ngbe_add_del_ethertype_filter(dev,
				&ethertype_filter, FALSE);
		if (!ret) {
			TAILQ_REMOVE(&filter_ethertype_list,
				ethertype_filter_ptr, entries);
			rte_free(ethertype_filter_ptr);
		}
		break;
	case RTE_ETH_FILTER_SYN:
		syn_filter_ptr = (struct ngbe_eth_syn_filter_ele *)
				pmd_flow->rule;
		rte_memcpy(&syn_filter,
			&syn_filter_ptr->filter_info,
			sizeof(struct rte_eth_syn_filter));
		ret = ngbe_syn_filter_set(dev, &syn_filter, FALSE);
		if (!ret) {
			TAILQ_REMOVE(&filter_syn_list,
				syn_filter_ptr, entries);
			rte_free(syn_filter_ptr);
		}
		break;
	case RTE_ETH_FILTER_L2_TUNNEL:
		l2_tn_filter_ptr = (struct ngbe_eth_l2_tunnel_conf_ele *)
				pmd_flow->rule;
		rte_memcpy(&l2_tn_filter, &l2_tn_filter_ptr->filter_info,
			sizeof(struct rte_eth_l2_tunnel_conf));
		ret = ngbe_dev_l2_tunnel_filter_del(dev, &l2_tn_filter);
		if (!ret) {
			TAILQ_REMOVE(&filter_l2_tunnel_list,
				l2_tn_filter_ptr, entries);
			rte_free(l2_tn_filter_ptr);
		}
		break;
	case RTE_ETH_FILTER_HASH:
		rss_filter_ptr = (struct ngbe_rss_conf_ele *)
				pmd_flow->rule;
		ret = ngbe_config_rss_filter(dev,
					&rss_filter_ptr->filter_info, FALSE);
		if (!ret) {
			TAILQ_REMOVE(&filter_rss_list,
				rss_filter_ptr, entries);
			rte_free(rss_filter_ptr);
		}
		break;
	default:
		PMD_DRV_LOG(WARNING, "Filter type (%d) not supported",
			    filter_type);
		ret = -EINVAL;
		break;
	}

	if (ret) {
		rte_flow_error_set(error, EINVAL,
				RTE_FLOW_ERROR_TYPE_HANDLE,
				NULL, "Failed to destroy flow");
		return ret;
	}

	TAILQ_FOREACH(ngbe_flow_mem_ptr, &ngbe_flow_list, entries) {
		if (ngbe_flow_mem_ptr->flow == pmd_flow) {
			TAILQ_REMOVE(&ngbe_flow_list,
				ngbe_flow_mem_ptr, entries);
			rte_free(ngbe_flow_mem_ptr);
		}
	}
	rte_free(flow);

	return ret;
}

/*  Destroy all flow rules associated with a port on ngbe. */
static int
ngbe_flow_flush(struct rte_eth_dev *dev,
		struct rte_flow_error *error)
{
	int ret = 0;

	ngbe_clear_all_ntuple_filter(dev);
	ngbe_clear_all_ethertype_filter(dev);
	ngbe_clear_syn_filter(dev);

	ret = ngbe_clear_all_l2_tn_filter(dev);
	if (ret < 0) {
		rte_flow_error_set(error, EINVAL, RTE_FLOW_ERROR_TYPE_HANDLE,
					NULL, "Failed to flush rule");
		return ret;
	}

	ngbe_clear_rss_filter(dev);

	ngbe_filterlist_flush();

	return 0;
}

const struct rte_flow_ops ngbe_flow_ops = {
	.validate = ngbe_flow_validate,
	.create = ngbe_flow_create,
	.destroy = ngbe_flow_destroy,
	.flush = ngbe_flow_flush,
};
