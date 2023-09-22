#include "rsp_asym_sess.h"
#include "rsp_logs.h"


int rsp_asym_session_configure(struct rte_cryptodev *dev,
        struct rte_crypto_asym_xform *xform,
        struct rte_cryptodev_asym_session *sess,
        struct rte_mempool *mempool)
{
    int err = 0;
    void *sess_private_data;
    struct rsp_asym_session *session;

    if (rte_mempool_get(mempool, &sess_private_data)) 
    {
        RSP_LOG(ERR, "Couldn't get object from session mempool");
        return -ENOMEM;
    }
    session = sess_private_data;
    switch(xform->xform_type)
    {
        case RTE_CRYPTO_ASYM_XFORM_RSA:
            //sign verify enc dec
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
    session->xform = xform;
    set_asym_session_private_data(sess, dev->driver_id, session);

    return 0;
error:
    rte_mempool_put(mempool, sess_private_data);
    return err;
}

unsigned int rsp_asym_session_get_private_size(
        struct rte_cryptodev *dev __rte_unused)
{
    RSP_LOG(DEBUG,"sizeof(struct rsp_asym_session)=%d ",(int)sizeof(struct rsp_asym_session));
    return RTE_ALIGN_CEIL(sizeof(struct rsp_asym_session), 8);
}

void rsp_asym_session_clear(struct rte_cryptodev *dev,
        struct rte_cryptodev_asym_session *sess)
{
    uint8_t index = dev->driver_id;
    void *sess_priv = get_asym_session_private_data(sess, index);
    struct rsp_asym_session *s = (struct rsp_asym_session *)sess_priv;

    if (sess_priv) 
    {
        memset(s, 0, rsp_asym_session_get_private_size(dev));
        struct rte_mempool *sess_mp = rte_mempool_from_obj(sess_priv);

        set_asym_session_private_data(sess, index, NULL);
        rte_mempool_put(sess_mp, sess_priv);
    }
}
