/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef _TXGBE_LOGS_H_
#define _TXGBE_LOGS_H_

extern int txgbe_logtype_init;
#define PMD_INIT_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, txgbe_logtype_init, \
		"%s(): " fmt "\n", __func__, ##args)

extern int txgbe_logtype_driver;
#define PMD_DRV_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, txgbe_logtype_driver, \
		"%s(): " fmt "\n", __func__, ##args)

#define PMD_INIT_FUNC_TRACE() PMD_INIT_LOG(DEBUG, " >>")

#ifdef RTE_LIBRTE_TXGBE_DEBUG_RX
#define PMD_RX_LOG(level, fmt, args...) \
	RTE_LOG(level, PMD, "%s(): " fmt "\n", __func__, ##args)
#else
#define PMD_RX_LOG(level, fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_DEBUG_TX
#define PMD_TX_LOG(level, fmt, args...) \
	RTE_LOG(level, PMD, "%s(): " fmt "\n", __func__, ##args)
#else
#define PMD_TX_LOG(level, fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_DEBUG_TX_FREE
#define PMD_TX_FREE_LOG(level, fmt, args...) \
	RTE_LOG(level, PMD, "%s(): " fmt "\n", __func__, ##args)
#else
#define PMD_TX_FREE_LOG(level, fmt, args...) do { } while(0)
#endif

/* level log:
 * EMERG    1  System is unusable.
 * ALERT    2  Action must be taken immediately.
 * CRIT     3  Critical conditions.
 * ERR      4  Error conditions.
 * WARNING  5  Warning conditions.
 * NOTICE   6  Normal but significant condition.
 * INFO     7  Informational.
 * DEBUG    8  Debug-level messages.
 */
#ifdef RTE_LIBRTE_TXGBE_LOG
#define PMD_TLOG_INIT(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, txgbe_logtype_init, \
		"%s(): " fmt, __func__, ##args)
#define PMD_TLOG_DRIVER(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, txgbe_logtype_driver, \
		"%s(): " fmt, __func__, ##args)
#else
#define PMD_TLOG_INIT(level, fmt, args...)   do { } while (0)
#define PMD_TLOG_DRIVER(level, fmt, args...) do { } while (0)
#endif

#define TLOG_EMERG(fmt, args...)    PMD_TLOG_DRIVER(EMERG, fmt, ##args)
#define TLOG_ALERT(fmt, args...)    PMD_TLOG_DRIVER(ALERT, fmt, ##args)
#define TLOG_CRIT(fmt, args...)     PMD_TLOG_DRIVER(CRIT, fmt, ##args)
#define TLOG_ERR(fmt, args...)      PMD_TLOG_DRIVER(ERR, fmt, ##args)
#define TLOG_WARNING(fmt, args...)  PMD_TLOG_DRIVER(WARNING, fmt, ##args)
#define TLOG_NOTICE(fmt, args...)   PMD_TLOG_DRIVER(NOTICE, fmt, ##args)
#define TLOG_INFO(fmt, args...)     PMD_TLOG_DRIVER(INFO, fmt, ##args)
#define TLOG_DEBUG(fmt, args...)    PMD_TLOG_DRIVER(DEBUG, fmt, ##args)

/* debug log */
#ifdef RTE_LIBRTE_TXGBE_LOG_INIT
#define TLOG_INIT(fmt, args...) TLOG_DEBUG("[init]" fmt, ##args)
#else
#define TLOG_INIT(fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_LOG_DRV
#define TLOG_DRV(fmt, args...) TLOG_DEBUG("[drv]" fmt, ##args)
#else
#define TLOG_DRV(fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_LOG_RX
#define TLOG_RX(fmt, args...) TLOG_DEBUG("[rx]" fmt, ##args)
#else
#define TLOG_RX(fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_LOG_TX_FILL
#define TLOG_TX_FILL(fmt, args...) TLOG_DEBUG("[rxf]" fmt, ##args)
#else
#define TLOG_TX_FILL(fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_LOG_TX
#define TLOG_TX(fmt, args...) TLOG_DEBUG("[tx]" fmt, ##args)
#else
#define TLOG_TX(fmt, args...) do { } while(0)
#endif

#ifdef RTE_LIBRTE_TXGBE_LOG_TX_DONE
#define TLOG_TX_DONE(fmt, args...) TLOG_DEBUG("[txd]" fmt, ##args)
#else
#define TLOG_TX_DONE(fmt, args...) do { } while(0)
#endif


#define DEBUGOUT(fmt, args...)    PMD_TLOG_DRIVER(DEBUG, fmt, ##args)
#define DEBUGFUNC(fmt)            DEBUGOUT(fmt "\n");

#ifdef RTE_LIBRTE_TXGBE_DEBUG
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
	int j; char buf[128] = ""; \
	wjmsg("data_len=%d pkt_len=%d vlan_tci=%d " \
		"packet_type=0x%08x ol_flags=0x%016lx " \
		"hash.rss=0x%08x hash.fdir.hash=0x%04x hash.fdir.id=%d\n", \
		mb->data_len, mb->pkt_len, mb->vlan_tci, \
		mb->packet_type, mb->ol_flags, \
		mb->hash.rss, mb->hash.fdir.hash, mb->hash.fdir.id); \
	for (j = 0; j < mb->data_len; j++) { \
		sprintf(buf + strlen(buf), "%02x ", \
			((uint8_t*)(mb->buf_addr) + mb->data_off)[j]); \
		if (j % 16 == 15) {\
			wjmsg("%s\n", buf); \
			buf[0] = '\0'; \
		} \
	} \
	wjmsg("%s\n", buf); \
}
#else /* RTE_LIBRTE_TXGBE_DEBUG */
#define wjmsg_line(fmt, args...) do {} while (0)
#define wjmsg_limit(fmt, args...) do {} while (0)
#define wjmsg_stack(fmt, args...) do {} while (0)
#define wjmsg(fmt, args...) do {} while (0)
#define wjdump(fmt, args...) do {} while (0)
#endif /* RTE_LIBRTE_TXGBE_DEBUG */

#ifdef RTE_TXGBE_DEBUG_BP
#define BP_LOG(fmt, ...) \
    do { \
	RTE_LOG(CRIT, PMD, "[%lu.%lu]%s(%d): " fmt, \
		usec_stamp() / 1000000, usec_stamp() % 1000000, \
		__FUNCTION__, __LINE__, ## __VA_ARGS__); \
    } while (0)
#else
#define BP_LOG(fmt, ...) do {} while (0)
#endif

#endif /* _TXGBE_LOGS_H_ */
