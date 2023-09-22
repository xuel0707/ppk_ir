#include "rsp_common.h"
#include "rsp_device.h"
#include "rsp_logs.h"
void rsp_stats_get(struct rsp_pci_device *rsp_dev, int count, struct rsp_common_stats *stats, enum rsp_service_type type)
{
    struct rsp_QP **QP=NULL;
    int i;
    if (stats == NULL || rsp_dev == NULL ) 
    {
        RSP_LOG(ERR, "invalid param: stats %p, rsp_dev %p", stats, rsp_dev);
        return;
    }
    switch(type)
    {
        case RSP_SERVICE_ASYMMETRIC:
            QP = rsp_dev->pub_qps_in_use;
            break;
        case RSP_SERVICE_SYMMETRIC:
            QP = rsp_dev->rpu_qps_in_use;
            break;
        case RSP_SERVICE_BULK:
            QP = rsp_dev->bulk_qps_in_use;
            break;
        default:
            RSP_LOG(ERR,"invalied service type");
            break;
    }
    for (i = 0; i < count; i++) 
    {
        if (QP[i] == NULL) 
        {
            continue;
        }

        stats->enqueued_count         += QP[i]->stats.enqueued_count;
        stats->dequeued_count         += QP[i]->stats.dequeued_count;
        stats->enqueue_err_count      += QP[i]->stats.enqueue_err_count;
        stats->dequeue_err_count      += QP[i]->stats.dequeue_err_count;
        stats->threshold_hit_count    += QP[i]->stats.threshold_hit_count;
        RSP_LOG(DEBUG, "Threshold was used for QP %d %"PRIu64" times", i, stats->threshold_hit_count);
    }
}

