/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _NGBE_LOGS_H_
#define _NGBE_LOGS_H_

/*
 * PMD_USER_LOG: for user
 */
extern int ngbe_logtype_init;
#define PMD_INIT_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_init, \
		"%s(): " fmt "\n", __func__, ##args)

extern int ngbe_logtype_driver;
#define PMD_DRV_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_driver, \
		"%s(): " fmt "\n", __func__, ##args)

#ifdef RTE_LIBRTE_NGBE_DEBUG_RX
extern int ngbe_logtype_rx;
#define PMD_RX_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_rx,	\
		"%s(): " fmt "\n", __func__, ##args)
#else
#define PMD_RX_LOG(level, fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_NGBE_DEBUG_TX
extern int ngbe_logtype_tx;
#define PMD_TX_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_tx,	\
		"%s(): " fmt "\n", __func__, ##args)
#else
#define PMD_TX_LOG(level, fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_NGBE_DEBUG_TX_FREE
extern int ngbe_logtype_tx_free;
#define PMD_TX_FREE_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_tx_free,	\
		"%s(): " fmt "\n", __func__, ##args)
#else
#define PMD_TX_FREE_LOG(level, fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_NGBE_DEBUG_INIT
#define PMD_TLOG_INIT(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_init, \
		"%s(): " fmt, __func__, ##args)
#else
#define PMD_TLOG_INIT(level, fmt, args...)   do { } while (0)
#endif

#if 1
#define PMD_TLOG_DRIVER(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, ngbe_logtype_driver, \
		"%s(): " fmt, __func__, ##args)
#else
#define PMD_TLOG_DRIVER(level, fmt, args...) do { } while (0)
#endif

/*
 * PMD_DEBUG_LOG: for debugger
 */
#define TLOG_EMERG(fmt, args...)    PMD_TLOG_DRIVER(EMERG, fmt, ##args)
#define TLOG_ALERT(fmt, args...)    PMD_TLOG_DRIVER(ALERT, fmt, ##args)
#define TLOG_CRIT(fmt, args...)     PMD_TLOG_DRIVER(CRIT, fmt, ##args)
#define TLOG_ERR(fmt, args...)      PMD_TLOG_DRIVER(ERR, fmt, ##args)
#define TLOG_WARN(fmt, args...)     PMD_TLOG_DRIVER(WARNING, fmt, ##args)
#define TLOG_NOTICE(fmt, args...)   PMD_TLOG_DRIVER(NOTICE, fmt, ##args)
#define TLOG_INFO(fmt, args...)     PMD_TLOG_DRIVER(INFO, fmt, ##args)
#define TLOG_DEBUG(fmt, args...)    PMD_TLOG_DRIVER(DEBUG, fmt, ##args)

/* to be deleted */
#define DEBUGOUT(fmt, args...)    TLOG_DEBUG(fmt, ##args)
#define PMD_INIT_FUNC_TRACE()     TLOG_DEBUG(" >>")
#define DEBUGFUNC(fmt)            do { } while (0)

/*
 * PMD_TEMP_LOG: for tester
 */
#if 0
#define wjmsg_line(fmt, ...) \
    do { \
	RTE_LOG(CRIT, PMD, "%s(%d): " fmt, \
	       __FUNCTION__, __LINE__, ## __VA_ARGS__); \
    } while (0)
#define wjmsg_stack(fmt, ...) \
    do { \
	wjmsg_line(fmt, ## __VA_ARGS__); \
	rte_dump_stack(); \
    } while (0)
#define wjmsg wjmsg_line

#define wjdump(mb) { \
	int j; char buf[128] = "";\
	wjmsg("data_len=%d pkt_len=%d vlan_tci=%d " \
		"packet_type=0x%08x ol_flags=0x%016lx " \
		"hash.rss=0x%08x hash.fdir.hash=0x%04x hash.fdir.id=%d\n", \
		mb->data_len, mb->pkt_len, mb->vlan_tci, \
		mb->packet_type, mb->ol_flags, \
		mb->hash.rss, mb->hash.fdir.hash, mb->hash.fdir.id); \
	for (j = 0; j < mb->data_len; j++) { \
		sprintf(buf + strlen(buf), "%02x ", \
			((uint8_t*)(mb->buf_addr) + mb->data_off)[j]); \
		if (j % 8 == 7) {\
			wjmsg("%s\n", buf); \
			buf[0] = '\0'; \
		} \
	} \
	wjmsg("%s\n", buf); \
}
#else /* RTE_LIBRTE_NGBE_DEBUG */
#define wjmsg_line(fmt, args...) do {} while (0)
#define wjmsg_limit(fmt, args...) do {} while (0)
#define wjmsg_stack(fmt, args...) do {} while (0)
#define wjmsg(fmt, args...) do {} while (0)
#define wjdump(fmt, args...) do {} while (0)
#endif /* RTE_LIBRTE_NGBE_DEBUG */

#endif /* _NGBE_LOGS_H_ */
