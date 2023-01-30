#ifndef _RSP_DEVICE_H_
#define _RSP_DEVICE_H_

#include <rte_bus_pci.h>
#include "rsp_common.h"
#include "rsp_logs.h"
#include "rsp_qp.h"

#define RSP_VENDER          0x1dab
#define RSP_DEVICE_S10      0x7001
#define RSP_DEVICE_S20      0x7002
#define RSP_DEVICE_S10_VF   0x8002
#define RSP_DEVICE_S20_VF   0x8001


#define RSP_DETACHED  (0)
#define RSP_ATTACHED  (1)
#define RSP_MAX_PCI_DEVICES        48
#define RSP_DEV_NAME_MAX_LEN       64
#define RSP_DEV_NAME_SUFFIX        "_rsp"

#define SYM_ENQ_THRESHOLD_NAME      "rsp_sym_enq_threshold"
#define ASYM_ENQ_THRESHOLD_NAME     "rsp_asym_enq_threshold"
#define RSP_VF_NUM                  "vf_num"
#define RSP_PF_RPU_QUEUE            "pf_sym_queue"
#define RSP_VF_RPU_QUEUE            "vf_sym_queue"
#define RSP_PF_BANK                 "pf bank"
#define RSP_VF_BANK                 "vf bank"

#define MAX_QP_THRESHOLD_SIZE    32

struct rsp_dev_cmd_param {
    const char *name;
    uint16_t val;
};

struct rsp_sym_dev_private;
struct rsp_asym_dev_private;

struct rsp_pci_device {

    char name[RSP_DEV_NAME_MAX_LEN];
    /**< Name of rsp pci device */
    int model;
    uint8_t dev_id;
    int8_t vf_num;
    int8_t qp_pf;
    int8_t qp_vf;

    /**< Device instance for the rsp pci device 1\2\3...*/
    struct rte_pci_device *pci_dev;
    /**< PCI device information. */

    __extension__
    uint8_t attached : 1;
    /**< Flag indicating the device is attached */

    struct rsp_QP *rpu_qps_in_use[128];
    struct rsp_QP *bulk_qps_in_use[128];
    struct rsp_QP *pub_qps_in_use[128];

    struct rsp_sym_dev_private *sym_dev_priv;
    struct rte_device sym_rte_dev;

    struct rsp_asym_dev_private *asym_dev_priv;
    struct rte_device asym_rte_dev;

    struct rsp_bulk_dev_private *bulk_dev_priv;
    struct rte_device bulk_rte_dev;
};

struct rsp_pci_device *rsp_pci_device_setup(struct rte_pci_device *pci_dev,
        struct rsp_dev_cmd_param *rsp_dev_cmd_param);

int rsp_pci_device_release(struct rte_pci_device *pci_dev);

struct rsp_pci_device *rsp_get_rsp_dev_from_pci_dev(struct rte_pci_device *pci_dev);

/* declaration needed for weak functions */
int rsp_sym_dev_create(struct rsp_pci_device *rsp_pci_dev __rte_unused,
        struct rsp_dev_cmd_param *rsp_dev_cmd_param);

int rsp_asym_dev_create(struct rsp_pci_device *rsp_pci_dev __rte_unused,
        struct rsp_dev_cmd_param *rsp_dev_cmd_param);

int rsp_sym_dev_destroy(struct rsp_pci_device *rsp_pci_dev __rte_unused);

int rsp_asym_dev_destroy(struct rsp_pci_device *rsp_pci_dev __rte_unused);

#endif /* _RSP_DEVICE_H_ */
