/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015-2020
 */

#include <rte_ethdev_driver.h>

#include "base/txgbe.h"
#include "txgbe_ethdev.h"
#include "rte_pmd_txgbe.h"

int
rte_pmd_txgbe_set_vf_mac_addr(uint16_t port, uint16_t vf,
			      struct rte_ether_addr *mac_addr)
{
	struct txgbe_hw *hw;
	struct txgbe_vf_info *vfinfo;
	int rar_entry;
	uint8_t *new_mac = (uint8_t *)(mac_addr);
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	vfinfo = *(TXGBE_DEV_VFDATA(dev));
	rar_entry = hw->mac.num_rar_entries - (vf + 1);

	if (rte_is_valid_assigned_ether_addr((struct rte_ether_addr *)new_mac)) {
		rte_memcpy(vfinfo[vf].vf_mac_addresses, new_mac,
			   RTE_ETHER_ADDR_LEN);
		return hw->mac.set_rar(hw, rar_entry, new_mac, vf,
					   true);
	}
	return -EINVAL;
}

int
rte_pmd_txgbe_ping_vf(uint16_t port, uint16_t vf)
{
	struct txgbe_hw *hw;
	struct txgbe_vf_info *vfinfo;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	vfinfo = *(TXGBE_DEV_VFDATA(dev));

	ctrl = TXGBE_PF_CONTROL_MSG;
	if (vfinfo[vf].clear_to_send)
		ctrl |= TXGBE_VT_MSGTYPE_CTS;

	txgbe_write_mbx(hw, &ctrl, 1, vf);

	return 0;
}

int
rte_pmd_txgbe_set_vf_vlan_anti_spoof(uint16_t port, uint16_t vf, uint8_t on)
{
	struct txgbe_hw *hw;
	struct txgbe_mac_info *mac;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	mac = &hw->mac;

	mac->set_vlan_anti_spoofing(hw, on, vf);

	return 0;
}

int
rte_pmd_txgbe_set_vf_mac_anti_spoof(uint16_t port, uint16_t vf, uint8_t on)
{
	struct txgbe_hw *hw;
	struct txgbe_mac_info *mac;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	mac = &hw->mac;
	mac->set_mac_anti_spoofing(hw, on, vf);

	return 0;
}

int
rte_pmd_txgbe_set_vf_vlan_insert(uint16_t port, uint16_t vf, uint16_t vlan_id)
{
	struct txgbe_hw *hw;
	uint32_t ctrl;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (vlan_id > RTE_ETHER_MAX_VLAN_ID)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	ctrl = rd32(hw, TXGBE_POOLTAG(vf));
	if (vlan_id) {
		ctrl = TXGBE_POOLTAG(vf);
		ctrl &= ~TXGBE_POOLTAG_ACT_MASK;
		ctrl |= TXGBE_POOLTAG_ACT_ALWAYS;
	
		ctrl &= ~TXGBE_POOLTAG_VTAG_MASK;
		ctrl |= TXGBE_POOLTAG_VTAG(vlan_id);
	} else {
		ctrl = 0;
	}

	wr32(hw, TXGBE_POOLTAG(vf), ctrl);

	return 0;
}

int
rte_pmd_txgbe_set_tx_loopback(uint16_t port, uint8_t on)
{
	struct txgbe_hw *hw;
	uint32_t ctrl;
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	ctrl = rd32(hw, TXGBE_PSRCTL);
	/* enable or disable VMDQ loopback */
	if (on)
		ctrl |= TXGBE_PSRCTL_LBENA;
	else
		ctrl &= ~TXGBE_PSRCTL_LBENA;

	wr32(hw, TXGBE_PSRCTL, ctrl);

	return 0;
}

int
rte_pmd_txgbe_set_all_queues_drop_en(uint16_t port, uint8_t on)
{
	struct txgbe_hw *hw;
	int i;
	int num_queues = 128;
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	for (i = 0; i <= num_queues; i++) {
		u32 val = 1 << (i % 32);
		wr32m(hw, TXGBE_QPRXDROP(i / 32), val, val);
	}

	return 0;
}

