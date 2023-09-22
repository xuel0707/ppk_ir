/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2016 Intel Corporation
 */

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <inttypes.h>

#include <rte_interrupts.h>
#include <rte_log.h>
#include <rte_debug.h>
#include <rte_eal.h>
#include <rte_ether.h>
#include <rte_memcpy.h>
#include <rte_malloc.h>
#include <rte_random.h>

#include "base/ngbe.h"
#include "ngbe_ethdev.h"
#include "rte_pmd_ngbe.h"

#define NGBE_MAX_VFTA     (128)
#define NGBE_VF_MSG_SIZE_DEFAULT 1
#define NGBE_VF_GET_QUEUE_MSG_SIZE 5

static inline uint16_t
dev_num_vf(struct rte_eth_dev *eth_dev)
{
	struct rte_pci_device *pci_dev = RTE_ETH_DEV_TO_PCI(eth_dev);

	return pci_dev->max_vfs;
}

static inline
int ngbe_vf_perm_addr_gen(struct rte_eth_dev *dev, uint16_t vf_num)
{
	unsigned char vf_mac_addr[ETHER_ADDR_LEN];
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(dev);
	uint16_t vfn;

	for (vfn = 0; vfn < vf_num; vfn++) {
		eth_random_addr(vf_mac_addr);
		/* keep the random address as default */
		memcpy(vfinfo[vfn].vf_mac_addresses, vf_mac_addr,
			   ETHER_ADDR_LEN);
	}

	return 0;
}

static inline int
ngbe_mb_intr_setup(struct rte_eth_dev *dev)
{
	struct ngbe_interrupt *intr = NGBE_DEV_INTR(dev);

	intr->mask_misc |= NGBE_ICRMISC_VFMBX;

	return 0;
}

void ngbe_pf_host_init(struct rte_eth_dev *eth_dev)
{
	struct ngbe_vf_info **vfinfo = NGBE_DEV_VFDATA(eth_dev);
	struct ngbe_mirror_info *mirror_info = NGBE_DEV_MR_INFO(eth_dev);
	struct ngbe_uta_info *uta_info = NGBE_DEV_UTA_INFO(eth_dev);
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	uint16_t vf_num;
	uint8_t nb_queue;

	PMD_INIT_FUNC_TRACE();

	RTE_ETH_DEV_SRIOV(eth_dev).active = 0;
	vf_num = dev_num_vf(eth_dev);
	if (vf_num == 0)
		return;

	*vfinfo = rte_zmalloc("vf_info", sizeof(struct ngbe_vf_info) * vf_num, 0);
	if (*vfinfo == NULL)
		rte_panic("Cannot allocate memory for private VF data\n");

	rte_eth_switch_domain_alloc(&(*vfinfo)->switch_domain_id);

	memset(mirror_info, 0, sizeof(struct ngbe_mirror_info));
	memset(uta_info, 0, sizeof(struct ngbe_uta_info));
	hw->mac.mc_filter_type = 0;

	if (vf_num >= ETH_32_POOLS) {
		nb_queue = 2;
		RTE_ETH_DEV_SRIOV(eth_dev).active = ETH_64_POOLS;
	} else if (vf_num >= ETH_16_POOLS) {
		nb_queue = 4;
		RTE_ETH_DEV_SRIOV(eth_dev).active = ETH_32_POOLS;
	} else {
		nb_queue = 8;
		RTE_ETH_DEV_SRIOV(eth_dev).active = ETH_16_POOLS;
	}

	RTE_ETH_DEV_SRIOV(eth_dev).nb_q_per_pool = nb_queue;
	RTE_ETH_DEV_SRIOV(eth_dev).def_vmdq_idx = vf_num;
	RTE_ETH_DEV_SRIOV(eth_dev).def_pool_q_idx = (uint16_t)(vf_num * nb_queue);

	ngbe_vf_perm_addr_gen(eth_dev, vf_num);

	/* init_mailbox_params */
	hw->mbx.init_params(hw);

	/* set mb interrupt mask */
	ngbe_mb_intr_setup(eth_dev);
}

