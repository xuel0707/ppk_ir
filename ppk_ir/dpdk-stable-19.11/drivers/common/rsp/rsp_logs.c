#include <rte_log.h>
#include <rte_hexdump.h>

#include "rsp_logs.h"

int rsp_gen_logtype;
int rsp_dp_logtype;

int rsp_hexdump_log(uint32_t level, uint32_t logtype, const char *title,
        const void *buf, unsigned int len)
{
    //if (rte_log_can_log(logtype, level))
    if(level > rte_log_get_global_level())
        return 0;
    if(level > (uint32_t)(rte_log_get_level(logtype)))
        return 0;
        
    rte_hexdump(rte_logs.file == NULL ? stderr : rte_logs.file, title, buf, len);
    
    return 0;
}

RTE_INIT(rsp_pci_init_log)
{
    /* Non-data-path logging for pci device and all services */
    rsp_gen_logtype = rte_log_register("pmd.rsp_general");
    if (rsp_gen_logtype >= 0)
        rte_log_set_level(rsp_gen_logtype, RTE_LOG_ERR);

    /* data-path logging for all services */
    rsp_dp_logtype = rte_log_register("pmd.rsp_dp");
    if (rsp_dp_logtype >= 0)
        rte_log_set_level(rsp_dp_logtype, RTE_LOG_ERR);
}
void rsp_gen_log_set(int level)
{
    /* Non-data-path logging for pci device and all services */
    if (rsp_gen_logtype >= 0)
        rte_log_set_level(rsp_gen_logtype, level);
}
void rsp_dp_log_set(int level)
{
    /* data-path logging for all services */
    if (rsp_dp_logtype >= 0)
        rte_log_set_level(rsp_dp_logtype, level);
}
