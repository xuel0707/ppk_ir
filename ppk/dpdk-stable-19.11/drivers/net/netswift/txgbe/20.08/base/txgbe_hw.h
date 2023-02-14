/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2015-2020
 */

#ifndef _TXGBE_HW_H_
#define _TXGBE_HW_H_

#include "txgbe_type.h"

struct txgbe_pba {
	u16 word[2];
	u16 *pba_block;
};

void txgbe_dcb_get_rtrup2tc(struct txgbe_hw *hw, u8 *map);

u16 txgbe_get_pcie_msix_count(struct txgbe_hw *hw);
s32 txgbe_init_hw(struct txgbe_hw *hw);
s32 txgbe_start_hw(struct txgbe_hw *hw);
s32 txgbe_stop_hw(struct txgbe_hw *hw);
s32 txgbe_start_hw_gen2(struct txgbe_hw *hw);
s32 txgbe_clear_hw_cntrs(struct txgbe_hw *hw);
s32 txgbe_read_pba_num(struct txgbe_hw *hw, u32 *pba_num);
s32 txgbe_read_pba_string(struct txgbe_hw *hw, u8 *pba_num,
				  u32 pba_num_size);
s32 txgbe_read_pba_raw(struct txgbe_hw *hw, u16 *eeprom_buf,
		       u32 eeprom_buf_size, u16 max_pba_block_size,
		       struct txgbe_pba *pba);
s32 txgbe_write_pba_raw(struct txgbe_hw *hw, u16 *eeprom_buf,
			u32 eeprom_buf_size, struct txgbe_pba *pba);
s32 txgbe_get_pba_block_size(struct txgbe_hw *hw, u16 *eeprom_buf,
			     u32 eeprom_buf_size, u16 *pba_block_size);
s32 txgbe_get_mac_addr(struct txgbe_hw *hw, u8 *mac_addr);

void txgbe_set_lan_id_multi_port(struct txgbe_hw *hw);

s32 txgbe_led_on(struct txgbe_hw *hw, u32 index);
s32 txgbe_led_off(struct txgbe_hw *hw, u32 index);

s32 txgbe_set_rar(struct txgbe_hw *hw, u32 index, u8 *addr, u32 vmdq,
			  u32 enable_addr);
s32 txgbe_clear_rar(struct txgbe_hw *hw, u32 index);
s32 txgbe_init_rx_addrs(struct txgbe_hw *hw);
s32 txgbe_update_mc_addr_list(struct txgbe_hw *hw, u8 *mc_addr_list,
				      u32 mc_addr_count,
				      txgbe_mc_addr_itr func, bool clear);
s32 txgbe_update_uc_addr_list(struct txgbe_hw *hw, u8 *addr_list,
				      u32 addr_count, txgbe_mc_addr_itr func);
s32 txgbe_enable_mc(struct txgbe_hw *hw);
s32 txgbe_disable_mc(struct txgbe_hw *hw);
s32 txgbe_disable_sec_rx_path(struct txgbe_hw *hw);
s32 txgbe_enable_sec_rx_path(struct txgbe_hw *hw);
s32 txgbe_disable_sec_tx_path(struct txgbe_hw *hw);
s32 txgbe_enable_sec_tx_path(struct txgbe_hw *hw);

s32 txgbe_fc_enable(struct txgbe_hw *hw);
bool txgbe_device_supports_autoneg_fc(struct txgbe_hw *hw);
void txgbe_fc_autoneg(struct txgbe_hw *hw);
s32 txgbe_setup_fc(struct txgbe_hw *hw);

s32 txgbe_validate_mac_addr(u8 *mac_addr);
s32 txgbe_acquire_swfw_sync(struct txgbe_hw *hw, u32 mask);
void txgbe_release_swfw_sync(struct txgbe_hw *hw, u32 mask);

