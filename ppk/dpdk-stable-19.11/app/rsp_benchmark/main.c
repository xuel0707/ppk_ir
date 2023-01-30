#include <stdio.h>
#include <unistd.h>
//#include <rte_cycles_64.h>
#include <rte_malloc.h>
#include <rte_random.h>
#include <rte_eal.h>
#include <rte_cryptodev.h>
#include "vector.h"
#include "crypto_api.h"
#define MAX_MBUF_SIZE       4096
#define RSP_MAX_DEPTH       512
#define RSP_CACHE_SIZE      128
int32_t data_offset=2;
int esp_len=24;
int init_crypto_device(char *name, int *dev_id, int *socket_id);
uint8_t aes_p_data[128]={0x12};
uint8_t tag_digest[16]={0};
typedef struct rsp_context_s{
    char *dev_name;
    int dev_id;
    int socket_id;
    int lcore;
    int qp_id;
    int op_type;
    int op;
    int nb_pkt;
    int burst;
    int size;
    int check_rst;
}rsp_context_t;
#define rsp_log_dump(buf, len, format, args...) \
{ \
    int loop11;\
    uint8_t *temp = (uint8_t *)(buf); \
    { \
        for (loop11 = 0; loop11 < len; loop11++) \
        { \
            if (((loop11 % 16) == 0)) \
                printf("\n  "); \
            printf("%02X ", temp[loop11]); \
        } \
        printf("\n\n"); \
    } \
}
enum rsp_op_e{
     GET_DEV_INFO       = 0,    
     AES_128_CBC        ,    
     AES_128_GCM        ,    
     AES_256_CBC        ,    
     AES_256_GCM        ,    
     SM4_CBC            ,    
     SM4_GCM            ,    
     SM1_CBC            ,    
     RSA_1024_SIGN      ,    
     RSA_1024_SIGN_CRT  ,    
     RSA_1024_VERI      ,    
     RSA_1024_ENC       ,   
     RSA_1024_DEC       ,   
     RSA_2048_SIGN      ,   
     RSA_2048_SIGN_CRT  ,   
     RSA_2048_VERI      ,   
     RSA_2048_ENC       ,   
     RSA_2048_DEC       ,   
     ECC_256R1_SIGN     ,   
     ECC_256R1_VERIFY   ,   
     ECC_SM2_SIGN       ,   
     ECC_SM2_VERIFY     ,   
     BULK_TRNG_GEN      ,
     BULK_SHA1_HMAC     ,
     BULK_SHA256_HMAC   ,
     BULK_SM3_HMAC      ,
     AES_128_CBC_SHA1   ,
     AES_128_CBC_SHA256 ,
     AES_256_CBC_SHA1   ,
     AES_256_CBC_SHA256 ,
     SM4_SM3            ,
     SM1_SM3            ,
     SHA1_AES_128_CBC   ,
     SHA256_AES_128_CBC ,
     SHA1_AES_256_CBC   ,
     SHA256_AES_256_CBC ,
     SM3_SM4            ,
     SM3_SM1            ,
     ALGO_MAX            ,
};
const char *algo_str[]={
     "GET_DEV_INFO       ",
     "AES_128_CBC        ",
     "AES_128_GCM        ",
     "AES_256_CBC        ",
     "AES_256_GCM        ",
     "SM4_CBC            ",
     "SM4_GCM            ",
     "SM1_CBC            ",
     "RSA_1024_SIGN      ",
     "RSA_1024_SIGN_CRT  ",
     "RSA_1024_VERI      ",
     "RSA_1024_ENC       ",
     "RSA_1024_DEC       ",
     "RSA_2048_SIGN      ",
     "RSA_2048_SIGN_CRT  ",
     "RSA_2048_VERI      ",
     "RSA_2048_ENC       ",
     "RSA_2048_DEC       ",
     "ECC_256R1_SIGN     ",
     "ECC_256R1_VERIFY   ",
     "ECC_SM2_SIGN       ",
     "ECC_SM2_VERIFY     ",
     "BULK_TRNG_GEN      ",
     "SHA1_HMAC          ",
     "SHA256_HMAC        ",
     "SM3_HMAC           ",
     "AES_128_CBC_SHA1-HMAC   ",
     "AES_128_CBC_SHA256-HMAC ",
     "AES_256_CBC_SHA1-HMAC   ",
     "AES_256_CBC_SHA256-HMAC ",
     "SM4_SM3-HMAC            ",
     "SM1_SM3-HMAC            ",
     "  SHA1-HMAC_AES_128_CBC ",
     "SHA256-HMAC_AES_128_CBC ",
     "  SHA1-HMAC_AES_256_CBC ",
     "SHA256-HMAC_AES_256_CBC ",
     "   SM3-HMAC_SM4         ",
     "   SM3-HMAC_SM1         ",
     "               ",
     };
     
void usage_print(void);
void random_data(void *data, int len);
int rsp_cipher_running(void *param);
int rsp_rsa_runing(void *param);


void usage_print(void)
{
    int i=0;
    printf("uasge: <op> <count> <burst> <len> <device name> <check result>\n");
    for(i=0; i<ALGO_MAX; i++)
    {
        printf("        %s : %d \n",algo_str[i], GET_DEV_INFO+i);
    }
}
void random_data(void *data, int len)
{
    uint64_t *p = (uint64_t *)data;
    int i=0;
    for(i=0; i < (len/sizeof(uint64_t)); i++)
    {
        p[i] = rte_rand();
    }
}
int rsp_cipher_running(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int algo;
    int mode;
    int dir;
    int nb_sessions=1;
    int sess_size=0;

    uint8_t  *iv_data;
    uint16_t iv_offset;
    uint16_t iv_length;

    uint8_t  *key_data;
    uint16_t key_length;
    
    uint8_t  *aad_data;
    uint16_t aad_length;
    
    uint8_t *in_data;
    uint32_t in_data_len;
    uint8_t *out_data;

    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_cryptodev_sym_session *sess[1];
    struct rte_crypto_sym_xform xform;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;

    rsp_context_t *ctx=(rsp_context_t *)param;
    
    switch(ctx->op)
    {
        case AES_128_CBC  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;

            break;
        case AES_128_GCM  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_GCM;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 12;
            iv_data    = cipher_iv;
            aad_length = 16;
            aad_data   = cipher_aad;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type           = RTE_CRYPTO_SYM_XFORM_AEAD;
            xform.next           = NULL;
            xform.aead.algo      = RTE_CRYPTO_AEAD_AES_GCM;
            xform.aead.op        = dir;
            xform.aead.iv.offset = iv_offset;
            xform.aead.iv.length = iv_length;
            xform.aead.key.data  = key_data;
            xform.aead.key.length= key_length;
            xform.aead.aad_length=16;
            xform.aead.digest_length=16;
                
            break;
        case AES_256_CBC  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 32;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;

            break;
        case AES_256_GCM  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_GCM;
            key_data   = cipher_key;
            key_length = 32;
            iv_length  = 12;
            iv_data    = cipher_iv;
            aad_length = 16;
            aad_data   = cipher_aad;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type           = RTE_CRYPTO_SYM_XFORM_AEAD;
            xform.next           = NULL;
            xform.aead.algo      = RTE_CRYPTO_AEAD_AES_GCM;
            xform.aead.op        = dir;
            xform.aead.iv.offset = iv_offset;
            xform.aead.iv.length = iv_length;
            xform.aead.key.data  = key_data;
            xform.aead.key.length= key_length;
            xform.aead.aad_length=16;
            xform.aead.digest_length=16;

            break;
        case SM4_CBC      :
            algo = CIPHER_SM4;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = 0xFF;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;
            break;
        case SM4_GCM      :
            algo = CIPHER_SM4;
            mode = CIPHER_MODE_GCM;
            key_data   = cipher_key;
            key_length = 32;
            iv_length  = 12;
            iv_data    = cipher_iv;
            aad_length = 16;
            aad_data   = cipher_aad;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type           = RTE_CRYPTO_SYM_XFORM_AEAD;
            xform.next           = NULL;
            xform.aead.algo      = 0xFF;
            xform.aead.op        = dir;
            xform.aead.iv.offset = iv_offset;
            xform.aead.iv.length = iv_length;
            xform.aead.key.data  = key_data;
            xform.aead.key.length= key_length;
            xform.aead.aad_length=16;
            xform.aead.digest_length=16;

            break;
        case SM1_CBC      :
            algo = 0xFF;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = 0xFE;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;
            break;
    }
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_cryptodev_sym_session_pool_create(mp_name, 
                                nb_sessions,
                                0,
                                0,
                                1024,
                                ctx->socket_id);
    sess_size = rte_cryptodev_sym_get_private_session_size(ctx->dev_id);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                                nb_sessions,
                                sess_size,
                                0, 0, NULL, NULL, NULL,
                                NULL, ctx->socket_id,
                                0);
    //create session & init
    sess[0] = rte_cryptodev_sym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "Session create failed !\n");
    }
    
    ret=rte_cryptodev_sym_session_init(ctx->dev_id, sess[0], &xform, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "sym_sess init failed ret=%d !\n",ret);
        return 0;
    }
    
    struct rte_mempool *op_pool;
    struct rte_mempool *mbuf_pool;
    struct rte_mbuf *in_mbufs[RSP_MAX_DEPTH];//  = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);
    struct rte_mbuf *out_mbufs[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "mbuf_pool_%u", ctx->lcore);
    mbuf_pool = rte_pktmbuf_pool_create(mp_name,
                                RSP_MAX_DEPTH*2,//in / out
                                0,
                                0,
                                MAX_MBUF_SIZE,//data room size include head(128)
                                ctx->socket_id);
    if(mbuf_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "mbuf_pool alloc failed!\n");
    }

    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_SYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        64,//use for iv and aad
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "op_pool alloc failed!\n");
    }

    struct rte_crypto_sym_op *sym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;
