#include <rte_string_fns.h>
#include <rte_devargs.h>
#include <ctype.h>
#include "rsp_reg.h"
#include "rsp_device.h"
#include "rsp_sym_pmd.h"
#include "rsp_asym_pmd.h"
#include "rsp_bulk_pmd.h"
#include "rpu_fw_data.h"
static struct rsp_pci_device rsp_pci_dev[RSP_MAX_PCI_DEVICES];
static int rsp_total_pci_devices;

static const struct rte_pci_id pci_id_rsp_map[] = 
{
    {
       RTE_PCI_DEVICE(RSP_VENDER, RSP_DEVICE_S20),
    },
    {
       RTE_PCI_DEVICE(RSP_VENDER, RSP_DEVICE_S20_VF),
    },
    {
       RTE_PCI_DEVICE(RSP_VENDER, RSP_DEVICE_S10),
    },
    {
       RTE_PCI_DEVICE(RSP_VENDER, RSP_DEVICE_S10_VF),
    },
    {.device_id = 0},
};
//get rsp pci device 
static struct rsp_pci_device *rsp_pci_get_dev_by_id(uint8_t dev_id)
{
    return &rsp_pci_dev[dev_id];
}

static struct rsp_pci_device *rsp_pci_get_dev_by_name(const char *name)
{
    struct rsp_pci_device *dev;
    unsigned int i;

    if (name == NULL)
        return NULL;

    for (i = 0; i < RSP_MAX_PCI_DEVICES; i++)
    {
        dev = &rsp_pci_dev[i];
        if ((dev->attached == RSP_ATTACHED) && (strcmp(dev->name, name) == 0))
        return dev;
    }
    return NULL;
}

static uint8_t rsp_pci_get_free_device_id(void)
{
    uint8_t dev_id;
    for (dev_id = 0; dev_id < RSP_MAX_PCI_DEVICES; dev_id++) 
    {
        if (rsp_pci_dev[dev_id].attached == RSP_DETACHED)
        break;
    }
    return dev_id;
}

struct rsp_pci_device *rsp_get_rsp_dev_from_pci_dev(struct rte_pci_device *pci_dev)
{
    char name[RSP_DEV_NAME_MAX_LEN];

    rte_pci_device_name(&pci_dev->addr, name, sizeof(name));

    return rsp_pci_get_dev_by_name(name);
}
//devive paramters parse
static void rsp_dev_parse_cmd(const char *str, struct rsp_dev_cmd_param *cmd_param)
{
    int i = 0;
    const char *param;

    while (1) 
    {
        char value_str[4] = { };
        param = cmd_param[i].name;
        if (param == NULL)
            return;
        long value = 0;
        const char *arg = strstr(str, param);
        const char *arg2 = NULL;

        if (arg) 
        {
            arg2 = arg + strlen(param);
            if (*arg2 != '=') 
            {
                RSP_LOG(DEBUG, "parsing error '=' sign should immediately follow %s", param);
                arg2 = NULL;
            } else
                arg2++;
        } else
        {
            RSP_LOG(DEBUG, "%s not provided", param);
        }
        if (arg2)
        {
            int iter = 0;
            while (iter < 2) 
            {
                if (!isdigit(*(arg2 + iter)))
                    break;
                iter++;
            }
            if (!iter)
            {
                RSP_LOG(DEBUG, "parsing error %s" " no number provided",param);
            } else 
            {
                memcpy(value_str, arg2, iter);
                value = strtol(value_str, NULL, 10);
                if (value > MAX_QP_THRESHOLD_SIZE) 
                {
                    RSP_LOG(DEBUG, "Exceeded max size of"
                    " threshold, setting to %d",
                    MAX_QP_THRESHOLD_SIZE);
                    value = MAX_QP_THRESHOLD_SIZE;
                }
                RSP_LOG(DEBUG, "parsing %s = %ld", param, value);
            }
        }
        cmd_param[i].val = value;
        i++;
    }
}

struct rsp_pci_device *rsp_pci_device_setup(struct rte_pci_device *pci_dev, struct rsp_dev_cmd_param *cmd_param)
{
    struct rsp_pci_device *rsp_dev;
    uint8_t dev_id;
    char name[RSP_DEV_NAME_MAX_LEN];
    struct rte_devargs *devargs = pci_dev->device.devargs;

    //get dev name
    rte_pci_device_name(&pci_dev->addr, name, sizeof(name));
    snprintf(name+strlen(name), RSP_DEV_NAME_MAX_LEN-strlen(name), RSP_DEV_NAME_SUFFIX);
    if (rsp_pci_get_dev_by_name(name) != NULL) 
    {
        RSP_LOG(ERR, "RSP device with name %s already allocated!", name);
        return NULL;
    }

    dev_id = rsp_pci_get_free_device_id();
    if (dev_id == RSP_MAX_PCI_DEVICES) 
    {
        RSP_LOG(ERR, "Reached maximum number of RSP S20 devices");
        return NULL;
    }

