//#include <openssl/evp.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <rte_crypto_sym.h>
#include <rte_bus_pci.h>
#include <rte_byteorder.h>
#include <rte_cryptodev_pmd.h>
#include "rsp_bulk.h"
#include "rsp_logs.h"
#include "rsp_common.h"
#include "rsp_asym.h"
int build_bulk_trng_req_msg(struct hash_req_st *req ,
                        uint64_t cd_phy,  void *cd_vir,
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint32_t kek)
{
    //TRNG demo
    auth_cd_t *cd = cd_vir;
    int i =0;
    int ret=0;
    //cd 
    cd->auth_algo = 3;
    cd->auth_mode = 2;
    cd->auth_type = 0;
    req->cd_pars.content_desc_addr_lo   = (uint32_t)(cd_phy & 0xFFFFFFFF);
    req->cd_pars.content_desc_addr_hi   = (uint32_t)(cd_phy<<32 & 0xFFFFFFFF);
    req->cd_pars.content_desc_params_sz = 1;
    RSP_DP_HEXDUMP_LOG(DEBUG, "cd_pars:", cd_vir,sizeof(auth_cd_t));
    //hdr
    req->comm_hdr.service_cmd_id            = LA_CMD_RAND_GET_RANDOM;
    req->comm_hdr.service_type              = COMN_REQ_CPM_LA;
    req->comm_hdr.hdr_flags                 = 0x80 |algo<<3| kek | (1 << 5);
    req->comm_hdr.serv_specif_flags.iv_field    = 1;
    req->comm_hdr.serv_specif_flags.gcm_iv_len  = 0;
    req->comm_hdr.serv_specif_flags.proto_flags = 0;
    req->comm_hdr.serv_specif_flags.partial     = LA_PARTIAL_NONE;
    //mid
    req->comm_mid.src_data_addr_lo = (uint32_t)(dst_phy & 0xFFFFFFFF);
    req->comm_mid.src_data_addr_hi = (uint32_t)(dst_phy>>32 & 0xFFFFFFFF);
    req->comm_mid.dst_data_addr_lo = (uint32_t)(dst_phy & 0xFFFFFFFF);
    req->comm_mid.dst_data_addr_hi = (uint32_t)(dst_phy>>32 & 0xFFFFFFFF);
    req->comm_mid.src_len          = ((dst_len+31)&(~0x1F))/8;
    req->comm_mid.dst_len          = ((dst_len+31)&(~0x1F))/8;
    req->comm_mid.opaque_data_lo   = (uint32_t)((uint64_t)cb & 0xFFFFFFFF);
    req->comm_mid.opaque_data_hi   = (uint32_t)((uint64_t)cb>>32 & 0xFFFFFFFF);
    //cd_ctrl
    req->cd_ctrl.cipher_state_sz   = 16/8;
    req->cd_ctrl.cipher_key_sz     = 16/8;
    req->cd_ctrl.final_sz          = 0;
    req->cd_ctrl.hash_total_len    = 0;
    req->cd_ctrl.current_msg_len   = 0;

    return ret;
}
int build_auth_req_msg(struct hash_req_st *req , 
                       uint32_t block_len, 
                       uint32_t up_len,
                       uint64_t src_phy, uint32_t src_len, 
                       uint64_t dst_phy, uint32_t dst_len,
                       uint64_t cd_phy,  void *cd_vir,
                       uint8_t algo, void *cb, 
                       uint32_t total_len, int hash_final, int op_type, uint32_t kek )
{
    auth_cd_t *cd = cd_vir;
    int i =0;
    int ret=0;
    int dgst=0;
    dgst = rsp_get_dgst_len(algo);

    //cd 
    cd->auth_algo = algo;// == AUTH_ALGO_SHA384 ? AUTH_ALGO_SHA512:algo;

    cd->auth_mode = hash_final==0? NOPADDING:PADDING;
    cd->auth_type = op_type;
    req->cd_pars.content_desc_addr_lo   = (uint32_t)(cd_phy & 0xFFFFFFFF);
    req->cd_pars.content_desc_addr_hi   = (uint32_t)(cd_phy<<32 & 0xFFFFFFFF);
    req->cd_pars.content_desc_params_sz = 1;
    RSP_DP_HEXDUMP_LOG(DEBUG, "cd_pars",cd_vir,sizeof(auth_cd_t));
    //hdr
    req->comm_hdr.service_cmd_id            = LA_CMD_AUTH;
    req->comm_hdr.service_type              = COMN_REQ_CPM_LA;
    req->comm_hdr.hdr_flags                 = 0x80 | kek;;
    if((AUTH_TYPE_HMAC == op_type) || (AUTH_TYPE_PRF == op_type))
        req->comm_hdr.serv_specif_flags.partial=0;
    else
        req->comm_hdr.serv_specif_flags.partial = hash_final==0? LA_PARTIAL_MID:LA_PARTIAL_END;
    //mid
    req->comm_mid.src_data_addr_lo = (uint32_t)(src_phy & 0xFFFFFFFF);
    req->comm_mid.src_data_addr_hi = (uint32_t)(src_phy>>32 & 0xFFFFFFFF);
    req->comm_mid.dst_data_addr_lo = (uint32_t)(dst_phy & 0xFFFFFFFF);
    req->comm_mid.dst_data_addr_hi = (uint32_t)(dst_phy>>32 & 0xFFFFFFFF);
    req->comm_mid.src_len          = ((src_len+block_len-1)&~(block_len-1))/8;
    
    if(AUTH_TYPE_PRF == op_type)
        dst_len=((dst_len+(dgst-1))/dgst)*dgst;
    
    req->comm_mid.dst_len          = ((dst_len+15)&(~0xF))/8;
    req->comm_mid.opaque_data_lo   = (uint32_t)((uint64_t)cb & 0xFFFFFFFF);
    req->comm_mid.opaque_data_hi   = (uint32_t)((uint64_t)cb>>32 & 0xFFFFFFFF);
    //cd_ctrl
    req->cd_ctrl.cipher_state_sz   = block_len/8;
    req->cd_ctrl.cipher_key_sz     = up_len/8;
    req->cd_ctrl.final_sz          = dst_len;
    req->cd_ctrl.hash_total_len    = total_len;
    req->cd_ctrl.current_msg_len   = src_len;

    return ret;
}
int rsp_bulk_build_request(void *in_op, uint8_t *out_msg, void *op_cookie, void *param)
{
    uint8_t *aad;
    uint8_t aad_len;
    uint8_t tag_len;
    uint32_t kek=0;
    uint32_t vf_number=0;
    uint8_t split_flag=0;
    uint16_t total_len=0;
    uint8_t iv_az[16];
    uint8_t *iv;
    uint8_t iv_len;
    int ret = 0;
    uint16_t digest_length;
    uint32_t vf_num;
    struct rsp_QP *QP = (struct rsp_QP *)param;
    struct rsp_sym_session *ctx;
    register struct hash_req_st *req=(struct hash_req_st *)out_msg;
    uint8_t do_auth = 0, do_cipher = 0, do_aead = 0;
    uint32_t cipher_len = 0, cipher_ofs = 0;
    uint32_t data_length = 0, data_offset = 0;
    uint32_t key_length = 0;
    uint8_t  *key_data;
    auth_cd_t *cd = NULL;
    uint64_t src_buf_start = 0, dst_buf_start = 0;

    struct rsp_bulk_dev_private *priv = QP->rsp_dev->bulk_dev_priv;
    vf_num = priv->vf_num;
    struct rte_crypto_op *op = (struct rte_crypto_op *)in_op;
    struct rsp_bulk_cookies *cookie_tmp = (struct rsp_bulk_cookies *)op_cookie;

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
    /*
        get ctx
    */
    ctx = (struct rsp_sym_session *)get_sym_session_private_data(op->sym->session, rsp_bulk_driver_id);

    if (unlikely(ctx == NULL)) 
    {
        RSP_DP_LOG(ERR, "Session was not created for this device");
        return -EINVAL;
    }
    data_offset = op->sym->auth.data.offset;
    data_length = op->sym->auth.data.length;
    key_data    = ctx->auth.key.data;
    key_length  = ctx->auth.key.length;
    digest_length=ctx->auth.digest_length;

    src_buf_start =    rte_pktmbuf_iova_offset(op->sym->m_src, data_offset);
    printf("data_offset=0x%x \n",data_offset);
    RSP_DP_HEXDUMP_LOG(DEBUG, "Src1:", rte_pktmbuf_mtod_offset(op->sym->m_src, uint8_t *,data_offset), data_length);
    dst_buf_start =    op->sym->auth.digest.phys_addr;
    //HASH
    switch(ctx->rsp_hash_algo)
    {
        case RSP_HASH_ALGO_SHA1:
        case RSP_HASH_ALGO_MD5:
        case RSP_HASH_ALGO_SHA224:
        case RSP_HASH_ALGO_SHA256:
        case RSP_HASH_ALGO_SHA384:
        case RSP_HASH_ALGO_SHA512:
        case RSP_HASH_ALGO_SM3:
            return -EINVAL;//bulk hash not used
            if(ctx->rsp_auth_mode == RSP_AUTH_TYPE_HASH)
            {
                //HASH need src
                if ( (op->sym->m_src==NULL) || (op->sym->m_src->nb_segs > 1) )
                {
                    /*At now we just support single segs
                    */
                    RSP_DP_LOG(ERR, "nb_segs(%d)>1",op->sym->m_src->nb_segs);
                    return -EINVAL;
                }
            }else if(ctx->rsp_auth_mode == RSP_AUTH_TYPE_HMAC)
            {
                //HMAC
                cd = (auth_cd_t *)cookie_tmp->virt;
                memcpy(cd->auth_data, key_data, key_length);
                build_auth_req_msg( req ,
                        rsp_get_block_len(ctx->rsp_hash_algo), 
                        ((key_length+(15))&(~0xf)),
                        src_buf_start, data_length, 
                        dst_buf_start, digest_length, 
                        cookie_tmp->phy,  cookie_tmp->virt,
                        (uint8_t )ctx->rsp_hash_algo, op,
                        data_length, 1, AUTH_TYPE_HMAC, 0);
            }
            break;
        case RSP_TRNG:
            dst_buf_start =    op->sym->auth.digest.phys_addr;
            build_bulk_trng_req_msg(req ,
                        cookie_tmp->phy,  cookie_tmp->virt,
                        dst_buf_start, data_length, op,
                        3, 0);
        break;
    }
    RSP_DP_HEXDUMP_LOG(DEBUG, "req:", req, 128);

    return 0;
}
void rsp_bulk_process_response(void **op, uint8_t *resp, struct rsp_bulk_cookies *cookie)
{
    struct rsp_sym_session *ctx;
    uint32_t data_length = 0, data_offset = 0;
    uint8_t *data;
    uint8_t *tag;
    struct cipher_resp_st *resp_msg = (struct cipher_resp_st *)resp;
    struct rte_crypto_op *rop = (struct rte_crypto_op *)
        ((uint64_t)(resp_msg->operate_data_h) << 32 | resp_msg->operate_data_l);

    if (0) 
    {
        rop->status = RTE_CRYPTO_OP_STATUS_AUTH_FAILED;
    } else 
    {
        rop->status = RTE_CRYPTO_OP_STATUS_SUCCESS;
    }
    *op = (void *)rop;
}
