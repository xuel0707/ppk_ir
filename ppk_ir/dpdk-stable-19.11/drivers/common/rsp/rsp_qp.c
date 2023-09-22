
#include <rte_common.h>
#include <rte_cycles.h>
#include <rte_dev.h>
#include <rte_malloc.h>
#include <rte_memzone.h>
#include <rte_pci.h>
#include <rte_bus_pci.h>
#include <rte_atomic.h>
#include <rte_prefetch.h>

#include "rsp_logs.h"
#include "rsp_device.h"
#include "rsp_qp.h"
#include "rsp_sym.h"
#include "rsp_asym.h"
#include "rsp_bulk.h"
#define RSP_CQ_MAX_DEQ_RETRIES 10

#define RSP_MAX_DESC                4096
#define RSP_MIN_DESC                2

static int rsp_qp_check_queue_alignment(uint64_t phys_addr, uint32_t queue_size_bytes);

static int rsp_queue_create(struct rsp_pci_device *rsp_dev, struct rsp_QP *QP, struct rsp_qp_config *conf);

static const struct rte_memzone *
queue_dma_zone_reserve(const char *queue_name, uint32_t queue_size, int socket_id)
{
    const struct rte_memzone *mz;
    mz = rte_memzone_lookup(queue_name);
    if (mz != 0) 
    {
        if (((size_t)queue_size <= mz->len) && //current mz name already reserve , Check if can be re-use or not
            ((socket_id == SOCKET_ID_ANY) ||
             (socket_id == mz->socket_id))) 
        {
            RSP_LOG(DEBUG, "re-use memzone already "
                    "allocated for %s", queue_name);
            return mz;
        }

        RSP_LOG(ERR, "Incompatible memzone already "
                "allocated %s, size %u, socket %d. "
                "Requested size %u, socket %u",
                queue_name, (uint32_t)mz->len,
                mz->socket_id, queue_size, socket_id);
        return NULL;
    }

    //RSP_LOG(DEBUG, "Allocate memzone for %s, size %u on socket %u",
    //                queue_name, queue_size, socket_id);
    //reserve dma zone for qp
    return rte_memzone_reserve_aligned(queue_name, queue_size, socket_id, RTE_MEMZONE_IOVA_CONTIG, RSP_DMA_ALIGN_SIZE);
}
static int rsp_qp_check_queue_alignment(uint64_t phys_addr, uint32_t queue_size_bytes)
{
    if (((queue_size_bytes - 1) & phys_addr) != 0)
        return -EINVAL;
    return 0;
}

int rsp_qp_setup(struct rsp_pci_device *rsp_dev, struct rsp_QP **QP_addr, struct rsp_qp_config *conf)
{
    struct rsp_QP *QP;
    struct rte_pci_device *pci_dev = rsp_dev->pci_dev;
    uint32_t i;

    RSP_LOG(DEBUG, "Setup qp %u on rsp pci device %d ", conf->queue_pair_id, rsp_dev->dev_id);
    if ((conf->nb_descriptors > RSP_MAX_DESC) || (conf->nb_descriptors < RSP_MIN_DESC))
    {
        RSP_LOG(ERR, "Can't create qp for %u descriptors", conf->nb_descriptors);
        return -EINVAL;
    }
    if (pci_dev->mem_resource[0].addr == NULL) 
    {
        RSP_LOG(ERR, "Could not find config space " "(UIO driver attached?).");
        return -EINVAL;
    }

    /* Allocate the queue pair data structure. */
    QP = rte_zmalloc_socket("rsp PMD QP metadata", sizeof(*QP), RTE_CACHE_LINE_SIZE, conf->socket_id);
    if (QP == NULL) 
    {
        RSP_LOG(ERR, "Failed to alloc mem for qp struct");
        return -ENOMEM;
    }

    QP->enqueued = QP->dequeued = 0;
    QP->min_enq_burst_threshold = conf->min_enq_burst_threshold;
    QP->max_inflights           = conf->nb_descriptors;
    QP->ptr_base                = pci_dev->mem_resource[0].addr;
    QP->build_request           = conf->build_request;
    QP->rsp_dev                 = rsp_dev;
    QP->ring_id                 = conf->queue_pair_id;
    QP->bank_id                 = conf->bank_id;
    QP->ring_type               = conf->type;
    QP->model                   = rsp_dev->model;
    
    if( (QP->ring_type == RSP_RING_TYPE_PUB) && (QP->model == RSP_MODEL_S10) )
    {
        RSP_LOG(ERR, "S10 not support pub now");
        goto create_err;
    }

    if (rsp_queue_create(rsp_dev, QP, conf) != 0) 
    {
        RSP_LOG(ERR, "queue create failed " "queue_pair_id=%u", conf->queue_pair_id);
        goto create_err;
    }

    *QP_addr = QP;
    return 0;
create_err:
    rte_free(QP);
    return -EFAULT;
}