    rsp_dev = rsp_pci_get_dev_by_id(dev_id);
    memset(rsp_dev, 0, sizeof(*rsp_dev));
    strlcpy(rsp_dev->name, name, RSP_DEV_NAME_MAX_LEN);
    rsp_dev->dev_id = dev_id;
    rsp_dev->pci_dev = pci_dev;
    if (devargs && devargs->drv_str)
        rsp_dev_parse_cmd(devargs->drv_str, cmd_param);
    //mark device as at attached
    rsp_dev->attached = RSP_ATTACHED;
    
    if ( (pci_dev->id.device_id == RSP_DEVICE_S20) || (pci_dev->id.device_id == RSP_DEVICE_S20_VF) )
    {
        RSP_LOG(DEBUG, "RSP S20");
        rsp_dev->model = RSP_MODEL_S20;
    }else
    {
        RSP_LOG(DEBUG, "RSP S10");
        rsp_dev->model = RSP_MODEL_S10;
    }
    rsp_total_pci_devices++;
    //get vf number
    if(pci_dev->mem_resource[0].addr)
    {
        rsp_dev->vf_num=*((uint32_t *)((uint8_t *)pci_dev->mem_resource[0].addr + VF_NUM_REG(rsp_dev->model)));
        uint32_t config=*((uint32_t *)((uint8_t *)pci_dev->mem_resource[0].addr + RSP_RING_INFO_REG));
        
        rsp_dev->qp_pf=(config>>8)&0xFF;
        rsp_dev->qp_vf=(config>>0)&0xFF;
        RSP_LOG(DEBUG, "RSP config=0x%08x",config);
        RSP_LOG(DEBUG, "RSP device(%x) %d allocated, name %s, total RSPs %d", rsp_dev->vf_num, rsp_dev->dev_id, rsp_dev->name, rsp_total_pci_devices);
        RSP_LOG(DEBUG, "RSP device qp_pf=%d qp_vf=%d",rsp_dev->qp_pf, rsp_dev->qp_vf);
        if(rsp_dev->vf_num == 0)
        {
            //PF download fw
            if(firmware_download((void *)pci_dev->mem_resource[0].addr, rpu_fw_data, sizeof(rpu_fw_data), rsp_dev->model) == 0)
            { 
                RSP_LOG(DEBUG, "Download success !");
            }else
            {
                RSP_LOG(DEBUG, "Download success ! (already)");
            }
        //if(!(config&0x80000000))
            {
                *((uint32_t *)((uint8_t *)pci_dev->mem_resource[0].addr + RSP_RING_INFO_REG)) = config|0x80000000;
                rsp_disable_rpu((void *)pci_dev->mem_resource[0].addr, rsp_dev->model);//
                RSP_REG_WRITE((uint8_t *)pci_dev->mem_resource[0].addr + RPU_ENDIAN_REG(rsp_dev->model), 0x03);
                RSP_REG_WRITE((uint8_t *)pci_dev->mem_resource[0].addr + RPU_START_REG(rsp_dev->model), 0x01);
                //just PF driver WR once
                //mask core 0
                *((uint32_t *)((uint8_t *)pci_dev->mem_resource[0].addr + RPU_CORE_MASK(rsp_dev->model))) = 0xFFFFFFFE;
                if(rsp_dev->model == RSP_MODEL_S20)
                    *((uint32_t *)((uint8_t *)pci_dev->mem_resource[0].addr + RSP_COMMOM_CFG3)) = 0x00800000;
            }
        }
    }
    if( (rsp_dev->qp_pf == 0) && (rsp_dev->qp_vf == 0) )
    {
        //not config ring info, use default value
        rsp_dev->qp_pf = RSP_DEFAULT_PF_RPU_QP_NUM;
        rsp_dev->qp_vf = RSP_DEFAULT_VF_RPU_QP_NUM;
    }
    return rsp_dev;
}

int rsp_pci_device_release(struct rte_pci_device *pci_dev)
{
    struct rsp_pci_device *rsp_dev;
    char name[RSP_DEV_NAME_MAX_LEN];

    if (pci_dev == NULL)
        return -EINVAL;

    rte_pci_device_name(&pci_dev->addr, name, sizeof(name));
    snprintf(name+strlen(name), RSP_DEV_NAME_MAX_LEN-strlen(name), RSP_DEV_NAME_SUFFIX);
    rsp_dev = rsp_pci_get_dev_by_name(name);
    if (rsp_dev != NULL)
    {
        /* Check that there are no service devs still on pci device */
        if (rsp_dev->sym_dev_priv != NULL)
            return -EBUSY;
        if (rsp_dev->asym_dev_priv != NULL)
            return -EBUSY;
        if (rsp_dev->bulk_dev_priv != NULL)
            return -EBUSY;
        rsp_dev->attached = RSP_DETACHED;
        rsp_total_pci_devices--;
    }
    RSP_LOG(DEBUG, "RSP device %s released, total RSPs %d", name, rsp_total_pci_devices);
    return 0;
}

