//#include <openssl/evp.h>

#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_crypto_sym.h>
#include <rte_bus_pci.h>
#include <rte_byteorder.h>

#include "rsp_sym.h"
#include "rsp_logs.h"

int build_chain_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len, uint32_t cipher_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint8_t mode, uint8_t dir, 
                        uint32_t c_align_offset, uint32_t a_align_offset,
                        const uint8_t *cipher_key, uint8_t cipher_key_len,
                        const uint8_t *auth_key, uint8_t auth_key_len,
                        uint8_t *iv,  uint8_t iv_len)
{
    int i =0;
    int ret=0;
    
    req->operate_data_l = (uint64_t)cb & 0xFFFFFFFF;
    req->operate_data_h = (uint64_t)cb>>32 & 0xFFFFFFFF;
    
    req->src_addr_l = rsp_cpu_to_be32((uint64_t)src_phy);
    req->src_addr_h = rsp_cpu_to_be32((((uint64_t)src_phy)>>32)&0xFFFFFFFF);
    
    req->dst_addr_l = rsp_cpu_to_be32((uint64_t)dst_phy);
    req->dst_addr_h = rsp_cpu_to_be32((((uint64_t)dst_phy)>>32)&0xFFFFFFFF);
    
    req->src_len = rsp_cpu_to_be32(src_len);
    req->dst_len = rsp_cpu_to_be32(dst_len);
    
    req->align_offset = a_align_offset;
    
    //cipher key
    if((cipher_key != NULL) && (cipher_key_len <= 32))
    {
        if(cipher_key_len <= 16)
        {
            memcpy(req->cipher_key_l, cipher_key,    cipher_key_len);
        }else
        {
            memcpy(req->cipher_key_l, cipher_key,    16);
            memcpy(req->cipher_key_h, cipher_key+16, cipher_key_len-16);
        }
    }
    
    //auth key
    if((auth_key != NULL) && (auth_key_len <= 32))//max auth key length is 32 bytes
    {
        if(auth_key_len <= 8)
        {
            memcpy(req->u1.auth_key_l, auth_key, auth_key_len);
        }else if(auth_key_len <= 16)
        {
            memcpy(req->u1.auth_key_l, auth_key,    8);
            memcpy(req->u2.auth_key_h, auth_key+8, auth_key_len-8);
        }else if(auth_key_len <= 32)
        {
            memcpy(req->u1.auth_key_l, auth_key,    8);
            memcpy(req->u2.auth_key_h, auth_key+8,  8);
            memcpy(req->u3.auth_key_hh, auth_key+16, auth_key_len-16);
        }
    }

    //IV
    if((iv != NULL) && (iv_len > 0) && (iv_len < 32))
    {
        memset(req->iv_l, 0x7F, iv_len);
        if(iv_len <= 16)
        {
            if(mode == RSP_CIPHER_MODE_GCM)
                req->iv_l[15]=1;
            memcpy(req->iv_l, iv, iv_len);
        }else
        {
            memcpy(req->iv_l,     iv,    16);
            memcpy(req->u3.iv_h,  iv+16, iv_len-16);
        }
    }
    
    req->cipher_type = algo;
    req->cipher_mode = mode;
    req->cipher_flag = dir;

    req->u4.cipher_len= (uint16_t)rsp_cpu_to_be16(cipher_len);
    return ret;
}
int build_cipher_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint8_t mode, uint8_t dir,
                        const uint8_t *key, uint8_t key_len,
                        uint8_t *iv,  uint8_t iv_len,
                        uint8_t *aad, uint8_t aad_len, uint32_t kek, uint32_t vf_number, uint8_t split_flag, uint16_t total_len, uint32_t offset)
{

    int i =0;
    int ret=0;

    req->operate_data_l = (uint64_t)cb & 0xFFFFFFFF;
    req->operate_data_h = (uint64_t)cb>>32 & 0xFFFFFFFF;

    req->src_addr_l = rsp_cpu_to_be32((uint64_t)src_phy);
    req->src_addr_h = rsp_cpu_to_be32((((uint64_t)src_phy)>>32)&0xFFFFFFFF);

    req->dst_addr_l = rsp_cpu_to_be32((uint64_t)dst_phy);
    req->dst_addr_h = rsp_cpu_to_be32((((uint64_t)dst_phy)>>32)&0xFFFFFFFF);

    req->src_len = rsp_cpu_to_be32(src_len+offset);
    req->dst_len = rsp_cpu_to_be32(dst_len+offset);
    
    req->align_offset = offset;
    
    if((key != NULL) && (key_len >0 ) && (key_len <= 32))
    {
        if(key_len <= 16)
        {
            memcpy(req->cipher_key_l, key, key_len);
        }else
        {
            memcpy(req->cipher_key_l, key,    16);
            memcpy(req->cipher_key_h, key+16, key_len-16);
        }
    }
    if((iv != NULL) && (iv_len > 0) && (iv_len < 32))
    {
        memset(req->iv_l, 0x7F, iv_len);
        if(iv_len <= 16)
        {
            if(mode == RSP_CIPHER_MODE_GCM)
                req->iv_l[15]=1;
            memcpy(req->iv_l, iv, iv_len);
        }
        else
        {
            memcpy(req->iv_l,     iv,    16);
            memcpy(req->u3.iv_h,  iv+16, iv_len-16);
        }
    }
    switch (algo) {
        case RSP_CIPHER_AES_128:
            req->cipher_type = 0;
            break;
        case RSP_CIPHER_AES_256:
            req->cipher_type = 1;
            break;
        case RSP_CIPHER_SM4:
            req->cipher_type = 2;
            break;
        case RSP_CIPHER_SM1:
            req->cipher_type = 8;
            break;
        default:
            req->cipher_type = 0xFF;
            break;
    }

    switch (mode) {
        case RSP_CIPHER_MODE_ECB:
            req->cipher_mode = 0;
            break;
        case RSP_CIPHER_MODE_CBC:
            req->cipher_mode = 1;
            break;
        case RSP_CIPHER_MODE_GCM:
            src_len-=offset;
            req->cipher_mode = 2;
            if ((split_flag == 2) || (split_flag == 3))
            {
                memcpy(req->u3.aad, aad, 16);
            }
            else
            {
                memcpy(req->u3.aad, aad, aad_len);
            }
            RSP_LOG(DEBUG,"aad_size = %d\n", aad_len);
            req->aad_size = rsp_cpu_to_be16(aad_len*8);
            if (src_len % 16 == 0) {
                memset(req->u1.gcm_mask_l, 0xFF, 8);
                memset(req->u2.gcm_mask_h, 0xFF, 8);
            }
            else if (src_len % 16 <= 8) {
                for (i = 0; i < (int)(src_len % 16); i++) {
                    req->u1.gcm_mask_l[i] = 0xFF;
                }
            }
            else if (src_len % 16 > 8) {
                memset(req->u1.gcm_mask_l, 0xFF, 8);
                for (i = 0; i < (int)(src_len % 16 - 8); i++) {
                    req->u2.gcm_mask_h[i] = 0xFF;
                }
            }

            req->split_flag = split_flag;
            if(total_len == src_len)
            {
                req->split_flag = 0;
            }
            req->u4.total_len = rsp_cpu_to_be16(total_len);
            break;
        case RSP_CIPHER_MODE_CTR:
            src_len-=offset;
            req->cipher_mode = 3;
            if (src_len % 16 == 0) {
                memset(req->u1.gcm_mask_l, 0xFF, 8);
                memset(req->u2.gcm_mask_h, 0xFF, 8);
            }
            else if (src_len % 16 <= 8) {
                for (i = 0; i < (int)(src_len % 16); i++) {
                    req->u1.gcm_mask_l[i] = 0xFF;
                }
            }
            else if (src_len % 16 > 8) {
                memset(req->u1.gcm_mask_l, 0xFF, 8);
                for (i = 0; i < (int)(src_len % 16 - 8); i++) {
                    req->u2.gcm_mask_h[i] = 0xFF;
                }
            }
            break;
        default:
            req->cipher_mode = 0xFF;
            break;
    }

    switch (dir) {
        case RSP_ENCRYPT:
            req->cipher_flag = 0;
            break;
        case RSP_DECRYPT:
            req->cipher_flag = 1;
            break;
        default:
            req->cipher_flag = 0xFF;
            break;
    }
    req->u4.cipher_len= (uint16_t)rsp_cpu_to_be16(src_len);
    return ret;
}
int build_rpu_auth_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        const uint8_t *auth_key, uint8_t auth_key_len,
                        uint8_t algo, uint8_t mode, uint32_t offset)
{

    int i =0;
    int ret=0;

    req->operate_data_l = (uint64_t)cb & 0xFFFFFFFF;
    req->operate_data_h = (uint64_t)cb>>32 & 0xFFFFFFFF;

    req->src_addr_l = rsp_cpu_to_be32((uint64_t)src_phy);
    req->src_addr_h = rsp_cpu_to_be32((((uint64_t)src_phy)>>32)&0xFFFFFFFF);

    req->dst_addr_l = rsp_cpu_to_be32((uint64_t)dst_phy);
    req->dst_addr_h = rsp_cpu_to_be32((((uint64_t)dst_phy)>>32)&0xFFFFFFFF);

    req->src_len = rsp_cpu_to_be32(src_len+offset);
    req->dst_len = rsp_cpu_to_be32(dst_len+offset);

    req->align_offset = offset;
    //auth key
    if((auth_key != NULL) && (auth_key_len <= 32))//max auth key length is 32 bytes
    {
        if(auth_key_len <= 8)
        {
            memcpy(req->u1.auth_key_l, auth_key, auth_key_len);
        }else if(auth_key_len <= 16)
        {
            memcpy(req->u1.auth_key_l, auth_key,    8);
            memcpy(req->u2.auth_key_h, auth_key+8, auth_key_len-8);
        }else if(auth_key_len <= 32)
        {
            memcpy(req->u1.auth_key_l, auth_key,    8);
            memcpy(req->u2.auth_key_h, auth_key+8,  8);
            memcpy(req->u3.auth_key_hh, auth_key+16, auth_key_len-16);
        }
    }
    
    req->cipher_type = 0xc;
    
    switch (algo) {

        case RSP_HASH_ALGO_SM3:
            req->cipher_mode = 7;
            break;
        case RSP_HASH_ALGO_SHA1:
            req->cipher_mode = 8;
            break;
        case RSP_HASH_ALGO_SHA256:
            req->cipher_mode = 9;
            break;
        default:
           req->cipher_type = 0xFF;
           break;
    }

    //req->cipher_mode = mode;//HASH / HMAC
    req->u4.cipher_len= (uint16_t)rsp_cpu_to_be16(src_len);
    return ret;
}


