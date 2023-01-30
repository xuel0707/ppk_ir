#ifndef _RSP_COMMON_H_
#define _RSP_COMMON_H_

#include <stdint.h>
#include <rte_mbuf.h>
#include "rsp_reg.h"
#define RSP_PCI_NAME            rsp
#define RSP_MODEL_S10           1
#define RSP_MODEL_S20           0

#define RSP_CRYPTO_CIPHER_SM4_CBC       0xFF
#define RSP_CRYPTO_CIPHER_SM1_CBC       0xFE

#define RSP_CRYPTO_AEAD_SM4_GCM         0xFF

#define RSP_CRYPTO_AUTH_TRNG            0xFF
#define RSP_CRYPTO_AUTH_SM3             0xFE
#define RSP_CRYPTO_AUTH_SM3_HMAC        0xFD

enum rsp_service_type {
    RSP_SERVICE_ASYMMETRIC = 0,
    RSP_SERVICE_SYMMETRIC,
    RSP_SERVICE_BULK,
    RSP_SERVICE_INVALID
};

struct rsp_common_stats {
    uint64_t enqueued_count;
    uint64_t dequeued_count;

    uint64_t enqueue_err_count;
    uint64_t dequeue_err_count;

    uint64_t threshold_hit_count;
};

struct rsp_pci_device;

void rsp_stats_get(struct rsp_pci_device *rsp_dev, int count, struct rsp_common_stats *stats, enum rsp_service_type type);
void rsp_stats_reset(struct rsp_pci_device *rsp_dev, int count,enum rsp_service_type type);

#define RSP_RTE_CRYPTODEV_HW_RPU        (1ULL << 24)
#define RSP_RTE_CRYPTODEV_HW_BULK        (1ULL << 25)

enum rsp_session_type_e
{
    RSP_CIPHER              =0,
    RSP_AUTH                =1,
    RSP_CIPHER_AUTH         =2,
    RSP_AUTH_CIPHER         =3,
};

enum rsp_cipher_algo_e
{
    /*cipher algo
    */
    RSP_CIPHER_AES_128              =0,
    RSP_CIPHER_AES_256              =1,
    RSP_CIPHER_SM4                  =2,
    RSP_CIPHER_SM1                  =8,
    RSP_CIPHER_AES_192              =9,
    RSP_HMAC                        =0xC,
};
enum rsp_cipher_mode_e
{
    /*cipher mode
    */
    RSP_CIPHER_MODE_ECB             =0,
    RSP_CIPHER_MODE_CBC             =1,
    RSP_CIPHER_MODE_GCM             =2,
    RSP_CIPHER_MODE_CTR             =3,
    RSP_CIPHER_MODE_HMAC_SM3        =7,
    RSP_CIPHER_MODE_HMAC_SHA1       =8,
    RSP_CIPHER_MODE_HMAC_SHA256     =9,
};
#define RSP_128_KEY_SZ          16
#define RSP_192_KEY_SZ          24
#define RSP_256_KEY_SZ          32

enum rsp_dir_e
{
    RSP_ENCRYPT                     =0,
    RSP_DECRYPT                     =1,
};
enum rsp_hash_algo_e
{
    RSP_HASH_ALGO_NULL              =0,
    RSP_HASH_ALGO_SHA1              =1,
    RSP_HASH_ALGO_MD5               =2,
    RSP_HASH_ALGO_SHA224            =3,
    RSP_HASH_ALGO_SHA256            =4,
    RSP_HASH_ALGO_SHA384            =5,
    RSP_HASH_ALGO_SHA512            =6,
    RSP_HASH_ALGO_SM3               =15,
    RSP_TRNG                        =16,
};
enum rsp_auth_type_e
{
    RSP_AUTH_TYPE_HASH             =0,
    RSP_AUTH_TYPE_HMAC             =1,
    RSP_AUTH_TYPE_PRF              =3,
};

int firmware_download(void *bar0_base, uint8_t *data, int len, int model);
int rsp_get_dgst_len(uint32_t algo);
int rsp_get_block_len(uint32_t algo);
int rsp_get_hmac_len(uint32_t algo);
uint8_t *rsp_get_init(int algo);
uint8_t rsp_get_init_len(int algo);
int hash_padding(uint8_t *data, int data_size, int block_size, int total_len);

#define rsp_cpu_to_be32(v) ((((v)>>24)&0x000000FF) | (((v)>>8)&0x0000ff00) | (((v)<<8)&0x00ff0000) | ((v)<<24&0xFF000000))
#define rsp_cpu_to_be16(v) ((((v)>>8)&0x00FF) | (((v)<<8)&0xff00))
#define rsp_cpu_to_le32(v) (v)
#define rsp_cpu_to_le64(v) (v)
#endif /* _RSP_COMMON_H_ */