//    printf("ctx->nb_pkt=%d burst=%d \n",ctx->nb_pkt,burst);
//    printf("DEBUG total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);
    
    //prepare compare data
    in_data_len = ctx->size;
    in_data = rte_zmalloc("src buf", in_data_len, 0);
    out_data = rte_zmalloc("dst buf", in_data_len+32, 0);//more 32 byte for tag
    random_data(in_data, in_data_len);
    //memset(in_data,0x34,in_data_len);
    if(algo != 0xFF)
    {//skip some unsupport algo
        ret=crypto_cipher(algo, mode, 1/*enc*/, key_data, key_length, iv_data, iv_length, aad_data, aad_length, in_data, in_data_len, out_data, out_data+in_data_len/*for tag*/);
        if( ret == CRYPTO_RET_SUCCESS)
        {
#if 0
            rsp_log_dump(out_data,in_data_len,"SW out");
            if(mode == CIPHER_MODE_GCM)
                rsp_log_dump(out_data+in_data_len,16,"SW tag");
#endif
        }else{
            rte_exit(EXIT_FAILURE, "Compare data prepare failed! ret=%d \n",ret);
        }
    }
    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))//until all pkt enq->deq
    {
        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        if(once != 0)
        {
            ret=0;
            //alloc mbuf fro pool
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, in_mbufs, once);
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, out_mbufs, once);
            if(ret != 0)
            {
                rte_exit(EXIT_FAILURE, "mbuf alloc failed!\n");
            }

            uint8_t *ptr;
            for(i=0;i<once;i++)
            {
                ptr = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *);
                memcpy(ptr+data_offset,in_data, in_data_len);
            }
            
            //just alloc enq op from pool
            rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_SYMMETRIC, enqueued_ops, once);
            //setup all ops
            for (i = 0; i < once; i++) 
            {
                enqueued_ops[i]->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;
                rte_crypto_op_attach_sym_session(enqueued_ops[i], sess[0]);
                //set IV
                uint8_t *iv_ptr = rte_crypto_op_ctod_offset(enqueued_ops[i], uint8_t *,iv_offset);
                memcpy(iv_ptr, iv_data, iv_length);
                sym_op = enqueued_ops[i]->sym;
                sym_op->m_src = in_mbufs[i];
                sym_op->m_dst = out_mbufs[i];
                if(xform.type == RTE_CRYPTO_SYM_XFORM_CIPHER)
                {
                    sym_op->cipher.data.length = in_data_len;
                    sym_op->cipher.data.offset = data_offset;
                }else{
                    sym_op->aead.data.length = in_data_len;
                    sym_op->aead.data.offset = data_offset;
                    sym_op->aead.digest.data = tag_digest;
                    sym_op->aead.aad.data    = cipher_aad;
                }
            }
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        //printf("Enqueue ops %d \n",ret);
        for(i=(ret);i<once;i++)
        {
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_src);
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_dst);
            //free enq failed ops
            rte_crypto_op_free(enqueued_ops[i]);
        }
        failed_enq += (once-ret);
        total_pkt_need_enqueue-=ret;
        
        ret=0;
        //deq ops
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
//            printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {

                struct rte_mbuf *mbuf;
                mbuf=dequeued_ops[i]->sym->m_dst;
                uint8_t *ptr = rte_pktmbuf_mtod(mbuf, uint8_t *);
#if 0
                rsp_log_dump(ptr,in_data_len,"dst");
                if(mode == CIPHER_MODE_GCM)
                     rsp_log_dump(dequeued_ops[i]->sym->aead.digest.data,16,"tag");
#endif

                if(ctx->check_rst)
                {
                    if ( (memcmp(ptr+data_offset, out_data, in_data_len) != 0) )
                    {
                        rsp_log_dump(ptr+data_offset,in_data_len,"HW:");
                        rsp_log_dump(out_data,in_data_len,"SW:");
                        rte_exit(EXIT_FAILURE, "cipher failed!\n");
                    }
                    
                    if ( (mode == CIPHER_MODE_GCM) )
                    {
                        if ( (memcmp(dequeued_ops[i]->sym->aead.digest.data, out_data+in_data_len, 16) != 0) )
                        {
                            rsp_log_dump(dequeued_ops[i]->sym->aead.digest.data, 16, "HW tag");
                            rsp_log_dump(out_data+in_data_len, 16, "SW tag");
                            rte_exit(EXIT_FAILURE, "tag failed!\n");
                        }
                    }
                }
                //free all bufs
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_src);
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_dst);
                rte_crypto_op_free(dequeued_ops[i]);
            }

            total_pkt_need_dequeue+=ret;

        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);
    rte_mempool_free(mbuf_pool);
    rte_cryptodev_sym_session_clear(ctx->dev_id, sess[0]);
    rte_cryptodev_sym_session_free(sess[0]);
    return 0;
}
int rsp_cipher_running1(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int algo;
    int mode;
    int dir;
    int nb_sessions=1;
    int sess_size=0;

    uint8_t  *iv_data;
    uint16_t iv_offset;
    uint16_t iv_length;

    uint8_t  *key_data;
    uint16_t key_length;
    
    uint8_t  *aad_data;
    uint16_t aad_length;
    
    uint8_t *in_data;
    uint32_t in_data_len;
    uint8_t *out_data;

    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_cryptodev_sym_session *sess[1];
    struct rte_crypto_sym_xform xform;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;

    rsp_context_t *ctx=(rsp_context_t *)param;
    
    switch(ctx->op)
    {
        case AES_128_CBC  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;

            break;
        case AES_128_GCM  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_GCM;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 12;
            iv_data    = cipher_iv;
            aad_length = 16;
            aad_data   = cipher_aad;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type           = RTE_CRYPTO_SYM_XFORM_AEAD;
            xform.next           = NULL;
            xform.aead.algo      = RTE_CRYPTO_AEAD_AES_GCM;
            xform.aead.op        = dir;
            xform.aead.iv.offset = iv_offset;
            xform.aead.iv.length = iv_length;
            xform.aead.key.data  = key_data;
            xform.aead.key.length= key_length;
            xform.aead.aad_length=16;
            xform.aead.digest_length=16;
                
            break;
        case AES_256_CBC  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 32;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;

            break;
        case AES_256_GCM  :
            algo = CIPHER_AES;
            mode = CIPHER_MODE_GCM;
            key_data   = cipher_key;
            key_length = 32;
            iv_length  = 12;
            iv_data    = cipher_iv;
            aad_length = 16;
            aad_data   = cipher_aad;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type           = RTE_CRYPTO_SYM_XFORM_AEAD;
            xform.next           = NULL;
            xform.aead.algo      = RTE_CRYPTO_AEAD_AES_GCM;
            xform.aead.op        = dir;
            xform.aead.iv.offset = iv_offset;
            xform.aead.iv.length = iv_length;
            xform.aead.key.data  = key_data;
            xform.aead.key.length= key_length;
            xform.aead.aad_length=16;
            xform.aead.digest_length=16;

            break;
        case SM4_CBC      :
            algo = CIPHER_SM4;
            mode = CIPHER_MODE_CBC;
            key_data   = cipher_key;
            key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = 0xFF;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = key_data;
            xform.cipher.key.length= key_length;
            break;
        case SM4_GCM      :
            algo = CIPHER_SM4;
            mode = CIPHER_MODE_GCM;
            key_data   = cipher_key;
            key_length = 32;
            iv_length  = 12;
            iv_data    = cipher_iv;
            aad_length = 16;
            aad_data   = cipher_aad;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type           = RTE_CRYPTO_SYM_XFORM_AEAD;
            xform.next           = NULL;
            xform.aead.algo      = 0xFF;
            xform.aead.op        = dir;
            xform.aead.iv.offset = iv_offset;
            xform.aead.iv.length = iv_length;
            xform.aead.key.data  = key_data;
            xform.aead.key.length= key_length;
            xform.aead.aad_length=16;
            xform.aead.digest_length=16;

            break;
    }
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_cryptodev_sym_session_pool_create(mp_name, 
                                nb_sessions,
                                0,
                                0,
                                1024,
                                ctx->socket_id);
    sess_size = rte_cryptodev_sym_get_private_session_size(ctx->dev_id);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                                nb_sessions,
                                sess_size,
                                0, 0, NULL, NULL, NULL,
                                NULL, ctx->socket_id,
                                0);
    //create session & init
    sess[0] = rte_cryptodev_sym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "Session create failed !\n");
    }
    
    ret=rte_cryptodev_sym_session_init(ctx->dev_id, sess[0], &xform, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "sym_sess init failed ret=%d !\n",ret);
        return 0;
    }

    struct rte_mempool *op_pool;
    struct rte_mempool *mbuf_pool;
    struct rte_mbuf *in_mbufs[RSP_MAX_DEPTH];//  = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);
    struct rte_mbuf *out_mbufs[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "mbuf_pool_%u", ctx->lcore);
    mbuf_pool = rte_pktmbuf_pool_create(mp_name,
                                RSP_MAX_DEPTH*2,//in / out
                                0,
                                0,
                                MAX_MBUF_SIZE,//data room size include head(128)
                                ctx->socket_id);
    if(mbuf_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "mbuf_pool alloc failed!\n");
    }

    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_SYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        64,//use for iv and aad
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "op_pool alloc failed!\n");
    }

    struct rte_crypto_sym_op *sym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;