void ngbe_pf_host_uninit(struct rte_eth_dev *eth_dev)
{
	struct ngbe_vf_info **vfinfo;
	uint16_t vf_num;
	int ret;

	PMD_INIT_FUNC_TRACE();

	RTE_ETH_DEV_SRIOV(eth_dev).active = 0;
	RTE_ETH_DEV_SRIOV(eth_dev).nb_q_per_pool = 0;
	RTE_ETH_DEV_SRIOV(eth_dev).def_vmdq_idx = 0;
	RTE_ETH_DEV_SRIOV(eth_dev).def_pool_q_idx = 0;

	vf_num = dev_num_vf(eth_dev);
	if (vf_num == 0)
		return;

	vfinfo = NGBE_DEV_VFDATA(eth_dev);
	if (*vfinfo == NULL)
		return;

	ret = rte_eth_switch_domain_free((*vfinfo)->switch_domain_id);
	if (ret)
		PMD_INIT_LOG(WARNING, "failed to free switch domain: %d", ret);

	rte_free(*vfinfo);
	*vfinfo = NULL;
}

static void
ngbe_add_tx_flow_control_drop_filter(struct rte_eth_dev *eth_dev)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_filter_info *filter_info = NGBE_DEV_FILTER(eth_dev);
	uint16_t vf_num;
	int i;
	struct ngbe_ethertype_filter ethertype_filter;

	if (!hw->mac.set_ethertype_anti_spoofing) {
		RTE_LOG(INFO, PMD, "ether type anti-spoofing is not"
			" supported.\n");
		return;
	}

	i = ngbe_ethertype_filter_lookup(filter_info,
					  NGBE_ETHERTYPE_FLOW_CTRL);
	if (i >= 0) {
		RTE_LOG(ERR, PMD, "A ether type filter"
			" entity for flow control already exists!\n");
		return;
	}

	ethertype_filter.ethertype = NGBE_ETHERTYPE_FLOW_CTRL;
	ethertype_filter.etqf = NGBE_ETFLT_ENA |
				NGBE_ETFLT_TXAS |
				NGBE_ETHERTYPE_FLOW_CTRL;
	ethertype_filter.etqs = 0;
	ethertype_filter.conf = TRUE;
	i = ngbe_ethertype_filter_insert(filter_info,
					  &ethertype_filter);
	if (i < 0) {
		RTE_LOG(ERR, PMD, "Cannot find an unused ether type filter"
			" entity for flow control.\n");
		return;
	}

	wr32(hw, NGBE_ETFLT(i),
			(NGBE_ETFLT_ENA |
			NGBE_ETFLT_TXAS |
			NGBE_ETHERTYPE_FLOW_CTRL));

	vf_num = dev_num_vf(eth_dev);
	for (i = 0; i < vf_num; i++)
		hw->mac.set_ethertype_anti_spoofing(hw, true, i);
}

int ngbe_pf_host_configure(struct rte_eth_dev *eth_dev)
{
	uint32_t vtctl, fcrth;
	uint32_t vfre_slot, vfre_offset;
	uint16_t vf_num;
	const uint8_t VFRE_SHIFT = 5;  /* VFRE 32 bits per slot */
	const uint8_t VFRE_MASK = (uint8_t)((1U << VFRE_SHIFT) - 1);
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	uint32_t gpie;
	uint32_t gcr_ext;
	uint32_t vlanctrl;
	int i;

	vf_num = dev_num_vf(eth_dev);
	if (vf_num == 0)
		return -1;

	/* enable VMDq and set the default pool for PF */
	vtctl = rd32(hw, NGBE_POOLCTL);
	vtctl &= ~NGBE_POOLCTL_DEFPL_MASK;
	vtctl |= NGBE_POOLCTL_DEFPL(RTE_ETH_DEV_SRIOV(eth_dev).def_vmdq_idx);
	vtctl |= NGBE_POOLCTL_RPLEN;
	wr32(hw, NGBE_POOLCTL, vtctl);

	vfre_offset = vf_num & VFRE_MASK;
	vfre_slot = (vf_num >> VFRE_SHIFT) > 0 ? 1 : 0;

	/* Enable pools reserved to PF only */
	wr32(hw, NGBE_POOLRXENA(vfre_slot), (~0U) << vfre_offset);
	wr32(hw, NGBE_POOLRXENA(vfre_slot ^ 1), vfre_slot - 1);
	wr32(hw, NGBE_POOLTXENA(vfre_slot), (~0U) << vfre_offset);
	wr32(hw, NGBE_POOLTXENA(vfre_slot ^ 1), vfre_slot - 1);

	/* PFDMA Tx General Switch Control Enables VMDQ loopback */
	wr32(hw, NGBE_PSRCTL, NGBE_PSRCTL_LBENA);

	/* clear VMDq map to perment rar 0 */
	hw->mac.clear_vmdq(hw, 0, BIT_MASK32);

	/* clear VMDq map to scan rar 31 */
	wr32(hw, NGBE_ETHADDRIDX, hw->mac.num_rar_entries);
	wr32(hw, NGBE_ETHADDRASS, 0);

	/* set VMDq map to default PF pool */
	hw->mac.set_vmdq(hw, 0, RTE_ETH_DEV_SRIOV(eth_dev).def_vmdq_idx);

	/*
	 * SW msut set GCR_EXT.VT_Mode the same as GPIE.VT_Mode
	 */
	gpie = rd32(hw, NGBE_GPIE);
	gpie |= NGBE_GPIE_MSIX;
	gcr_ext = rd32(hw, NGBE_PORTCTL);
	gcr_ext &= ~NGBE_PORTCTL_NUMVT_MASK;

	if (RTE_ETH_DEV_SRIOV(eth_dev).active == ETH_8_POOLS)
		gcr_ext |= NGBE_PORTCTL_NUMVT_8;

	wr32(hw, NGBE_PORTCTL, gcr_ext);
	wr32(hw, NGBE_GPIE, gpie);

	/*
	 * enable vlan filtering and allow all vlan tags through
	 */
	vlanctrl = rd32(hw, NGBE_VLANCTL);
	vlanctrl |= NGBE_VLANCTL_VFE; /* enable vlan filters */
	wr32(hw, NGBE_VLANCTL, vlanctrl);

	/* VFTA - enable all vlan filters */
	for (i = 0; i < NGBE_MAX_VFTA; i++)
		wr32(hw, NGBE_VLANTBL(i), 0xFFFFFFFF);

	/* Enable MAC Anti-Spoofing */
	hw->mac.set_mac_anti_spoofing(hw, FALSE, vf_num);

	/* set flow control threshold to max to avoid tx switch hang */
	wr32(hw, NGBE_FCWTRLO, 0);
	fcrth = rd32(hw, NGBE_PBRXSIZE) - 32;
	wr32(hw, NGBE_FCWTRHI, fcrth);

	ngbe_add_tx_flow_control_drop_filter(eth_dev);

	return 0;
}

