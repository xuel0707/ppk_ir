#ifndef _RSP_BULK_H_
#define _RSP_BULK_H_
#include <rte_cryptodev_pmd.h>
//#include <openssl/evp.h>
#include "rsp_common.h"
#include "rsp_session.h"
#include "rsp_bulk_pmd.h"
#include "rsp_asym.h"
#include "rsp_sym.h"
#include "rsp_logs.h"
#define BYTE_LENGTH    8
#define _BULK_ALIGN_ __rte_aligned(8)
struct rsp_bulk_item {
    uint8_t cd_data[256];
    uint8_t out_data[256];
} _BULK_ALIGN_;

struct rsp_bulk_cookies {
        struct rsp_bulk_item   *virt;
        phys_addr_t   phy;
        uint32_t      alg_size;
        struct rte_memzone *mz;
};
int build_bulk_trng_req_msg(struct hash_req_st *req ,
                        uint64_t cd_phy,  void *cd_vir,
                        uint64_t dst_phy, uint32_t dst_len, void *cb,
                        uint8_t algo, uint32_t kek);
int build_auth_req_msg(struct hash_req_st *req , 
                       uint32_t block_len, 
                       uint32_t up_len,
                       uint64_t src_phy, uint32_t src_len, 
                       uint64_t dst_phy, uint32_t dst_len,
                       uint64_t cd_phy,  void *cd_vir,
                       uint8_t algo, void *cb, 
                       uint32_t total_len, int hash_final, int op_type, uint32_t kek );
int rsp_bulk_build_request(void *in_op, uint8_t *out_msg, void *op_cookie, void *param);
void rsp_bulk_process_response(void **op, uint8_t *resp, struct rsp_bulk_cookies *cookie);
#endif /* _RSP_BULK_H_ */
