#ifndef _RSP_SYM_PMD_H_
#define _RSP_SYM_PMD_H_

#ifdef BUILD_RSP_SYM

#include <rte_cryptodev.h>

#include "rsp_sym_capabilities.h"
#include "rsp_device.h"
#include "rsp_common.h"

#define CRYPTODEV_NAME_RSP_SYM_PMD    crypto_rsp_sym

extern uint8_t rsp_sym_driver_id;

struct rsp_sym_dev_private {
    struct rsp_pci_device *rsp_dev;
    int model;
    uint8_t dev_id;
    const struct rte_cryptodev_capabilities *rsp_dev_capabilities;
    uint16_t min_enq_burst_threshold;
    uint32_t internal_capabilities; /* see flags RSP_SYM_CAP_xxx */
    int8_t   qp_num;
    int8_t   qp_ids[128];
    uint32_t vf_num;
    uint32_t rpu_queue;
    uint32_t npub_bank;
};

int rsp_sym_dev_create(struct rsp_pci_device *rsp_pci_dev, struct rsp_dev_cmd_param *rsp_dev_cmd_param);
int rsp_sym_dev_destroy(struct rsp_pci_device *rsp_pci_dev);

#endif
#endif /* _RSP_SYM_PMD_H_ */