int
rte_pmd_txgbe_set_vf_split_drop_en(uint16_t port, uint16_t vf, uint8_t on)
{
	struct txgbe_hw *hw;
	uint32_t reg_value;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	/* only support VF's 0 to 63 */
	if ((vf >= pci_dev->max_vfs) || (vf > 63))
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	reg_value = rd32(hw, TXGBE_RXCFG(vf * 2));
	if (on)
		reg_value |= TXGBE_RXCFG_DROP;
	else
		reg_value &= ~TXGBE_RXCFG_DROP;

	wr32(hw, TXGBE_RXCFG(vf * 2), reg_value);

	reg_value = rd32(hw, TXGBE_RXCFG(vf * 2 + 1));
	if (on)
		reg_value |= TXGBE_RXCFG_DROP;
	else
		reg_value &= ~TXGBE_RXCFG_DROP;

	wr32(hw, TXGBE_RXCFG(vf * 2 + 1), reg_value);

	return 0;
}

int
rte_pmd_txgbe_set_vf_vlan_stripq(uint16_t port, uint16_t vf, uint8_t on)
{
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	struct txgbe_hw *hw;
	uint16_t queues_per_pool;
	uint32_t q;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	hw = TXGBE_DEV_HW(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	RTE_FUNC_PTR_OR_ERR_RET(*dev->dev_ops->vlan_strip_queue_set, -ENOTSUP);

	/* The PF has 128 queue pairs and in SRIOV configuration
	 * those queues will be assigned to VF's, so RXDCTL
	 * registers will be dealing with queues which will be
	 * assigned to VF's.
	 * Let's say we have SRIOV configured with 31 VF's then the
	 * first 124 queues 0-123 will be allocated to VF's and only
	 * the last 4 queues 123-127 will be assigned to the PF.
	 */
	queues_per_pool = (uint16_t)hw->mac.max_rx_queues /
			  ETH_64_POOLS;

	for (q = 0; q < queues_per_pool; q++)
		(*dev->dev_ops->vlan_strip_queue_set)(dev,
				q + vf * queues_per_pool, on);
	return 0;
}

int
rte_pmd_txgbe_set_vf_rxmode(uint16_t port, uint16_t vf,
			    uint16_t rx_mask, uint8_t on)
{
	int val = 0;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	struct txgbe_hw *hw;
	uint32_t vmolr;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	vmolr = rd32(hw, TXGBE_POOLETHCTL(vf));

	if (txgbe_vt_check(hw) < 0)
		return -ENOTSUP;

	val = txgbe_convert_vm_rx_mask_to_val(rx_mask, val);

	if (on)
		vmolr |= val;
	else
		vmolr &= ~val;

	wr32(hw, TXGBE_POOLETHCTL(vf), vmolr);

	return 0;
}

int
rte_pmd_txgbe_set_vf_rx(uint16_t port, uint16_t vf, uint8_t on)
{
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	uint32_t reg, addr;
	uint32_t val;
	const uint8_t bit1 = 0x1;
	struct txgbe_hw *hw;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);

	if (txgbe_vt_check(hw) < 0)
		return -ENOTSUP;

	/* for vf >= 32, set bit in PFVFRE[1], otherwise PFVFRE[0] */
	if (vf >= 32) {
		addr = TXGBE_POOLRXENA(1);
		val = bit1 << (vf - 32);
	} else {
		addr = TXGBE_POOLRXENA(0);
		val = bit1 << vf;
	}

	reg = rd32(hw, addr);

	if (on)
		reg |= val;
	else
		reg &= ~val;

	wr32(hw, addr, reg);

	return 0;
}

int
rte_pmd_txgbe_set_vf_tx(uint16_t port, uint16_t vf, uint8_t on)
{
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	uint32_t reg, addr;
	uint32_t val;
	const uint8_t bit1 = 0x1;

	struct txgbe_hw *hw;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	if (txgbe_vt_check(hw) < 0)
		return -ENOTSUP;

	/* for vf >= 32, set bit in PFVFTE[1], otherwise PFVFTE[0] */
	if (vf >= 32) {
		addr = TXGBE_POOLTXENA(1);
		val = bit1 << (vf - 32);
	} else {
		addr = TXGBE_POOLTXENA(0);
		val = bit1 << vf;
	}

	reg = rd32(hw, addr);

	if (on)
		reg |= val;
	else
		reg &= ~val;

	wr32(hw, addr, reg);

	return 0;
}