//    printf("ctx->nb_pkt=%d burst=%d \n",ctx->nb_pkt,burst);
//    printf("DEBUG total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);
    
    //prepare compare data
    in_data_len = ctx->size;
    in_data = rte_zmalloc("src buf", in_data_len, 0);
    out_data = rte_zmalloc("dst buf", in_data_len+32, 0);//more 32 byte for tag
    //random_data(in_data, in_data_len);
    memset(in_data, 0x12, in_data_len);
    ret=crypto_cipher(algo, mode, 1/*enc*/, key_data, key_length, iv_data, iv_length, aad_data, aad_length, in_data, in_data_len, out_data, out_data+in_data_len/*for tag*/);
    if( ret == CRYPTO_RET_SUCCESS)
    {
#if 0
        rsp_log_dump(out_data,in_data_len,"SW out");
        if(mode == CIPHER_MODE_GCM)
            rsp_log_dump(out_data+in_data_len,16,"SW tag");
#endif
    }else{
        rte_exit(EXIT_FAILURE, "Compare data prepare failed! ret=%d \n",ret);
    }
    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))//until all pkt enq->deq
    {
        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        if(once != 0)
        {
            ret=0;
            //alloc mbuf fro pool
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, in_mbufs, once);
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, out_mbufs, once);
            if(ret != 0)
            {
                rte_exit(EXIT_FAILURE, "mbuf alloc failed!\n");
            }

            uint8_t *ptr;
            for(i=0;i<once;i++)
            {
                ptr = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *);
                memcpy(ptr+data_offset,out_data, in_data_len);
            }
            //just alloc enq op from pool
            rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_SYMMETRIC, enqueued_ops, once);
            //setup all ops
            for (i = 0; i < once; i++) 
            {
                enqueued_ops[i]->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;
                rte_crypto_op_attach_sym_session(enqueued_ops[i], sess[0]);
                //set IV
                uint8_t *iv_ptr = rte_crypto_op_ctod_offset(enqueued_ops[i], uint8_t *,iv_offset);
                memcpy(iv_ptr, iv_data, iv_length);
                sym_op = enqueued_ops[i]->sym;
                sym_op->m_src = in_mbufs[i];
                sym_op->m_dst = out_mbufs[i];
                if(xform.type == RTE_CRYPTO_SYM_XFORM_CIPHER)
                {
                    sym_op->cipher.data.length = in_data_len;
                    sym_op->cipher.data.offset = data_offset;
                }else{
                    sym_op->aead.data.length = in_data_len;
                    sym_op->aead.data.offset = data_offset;
                    sym_op->aead.digest.data = tag_digest;
                    sym_op->aead.aad.data    = cipher_aad;
                }
            }
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        //printf("Enqueue ops %d \n",ret);
        for(i=(ret);i<once;i++)
        {
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_src);
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_dst);
            //free enq failed ops
            rte_crypto_op_free(enqueued_ops[i]);
        }
        failed_enq += (once-ret);
        total_pkt_need_enqueue-=ret;
        
        ret=0;
        //deq ops
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
//            printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {

                struct rte_mbuf *mbuf;
                mbuf=dequeued_ops[i]->sym->m_dst;
                uint8_t *ptr = rte_pktmbuf_mtod(mbuf, uint8_t *);
#if 1
                rsp_log_dump(ptr,in_data_len,"dst");
                if(mode == CIPHER_MODE_GCM)
                     rsp_log_dump(dequeued_ops[i]->sym->aead.digest.data,16,"tag");
#endif

                if(ctx->check_rst)
                {
                    if ( (memcmp(ptr+data_offset, in_data, in_data_len) != 0) )
                    {
                        rsp_log_dump(in_data,in_data_len,"HW:");
                        rsp_log_dump(ptr,in_data_len,"SW:");
                        rte_exit(EXIT_FAILURE, "cipher failed!\n");
                    }
                    
                    if ( (mode == CIPHER_MODE_GCM) )
                    {
                        if ( (memcmp(dequeued_ops[i]->sym->aead.digest.data, out_data+in_data_len, 16) != 0) )
                        {
                            rsp_log_dump(dequeued_ops[i]->sym->aead.digest.data, 16, "HW tag");
                            rsp_log_dump(out_data+in_data_len, 16, "SW tag");
                            rte_exit(EXIT_FAILURE, "tag failed!\n");
                        }
                    }
                }
                //free all bufs
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_src);
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_dst);
                rte_crypto_op_free(dequeued_ops[i]);
            }

            total_pkt_need_dequeue+=ret;

        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);
    rte_mempool_free(mbuf_pool);
    rte_cryptodev_sym_session_clear(ctx->dev_id, sess[0]);
    rte_cryptodev_sym_session_free(sess[0]);
    return 0;
}
int rsp_rsa_runing(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int sess_size=0;
    int nb_sessions=1;
    
    uint8_t *in_data;
    uint32_t in_data_len;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;
    struct rte_crypto_asym_xform xform;
    
    rsp_context_t *ctx=(rsp_context_t *)param;
    struct rte_cryptodev_asym_session *sess[0];

    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_mempool *op_pool;

    switch(ctx->op)
    {
        case RSA_1024_SIGN:
            in_data     =  rsa1024_msg;
            in_data_len =  sizeof(rsa1024_msg);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa1024_n;
            xform.rsa.n.length    = sizeof(rsa1024_n);
            xform.rsa.d.data      = rsa1024_d;
            xform.rsa.d.length    = sizeof(rsa1024_d);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_1024_SIGN_CRT:
            in_data     =  rsa1024_msg;
            in_data_len =  sizeof(rsa1024_msg);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.qt.p.data   = rsa1024_p;
            xform.rsa.qt.p.length = sizeof(rsa1024_p);
            xform.rsa.qt.q.data   = rsa1024_q;
            xform.rsa.qt.q.length = sizeof(rsa1024_q);

            xform.rsa.qt.dP.data   = rsa1024_dmp1;
            xform.rsa.qt.dP.length = sizeof(rsa1024_dmp1);
            xform.rsa.qt.dQ.data   = rsa1024_dmq1;
            xform.rsa.qt.dQ.length = sizeof(rsa1024_dmq1);

            xform.rsa.qt.qInv.data   = rsa1024_iqmp;
            xform.rsa.qt.qInv.length = sizeof(rsa1024_iqmp);

            xform.rsa.key_type    = RTE_RSA_KET_TYPE_QT;
            break;
        case RSA_1024_VERI:
            in_data     =  rsa1024_signature;
            in_data_len =  sizeof(rsa1024_signature);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa1024_n;
            xform.rsa.n.length    = sizeof(rsa1024_n);
            xform.rsa.e.data      = rsa1024_e;
            xform.rsa.e.length    = sizeof(rsa1024_e);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_1024_ENC:
            in_data     =  rsa1024_msg;
            in_data_len =  sizeof(rsa1024_msg);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa1024_n;
            xform.rsa.n.length    = sizeof(rsa1024_n);
            xform.rsa.e.data      = rsa1024_e;
            xform.rsa.e.length    = sizeof(rsa1024_e);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_1024_DEC:
            in_data     =  rsa1024_public_enc_result;
            in_data_len =  sizeof(rsa1024_public_enc_result);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa1024_n;
            xform.rsa.n.length    = sizeof(rsa1024_n);
            xform.rsa.d.data      = rsa1024_d;
            xform.rsa.d.length    = sizeof(rsa1024_d);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_2048_SIGN:
            in_data     =  rsa2048_msg;
            in_data_len =  sizeof(rsa2048_msg);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa2048_n;
            xform.rsa.n.length    = sizeof(rsa2048_n);
            xform.rsa.d.data      = rsa2048_d;
            xform.rsa.d.length    = sizeof(rsa2048_d);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_2048_SIGN_CRT:
            in_data     =  rsa2048_msg;
            in_data_len =  sizeof(rsa2048_msg);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.qt.p.data   = rsa2048_p;
            xform.rsa.qt.p.length = sizeof(rsa2048_p);
            xform.rsa.qt.q.data   = rsa2048_q;
            xform.rsa.qt.q.length = sizeof(rsa2048_q);

            xform.rsa.qt.dP.data   = rsa2048_dmp1;
            xform.rsa.qt.dP.length = sizeof(rsa2048_dmp1);
            xform.rsa.qt.dQ.data   = rsa2048_dmq1;
            xform.rsa.qt.dQ.length = sizeof(rsa2048_dmq1);

            xform.rsa.qt.qInv.data   = rsa2048_iqmp;
            xform.rsa.qt.qInv.length = sizeof(rsa2048_iqmp);

            xform.rsa.key_type    = RTE_RSA_KET_TYPE_QT;
            break;
        case RSA_2048_VERI:
            in_data     =  rsa2048_signature;
            in_data_len =  sizeof(rsa2048_signature);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa2048_n;
            xform.rsa.n.length    = sizeof(rsa2048_n);
            xform.rsa.e.data      = rsa2048_e;
            xform.rsa.e.length    = sizeof(rsa2048_e);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_2048_ENC:
            in_data     =  rsa2048_msg;
            in_data_len =  sizeof(rsa2048_msg);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa2048_n;
            xform.rsa.n.length    = sizeof(rsa2048_n);
            xform.rsa.e.data      = rsa2048_e;
            xform.rsa.e.length    = sizeof(rsa2048_e);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
        case RSA_2048_DEC:
            in_data     =  rsa2048_pubkey_enc_result;
            in_data_len =  sizeof(rsa2048_pubkey_enc_result);
            xform.next            = NULL;
            xform.xform_type      = RTE_CRYPTO_ASYM_XFORM_RSA;
            xform.rsa.n.data      = rsa2048_n;
            xform.rsa.n.length    = sizeof(rsa2048_n);
            xform.rsa.d.data      = rsa2048_d;
            xform.rsa.d.length    = sizeof(rsa2048_d);
            xform.rsa.key_type    = RTE_RSA_KEY_TYPE_EXP;
            break;
    }
    
    sess_size = rte_cryptodev_asym_get_header_session_size();
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_mempool_create(mp_name,
                        nb_sessions,
                        sess_size,
                        0, 0, NULL, NULL, NULL,
                        NULL, ctx->socket_id,
                        0);
    sess_size = rte_cryptodev_asym_get_private_session_size(ctx->dev_id),

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                        nb_sessions,
                        sess_size,
                        0, 0, NULL, NULL, NULL,
                        NULL, ctx->socket_id,
                        0);
    //sess create & init
    sess[0]=rte_cryptodev_asym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "asym_sess create failed !\n");
    }
    ret=rte_cryptodev_asym_session_init(ctx->dev_id, sess[0], &xform, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "asym_sess inite failed !\n");
    }
    
    
    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_ASYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        32,//append for what
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "crypto_op_pool alloc failed!\n");
    }

    struct rte_crypto_asym_op *asym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;