void rsp_stats_reset(struct rsp_pci_device *rsp_dev, int count, enum rsp_service_type type)
{
    struct rsp_QP **QP=NULL;
    int i;
    if (rsp_dev == NULL ) 
    {
        RSP_LOG(ERR, "invalid param: rsp_dev %p", rsp_dev);
        return;
    }
    switch(type)
    {
        case RSP_SERVICE_ASYMMETRIC:
            QP = rsp_dev->pub_qps_in_use;
            break;
        case RSP_SERVICE_SYMMETRIC:
            QP = rsp_dev->rpu_qps_in_use;
            break;
        case RSP_SERVICE_BULK:
            QP = rsp_dev->bulk_qps_in_use;
            break;
        default:
            RSP_LOG(ERR,"invalied service type");
            break;
    }
    for (i = 0; i < count; i++) 
    {
        if (QP[i] == NULL) 
        {
            continue;
        }
        memset(&(QP[i]->stats), 0, sizeof(QP[i]->stats));
    }
}
int firmware_download(void *bar0_base, uint8_t *data, int len, int model)
{
    int core_id=0;
    int start=0;
    int end=0;
    int i=0;
    int same_num=0;
    void *addr = NULL;
    unsigned int *reg_addr = NULL;
    uint32_t temp;
    uint8_t *p=data;
    uint8_t *q=NULL;
    int left = 0;
    uint32_t frm_addr=0;
    uint32_t frm_data=0;
    if(len <= (16*34) || (len%8 != 0 )) return -1;
    
    RSP_LOG(DEBUG, "Version:%s\n", p);
    p+=16;
    RSP_LOG(DEBUG, "Time:%s\n", p);
    p+=16;
    RSP_LOG(DEBUG, "Algorithms:\n");
    for (i = 0; i < 32; i++) 
    {
        if (0x0 != *((uint32_t *)p)) 
        RSP_LOG(DEBUG, "\t %s\n", p);
        p+=16;
    }
    
    left = len - 16*34;
    if(model == RSP_MODEL_S20)
    {
        start=1;
        end =48;
    }else if(model == RSP_MODEL_S10)
    {
        start=0;
        end =32;
    }else
    {
        RSP_LOG(ERR, "Invalid model=%d \n", model);
    }
#if 0
    
    for(core_id=start; core_id<end; core_id++)
    {
        q=p;    
        for(i=0;i<left/8;i++)
        {
            frm_addr = q[0]<<24 | q[1]<<16 | q[2]<<8 | q[3];
            frm_data = q[4]<<24 | q[5]<<16 | q[6]<<8 | q[7];
            //printk("addr=0x%08x data=0x%08x\n",frm_addr,frm_data);
            RSP_REG_WRITE((uint64_t)bar0_base + frm_addr - 0x210000 + core_id*0x10000 ,0);
            q+=8;
        }
    }
#endif

    // XV core0 sbox
    core_id = 0;
    {
        q=p;    
        for(i=0;i<left/8;i++)
        {
            frm_addr = (q[0]<<24 | q[1]<<16 | q[2]<<8 | q[3]) & 0x0000ffff;
            if((frm_addr >= 0x2000) && (frm_addr <= 0x3ffc))
            {
                frm_data = q[4]<<24 | q[5]<<16 | q[6]<<8 | q[7];
                if(RSP_REG_READ((uint64_t)bar0_base + frm_addr + core_id*0x10000 ) == frm_data)
                    same_num++;
                else
                    RSP_REG_WRITE((uint64_t)bar0_base + frm_addr + core_id*0x10000 ,frm_data);
            }
            q+=8;
        }
    }

    for(core_id=start; core_id<end; core_id++)
    {
        q=p;    
        for(i=0;i<left/8;i++)
        {
            frm_addr = (q[0]<<24 | q[1]<<16 | q[2]<<8 | q[3]) & 0x0000ffff;
            frm_data = q[4]<<24 | q[5]<<16 | q[6]<<8 | q[7];
            //printk("addr=0x%08x data=0x%08x\n",frm_addr,frm_data);
            if(RSP_REG_READ((uint64_t)bar0_base + frm_addr + core_id*0x10000 ) == frm_data)
                same_num++;
            else
                RSP_REG_WRITE((uint64_t)bar0_base + frm_addr + core_id*0x10000 ,frm_data);
            q+=8;
        }
    }
    //printk("same_num=%d left/8=%d (end-start)=%d \n",same_num,left/8,(end-start));
    if(same_num >= (left/8 * (end-start))) 
        return 1;
    else
        return 0;
}
int rsp_get_dgst_len(uint32_t algo)
{
    int hash_block_len=0;
    switch(algo)
    {
        case  RSP_HASH_ALGO_SHA1:
            hash_block_len = 20;
            break;
        case  RSP_HASH_ALGO_SHA224:
            hash_block_len = 28;
            break;
        case  RSP_HASH_ALGO_SHA256:
            hash_block_len = 32;
            break;        
        case  RSP_HASH_ALGO_SHA384:
            hash_block_len = 48;
            break;
        case  RSP_HASH_ALGO_SHA512:
            hash_block_len = 64;
            break;
        case  RSP_HASH_ALGO_SM3:
            hash_block_len = 32;
            break;
        case  RSP_HASH_ALGO_MD5:
            hash_block_len = 32;
            break;
        default:
            break;
    }   
    return hash_block_len;
}
int rsp_get_block_len(uint32_t algo)
{
    int hash_block_len=0;
    switch(algo)
    {
        case  RSP_HASH_ALGO_SHA1:
            hash_block_len = 64;
            break;
        case  RSP_HASH_ALGO_SHA224:
            hash_block_len = 64;
            break;
        case  RSP_HASH_ALGO_SHA256:
            hash_block_len = 64;
            break;        
        case  RSP_HASH_ALGO_SHA384:
            hash_block_len = 128;
            break;
        case  RSP_HASH_ALGO_SHA512:
            hash_block_len = 128;
            break;
        case  RSP_HASH_ALGO_SM3:
            hash_block_len = 64;
            break;
        case  RSP_HASH_ALGO_MD5:
            hash_block_len = 32;
            break;
        default:
            break;
    }   
    return hash_block_len;
}

int rsp_get_hmac_len(uint32_t algo)
{
    int hash_block_len=0;
    switch(algo)
    {
        case  RSP_HASH_ALGO_SHA1:
            hash_block_len = 20;
            break;

        case  RSP_HASH_ALGO_SHA256:
            hash_block_len = 32;
            break;        
        case  RSP_HASH_ALGO_SHA384:
            hash_block_len = 48;
            break;
        case  RSP_HASH_ALGO_SHA512:
            hash_block_len = 64;
            break;
        case  RSP_HASH_ALGO_SM3:
            hash_block_len = 32;
            break;
        default:
            break;
    }   
    return hash_block_len;
}
static uint32_t sha1_init_vaule[5] =
{
    0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210, 0xF0E1D2C3 
};

static uint32_t sha224_init_vaule[8] =
{
    0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939, 
    0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};