static void
ngbe_set_rx_mode(struct rte_eth_dev *eth_dev)
{
	struct rte_eth_dev_data *dev_data = eth_dev->data;
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	u32 fctrl, vmolr;
	uint16_t vfn = dev_num_vf(eth_dev);

	/* disable store-bad-packets */
	wr32m(hw, NGBE_SECRXCTL, NGBE_SECRXCTL_SAVEBAD, 0);

	/* Check for Promiscuous and All Multicast modes */
	fctrl = rd32m(hw, NGBE_PSRCTL,
			~(NGBE_PSRCTL_UCP | NGBE_PSRCTL_MCP));
	fctrl |= NGBE_PSRCTL_BCA |
		 NGBE_PSRCTL_MCHFENA;

	vmolr = rd32m(hw, NGBE_POOLETHCTL(vfn),
			~(NGBE_POOLETHCTL_UCP |
			  NGBE_POOLETHCTL_MCP |
			  NGBE_POOLETHCTL_UCHA |
			  NGBE_POOLETHCTL_MCHA));
	vmolr |= NGBE_POOLETHCTL_BCA |
		 NGBE_POOLETHCTL_UTA |
		 NGBE_POOLETHCTL_VLA;

	if (dev_data->promiscuous) {
		fctrl |= NGBE_PSRCTL_UCP |
			 NGBE_PSRCTL_MCP;
		/* pf don't want packets routing to vf, so clear UPE */
		vmolr |= NGBE_POOLETHCTL_MCP;
	} else if (dev_data->all_multicast) {
		fctrl |= NGBE_PSRCTL_MCP;
		vmolr |= NGBE_POOLETHCTL_MCP;
	} else {
		vmolr |= NGBE_POOLETHCTL_UCHA;
		vmolr |= NGBE_POOLETHCTL_MCHA;
	}

	wr32(hw, NGBE_POOLETHCTL(vfn), vmolr);

	wr32(hw, NGBE_PSRCTL, fctrl);

	ngbe_vlan_hw_strip_config(eth_dev);
}

static inline void
ngbe_vf_reset_event(struct rte_eth_dev *eth_dev, uint16_t vf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *(NGBE_DEV_VFDATA(eth_dev));
	int rar_entry = hw->mac.num_rar_entries - (vf + 1);
	uint32_t vmolr = rd32(hw, NGBE_POOLETHCTL(vf));

	vmolr |= (NGBE_POOLETHCTL_UCHA |
			NGBE_POOLETHCTL_BCA | NGBE_POOLETHCTL_UTA);
	wr32(hw, NGBE_POOLETHCTL(vf), vmolr);

	wr32(hw, NGBE_POOLTAG(vf), 0);

	/* reset multicast table array for vf */
	vfinfo[vf].num_vf_mc_hashes = 0;

	/* reset rx mode */
	ngbe_set_rx_mode(eth_dev);

	hw->mac.clear_rar(hw, rar_entry);
}