//    printf("ctx->nb_pkt=%d burst=%d \n",ctx->nb_pkt,burst);
//    printf("DEBUG total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))
    {

        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        //alloc ops from pool
        rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_ASYMMETRIC, enqueued_ops, once);
        for (i = 0; i < once; i++) 
        {
            asym_op = enqueued_ops[i]->asym;
            switch(ctx->op)
            {
                case RSA_1024_ENC:
                case RSA_2048_ENC:
                    asym_op->rsa.op_type        = RTE_CRYPTO_ASYM_OP_ENCRYPT;
                    asym_op->rsa.message.data   = in_data;
                    asym_op->rsa.message.length = in_data_len;
                    asym_op->rsa.cipher.length  = in_data_len;
                    asym_op->rsa.cipher.data    = rte_zmalloc("user buf", 256, 0);//output_buf[i];
                    asym_op->rsa.pad            = RTE_CRYPTO_RSA_PADDING_NONE;
                    break;
                case RSA_1024_DEC:
                case RSA_2048_DEC:
                    asym_op->rsa.op_type        = RTE_CRYPTO_ASYM_OP_DECRYPT;
                    asym_op->rsa.cipher.data    = in_data;
                    asym_op->rsa.cipher.length  = in_data_len;
                    asym_op->rsa.message.length = in_data_len;
                    asym_op->rsa.message.data   = rte_zmalloc("user buf", 256, 0);//output_buf[i];
                    asym_op->rsa.pad            = RTE_CRYPTO_RSA_PADDING_NONE;
                    break;
                case RSA_1024_VERI:
                case RSA_2048_VERI:
                    asym_op->rsa.op_type        = RTE_CRYPTO_ASYM_OP_VERIFY;
                    asym_op->rsa.sign.data      = in_data;
                    asym_op->rsa.sign.length    = in_data_len;
                    asym_op->rsa.cipher.length  = in_data_len;
                    asym_op->rsa.cipher.data    = rte_zmalloc("user buf", 256, 0);//output_buf[i];
                    asym_op->rsa.pad            = RTE_CRYPTO_RSA_PADDING_NONE;
                    break;

                case RSA_1024_SIGN_CRT:
                case RSA_2048_SIGN_CRT:
                case RSA_1024_SIGN:
                case RSA_2048_SIGN:
                    asym_op->rsa.op_type        = RTE_CRYPTO_ASYM_OP_SIGN;
                    asym_op->rsa.message.data   = in_data;
                    asym_op->rsa.message.length = in_data_len;
                    asym_op->rsa.sign.length    = in_data_len;
                    asym_op->rsa.sign.data      = rte_zmalloc("user buf", 256, 0);//output_buf[i];
                    asym_op->rsa.pad            = RTE_CRYPTO_RSA_PADDING_NONE;
                    break;
            }
            rte_crypto_op_attach_asym_session(enqueued_ops[i], sess[0]);
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        failed_enq += (once-ret);
        for(i=(ret);i<once;i++)
        {
            //free enq failed ops
            uint8_t *ptr;
            asym_op = enqueued_ops[i]->asym;
            if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_ENCRYPT)
                ptr = asym_op->rsa.cipher.data;
            else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_DECRYPT)
                ptr = asym_op->rsa.message.data;
            else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_VERIFY)
                ptr = asym_op->rsa.cipher.data;
            else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_SIGN)
                ptr = asym_op->rsa.sign.data;

            rte_free(ptr);
            rte_crypto_op_free(enqueued_ops[i]);
        }
        total_pkt_need_enqueue-=ret;
        ret=0;
        //deq
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
            //process deq
//            printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {
                uint8_t *ptr;
                asym_op = dequeued_ops[i]->asym;
                if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_ENCRYPT)
                    ptr = asym_op->rsa.cipher.data;
                else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_DECRYPT)
                    ptr = asym_op->rsa.message.data;
                else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_VERIFY)
                    ptr = asym_op->rsa.cipher.data;
                else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_SIGN)
                    ptr = asym_op->rsa.sign.data;
#if 0
                rsp_log_dump(ptr,in_data_len,"dst");
#endif
                rte_free(ptr);
                rte_crypto_op_free(dequeued_ops[i]);
            }
            total_pkt_need_dequeue+=ret;
        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);

    return 0;
}
int rsp_ecc_runing(void *param)
{
    rte_exit(EXIT_FAILURE, "ECC not support !\n");

    return 0;
}
int rsp_trng_runing(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int algo;
    int mode;
    int dir=0;
    int nb_sessions=1;
    int sess_size=0;

    uint8_t *in_data;
    uint32_t in_data_len;
    uint8_t *out_data;

    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_cryptodev_sym_session *sess[1];
    struct rte_crypto_sym_xform xform;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;

    rsp_context_t *ctx=(rsp_context_t *)param;
    
    switch(ctx->op)
    {
        case BULK_TRNG_GEN  :
            algo = 0;//no need
            xform.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform.next             = NULL;
            xform.cipher.algo      = 0xFF;
            xform.cipher.op        = dir;
            break;
    }
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_cryptodev_sym_session_pool_create(mp_name, 
                                nb_sessions,
                                0,
                                0,
                                1024,
                                ctx->socket_id);
    sess_size = rte_cryptodev_sym_get_private_session_size(ctx->dev_id);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                                nb_sessions,
                                sess_size,
                                0, 0, NULL, NULL, NULL,
                                NULL, ctx->socket_id,
                                0);
    //create session & init
    sess[0] = rte_cryptodev_sym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "Session create failed !\n");
    }
    
    ret=rte_cryptodev_sym_session_init(ctx->dev_id, sess[0], &xform, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "sym_sess init failed ret=%d !\n",ret);
        return 0;
    }

    
    struct rte_mempool *op_pool;
    struct rte_mempool *mbuf_pool;
    struct rte_mbuf *in_mbufs[RSP_MAX_DEPTH];//  = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);
    struct rte_mbuf *out_mbufs[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "mbuf_pool_%u", ctx->lcore);
    mbuf_pool = rte_pktmbuf_pool_create(mp_name,
                                RSP_MAX_DEPTH*2,//in / out
                                0,
                                0,
                                MAX_MBUF_SIZE,//data room size include head(128)
                                ctx->socket_id);
    if(mbuf_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "mbuf_pool alloc failed!\n");
    }

    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_SYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        64,//use for iv and aad
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "op_pool alloc failed!\n");
    }

    struct rte_crypto_sym_op *sym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;

    //prepare compare data
    in_data_len = ctx->size;
    in_data = rte_zmalloc("src buf", in_data_len, 0);
    out_data = rte_zmalloc("dst buf", in_data_len, 0);
    random_data(in_data, in_data_len);
    if(algo!=0)
    {
        ret=crypto_md(algo,  in_data, in_data_len, out_data);
        if( ret == CRYPTO_RET_SUCCESS)
        {
    #if 0
            rsp_log_dump(out_data,in_data_len,"SW out");
    #endif
        }else{
            rte_exit(EXIT_FAILURE, "Compare data prepare failed! ret=%d \n",ret);
        }
    }
    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))//until all pkt enq->deq
    {
        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        if(once != 0)
        {
            ret=0;
            //alloc mbuf fro pool
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, in_mbufs, once);
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, out_mbufs, once);
            if(ret != 0)
            {
                rte_exit(EXIT_FAILURE, "mbuf alloc failed!\n");
            }

            
            //just alloc enq op from pool
            rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_SYMMETRIC, enqueued_ops, once);
            //setup all ops
            uint8_t *ptr;
            for (i = 0; i < once; i++) 
            {
                enqueued_ops[i]->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;
                rte_crypto_op_attach_sym_session(enqueued_ops[i], sess[0]);
                sym_op = enqueued_ops[i]->sym;
                //set src 
                ptr = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *);
                memcpy(ptr,in_data, in_data_len);
                sym_op->auth.data.offset = 0;
                sym_op->auth.data.length = in_data_len;
                sym_op->m_src=in_mbufs[i];
                sym_op->m_dst=out_mbufs[i];//not used just for free convenient
                //set dst
                sym_op->auth.digest.data      = rte_pktmbuf_mtod(out_mbufs[i], uint8_t *);
                sym_op->auth.digest.phys_addr = rte_pktmbuf_iova(out_mbufs[i]);
            }
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        //printf("Enqueue ops %d \n",ret);
        for(i=(ret);i<once;i++)
        {
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_src);
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_dst);
            //free enq failed ops
            rte_crypto_op_free(enqueued_ops[i]);
        }
        failed_enq += (once-ret);
        total_pkt_need_enqueue-=ret;
        
        ret=0;
        //deq ops
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
//            printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {
                uint8_t *ptr = dequeued_ops[i]->sym->auth.digest.data;
