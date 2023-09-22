#include <rte_cryptodev_pmd.h>
#include "rsp_logs.h"
#include "rsp_asym.h"
#include "rsp_asym_pmd.h"
#include "rsp_sym_capabilities.h"
#include "rsp_asym_capabilities.h"
#include "rsp_asym_sess.h"
uint8_t rsp_asym_driver_id;

static const struct rte_cryptodev_capabilities rsp_asym_capabilities[] = {
    RSP_BASE_ASYM_CAPABILITIES,
    RTE_CRYPTODEV_END_OF_CAPABILITIES_LIST()
};

static int rsp_asym_qp_release(struct rte_cryptodev *dev,
                   uint16_t queue_pair_id);

static int rsp_asym_dev_config(__rte_unused struct rte_cryptodev *dev,
                   __rte_unused struct rte_cryptodev_config *config)
{
    return 0;
}

static int rsp_asym_dev_start(__rte_unused struct rte_cryptodev *dev)
{
    return 0;
}

static void rsp_asym_dev_stop(__rte_unused struct rte_cryptodev *dev)
{

}

static int rsp_asym_dev_close(struct rte_cryptodev *dev)
{
    int i, ret;
    for (i = 0; i < dev->data->nb_queue_pairs; i++) {
        ret = rsp_asym_qp_release(dev, i);
        if (ret < 0)
            return ret;
    }
    return 0;
}

static void rsp_asym_dev_info_get(struct rte_cryptodev *dev, struct rte_cryptodev_info *info)
{
    struct rsp_asym_dev_private *priv = dev->data->dev_private;
    if( (info == NULL) || (dev == NULL) )
    {
        RSP_LOG(ERR, "invalid dev or info ");
    }

    info->max_nb_queue_pairs  = priv->qp_num;
    info->feature_flags       = dev->feature_flags;
    info->capabilities        = priv->rsp_dev_capabilities;
    info->driver_id           = rsp_asym_driver_id;
    info->sym.max_nb_sessions = 0;//no sess number limit
}

static void rsp_asym_stats_get(struct rte_cryptodev *dev, struct rte_cryptodev_stats *stats)
{
    struct rsp_common_stats rsp_stats = {0};
    struct rsp_asym_dev_private *priv;
    if (stats == NULL || dev == NULL) 
    {
        RSP_LOG(ERR, "invalid ptr: stats %p, dev %p", stats, dev);
        return;
    }
    priv = dev->data->dev_private;
    rsp_stats_get(priv->rsp_dev, 128, &rsp_stats, RSP_SERVICE_ASYMMETRIC);
    stats->enqueued_count        = rsp_stats.enqueued_count;
    stats->dequeued_count        = rsp_stats.dequeued_count;
    stats->enqueue_err_count     = rsp_stats.enqueue_err_count;
    stats->dequeue_err_count     = rsp_stats.dequeue_err_count;
}

static void rsp_asym_stats_reset(struct rte_cryptodev *dev)
{
    struct rsp_asym_dev_private *priv;
    if (dev == NULL) 
    {
        RSP_LOG(ERR, "invalid asymmetric cryptodev ptr %p", dev);
        return;
    }
    priv = dev->data->dev_private;

    rsp_stats_reset(priv->rsp_dev, 128, RSP_SERVICE_ASYMMETRIC);
}

static int rsp_asym_qp_release(struct rte_cryptodev *dev,
                   uint16_t qp_id)
{
    struct rsp_asym_dev_private *priv = dev->data->dev_private;

    RSP_LOG(DEBUG, "Release asym qp %u on device %d", qp_id, dev->data->dev_id);

    priv->rsp_dev->pub_qps_in_use[qp_id]= NULL;

    rsp_qp_release((struct rsp_QP *)(dev->data->queue_pairs[qp_id]));

    dev->data->queue_pairs[qp_id]=NULL;
    return 0;
}

static int rsp_asym_qp_setup(struct rte_cryptodev *dev,
                    uint16_t qp_id,
                     const struct rte_cryptodev_qp_conf *rte_qp_conf,
                     int socket_id)
{
    struct rsp_qp_config conf;
    int ret = 0;
    uint32_t i;

    struct rsp_QP **QP_addr = (struct rsp_QP **)&(dev->data->queue_pairs[qp_id]);
    struct rsp_asym_dev_private *priv = dev->data->dev_private;

    if(*QP_addr != NULL)
    {
        ret = rsp_asym_qp_release(dev, qp_id);
        if (ret < 0)
            return ret;
    }
    if (qp_id >= priv->qp_num) 
    {
        RSP_LOG(ERR, "qp_id %u invalid for this device", qp_id);
        return -EINVAL;
    }

    conf.msg_size          = 64;
    conf.resp_size         = 32;
    conf.type              = RSP_RING_TYPE_PUB;
    conf.build_request     = rsp_asym_build_request;
    conf.cookie_size       = sizeof(struct rsp_asym_item);
    conf.nb_descriptors    = rte_qp_conf->nb_descriptors;
    conf.socket_id         = socket_id;
    conf.queue_pair_id     = priv->qp_ids[qp_id];
    conf.bank_id           = priv->npub_bank;
    conf.service_str       = "asym";
    conf.min_enq_burst_threshold = priv->min_enq_burst_threshold;

    ret = rsp_qp_setup(priv->rsp_dev, QP_addr, &conf);
    if (ret != 0)
        return ret;

    /* store a link to the qp in the rsp_pci_device */
    priv->rsp_dev->pub_qps_in_use[qp_id] = *QP_addr;
    return ret;
}

