/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2017 Intel Corporation
 */


#include "base/ngbe.h"
#include "ngbe_ethdev.h"
#include "rte_pmd_ngbe.h"

int
rte_pmd_ngbe_set_vf_mac_addr(uint16_t port, uint16_t vf,
			      struct ether_addr *mac_addr)
{
	struct ngbe_hw *hw;
	struct ngbe_vf_info *vfinfo;
	int rar_entry;
	uint8_t *new_mac = (uint8_t *)(mac_addr);
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	vfinfo = *(NGBE_DEV_VFDATA(dev));
	rar_entry = hw->mac.num_rar_entries - (vf + 1);

	if (is_valid_assigned_ether_addr((struct ether_addr *)new_mac)) {
		rte_memcpy(vfinfo[vf].vf_mac_addresses, new_mac,
			   ETHER_ADDR_LEN);
		return hw->mac.set_rar(hw, rar_entry, new_mac, vf,
					   true);
	}
	return -EINVAL;
}

int
rte_pmd_ngbe_ping_vf(uint16_t port, uint16_t vf)
{
	struct ngbe_hw *hw;
	struct ngbe_vf_info *vfinfo;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	vfinfo = *(NGBE_DEV_VFDATA(dev));

	ctrl = NGBE_PF_CONTROL_MSG;
	if (vfinfo[vf].clear_to_send)
		ctrl |= NGBE_VT_MSGTYPE_CTS;

	ngbe_write_mbx(hw, &ctrl, 1, vf);

	return 0;
}

int
rte_pmd_ngbe_set_vf_vlan_anti_spoof(uint16_t port, uint16_t vf, uint8_t on)
{
	struct ngbe_hw *hw;
	struct ngbe_mac_info *mac;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	mac = &hw->mac;

	mac->set_vlan_anti_spoofing(hw, on, vf);

	return 0;
}

int
rte_pmd_ngbe_set_vf_mac_anti_spoof(uint16_t port, uint16_t vf, uint8_t on)
{
	struct ngbe_hw *hw;
	struct ngbe_mac_info *mac;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	mac = &hw->mac;
	mac->set_mac_anti_spoofing(hw, on, vf);

	return 0;
}

int
rte_pmd_ngbe_set_vf_vlan_insert(uint16_t port, uint16_t vf, uint16_t vlan_id)
{
	struct ngbe_hw *hw;
	uint32_t ctrl;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (vlan_id > ETHER_MAX_VLAN_ID)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	ctrl = rd32(hw, NGBE_POOLTAG(vf));
	if (vlan_id) {
		ctrl = NGBE_POOLTAG(vf);
		ctrl &= ~NGBE_POOLTAG_ACT_MASK;
		ctrl |= NGBE_POOLTAG_ACT_ALWAYS;
	
		ctrl &= ~NGBE_POOLTAG_VTAG_MASK;
		ctrl |= NGBE_POOLTAG_VTAG(vlan_id);
	} else {
		ctrl = 0;
	}

	wr32(hw, NGBE_POOLTAG(vf), ctrl);

	return 0;
}

int
rte_pmd_ngbe_set_tx_loopback(uint16_t port, uint8_t on)
{
	struct ngbe_hw *hw;
	uint32_t ctrl;
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	ctrl = rd32(hw, NGBE_PSRCTL);
	/* enable or disable VMDQ loopback */
	if (on)
		ctrl |= NGBE_PSRCTL_LBENA;
	else
		ctrl &= ~NGBE_PSRCTL_LBENA;

	wr32(hw, NGBE_PSRCTL, ctrl);

	return 0;
}

int
rte_pmd_ngbe_set_all_queues_drop_en(uint16_t port, uint8_t on)
{
	struct ngbe_hw *hw;
	int i;
	int num_queues = 8;
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	for (i = 0; i < num_queues; i++) {
		u32 val = 1 << (i % 32);
		wr32m(hw, NGBE_QPRXDROP, val, val);
	}

	return 0;
}

