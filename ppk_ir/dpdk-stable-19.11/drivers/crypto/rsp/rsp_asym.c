#include <stdarg.h>
#include "rsp_asym.h"
#include "rsp_asym_sess.h"
int build_pke_req_msg(struct pke_req_st *req,
                      void *in_addr_hold_virt,  uint64_t in_addr_hold_phy,
                      void *out_addr_hold_virt, uint64_t out_addr_hold_phy,
                      uint32_t in_cnt,  param_t *in_param,
                      uint32_t out_cnt, param_t *out_param,
                      void *cb_data, uint32_t func_id, uint32_t kek);
static int rsp_asym_check_nonzero(rte_crypto_param n)
{
    if (n.length < 8) {
        /* Not a case for any cryptograpic function except for DH
         * generator which very often can be of one byte length
         */
        size_t i;

        if (n.data[n.length - 1] == 0x0) 
        {
            for (i = 0; i < n.length - 1; i++)
                if (n.data[i] != 0x0)
                    break;
            if (i == n.length - 1)
                return -(EINVAL);
        }
    } else if (*(uint64_t *)&n.data[n.length - 8] == 0) 
    {
        /* Very likely it is zeroed modulus */
        size_t i;

        for (i = 0; i < n.length - 8; i++)
            if (n.data[i] != 0x0)
                break;
        if (i == n.length - 8)
            return -(EINVAL);
    }

    return 0;
}

