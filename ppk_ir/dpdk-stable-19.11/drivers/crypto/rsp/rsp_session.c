#include <rte_memcpy.h>
#include <rte_common.h>
#include <rte_spinlock.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_crypto_sym.h>

#include "rsp_logs.h"
#include "rsp_session.h"
#include "rsp_sym_pmd.h"

/* SHA1 - 20 bytes - Initialiser state can be found in FIPS stds 180-2 */
static const uint8_t sha1InitialState[] = {
    0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x98, 0xba,
    0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 0xc3, 0xd2, 0xe1, 0xf0};

/* SHA 224 - 32 bytes - Initialiser state can be found in FIPS stds 180-2 */
static const uint8_t sha224InitialState[] = {
    0xc1, 0x05, 0x9e, 0xd8, 0x36, 0x7c, 0xd5, 0x07, 0x30, 0x70, 0xdd,
    0x17, 0xf7, 0x0e, 0x59, 0x39, 0xff, 0xc0, 0x0b, 0x31, 0x68, 0x58,
    0x15, 0x11, 0x64, 0xf9, 0x8f, 0xa7, 0xbe, 0xfa, 0x4f, 0xa4};

/* SHA 256 - 32 bytes - Initialiser state can be found in FIPS stds 180-2 */
static const uint8_t sha256InitialState[] = {
    0x6a, 0x09, 0xe6, 0x67, 0xbb, 0x67, 0xae, 0x85, 0x3c, 0x6e, 0xf3,
    0x72, 0xa5, 0x4f, 0xf5, 0x3a, 0x51, 0x0e, 0x52, 0x7f, 0x9b, 0x05,
    0x68, 0x8c, 0x1f, 0x83, 0xd9, 0xab, 0x5b, 0xe0, 0xcd, 0x19};

/* SHA 384 - 64 bytes - Initialiser state can be found in FIPS stds 180-2 */
static const uint8_t sha384InitialState[] = {
    0xcb, 0xbb, 0x9d, 0x5d, 0xc1, 0x05, 0x9e, 0xd8, 0x62, 0x9a, 0x29,
    0x2a, 0x36, 0x7c, 0xd5, 0x07, 0x91, 0x59, 0x01, 0x5a, 0x30, 0x70,
    0xdd, 0x17, 0x15, 0x2f, 0xec, 0xd8, 0xf7, 0x0e, 0x59, 0x39, 0x67,
    0x33, 0x26, 0x67, 0xff, 0xc0, 0x0b, 0x31, 0x8e, 0xb4, 0x4a, 0x87,
    0x68, 0x58, 0x15, 0x11, 0xdb, 0x0c, 0x2e, 0x0d, 0x64, 0xf9, 0x8f,
    0xa7, 0x47, 0xb5, 0x48, 0x1d, 0xbe, 0xfa, 0x4f, 0xa4};

/* SHA 512 - 64 bytes - Initialiser state can be found in FIPS stds 180-2 */
static const uint8_t sha512InitialState[] = {
    0x6a, 0x09, 0xe6, 0x67, 0xf3, 0xbc, 0xc9, 0x08, 0xbb, 0x67, 0xae,
    0x85, 0x84, 0xca, 0xa7, 0x3b, 0x3c, 0x6e, 0xf3, 0x72, 0xfe, 0x94,
    0xf8, 0x2b, 0xa5, 0x4f, 0xf5, 0x3a, 0x5f, 0x1d, 0x36, 0xf1, 0x51,
    0x0e, 0x52, 0x7f, 0xad, 0xe6, 0x82, 0xd1, 0x9b, 0x05, 0x68, 0x8c,
    0x2b, 0x3e, 0x6c, 0x1f, 0x1f, 0x83, 0xd9, 0xab, 0xfb, 0x41, 0xbd,
    0x6b, 0x5b, 0xe0, 0xcd, 0x19, 0x13, 0x7e, 0x21, 0x79};
    