static inline void
ngbe_vf_reset_msg(struct rte_eth_dev *eth_dev, uint16_t vf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	uint32_t reg;
	uint32_t reg_offset, vf_shift;
	const uint8_t VFRE_SHIFT = 5;  /* VFRE 32 bits per slot */
	const uint8_t VFRE_MASK = (uint8_t)((1U << VFRE_SHIFT) - 1);
	uint8_t  nb_q_per_pool;
	int i;

	vf_shift = vf & VFRE_MASK;
	reg_offset = (vf >> VFRE_SHIFT) > 0 ? 1 : 0;

	/* enable transmit for vf */
	reg = rd32(hw, NGBE_POOLTXENA(reg_offset));
	reg |= (1 << vf_shift);
	wr32(hw, NGBE_POOLTXENA(reg_offset), reg);

	/* enable all queue drop for IOV */
	nb_q_per_pool = RTE_ETH_DEV_SRIOV(eth_dev).nb_q_per_pool;
	for (i = vf * nb_q_per_pool; i < (vf + 1) * nb_q_per_pool; i++) {
		ngbe_flush(hw);
		reg = 1 << (i % 32);
		wr32m(hw, NGBE_QPRXDROP, reg, reg);
	}

	/* enable receive for vf */
	reg = rd32(hw, NGBE_POOLRXENA(reg_offset));
	reg |= (reg | (1 << vf_shift));
	wr32(hw, NGBE_POOLRXENA(reg_offset), reg);

	/* Enable counting of spoofed packets in the SSVPC register */
//fixme:reg = rd32(hw, NGBE_VMECM(reg_offset));
//	reg |= (1 << vf_shift);
//	wr32(hw, NGBE_VMECM(reg_offset), reg);

	ngbe_vf_reset_event(eth_dev, vf);
}

static int
ngbe_disable_vf_mc_promisc(struct rte_eth_dev *eth_dev, uint32_t vf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	uint32_t vmolr;

	vmolr = rd32(hw, NGBE_POOLETHCTL(vf));

	RTE_LOG(INFO, PMD, "VF %u: disabling multicast promiscuous\n", vf);

	vmolr &= ~NGBE_POOLETHCTL_MCP;

	wr32(hw, NGBE_POOLETHCTL(vf), vmolr);

	return 0;
}

static int
ngbe_vf_reset(struct rte_eth_dev *eth_dev, uint16_t vf, uint32_t *msgbuf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *(NGBE_DEV_VFDATA(eth_dev));
	unsigned char *vf_mac = vfinfo[vf].vf_mac_addresses;
	int rar_entry = hw->mac.num_rar_entries - (vf + 1);
	uint8_t *new_mac = (uint8_t *)(&msgbuf[1]);

	ngbe_vf_reset_msg(eth_dev, vf);

	hw->mac.set_rar(hw, rar_entry, vf_mac, vf, true);

	/* Disable multicast promiscuous at reset */
	ngbe_disable_vf_mc_promisc(eth_dev, vf);

	/* reply to reset with ack and vf mac address */
	msgbuf[0] = NGBE_VF_RESET | NGBE_VT_MSGTYPE_ACK;
	rte_memcpy(new_mac, vf_mac, ETHER_ADDR_LEN);
	/*
	 * Piggyback the multicast filter type so VF can compute the
	 * correct vectors
	 */
	msgbuf[3] = hw->mac.mc_filter_type;
	ngbe_write_mbx(hw, msgbuf, NGBE_VF_PERMADDR_MSG_LEN, vf);

	return 0;
}

static int
ngbe_vf_set_mac_addr(struct rte_eth_dev *eth_dev, uint32_t vf, uint32_t *msgbuf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *(NGBE_DEV_VFDATA(eth_dev));
	int rar_entry = hw->mac.num_rar_entries - (vf + 1);
	uint8_t *new_mac = (uint8_t *)(&msgbuf[1]);

	if (is_valid_assigned_ether_addr((struct ether_addr *)new_mac)) {
		rte_memcpy(vfinfo[vf].vf_mac_addresses, new_mac, 6);
		return hw->mac.set_rar(hw, rar_entry, new_mac, vf, true);
	}
	return -1;
}

