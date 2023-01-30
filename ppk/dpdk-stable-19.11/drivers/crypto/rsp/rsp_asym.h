#ifndef _RSP_ASYM_H_
#define _RSP_ASYM_H_

#include <rte_cryptodev_pmd.h>
#include <rte_crypto_asym.h>
#include "rsp_common.h"
#include "rsp_asym_pmd.h"

#define MAX_PKE_PARAMS                      8
#define RSP_PKE_MAX_LN_SIZE                 512
#define _PKE_ALIGN_ __rte_aligned(8)

#define RSP_MAX_IN_PARAM_NUM                8
#define RSP_MAX_OUT_PARAM_NUM               2
#define RSP_MAX_PARAM_SIZE                  256


// bit 0/1 1024/2048, 2 crt/no-crt, 3 NULL, 4 input swap, 5 output swap
#define RSA_1024_FUNC_ID            0x10000031
#define RSA_1024_CRT_FUNC_ID        0x10000035

#define RSA_2048_FUNC_ID            0x10000030
#define RSA_2048_CRT_FUNC_ID        0x10000034

typedef struct hash_cd_
{
    // 算法 3:0
    #define AUTH_ALGO_NULL          0
    #define AUTH_ALGO_SHA1          1
    #define AUTH_ALGO_MD5           2
    #define AUTH_ALGO_SHA224        3
    #define AUTH_ALGO_SHA256        4
    #define AUTH_ALGO_SHA384        5
    #define AUTH_ALGO_SHA512        6
    #define AUTH_SM3_1              15
    #define AUTH_ALGO_SHA3_256      17
    #define AUTH_ALGO_SHA3_512      19

    uint32_t auth_algo : 4;

    // 模式 7:4
    #define PADDING                 0
    #define NOPADDING               1
    #define AUTH_MODE2              2
    #define AUTH_MODE_DELIMITER     3

    uint32_t auth_mode : 4;

    // AUTH 类型 9:8
    #define AUTH_TYPE_HASH          0
    #define AUTH_TYPE_HMAC          1
    #define AUTH_TYPE_PRF           3

    uint32_t auth_type : 2;
    uint32_t cmp : 5;    
    uint32_t reserved1 : 1;
    uint32_t sha3_pad_en : 1;
    uint32_t sha3_pad_override : 1;
    uint32_t reserved2 : 4;
    uint32_t auth_algo_hi : 2;
    uint32_t reserved3 : 8;
    uint32_t reserved4 : 16;
    uint32_t prog_pad_sha3_lo : 4;
    uint32_t reserved5 : 4;
    uint32_t prog_pad_sha3_hi : 8;
    uint8_t auth_data[128];
} __attribute__ ((packed)) auth_cd_t;
// 4:5 byte
typedef struct serv_specif_flags_
{
    // 标识是否是部分包，如计算hash时需要大量数据，要分多次传递
    #define LA_PARTIAL_NONE   0
    #define LA_PARTIAL_START  1
    #define LA_PARTIAL_MID    2
    #define LA_PARTIAL_END    3    
    uint16_t partial : 2; 

    // Cipher IV存储在msg里边还是只存储IV的物理地址
    #define CIPH_IV_64BIT_PTR   0
    #define CIPH_IV_16BYTE_DATA   1    
    uint16_t iv_field : 1;

    // Cipher/Auth的配置在content descriptor还是sharam
    // 加速卡里边存有一些预设的算法，有相应的offset对应，如MD5、SHA1等，
    // 如单独调用这些预设的算法，则直接置位此位，传入算法的offset即可；
    // 否则通过content descriptor来获取算法的具体信息，   
    uint16_t cfg_offset : 1;

    // 加速卡是否更新STATE
    //（ECB模式的加密设置为disable，其他加密和认证模式在整包和分包结束时设为disable）
    uint16_t upd_state : 1; 

    uint16_t rtn_auth : 1; // 加速卡是否返回认证结果，当单独的auth分包时，设置为0
    uint16_t cmp_auth : 1; // 加速卡是否比较认证结果, 1比较 0不比较

    // 协议类型
    #define LA_SNOW_3G_PROTO     4
    #define LA_GCM_PROTO         2
    #define LA_CCM_PROTO         1
    #define LA_NO_PROTO          0
    uint16_t proto_flags : 3;

    // 表示hash的结果存的位置
    // 为1，忽略Auth Result的设置，直接放在明文的后边；
    // 为0，则把结果存在Auth Result(auth_res_addr)的位置
    #define LA_NO_DIGEST_IN_BUFFER  0
    #define LA_DIGEST_IN_BUFFER     1
    uint16_t digest_in_buf : 1;

    uint16_t gcm_iv_len : 1; // 是否为 96比特 IV    
    uint16_t zuc : 1;        // 是否为 zuc 加密
    uint16_t reserved : 3;   // 保留

} __attribute__ ((packed)) serv_specif_flags_t;
// 6:7 byte
typedef struct comn_reg_flags_
{
    // 使用flat模式还是scater-gather模式
    #define LAC_SYM_DEFAULT_PTR_TYPE_SGL    1 // SGL
    #define LAC_SYM_DP_PTR_TYPE_FLAT        0 // FLAT
    uint16_t ptr : 1;

    // 为0表示把Cd_pars（Byte8-Byte23）
    // 里边按照content descriptor的地址，长度等来解析；
    // 为1表示把Cd_pars（Byte8-Byte23）
    // 按照数据来解析，这个时候数据为key的内容（cipher模式使用instanttable时）
    #define COMN_CD_FLD_TYPE_64BIT_ADR       0
    #define COMN_CD_FLD_TYPE_16BYTE_DATA     1  
    uint16_t cdt : 1;

    uint16_t bnp : 1;
    uint16_t reseved : 13;
    
} __attribute__ ((packed)) comn_reg_flags_t;

