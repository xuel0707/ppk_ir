#include <rte_bus_pci.h>
#include <rte_common.h>
#include <rte_dev.h>
#include <rte_malloc.h>
#include <rte_pci.h>
#include <rte_cryptodev_pmd.h>

#include "rsp_logs.h"
#include "rsp_sym.h"
#include "rsp_session.h"
#include "rsp_sym_pmd.h"

uint8_t rsp_sym_driver_id;

const struct rte_cryptodev_capabilities rsp_sym_capabilities[] = {
    RSP_BASE_SYM_CAPABILITIES,
    RTE_CRYPTODEV_END_OF_CAPABILITIES_LIST()
};

static int rsp_sym_qp_release(struct rte_cryptodev *dev, uint16_t qp_id);
static int rsp_sym_dev_config(__rte_unused struct rte_cryptodev *dev,
        __rte_unused struct rte_cryptodev_config *config)
{
    return 0;
}

static int rsp_sym_dev_start(__rte_unused struct rte_cryptodev *dev)
{
    return 0;
}

static void rsp_sym_dev_stop(__rte_unused struct rte_cryptodev *dev)
{
    return;
}

static int rsp_sym_dev_close(struct rte_cryptodev *dev)
{
    int i, ret;

    for (i = 0; i < dev->data->nb_queue_pairs; i++) 
    {
        ret = rsp_sym_qp_release(dev, i);
        if (ret < 0)
            return ret;
    }

    return 0;
}

static void rsp_sym_dev_info_get(struct rte_cryptodev *dev, struct rte_cryptodev_info *info)
{
    struct rsp_sym_dev_private *priv = dev->data->dev_private;
    if ((info == NULL) || (dev==NULL))
    {
        RSP_LOG(ERR, "invalid ptr: info %p, dev %p", info, dev);
        return;
    } 
    
    info->max_nb_queue_pairs     = priv->qp_num;
    info->feature_flags          = dev->feature_flags;
    info->capabilities           = priv->rsp_dev_capabilities;
    info->driver_id              = rsp_sym_driver_id;
    info->sym.max_nb_sessions    = 0;//no sess number limit

}

static void rsp_sym_stats_get(struct rte_cryptodev *dev, struct rte_cryptodev_stats *stats)
{
    struct rsp_sym_dev_private *priv;
    struct rsp_common_stats rsp_stats = {0};
    struct rsp_QP *QP;
    if (stats == NULL || dev == NULL) 
    {
        RSP_LOG(ERR, "invalid ptr: stats %p, dev %p", stats, dev);
        return;
    }
    priv = dev->data->dev_private;
    rsp_stats_get(priv->rsp_dev, 128, &rsp_stats, RSP_SERVICE_SYMMETRIC);
    stats->enqueued_count      = rsp_stats.enqueued_count;
    stats->dequeued_count      = rsp_stats.dequeued_count;
    stats->enqueue_err_count   = rsp_stats.enqueue_err_count;
    stats->dequeue_err_count   = rsp_stats.dequeue_err_count;
}

static void rsp_sym_stats_reset(struct rte_cryptodev *dev)
{
    struct rsp_QP *QP;
    struct rsp_sym_dev_private *priv;

    if (dev == NULL) 
    {
        RSP_LOG(ERR, "invalid cryptodev ptr %p", dev);
        return;
    }
    priv = dev->data->dev_private;
    rsp_stats_reset(priv->rsp_dev, 128, RSP_SERVICE_SYMMETRIC);
}

static int rsp_sym_qp_release(struct rte_cryptodev *dev, uint16_t qp_id)
{
    int ret=0;
    struct rsp_sym_dev_private *priv = dev->data->dev_private;
    RSP_LOG(DEBUG, "Release sym qp %u on device %d", qp_id, dev->data->dev_id);

    priv->rsp_dev->rpu_qps_in_use[qp_id]  = NULL;
    rsp_qp_release((struct rsp_QP *)(dev->data->queue_pairs[qp_id]));
    dev->data->queue_pairs[qp_id]=NULL;
    return 0;
}

