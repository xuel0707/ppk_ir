#ifndef _RSP_SYM_CAPABILITIES_H_
#define _RSP_SYM_CAPABILITIES_H_

#define RSP_BASE_SYM_CAPABILITIES                    \
    {    /* AES GCM */                        \
        .op = RTE_CRYPTO_OP_TYPE_SYMMETRIC,            \
        {.sym = {                        \
            .xform_type = RTE_CRYPTO_SYM_XFORM_AEAD,    \
            {.aead = {                    \
                .algo = RTE_CRYPTO_AEAD_AES_GCM,    \
                .block_size = 16,            \
                .key_size = {                \
                    .min = 16,            \
                    .max = 32,            \
                    .increment = 8            \
                },                    \
                .digest_size = {            \
                    .min = 8,            \
                    .max = 16,            \
                    .increment = 4            \
                },                    \
                .aad_size = {                \
                    .min = 0,            \
                    .max = 16,            \
                    .increment = 1            \
                },                    \
                .iv_size = {                \
                    .min = 0,            \
                    .max = 12,            \
                    .increment = 12            \
                },                    \
            }, }                        \
        }, }                            \
    },                                \
    {    /* AES CBC */                        \
        .op = RTE_CRYPTO_OP_TYPE_SYMMETRIC,            \
        {.sym = {                        \
            .xform_type = RTE_CRYPTO_SYM_XFORM_CIPHER,    \
            {.cipher = {                    \
                .algo = RTE_CRYPTO_CIPHER_AES_CBC,    \
                .block_size = 16,            \
                .key_size = {                \
                    .min = 16,            \
                    .max = 32,            \
                    .increment = 8            \
                },                    \
                .iv_size = {                \
                    .min = 16,            \
                    .max = 16,            \
                    .increment = 0            \
                }                    \
            }, }                        \
        }, }                            \
    },                                \
    {    /* AES CTR */                        \
        .op = RTE_CRYPTO_OP_TYPE_SYMMETRIC,            \
        {.sym = {                        \
            .xform_type = RTE_CRYPTO_SYM_XFORM_CIPHER,    \
            {.cipher = {                    \
                .algo = RTE_CRYPTO_CIPHER_AES_CTR,    \
                .block_size = 16,            \
                .key_size = {                \
                    .min = 16,            \
                    .max = 32,            \
                    .increment = 8            \
                },                    \
                .iv_size = {                \
                    .min = 16,            \
                    .max = 16,            \
                    .increment = 0            \
                }                    \
            }, }                        \
        }, }                            \
    },                               \
    {    /* SHA1 HMAC */                        \
        .op = RTE_CRYPTO_OP_TYPE_SYMMETRIC,            \
        {.sym = {                        \
            .xform_type = RTE_CRYPTO_SYM_XFORM_AUTH,    \
            {.auth = {                    \
                .algo = RTE_CRYPTO_AUTH_SHA1_HMAC,    \
                .block_size = 64,            \
                .key_size = {                \
                    .min = 1,            \
                    .max = 64,            \
                    .increment = 1            \
                },                    \
                .digest_size = {            \
                    .min = 1,            \
                    .max = 20,            \
                    .increment = 1            \
                },                    \
                .iv_size = { 0 }            \
            }, }                        \
        }, }                            \
    },                                \
    {    /* SHA256 HMAC */                    \
        .op = RTE_CRYPTO_OP_TYPE_SYMMETRIC,            \
        {.sym = {                        \
            .xform_type = RTE_CRYPTO_SYM_XFORM_AUTH,    \
            {.auth = {                    \
                .algo = RTE_CRYPTO_AUTH_SHA256_HMAC,    \
                .block_size = 64,            \
                .key_size = {                \
                    .min = 1,            \
                    .max = 64,            \
                    .increment = 1            \
                },                    \
                .digest_size = {            \
                    .min = 1,            \
                    .max = 32,            \
                    .increment = 1            \
                },                    \
                .iv_size = { 0 }            \
            }, }                        \
        }, }                            \
    }

#endif /* _RSP_SYM_CAPABILITIES_H_ */