void rsp_qp_release(struct rsp_QP *QP)
{
    struct rsp_op_cookie *op_cookie=NULL;
    struct rsp_asym_cookies *sym_cookie;
    struct rsp_asym_cookies *asym_cookie;
    const struct rte_memzone *mz;
    int status = 0;
    int i=0;
    if (QP == NULL) 
    {
        RSP_LOG(DEBUG, "Invalid QP");
        return;
    }
    RSP_LOG(DEBUG, "Free ring %d, memzone: %s-%s", QP->ring_id, 
                QP->queue.memz_name_req,QP->queue.memz_name_resp);

    mz = rte_memzone_lookup(QP->queue.memz_name_req);
    if (mz != NULL)    
    {
        /* Write an unused pattern to the queue memory. */
        memset(QP->queue.req_virt, 0x7F, QP->queue.queue_size*QP->queue.msg_size);
        status = rte_memzone_free(mz);
        if (status != 0)
            RSP_LOG(ERR, "Error %d on freeing queue %s", status, QP->queue.memz_name_req);
    } else 
    {
        RSP_LOG(DEBUG, "queue %s doesn't exist", QP->queue.memz_name_req);
    }

    mz = rte_memzone_lookup(QP->queue.memz_name_resp);
    if (mz != NULL)    
    {
        /* Write an unused pattern to the queue memory. */
        memset(QP->queue.resp_virt, 0x7F, QP->queue.queue_size*QP->queue.resp_size);
        status = rte_memzone_free(mz);
        if (status != 0)
            RSP_LOG(ERR, "Error %d on freeing queue %s", status, QP->queue.memz_name_resp);
    } else 
    {
        RSP_LOG(DEBUG, "queue %s doesn't exist", QP->queue.memz_name_resp);
    }
    if( QP->ring_type == RSP_RING_TYPE_PUB)
    {   //free cookies
        op_cookie = &QP->queue.op_cookie;
        asym_cookie = (struct rsp_asym_cookies *)op_cookie->cookies;
        for (i = 0; i < (int)(op_cookie->count); i++) 
        {
            if (asym_cookie[i].mz != NULL) 
            {
                rte_memzone_free(asym_cookie[i].mz);
            }
        }
        rte_free(QP->queue.op_cookie.cookies);
    }
    rte_free(QP);
}

