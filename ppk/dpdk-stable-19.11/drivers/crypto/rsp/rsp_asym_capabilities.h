#ifndef _RSP_ASYM_CAPABILITIES_H_
#define _RSP_ASYM_CAPABILITIES_H_

#define RSP_BASE_ASYM_CAPABILITIES                        \
    {    /* RSA */                            \
        .op = RTE_CRYPTO_OP_TYPE_ASYMMETRIC,                \
        {.asym = {                            \
            .xform_capa = {                        \
                .xform_type = RTE_CRYPTO_ASYM_XFORM_RSA,    \
                .op_types = ((1 << RTE_CRYPTO_ASYM_OP_SIGN) |    \
                    (1 << RTE_CRYPTO_ASYM_OP_VERIFY) |    \
                    (1 << RTE_CRYPTO_ASYM_OP_ENCRYPT) |    \
                    (1 << RTE_CRYPTO_ASYM_OP_DECRYPT)),    \
                {                        \
                .modlen = {                    \
                /* min length is based on openssl rsa keygen */    \
                .min = 1024,                    \
                /* value 0 symbolizes no limit on max length */    \
                .max = 2048,                    \
                .increment = 1024                    \
                }, }                        \
            }                            \
        },                                \
        }                                \
    }

#endif /* _RSP_ASYM_CAPABILITIES_H_ */