#if 0
                rsp_log_dump(ptr,in_data_len,"dst");
#endif
                if(ctx->check_rst)
                {
                    //HASH result check
                    //if ( (memcmp(ptr, out_data, in_data_len) != 0) )
                    //{
                    //    rsp_log_dump(out_data,in_data_len,"HW:");
                    //    rsp_log_dump(ptr,in_data_len,"SW:");
                    //    rte_exit(EXIT_FAILURE, "cipher failed!\n");
                    //}
                }
                //free all bufs
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_src);
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_dst);
                rte_crypto_op_free(dequeued_ops[i]);
            }

            total_pkt_need_dequeue+=ret;
        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);
    rte_mempool_free(mbuf_pool);
    rte_cryptodev_sym_session_clear(ctx->dev_id, sess[0]);
    rte_cryptodev_sym_session_free(sess[0]);
    return 0;
}
int rsp_hmac_runing(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int algo;
    int nb_sessions=1;
    int sess_size=0;

    uint8_t  *key_data;
    uint16_t key_length;

    uint8_t *in_data;
    uint32_t in_data_len;
    uint8_t *out_data;
    uint16_t mac_len=0;
    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_cryptodev_sym_session *sess[1];
    struct rte_crypto_sym_xform xform;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;

    rsp_context_t *ctx=(rsp_context_t *)param;
    
    switch(ctx->op)
    {
        case BULK_SHA1_HMAC  :
            algo = MD_ALGO_SHA1;
            key_data   = auth_key;
            key_length = 16;
            mac_len    = 20;
            xform.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform.next             = NULL;
            xform.auth.algo        = RTE_CRYPTO_AUTH_SHA1_HMAC;
            xform.auth.key.data    = key_data;
            xform.auth.key.length  = key_length;
            xform.auth.digest_length = mac_len;

            break;
        case BULK_SHA256_HMAC  :
            algo = MD_ALGO_SHA256;
            key_data   = auth_key;
            key_length = 16;
            mac_len    = 32;
            xform.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform.next             = NULL;
            xform.auth.algo        = RTE_CRYPTO_AUTH_SHA256_HMAC;
            xform.auth.key.data    = key_data;
            xform.auth.key.length  = key_length;
            xform.auth.digest_length = mac_len;
                
            break;
        case BULK_SM3_HMAC  :
            algo = MD_ALGO_SM3;
            key_data   = auth_key;
            key_length = 32;
            mac_len    = 32;
            xform.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform.next             = NULL;
            xform.auth.algo        = 0xFD;
            xform.auth.key.data    = key_data;
            xform.auth.key.length  = key_length;
            xform.auth.digest_length = mac_len;
            break;
    }
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_cryptodev_sym_session_pool_create(mp_name, 
                                nb_sessions,
                                0,
                                0,
                                1024,
                                ctx->socket_id);
    sess_size = rte_cryptodev_sym_get_private_session_size(ctx->dev_id);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                                nb_sessions,
                                sess_size,
                                0, 0, NULL, NULL, NULL,
                                NULL, ctx->socket_id,
                                0);
    //create session & init
    sess[0] = rte_cryptodev_sym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "Session create failed !\n");
    }
    
    ret=rte_cryptodev_sym_session_init(ctx->dev_id, sess[0], &xform, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "sym_sess init failed ret=%d !\n",ret);
        return 0;
    }

    struct rte_mempool *op_pool;
    struct rte_mempool *mbuf_pool;
    struct rte_mbuf *in_mbufs[RSP_MAX_DEPTH];//  = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);
    struct rte_mbuf *out_mbufs[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "mbuf_pool_%u", ctx->lcore);
    mbuf_pool = rte_pktmbuf_pool_create(mp_name,
                                RSP_MAX_DEPTH*2,//in / out
                                0,
                                0,
                                MAX_MBUF_SIZE,//data room size include head(128)
                                ctx->socket_id);
    if(mbuf_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "mbuf_pool alloc failed!\n");
    }

    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_SYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        0,
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "op_pool alloc failed!\n");
    }

    struct rte_crypto_sym_op *sym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;
//    printf("ctx->nb_pkt=%d burst=%d \n",ctx->nb_pkt,burst);
//    printf("DEBUG total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);
    
    //prepare compare data
    in_data_len = ctx->size;
    in_data = rte_zmalloc("src buf", in_data_len, 0);
    out_data = rte_zmalloc("dst buf", in_data_len, 0);
    random_data(in_data, in_data_len);
    //memset(in_data,0x12, in_data_len);
    ret=crypto_hmac(algo, key_data, key_length, in_data, in_data_len, out_data);
    if( ret == CRYPTO_RET_SUCCESS)
    {
#if 0
        rsp_log_dump(in_data,in_data_len,"SW in");
        rsp_log_dump(out_data,in_data_len,"SW out");
#endif
    }else{
        rte_exit(EXIT_FAILURE, "Compare data prepare failed! ret=%d \n",ret);
    }
    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))//until all pkt enq->deq
    {
        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        if(once != 0)
        {
            ret=0;
            //alloc mbuf fro pool
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, in_mbufs, once);
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, out_mbufs, once);
            if(ret != 0)
            {
                rte_exit(EXIT_FAILURE, "mbuf alloc failed!\n");
            }

            uint8_t *ptr;
            for(i=0;i<once;i++)
            {
                ptr = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *);
                memcpy(ptr+data_offset,in_data, in_data_len);
            }
            //just alloc enq op from pool
            rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_SYMMETRIC, enqueued_ops, once);
            //setup all ops
            for (i = 0; i < once; i++)
            {
                enqueued_ops[i]->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;
                rte_crypto_op_attach_sym_session(enqueued_ops[i], sess[0]);

                sym_op = enqueued_ops[i]->sym;
                sym_op->m_src = in_mbufs[i];
                sym_op->m_dst = out_mbufs[i];
                
                sym_op->auth.data.length = in_data_len;
                sym_op->auth.data.offset = data_offset;

                sym_op->auth.digest.data      = rte_pktmbuf_mtod(out_mbufs[i], uint8_t *);
                sym_op->auth.digest.phys_addr = rte_pktmbuf_iova(out_mbufs[i]);
            }
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        //printf("Enqueue ops %d \n",ret);
        for(i=(ret);i<once;i++)
        {
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_src);
            rte_pktmbuf_free(enqueued_ops[i]->sym->m_dst);
            //free enq failed ops
            rte_crypto_op_free(enqueued_ops[i]);
        }
        failed_enq += (once-ret);
        total_pkt_need_enqueue-=ret;
        
        ret=0;
        //deq ops
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
            //printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {
#if 0
                rsp_log_dump(dequeued_ops[i]->sym->auth.digest.data,mac_len,"dst");