static int rsp_queue_create(struct rsp_pci_device *rsp_dev, struct rsp_QP *QP, struct rsp_qp_config *conf)
{
    int i=0;
    int j=0;
    int ret=0;
    struct rsp_asym_item *cookie_temp;
    struct rsp_qp *queue;
    char op_cookie_pool_name[RTE_RING_NAMESIZE];
    char op_cookie_name[RTE_RING_NAMESIZE];
    uint64_t queue_base;
    void *io_addr;
    struct rsp_op_cookie *op_cookie=NULL;
    struct rsp_sym_cookies  *sym_cookie;
    struct rsp_asym_cookies *asym_cookie;
    struct rsp_bulk_cookies *bulk_cookie;
    const struct rte_memzone *qp_mz;
    uint32_t vf_num = rsp_dev->vf_num;
    struct rte_pci_device *pci_dev = rsp_dev->pci_dev;
    queue = &QP->queue;
    queue->queue_size   = conf->nb_descriptors;
    queue->req_head     = 0;
    queue->req_tail     = 0;
    queue->msg_size     = conf->msg_size;
    queue->resp_head    = 0;
    queue->resp_tail    = 0;
    queue->resp_size    = conf->resp_size;

    snprintf(queue->memz_name_req, sizeof(queue->memz_name_req),//must unique
            "%s_%d_%s_%s_%d",
            pci_dev->driver->driver.name, rsp_dev->dev_id,
            conf->service_str, "req",
            QP->ring_id);
    qp_mz = queue_dma_zone_reserve(queue->memz_name_req, 
                                queue->queue_size*queue->msg_size, 
                                rsp_dev->pci_dev->device.numa_node);
    if (qp_mz == NULL) 
    {
        RSP_LOG(ERR, "Failed to allocate ring memzone for request");
        return -ENOMEM;
    }
    queue->req_virt = (char *)qp_mz->addr;
    queue->req_phy  = qp_mz->iova | (uint64_t)vf_num<<56;
    if (rsp_qp_check_queue_alignment(queue->req_phy, RSP_DMA_ALIGN_SIZE)) 
    {
        RSP_LOG(ERR, "Invalid alignment on queue create " " 0x%"PRIx64"\n",    queue->req_phy);
        ret = -EFAULT;
        goto queue_create_err1;
    }

    snprintf(queue->memz_name_resp, sizeof(queue->memz_name_resp),//must unique
            "%s_%d_%s_%s_%d",
            pci_dev->driver->driver.name, rsp_dev->dev_id,
            conf->service_str, "resp",
            QP->ring_id);
    qp_mz = queue_dma_zone_reserve(queue->memz_name_resp, 
                                queue->queue_size*queue->resp_size, 
                                rsp_dev->pci_dev->device.numa_node);
    if (qp_mz == NULL) 
    {
        RSP_LOG(ERR, "Failed to allocate ring memzone for response");
        ret = -EFAULT;
        goto queue_create_err1;
    }

    queue->resp_virt = (char *)qp_mz->addr;
    queue->resp_phy = qp_mz->iova | (uint64_t)vf_num<<56;
    if (rsp_qp_check_queue_alignment(queue->resp_phy, RSP_DMA_ALIGN_SIZE))
    {
        RSP_LOG(ERR, "Invalid alignment on queue create " " 0x%"PRIx64"\n",    queue->resp_phy);
        ret = -EFAULT;
        goto queue_create_err2;
    }
    /*
     * Write an magic pattern to the queue memory.
     */
    memset(queue->req_virt, 0x00, queue->queue_size*queue->msg_size);
    memset(queue->resp_virt, 0x7F, queue->queue_size*queue->resp_size);

    if( QP->ring_type == RSP_RING_TYPE_PUB)
    {
        op_cookie = &queue->op_cookie;
        //set op_cokies info
        op_cookie->count   = queue->queue_size;
        op_cookie->size    = conf->cookie_size;
        op_cookie->cookies = rte_zmalloc_socket("rsp PMD asym cookies pointer", 
                                                op_cookie->size*(op_cookie->count),
                                                RTE_CACHE_LINE_SIZE, 
                                                conf->socket_id);

        if (op_cookie->cookies == NULL) 
        {
            RSP_LOG(ERR, "Failed to alloc mem for cookies");
            ret = -EFAULT;
            goto queue_create_err2;
        }

        asym_cookie = (struct rsp_asym_cookies *)op_cookie->cookies;
        for (i = 0; i < (int)(op_cookie->count); i++) 
        {
            snprintf(op_cookie_name, RTE_RING_NAMESIZE, "%s%d_%s_q%hu_c%d",//must unique
                    pci_dev->driver->driver.name, rsp_dev->dev_id,
                    conf->service_str, conf->queue_pair_id,i);
            qp_mz = rte_memzone_reserve_aligned(op_cookie_name, 
                                                sizeof(struct rsp_asym_item),
                                                rsp_dev->pci_dev->device.numa_node,
                                                RTE_MEMZONE_IOVA_CONTIG, RSP_DMA_ALIGN_SIZE);

            if (qp_mz == NULL) 
            {
                RSP_LOG(ERR, "RSP PMD Cannot get cookie");
                ret = -EFAULT;
                goto queue_create_err3;
            }
            
            asym_cookie[i].mz   = qp_mz;
            asym_cookie[i].virt = qp_mz->addr;
            asym_cookie[i].phy  = qp_mz->iova | (uint64_t)vf_num<<56;
            
            cookie_temp = qp_mz->addr;
            //RSP_DP_LOG(DEBUG, "asym_cookie[%d].virt=%"PRIu64" asym_cookie[%d].phy=%"PRIu64"",i, asym_cookie[i].virt, i, asym_cookie[i].phy);
            for(j=0;j<RSP_MAX_IN_PARAM_NUM;j++)
            {
                cookie_temp->in_param[j] = asym_cookie[i].phy+offsetof(struct rsp_asym_item, in_param2)+sizeof(param_t)*j;
                cookie_temp->in_param2[j].phy = asym_cookie[i].phy+offsetof(struct rsp_asym_item, in_data)+RSP_MAX_PARAM_SIZE*j;
            }
            
            for(j=0;j<RSP_MAX_OUT_PARAM_NUM;j++)
            {
                cookie_temp->out_param[j] = asym_cookie[i].phy+offsetof(struct rsp_asym_item, out_param2)+sizeof(param_t)*j;
                cookie_temp->out_param2[j].phy = asym_cookie[i].phy+offsetof(struct rsp_asym_item, out_data)+RSP_MAX_PARAM_SIZE*j;
            }
        }
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_CLEAR(QP->bank_id,QP->ring_id),              1);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_REQUEST_BASE_LO(QP->bank_id,QP->ring_id),   queue->req_phy       & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_REQUEST_BASE_HI(QP->bank_id,QP->ring_id),  (queue->req_phy>>32)  & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_RESPONSE_BASE_LO(QP->bank_id,QP->ring_id),  queue->resp_phy      & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_RESPONSE_BASE_HI(QP->bank_id,QP->ring_id), (queue->resp_phy>>32) & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_SIZE(QP->bank_id,QP->ring_id),              queue->msg_size * queue->queue_size);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_TYPE(QP->bank_id,QP->ring_id),              NPUB_RING_TYPE_PUB);

    }else if( QP->ring_type == RSP_RING_TYPE_BULK)
    {
        op_cookie = &queue->op_cookie;
        //set op_cokies info
        op_cookie->count   = queue->queue_size;
        op_cookie->size    = conf->cookie_size;
        op_cookie->cookies = rte_zmalloc_socket("rsp PMD rpu cookies pointer", 
                                                op_cookie->size*(op_cookie->count),
                                                RTE_CACHE_LINE_SIZE, 
                                                conf->socket_id);

        if (op_cookie->cookies == NULL) 
        {
            RSP_LOG(ERR, "Failed to alloc mem for rpu cookies");
            ret = -EFAULT;
            goto queue_create_err2;
        }

        bulk_cookie = (struct rsp_bulk_cookies *)op_cookie->cookies;
        for (i = 0; i < (int)(op_cookie->count); i++) 
        {
            snprintf(op_cookie_name, RTE_RING_NAMESIZE, "%s%d_%s_q%hu_c%d",//must unique
                    pci_dev->driver->driver.name, rsp_dev->dev_id,
                    conf->service_str, conf->queue_pair_id,i);
            qp_mz = rte_memzone_reserve_aligned(op_cookie_name, 
                                                sizeof(struct rsp_bulk_item),
                                                rsp_dev->pci_dev->device.numa_node,
                                                RTE_MEMZONE_IOVA_CONTIG, RSP_DMA_ALIGN_SIZE);

            if (qp_mz == NULL) 
            {
                RSP_LOG(ERR, "RSP PMD Cannot get cookie");
                ret = -EFAULT;
                goto queue_create_err3;
            }
            bulk_cookie[i].mz   = qp_mz;
            bulk_cookie[i].virt = qp_mz->addr;
            bulk_cookie[i].phy  = qp_mz->iova | (uint64_t)vf_num<<56;
            //printf("bulk_cookie[%d].virt=%p bulk_cookie[%d].phy=0x%x \n",i,bulk_cookie[i].virt,i,bulk_cookie[i].phy);
        }
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_CLEAR(QP->bank_id,QP->ring_id),              1);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_REQUEST_BASE_LO(QP->bank_id,QP->ring_id),   queue->req_phy       & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_REQUEST_BASE_HI(QP->bank_id,QP->ring_id),  (queue->req_phy>>32)  & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_RESPONSE_BASE_LO(QP->bank_id,QP->ring_id),  queue->resp_phy      & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_RESPONSE_BASE_HI(QP->bank_id,QP->ring_id), (queue->resp_phy>>32) & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_SIZE(QP->bank_id,QP->ring_id),              queue->msg_size * queue->queue_size);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_TYPE(QP->bank_id,QP->ring_id),              NPUB_RING_TYPE_BULK);
    }else{
        op_cookie = &queue->op_cookie;
        //set op_cokies info
        op_cookie->count   = queue->queue_size;
        op_cookie->size    = conf->cookie_size;
        op_cookie->cookies = rte_zmalloc_socket("rsp PMD bulk cookies pointer", 
                                                op_cookie->size*(op_cookie->count),
                                                RTE_CACHE_LINE_SIZE, 
                                                conf->socket_id);

        if (op_cookie->cookies == NULL) 
        {
            RSP_LOG(ERR, "Failed to alloc mem for cookies");
            ret = -EFAULT;
            goto queue_create_err2;
        }

        sym_cookie = (struct rsp_sym_cookies *)op_cookie->cookies;
        for (i = 0; i < (int)(op_cookie->count); i++) 
        {
            snprintf(op_cookie_name, RTE_RING_NAMESIZE, "%s%d_%s_q%hu_c%d",//must unique
                    pci_dev->driver->driver.name, rsp_dev->dev_id,
                    conf->service_str, conf->queue_pair_id,i);
            qp_mz = rte_memzone_reserve_aligned(op_cookie_name, 
                                                sizeof(struct rsp_sym_item),
                                                rsp_dev->pci_dev->device.numa_node,
                                                RTE_MEMZONE_IOVA_CONTIG, RSP_DMA_ALIGN_SIZE);

            if (qp_mz == NULL) 
            {
                RSP_LOG(ERR, "RSP PMD Cannot get cookie");
                ret = -EFAULT;
                goto queue_create_err3;
            }
            sym_cookie[i].mz   = qp_mz;
            sym_cookie[i].virt = qp_mz->addr;
            sym_cookie[i].phy  = qp_mz->iova | (uint64_t)vf_num<<56;
            //printf("sym_cookie[%d].virt=%p sym_cookie[%d].phy=0x%x \n",i,sym_cookie[i].virt,i,sym_cookie[i].phy);
        }
        
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_STATUS_REG(rsp_dev->model, QP->ring_id), 0x0);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_STATUS_REG(rsp_dev->model, QP->ring_id), 0x0);
        /*
        * RPU-TX ring Initialize
        */
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_BASE_ADDR_L_REG(rsp_dev->model, QP->ring_id),  queue->req_phy      & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_BASE_ADDR_H_REG(rsp_dev->model, QP->ring_id), (queue->req_phy>>32) & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_SIZE(rsp_dev->model, QP->ring_id),             queue->msg_size * queue->queue_size);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_HEAD_REG(rsp_dev->model, QP->ring_id), 0x0);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_TAIL_REG(rsp_dev->model, QP->ring_id), 0x0);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_STATUS_REG(rsp_dev->model, QP->ring_id), 0x1);
        /*
         * RPU-RX ring Initialize
         */

        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_BASE_ADDR_L_REG(rsp_dev->model, QP->ring_id),  queue->resp_phy      & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_BASE_ADDR_H_REG(rsp_dev->model, QP->ring_id), (queue->resp_phy>>32) & 0xFFFFFFFF);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_SIZE(rsp_dev->model, QP->ring_id),             queue->resp_size * queue->queue_size);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_HEAD_REG(rsp_dev->model, QP->ring_id), 0x0);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_TAIL_REG(rsp_dev->model, QP->ring_id), 0x0);
        RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_STATUS_REG(rsp_dev->model, QP->ring_id), 0x1);
    }
    
    RSP_LOG(DEBUG, "ring info:");
    RSP_LOG(DEBUG, "        QP->bank_id=%d   QP->ring_id=%d",QP->bank_id, QP->ring_id);
    RSP_LOG(DEBUG, "        memz_name_req=%s   memz_name_resp=%s",queue->memz_name_req, queue->memz_name_resp);
    RSP_LOG(DEBUG, "        req_virt=%p req_phy=0x%x",  queue->req_virt,queue->req_phy);
    RSP_LOG(DEBUG, "        resp_virt=%p resp_phy=0x%x",queue->resp_virt,queue->resp_phy);
    RSP_LOG(DEBUG, "        cookies_pool=%p op_cookie.cokies=%p",queue->cookies_pool,queue->op_cookie.cookies);

    return 0;