int
rte_pmd_txgbe_set_vf_vlan_filter(uint16_t port, uint16_t vlan,
				 uint64_t vf_mask, uint8_t vlan_on)
{
	struct rte_eth_dev *dev;
	int ret = 0;
	uint16_t vf_idx;
	struct txgbe_hw *hw;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if ((vlan > RTE_ETHER_MAX_VLAN_ID) || (vf_mask == 0))
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);
	if (txgbe_vt_check(hw) < 0)
		return -ENOTSUP;

	for (vf_idx = 0; vf_idx < 64; vf_idx++) {
		if (vf_mask & ((uint64_t)(1ULL << vf_idx))) {
			ret = hw->mac.set_vfta(hw, vlan, vf_idx,
						   vlan_on, false);
			if (ret < 0)
				return ret;
		}
	}

	return ret;
}

int
rte_pmd_txgbe_set_vf_rate_limit(uint16_t port, uint16_t vf,
				uint16_t tx_rate, uint64_t q_msk)
{
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	return txgbe_set_vf_rate_limit(dev, vf, tx_rate, q_msk);
}

int
rte_pmd_txgbe_macsec_enable(uint16_t port, uint8_t en, uint8_t rp)
{
	struct rte_eth_dev *dev;
	struct txgbe_macsec_setting macsec_setting;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	macsec_setting.offload_en = 1;
	macsec_setting.encrypt_en = en;
	macsec_setting.replayprotect_en = rp;

	txgbe_dev_macsec_setting_save(dev, &macsec_setting);

	txgbe_dev_macsec_register_enable(dev, &macsec_setting);

	return 0;
}

int
rte_pmd_txgbe_macsec_disable(uint16_t port)
{
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	txgbe_dev_macsec_setting_reset(dev);

	txgbe_dev_macsec_register_disable(dev);

	return 0;
}

int
rte_pmd_txgbe_macsec_config_txsc(uint16_t port, uint8_t *mac)
{
	struct txgbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	hw = TXGBE_DEV_HW(dev);

	ctrl = mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
	wr32(hw, TXGBE_LSECTXSCIL, ctrl);

	ctrl = mac[4] | (mac[5] << 8);
	wr32(hw, TXGBE_LSECTXSCIH, ctrl);

	return 0;
}

int
rte_pmd_txgbe_macsec_config_rxsc(uint16_t port, uint8_t *mac, uint16_t pi)
{
	struct txgbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	hw = TXGBE_DEV_HW(dev);

	ctrl = mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
	wr32(hw, TXGBE_LSECRXSCIL, ctrl);

	pi = rte_cpu_to_be_16(pi);
	ctrl = mac[4] | (mac[5] << 8) | (pi << 16);
	wr32(hw, TXGBE_LSECRXSCIH, ctrl);

	return 0;
}

int
rte_pmd_txgbe_macsec_select_txsa(uint16_t port, uint8_t idx, uint8_t an,
				 uint32_t pn, uint8_t *key)
{
	struct txgbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl, i;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	hw = TXGBE_DEV_HW(dev);

	if (idx != 0 && idx != 1)
		return -EINVAL;

	if (an >= 4)
		return -EINVAL;

	hw = TXGBE_DEV_HW(dev);

	/* Set the PN and key */
	pn = rte_cpu_to_be_32(pn);
	if (idx == 0) {
		wr32(hw, TXGBE_LSECTXPN0, pn);

		for (i = 0; i < 4; i++) {
			ctrl = (key[i * 4 + 0] <<  0) |
			       (key[i * 4 + 1] <<  8) |
			       (key[i * 4 + 2] << 16) |
			       (key[i * 4 + 3] << 24);
			wr32(hw, TXGBE_LSECTXKEY0(i), ctrl);
		}
	} else {
		wr32(hw, TXGBE_LSECTXPN1, pn);

		for (i = 0; i < 4; i++) {
			ctrl = (key[i * 4 + 0] <<  0) |
			       (key[i * 4 + 1] <<  8) |
			       (key[i * 4 + 2] << 16) |
			       (key[i * 4 + 3] << 24);
			wr32(hw, TXGBE_LSECTXKEY1(i), ctrl);
		}
	}

	/* Set AN and select the SA */
	ctrl = (an << idx * 2) | (idx << 4);
	wr32(hw, TXGBE_LSECTXSA, ctrl);

	return 0;
}