static uint32_t sha256_init_vaule[8] =
{
    0x67e6096a, 0x85ae67bb, 0x72f36e3c, 0x3af54fa5,
    0x7f520e51, 0x8c68059b, 0xabd9831f, 0x19cde05b
};
uint8_t sha384_init_vaule[64]={
    0xcb, 0xbb, 0x9d, 0x5d, 0xc1, 0x05, 0x9e, 0xd8,
    0x62, 0x9a, 0x29, 0x2a, 0x36, 0x7c, 0xd5, 0x07,
    0x91, 0x59, 0x01, 0x5a, 0x30, 0x70, 0xdd, 0x17,
    0x15, 0x2f, 0xec, 0xd8, 0xf7, 0x0e, 0x59, 0x39,
    0x67, 0x33, 0x26, 0x67, 0xff, 0xc0, 0x0b, 0x31,
    0x8e, 0xb4, 0x4a, 0x87, 0x68, 0x58, 0x15, 0x11,
    0xdb, 0x0c, 0x2e, 0x0d, 0x64, 0xf9, 0x8f, 0xa7,
    0x47, 0xb5, 0x48, 0x1d, 0xbe, 0xfa, 0x4f, 0xa4
};
uint8_t sha512_init_vaule[64]={
    0x6a, 0x09, 0xe6, 0x67, 0xf3, 0xbc, 0xc9, 0x08,
    0xbb, 0x67, 0xae, 0x85, 0x84, 0xca, 0xa7, 0x3b,
    0x3c, 0x6e, 0xf3, 0x72, 0xfe, 0x94, 0xf8, 0x2b,
    0xa5, 0x4f, 0xf5, 0x3a, 0x5f, 0x1d, 0x36, 0xf1,
    0x51, 0x0e, 0x52, 0x7f, 0xad, 0xe6, 0x82, 0xd1,
    0x9b, 0x05, 0x68, 0x8c, 0x2b, 0x3e, 0x6c, 0x1f,
    0x1f, 0x83, 0xd9, 0xab, 0xfb, 0x41, 0xbd, 0x6b,
    0x5b, 0xe0, 0xcd, 0x19, 0x13, 0x7e, 0x21, 0x79
};
static uint32_t sm3_init_vaule[8] =
{
   // 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600,
   // 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e
      0x6f168073, 0xb9b21449, 0xd7422417, 0x00068ada,
      0xbc306fa9, 0xaa383116, 0x4dee8de3, 0x4e0efbb0
};
uint8_t *rsp_get_init(int algo)
{
    uint8_t *p=NULL;
    switch(algo)
    {
        case  RSP_HASH_ALGO_SHA1:
            p = (uint8_t *)sha1_init_vaule;
            break;
        case  RSP_HASH_ALGO_SHA224:
            p = (uint8_t *)sha224_init_vaule;
            break;
        case  RSP_HASH_ALGO_SHA256:
            p = (uint8_t *)sha256_init_vaule;
            break;
        case  RSP_HASH_ALGO_SHA384:
            p = (uint8_t *)sha384_init_vaule;
            break;
        case  RSP_HASH_ALGO_SHA512:
            p = (uint8_t *)sha512_init_vaule;
            break;
        case  RSP_HASH_ALGO_SM3:
            p = (uint8_t *)sm3_init_vaule;
            break;
        case  RSP_HASH_ALGO_MD5:
            p = (uint8_t *)sm3_init_vaule;
            break;
    }
    return p;
}
uint8_t rsp_get_init_len(int algo)
{
    uint8_t len=0;
    switch(algo)
    {
        case  RSP_HASH_ALGO_SHA1:
            len = 20;
            break;
        case  RSP_HASH_ALGO_SHA224:
            len = 32;
            break;
        case  RSP_HASH_ALGO_SHA256:
            len = 32;
            break;
        case  RSP_HASH_ALGO_SHA384:
            len = 64;
            break;
        case  RSP_HASH_ALGO_SHA512:
            len = 64;
            break;
        case  RSP_HASH_ALGO_SM3:
            len = 32;
            break;
        case  RSP_HASH_ALGO_MD5:
            len = 32;
            break;
    }
    return len;
}

void data_swap(unsigned char *buff, int len) 
{
    int i = 0, start = 0;
    unsigned char tmp = 0;
    /*
    for (start = len -1; start >= 0; start--) {
        if (0x0 != *(buff + start)) break;
    }
    */

    start = len -1;

    for (i = 0; i < ((start+1)/2); i++) {
        tmp = *(buff + i);
        *(buff + i) = *(buff + (start - i));
        *(buff + (start - i)) = tmp;
    };
}
int hash_padding(uint8_t *data, int data_size, int block_size, int total_len)
{
    int pad_len=0;
    uint64_t *q;
    
    data[data_size]=0x80;
    if(data_size+9 <= block_size)
    {
        memset(data+data_size+1, 0x00, block_size-data_size-9);
        pad_len = (block_size-data_size);
    }else
    {
        memset(data + data_size + 1, 0, block_size - data_size - 1);
        data += block_size;
        memset(data,0x00,block_size-8);
        
        pad_len = (block_size*2-data_size);
    }
    
    q  = (uint64_t *)(data+(block_size-8));
    *q = (total_len*8);
    data_swap((void *)q,8);
    
    return pad_len;
}