static int rsp_sym_qp_setup(struct rte_cryptodev *dev,
                         uint16_t qp_id,
                         const struct rte_cryptodev_qp_conf *rte_qp_conf,
                         int socket_id)
{
    struct rsp_QP *QP=NULL;
    int ret = 0;
    uint32_t i;
    struct rsp_qp_config conf;
    struct rsp_sym_dev_private *priv = dev->data->dev_private;
    if (qp_id >= priv->qp_num) 
    {
        RSP_LOG(ERR, "qp_id %u invalid for this device (max qp=%d )", qp_id,priv->qp_num);
        return -EINVAL;
    }

    struct rsp_QP **QP_addr = (struct rsp_QP **)&(dev->data->queue_pairs[qp_id]);

    if (*QP_addr != NULL) 
    {
        ret = rsp_sym_qp_release(dev, qp_id);
        if (ret < 0)
            return ret;
    }
    if((priv->qp_ids[qp_id] < 0))// || (priv->qp_ids[qp_id] >= 128))
    {
        RSP_LOG(ERR,"Invalid ring id");
        return -EINVAL;
    }
    conf.msg_size         = 128;
    conf.resp_size        = 32;
    conf.type             = RSP_RING_TYPE_RPU;
    conf.build_request    = rsp_sym_build_request;
    conf.cookie_size      = RSP_RPU_COKIES_SIZE;
    conf.nb_descriptors   = rte_qp_conf->nb_descriptors;
    conf.socket_id        = socket_id;
    conf.queue_pair_id    = priv->qp_ids[qp_id];
    conf.service_str      = "sym";
    conf.min_enq_burst_threshold = priv->min_enq_burst_threshold;//default 0 . Always enqueue

    RSP_LOG(DEBUG, "nb_descriptors=%d conf.queue_pair_id=%d conf.service_str=%s ",conf.nb_descriptors,conf.queue_pair_id,conf.service_str);

    ret = rsp_qp_setup(priv->rsp_dev, QP_addr, &conf);
    if (ret != 0)
        return ret;

    priv->rsp_dev->rpu_qps_in_use[qp_id]=*QP_addr;

    QP = (struct rsp_QP *)*QP_addr;
    QP->bank_id                 = 0;
    return ret;
}

struct rte_cryptodev_ops crypto_rsp_sym_ops = {

        /* Device related operations */
        .dev_configure        = rsp_sym_dev_config,
        .dev_start            = rsp_sym_dev_start,
        .dev_stop             = rsp_sym_dev_stop,
        .dev_close            = rsp_sym_dev_close,
        .dev_infos_get        = rsp_sym_dev_info_get,

        .stats_get            = rsp_sym_stats_get,
        .stats_reset          = rsp_sym_stats_reset,
        .queue_pair_setup     = rsp_sym_qp_setup,
        .queue_pair_release   = rsp_sym_qp_release,

        /* Crypto related operations */
        .sym_session_get_size    = rsp_sym_session_get_private_size,
        .sym_session_configure   = rsp_sym_session_configure,
        .sym_session_clear       = rsp_sym_session_clear
};

static uint16_t rsp_sym_pmd_enqueue_op_burst(void *qp, struct rte_crypto_op **ops,
        uint16_t nb_ops)
{
    return rsp_enqueue_op_burst(qp, (void **)ops, nb_ops);
}

static uint16_t rsp_sym_pmd_dequeue_op_burst(void *qp, struct rte_crypto_op **ops,
        uint16_t nb_ops)
{
    return rsp_dequeue_op_burst(qp, (void **)ops, nb_ops);
}

const char rsp_sym_drv_name[] = RTE_STR(CRYPTODEV_NAME_RSP_SYM_PMD);
const struct rte_driver cryptodev_rsp_sym_driver = 
{
    .name  = rsp_sym_drv_name,
    .alias = rsp_sym_drv_name
};

int rsp_sym_dev_create(struct rsp_pci_device *rsp_pci_dev,
        struct rsp_dev_cmd_param *rsp_dev_cmd_param __rte_unused)
{
    int offset=0;
    int i = 0;
    int qp_num=0;
    struct rte_cryptodev_pmd_init_params init_params = {
            .name = "",
            .socket_id = rsp_pci_dev->pci_dev->device.numa_node,
            .private_data_size = sizeof(struct rsp_sym_dev_private)
    };
    char name[RTE_CRYPTODEV_NAME_MAX_LEN];
    struct rte_cryptodev *cryptodev;
    struct rsp_sym_dev_private *priv;
    if(rsp_pci_dev->vf_num==0)
    {
        qp_num = rsp_pci_dev->qp_pf;
        RSP_LOG(INFO, "pf qp_num=%d ",qp_num);
    }else
    {
        //VF
        qp_num = rsp_pci_dev->qp_vf;
        RSP_LOG(INFO, "vf qp_num=%d ",qp_num);
    }
    init_params.max_nb_queue_pairs = qp_num;
    