int
rte_pmd_txgbe_macsec_select_rxsa(uint16_t port, uint8_t idx, uint8_t an,
				 uint32_t pn, uint8_t *key)
{
	struct txgbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl, i;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	hw = TXGBE_DEV_HW(dev);

	if (idx != 0 && idx != 1)
		return -EINVAL;

	if (an >= 4)
		return -EINVAL;

	/* Set the PN */
	pn = rte_cpu_to_be_32(pn);
	wr32(hw, TXGBE_LSECRXPN(idx), pn);

	/* Set the key */
	for (i = 0; i < 4; i++) {
		ctrl = (key[i * 4 + 0] <<  0) |
		       (key[i * 4 + 1] <<  8) |
		       (key[i * 4 + 2] << 16) |
		       (key[i * 4 + 3] << 24);
		wr32(hw, TXGBE_LSECRXKEY(idx, i), ctrl);
	}

	/* Set the AN and validate the SA */
	ctrl = an | (1 << 2);
	wr32(hw, TXGBE_LSECRXSA(idx), ctrl);

	return 0;
}

int
rte_pmd_txgbe_set_tc_bw_alloc(uint16_t port,
			      uint8_t tc_num,
			      uint8_t *bw_weight)
{
	struct rte_eth_dev *dev;
	struct txgbe_dcb_config *dcb_config;
	struct txgbe_dcb_tc_config *tc;
	struct rte_eth_conf *eth_conf;
	struct txgbe_bw_conf *bw_conf;
	uint8_t i;
	uint8_t nb_tcs;
	uint16_t sum;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_txgbe_supported(dev))
		return -ENOTSUP;

	if (tc_num > TXGBE_DCB_TC_MAX) {
		PMD_DRV_LOG(ERR, "TCs should be no more than %d.",
			    TXGBE_DCB_TC_MAX);
		return -EINVAL;
	}

	dcb_config = TXGBE_DEV_DCB_CONFIG(dev);
	bw_conf = TXGBE_DEV_BW_CONF(dev);
	eth_conf = &dev->data->dev_conf;

	if (eth_conf->txmode.mq_mode == ETH_MQ_TX_DCB) {
		nb_tcs = eth_conf->tx_adv_conf.dcb_tx_conf.nb_tcs;
	} else if (eth_conf->txmode.mq_mode == ETH_MQ_TX_VMDQ_DCB) {
		if (eth_conf->tx_adv_conf.vmdq_dcb_tx_conf.nb_queue_pools ==
		    ETH_32_POOLS)
			nb_tcs = ETH_4_TCS;
		else
			nb_tcs = ETH_8_TCS;
	} else {
		nb_tcs = 1;
	}

	if (nb_tcs != tc_num) {
		PMD_DRV_LOG(ERR,
			    "Weight should be set for all %d enabled TCs.",
			    nb_tcs);
		return -EINVAL;
	}

	sum = 0;
	for (i = 0; i < nb_tcs; i++)
		sum += bw_weight[i];
	if (sum != 100) {
		PMD_DRV_LOG(ERR,
			    "The summary of the TC weight should be 100.");
		return -EINVAL;
	}

	for (i = 0; i < nb_tcs; i++) {
		tc = &dcb_config->tc_config[i];
		tc->path[TXGBE_DCB_TX_CONFIG].bwg_percent = bw_weight[i];
	}
	for (; i < TXGBE_DCB_TC_MAX; i++) {
		tc = &dcb_config->tc_config[i];
		tc->path[TXGBE_DCB_TX_CONFIG].bwg_percent = 0;
	}

	bw_conf->tc_num = nb_tcs;

	return 0;
}