static int
ngbe_vf_set_multicast(struct rte_eth_dev *eth_dev, uint32_t vf, uint32_t *msgbuf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *(NGBE_DEV_VFDATA(eth_dev));
	int nb_entries = (msgbuf[0] & NGBE_VT_MSGINFO_MASK) >>
		NGBE_VT_MSGINFO_SHIFT;
	uint16_t *hash_list = (uint16_t *)&msgbuf[1];
	uint32_t mta_idx;
	uint32_t mta_shift;
	const uint32_t NGBE_MTA_INDEX_MASK = 0x7F;
	const uint32_t NGBE_MTA_BIT_SHIFT = 5;
	const uint32_t NGBE_MTA_BIT_MASK = (0x1 << NGBE_MTA_BIT_SHIFT) - 1;
	uint32_t reg_val;
	int i;
	u32 vmolr = rd32(hw, NGBE_POOLETHCTL(vf));

	/* Disable multicast promiscuous first */
	ngbe_disable_vf_mc_promisc(eth_dev, vf);

	/* only so many hash values supported */
	nb_entries = RTE_MIN(nb_entries, NGBE_MAX_VF_MC_ENTRIES);

	/* store the mc entries  */
	vfinfo->num_vf_mc_hashes = (uint16_t)nb_entries;
	for (i = 0; i < nb_entries; i++) {
		vfinfo->vf_mc_hashes[i] = hash_list[i];
	}

	if (nb_entries == 0) {
		vmolr &= ~NGBE_POOLETHCTL_MCHA;
		wr32(hw, NGBE_POOLETHCTL(vf), vmolr);
		return 0;
	}

	for (i = 0; i < vfinfo->num_vf_mc_hashes; i++) {
		mta_idx = (vfinfo->vf_mc_hashes[i] >> NGBE_MTA_BIT_SHIFT)
				& NGBE_MTA_INDEX_MASK;
		mta_shift = vfinfo->vf_mc_hashes[i] & NGBE_MTA_BIT_MASK;
		reg_val = rd32(hw, NGBE_MCADDRTBL(mta_idx));
		reg_val |= (1 << mta_shift);
		wr32(hw, NGBE_MCADDRTBL(mta_idx), reg_val);
	}

	vmolr |= NGBE_POOLETHCTL_MCHA;
	wr32(hw, NGBE_POOLETHCTL(vf), vmolr);

	return 0;
}

static int
ngbe_vf_set_vlan(struct rte_eth_dev *eth_dev, uint32_t vf, uint32_t *msgbuf)
{
	int add, vid;
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *(NGBE_DEV_VFDATA(eth_dev));

	add = (msgbuf[0] & NGBE_VT_MSGINFO_MASK)
		>> NGBE_VT_MSGINFO_SHIFT;
	vid = NGBE_PSRVLAN_VID(msgbuf[1]);

	if (add)
		vfinfo[vf].vlan_count++;
	else if (vfinfo[vf].vlan_count)
		vfinfo[vf].vlan_count--;
	return hw->mac.set_vfta(hw, vid, vf, (bool)add, false);
}

static int
ngbe_set_vf_lpe(struct rte_eth_dev *eth_dev, __rte_unused uint32_t vf, uint32_t *msgbuf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	uint32_t new_mtu = msgbuf[1];
	uint32_t max_frs;
	int max_frame = new_mtu + ETHER_HDR_LEN + ETHER_CRC_LEN;

	/* X540 and X550 support jumbo frames in IOV mode */
	if (1)
		return -1;

	if ((max_frame < ETHER_MIN_LEN) || (max_frame > ETHER_MAX_JUMBO_FRAME_LEN))
		return -1;

	max_frs = rd32m(hw, NGBE_FRMSZ, NGBE_FRMSZ_MAX_MASK);
	if (max_frs < new_mtu) {
		max_frs = new_mtu;
		wr32m(hw, NGBE_FRMSZ, NGBE_FRMSZ_MAX_MASK,
			NGBE_FRMSZ_MAX(max_frs));
	}

	return 0;
}

static int
ngbe_negotiate_vf_api(struct rte_eth_dev *eth_dev, uint32_t vf, uint32_t *msgbuf)
{
	uint32_t api_version = msgbuf[1];
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(eth_dev);

	switch (api_version) {
	case ngbe_mbox_api_10:
	case ngbe_mbox_api_11:
	case ngbe_mbox_api_12:
	case ngbe_mbox_api_13:
		vfinfo[vf].api_version = (uint8_t)api_version;
		return 0;
	default:
		break;
	}

	RTE_LOG(ERR, PMD, "Negotiate invalid api version %u from VF %d\n",
		api_version, vf);

	return -1;
}