int build_pke_req_msg(struct pke_req_st *req,
                      void *in_addr_hold_virt,  uint64_t in_addr_hold_phy,
                      void *out_addr_hold_virt, uint64_t out_addr_hold_phy,
                      uint32_t in_cnt,  param_t *in_param,
                      uint32_t out_cnt, param_t *out_param,
                      void *cb_data, uint32_t func_id, uint32_t kek)
{
    int i=0;
    // uint64_t *p_vir = NULL;
    // uint64_t p_phy  = 0;
    // param_t  *q_vir = NULL;
    // uint64_t  q_phy = 0;

    // p_vir = in_addr_hold_virt;
    // p_phy = in_addr_hold_phy;

    // q_vir = (param_t  *)((void*)p_vir+512);
    // q_phy = p_phy+512;
    // for (i = 0; i < in_cnt; ++i)
    // {
    //     p_vir[i]     = (uint64_t)(q_phy+i*sizeof(param_t));
    //     q_vir[i].phy = in_param[i].phy;
    //     q_vir[i].len = (in_param[i].len + 15)&(~0xf);
    // }

    // p_vir = out_addr_hold_virt;
    // p_phy = out_addr_hold_phy;
    // q_vir = (param_t  *)((void*)p_vir+512);
    // q_phy = p_phy+512;
    // for (i = 0; i < out_cnt; ++i)
    // {
    //     p_vir[i]     = (uint64_t)(q_phy+i*sizeof(param_t));
    //     q_vir[i].phy = out_param[i].phy;
    //     q_vir[i].len = (out_param[i].len+15)&(~0xF);
    // }
    // LOG_DEBUG("in_addr_hold_phy=%llx \n",in_addr_hold_phy);
    // dump_buf("in_addr_hold",in_addr_hold_virt,8*in_cnt);
    // dump_buf("in_addr_hold",in_addr_hold_virt+512,16*in_cnt);
    // LOG_DEBUG("out_addr_hold_phy=%llx \n",out_addr_hold_phy);
    // dump_buf("out_addr_hold",out_addr_hold_virt,8*out_cnt);
    // dump_buf("out_addr_hold",out_addr_hold_virt+512,16*out_cnt);

    req->pke_mid.opaque_data_lo = (uint64_t)cb_data & 0xFFFFFFFF;
    req->pke_mid.opaque_data_hi = (uint64_t)cb_data>>32 & 0xFFFFFFFF;

    req->pke_mid.src_data_addr_lo = (uint64_t)in_addr_hold_phy & 0xFFFFFFFF;
    req->pke_mid.src_data_addr_hi = (uint64_t)in_addr_hold_phy>>32 & 0xFFFFFFFF;

    req->pke_mid.dst_data_addr_lo = (uint64_t)out_addr_hold_phy & 0xFFFFFFFF;
    req->pke_mid.dst_data_addr_hi = (uint64_t)out_addr_hold_phy>>32 & 0xFFFFFFFF;

    req->input_param_count    = in_cnt;
    req->output_param_count   = out_cnt;
    req->pke_cd_pars.func_id  = func_id;
    req->pke_hdr.service_type = COMN_REQ_CPM_PKE;
    req->pke_hdr.hdr_flags    = 0x80 | kek;

    // if((func_id == SM2_ENC_FUNC_ID) ||
    //    (func_id == SM2_DEC_FUNC_ID)) 
    //     req->reservd2 = in_param[3].len;
    return 0;
}
int rsp_asym_build_request(void *in_op, uint8_t *out_msg, void *cookie, void *param)
{
    uint8_t *rsp_n,*rsp_e,*rsp_d,*rsp_m,*rsp_r;
    uint32_t rsp_n_l,rsp_e_l,rsp_d_l,rsp_m_l,rsp_r_l;
    int ret;
    int index=0;
    uint32_t func_id=0; 
    uint32_t kek=0;
    int in_cnt=0;
    int out_cnt=0;
    size_t alg_size;
    struct rsp_asym_session *ctx;
    struct rte_crypto_op *op = (struct rte_crypto_op *)in_op;
    struct rte_crypto_asym_op *asym_op = op->asym;
    struct pke_req_st *req = (struct pke_req_st *)out_msg;
    struct rsp_asym_cookies *cookie_tmp = (struct rsp_asym_cookies *)cookie;
    int err = 0;
    struct rte_crypto_asym_xform *xform;
    
    op->status = RTE_CRYPTO_OP_STATUS_NOT_PROCESSED;

    if (op->sess_type == RTE_CRYPTO_OP_WITH_SESSION) 
    {
        if(op->asym->session == NULL)
        {
            RSP_LOG(ERR, "session is NULL !");
            op->status = RTE_CRYPTO_OP_STATUS_INVALID_ARGS;
            goto error;
        }
        ctx = (struct rsp_asym_session *) get_asym_session_private_data( op->asym->session, rsp_asym_driver_id);
        if (unlikely(ctx == NULL)) 
        {
            RSP_LOG(DEBUG, "Session has not been created for this device");
            op->status = RTE_CRYPTO_OP_STATUS_INVALID_ARGS;
            goto error;
        }else{
            xform = ctx->xform;
        }
    } else if (op->sess_type == RTE_CRYPTO_OP_SESSIONLESS) 
    {
        ;//xform = asym_op->xform;
        RSP_DP_LOG(ERR, "Invalid sessionless settings");
        op->status = RTE_CRYPTO_OP_STATUS_INVALID_SESSION;
        goto error;

    }else 
    {
        RSP_DP_LOG(ERR, "Invalid session/xform settings");
        op->status = RTE_CRYPTO_OP_STATUS_INVALID_SESSION;
        goto error;
    }

    switch(xform->xform_type)
    {
        case RTE_CRYPTO_ASYM_XFORM_RSA:
            if (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_ENCRYPT ||
                asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_VERIFY) 
            {
                ret = rsp_asym_check_nonzero(xform->rsa.n);
                if(ret!=0)
                {
                    RSP_LOG(ERR, "Empty modulus in RSA inverse, aborting this operation");
                    op->status = RTE_CRYPTO_OP_STATUS_INVALID_ARGS;
                    goto error;
                }
                alg_size = xform->rsa.n.length;
                cookie_tmp->alg_size = alg_size;
                
                if((asym_op->rsa.pad !=RTE_CRYPTO_RSA_PADDING_NONE) && (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_ENCRYPT) )
                {
                    RSP_LOG(ERR, "Just support RTE_CRYPTO_RSA_PADDING_NONE");
                    op->status = RTE_CRYPTO_OP_STATUS_INVALID_ARGS;
                    goto error;
                }
                //public modexp
                in_cnt = 3;
                out_cnt = 1;
                //n e m r
                rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-xform->rsa.n.length, 
                                                                        xform->rsa.n.data,
                                                                        xform->rsa.n.length);
                cookie_tmp->virt->in_param2[index++].len=xform->rsa.n.length;
                rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-xform->rsa.e.length, 
                                                                        xform->rsa.e.data,    
                                                                        xform->rsa.e.length);
                cookie_tmp->virt->in_param2[index++].len=xform->rsa.e.length;
                
                if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_ENCRYPT)
                {
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-asym_op->rsa.message.length, 
                                                                        asym_op->rsa.message.data,
                                                                        asym_op->rsa.message.length);
                    cookie_tmp->virt->in_param2[index++].len=asym_op->rsa.message.length;
                }else{
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-asym_op->rsa.sign.length, 
                                                                        asym_op->rsa.sign.data,
                                                                        asym_op->rsa.sign.length);
                    cookie_tmp->virt->in_param2[index++].len=asym_op->rsa.sign.length;
                }
                func_id = (alg_size==128)? RSA_1024_FUNC_ID : RSA_2048_FUNC_ID;
                cookie_tmp->virt->out_param2[0].len=alg_size;
            }else if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_DECRYPT ||
                     asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_SIGN) 
            {            
                //n d m r
                if((asym_op->rsa.pad !=RTE_CRYPTO_RSA_PADDING_NONE) && (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_SIGN) )
                {
                    RSP_LOG(ERR, "Just support RTE_CRYPTO_RSA_PADDING_NONE");
                    op->status = RTE_CRYPTO_OP_STATUS_INVALID_ARGS;
                    goto error;
                }
                index=0;
                if(xform->rsa.key_type == RTE_RSA_KEY_TYPE_EXP)
                {
                    ret = rsp_asym_check_nonzero(xform->rsa.n);
                    if(ret!=0)
                    {
                        RSP_LOG(ERR, "Empty modulus in RSA inverse, aborting this operation");
                        op->status = RTE_CRYPTO_OP_STATUS_INVALID_ARGS;
                        goto error;
                    }
                    alg_size = xform->rsa.n.length;
                    cookie_tmp->alg_size = alg_size;

                    in_cnt = 3;
                    out_cnt = 1;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-xform->rsa.n.length, 
                                                                            xform->rsa.n.data,
                                                                            xform->rsa.n.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.n.length;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-xform->rsa.d.length, 
                                                                            xform->rsa.d.data,
                                                                            xform->rsa.d.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.d.length;
                    
                    RSP_DP_HEXDUMP_LOG(DEBUG, "n:", xform->rsa.n.data, xform->rsa.n.length);
                    RSP_DP_HEXDUMP_LOG(DEBUG, "d:", xform->rsa.d.data, xform->rsa.d.length);
                    
                    func_id = (alg_size==128)? RSA_1024_FUNC_ID : RSA_2048_FUNC_ID;
                    cookie_tmp->virt->out_param2[0].len=alg_size;
                }else
                {//CRT
                    alg_size = xform->rsa.qt.dP.length;
                    cookie_tmp->alg_size = alg_size*2;
                    in_cnt = 6;
                    out_cnt = 1;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0] + alg_size - xform->rsa.qt.dP.length,
                            xform->rsa.qt.dP.data,
                            xform->rsa.qt.dP.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.qt.dP.length;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0] + alg_size - xform->rsa.qt.dQ.length,
                            xform->rsa.qt.dQ.data,
                            xform->rsa.qt.dQ.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.qt.dQ.length;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0] + alg_size - xform->rsa.qt.p.length,
                            xform->rsa.qt.p.data,
                            xform->rsa.qt.p.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.qt.p.length;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0] + alg_size - xform->rsa.qt.q.length,
                            xform->rsa.qt.q.data,
                            xform->rsa.qt.q.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.qt.q.length;
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0] + alg_size - xform->rsa.qt.qInv.length,
                            xform->rsa.qt.qInv.data,
                            xform->rsa.qt.qInv.length);
                            cookie_tmp->virt->in_param2[index++].len=xform->rsa.qt.qInv.length;

                    RSP_DP_HEXDUMP_LOG(DEBUG, "dP:", xform->rsa.qt.dP.data, xform->rsa.qt.dP.length);
                    RSP_DP_HEXDUMP_LOG(DEBUG, "dQ:", xform->rsa.qt.dQ.data, xform->rsa.qt.dQ.length);
                    RSP_DP_HEXDUMP_LOG(DEBUG, "p:", xform->rsa.qt.p.data, xform->rsa.qt.p.length);
                    RSP_DP_HEXDUMP_LOG(DEBUG, "q:", xform->rsa.qt.q.data, xform->rsa.qt.q.length);
                    RSP_DP_HEXDUMP_LOG(DEBUG, "qInv:", xform->rsa.qt.qInv.data, xform->rsa.qt.qInv.length);
                    func_id = (alg_size*2==128)? RSA_1024_CRT_FUNC_ID : RSA_2048_CRT_FUNC_ID;
                    cookie_tmp->virt->out_param2[0].len=alg_size*2;
                }
                if(asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_DECRYPT)
                {
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0]+cookie_tmp->alg_size-asym_op->rsa.cipher.length, asym_op->rsa.cipher.data, asym_op->rsa.cipher.length);
                    cookie_tmp->virt->in_param2[index++].len=asym_op->rsa.cipher.length;
                    RSP_DP_HEXDUMP_LOG(DEBUG, "cipher:", asym_op->rsa.cipher.data, asym_op->rsa.cipher.length);
                }else if (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_SIGN)
                {
                    rte_memcpy(&cookie_tmp->virt->in_data[index][0]+cookie_tmp->alg_size-asym_op->rsa.message.length, asym_op->rsa.message.data, asym_op->rsa.message.length);
                    cookie_tmp->virt->in_param2[index++].len=asym_op->rsa.message.length;
                    RSP_DP_HEXDUMP_LOG(DEBUG, "message:", asym_op->rsa.message.data, asym_op->rsa.message.length);
                }

            }else{
                RSP_LOG(ERR, "Invalid param");
                err = -EINVAL;
                goto error;
            }
            break;
        case RTE_CRYPTO_ASYM_XFORM_DH:
            RSP_LOG(ERR, "RTE_CRYPTO_ASYM_XFORM_ECDSA add later");
            err = -EINVAL;
            goto error;
            break;
        case RTE_CRYPTO_ASYM_XFORM_DSA:
            RSP_LOG(ERR, "RTE_CRYPTO_ASYM_XFORM_ECDSA add later");
            err = -EINVAL;
            goto error;
            break;
        case RTE_CRYPTO_ASYM_XFORM_MODINV:
            RSP_LOG(ERR, "RTE_CRYPTO_ASYM_XFORM_MODINV unsupport");
            err = -EINVAL;
            goto error;
            break;
        case RTE_CRYPTO_ASYM_XFORM_MODEX:
            alg_size = xform->modex.exponent.length;
            cookie_tmp->alg_size = alg_size;
            //n x m r
            in_cnt = 3;
            out_cnt = 1;
            
            rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-xform->modex.exponent.length, 
                                                            xform->modex.exponent.data,
                                                            xform->modex.exponent.length);
            cookie_tmp->virt->in_param2[index++].len=xform->modex.exponent.length;
            rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-xform->modex.modulus.length, 
                                                            xform->modex.modulus.data,
                                                            xform->modex.modulus.length);
            cookie_tmp->virt->in_param2[index++].len=xform->modex.modulus.length;

            rte_memcpy(&cookie_tmp->virt->in_data[index][0]+alg_size-asym_op->modex.base.length, 
                                                            asym_op->modex.base.data,
                                                            asym_op->modex.base.length);
            cookie_tmp->virt->in_param2[index++].len=asym_op->modex.base.length;

            RSP_DP_HEXDUMP_LOG(DEBUG, "exponet:", xform->modex.exponent.data, xform->modex.exponent.length);
            RSP_DP_HEXDUMP_LOG(DEBUG, "modules:", xform->modex.modulus.data, xform->modex.modulus.length);
            RSP_DP_HEXDUMP_LOG(DEBUG, "base:", asym_op->modex.base.data, asym_op->modex.base.length);
            
            func_id = (alg_size==128)? RSA_1024_FUNC_ID : RSA_2048_FUNC_ID;
            cookie_tmp->virt->out_param2[0].len=alg_size;
            break;
        //case RTE_CRYPTO_ASYM_XFORM_ECDSA:
        //    RSP_LOG(ERR, "RTE_CRYPTO_ASYM_XFORM_ECDSA add later");
        //    err = -EINVAL;
        //    goto error;
        //    break;
        //case RTE_CRYPTO_ASYM_XFORM_ECPM:
        //    RSP_LOG(ERR, "RTE_CRYPTO_ASYM_XFORM_ECPM unsupport");
        //    err = -EINVAL;
        //    goto error;
        //    break;
        default:
            RSP_LOG(ERR, "Invalid asymmetric crypto xform");
            err = -EINVAL;
            goto error;
            break;
    }

    build_pke_req_msg(req, 
              &cookie_tmp->virt+offsetof(struct rsp_asym_item, in_param),
              cookie_tmp->phy+offsetof(struct rsp_asym_item, in_param),
              &cookie_tmp->virt+offsetof(struct rsp_asym_item, out_param),
              cookie_tmp->phy+offsetof(struct rsp_asym_item, out_param),
              in_cnt,  NULL,
              out_cnt, NULL,
              (void *)op, func_id, kek);

    RSP_DP_HEXDUMP_LOG(DEBUG, "req:", req, sizeof(struct pke_req_st));
    //RSP_DP_HEXDUMP_LOG(DEBUG, "cookie_tmp->virt:",cookie_tmp->virt,sizeof(struct rsp_asym_item));
    return 0;
