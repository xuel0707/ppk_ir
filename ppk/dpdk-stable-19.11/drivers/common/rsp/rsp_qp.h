#ifndef _RSP_QP_H_
#define _RSP_QP_H_
#include <rte_atomic.h>
#include "rsp_common.h"
#include "rsp_reg.h"

#define RSP_DEFAULT_PF_RPU_QP_NUM           4
#define RSP_DEFAULT_PF_BANK_NUM             1

#define RSP_DEFAULT_VF_RPU_QP_NUM           4
#define RSP_DEFAULT_VF_BANK_NUM             1


#define RSP_RPU_MAX_NUM                     128
#define RSP_NPUB_MAX_BANK                   32
#define RSP_RPU_COKIES_SIZE         (16*1024)
#define RSP_BULK_COKIES_SIZE        (16*1024)
#define RSP_PUB_COKIES_SIZE         (16*1024)

#define RSP_RESP_MAGIC_CODE         0x7F7F7F7F

#define RSP_DMA_ALIGN_SIZE          4096
enum rsp_ring_type_e
{
    RSP_RING_TYPE_RPU        =1,
    RSP_RING_TYPE_BULK       =2,
    RSP_RING_TYPE_PUB        =3,
};


struct rsp_pci_device;

#define RSP_CSR_HEAD_WRITE_THRESH   32U

#define RSP_MAX_QPS_PER_SERVICE     16
#define RSP_QP_MIN_INFL_THRESHOLD   256

typedef int (*build_request_t)(void *op, uint8_t *req, void *op_cookie, void *param);
/**< Build a request from an op. */

/**
 * Structure with data needed for creation of queue pair.
 */
struct rsp_qp_config {
    uint32_t type;
    uint32_t nb_descriptors;
    uint32_t cookie_size;
    int socket_id;
    uint16_t queue_pair_id;
    uint16_t bank_id;
    uint16_t msg_size;
    uint16_t resp_size;
    
    build_request_t build_request;
    uint16_t min_enq_burst_threshold;
    const char *service_str;
};
struct rsp_op_cookie {
    uint32_t         count;
    uint32_t         size;
    void            *cookies;
};
/**
 * Structure associated with each queue.
 */
struct rsp_qp {
    char        memz_name_req[RTE_MEMZONE_NAMESIZE];
    char        memz_name_resp[RTE_MEMZONE_NAMESIZE];
    void        *req_virt;
    rte_iova_t    req_phy;
    void        *resp_virt;
    rte_iova_t    resp_phy;

    uint32_t    req_head;
    uint32_t    req_tail;
    uint32_t    resp_head;
    uint32_t    resp_tail;
    uint32_t    modulo_mask;
    uint32_t    msg_size;
    uint32_t    resp_size;
    uint32_t    queue_size;

    uint16_t    nb_processed_responses;
    /* number of responses processed since last CSR head write */

    struct rte_mempool   *cookies_pool;
    struct rsp_op_cookie op_cookie;
};

struct rsp_QP {
    void    *ptr_base;
    uint32_t ring_type;
    uint32_t bank_id;
    uint32_t ring_id;
    int model;
    struct rsp_qp  queue;
    build_request_t build_request;
    struct rsp_pci_device *rsp_dev;
    uint32_t enqueued;
    uint32_t dequeued __rte_aligned(4);
    uint32_t max_inflights;
    struct rsp_common_stats stats;
    uint16_t min_enq_burst_threshold;
} __rte_cache_aligned;


uint16_t rsp_enqueue_op_burst(void *qp, void **ops, uint16_t nb_ops);

uint16_t rsp_enqueue_comp_op_burst(void *qp, void **ops, uint16_t nb_ops);

uint16_t rsp_dequeue_op_burst(void *qp, void **ops, uint16_t nb_ops);

void rsp_qp_release(struct rsp_QP *QP);

int rsp_qp_setup(struct rsp_pci_device *rsp_dev, struct rsp_QP **qp_addr, struct rsp_qp_config *conf);
void rsp_disable_rpu(void *ptr_base, int model);
#endif /* _RSP_QP_H_ */
