#ifndef _RSP_SYM_SESSION_H_
#define _RSP_SYM_SESSION_H_

#include <rte_crypto.h>
#include <rte_cryptodev_pmd.h>

#include "rsp_common.h"

#define AES_GCM_AZ_LEN          16
#define RSP_AES_GCM_SPC_IV_SIZE 12

struct rsp_sym_session 
{
    enum rsp_session_type_e    type;
    /*cipher
    */
    enum rsp_dir_e             rsp_dir;
    enum rsp_cipher_algo_e     rsp_cipher_algo;
    enum rsp_cipher_mode_e     rsp_cipher_mode;
    /*auth
    */
    enum rsp_hash_algo_e     rsp_hash_algo;
    enum rsp_auth_type_e     rsp_auth_mode;

    uint8_t aad_len;
    struct rsp_crypto_instance *inst;
    struct {
        struct {
            uint8_t *data;
            uint8_t length;
        } key;
        struct {
            uint32_t offset;
            uint16_t length;
        } iv;
        struct {
            uint8_t  data[32];
            uint16_t length;
        } aad;
        struct {
            uint8_t  *data;
            uint16_t length;
        } tag;
    }cipher;
    struct {
        struct {
            uint8_t *data;    /**< pointer to key data */
            uint16_t length;    /**< key length in bytes */
        } key;
        uint16_t digest_length;
    }auth;

    struct {
        uint8_t data[64];
        uint8_t digest_len;
        uint8_t block_len;
    } hash;

    rte_spinlock_t lock;    /* protects this struct */
};

void rsp_sym_session_clear(struct rte_cryptodev *dev, struct rte_cryptodev_sym_session *sess);
struct rte_crypto_auth_xform *rsp_get_auth_xform(struct rte_crypto_sym_xform *xform);
struct rte_crypto_cipher_xform *rsp_get_cipher_xform(struct rte_crypto_sym_xform *xform);
int rsp_sym_session_configure(struct rte_cryptodev *dev, struct rte_crypto_sym_xform *xform, struct rte_cryptodev_sym_session *sess, struct rte_mempool *mempool);
int rsp_sym_session_set(struct rte_cryptodev *dev, struct rte_crypto_sym_xform *xform, void *sess_priv);
int rsp_sym_configure_auth(struct rte_cryptodev *dev, struct rte_crypto_sym_xform *xform, struct rsp_sym_session *session);
int rsp_sym_configure_chain(struct rte_cryptodev *dev, struct rte_crypto_sym_xform *xform, struct rsp_sym_session *session);
int rsp_sym_configure_cipher(struct rte_cryptodev *dev, struct rte_crypto_sym_xform *xform, struct rsp_sym_session *session);
int rsp_sym_configure_aead(struct rte_cryptodev *dev, struct rte_crypto_sym_xform *xform, struct rsp_sym_session *session);
unsigned int rsp_sym_session_get_private_size(struct rte_cryptodev *dev __rte_unused);
int rsp_hash_get_digest_size( enum rsp_hash_algo_e hash_algo);
int rsp_hash_get_block_size(enum rsp_hash_algo_e hash_algo);
int rsp_sym_get_aes_algo(int key_len);
int rsp_sym_get_sm4_algo(int key_len);
#endif /* _RSP_SYM_SESSION_H_ */