// 0:7 byte
typedef struct comm_hdr_
{
    uint8_t reseved;

    // 服务 ID，可以取值如下：
    #define LA_CMD_CIPHER               0
    #define LA_CMD_AUTH                 1
    #define LA_CMD_CIPHER_HASH          2
    #define LA_CMD_HASH_CIPHER          3
    #define LA_CMD_RAND_GET_RANDOM      4
    #define LA_CMD_TRNG_TEST            5
    #define LA_CMD_SSL3_KEY_DERIVE      6
    #define LA_CMD_TLS_V1_1_KEY_DERIVE  7
    #define LA_CMD_TLS_V1_2_KEY_DERIVE  8
    #define LA_CMD_MGF1                 9
    #define LA_CMD_AUTH_PRE_COMP        10
    #define LA_CMD_CIPHER_PRE_COMP      11
    uint8_t service_cmd_id;

    // 服务类型
    #define COMN_REQ_NULL       0
    #define COMN_REQ_CPM_PKE    3
    #define COMN_REQ_CPM_LA     4
    #define COMN_REQ_CPM_DMA    7
    #define COMN_REQ_CPM_COMP   9
    uint8_t service_type;

    // /**< This represents a flags field for the Service Request. 
    // * The most significant bit is the 'valid' flag and the only
    // * one used. All remaining bit positions are unused and
    // * are therefore reserved and need to be set to 0. */
    // Bit 7:   0:InValid; 1:Valid
    // Bit 6-0：Reserved"
    uint8_t hdr_flags;

    // 4:5 byte
    serv_specif_flags_t serv_specif_flags;
    // 6:7 byte
    comn_reg_flags_t comn_reg_flags;
} __attribute__ ((packed)) comm_hdr_t;

// 8:23 byte
typedef struct cd_pars_
{
    uint32_t content_desc_addr_lo;
    uint32_t content_desc_addr_hi;
    uint16_t reserved1;
    uint8_t content_desc_params_sz; // content descriptor的长度
    uint8_t reserved2;
    uint32_t reserved3;
} __attribute__ ((packed)) cd_pars_t;