int
rte_pmd_ngbe_set_vf_split_drop_en(uint16_t port, uint16_t vf, uint8_t on)
{
	struct ngbe_hw *hw;
	uint32_t reg_value;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	/* only support VF's 0 to 63 */
	if ((vf >= pci_dev->max_vfs) || (vf > 63))
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	reg_value = rd32(hw, NGBE_RXCFG(vf * 2));
	if (on)
		reg_value |= NGBE_RXCFG_DROP;
	else
		reg_value &= ~NGBE_RXCFG_DROP;

	wr32(hw, NGBE_RXCFG(vf * 2), reg_value);

	reg_value = rd32(hw, NGBE_RXCFG(vf * 2 + 1));
	if (on)
		reg_value |= NGBE_RXCFG_DROP;
	else
		reg_value &= ~NGBE_RXCFG_DROP;

	wr32(hw, NGBE_RXCFG(vf * 2 + 1), reg_value);

	return 0;
}

int
rte_pmd_ngbe_set_vf_vlan_stripq(uint16_t port, uint16_t vf, uint8_t on)
{
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	struct ngbe_hw *hw;
	uint16_t queues_per_pool;
	uint32_t q;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);
	hw = NGBE_DEV_HW(dev);

	if (!is_ngbe_supported(dev))
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
rte_pmd_ngbe_set_vf_rxmode(uint16_t port, uint16_t vf,
			    uint16_t rx_mask, uint8_t on)
{
	int val = 0;
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	struct ngbe_hw *hw;
	uint32_t vmolr;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	vmolr = rd32(hw, NGBE_POOLETHCTL(vf));

	if (ngbe_vt_check(hw) < 0)
		return -ENOTSUP;

	val = ngbe_convert_vm_rx_mask_to_val(rx_mask, val);

	if (on)
		vmolr |= val;
	else
		vmolr &= ~val;

	wr32(hw, NGBE_POOLETHCTL(vf), vmolr);

	return 0;
}