s32 txgbe_get_san_mac_addr(struct txgbe_hw *hw, u8 *san_mac_addr);
s32 txgbe_set_san_mac_addr(struct txgbe_hw *hw, u8 *san_mac_addr);

s32 txgbe_set_vmdq(struct txgbe_hw *hw, u32 rar, u32 vmdq);
s32 txgbe_set_vmdq_san_mac(struct txgbe_hw *hw, u32 vmdq);
s32 txgbe_clear_vmdq(struct txgbe_hw *hw, u32 rar, u32 vmdq);
s32 txgbe_insert_mac_addr(struct txgbe_hw *hw, u8 *addr, u32 vmdq);
s32 txgbe_init_uta_tables(struct txgbe_hw *hw);
s32 txgbe_set_vfta(struct txgbe_hw *hw, u32 vlan,
			 u32 vind, bool vlan_on, bool vlvf_bypass);
s32 txgbe_set_vlvf(struct txgbe_hw *hw, u32 vlan, u32 vind,
			   bool vlan_on, u32 *vfta_delta, u32 vfta,
			   bool vlvf_bypass);
s32 txgbe_clear_vfta(struct txgbe_hw *hw);
s32 txgbe_find_vlvf_slot(struct txgbe_hw *hw, u32 vlan, bool vlvf_bypass);

s32 txgbe_check_mac_link(struct txgbe_hw *hw,
			       u32 *speed,
			       bool *link_up, bool link_up_wait_to_complete);

s32 txgbe_get_wwn_prefix(struct txgbe_hw *hw, u16 *wwnn_prefix,
				 u16 *wwpn_prefix);

s32 txgbe_get_fcoe_boot_status(struct txgbe_hw *hw, u16 *bs);
void txgbe_set_mac_anti_spoofing(struct txgbe_hw *hw, bool enable, int vf);
void txgbe_set_vlan_anti_spoofing(struct txgbe_hw *hw, bool enable, int vf);
s32 txgbe_get_device_caps(struct txgbe_hw *hw, u16 *device_caps);
void txgbe_set_pba(struct txgbe_hw *hw, int num_pb, u32 headroom,
			     int strategy);
void txgbe_enable_relaxed_ordering_gen2(struct txgbe_hw *hw);
s32 txgbe_shutdown_fw_phy(struct txgbe_hw *);
void txgbe_clear_tx_pending(struct txgbe_hw *hw);

extern s32 txgbe_reset_pipeline_raptor(struct txgbe_hw *hw);
extern void txgbe_stop_mac_link_on_d3_raptor(struct txgbe_hw *hw);

s32 txgbe_get_thermal_sensor_data(struct txgbe_hw *hw);
s32 txgbe_init_thermal_sensor_thresh(struct txgbe_hw *hw);

void txgbe_get_etk_id(struct txgbe_hw *hw, struct txgbe_nvm_version *nvm_ver);
void txgbe_get_oem_prod_version(struct txgbe_hw *hw,
				struct txgbe_nvm_version *nvm_ver);
void txgbe_get_orom_version(struct txgbe_hw *hw,
			    struct txgbe_nvm_version *nvm_ver);
void txgbe_disable_rx(struct txgbe_hw *hw);
void txgbe_enable_rx(struct txgbe_hw *hw);
s32 txgbe_setup_mac_link_multispeed_fiber(struct txgbe_hw *hw,
					  u32 speed,
					  bool autoneg_wait_to_complete);
void txgbe_set_soft_rate_select_speed(struct txgbe_hw *hw,
				      u32 speed);
void txgbe_add_uc_addr(struct txgbe_hw *hw, u8 *addr_list, u32 vmdq);
void txgbe_set_mta(struct txgbe_hw *hw, u8 *mc_addr);
s32 txgbe_negotiate_fc(struct txgbe_hw *hw, u32 adv_reg, u32 lp_reg,
			u32 adv_sym, u32 adv_asm, u32 lp_sym, u32 lp_asm);