queue_create_err3:
    for (i = 0; i < (int)(op_cookie->count); i++) 
    {
        if (asym_cookie[i].mz != NULL) 
        {
            rte_memzone_free(asym_cookie[i].mz);
        }
    }
queue_create_err2:
    qp_mz=rte_memzone_lookup(queue->memz_name_resp);
    if(qp_mz!=NULL)
        rte_memzone_free(qp_mz);
queue_create_err1:
    qp_mz=rte_memzone_lookup(queue->memz_name_req);
    if(qp_mz!=NULL)
        rte_memzone_free(qp_mz);
    return ret;
}
void rsp_disable_rpu(void *ptr_base, int model)
{
    //force disable all ring , do not care other user. need fix better
    int i;
    for(i=0;i<RSP_RPU_MAX_NUM;i++)
    {
        RSP_REG_WRITE((uint64_t)(ptr_base) + RPU_TX_RING_STATUS_REG(model, i), 0x0);
        RSP_REG_WRITE((uint64_t)(ptr_base) + RPU_RX_RING_STATUS_REG(model, i), 0x0);
    }
}

uint16_t rsp_enqueue_op_burst(void *qp, void **ops, uint16_t nb_ops)
{
    uint32_t i;
    struct rsp_asym_cookies *asym_cookie;
    struct rsp_sym_cookies *sym_cookie;
    struct rsp_bulk_cookies *bulk_cookie;
    register struct rsp_qp *queue;
    struct rsp_QP *QP = (struct rsp_QP *)qp;
    register uint32_t nb_ops_sent = 0;
    register int ret;
    uint16_t nb_ops_possible = nb_ops;
    register uint32_t tail;
    //RSP_LOG(DEBUG,"nb_ops=%d ",nb_ops);
    if (unlikely(nb_ops == 0))
        return 0;

    queue = &(QP->queue);
    tail = queue->req_tail;

    {
        uint32_t inflights = QP->enqueued - QP->dequeued;
        if ((inflights + nb_ops) > (QP->max_inflights-1)) 
        {
            nb_ops_possible = (QP->max_inflights-1) - inflights;
            if (nb_ops_possible == 0)
                return 0;
        }
        if (QP->min_enq_burst_threshold && 
            inflights > RSP_QP_MIN_INFL_THRESHOLD && 
            nb_ops_possible < QP->min_enq_burst_threshold)
        {
            QP->stats.threshold_hit_count++;
            return 0;
        }
    }
    asym_cookie = queue->op_cookie.cookies;
    sym_cookie  = queue->op_cookie.cookies;
    bulk_cookie = queue->op_cookie.cookies;
    while (nb_ops_sent != nb_ops_possible) 
    {
        if(QP->ring_type == RSP_RING_TYPE_PUB)
            ret = QP->build_request((void *)(*ops), (uint8_t *)(queue->req_virt)+tail*queue->msg_size, &asym_cookie[tail], QP);
        else if(QP->ring_type == RSP_RING_TYPE_RPU)
            ret = QP->build_request((void *)(*ops), (uint8_t *)(queue->req_virt)+tail*queue->msg_size, &sym_cookie[tail],  QP);
        else
            ret = QP->build_request((void *)(*ops), (uint8_t *)(queue->req_virt)+tail*queue->msg_size, &bulk_cookie[tail], QP);
        if (ret != 0) 
        {
            QP->stats.enqueue_err_count++;
            if (nb_ops_sent == 0)
                return 0;
            goto kick_tail;
        }
        tail++;
        if(tail >= queue->queue_size)
            tail=0;
        ops++;
        nb_ops_sent++;
    }

kick_tail:
    queue->req_tail = tail;
    QP->enqueued += nb_ops_sent;
    QP->stats.enqueued_count += nb_ops_sent;

    if(nb_ops_sent > 0)
    {
        if(QP->ring_type == RSP_RING_TYPE_RPU)
        {
            RSP_LOG(DEBUG,"RPU pluse QP->ring_id=%d ",QP->ring_id);
            RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_TX_RING_TAIL_REG(QP->model, QP->ring_id), queue->msg_size*queue->req_tail);
        }else{
            RSP_LOG(DEBUG,"PUB pluse QP->bank_id=%d QP->ring_id=%d ",QP->bank_id, QP->ring_id);
            for(i=0;i<nb_ops_sent;i++)
                RSP_REG_WRITE((uint64_t)(QP->ptr_base) + NPUB_RING_PLUS(QP->bank_id,QP->ring_id), 1);
        }
    }
    RSP_LOG(DEBUG,"QP->bank_id=%d QP->ring_id=%d nb_ops_sent=%d nb_ops_possible=%d  tail=%d ",QP->bank_id, QP->ring_id, nb_ops_sent, nb_ops_possible, tail);
    return nb_ops_sent;
}