// 24:55 byte
typedef struct comm_mid_
{
    uint32_t opaque_data_lo;
    uint32_t opaque_data_hi;
    uint32_t src_data_addr_lo;
    uint32_t src_data_addr_hi;
    uint32_t dst_data_addr_lo;
    uint32_t dst_data_addr_hi;
    uint32_t src_len; // 单位：8byte
    uint32_t dst_len; // 单位：8byte
} __attribute__ ((packed)) comm_mid_t;

// 56:107 byte
typedef struct serv_specif_rqpars_
{
    // 56:79 byte
    uint32_t cipher_offset; // 待加解密数据在输入数据中的偏移（in LWs）
    uint32_t cipher_length; // 待加解密数据的长度
    uint32_t cipher_IV_ptr_lo; // 存储IV的物理地址 当IV长度大于4 LWs
    uint32_t cipher_IV_ptr_hi;
    uint32_t reserved1_lo;
    uint32_t reserved1_hi;

    // 80:107 byte
    uint32_t auth_off; // 计算摘要数据在输入数据中的偏移（in Bytes）
    uint32_t auth_len; // 计算摘要数据长度（in Bytes）
    uint32_t aad_addr_lo;
    uint32_t aad_addr_hi;
    // 存储摘要的物理地址，仅在serv_specif_flags-> Digest In Buf为0时有效
    uint32_t auth_res_addr_lo;
    uint32_t auth_res_addr_hi;   
    uint8_t aad_size; 
    uint8_t reserved2; 
    // Prefix data的长度，当有prefix的时候，分包的情况下也需要使用
    uint8_t hash_state_sz;
    uint8_t auth_res_sz;

} __attribute__ ((packed)) serv_specif_rqpars_t;

// 108:127 byte
typedef struct cd_ctrl_
{
    // 108:111 byte
    uint8_t cipher_state_sz; // 加解密分组大小（in LWs）(单次)
    uint8_t cipher_key_sz; // 加解密的key长度 （in LWs）(总长度)

    // cipher block 相对于conten descriptor的偏移
    // 与content物理地址配合使用，找到cipher的配置信息
    uint8_t cipher_cfg_offset;  
    
    uint8_t reserved1;
    
    // 112:127 byte
    uint8_t cipher_padding_sz;

    // 是否有嵌套hash
    // General flags defining the processing to perform. 
    // 0 is normal processing and 1 means there is a 
    // nested hash processing loop to go through
    #define AUTH_HDR_FLAG_DO_NESTED  1
    #define AUTH_HDR_FLAG_NO_NESTED  0    
    uint8_t hash_flags;

    // Hash cfg的偏移（QW）
    // serv_specif_flags->Ciph/Auth为0时，相对于content descriptor的偏移
    // serv_specif_flags->Ciph/Auth为1时，在constantstable中的偏移"
    uint8_t hash_cfg_offset;

    #define SLICE_NULL      0
    #define SLICE_CIPHER    1
    #define SLICE_AUTH      2
    #define SLICE_DRAM_RD   3
    #define SLICE_DRAM_WR   4
    #define SLICE_COMP      5
    #define SLICE_XLAT      6
    #define SLICE_DELIMITER 7
    uint8_t next_curr_id;
    uint8_t reserved3;
    uint8_t outer_prefix_sz; // Outer prefic的长度
    uint8_t final_sz; // 最终返回摘要的结果长度
    uint8_t inner_res_sz; // Inner返回的摘要结果长度
    uint32_t hash_total_len;
    
    uint32_t current_msg_len;
} __attribute__ ((packed)) cd_ctrl_t;

struct hash_req_st 
{