#endif

                if(ctx->check_rst)
                {
                    if ( (memcmp(dequeued_ops[i]->sym->auth.digest.data, out_data, mac_len) != 0) )
                    {
                        rsp_log_dump(dequeued_ops[i]->sym->auth.digest.data,mac_len,"HW:");
                        rsp_log_dump(out_data,mac_len,"SW:");
                        rte_exit(EXIT_FAILURE, "auth failed!\n");
                    }
                }
                //free all bufs
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_src);
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_dst);
                rte_crypto_op_free(dequeued_ops[i]);
            }

            total_pkt_need_dequeue+=ret;

        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);
    rte_mempool_free(mbuf_pool);
    rte_cryptodev_sym_session_clear(ctx->dev_id, sess[0]);
    rte_cryptodev_sym_session_free(sess[0]);
    return 0;
}
int rsp_cipher_auth_runing(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int c_algo;
    int a_algo;
    int c_mode;
    uint16_t mac_len=0;
    int dir;
    int nb_sessions=1;
    int sess_size=0;

    uint8_t  *iv_data;
    uint16_t iv_offset;
    uint16_t iv_length;

    uint8_t  *c_key_data;
    uint16_t c_key_length;
    
    uint8_t  *a_key_data;
    uint16_t a_key_length;
    
    uint8_t  *aad_data;
    uint16_t aad_length;
    
    uint8_t *in_data;
    uint8_t *c_data;
    uint8_t *a_data;
    uint32_t in_data_len;
    uint8_t *out_data;
    uint8_t *out_data1;
    
    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_cryptodev_sym_session *sess[1];
    struct rte_crypto_sym_xform xform;
    struct rte_crypto_sym_xform xform1;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;

    rsp_context_t *ctx=(rsp_context_t *)param;
    
    switch(ctx->op)
    {
        case AES_128_CBC_SHA1  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = &xform1;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA1;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len      = 20;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = NULL;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA1_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case AES_128_CBC_SHA256  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = &xform1;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA256;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 32;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = NULL;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA256_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case AES_256_CBC_SHA1  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 32;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = &xform1;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA1;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 20;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = NULL;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA1_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case AES_256_CBC_SHA256  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 32;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = &xform1;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA256;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 32;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = NULL;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA256_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SM4_SM3      :
            c_algo = CIPHER_SM4;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            //SM4 param config
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = &xform1;
            xform.cipher.algo      = 0xFF;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SM3;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 32;
            //SM3 config
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = NULL;
            xform1.auth.algo        = 0xFD;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SM1_SM3      :
            c_algo = 0xFF;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_ENCRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            //SM4 param config
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = &xform1;
            xform.cipher.algo      = 0xFE;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SM3;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 32;
            //SM3 config
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = NULL;
            xform1.auth.algo        = 0xFD;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
    }
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_cryptodev_sym_session_pool_create(mp_name, 
                                nb_sessions,
                                0,
                                0,
                                1024,
                                ctx->socket_id);
    sess_size = rte_cryptodev_sym_get_private_session_size(ctx->dev_id);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                                nb_sessions,
                                sess_size,
                                0, 0, NULL, NULL, NULL,
                                NULL, ctx->socket_id,
                                0);
    //create session & init
    sess[0] = rte_cryptodev_sym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "Session create failed !\n");
    }
    
    ret=rte_cryptodev_sym_session_init(ctx->dev_id, sess[0], &xform, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "sym_sess init failed ret=%d !\n",ret);
        return 0;
    }

    struct rte_mempool *op_pool;
    struct rte_mempool *mbuf_pool;
    struct rte_mbuf *in_mbufs[RSP_MAX_DEPTH];//  = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);
    struct rte_mbuf *mac_out_mbufs[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "mbuf_pool_%u", ctx->lcore);
    mbuf_pool = rte_pktmbuf_pool_create(mp_name,
                                RSP_MAX_DEPTH*2,//in / out
                                0,
                                0,
                                MAX_MBUF_SIZE,//data room size include head(128)
                                ctx->socket_id);
    if(mbuf_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "mbuf_pool alloc failed!\n");
    }

    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_SYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        64,//use for iv
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "op_pool alloc failed!\n");
    }

    struct rte_crypto_sym_op *sym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;

    //prepare compare data

    in_data_len = ctx->size;
    in_data = rte_zmalloc("src buf", in_data_len+esp_len+2, 0);
    out_data = rte_zmalloc("dst buf", in_data_len, 0);
    out_data1= rte_zmalloc("auth buf", 32, 0);
    random_data(in_data, in_data_len+esp_len);
    memset(in_data, 0x12,esp_len);
    memset(in_data+esp_len,0x34, in_data_len+2);


    c_data = in_data+esp_len;
    a_data = rte_zmalloc("src buf", in_data_len+esp_len, 0);
    if(c_algo != 0xff)
    {
        ret|=crypto_cipher(c_algo, c_mode, 1/*enc*/, c_key_data, c_key_length, iv_data, iv_length, aad_data, aad_length, c_data, in_data_len, out_data, NULL);

        memcpy(a_data,in_data,in_data_len);
        memcpy(a_data+esp_len,out_data,in_data_len);
        //rsp_log_dump(a_data,in_data_len+esp_len,"SW out");
        ret|=crypto_hmac(a_algo, a_key_data, a_key_length, a_data, in_data_len+esp_len, out_data1);

        if( ret == CRYPTO_RET_SUCCESS)
        {
    #if 0
            rsp_log_dump(in_data,in_data_len+esp_len,"SRC");
            rsp_log_dump(out_data,in_data_len,"SW out");
            rsp_log_dump(out_data1,mac_len,"SW auth");
    #endif
        }else{
            rte_exit(EXIT_FAILURE, "Compare data prepare failed! ret=%d \n",ret);
        }
    }
    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))//until all pkt enq->deq
    {
        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        if(once != 0)
        {
            ret=0;
            //alloc mbuf from pool, only one buffer, we using src=dst bellow
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, in_mbufs, once);
            if(ret != 0)
            {
                rte_exit(EXIT_FAILURE, "mbuf alloc failed!\n");
            }

            uint8_t *ptr;
            for(i=0;i<once;i++)
            {
                ptr = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *);
                memcpy(ptr+data_offset,in_data, in_data_len+esp_len);//fill data esp+data
            }
            //just alloc enq op from pool
            rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_SYMMETRIC, enqueued_ops, once);
            //setup all ops
            for (i = 0; i < once; i++) 
            {
                enqueued_ops[i]->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;
                rte_crypto_op_attach_sym_session(enqueued_ops[i], sess[0]);
                //set IV
                uint8_t *iv_ptr = rte_crypto_op_ctod_offset(enqueued_ops[i], uint8_t *,iv_offset);
                memcpy(iv_ptr, iv_data, iv_length);
                sym_op = enqueued_ops[i]->sym;
                //set plain buf
                sym_op->m_src = in_mbufs[i];
                sym_op->m_dst = NULL;//dst buffer is null,so the cipher will instead of src buffer
                
                sym_op->cipher.data.length = in_data_len;//cipher length
                sym_op->cipher.data.offset = data_offset+esp_len;//cipher data offset skip esp
                
                sym_op->auth.data.length = in_data_len+esp_len;//auth length(contain esp+cipher)
                sym_op->auth.data.offset = data_offset;
                //set buf for store HMAC
                sym_op->auth.digest.data      = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *)+3072;//2048 offset position using for HMAC result
                sym_op->auth.digest.phys_addr = rte_pktmbuf_iova(in_mbufs[i])+3072;
            }
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        //printf("Enqueue ops %d \n",ret);
        for(i=(ret);i<once;i++)
        {
            rte_pktmbuf_free(in_mbufs[i]);
            //free enq failed ops
            rte_crypto_op_free(enqueued_ops[i]);
        }
        failed_enq += (once-ret);
        total_pkt_need_enqueue-=ret;
        
        ret=0;
        //deq ops
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
            //printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {
                struct rte_mbuf *mbuf_in;
                mbuf_in=dequeued_ops[i]->sym->m_src;
                uint8_t *ptr_in   = rte_pktmbuf_mtod(mbuf_in, uint8_t *);
                uint8_t *ptr_hmac = dequeued_ops[i]->sym->auth.digest.data;
#if 0
                /*
                    we set src=dst ,so cipher is in src buf
                */
                rsp_log_dump(ptr_in+data_offset+esp_len, in_data_len,"cipher");
                rsp_log_dump(ptr_hmac, mac_len,"auth");