int rsp_sym_build_request(void *in_op, uint8_t *out_msg, void *op_cookie, void *param)
{
    int ret = 0;
    int i=0;
    uint32_t vf_num;
    
    uint8_t *aad;
    uint8_t aad_len;
    
    uint8_t tag_len;
    uint32_t kek=0;
    uint32_t vf_number=0;
    uint8_t  split_flag=0;
    uint16_t total_len=0;
    
    uint32_t align_offset=0;
    uint32_t align_offset1=0;
    
    uint8_t iv_az[16];
    uint8_t *iv=NULL;
    uint8_t iv_len;

    int      init_len=0;
    uint8_t *init_data=NULL;

    
    struct rsp_QP *QP = (struct rsp_QP *)param;
    struct rsp_sym_session *ctx;
    register struct cipher_req_st *req=(struct cipher_req_st *)out_msg;
    
    uint8_t *data=NULL;
    uint32_t data_length = 0;
    uint32_t data_offset = 0;

    uint32_t mac_len=0;
    uint32_t auth_length_pad=0;
    uint32_t auth_length = 0;
    uint32_t auth_offset = 0;
    
    uint8_t *key_data=NULL;
    uint32_t key_length = 0;

    uint8_t *a_key_data=NULL;
    uint32_t a_key_length = 0;

    uint8_t *p = NULL;
    uint8_t *q = NULL;
    
    uint8_t  none_block_size=0;
    uint32_t digest_length = 0;

    uint64_t src_iova = 0;
    uint64_t dst_iova = 0;
    void *src_virt=NULL;
    void *dst_virt=NULL;

    uint32_t block_size=0;
    uint32_t digest_len=0;

    struct rsp_sym_dev_private *priv = QP->rsp_dev->sym_dev_priv;
    struct rte_crypto_op *op = (struct rte_crypto_op *)in_op;
    struct rsp_sym_cookies *cookie = (struct rsp_sym_cookies *)op_cookie;
    vf_num = priv->vf_num;
	
    if (unlikely(op->type != RTE_CRYPTO_OP_TYPE_SYMMETRIC)) 
    {
        RSP_DP_LOG(ERR, "RSP PMD only supports symmetric crypto "
                "operation requests, op (%p) is not a "
                "symmetric operation.", op);
        return -EINVAL;
    }
    if (unlikely(op->sess_type == RTE_CRYPTO_OP_SESSIONLESS)) 
    {
        RSP_DP_LOG(ERR, "RSP PMD only supports session oriented requests, op (%p) is sessionless.", op);
        return -EINVAL;
    }


    if ( (op->sym->m_src==NULL) || (op->sym->m_src->nb_segs > 1) )
    {
        /*At now we just support single segs
        */
        RSP_DP_LOG(ERR, "nb_segs(%d)>1",op->sym->m_src->nb_segs);
        return -EINVAL;
    }

    /*get ctx
    */
    ctx = (struct rsp_sym_session *)get_sym_session_private_data(op->sym->session, rsp_sym_driver_id);
    if (ctx == NULL)
    {
        RSP_DP_LOG(ERR, "Session was not created for this device");
        return -EINVAL;
    }

    
    if(ctx->type == RSP_CIPHER)
    {
        data_offset = op->sym->cipher.data.offset;
        data_length = op->sym->cipher.data.length;
        if(data_length%16 != 0)
        {
            RSP_DP_LOG(ERR,"cipher data size must align 16");
            return -EINVAL;
        }
        if ( data_length > RSP_MAX_CIPHER_DATA_LENGTH )
        {
            RSP_DP_LOG(ERR, "data length(%d)>%d", data_length,RSP_MAX_CIPHER_DATA_LENGTH);
            return -EINVAL;
        }
        if (unlikely((op->sym->m_dst != NULL) && (op->sym->m_dst != op->sym->m_src)))
        {
            src_iova =    rte_pktmbuf_iova_offset(op->sym->m_src, data_offset);
            dst_iova =    rte_pktmbuf_iova_offset(op->sym->m_dst, data_offset);
            
        }else{
            //same buf
            src_iova =    rte_pktmbuf_iova_offset(op->sym->m_src, data_offset);
            dst_iova =    src_iova;
        }
        src_iova |= (uint64_t)vf_num << 56;
        dst_iova |= (uint64_t)vf_num << 56;

        align_offset = src_iova&0x0F;
        src_iova -= align_offset;
        dst_iova -= align_offset;

        RSP_DP_HEXDUMP_LOG(DEBUG, "Cipher Src:", rte_pktmbuf_mtod_offset(op->sym->m_src, uint8_t *,data_offset+align_offset), data_length);
        RSP_DP_LOG(DEBUG, "src_iova=0x%x dst_iova=0x%x ",src_iova, dst_iova);
        RSP_DP_LOG(DEBUG, "data_offset=%d align_offset=%d ",data_offset, align_offset);
        
        ////TAG record
        /*
            
        */
        ctx->cipher.tag.data = op->sym->aead.digest.data;
        tag_len = ctx->cipher.tag.length;
        RSP_DP_LOG(DEBUG,"tag_len=%d ",tag_len);
        ////IV 
        iv_len = ctx->cipher.iv.length;
        iv = rte_crypto_op_ctod_offset(op, uint8_t *, ctx->cipher.iv.offset);

        if(iv_len > 32)
        {
            RSP_DP_LOG(ERR, "invalid iv len ,max is %d ",32);
            return -EINVAL;
        }else if(iv_len == 0)
        {
            iv = iv_az;
            iv_len = AES_GCM_AZ_LEN;
            memset(iv,0x00,iv_len);
        }

        RSP_DP_HEXDUMP_LOG(DEBUG, "IV:", iv, iv_len);
        ////AAD
        if(ctx->rsp_cipher_mode == RSP_CIPHER_MODE_GCM)
        {
            aad_len = ctx->cipher.aad.length;
            aad = op->sym->aead.aad.data;
            if( (aad_len >16) || (aad_len <= 0) )
            {
                RSP_DP_LOG(ERR, "invalid %d aad len ,range is 1~16 ",aad_len);
                return -EINVAL;
            }
            RSP_DP_HEXDUMP_LOG(DEBUG, "aad:", aad, aad_len);
        }
        
        total_len = data_length;
        //set 
        int out_len = 0;
        out_len = (data_length+15)&(~0xF);
        build_cipher_req_msg(req ,
                            src_iova, data_length,
                            dst_iova, out_len+((tag_len==0)? 0:16), op,
                            ctx->rsp_cipher_algo, ctx->rsp_cipher_mode, ctx->rsp_dir,
                            ctx->cipher.key.data, ctx->cipher.key.length,
                            iv,  iv_len,
                            aad, aad_len, kek, vf_number, split_flag, total_len, align_offset);
        RSP_DP_HEXDUMP_LOG(DEBUG, "req:", req, 128);
    }else if(ctx->type == RSP_AUTH)
    {
        auth_offset = op->sym->auth.data.offset;
        auth_length = op->sym->auth.data.length;
        key_data    = ctx->auth.key.data;
        key_length  = ctx->auth.key.length;
        digest_length=ctx->auth.digest_length;
        if( (op->sym->m_src != NULL) && (op->sym->auth.digest.phys_addr != NULL) )
        {
            src_virt = rte_pktmbuf_mtod_offset(op->sym->m_src, uint8_t *,auth_offset);
            src_iova = rte_pktmbuf_iova_offset(op->sym->m_src, auth_offset);
            dst_iova = cookie->phy;

        }else{
            RSP_DP_LOG(ERR, "Auth need src and digest ");
            return -EINVAL;
        }
        src_iova |= (uint64_t)vf_num << 56;
        dst_iova |= (uint64_t)vf_num << 56;
        
        align_offset = src_iova&0x0F;
        
        src_iova -= align_offset;
        //dst_iova -= align_offset;
        
        RSP_DP_LOG(DEBUG, "src_buf_start=0x%x dst_buf_start=0x%x ",src_iova, dst_iova);
        RSP_DP_LOG(DEBUG, "auth_offset=%d align_offset=%d ",auth_offset, align_offset);

        op->sym->auth.digest.phys_addr = (rte_iova_t )cookie;
        switch(ctx->rsp_hash_algo)
        {
            case RSP_HASH_ALGO_SHA1:
            case RSP_HASH_ALGO_MD5:
            case RSP_HASH_ALGO_SHA224:
            case RSP_HASH_ALGO_SHA256:
            case RSP_HASH_ALGO_SHA384:
            case RSP_HASH_ALGO_SHA512:
            case RSP_HASH_ALGO_SM3:
                if(ctx->rsp_auth_mode == RSP_AUTH_TYPE_HASH)
                {
                    //HASH need src
                    if ( (op->sym->m_src==NULL) || (op->sym->m_src->nb_segs > 1) )
                    {
                        /*At now we just support single segs
                        */
                        RSP_DP_LOG(ERR, "nb_segs(%d)>1",op->sym->m_src->nb_segs);
                        return -EINVAL;
                    }else{
                        RSP_DP_LOG(ERR, "Support later ");
                        return -EINVAL;
                    }
                }else if(ctx->rsp_auth_mode == RSP_AUTH_TYPE_HMAC)
                {
#if 1
                    //                 *                                 *
                    //*----------------*----------------*----------------*----------------*----------------*----------------*
                    //*      ipad      *      text      *   padding-1    *       opad     *     HASH1      *    padding-2   *
                    //*----------------*----------------*----------------*----------------*----------------*----------------*
                    //                 *                                 *
                    p = src_virt;
                    //message
                    p += auth_length;
                    //padding-1
                    block_size = rsp_get_block_len(ctx->rsp_hash_algo);
                    none_block_size = (auth_length+64)%block_size;
                    q = p-none_block_size;
                    
                    p += hash_padding(q, none_block_size, block_size, auth_length+64);
                    auth_length = (int)((void*)p-(void *)(src_virt));
                    RSP_DP_HEXDUMP_LOG(DEBUG, "Auth src:", src_virt, auth_length);
#endif
                    if ( auth_length > RSP_MAX_AUTH_DATA_LENGTH )
                    {
                        RSP_DP_LOG(ERR, "data length(%d)>%d", data_length,RSP_MAX_AUTH_DATA_LENGTH);
                        return -EINVAL;
                    }
                    //HMAC
                    build_rpu_auth_req_msg(req ,
                        src_iova, auth_length,
                        dst_iova, digest_length, op,
                        ctx->auth.key.data, ctx->auth.key.length,
                         (uint8_t )ctx->rsp_hash_algo, RSP_AUTH_TYPE_HMAC, align_offset);
                }
                break;
            default:
                break;
        }
        RSP_DP_HEXDUMP_LOG(DEBUG, "req:", req, 128);
    }else if((ctx->type == RSP_CIPHER_AUTH) || (ctx->type == RSP_AUTH_CIPHER))
    {
        data_offset = op->sym->cipher.data.offset;
        data_length = op->sym->cipher.data.length;
        auth_offset = op->sym->auth.data.offset;
        auth_length = op->sym->auth.data.length;
        key_data    = ctx->auth.key.data;
        key_length  = ctx->auth.key.length;
        digest_length=ctx->auth.digest_length;


        if(data_length%16 != 0)
        {
            RSP_DP_LOG(ERR,"cipher data size must align 16");
            return -EINVAL;
        }
        ////IV 
        iv_len = ctx->cipher.iv.length;
        iv = rte_crypto_op_ctod_offset(op, uint8_t *, ctx->cipher.iv.offset);


        RSP_DP_HEXDUMP_LOG(DEBUG, "iv:", iv, 16);

        if (unlikely((op->sym->m_dst != NULL) && (op->sym->m_dst != op->sym->m_src)))
        {
            src_virt =    rte_pktmbuf_mtod_offset(op->sym->m_src, uint8_t *, auth_offset);
            src_iova =    rte_pktmbuf_iova_offset(op->sym->m_src, auth_offset);
            dst_iova =    rte_pktmbuf_iova_offset(op->sym->m_dst, auth_offset);
        }else{
            //same buf
            src_virt =    rte_pktmbuf_mtod_offset(op->sym->m_src, uint8_t *, auth_offset);
            src_iova =    rte_pktmbuf_iova_offset(op->sym->m_src, auth_offset);
            dst_iova =    src_iova;
        }
        src_iova |= (uint64_t)vf_num << 56;
        dst_iova |= (uint64_t)vf_num << 56;

        align_offset = src_iova&0x0F;

        src_iova -= align_offset;
        dst_iova -= align_offset;
        
        RSP_DP_LOG(DEBUG, "after align src_iova=0x%x dst_iova=0x%x ",src_iova, dst_iova);
        RSP_DP_LOG(DEBUG, "auth_length=%d auth_offset=%d data_offset=%d align_offset=%d ",auth_length, auth_offset, data_offset, align_offset);

        block_size = rsp_get_block_len(ctx->rsp_hash_algo);
        none_block_size = (auth_length)%block_size;
        p = src_virt+auth_length;
        q = p-none_block_size;
		
        p += hash_padding(q, none_block_size, block_size, auth_length+64);
        auth_length_pad = p-(uint8_t *)src_virt;//update auth length

        
        RSP_DP_HEXDUMP_LOG(DEBUG, "Auth Src:", src_virt-align_offset, auth_length_pad+align_offset);

        uint32_t c_align_offset;
        uint32_t a_align_offset;
        c_align_offset = (data_offset-auth_offset)+align_offset;
        a_align_offset = align_offset;
        RSP_DP_LOG(DEBUG,"c_align_offset=%d a_align_offset=%d ", c_align_offset, a_align_offset);
        mac_len=rsp_get_hmac_len(ctx->rsp_hash_algo);
        
        if ( (auth_length_pad + align_offset) > RSP_MAX_AUTH_DATA_LENGTH )
        {
            RSP_DP_LOG(ERR, "data length(%d)>%d", (auth_length_pad + align_offset),RSP_MAX_AUTH_DATA_LENGTH);
            return -EINVAL;
        }

        build_chain_req_msg(req ,
                        src_iova, auth_length_pad + align_offset, data_length,
                        dst_iova, auth_length+mac_len + align_offset, op,
                        ctx->rsp_cipher_algo, ctx->rsp_cipher_mode, ctx->rsp_dir, c_align_offset, a_align_offset,
                        ctx->cipher.key.data, ctx->cipher.key.length,
                        ctx->auth.key.data,   ctx->auth.key.length,
                        iv, iv_len);

        RSP_DP_HEXDUMP_LOG(DEBUG, "req:", req, 128);
    }else
    {
        RSP_DP_LOG(ERR,"unsport session type (%d) ", ctx->type);
    }
    return 0;
}
void rsp_sym_process_response(void **op, uint8_t *resp, struct rsp_sym_cookies *cookie)
{
    int timeout=100000;
    uint8_t temp[32];
    struct rsp_sym_session *ctx;
    uint32_t data_length = 0, data_offset = 0, align_offset=0;
    uint8_t *data=NULL;
    uint8_t *tag;
    struct cipher_resp_st *resp_msg = (struct cipher_resp_st *)resp;
    struct rte_crypto_op *rop = (struct rte_crypto_op *)
        ((uint64_t)(resp_msg->operate_data_h) << 32 | resp_msg->operate_data_l);

    //RSP_DP_HEXDUMP_LOG(DEBUG, "rsp_response:", (uint8_t *)resp_msg, sizeof(struct cipher_resp_st));

    if (0) 
    {
        rop->status = RTE_CRYPTO_OP_STATUS_AUTH_FAILED;
    } else 
    {
        rop->status = RTE_CRYPTO_OP_STATUS_SUCCESS;
    }
    *op = (void *)rop;
    
    //GCM only
    /*get ctx
    */
    ctx = (struct rsp_sym_session *)get_sym_session_private_data(rop->sym->session, rsp_sym_driver_id);

    if (unlikely(ctx == NULL)) 
    {
        RSP_DP_LOG(ERR, "Session was not created for this device");
        return;
    }
    if( ctx->rsp_cipher_mode == RSP_CIPHER_MODE_GCM)
    {
        data_offset = rop->sym->cipher.data.offset;
        data_length = rop->sym->cipher.data.length;
        //copy tag to ctx->cipher_tag.data
        if (unlikely((rop->sym->m_dst != NULL) && (rop->sym->m_dst != rop->sym->m_src)))
        {
            data =    rte_pktmbuf_mtod_offset(rop->sym->m_dst, uint8_t *, data_offset);
        }else{
            //same buf
            data =    rte_pktmbuf_mtod_offset(rop->sym->m_src, uint8_t *, data_offset);
        }
        RSP_DP_HEXDUMP_LOG(DEBUG, "data0:", data, data_length);
        data += data_length;
        tag = (uint8_t *)((uint64_t)(data+15)&(~0xF));//16byte align
        RSP_DP_HEXDUMP_LOG(DEBUG, "Tag:", tag, 16);
        if(rop->sym->aead.digest.data != NULL)
            memcpy(rop->sym->aead.digest.data, tag, 16);
    }
    if((ctx->type == RSP_CIPHER_AUTH) || (ctx->type == RSP_AUTH_CIPHER) )
    {
        data_offset = rop->sym->auth.data.offset;
        data_length = rop->sym->auth.data.length;
        if (unlikely((rop->sym->m_dst != NULL) && (rop->sym->m_dst != rop->sym->m_src)))
        {
            data =    rte_pktmbuf_mtod_offset(rop->sym->m_dst, uint8_t *, data_offset);
            RSP_DP_HEXDUMP_LOG(DEBUG, "data1:", data, data_length);
        }else{
            //same buf
            data =    rte_pktmbuf_mtod_offset(rop->sym->m_src, uint8_t *, data_offset);
            RSP_DP_HEXDUMP_LOG(DEBUG, "data2:", data, data_length + 32);
        }

        if((data != NULL) && (rop->sym->auth.digest.data != NULL) )
            memcpy(rop->sym->auth.digest.data, data+data_length,ctx->auth.digest_length);
    }
    if(ctx->type == RSP_AUTH)
    {
        cookie = (struct rsp_sym_cookies *)(rop->sym->auth.digest.phys_addr);
        data_offset = rop->sym->auth.data.offset;
        align_offset = rte_pktmbuf_iova_offset(rop->sym->m_src, data_offset) & 0xF;
        RSP_DP_HEXDUMP_LOG(DEBUG, "data3:", cookie->virt->src, ctx->auth.digest_length);
        memcpy(rop->sym->auth.digest.data, cookie->virt->src+align_offset, ctx->auth.digest_length);
    }
}