    comm_hdr_t comm_hdr;
    cd_pars_t cd_pars;
    comm_mid_t comm_mid;
    serv_specif_rqpars_t serv_specif_rqpars;
    cd_ctrl_t cd_ctrl;
}__attribute__ ((packed));
// 0:7 byte
typedef struct pke_hdr_
{
    uint8_t reseved;
    uint8_t reseved1;

    // 服务类型
    #define COMN_REQ_NULL       0
    #define COMN_REQ_CPM_PKE    3
    #define COMN_REQ_CPM_LA     4
    #define COMN_REQ_CPM_DMA    7
    #define COMN_REQ_CPM_COMP   9
    uint8_t service_type;

    // /**< This represents a flags field for the Service Request. 
    // * The most significant bit is the 'valid' flag and the only
    // * one used. All remaining bit positions are unused and
    // * are therefore reserved and need to be set to 0. */
    // Bit 7:   0:InValid; 1:Valid
    // Bit 6-0：Reserved"
    uint8_t hdr_flags;

    // 4:5 byte
    comn_reg_flags_t comn_reg_flags;

    // 表示输入的flat array里边哪些个flat是加密之后的，
    // 以每一个字节的对应一个flat块。例如rsa签名过程，
    // 输入序列为C,D’,N；其中的私钥D可以为密文的形式，
    // 所以此变量为 B010 (0-array[0] 1-array[1]) 0X2
    uint8_t kpt_mask;

    // 表示输入的flat array里边哪些个flat是随机数需要加速卡自己填的
    uint8_t kpt_rn_mask;
} __attribute__ ((packed)) pke_hdr_t;

// 8:23 byte
typedef struct pke_cd_pars_
{
    uint32_t content_desc_addr_lo;
    uint32_t content_desc_addr_hi;
    uint32_t reserved1;

    // 使用的函数方法id，最重要的参数，通过此参数，
    // 加速卡来解析inputdata的内容
    // Bit 31:指示当前cmd为rsa还是sm2。
    //        该bit为0表示当前cmd为rsa；
    //        该bit为1表示当前cmd为sm2；
    uint32_t func_id;
} __attribute__ ((packed)) pke_cd_pars_t;

// 24:47 byte
typedef struct pke_mid_
{
    uint32_t opaque_data_lo;
    uint32_t opaque_data_hi;
    uint32_t src_data_addr_lo;
    uint32_t src_data_addr_hi;
    uint32_t dst_data_addr_lo;
    uint32_t dst_data_addr_hi;
} __attribute__ ((packed)) pke_mid_t;

struct pke_req_st
{
    pke_hdr_t pke_hdr;
    pke_cd_pars_t pke_cd_pars;
    pke_mid_t pke_mid;
    
    // 输出的参数个数，每个参数都是一个flat
    uint8_t output_param_count;

    // 输入的参数个数，每个参数都是一个flat 
    uint8_t input_param_count;

    uint16_t reservd1;
    uint32_t reservd2;

    // 下一个PKE包的物理地址，可以形成一个chain，
    // 发送一次请求即可处理多个包。
    uint32_t next_req_addr_lo;
    uint32_t next_req_addr_hi;
} __attribute__ ((packed));

struct pke_resp_st {
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
typedef struct param_s
{
    uint64_t len;
    uint64_t phy;
} __attribute__ ((packed)) param_t;
struct rsp_asym_item {
    uint64_t in_param[RSP_MAX_IN_PARAM_NUM];
    param_t in_param2[RSP_MAX_IN_PARAM_NUM];
    uint8_t in_data[RSP_MAX_IN_PARAM_NUM][RSP_MAX_PARAM_SIZE];

    uint64_t out_param[RSP_MAX_OUT_PARAM_NUM];
    param_t out_param2[RSP_MAX_OUT_PARAM_NUM];
    uint8_t out_data[RSP_MAX_OUT_PARAM_NUM][RSP_MAX_PARAM_SIZE];
} _PKE_ALIGN_;

struct rsp_asym_cookies {
        struct rsp_asym_item   *virt;
        phys_addr_t   phy;
        uint32_t      alg_size;
        struct rte_memzone *mz;
};
int rsp_asym_build_request(void *in_op, uint8_t *out_msg, void *cookies, void *param);

void rsp_asym_process_response(void **op, uint8_t *resp, struct rsp_asym_cookies *cookie);

#endif /* _RSP_ASYM_H_ */