#endif

                if(ctx->check_rst)
                {
                    if ( (memcmp(ptr_in+data_offset+esp_len, out_data, in_data_len) != 0) )//compare cipher
                    {
                        rsp_log_dump(ptr_in+data_offset+esp_len,in_data_len,"HW:");
                        rsp_log_dump(out_data,in_data_len,"SW:");
                        rte_exit(EXIT_FAILURE, "cipher failed!\n");
                    }
                    if ( (memcmp(ptr_hmac, out_data1, mac_len) != 0) )//compare HMAC
                    {
                        rsp_log_dump(ptr_hmac,mac_len,"HW:");
                        rsp_log_dump(out_data1,mac_len,"SW:");
                        rte_exit(EXIT_FAILURE, "auth failed!\n");
                    }
                }
                //free all bufs
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_src);
                rte_crypto_op_free(dequeued_ops[i]);
            }

            total_pkt_need_dequeue+=ret;

        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);
    rte_mempool_free(mbuf_pool);
    rte_cryptodev_sym_session_clear(ctx->dev_id, sess[0]);
    rte_cryptodev_sym_session_free(sess[0]);
    return 0;
}
int rsp_auth_cipher_runing(void *param)
{
    char mp_name[RTE_MEMPOOL_NAMESIZE];
    int ret=0;
    int i;
    int c_algo;
    int a_algo;
    int c_mode;
    uint16_t mac_len=0;
    int dir;
    int nb_sessions=1;
    int sess_size=0;

    uint8_t  *iv_data;
    uint16_t iv_offset;
    uint16_t iv_length;

    uint8_t  *c_key_data;
    uint16_t c_key_length;
    
    uint8_t  *a_key_data;
    uint16_t a_key_length;
    
    uint8_t  *aad_data;
    uint16_t aad_length;
    
    uint8_t *in_data;
    uint8_t *c_data;
    uint8_t *a_data;
    uint32_t in_data_len;
    uint8_t *out_data;
    uint8_t *out_data1;
    
    struct rte_mempool *sess_mempool;
    struct rte_mempool *sess_priv_mempool;
    struct rte_cryptodev_sym_session *sess[1];
    struct rte_crypto_sym_xform xform;
    struct rte_crypto_sym_xform xform1;
    
    uint64_t tsc_start=0;
    uint64_t tsc_end=0;
    uint64_t tsc_duration=0;

    rsp_context_t *ctx=(rsp_context_t *)param;
    
    switch(ctx->op)
    {
        case SHA1_AES_128_CBC  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;

            a_algo = MD_ALGO_SHA1;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 20;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = &xform;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA1_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SHA256_AES_128_CBC  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length    = 16;
            iv_data      = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA256;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len      = 32;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = &xform;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA256_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SHA1_AES_256_CBC  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 32;
            iv_length    = 16;
            iv_data      = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA1;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len      = 20;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = &xform;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA1_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SHA256_AES_256_CBC  :
            c_algo = CIPHER_AES;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 32;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = RTE_CRYPTO_CIPHER_AES_CBC;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SHA256;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len      = 32;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = &xform;
            xform1.auth.algo        = RTE_CRYPTO_AUTH_SHA256_HMAC;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SM3_SM4      :
            c_algo = CIPHER_SM4;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = 0xFF;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SM3;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 32;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = &xform;
            xform1.auth.algo        = 0xFD;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
        case SM3_SM1      :
            c_algo = 0xFF;
            c_mode = CIPHER_MODE_CBC;
            c_key_data   = cipher_key;
            c_key_length = 16;
            iv_length  = 16;
            iv_data    = cipher_iv;
            dir = RTE_CRYPTO_CIPHER_OP_DECRYPT;
            iv_offset  = sizeof(struct rte_crypto_op) + sizeof(struct rte_crypto_sym_op);
            xform.type             = RTE_CRYPTO_SYM_XFORM_CIPHER;
            xform.next             = NULL;
            xform.cipher.algo      = 0xFE;
            xform.cipher.op        = dir;
            xform.cipher.iv.offset = iv_offset;
            xform.cipher.iv.length = iv_length;
            xform.cipher.key.data  = c_key_data;
            xform.cipher.key.length= c_key_length;
            
            a_algo = MD_ALGO_SM3;
            a_key_data   = auth_key;
            a_key_length = 32;
            mac_len    = 32;
            xform1.type             = RTE_CRYPTO_SYM_XFORM_AUTH;
            xform1.next             = &xform;
            xform1.auth.algo        = 0xFD;
            xform1.auth.key.data    = a_key_data;
            xform1.auth.key.length  = a_key_length;
            xform1.auth.digest_length = mac_len;
            break;
    }
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_%u_%u", ctx->lcore, ctx->socket_id);
    sess_mempool = rte_cryptodev_sym_session_pool_create(mp_name, 
                                nb_sessions,
                                0,
                                0,
                                1024,
                                ctx->socket_id);
    sess_size = rte_cryptodev_sym_get_private_session_size(ctx->dev_id);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "s_mp_p_%u_%u", ctx->lcore, ctx->socket_id);
    sess_priv_mempool = rte_mempool_create(mp_name,
                                nb_sessions,
                                sess_size,
                                0, 0, NULL, NULL, NULL,
                                NULL, ctx->socket_id,
                                0);
    //create session & init
    sess[0] = rte_cryptodev_sym_session_create(sess_mempool);
    if(sess[0] == NULL)
    {
        rte_exit(EXIT_FAILURE, "Session create failed !\n");
    }
    
    ret=rte_cryptodev_sym_session_init(ctx->dev_id, sess[0], &xform1, sess_priv_mempool);
    if(ret!=0)
    {
        rte_exit(EXIT_FAILURE, "sym_sess init failed ret=%d !\n",ret);
        return 0;
    }

    struct rte_mempool *op_pool;
    struct rte_mempool *mbuf_pool;
    struct rte_mbuf *in_mbufs[RSP_MAX_DEPTH];//  = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);
    struct rte_mbuf *mac_out_mbufs[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_mbuf *)*sizeof(ctx->burst),16);

    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "mbuf_pool_%u", ctx->lcore);
    mbuf_pool = rte_pktmbuf_pool_create(mp_name,
                                RSP_MAX_DEPTH*2,//in / out
                                0,
                                0,
                                MAX_MBUF_SIZE,//2K+128byte 
                                ctx->socket_id);
    if(mbuf_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "mbuf_pool alloc failed!\n");
    }

    struct rte_crypto_op *enqueued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    struct rte_crypto_op *dequeued_ops[RSP_MAX_DEPTH];// = rte_zmalloc(NULL,sizeof(struct rte_crypto_op *)*ctx->burst,16);
    snprintf(mp_name, RTE_MEMPOOL_NAMESIZE, "crypto_op_pool%u", ctx->lcore);
    op_pool = rte_crypto_op_pool_create(mp_name,
                                        RTE_CRYPTO_OP_TYPE_SYMMETRIC,
                                        RSP_MAX_DEPTH,
                                        0,
                                        64,//use for iv and aad
                                        ctx->socket_id);
    if(op_pool==NULL)
    {
        rte_exit(EXIT_FAILURE, "op_pool alloc failed!\n");
    }

    struct rte_crypto_sym_op *sym_op;
    int failed_enq=0;
    int failed_deq=0;
    int total_pkt_need_dequeue=0;
    int total_pkt_need_enqueue=ctx->nb_pkt;
    int burst=ctx->burst;
    int once=0;

    //prepare compare data

    in_data_len = ctx->size;
    in_data = rte_zmalloc("src buf", in_data_len+esp_len, 0);
    out_data = rte_zmalloc("dst buf", in_data_len, 0);
    out_data1= rte_zmalloc("auth buf", 32, 0);
    random_data(in_data+esp_len, in_data_len);
    //memset(in_data, 0x12,in_data_len+esp_len);
    //memset(in_data+esp_len,0x34, in_data_len);
    //rsp_log_dump(in_data,in_data_len+esp_len,"SRC");

    c_data = in_data+esp_len;
    a_data = rte_zmalloc("src buf", in_data_len+esp_len, 0);
    if(c_algo != 0xFF)
    {
        ret|=crypto_cipher(c_algo, c_mode, 1/*enc*/, c_key_data, c_key_length, iv_data, iv_length, aad_data, aad_length, c_data, in_data_len, out_data, NULL);

        memcpy(a_data,in_data,in_data_len);
        memcpy(a_data+esp_len,out_data,in_data_len);
        //rsp_log_dump(a_data,in_data_len+esp_len,"SW out");
        ret|=crypto_hmac(a_algo, a_key_data, a_key_length, a_data, in_data_len+esp_len, out_data1);
        
        if( ret == CRYPTO_RET_SUCCESS)
        {
    #if 0
            rsp_log_dump(out_data,in_data_len,"SW out");
            rsp_log_dump(out_data1,mac_len,"SW auth");
         
    #endif
        }else{
            rte_exit(EXIT_FAILURE, "Compare data prepare failed! ret=%d \n",ret);
        }
    }
    tsc_start = rte_rdtsc_precise();
    while((total_pkt_need_enqueue>0) || (total_pkt_need_dequeue != ctx->nb_pkt))//until all pkt enq->deq
    {
        if(total_pkt_need_enqueue>burst)
            once = burst;
        else
            once = total_pkt_need_enqueue;
        if(once != 0)
        {
            ret=0;
            //alloc mbuf from pool
            ret |=rte_pktmbuf_alloc_bulk(mbuf_pool, in_mbufs, once);
            if(ret != 0)
            {
                rte_exit(EXIT_FAILURE, "mbuf alloc failed!\n");
            }

            uint8_t *ptr;
            for(i=0;i<once;i++)
            {
                ptr = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *);
                //esp+cipher+hmac
                memcpy(ptr+data_offset,in_data, esp_len);
                memcpy(ptr+data_offset+esp_len,out_data, in_data_len+esp_len);
                memcpy(ptr+data_offset+esp_len+in_data_len,out_data1, mac_len);
            }
            //just alloc enq op from pool
            rte_crypto_op_bulk_alloc(op_pool, RTE_CRYPTO_OP_TYPE_SYMMETRIC, enqueued_ops, once);
            //setup all ops
            for (i = 0; i < once; i++) 
            {
                enqueued_ops[i]->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;
                rte_crypto_op_attach_sym_session(enqueued_ops[i], sess[0]);
                //set IV
                uint8_t *iv_ptr = rte_crypto_op_ctod_offset(enqueued_ops[i], uint8_t *,iv_offset);
                memcpy(iv_ptr, iv_data, iv_length);
                sym_op = enqueued_ops[i]->sym;
                sym_op->m_src = in_mbufs[i];
                sym_op->m_dst = NULL;
                
                sym_op->cipher.data.length = in_data_len;//cipher length
                sym_op->cipher.data.offset = data_offset+esp_len;//skip esp
                
                sym_op->auth.data.length = in_data_len+esp_len;//auth length(contain esp+cipher)
                sym_op->auth.data.offset = data_offset;
                
                sym_op->auth.digest.data      = rte_pktmbuf_mtod(in_mbufs[i], uint8_t *)+3072;
                sym_op->auth.digest.phys_addr = rte_pktmbuf_iova(in_mbufs[i])+3072;
            }
        }
        //enq ops
        ret = rte_cryptodev_enqueue_burst(ctx->dev_id, ctx->qp_id, enqueued_ops, once);
        //printf("Enqueue ops %d \n",ret);
        for(i=(ret);i<once;i++)
        {
            rte_pktmbuf_free(in_mbufs[i]);
            //free enq failed ops
            rte_crypto_op_free(enqueued_ops[i]);
        }
        failed_enq += (once-ret);
        total_pkt_need_enqueue-=ret;
        
        ret=0;
        //deq ops
        ret=rte_cryptodev_dequeue_burst(ctx->dev_id, ctx->qp_id, dequeued_ops, burst);
        failed_deq += (burst-ret);
        if(ret!=0)
        {
//            printf("Dequeue ops %d \n",ret);
            for(i=0;i<ret;i++)
            {

                struct rte_mbuf *mbuf_in, *mbuf_out;
                mbuf_in=dequeued_ops[i]->sym->m_src;
                uint8_t *ptr_in  = rte_pktmbuf_mtod(mbuf_in, uint8_t *);
                uint8_t *ptr_hmac = dequeued_ops[i]->sym->auth.digest.data;
#if 0
                rsp_log_dump(ptr_in+esp_len+data_offset, in_data_len,"cipher");//skip esp
                rsp_log_dump(ptr_hmac, mac_len,"auth");
#endif

                if(ctx->check_rst)
                {
                    if ( (memcmp(ptr_in+data_offset, in_data, in_data_len+esp_len) != 0) )//compare esp+plain
                    {
                        rsp_log_dump(ptr_in,in_data_len+esp_len,"HW:");
                        rsp_log_dump(in_data,in_data_len+esp_len,"SW:");
                        rte_exit(EXIT_FAILURE, "cipher failed!\n");
                    }
                    if ( (memcmp(ptr_hmac, out_data1, mac_len) != 0) )//compare hmac
                    {
                        rsp_log_dump(ptr_hmac,mac_len,"HW:");
                        rsp_log_dump(out_data1,mac_len,"SW:");
                        rte_exit(EXIT_FAILURE, "auth failed!\n");
                    }
                }
                //free all bufs
                rte_pktmbuf_free(dequeued_ops[i]->sym->m_src);
                rte_crypto_op_free(dequeued_ops[i]);
            }

            total_pkt_need_dequeue+=ret;

        }
    }
    tsc_end = rte_rdtsc_precise();
    tsc_duration = tsc_end - tsc_start;
    uint64_t cpu_hz=rte_get_tsc_hz();
    double ops_per_second=0;
    double Gbps=0;
    double cy_buf=0;
    double total_time_s=0;
    //printf("ctx->nb_pkt=%d tsc_duration=%llu cpu_hz()=%llu \n",ctx->nb_pkt,tsc_duration,cpu_hz);
    ops_per_second = ((double)(ctx->nb_pkt) / (tsc_duration)) * cpu_hz / 1000000;
    Gbps = (ops_per_second*(ctx->size)*8)/1000;
    cy_buf = (double)(tsc_duration)/ctx->nb_pkt;
    total_time_s = (double)tsc_duration/cpu_hz;
    printf("%20s %12u %12u %12u %12u %12u %12u %12.4f %12.4f %12.4f %12.4f \n",
            algo_str[ctx->op],
            ctx->lcore,ctx->size,ctx->nb_pkt,total_pkt_need_dequeue,failed_enq,failed_deq,
            ops_per_second,
            Gbps,
            cy_buf,
            total_time_s);
    //printf("%d total_pkt_need_enqueue=%d total_pkt_need_dequeue=%d \n",total_pkt_need_enqueue,total_pkt_need_dequeue);

    rte_mempool_free(op_pool);
    rte_mempool_free(mbuf_pool);
    rte_cryptodev_sym_session_clear(ctx->dev_id, sess[0]);
    rte_cryptodev_sym_session_free(sess[0]);
    return 0;
}
int init_crypto_device(char *name, int *dev_id, int *socket_id)
{
    int ret =0 ;
    int j=0;
    struct rte_cryptodev_config conf;
    struct rte_cryptodev_qp_conf qp_conf;
    int qp_num;
    struct rte_cryptodev_info dev_info;
    //Get crypto device id 
    *dev_id = rte_cryptodev_get_dev_id(name);
    if(*dev_id<0)
    {
        printf("Invalid crypto device name \n");
        return 1;
    }
    //get socket
    *socket_id = rte_cryptodev_socket_id(*dev_id);
    //get info
    rte_cryptodev_info_get(*dev_id, &dev_info);
    qp_num = dev_info.max_nb_queue_pairs;
    memset(&conf,0x00,sizeof(conf));
    conf.nb_queue_pairs = qp_num,
    conf.socket_id = *socket_id,
    ret = rte_cryptodev_configure(*dev_id, &conf);
    if(ret!=0)
    {
        printf("Config [%s] failed! \n",name);
        return 1;
    }
    
    memset(&qp_conf,0x00,sizeof(qp_conf));
    qp_conf.nb_descriptors = RSP_MAX_DEPTH/2;
    for (j = 0; j < qp_num; j++) 
    {//setup all QPs with MAX depth
        ret = rte_cryptodev_queue_pair_setup(*dev_id, j, &qp_conf, *socket_id);
        if (ret < 0)
        {
            printf("Failed to setup queue pair %u on " "cryptodev %u \n", j, *dev_id);
            return 1;
        }
    }
    return 0;
}