static int rsp_pci_dev_destroy(struct rsp_pci_device *rsp_pci_dev, struct rte_pci_device *pci_dev)
{
    rsp_sym_dev_destroy(rsp_pci_dev);
    rsp_asym_dev_destroy(rsp_pci_dev);
    return rsp_pci_device_release(pci_dev);
}

static int rsp_pci_probe(struct rte_pci_driver *pci_drv __rte_unused, struct rte_pci_device *pci_dev)
{
    int sym_ret = 0, asym_ret = 0, bulk_ret = 0;
    int num_pmds_created = 0;
    struct rsp_pci_device *rsp_pci_dev;
    struct rsp_dev_cmd_param cmd_param[] = 
    {
        { SYM_ENQ_THRESHOLD_NAME,  0 },
        { ASYM_ENQ_THRESHOLD_NAME, 0 },
        { NULL, 0 },
    };
    printf("%s %d at %02x:%02x.%x \n",__FUNCTION__,__LINE__,pci_dev->addr.bus,
        pci_dev->addr.devid,
        pci_dev->addr.function);
    RSP_LOG(DEBUG, "Found RSP device at %02x:%02x.%x",
    pci_dev->addr.bus,
    pci_dev->addr.devid,
    pci_dev->addr.function);
    if(rte_eal_process_type() != RTE_PROC_PRIMARY)
    {
        return 0;
    }
    rsp_pci_dev = rsp_pci_device_setup(pci_dev, cmd_param);
    if (rsp_pci_dev == NULL)
        return -ENODEV;
    if( (rsp_pci_dev->qp_pf ==0) && (rsp_pci_dev->vf_num == 0) )
        goto exit;// PF no ring 
    if( (rsp_pci_dev->qp_vf ==0) && (rsp_pci_dev->vf_num != 0) )
        goto exit;// VF no ring 
    sym_ret = rsp_sym_dev_create(rsp_pci_dev, cmd_param);
    if (sym_ret == 0)
        num_pmds_created++;
    else
        RSP_LOG(WARNING, "Failed to create RSP SYM PMD on device %s", rsp_pci_dev->name);

    //asym_ret = rsp_asym_dev_create(rsp_pci_dev, cmd_param);
    //if (asym_ret == 0)
    //    num_pmds_created++;
    //else
    //    RSP_LOG(WARNING, "Failed to create RSP ASYM PMD on device %s", rsp_pci_dev->name);
    if (rsp_pci_dev->model == RSP_MODEL_S20)
    {
        bulk_ret = rsp_bulk_dev_create(rsp_pci_dev, cmd_param);
        if (bulk_ret == 0)
            num_pmds_created++;
        else
            RSP_LOG(WARNING, "Failed to create RSP BULK PMD on device %s", rsp_pci_dev->name);
    }
    if (num_pmds_created == 0)
        rsp_pci_dev_destroy(rsp_pci_dev, pci_dev);
    RSP_LOG(DEBUG, "name:%s", rsp_pci_dev->name);
    return 0;
exit:
    rsp_pci_dev_destroy(rsp_pci_dev, pci_dev);
    return 0;
}

static int rsp_pci_remove(struct rte_pci_device *pci_dev)
{
    struct rsp_pci_device *rsp_pci_dev;

    if (pci_dev == NULL)
        return -EINVAL;

    rsp_pci_dev = rsp_get_rsp_dev_from_pci_dev(pci_dev);
    if (rsp_pci_dev == NULL)
        return 0;

    return rsp_pci_dev_destroy(rsp_pci_dev, pci_dev);
}

static struct rte_pci_driver rte_rsp_pmd = {
    .id_table   = pci_id_rsp_map,
    .drv_flags  = RTE_PCI_DRV_NEED_MAPPING,
    .probe      = rsp_pci_probe,
    .remove     = rsp_pci_remove
};

__rte_weak int
rsp_sym_dev_create(struct rsp_pci_device *rsp_pci_dev __rte_unused,
struct rsp_dev_cmd_param *rsp_dev_cmd_param __rte_unused)
{
    return 0;
}

__rte_weak int
rsp_asym_dev_create(struct rsp_pci_device *rsp_pci_dev __rte_unused,
struct rsp_dev_cmd_param *rsp_dev_cmd_param __rte_unused)
{
    return 0;
}

__rte_weak int
rsp_sym_dev_destroy(struct rsp_pci_device *rsp_pci_dev __rte_unused)
{
    return 0;
}

__rte_weak int
rsp_asym_dev_destroy(struct rsp_pci_device *rsp_pci_dev __rte_unused)
{
    return 0;
}

RTE_PMD_REGISTER_PCI(RSP_PCI_NAME, rte_rsp_pmd);
RTE_PMD_REGISTER_PCI_TABLE(RSP_PCI_NAME, pci_id_rsp_map);