    snprintf(name, RTE_CRYPTODEV_NAME_MAX_LEN, "%s_%s_%s",rsp_pci_dev->name, (rsp_pci_dev->model==RSP_MODEL_S10)? "s10":"s20", "sym");
    rsp_pci_dev->sym_rte_dev.driver     = &cryptodev_rsp_sym_driver;
    rsp_pci_dev->sym_rte_dev.numa_node  = rsp_pci_dev->pci_dev->device.numa_node;
    rsp_pci_dev->sym_rte_dev.devargs    = NULL;
    //create crypto
    cryptodev = rte_cryptodev_pmd_create(name, &(rsp_pci_dev->sym_rte_dev), &init_params);
    if (cryptodev == NULL)
    {
        RSP_LOG(ERR, "Creating RSP SYM device %s failed ", name);
        return -ENODEV;
    }
    rsp_pci_dev->sym_rte_dev.name= cryptodev->data->name;
    cryptodev->driver_id         = rsp_sym_driver_id;
    cryptodev->dev_ops           = &crypto_rsp_sym_ops;
    cryptodev->enqueue_burst     = rsp_sym_pmd_enqueue_op_burst;
    cryptodev->dequeue_burst     = rsp_sym_pmd_dequeue_op_burst;

    cryptodev->feature_flags = RTE_CRYPTODEV_FF_SYMMETRIC_CRYPTO |
                               RTE_CRYPTODEV_FF_HW_ACCELERATED |
                               RTE_CRYPTODEV_FF_SYM_OPERATION_CHAINING |
                               RTE_CRYPTODEV_FF_OOP_LB_IN_LB_OUT |
                               RSP_RTE_CRYPTODEV_HW_RPU;
    priv = cryptodev->data->dev_private;
    rsp_pci_dev->sym_dev_priv = priv;
    priv->rsp_dev             = rsp_pci_dev;
    priv->dev_id              = cryptodev->data->dev_id;
    priv->vf_num              = rsp_pci_dev->vf_num;
    priv->qp_num              = qp_num;
    priv->model               = rsp_pci_dev->model;
    if(rsp_pci_dev->vf_num==0)
    {
        //PF
        offset = 0;
    }else
    {
        //VF
        offset = rsp_pci_dev->qp_pf==0 ? 0 : (priv->vf_num-0x80)*rsp_pci_dev->qp_vf + rsp_pci_dev->qp_pf;
    }
    for(i=0; i<priv->qp_num; i++)
    {
        priv->qp_ids[i] = (int8_t)(i+offset);
        RSP_LOG(DEBUG, "priv->qp_ids[%d]=%d ",i, priv->qp_ids[i]);
    }
    
    //set capabilities 
    priv->rsp_dev_capabilities = rsp_sym_capabilities;
    priv->min_enq_burst_threshold = 1;//Min enqueue size
    //while (1) {
    //    /*if set threshold, we need get here*/
    //    if (rsp_dev_cmd_param[i].name == NULL)
    //        break;
    //    if (!strcmp(rsp_dev_cmd_param[i].name, SYM_ENQ_THRESHOLD_NAME))
    //        priv->min_enq_burst_threshold =    rsp_dev_cmd_param[i].val;
    //    if (!strcmp(rsp_dev_cmd_param[i].name, RSP_VF_NUM))
    //        priv->rpu_queue = rsp_dev_cmd_param[i].val;
    //    //if (!strcmp(rsp_dev_cmd_param[i].name, RSP_PF_BANK))
    //    //    priv->pub_bank = rsp_dev_cmd_param[i].val;
    //    i++;
    //}
    RSP_LOG(DEBUG, "Created RSP SYM device %s as cryptodev instance %d", cryptodev->data->name, priv->dev_id);
    return 0;
}

int rsp_sym_dev_destroy(struct rsp_pci_device *rsp_pci_dev)
{
    struct rte_cryptodev *cryptodev;

    if (rsp_pci_dev == NULL)
        return -ENODEV;
    if (rsp_pci_dev->sym_dev_priv == NULL)
        return 0;
    /* free crypto device */
    cryptodev = rte_cryptodev_pmd_get_dev(rsp_pci_dev->sym_dev_priv->dev_id);
    //free QP
    rsp_sym_dev_close(cryptodev);
    rte_cryptodev_pmd_destroy(cryptodev);
    rsp_pci_dev->sym_rte_dev.name = NULL;
    rsp_pci_dev->sym_dev_priv = NULL;
    return 0;
}

struct cryptodev_driver rsp_crypto_drv;
RTE_PMD_REGISTER_CRYPTO_DRIVER(rsp_crypto_drv,
        cryptodev_rsp_sym_driver,
        rsp_sym_driver_id);