int
rte_pmd_ngbe_set_vf_rx(uint16_t port, uint16_t vf, uint8_t on)
{
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	uint32_t reg, addr;
	uint32_t val;
	const uint8_t bit1 = 0x1;
	struct ngbe_hw *hw;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);

	if (ngbe_vt_check(hw) < 0)
		return -ENOTSUP;

	/* for vf >= 32, set bit in PFVFRE[1], otherwise PFVFRE[0] */
	if (vf >= 32) {
		addr = NGBE_POOLRXENA(1);
		val = bit1 << (vf - 32);
	} else {
		addr = NGBE_POOLRXENA(0);
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
rte_pmd_ngbe_set_vf_tx(uint16_t port, uint16_t vf, uint8_t on)
{
	struct rte_eth_dev *dev;
	struct rte_pci_device *pci_dev;
	uint32_t reg, addr;
	uint32_t val;
	const uint8_t bit1 = 0x1;

	struct ngbe_hw *hw;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];
	pci_dev = RTE_ETH_DEV_TO_PCI(dev);

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if (vf >= pci_dev->max_vfs)
		return -EINVAL;

	if (on > 1)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	if (ngbe_vt_check(hw) < 0)
		return -ENOTSUP;

	/* for vf >= 32, set bit in PFVFTE[1], otherwise PFVFTE[0] */
	if (vf >= 32) {
		addr = NGBE_POOLTXENA(1);
		val = bit1 << (vf - 32);
	} else {
		addr = NGBE_POOLTXENA(0);
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
rte_pmd_ngbe_set_vf_vlan_filter(uint16_t port, uint16_t vlan,
				 uint64_t vf_mask, uint8_t vlan_on)
{
	struct rte_eth_dev *dev;
	int ret = 0;
	uint16_t vf_idx;
	struct ngbe_hw *hw;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	if ((vlan > ETHER_MAX_VLAN_ID) || (vf_mask == 0))
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);
	if (ngbe_vt_check(hw) < 0)
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
rte_pmd_ngbe_set_vf_rate_limit(uint16_t port, uint16_t vf,
				uint16_t tx_rate, uint64_t q_msk)
{
	struct rte_eth_dev *dev;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	return ngbe_set_vf_rate_limit(dev, vf, tx_rate, q_msk);
}

int
rte_pmd_ngbe_macsec_enable(uint16_t port, uint8_t en, uint8_t rp)
{
	struct ngbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	hw = NGBE_DEV_HW(dev);

	/* Stop the data paths */
	if (hw->mac.disable_sec_rx_path(hw) != 0)
		return -ENOTSUP;
	hw->mac.disable_sec_tx_path(hw);

	/* Enable Ethernet CRC (required by MACsec offload) */
	ctrl = rd32(hw, NGBE_SECRXCTL);
	ctrl |= NGBE_SECRXCTL_CRCSTRIP;
	wr32(hw, NGBE_SECRXCTL, ctrl);

	/* Enable the TX and RX crypto engines */
	ctrl = rd32(hw, NGBE_SECTXCTL);
	ctrl &= ~NGBE_SECTXCTL_XDSA;
	wr32(hw, NGBE_SECTXCTL, ctrl);

	ctrl = rd32(hw, NGBE_SECRXCTL);
	ctrl &= ~NGBE_SECRXCTL_XDSA;
	wr32(hw, NGBE_SECRXCTL, ctrl);

	ctrl = rd32(hw, NGBE_SECTXIFG);
	ctrl &= ~NGBE_SECTXIFG_MIN_MASK;
	ctrl |= NGBE_SECTXIFG_MIN(0x3);
	wr32(hw, NGBE_SECTXIFG, ctrl);

	/* Enable SA lookup */
	ctrl = rd32(hw, NGBE_LSECTXCTL);
	ctrl &= ~NGBE_LSECTXCTL_MODE_MASK;
	ctrl |= en ? NGBE_LSECTXCTL_MODE_AENC : NGBE_LSECTXCTL_MODE_AUTH;
	ctrl &= ~NGBE_LSECTXCTL_PNTRH_MASK;
	ctrl |= NGBE_LSECTXCTL_PNTRH(NGBE_MACSEC_PNTHRSH);
	wr32(hw, NGBE_LSECTXCTL, ctrl);

	ctrl = rd32(hw, NGBE_LSECRXCTL);
	ctrl &= ~NGBE_LSECRXCTL_MODE_MASK;
	ctrl |= NGBE_LSECRXCTL_MODE_STRICT;
	ctrl &= ~NGBE_LSECRXCTL_POSTHDR;
	if (rp)
		ctrl |= NGBE_LSECRXCTL_REPLAY;
	else
		ctrl &= ~NGBE_LSECRXCTL_REPLAY;
	wr32(hw, NGBE_LSECRXCTL, ctrl);

	/* Start the data paths */
	hw->mac.enable_sec_rx_path(hw);
	hw->mac.enable_sec_tx_path(hw);

	return 0;
}

int
rte_pmd_ngbe_macsec_disable(uint16_t port)
{
	struct ngbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	hw = NGBE_DEV_HW(dev);

	/* Stop the data paths */
	if (hw->mac.disable_sec_rx_path(hw) != 0)
		return -ENOTSUP;
	hw->mac.disable_sec_tx_path(hw);

	/* Disable the TX and RX crypto engines */
	ctrl = rd32(hw, NGBE_SECTXCTL);
	ctrl |= NGBE_SECTXCTL_XDSA;
	wr32(hw, NGBE_SECTXCTL, ctrl);

	ctrl = rd32(hw, NGBE_SECRXCTL);
	ctrl |= NGBE_SECRXCTL_XDSA;
	wr32(hw, NGBE_SECRXCTL, ctrl);

	/* Disable SA lookup */
	ctrl = rd32(hw, NGBE_LSECTXCTL);
	ctrl &= ~NGBE_LSECTXCTL_MODE_MASK;
	wr32(hw, NGBE_LSECTXCTL, ctrl);

	ctrl = rd32(hw, NGBE_LSECRXCTL);
	ctrl &= ~NGBE_LSECRXCTL_MODE_MASK;
	wr32(hw, NGBE_LSECRXCTL, ctrl);

	/* Start the data paths */
	hw->mac.enable_sec_rx_path(hw);
	hw->mac.enable_sec_tx_path(hw);

	return 0;
}

int
rte_pmd_ngbe_macsec_config_txsc(uint16_t port, uint8_t *mac)
{
	struct ngbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	hw = NGBE_DEV_HW(dev);

	ctrl = mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
	wr32(hw, NGBE_LSECTXSCIL, ctrl);

	ctrl = mac[4] | (mac[5] << 8);
	wr32(hw, NGBE_LSECTXSCIH, ctrl);

	return 0;
}

int
rte_pmd_ngbe_macsec_config_rxsc(uint16_t port, uint8_t *mac, uint16_t pi)
{
	struct ngbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	hw = NGBE_DEV_HW(dev);

	ctrl = mac[0] | (mac[1] << 8) | (mac[2] << 16) | (mac[3] << 24);
	wr32(hw, NGBE_LSECRXSCIL, ctrl);

	pi = rte_cpu_to_be_16(pi);
	ctrl = mac[4] | (mac[5] << 8) | (pi << 16);
	wr32(hw, NGBE_LSECRXSCIH, ctrl);

	return 0;
}

int
rte_pmd_ngbe_macsec_select_txsa(uint16_t port, uint8_t idx, uint8_t an,
				 uint32_t pn, uint8_t *key)
{
	struct ngbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl, i;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	hw = NGBE_DEV_HW(dev);

	if (idx != 0 && idx != 1)
		return -EINVAL;

	if (an >= 4)
		return -EINVAL;

	hw = NGBE_DEV_HW(dev);

	/* Set the PN and key */
	pn = rte_cpu_to_be_32(pn);
	if (idx == 0) {
		wr32(hw, NGBE_LSECTXPN0, pn);

		for (i = 0; i < 4; i++) {
			ctrl = (key[i * 4 + 0] <<  0) |
			       (key[i * 4 + 1] <<  8) |
			       (key[i * 4 + 2] << 16) |
			       (key[i * 4 + 3] << 24);
			wr32(hw, NGBE_LSECTXKEY0(i), ctrl);
		}
	} else {
		wr32(hw, NGBE_LSECTXPN1, pn);

		for (i = 0; i < 4; i++) {
			ctrl = (key[i * 4 + 0] <<  0) |
			       (key[i * 4 + 1] <<  8) |
			       (key[i * 4 + 2] << 16) |
			       (key[i * 4 + 3] << 24);
			wr32(hw, NGBE_LSECTXKEY1(i), ctrl);
		}
	}

	/* Set AN and select the SA */
	ctrl = (an << idx * 2) | (idx << 4);
	wr32(hw, NGBE_LSECTXSA, ctrl);

	return 0;
}

int
rte_pmd_ngbe_macsec_select_rxsa(uint16_t port, uint8_t idx, uint8_t an,
				 uint32_t pn, uint8_t *key)
{
	struct ngbe_hw *hw;
	struct rte_eth_dev *dev;
	uint32_t ctrl, i;

	RTE_ETH_VALID_PORTID_OR_ERR_RET(port, -ENODEV);

	dev = &rte_eth_devices[port];

	if (!is_ngbe_supported(dev))
		return -ENOTSUP;

	hw = NGBE_DEV_HW(dev);

	if (idx != 0 && idx != 1)
		return -EINVAL;

	if (an >= 4)
		return -EINVAL;

	/* Set the PN */
	pn = rte_cpu_to_be_32(pn);
	wr32(hw, NGBE_LSECRXPN(idx), pn);

	/* Set the key */
	for (i = 0; i < 4; i++) {
		ctrl = (key[i * 4 + 0] <<  0) |
		       (key[i * 4 + 1] <<  8) |
		       (key[i * 4 + 2] << 16) |
		       (key[i * 4 + 3] << 24);
		wr32(hw, NGBE_LSECRXKEY(idx, i), ctrl);
	}

	/* Set the AN and validate the SA */
	ctrl = an | (1 << 2);
	wr32(hw, NGBE_LSECRXSA(idx), ctrl);

	return 0;
}

