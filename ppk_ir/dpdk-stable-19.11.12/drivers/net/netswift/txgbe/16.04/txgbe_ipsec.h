/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2017 Intel Corporation
 */

#ifndef TXGBE_IPSEC_H_
#define TXGBE_IPSEC_H_

#include <rte_security.h>

#define IPSRXMOD_VALID                                    0x00000001
#define IPSRXMOD_PROTO                                    0x00000004
#define IPSRXMOD_DECRYPT                                  0x00000008
#define IPSRXMOD_IPV6                                     0x00000010
//#define TXGBE_TXD_POPTS_IPSEC                          0x00000400
//#define TXGBE_TXD_TUCMD_IPSEC_TYPE_ESP                 0x00002000
//#define TXGBE_TXD_TUCMD_IPSEC_ENCRYPT_EN               0x00004000
//#define TXGBE_RXD_IPSEC_STATUS_SECP                    0x00020000
//#define TXGBE_RXD_IPSEC_ERROR_BIT_MASK                 0x18000000
//#define TXGBE_RXD_IPSEC_ERROR_INVALID_PROTOCOL         0x08000000
//#define TXGBE_RXD_IPSEC_ERROR_INVALID_LENGTH           0x10000000
//#define TXGBE_RXD_IPSEC_ERROR_AUTHENTICATION_FAILED    0x18000000

#define IPSEC_MAX_RX_IP_COUNT           128
#define IPSEC_MAX_SA_COUNT              1024

#define ESP_ICV_SIZE 16
#define ESP_TRAILER_SIZE 2

enum txgbe_operation {
	TXGBE_OP_AUTHENTICATED_ENCRYPTION,
	TXGBE_OP_AUTHENTICATED_DECRYPTION
};

enum txgbe_gcm_key {
	TXGBE_GCM_KEY_128,
	TXGBE_GCM_KEY_256
};

/**
 * Generic IP address structure
 * TODO: Find better location for this rte_net.h possibly.
 **/
struct ipaddr {
	enum ipaddr_type {
		IPv4,
		IPv6
	} type;
	/**< IP Address Type - IPv4/IPv6 */

	union {
		uint32_t ipv4;
		uint32_t ipv6[4];
	};
};

/** inline crypto crypto private session structure */
struct txgbe_crypto_session {
	enum txgbe_operation op;
	uint8_t *key;
	uint32_t salt;
	uint32_t sa_index;
	uint32_t spi;
	struct ipaddr src_ip;
	struct ipaddr dst_ip;
	struct rte_eth_dev *dev;
} __rte_cache_aligned;

struct txgbe_crypto_rx_ip_table {
	struct ipaddr ip;
	uint16_t ref_count;
};
struct txgbe_crypto_rx_sa_table {
	uint32_t spi;
	uint32_t ip_index;
	uint8_t  mode;
	uint8_t  used;
};

struct txgbe_crypto_tx_sa_table {
	uint32_t spi;
	uint8_t  used;
};

union txgbe_crypto_tx_desc_md {
	uint64_t data;
	struct {
		/**< SA table index */
		uint32_t sa_idx;
		/**< ICV and ESP trailer length */
		uint8_t pad_len;
		/**< enable encryption */
		uint8_t enc;
	};
};

struct txgbe_ipsec {
	struct txgbe_crypto_rx_ip_table rx_ip_tbl[IPSEC_MAX_RX_IP_COUNT];
	struct txgbe_crypto_rx_sa_table rx_sa_tbl[IPSEC_MAX_SA_COUNT];
	struct txgbe_crypto_tx_sa_table tx_sa_tbl[IPSEC_MAX_SA_COUNT];
};


int txgbe_ipsec_ctx_create(struct rte_eth_dev *dev);
int txgbe_crypto_enable_ipsec(struct rte_eth_dev *dev);
int txgbe_crypto_add_ingress_sa_from_flow(const void *sess,
					  const void *ip_spec,
					  uint8_t is_ipv6);



#endif /*TXGBE_IPSEC_H_*/