static int
ngbe_get_vf_queues(struct rte_eth_dev *eth_dev, uint32_t vf, uint32_t *msgbuf)
{
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(eth_dev);
	uint32_t default_q = vf * RTE_ETH_DEV_SRIOV(eth_dev).nb_q_per_pool;
	struct rte_eth_conf *eth_conf;
	struct rte_eth_vmdq_dcb_tx_conf *vmdq_dcb_tx_conf;
	u8 num_tcs;
	struct ngbe_hw *hw;
	u32 vmvir;
	u32 vlana;
	u32 vid;
	u32 user_priority;

	/* Verify if the PF supports the mbox APIs version or not */
	switch (vfinfo[vf].api_version) {
	case ngbe_mbox_api_20:
	case ngbe_mbox_api_11:
	case ngbe_mbox_api_12:
	case ngbe_mbox_api_13:
		break;
	default:
		return -1;
	}

	/* Notify VF of Rx and Tx queue number */
	msgbuf[NGBE_VF_RX_QUEUES] = RTE_ETH_DEV_SRIOV(eth_dev).nb_q_per_pool;
	msgbuf[NGBE_VF_TX_QUEUES] = RTE_ETH_DEV_SRIOV(eth_dev).nb_q_per_pool;

	/* Notify VF of default queue */
	msgbuf[NGBE_VF_DEF_QUEUE] = default_q;

	/* Notify VF of number of DCB traffic classes */
	eth_conf = &eth_dev->data->dev_conf;
	switch (eth_conf->txmode.mq_mode) {
	case ETH_MQ_TX_NONE:
	case ETH_MQ_TX_DCB:
		RTE_LOG(ERR, PMD, "PF must work with virtualization for VF %u"
			", but its tx mode = %d\n", vf,
			eth_conf->txmode.mq_mode);
		return -1;

	case ETH_MQ_TX_VMDQ_DCB:
		vmdq_dcb_tx_conf = &eth_conf->tx_adv_conf.vmdq_dcb_tx_conf;
		switch (vmdq_dcb_tx_conf->nb_queue_pools) {
		case ETH_16_POOLS:
			num_tcs = ETH_8_TCS;
			break;
		case ETH_32_POOLS:
			num_tcs = ETH_4_TCS;
			break;
		default:
			return -1;
		}
		break;

	/* ETH_MQ_TX_VMDQ_ONLY,  DCB not enabled */
	case ETH_MQ_TX_VMDQ_ONLY:
		hw = NGBE_DEV_HW(eth_dev);
		vmvir = rd32(hw, NGBE_POOLTAG(vf));
		vlana = vmvir & NGBE_POOLTAG_ACT_MASK;
		vid = vmvir & NGBE_POOLTAG_VTAG_MASK;
		user_priority =
			TXGBD_POOLTAG_VTAG_UP(vmvir);
		if ((vlana == NGBE_POOLTAG_ACT_ALWAYS) &&
			((vid !=  0) || (user_priority != 0)))
			num_tcs = 1;
		else
			num_tcs = 0;
		break;

	default:
		RTE_LOG(ERR, PMD, "PF work with invalid mode = %d\n",
			eth_conf->txmode.mq_mode);
		return -1;
	}
	msgbuf[NGBE_VF_TRANS_VLAN] = num_tcs;

	return 0;
}