int rsp_sym_session_configure(struct rte_cryptodev *dev,
        struct rte_crypto_sym_xform *xform,
        struct rte_cryptodev_sym_session *sess,
        struct rte_mempool *mempool)
{
    void *sess_priv;
    int ret;
    //get memory for sess private data
    if (rte_mempool_get(mempool, &sess_priv)) 
    {
        CDEV_LOG_ERR( "Couldn't get object from session mempool");
        return -ENOMEM;
    }

    ret = rsp_sym_session_set(dev, xform, sess_priv);
    if (ret != 0) 
    {
        RSP_LOG(ERR,
            "Crypto RSP PMD: failed to configure session parameters ret=%d ",ret);

        /* Return session to mempool */
        rte_mempool_put(mempool, sess_priv);
        return ret;
    }
    set_sym_session_private_data(sess, dev->driver_id, sess_priv);

    return 0;
}
unsigned int rsp_sym_session_get_private_size(
        struct rte_cryptodev *dev __rte_unused)
{
    //RSP_LOG(DEBUG, "sizeof(struct rsp_sym_session)=%d ", sizeof(struct rsp_sym_session));
    return RTE_ALIGN_CEIL(sizeof(struct rsp_sym_session), 8);
}

void rsp_sym_session_clear(struct rte_cryptodev *dev, struct rte_cryptodev_sym_session *sess)
{
    struct rte_mempool *sess_mp;
    uint8_t index = dev->driver_id;
    struct rsp_sym_session *sess_priv = get_sym_session_private_data(sess, index);

    if (sess_priv) {
        sess_mp = rte_mempool_from_obj(sess_priv);
        set_sym_session_private_data(sess, index, NULL);
        rte_mempool_put(sess_mp, (void *)sess_priv);
    }
}

struct rte_crypto_auth_xform *rsp_get_auth_xform(struct rte_crypto_sym_xform *xform)
{
    do {
        if (xform->type == RTE_CRYPTO_SYM_XFORM_AUTH)
            return &xform->auth;

        xform = xform->next;
    } while (xform);

    return NULL;
}

struct rte_crypto_cipher_xform *rsp_get_cipher_xform(struct rte_crypto_sym_xform *xform)
{
    do {
        if (xform->type == RTE_CRYPTO_SYM_XFORM_CIPHER)
            return &xform->cipher;

        xform = xform->next;
    } while (xform);

    return NULL;
}



int rsp_sym_session_set(struct rte_cryptodev *dev, 
                        struct rte_crypto_sym_xform *xform, 
                        void *sess_priv)
{
    int ret=0;
    int rsp_cmd_id;
    uint32_t feature_flags=0;

    feature_flags = dev->feature_flags;
    switch(xform->type)
    {
        case RTE_CRYPTO_SYM_XFORM_AUTH:

            if(xform->next == NULL)
            {
                ret = rsp_sym_configure_auth(dev, xform, sess_priv);
            }else{
                if(xform->next->type != RTE_CRYPTO_SYM_XFORM_CIPHER)
                {
                    RSP_LOG(ERR, "Currnet crypto dev just support AUTH->Cipher");
                    ret = -EINVAL;
                }else{
                    ret = rsp_sym_configure_chain(dev, xform, sess_priv);
                }
            }
            break;
        case RTE_CRYPTO_SYM_XFORM_CIPHER:
            if(xform->next == NULL)
            {
                if( (feature_flags & RSP_RTE_CRYPTODEV_HW_RPU)==0 )
                {
                    RSP_LOG(ERR, "Currnet crypto dev not support CIPHER (feature_flags=0x%08x)",feature_flags);
                    ret = -EINVAL;
                }else{
                    ret = rsp_sym_configure_cipher(dev, xform, sess_priv);
                }
            }else{
                //cipher->auth
                if(xform->next->type != RTE_CRYPTO_SYM_XFORM_AUTH)
                {
                    RSP_LOG(ERR, "Currnet crypto dev just support Cipher->AUTH");
                    ret = -EINVAL;
                }else{
                    ret = rsp_sym_configure_chain(dev, xform, sess_priv);
                }
            }
            break;
        case RTE_CRYPTO_SYM_XFORM_AEAD:
            if( (feature_flags & RSP_RTE_CRYPTODEV_HW_RPU)==0 )
            {
                RSP_LOG(ERR, "Currnet crypto dev not support AEAD");
                ret = -EINVAL;
            }else{
                ret = rsp_sym_configure_aead(dev, xform, sess_priv);
            }
            break;
        case RTE_CRYPTO_SYM_XFORM_NOT_SPECIFIED:
            RSP_LOG(ERR, "xform->type not specified");
            ret = -EINVAL;
            break;
        default:
            /* 
            */
            ret = -EINVAL;
            RSP_LOG(ERR, "Invalid xform->type");
            break;
    }


    return ret;
}

