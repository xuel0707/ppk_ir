#ifndef _RSP_LOGS_H_
#define _RSP_LOGS_H_

extern int rsp_gen_logtype;
extern int rsp_dp_logtype;

#define RSP_LOG(level, fmt, args...)            \
    rte_log(RTE_LOG_ ## level, rsp_gen_logtype, "%s(): " fmt "\n", __func__, ## args)

#define RSP_DP_LOG(level, fmt, args...)         \
    rte_log(RTE_LOG_ ## level, rsp_dp_logtype, "%s(): " fmt "\n", __func__, ## args)

#define RSP_DP_HEXDUMP_LOG(level, title, buf, len)        \
    rsp_hexdump_log(RTE_LOG_ ## level, rsp_dp_logtype, title, buf, len)


int rsp_hexdump_log(uint32_t level, uint32_t logtype, const char *title, const void *buf, unsigned int len);

void rsp_gen_log_set(int level);
void rsp_dp_log_set(int level);
#endif /* _RSP_LOGS_H_ */