static int
ngbe_set_vf_mc_promisc(struct rte_eth_dev *eth_dev, uint32_t vf, uint32_t *msgbuf)
{
	struct ngbe_vf_info *vfinfo = *(NGBE_DEV_VFDATA(eth_dev));
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	int xcast_mode = msgbuf[1];	/* msgbuf contains the flag to enable */
	u32 vmolr, fctrl, disable, enable;

	switch (vfinfo[vf].api_version) {
	case ngbe_mbox_api_12:
		/* promisc introduced in 1.3 version */
		if (xcast_mode == NGBEVF_XCAST_MODE_PROMISC)
			return -EOPNOTSUPP;
		break;
		/* Fall threw */
	case ngbe_mbox_api_13:
		break;
	default:
		return -1;
	}

	if (vfinfo[vf].xcast_mode == xcast_mode)
		goto out;

	switch (xcast_mode) {
	case NGBEVF_XCAST_MODE_NONE:
		disable = NGBE_POOLETHCTL_BCA | NGBE_POOLETHCTL_MCHA |
			  NGBE_POOLETHCTL_MCP | NGBE_POOLETHCTL_UCP |
			  NGBE_POOLETHCTL_VLP;
		enable = 0;
		break;
	case NGBEVF_XCAST_MODE_MULTI:
		disable = NGBE_POOLETHCTL_MCP | NGBE_POOLETHCTL_UCP |
			  NGBE_POOLETHCTL_VLP;
		enable = NGBE_POOLETHCTL_BCA | NGBE_POOLETHCTL_MCHA;
		break;
	case NGBEVF_XCAST_MODE_ALLMULTI:
		disable = NGBE_POOLETHCTL_UCP | NGBE_POOLETHCTL_VLP;
		enable = NGBE_POOLETHCTL_BCA | NGBE_POOLETHCTL_MCHA |
			 NGBE_POOLETHCTL_MCP;
		break;
	case NGBEVF_XCAST_MODE_PROMISC:
		fctrl = rd32(hw, NGBE_PSRCTL);
		if (!(fctrl & NGBE_PSRCTL_UCP)) {
			/* VF promisc requires PF in promisc */
			RTE_LOG(ERR, PMD,
			       "Enabling VF promisc requires PF in promisc\n");
			return -1;
		}

		disable = 0;
		enable = NGBE_POOLETHCTL_BCA | NGBE_POOLETHCTL_MCHA |
			 NGBE_POOLETHCTL_MCP | NGBE_POOLETHCTL_UCP |
			 NGBE_POOLETHCTL_VLP;
		break;
	default:
		return -1;
	}

	vmolr = rd32(hw, NGBE_POOLETHCTL(vf));
	vmolr &= ~disable;
	vmolr |= enable;
	wr32(hw, NGBE_POOLETHCTL(vf), vmolr);
	vfinfo[vf].xcast_mode = xcast_mode;

out:
	msgbuf[1] = xcast_mode;

	return 0;
}

static int
ngbe_set_vf_macvlan_msg(struct rte_eth_dev *dev, uint32_t vf, uint32_t *msgbuf)
{
	struct ngbe_hw *hw = NGBE_DEV_HW(dev);
	struct ngbe_vf_info *vf_info = *(NGBE_DEV_VFDATA(dev));
	uint8_t *new_mac = (uint8_t *)(&msgbuf[1]);
	int index = (msgbuf[0] & NGBE_VT_MSGINFO_MASK) >>
		    NGBE_VT_MSGINFO_SHIFT;

	if (index) {
		if (!rte_is_valid_assigned_ether_addr(
			(struct rte_ether_addr *)new_mac)) {
			PMD_DRV_LOG(ERR, "set invalid mac vf:%d\n", vf);
			return -1;
		}

		vf_info[vf].mac_count++;

		hw->mac.set_rar(hw, vf_info[vf].mac_count,
				new_mac, vf, true);
	} else {
		if (vf_info[vf].mac_count) {
			hw->mac.clear_rar(hw, vf_info[vf].mac_count);
			vf_info[vf].mac_count = 0;
		}
	}
	return 0;
}