int rsp_sym_configure_auth(struct rte_cryptodev *dev,
                struct rte_crypto_sym_xform *xform,
                struct rsp_sym_session *session)
{
    struct rte_crypto_auth_xform *auth_xform = rsp_get_auth_xform(xform);
    struct rsp_sym_dev_private *internals = dev->data->dev_private;
    const uint8_t *key_data = auth_xform->key.data;
    uint8_t key_length = auth_xform->key.length;

    session->type=RSP_AUTH;
    session->auth.key.length = auth_xform->key.length;
    session->auth.key.data   = auth_xform->key.data;
    session->auth.digest_length = auth_xform->digest_length;

    switch (auth_xform->algo) {
    case RTE_CRYPTO_AUTH_MD5:
        session->rsp_hash_algo = RSP_HASH_ALGO_MD5;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;        
    case RTE_CRYPTO_AUTH_SHA1:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA1;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;
    case RTE_CRYPTO_AUTH_SHA224:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA224;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;
    case RTE_CRYPTO_AUTH_SHA256:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA256;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;
    case RTE_CRYPTO_AUTH_SHA384:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA384;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;
    case RTE_CRYPTO_AUTH_SHA512:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA512;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;
    case RSP_CRYPTO_AUTH_SM3:
        session->rsp_hash_algo = RSP_HASH_ALGO_SM3;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HASH;
        break;
    case RTE_CRYPTO_AUTH_MD5_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_MD5;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RTE_CRYPTO_AUTH_SHA1_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA1;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RTE_CRYPTO_AUTH_SHA224_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA224;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RTE_CRYPTO_AUTH_SHA256_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA256;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RTE_CRYPTO_AUTH_SHA384_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA384;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RTE_CRYPTO_AUTH_SHA512_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_SHA512;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RSP_CRYPTO_AUTH_SM3_HMAC:
        session->rsp_hash_algo = RSP_HASH_ALGO_SM3;
        session->rsp_auth_mode = RSP_AUTH_TYPE_HMAC;
        break;
    case RSP_CRYPTO_AUTH_TRNG:
        session->rsp_hash_algo = RSP_TRNG;
        break;
    case RTE_CRYPTO_AUTH_NULL:
        session->rsp_hash_algo = RSP_HASH_ALGO_NULL;
        break;

    default:
        RSP_LOG(ERR, "Crypto: Undefined Hash algo %u specified",
                auth_xform->algo);
        return -EINVAL;
    }
    return 0;
}
/*chain
*/
int rsp_sym_configure_chain(struct rte_cryptodev *dev,
        struct rte_crypto_sym_xform *xform,
        struct rsp_sym_session *session)
{
    int ret=0;
    if((xform->type == RTE_CRYPTO_SYM_XFORM_CIPHER) && 
       (xform->next->type == RTE_CRYPTO_SYM_XFORM_AUTH) )
    {
        ret = rsp_sym_configure_cipher(dev, xform, session);
        if(ret != 0)
           return ret;

        ret = rsp_sym_configure_auth(dev, xform->next, session);
        if(ret != 0)
            return ret;
        session->type=RSP_CIPHER_AUTH;
    }else{
        ret = rsp_sym_configure_auth(dev, xform, session);
        if(ret != 0)
           return ret;

        ret = rsp_sym_configure_cipher(dev, xform->next, session);
        if(ret != 0)
            return ret;
        session->type=RSP_AUTH_CIPHER;
    }
    switch(session->rsp_hash_algo)
    {
        case RSP_HASH_ALGO_SM3:
            session->rsp_cipher_mode=RSP_CIPHER_MODE_HMAC_SM3;
            break;
        case RSP_HASH_ALGO_SHA1:
            session->rsp_cipher_mode=RSP_CIPHER_MODE_HMAC_SHA1;
            break;
        case RSP_HASH_ALGO_SHA256:
            session->rsp_cipher_mode=RSP_CIPHER_MODE_HMAC_SHA256;
            break;
        default:
            ret=-EINVAL;
            break;
    }
    RSP_LOG(DEBUG, "session->type=%d \n",session->type);
    return ret;
}
/*cipher only
*/
int rsp_sym_configure_cipher(struct rte_cryptodev *dev,
        struct rte_crypto_sym_xform *xform,
        struct rsp_sym_session *session)
{
    struct rsp_sym_dev_private *priv = dev->data->dev_private;
    struct rte_crypto_cipher_xform *cipher_xform = NULL;
    int ret;