error:
    return -1;
}



void rsp_asym_process_response(void **op, uint8_t *resp, struct rsp_asym_cookies *cookie)
{
    struct rte_crypto_asym_xform *xform;
    struct rsp_asym_session *ctx;
    struct rte_crypto_asym_op *asym_op;
    struct pke_resp_st *resp_msg = (struct pke_resp_st *)resp;
    struct rte_crypto_op *rop = (struct rte_crypto_op *)
            ((uint64_t)(resp_msg->operate_data_h) << 32 | resp_msg->operate_data_l);
    asym_op = rop->asym;
    RSP_DP_HEXDUMP_LOG(DEBUG, "resp:", resp, 32);
    if (rop->sess_type == RTE_CRYPTO_OP_WITH_SESSION) 
    {
        ctx = (struct rsp_asym_session *)get_asym_session_private_data( rop->asym->session, rsp_asym_driver_id);
        xform = ctx->xform;
    } else if (rop->sess_type == RTE_CRYPTO_OP_SESSIONLESS) 
    {
        ;//xform = rop->asym->xform;
    }
    {
        
        if (xform->xform_type == RTE_CRYPTO_ASYM_XFORM_MODEX)
        {
            RSP_LOG(ERR,"RTE_CRYPTO_ASYM_XFORM_MODEX mode error");
        }else if (xform->xform_type == RTE_CRYPTO_ASYM_XFORM_MODINV)
        {
            RSP_LOG(ERR,"RTE_CRYPTO_ASYM_XFORM_MODINV mode error");
        }else if (xform->xform_type == RTE_CRYPTO_ASYM_XFORM_RSA)
        {
            if (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_ENCRYPT ||
                asym_op->rsa.op_type ==    RTE_CRYPTO_ASYM_OP_VERIFY) 
            {
                if(asym_op->rsa.pad != RTE_CRYPTO_RSA_PADDING_NONE)
                {
                    RSP_LOG(ERR,"RTE_CRYPTO_RSA_PADDING_NONE JUAT");
                }
                rte_memcpy(asym_op->rsa.cipher.data, cookie->virt->out_data[0], cookie->alg_size);
            }else if (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_DECRYPT)
            {
                if(asym_op->rsa.pad != RTE_CRYPTO_RSA_PADDING_NONE)
                {
                    RSP_LOG(ERR,"RTE_CRYPTO_RSA_PADDING_NONE JUAT");
                }
                rte_memcpy(asym_op->rsa.message.data, cookie->virt->out_data[0], cookie->alg_size);
            }else if (asym_op->rsa.op_type == RTE_CRYPTO_ASYM_OP_SIGN) 
            {
                rte_memcpy(asym_op->rsa.sign.data, cookie->virt->out_data[0], cookie->alg_size);
            }
        }
    }
    *op = rop;

    RSP_DP_HEXDUMP_LOG(DEBUG, "out data:", cookie->virt->out_data[0], cookie->alg_size);
}