s32 txgbe_init_shared_code(struct txgbe_hw *hw);
s32 txgbe_set_mac_type(struct txgbe_hw *hw);
s32 txgbe_init_ops_pf(struct txgbe_hw *hw);
s32 txgbe_get_link_capabilities_raptor(struct txgbe_hw *hw,
				      u32 *speed, bool *autoneg);
u32 txgbe_get_media_type_raptor(struct txgbe_hw *hw);
void txgbe_disable_tx_laser_multispeed_fiber(struct txgbe_hw *hw);
void txgbe_enable_tx_laser_multispeed_fiber(struct txgbe_hw *hw);
void txgbe_flap_tx_laser_multispeed_fiber(struct txgbe_hw *hw);
void txgbe_set_hard_rate_select_speed(struct txgbe_hw *hw,
					u32 speed);
s32 txgbe_setup_mac_link_smartspeed(struct txgbe_hw *hw,
				    u32 speed,
				    bool autoneg_wait_to_complete);
s32 txgbe_start_mac_link_raptor(struct txgbe_hw *hw,
			       bool autoneg_wait_to_complete);
s32 txgbe_setup_mac_link(struct txgbe_hw *hw, u32 speed,
			       bool autoneg_wait_to_complete);
s32 txgbe_setup_sfp_modules(struct txgbe_hw *hw);
void txgbe_init_mac_link_ops(struct txgbe_hw *hw);
s32 txgbe_reset_hw(struct txgbe_hw *hw);
s32 txgbe_start_hw_raptor(struct txgbe_hw *hw);
s32 txgbe_init_phy_raptor(struct txgbe_hw *hw);
u64 txgbe_get_supported_physical_layer_raptor(struct txgbe_hw *hw);
s32 txgbe_enable_rx_dma_raptor(struct txgbe_hw *hw, u32 regval);
s32 txgbe_prot_autoc_read_raptor(struct txgbe_hw *hw, bool *locked, u64 *value);
s32 txgbe_prot_autoc_write_raptor(struct txgbe_hw *hw, bool locked, u64 value);
s32 txgbe_reinit_fdir_tables(struct txgbe_hw *hw);
s32 txgbe_init_fdir_signature_raptor(struct txgbe_hw *hw, u32 fdirctrl);
s32 txgbe_init_fdir_perfect(struct txgbe_hw *hw, u32 fdirctrl,
					bool cloud_mode);
void txgbe_fdir_add_signature_filter_raptor(struct txgbe_hw *hw,
					   union txgbe_atr_hash_dword input,
					   union txgbe_atr_hash_dword common,
					   u8 queue);
s32 txgbe_fdir_set_input_mask_raptor(struct txgbe_hw *hw,
				    struct txgbe_atr_input *input_mask, bool cloud_mode);
s32 txgbe_fdir_write_perfect_filter_raptor(struct txgbe_hw *hw,
					  struct txgbe_atr_input *input,
					  u16 soft_id, u8 queue, bool cloud_mode);
s32 txgbe_fdir_erase_perfect_filter_raptor(struct txgbe_hw *hw,
					  struct txgbe_atr_input *input,
					  u16 soft_id);
s32 txgbe_fdir_add_perfect_filter_raptor(struct txgbe_hw *hw,
					struct txgbe_atr_input *input,
					struct txgbe_atr_input *mask,
					u16 soft_id,
					u8 queue,
					bool cloud_mode);
void txgbe_atr_compute_perfect_hash_raptor(struct txgbe_atr_input *input,
					  struct txgbe_atr_input *mask);
u32 txgbe_atr_compute_sig_hash_raptor(union txgbe_atr_hash_dword input,
				     union txgbe_atr_hash_dword common);
bool txgbe_verify_lesm_fw_enabled_raptor(struct txgbe_hw *hw);
void txgbe_set_fdir_drop_queue_raptor(struct txgbe_hw *hw, u8 dropqueue);
#endif /* _TXGBE_HW_H_ */