struct rte_cryptodev_ops crypto_rsp_asym_ops = {

    /* Device related operations */
    .dev_configure        = rsp_asym_dev_config,
    .dev_start            = rsp_asym_dev_start,
    .dev_stop             = rsp_asym_dev_stop,
    .dev_close            = rsp_asym_dev_close,
    .dev_infos_get        = rsp_asym_dev_info_get,

    .stats_get            = rsp_asym_stats_get,
    .stats_reset          = rsp_asym_stats_reset,
    .queue_pair_setup     = rsp_asym_qp_setup,
    .queue_pair_release   = rsp_asym_qp_release,

    /* Crypto related operations */
    .asym_session_get_size     = rsp_asym_session_get_private_size,
    .asym_session_configure    = rsp_asym_session_configure,
    .asym_session_clear        = rsp_asym_session_clear
};

uint16_t rsp_asym_pmd_enqueue_op_burst(void *qp, struct rte_crypto_op **ops,
                       uint16_t nb_ops)
{
    return rsp_enqueue_op_burst(qp, (void **)ops, nb_ops);
}

uint16_t rsp_asym_pmd_dequeue_op_burst(void *qp, struct rte_crypto_op **ops,
                       uint16_t nb_ops)
{
    return rsp_dequeue_op_burst(qp, (void **)ops, nb_ops);
}

static const char rsp_asym_drv_name[] = RTE_STR(CRYPTODEV_NAME_RSP_ASYM_PMD);
static const struct rte_driver cryptodev_rsp_asym_driver = {
    .name = rsp_asym_drv_name,
    .alias = rsp_asym_drv_name
};

int rsp_asym_dev_create(struct rsp_pci_device *rsp_pci_dev,
        struct rsp_dev_cmd_param *rsp_dev_cmd_param)
{
    int i = 0;
    int j = 0;
    int qp_num=0;
    int offset=0;
    struct rte_cryptodev_pmd_init_params init_params = {
            .name = "",
            .socket_id = rsp_pci_dev->pci_dev->device.numa_node,
            .private_data_size = sizeof(struct rsp_asym_dev_private)
    };
    char name[RTE_CRYPTODEV_NAME_MAX_LEN];
    struct rte_cryptodev *cryptodev;
    struct rsp_asym_dev_private *priv;

    if(rsp_pci_dev->model==RSP_MODEL_S20)
    {
        //RSP S20 one bank fix 
        if(rsp_pci_dev->vf_num==0)
        {
            //PF
            qp_num = 2;
            RSP_LOG(INFO, "pf qp_num=%d ",qp_num);
        }else
        {
            //VF
            qp_num = 2;
            RSP_LOG(INFO, "vf qp_num=%d ",qp_num);
        }
    }else{
        //RSP S10 the same qp as rpu
        if(rsp_pci_dev->vf_num==0)
        {
            //PF
            qp_num = rsp_pci_dev->qp_pf;
            RSP_LOG(INFO, "pf qp_num=%d ",qp_num);
        }else
        {
            //VF
            qp_num = rsp_pci_dev->qp_vf;
            RSP_LOG(INFO, "vf qp_num=%d ",qp_num);
        }
    }
    init_params.max_nb_queue_pairs = qp_num;

    snprintf(name, RTE_CRYPTODEV_NAME_MAX_LEN, "%s_%s_%s", rsp_pci_dev->name, (rsp_pci_dev->model==RSP_MODEL_S10)? "s10":"s20", "asym");
    /* Populate subset device to use in cryptodev device creation */
    rsp_pci_dev->asym_rte_dev.driver = &cryptodev_rsp_asym_driver;
    rsp_pci_dev->asym_rte_dev.numa_node = rsp_pci_dev->pci_dev->device.numa_node;
    rsp_pci_dev->asym_rte_dev.devargs = NULL;

    cryptodev = rte_cryptodev_pmd_create(name,
            &(rsp_pci_dev->asym_rte_dev), &init_params);

    if (cryptodev == NULL)
        return -ENODEV;

