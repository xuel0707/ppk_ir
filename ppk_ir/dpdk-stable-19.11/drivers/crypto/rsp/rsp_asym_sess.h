#ifndef _RSP_ASYM_SESS_H_
#define _RSP_ASYM_SESS_H_

#include <stdarg.h>
#include <rte_cryptodev_pmd.h>
#include <rte_crypto_asym.h>
#include "rsp_common.h"

struct rsp_asym_session {
    struct {
        uint8_t     *data;
        uint32_t    length;
    }n;
    struct {
        uint8_t     *data;
        uint32_t    length;
    }e;
    struct {
        uint8_t     *data;
        uint32_t    length;
    }d;

    struct rte_crypto_asym_xform *xform;
};


int rsp_asym_session_configure(struct rte_cryptodev *dev,
        struct rte_crypto_asym_xform *xform,
        struct rte_cryptodev_asym_session *sess,
        struct rte_mempool *mempool);
unsigned int rsp_asym_session_get_private_size(
        struct rte_cryptodev *dev __rte_unused);
void rsp_asym_session_clear(struct rte_cryptodev *dev,
        struct rte_cryptodev_asym_session *sess);

#endif //_RSP_ASYM_SESS_H_