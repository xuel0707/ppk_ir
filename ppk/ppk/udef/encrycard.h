#ifndef __ENCRYCARD_H__
#define __ENCRYCARD_H__


/* �������� */
extern int zhijiang_init_crypto_device(void);
extern unsigned int IPSec(struct rte_mbuf *in_mbuf, unsigned int spi, uint8_t first_head_offset, uint8_t last_head_length, uint8_t last_head_offset,unsigned int coreid, unsigned int *prt1, unsigned int *prt2, SHORT_STDPARAMS);
extern unsigned int unIPSec(struct rte_mbuf *in_mbuf, unsigned int coreid, uint8_t first_head_offset,uint8_t last_head_length, uint8_t last_head_offset, unsigned int *prt1, unsigned int *prt2, SHORT_STDPARAMS);


#endif