uint16_t rsp_dequeue_op_burst(void *qp, void **ops, uint16_t nb_ops)
{
    struct rsp_qp *queue;
    struct rsp_QP *QP = (struct rsp_QP *)qp;
    uint32_t head;
    uint32_t op_resp_counter = 0; 
    uint8_t *resp_msg;
    struct rsp_asym_cookies *asym_cookie;
    struct rsp_bulk_cookies *bulk_cookie;
    struct rsp_sym_cookies *sym_cookie;
    queue = &(QP->queue);
    head = queue->resp_head;
    resp_msg = (uint8_t *)queue->resp_virt + head*queue->resp_size;
    while (*(uint32_t *)resp_msg != RSP_RESP_MAGIC_CODE && op_resp_counter != nb_ops) 
    {
        RSP_DP_HEXDUMP_LOG(DEBUG, "resp_msg:",queue->resp_virt,64);
        if (QP->ring_type == RSP_RING_TYPE_RPU)
        {
            sym_cookie = (struct rsp_sym_cookies *)(queue->op_cookie.cookies);
            rsp_sym_process_response(ops, resp_msg, &sym_cookie[head]);
        }else if (QP->ring_type == RSP_RING_TYPE_PUB)
        {
            asym_cookie = (struct rsp_asym_cookies *)(queue->op_cookie.cookies);
            rsp_asym_process_response(ops, resp_msg, &asym_cookie[head]);
        }else{
            bulk_cookie = (struct rsp_bulk_cookies *)(queue->op_cookie.cookies);
            rsp_bulk_process_response(ops, resp_msg, &bulk_cookie[head]);
        }
        
        *(uint32_t *)resp_msg = RSP_RESP_MAGIC_CODE;
        head++;
        if(head>=queue->queue_size)
            head=0;
        resp_msg = (uint8_t *)queue->resp_virt + head*queue->resp_size;

        ops++;
        op_resp_counter++;
        queue->nb_processed_responses++;
    }
    if(op_resp_counter>0)
    {
        QP->dequeued += op_resp_counter;
        QP->stats.dequeued_count += op_resp_counter;

        queue->resp_head = head;
        RSP_DP_LOG(DEBUG,"QP->ring_id=%d nb_ops=%d op_resp_counter=%d head=%d \n",QP->ring_id, nb_ops, op_resp_counter, head);
        if(QP->ring_type == RSP_RING_TYPE_RPU)
        {
            RSP_REG_WRITE((uint64_t)(QP->ptr_base) + RPU_RX_RING_HEAD_REG(QP->model, QP->ring_id), head*queue->resp_size);
        }
        RSP_DP_LOG(DEBUG, "Dequeue burst return: %u", op_resp_counter);
    }
    return op_resp_counter;
}
