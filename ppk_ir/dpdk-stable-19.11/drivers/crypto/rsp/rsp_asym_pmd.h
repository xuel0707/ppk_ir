#ifndef _RSP_ASYM_PMD_H_
#define _RSP_ASYM_PMD_H_
#include <rte_cryptodev.h>
#include "rsp_device.h"

#define CRYPTODEV_NAME_RSP_ASYM_PMD    crypto_rsp_asym
extern uint8_t rsp_asym_driver_id;
struct rsp_asym_dev_private {
    struct rsp_pci_device *rsp_dev;
    int model;
    uint8_t dev_id;

    const struct rte_cryptodev_capabilities *rsp_dev_capabilities;

    uint16_t min_enq_burst_threshold;

    int8_t qp_num;
    int8_t qp_ids[128];
    uint32_t vf_num;
    uint32_t rpu_queue;
    uint32_t npub_bank;
};

uint16_t rsp_asym_pmd_enqueue_op_burst(void *qp, struct rte_crypto_op **ops,
                  uint16_t nb_ops);

uint16_t rsp_asym_pmd_dequeue_op_burst(void *qp, struct rte_crypto_op **ops,
                  uint16_t nb_ops);

int rsp_asym_session_configure(struct rte_cryptodev *dev,
        struct rte_crypto_asym_xform *xform,
        struct rte_cryptodev_asym_session *sess,
        struct rte_mempool *mempool);

int rsp_asym_dev_create(struct rsp_pci_device *rsp_pci_dev, struct rsp_dev_cmd_param *rsp_dev_cmd_param);

int rsp_asym_dev_destroy(struct rsp_pci_device *rsp_pci_dev);

#endif /* _RSP_ASYM_PMD_H_ */