//#define _TEST_DATA_OFFSET 2
int main(int argc, char **argv)
{

    struct rte_cryptodev_info dev_info;
    uint32_t count, len;
    char *dev_name;
    int dev_id=0;
    int socket_id =0;
    int ret=0;
    int check_rst=0;
    enum rsp_op_e op;
    uint32_t lcore_id;
    int burst=0;
    int i=0;
    int qp_num=4;
    int cdev_id=0;
    rsp_context_t ctx[128];
    ret = rte_eal_init(argc, argv);
    if (ret < 0) rte_exit(EXIT_FAILURE, "Invalid EAL arguments!\n");
    argc -= ret;
    argv += ret;
    
    if((argc<2) || ((argc<5) && (atoi(argv[1]) != 0)) )
    {
        usage_print();
        return 0;
    }
    if(atoi(argv[1]) != 0)
    {
        op    = atoi(argv[1]);
        count = atoi(argv[2]);
        burst = atoi(argv[3]);
        len   = atoi(argv[4]);
        dev_name=argv[5];
        if(argc>6)
            check_rst=atoi(argv[6]);
        if(len>=MAX_MBUF_SIZE)
        {
            printf("Invalid size,The max size is MAX_MBUF_SIZE(%d) \n",MAX_MBUF_SIZE);
            return 0;
        }
        if(burst>RSP_MAX_DEPTH)
        {
            printf("Invalid brust size, max brust is %d \n",RSP_MAX_DEPTH);
            return 0;
        }
    }else{
            printf("\n\n Cryptodev Info:\n\n");
            for (cdev_id = 0; cdev_id < rte_cryptodev_count(); cdev_id++)
            {
                printf("    name=%s \n      feature:\n",rte_cryptodev_name_get(cdev_id));
                rte_cryptodev_info_get(cdev_id, &dev_info);
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_SYMMETRIC_CRYPTO?   "RTE_CRYPTODEV_FF_SYMMETRIC_CRYPTO=y"     :"RTE_CRYPTODEV_FF_SYMMETRIC_CRYPTO=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_ASYMMETRIC_CRYPTO?  "RTE_CRYPTODEV_FF_ASYMMETRIC_CRYPTO=y"    :"RTE_CRYPTODEV_FF_ASYMMETRIC_CRYPTO=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_HW_ACCELERATED=y"       :"RTE_CRYPTODEV_FF_HW_ACCELERATED=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_SECURITY=y"             :"RTE_CRYPTODEV_FF_SECURITY=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_RSA_PRIV_OP_KEY_EXP=y"  :"RTE_CRYPTODEV_FF_RSA_PRIV_OP_KEY_EXP=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_RSA_PRIV_OP_KEY_QT=y"   :"RTE_CRYPTODEV_FF_RSA_PRIV_OP_KEY_QT=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_ASYM_SESSIONLESS=y"     :"RTE_CRYPTODEV_FF_ASYM_SESSIONLESS=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_SYM_SESSIONLESS=y"      :"RTE_CRYPTODEV_FF_SYM_SESSIONLESS=n");
                printf("        %s\n",dev_info.feature_flags&RTE_CRYPTODEV_FF_HW_ACCELERATED?     "RTE_CRYPTODEV_FF_ASYM_SESSIONLESS=y"     :"RTE_CRYPTODEV_FF_ASYM_SESSIONLESS=n");    
            }
            return 0;
    }


    if( init_crypto_device(dev_name, &dev_id, &socket_id) != 0 )
    {
        rte_exit(EXIT_FAILURE, "Crypto init failed !\n");
    }
    //setup param for per lcore threads
    i=0;
    RTE_LCORE_FOREACH_SLAVE(lcore_id)
    {
        ctx[i].op           = op;
        ctx[i].lcore        = lcore_id;
        ctx[i].qp_id        = i;
        ctx[i].nb_pkt       = count;
        ctx[i].burst        = burst;
        ctx[i].size         = len;
        ctx[i].dev_name     = dev_name;
        ctx[i].dev_id       = dev_id;
        ctx[i].socket_id    = socket_id;
        ctx[i].check_rst    = check_rst;
        if(op <= SM1_CBC )
        {
            //cipher
            rte_eal_remote_launch(rsp_cipher_running,&ctx[i], lcore_id);
        }else if(op <= RSA_2048_DEC )
        {
            //asym RSA
            rte_eal_remote_launch(rsp_rsa_runing,&ctx[i], lcore_id);
        }else if(op <= ECC_SM2_VERIFY )
        {
            //ECC
            rte_eal_remote_launch(rsp_ecc_runing,&ctx[i], lcore_id);
        }else if(op <= BULK_TRNG_GEN )
        {
            //trng
            rte_eal_remote_launch(rsp_trng_runing,&ctx[i], lcore_id);
        }else if(op <= BULK_SM3_HMAC)
        {
            //HMAC support later
            rte_eal_remote_launch(rsp_hmac_runing,&ctx[i], lcore_id);
        }else if(op <= SM1_SM3)
        {
            //CHAIN cipher->auth
            rte_eal_remote_launch(rsp_cipher_auth_runing,&ctx[i], lcore_id);
        }else if(op <= SM3_SM1)
        {
            //CHAIN auth->cipher
            rte_eal_remote_launch(rsp_auth_cipher_runing,&ctx[i], lcore_id);
        }

        i++;
        if(i>=qp_num)break;//
    }

    printf("%20s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s \n",
                "algo",
                "lcore id",
                "buf size",
                "enqueued",
                "dequeued",
                "Failed enq",
                "failed deq",
                "MOps",
                "Gbps",
                "cycles/buf",
                "time(s)");

    //wait all threads end
    i=0;
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (i == qp_num)
            break;
        ret |= rte_eal_wait_lcore(lcore_id);
        i++;
    }
    return 0;
}