    rsp_pci_dev->asym_rte_dev.name = cryptodev->data->name;
    cryptodev->driver_id = rsp_asym_driver_id;
    cryptodev->dev_ops = &crypto_rsp_asym_ops;

    cryptodev->enqueue_burst = rsp_asym_pmd_enqueue_op_burst;
    cryptodev->dequeue_burst = rsp_asym_pmd_dequeue_op_burst;

    cryptodev->feature_flags = RTE_CRYPTODEV_FF_ASYMMETRIC_CRYPTO |
            RTE_CRYPTODEV_FF_HW_ACCELERATED |
            //RTE_CRYPTODEV_FF_ASYM_SESSIONLESS |
            RTE_CRYPTODEV_FF_RSA_PRIV_OP_KEY_EXP|
            RTE_CRYPTODEV_FF_RSA_PRIV_OP_KEY_QT;
    priv = cryptodev->data->dev_private;
    priv->rsp_dev = rsp_pci_dev;
    rsp_pci_dev->asym_dev_priv = priv;

    priv->dev_id = cryptodev->data->dev_id;
    priv->rsp_dev_capabilities      = rsp_asym_capabilities;
    priv->dev_id                    = cryptodev->data->dev_id;
    priv->vf_num                    = rsp_pci_dev->vf_num;
    priv->model                     = rsp_pci_dev->model;
    priv->min_enq_burst_threshold   = 1;
    //while (1)
    //{
    //    if (rsp_dev_cmd_param[i].name == NULL)
    //        break;
    //    if (!strcmp(rsp_dev_cmd_param[i].name, ASYM_ENQ_THRESHOLD_NAME))
    //        priv->min_enq_burst_threshold =    rsp_dev_cmd_param[i].val;
    //    i++;
    //}
    if(rsp_pci_dev->model==RSP_MODEL_S20)
    {
        uint32_t bank_mask=0;
        /*
            Get using bank according to vf_num 
        */
        if(rsp_pci_dev->qp_pf == 0)
            bank_mask = (rsp_pci_dev->vf_num == 0x00) ? 0x00 : (1 << (rsp_pci_dev->vf_num - 0x80 ));
        else
            bank_mask = (rsp_pci_dev->vf_num == 0x00) ? 0x01 : (1 << (rsp_pci_dev->vf_num - 0x80 +1));

        for(i=0;i<RSP_NPUB_MAX_BANK;i++)
        {
            if(((bank_mask>>i) & 0x01) == 0x01)
            {
                RSP_REG_WRITE((uint8_t *)(rsp_pci_dev->pci_dev->mem_resource[0].addr) + BANK0_VF_NUM_ADDR + BANK_VF_NUM_ADDR_OFFSET*i, rsp_pci_dev->vf_num);
                priv->npub_bank=i;
                for(j=0;j<qp_num;j++)
                {
                    priv->qp_ids[j] = j;
                    priv->qp_num++;
                    RSP_LOG(DEBUG, "priv->npub_bank=%d priv->qp_ids[%d]=%d ",priv->npub_bank,j,priv->qp_ids[j]);
                }
            }
        }
    }else
    {
        //The same as rpu;
        if(rsp_pci_dev->vf_num==0)
        {
            //PF
            offset = 0;
        }else
        {
            //VF
            offset = rsp_pci_dev->qp_pf==0 ? 0 : (priv->vf_num-0x80)*rsp_pci_dev->qp_vf + rsp_pci_dev->qp_pf;
        }
        for(i=0; i<qp_num; i++)
        {
            priv->qp_ids[i] = (int8_t)(i+offset);
            priv->qp_num++;
            RSP_LOG(DEBUG, "priv->qp_ids[%d]=%d ",i, priv->qp_ids[i]);
        }
    }
    RSP_LOG(DEBUG, "Created RSP ASYM device %s as cryptodev instance %d", cryptodev->data->name, priv->dev_id);
    return 0;
}

int rsp_asym_dev_destroy(struct rsp_pci_device *rsp_pci_dev)
{
    struct rte_cryptodev *cryptodev;

    if (rsp_pci_dev == NULL)
        return -ENODEV;
    if (rsp_pci_dev->asym_dev_priv == NULL)
        return 0;
    /* free crypto device */
    cryptodev = rte_cryptodev_pmd_get_dev(rsp_pci_dev->asym_dev_priv->dev_id);
    //free QP
    rsp_asym_dev_close(cryptodev);
    rte_cryptodev_pmd_destroy(cryptodev);
    rsp_pci_dev->asym_rte_dev.name = NULL;
    rsp_pci_dev->asym_dev_priv = NULL;

    return 0;
}

static struct cryptodev_driver rsp_crypto_drv;
RTE_PMD_REGISTER_CRYPTO_DRIVER(rsp_crypto_drv,
        cryptodev_rsp_asym_driver,
        rsp_asym_driver_id);