static int
ngbe_rcv_msg_from_vf(struct rte_eth_dev *eth_dev, uint16_t vf)
{
	uint16_t mbx_size = NGBE_P2VMBX_SIZE;
	uint16_t msg_size = NGBE_VF_MSG_SIZE_DEFAULT;
	uint32_t msgbuf[NGBE_P2VMBX_SIZE];
	int32_t retval;
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(eth_dev);
	struct rte_pmd_compat_mb_event_param ret_param;

	retval = ngbe_read_mbx(hw, msgbuf, mbx_size, vf);
	if (retval) {
		PMD_DRV_LOG(ERR, "Error mbx recv msg from VF %d", vf);
		return retval;
	}

	/* do nothing with the message already been processed */
	if (msgbuf[0] & (NGBE_VT_MSGTYPE_ACK | NGBE_VT_MSGTYPE_NACK))
		return retval;

	/* flush the ack before we write any messages back */
	ngbe_flush(hw);

	/**
	 * initialise structure to send to user application
	 * will return response from user in retval field
	 */
	ret_param.retval = RTE_PMD_COMPAT_MB_EVENT_PROCEED;
	ret_param.vfid = vf;
	ret_param.msg_type = msgbuf[0] & 0xFFFF;
	ret_param.msg = (void *)msgbuf;

	/* perform VF reset */
	if (msgbuf[0] == NGBE_VF_RESET) {
		int ret = ngbe_vf_reset(eth_dev, vf, msgbuf);

		vfinfo[vf].clear_to_send = true;

		/* notify application about VF reset */
		rte_eth_dev_callback_process(eth_dev, RTE_ETH_EVENT_VF_MBOX,
					      NULL, &ret_param);
		return ret;
	}

	/**
	 * ask user application if we allowed to perform those functions
	 * if we get ret_param.retval == RTE_PMD_COMPAT_MB_EVENT_PROCEED
	 * then business as usual,
	 * if 0, do nothing and send ACK to VF
	 * if ret_param.retval > 1, do nothing and send NAK to VF
	 */
	rte_eth_dev_callback_process(eth_dev, RTE_ETH_EVENT_VF_MBOX,
				      NULL, &ret_param);

	retval = ret_param.retval;

	/* check & process VF to PF mailbox message */
	switch ((msgbuf[0] & 0xFFFF)) {
	case NGBE_VF_SET_MAC_ADDR:
		if (retval == RTE_PMD_COMPAT_MB_EVENT_PROCEED)
			retval = ngbe_vf_set_mac_addr(eth_dev, vf, msgbuf);
		break;
	case NGBE_VF_SET_MULTICAST:
		if (retval == RTE_PMD_COMPAT_MB_EVENT_PROCEED)
			retval = ngbe_vf_set_multicast(eth_dev, vf, msgbuf);
		break;
	case NGBE_VF_SET_LPE:
		if (retval == RTE_PMD_COMPAT_MB_EVENT_PROCEED)
			retval = ngbe_set_vf_lpe(eth_dev, vf, msgbuf);
		break;
	case NGBE_VF_SET_VLAN:
		if (retval == RTE_PMD_COMPAT_MB_EVENT_PROCEED)
			retval = ngbe_vf_set_vlan(eth_dev, vf, msgbuf);
		break;
	case NGBE_VF_API_NEGOTIATE:
		retval = ngbe_negotiate_vf_api(eth_dev, vf, msgbuf);
		break;
	case NGBE_VF_GET_QUEUES:
		retval = ngbe_get_vf_queues(eth_dev, vf, msgbuf);
		msg_size = NGBE_VF_GET_QUEUE_MSG_SIZE;
		break;
	case NGBE_VF_UPDATE_XCAST_MODE:
		if (retval == RTE_PMD_COMPAT_MB_EVENT_PROCEED)
			retval = ngbe_set_vf_mc_promisc(eth_dev, vf, msgbuf);
		break;
	case NGBE_VF_SET_MACVLAN:
		if (retval == RTE_PMD_NGBE_MB_EVENT_PROCEED)
			retval = ngbe_set_vf_macvlan_msg(eth_dev, vf, msgbuf);
		break;
	default:
		PMD_DRV_LOG(DEBUG, "Unhandled Msg %8.8x", (unsigned)msgbuf[0]);
		retval = NGBE_ERR_MBX;
		break;
	}

	/* response the VF according to the message process result */
	if (retval)
		msgbuf[0] |= NGBE_VT_MSGTYPE_NACK;
	else
		msgbuf[0] |= NGBE_VT_MSGTYPE_ACK;

	msgbuf[0] |= NGBE_VT_MSGTYPE_CTS;

	ngbe_write_mbx(hw, msgbuf, msg_size, vf);

	return retval;
}

static inline void
ngbe_rcv_ack_from_vf(struct rte_eth_dev *eth_dev, uint16_t vf)
{
	uint32_t msg = NGBE_VT_MSGTYPE_NACK;
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);
	struct ngbe_vf_info *vfinfo = *NGBE_DEV_VFDATA(eth_dev);

	if (!vfinfo[vf].clear_to_send)
		ngbe_write_mbx(hw, &msg, 1, vf);
}

void ngbe_pf_mbx_process(struct rte_eth_dev *eth_dev)
{
	uint16_t vf;
	struct ngbe_hw *hw = NGBE_DEV_HW(eth_dev);

	for (vf = 0; vf < dev_num_vf(eth_dev); vf++) {
		/* check & process vf function level reset */
		if (!ngbe_check_for_rst(hw, vf))
			ngbe_vf_reset_event(eth_dev, vf);

		/* check & process vf mailbox messages */
		if (!ngbe_check_for_msg(hw, vf))
			ngbe_rcv_msg_from_vf(eth_dev, vf);

		/* check & process acks from vf */
		if (!ngbe_check_for_ack(hw, vf))
			ngbe_rcv_ack_from_vf(eth_dev, vf);
	}
}