    /* Get cipher xform from crypto xform chain */
    cipher_xform = rsp_get_cipher_xform(xform);
    session->type=RSP_CIPHER;
    switch (cipher_xform->algo) 
    {
    case RSP_CRYPTO_CIPHER_SM1_CBC:
        session->rsp_cipher_algo = rsp_sym_get_sm1_algo(cipher_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid SM1 cipher key size");
            ret = -EINVAL;
            goto error_out;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_CBC;
        break;
    case RSP_CRYPTO_CIPHER_SM4_CBC:
        session->rsp_cipher_algo = rsp_sym_get_sm4_algo(cipher_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid SM4 cipher key size");
            ret = -EINVAL;
            goto error_out;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_CBC;
        break;
    case RTE_CRYPTO_CIPHER_AES_CBC:
        session->rsp_cipher_algo = rsp_sym_get_aes_algo(cipher_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid AES cipher key size");
            ret = -EINVAL;
            goto error_out;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_CBC;
        break;
    case RTE_CRYPTO_CIPHER_AES_CTR:
        session->rsp_cipher_algo = rsp_sym_get_aes_algo(cipher_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid AES cipher key size");
            ret = -EINVAL;
            goto error_out;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_CTR;
        break;
    case RTE_CRYPTO_CIPHER_AES_ECB:
        session->rsp_cipher_algo = rsp_sym_get_aes_algo(cipher_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid AES cipher key size");
            ret = -EINVAL;
            goto error_out;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_ECB;
        break;
    default:
        RSP_LOG(ERR, "Crypto: Unsupport Cipher specified %u\n",    cipher_xform->algo);
        ret = -EINVAL;
        goto error_out;
    }
    RSP_DP_HEXDUMP_LOG(DEBUG, "cipher key:", cipher_xform->key.data, cipher_xform->key.length);
    session->cipher.key.length = cipher_xform->key.length;
    session->cipher.key.data   = cipher_xform->key.data;
    session->cipher.iv.offset  = cipher_xform->iv.offset;
    session->cipher.iv.length  = cipher_xform->iv.length;

    if (cipher_xform->op == RTE_CRYPTO_CIPHER_OP_ENCRYPT)
        session->rsp_dir = RSP_ENCRYPT;
    else
        session->rsp_dir = RSP_DECRYPT;

    return 0;

error_out:
    return ret;
}
/*
    AEAD
*/
int rsp_sym_configure_aead(struct rte_cryptodev *dev,
                            struct rte_crypto_sym_xform *xform,
                            struct rsp_sym_session *session)
{
    struct rte_crypto_aead_xform *aead_xform = &xform->aead;
    enum rte_crypto_auth_operation crypto_operation;
    switch (aead_xform->algo) 
    {
    case RSP_CRYPTO_AEAD_SM4_GCM:
        session->rsp_cipher_algo = rsp_sym_get_sm4_algo(aead_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid AES key size");
            return -EINVAL;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_GCM;
        break;
    case RTE_CRYPTO_AEAD_AES_GCM:
        session->rsp_cipher_algo = rsp_sym_get_aes_algo(aead_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid AES key size");
            return -EINVAL;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_GCM;
        break;
    case RTE_CRYPTO_AEAD_AES_CCM:
    
        session->rsp_cipher_algo = rsp_sym_get_aes_algo(aead_xform->key.length);
        if (session->rsp_cipher_algo < 0)
        {
            RSP_LOG(ERR, "Invalid AES key size");
            return -EINVAL;
        }
        session->rsp_cipher_mode = RSP_CIPHER_MODE_GCM;
        break;
    default:
        RSP_LOG(ERR, "Crypto: Undefined AEAD specified %u\n",
                aead_xform->algo);
        return -EINVAL;
    }
    session->cipher.key.length = aead_xform->key.length;
    session->cipher.key.data   = aead_xform->key.data;

    session->cipher.iv.offset = xform->aead.iv.offset;
    session->cipher.iv.length = xform->aead.iv.length;

    session->cipher.aad.length= xform->aead.aad_length;
    session->cipher.tag.length= xform->aead.digest_length;
    
    if (aead_xform->op == RTE_CRYPTO_AEAD_OP_ENCRYPT)
        session->rsp_dir = RSP_ENCRYPT;
    else
        session->rsp_dir = RSP_DECRYPT;
    return 0;
}


int rsp_hash_get_digest_size( enum rsp_hash_algo_e hash_algo)
{
    int hash_digest_len=0;
    switch(hash_algo)
    {
        case  RSP_HASH_ALGO_SHA1:
            hash_digest_len = 20;
            break;
        case  RSP_HASH_ALGO_SHA224:
            hash_digest_len = 28;
            break;
        case  RSP_HASH_ALGO_SHA256:
            hash_digest_len = 32;
            break;        
        case  RSP_HASH_ALGO_SHA384:
            hash_digest_len = 48;
            break;
        case  RSP_HASH_ALGO_SHA512:
            hash_digest_len = 64;
            break;
        case  RSP_HASH_ALGO_SM3:
            hash_digest_len = 32;
            break;
        case  RSP_HASH_ALGO_MD5:
            hash_digest_len = 32;
            break;
        default:
            RSP_LOG(ERR, "invalid hash algo %u", hash_algo);
            hash_digest_len = -EFAULT;
            break;
    }   
    return hash_digest_len;
}

int rsp_hash_get_block_size(enum rsp_hash_algo_e hash_algo)
{
    int hash_block_len=0;
    switch(hash_algo)
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
            RSP_LOG(ERR, "invalid hash alg %u", hash_algo);
            hash_block_len = -EFAULT;
            break;
    }   
    return hash_block_len;
}

int rsp_sym_get_aes_algo(int key_len)
{
    switch (key_len) 
    {
    case RSP_128_KEY_SZ:
        return RSP_CIPHER_AES_128;
        break;
    case RSP_192_KEY_SZ:
        return RSP_CIPHER_AES_192;
        break;
    case RSP_256_KEY_SZ:
        return RSP_CIPHER_AES_256;
        break;
    default:
        RSP_LOG(ERR,"Invalid key size (%d)",key_len);
        return -EINVAL;
    }
}
int rsp_sym_get_sm4_algo(int key_len)
{
    switch (key_len) 
    {
    case RSP_128_KEY_SZ:
        return RSP_CIPHER_SM4;
        break;
    default:
        RSP_LOG(ERR,"Invalid key size (%d)",key_len);
        return -EINVAL;
    }
}

int rsp_sym_get_sm1_algo(int key_len)
{
    switch (key_len) 
    {
    case RSP_128_KEY_SZ:
        return RSP_CIPHER_SM1;
        break;
    default:
        RSP_LOG(ERR,"Invalid key size (%d)",key_len);
        return -EINVAL;
    }
}
