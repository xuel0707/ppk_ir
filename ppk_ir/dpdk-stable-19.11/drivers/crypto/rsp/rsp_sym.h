#ifndef _RSP_SYM_H_
#define _RSP_SYM_H_
#include <rte_cryptodev_pmd.h>
//#include <openssl/evp.h>
#include "rsp_common.h"
#include "rsp_session.h"
#include "rsp_sym_pmd.h"

#define RSP_MAX_CIPHER_DATA_LENGTH     2400
#define RSP_MAX_AUTH_DATA_LENGTH       2400
#define RSP_MAX_CHAIN_DATA_LENGTH      2400

#define BYTE_LENGTH    8
/* bpi is only used for partial blocks of DES and AES
 * so AES block len can be assumed as max len for iv, src and dst
 */
#define BPI_MAX_ENCR_IV_LEN ICP_RSP_HW_AES_BLK_SZ
struct cipher_req_st {
    uint8_t flags;
    uint8_t request_type;
    uint8_t request_cmd_id;
    uint8_t resrvd1;
    uint8_t split_flag;
    uint8_t kek_index;
    uint8_t kek_algo;
    uint8_t resrvd3;

    /*
     * The first 16 bytes of the key used for symmetric
     */
    uint8_t cipher_key_l[16];

    uint32_t operate_data_l;
    uint32_t operate_data_h;

    uint32_t src_addr_l;
    uint32_t src_addr_h;
    uint32_t dst_addr_l;
    uint32_t dst_addr_h;
    uint32_t src_len;
    uint32_t dst_len;
    union{
        uint8_t  gcm_mask_l[8];
        uint8_t  auth_key_l[8];
    }u1;
    /*
     * The first 16 bytes of the IV used for symmetric or hash
     */
    uint8_t iv_l[16];
    union{
        uint8_t gcm_mask_h[8];
        uint8_t auth_key_h[8];
    }u2;
    /*
     * The second 16 bytes of the key used for symmetric, if has
     */
    uint8_t cipher_key_h[16];
    union{
        uint16_t total_len;
        uint16_t cipher_len;//chain mode cipher len (be)
    }u4;
    uint16_t aad_size;//bit
    uint8_t cipher_flag;
    uint8_t cipher_mode;
    uint8_t cipher_type;
    uint8_t align_offset;

    union{
        uint8_t aad[16];
        /*
         * The second 16 bytes of the IV used for symmetric or hash
         */
        uint8_t iv_h[16];
        uint8_t auth_key_hh[16];
    }u3;
}__attribute__ ((packed));

struct cipher_resp_st {
    uint8_t flags;
    uint8_t response_type;
    uint8_t response_id;
    uint8_t resrvd;

    uint8_t cmd_id;
    uint8_t comn_status;
    uint8_t comn_err_code;
    uint8_t xlat_err_code;

    uint32_t operate_data_l;
    uint32_t operate_data_h;
    uint32_t src_addr_l;
    uint32_t src_addr_h;
    uint32_t dst_addr_l;
    uint32_t dst_addr_h;
}__attribute__ ((packed));
struct rsp_sym_session;

#define _BULK_ALIGN_ __rte_aligned(8)
struct rsp_sym_item {
    uint8_t src[4096];
    uint8_t dst[4096];
} _BULK_ALIGN_;

struct rsp_sym_cookies
{
    struct rsp_sym_item *      virt;
    phys_addr_t phy;
    uint32_t      alg_size;
    struct rte_memzone *mz;
};

int build_chain_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len,  uint32_t cipher_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint8_t mode, uint8_t dir, uint32_t c_align_offset, uint32_t a_align_offset,
                        const uint8_t *cipher_key, uint8_t cipher_key_len,
                        const uint8_t *auth_key, uint8_t auth_key_len,
                        uint8_t *iv,  uint8_t iv_len);
int build_cipher_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint8_t mode, uint8_t dir,
                        const uint8_t *key, uint8_t key_len,
                        uint8_t *iv,  uint8_t iv_len,
                        uint8_t *aad, uint8_t aad_len, uint32_t kek, uint32_t vf_number, uint8_t split_flag, uint16_t total_len, uint32_t offset);
int build_rpu_auth_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        const uint8_t *auth_key, uint8_t auth_key_len,
                        uint8_t algo, uint8_t mode, uint32_t offset);
int build_cipher_req_msg(struct cipher_req_st *req ,
                        uint64_t src_phy, uint32_t src_len, 
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint8_t mode, uint8_t dir,
                        const uint8_t *key, uint8_t key_len,
                        uint8_t *iv,  uint8_t iv_len,
                        uint8_t *aad, uint8_t aad_len, uint32_t kek, uint32_t vf_number, uint8_t split_flag, uint16_t total_len, uint32_t offset);
int rsp_sym_build_request(void *in_op, uint8_t *out_msg, void *op_cookie, void *param);
void rsp_sym_process_response(void **op, uint8_t *resp, struct rsp_sym_cookies *cookie);
#endif /* _RSP_SYM_H_ */
