/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_HW_H_
#define _NGBE_HW_H_

#include "ngbe_type.h"
#include "ngbe_hw_em.h"
#include "ngbe_hw_sp.h"

struct ngbe_pba {
	u16 word[2];
	u16 *pba_block;
};

void ngbe_dcb_get_rtrup2tc(struct ngbe_hw *hw, u8 *map);

u16 ngbe_get_pcie_msix_count(struct ngbe_hw *hw);
s32 ngbe_init_hw(struct ngbe_hw *hw);
s32 ngbe_start_hw(struct ngbe_hw *hw);
s32 ngbe_stop_hw(struct ngbe_hw *hw);
s32 ngbe_clear_hw_cntrs(struct ngbe_hw *hw);
s32 ngbe_read_pba_num(struct ngbe_hw *hw, u32 *pba_num);
s32 ngbe_read_pba_string(struct ngbe_hw *hw, u8 *pba_num,
				  u32 pba_num_size);
s32 ngbe_read_pba_raw(struct ngbe_hw *hw, u16 *eeprom_buf,
		       u32 eeprom_buf_size, u16 max_pba_block_size,
		       struct ngbe_pba *pba);
s32 ngbe_write_pba_raw(struct ngbe_hw *hw, u16 *eeprom_buf,
			u32 eeprom_buf_size, struct ngbe_pba *pba);
s32 ngbe_get_pba_block_size(struct ngbe_hw *hw, u16 *eeprom_buf,
			     u32 eeprom_buf_size, u16 *pba_block_size);
s32 ngbe_get_mac_addr(struct ngbe_hw *hw, u8 *mac_addr);

void ngbe_set_lan_id_multi_port(struct ngbe_hw *hw);

s32 ngbe_led_on(struct ngbe_hw *hw, u32 index);
s32 ngbe_led_off(struct ngbe_hw *hw, u32 index);

s32 ngbe_set_rar(struct ngbe_hw *hw, u32 index, u8 *addr, u32 vmdq,
			  u32 enable_addr);
s32 ngbe_clear_rar(struct ngbe_hw *hw, u32 index);
s32 ngbe_init_rx_addrs(struct ngbe_hw *hw);
s32 ngbe_update_mc_addr_list(struct ngbe_hw *hw, u8 *mc_addr_list,
				      u32 mc_addr_count,
				      ngbe_mc_addr_itr func, bool clear);
s32 ngbe_update_uc_addr_list(struct ngbe_hw *hw, u8 *addr_list,
				      u32 addr_count, ngbe_mc_addr_itr func);
s32 ngbe_enable_mc(struct ngbe_hw *hw);
s32 ngbe_disable_mc(struct ngbe_hw *hw);
s32 ngbe_disable_sec_rx_path(struct ngbe_hw *hw);
s32 ngbe_enable_sec_rx_path(struct ngbe_hw *hw);
s32 ngbe_disable_sec_tx_path(struct ngbe_hw *hw);
s32 ngbe_enable_sec_tx_path(struct ngbe_hw *hw);

s32 ngbe_fc_enable(struct ngbe_hw *hw);
bool ngbe_device_supports_autoneg_fc(struct ngbe_hw *hw);
void ngbe_fc_autoneg(struct ngbe_hw *hw);
s32 ngbe_setup_fc(struct ngbe_hw *hw);

s32 ngbe_validate_mac_addr(u8 *mac_addr);
s32 ngbe_acquire_swfw_sync(struct ngbe_hw *hw, u32 mask);
void ngbe_release_swfw_sync(struct ngbe_hw *hw, u32 mask);
s32 ngbe_set_pcie_master(struct ngbe_hw *hw, bool enable);

s32 ngbe_get_san_mac_addr(struct ngbe_hw *hw, u8 *san_mac_addr);
s32 ngbe_set_san_mac_addr(struct ngbe_hw *hw, u8 *san_mac_addr);

s32 ngbe_set_vmdq(struct ngbe_hw *hw, u32 rar, u32 vmdq);
s32 ngbe_set_vmdq_san_mac(struct ngbe_hw *hw, u32 vmdq);
s32 ngbe_clear_vmdq(struct ngbe_hw *hw, u32 rar, u32 vmdq);
s32 ngbe_insert_mac_addr(struct ngbe_hw *hw, u8 *addr, u32 vmdq);
s32 ngbe_init_uta_tables(struct ngbe_hw *hw);
s32 ngbe_set_vfta(struct ngbe_hw *hw, u32 vlan,
			 u32 vind, bool vlan_on, bool vlvf_bypass);
s32 ngbe_set_vlvf(struct ngbe_hw *hw, u32 vlan, u32 vind,
			   bool vlan_on, u32 *vfta_delta, u32 vfta,
			   bool vlvf_bypass);
s32 ngbe_clear_vfta(struct ngbe_hw *hw);
s32 ngbe_find_vlvf_slot(struct ngbe_hw *hw, u32 vlan, bool vlvf_bypass);

s32 ngbe_check_mac_link(struct ngbe_hw *hw,
			       u32 *speed,
			       bool *link_up, bool link_up_wait_to_complete);
s32 ngbe_check_mac_link_rl(struct ngbe_hw *hw, u32 *speed,
				 bool *link_up, bool link_up_wait_to_complete);

s32 ngbe_get_wwn_prefix(struct ngbe_hw *hw, u16 *wwnn_prefix,
				 u16 *wwpn_prefix);

s32 ngbe_get_fcoe_boot_status(struct ngbe_hw *hw, u16 *bs);
void ngbe_set_mac_anti_spoofing(struct ngbe_hw *hw, bool enable, int vf);
void ngbe_set_vlan_anti_spoofing(struct ngbe_hw *hw, bool enable, int vf);
s32 ngbe_get_device_caps(struct ngbe_hw *hw, u16 *device_caps);
void ngbe_set_pba(struct ngbe_hw *hw, int num_pb, u32 headroom,
			     int strategy);
void ngbe_enable_relaxed_ordering_gen2(struct ngbe_hw *hw);
s32 ngbe_shutdown_fw_phy(struct ngbe_hw *);
void ngbe_clear_tx_pending(struct ngbe_hw *hw);

extern s32 ngbe_reset_pipeline_raptor(struct ngbe_hw *hw);
extern void ngbe_stop_mac_link_on_d3_raptor(struct ngbe_hw *hw);

s32 ngbe_get_thermal_sensor_data(struct ngbe_hw *hw);
s32 ngbe_init_thermal_sensor_thresh(struct ngbe_hw *hw);
s32 ngbe_mac_check_overtemp(struct ngbe_hw * hw);
void ngbe_get_etk_id(struct ngbe_hw *hw, struct ngbe_nvm_version *nvm_ver);
void ngbe_get_oem_prod_version(struct ngbe_hw *hw,
				struct ngbe_nvm_version *nvm_ver);
void ngbe_get_orom_version(struct ngbe_hw *hw,
			    struct ngbe_nvm_version *nvm_ver);
void ngbe_disable_rx(struct ngbe_hw *hw);
void ngbe_enable_rx(struct ngbe_hw *hw);
s32 ngbe_setup_mac_link_multispeed_fiber(struct ngbe_hw *hw,
					  u32 speed,
					  bool autoneg_wait_to_complete);
void ngbe_set_soft_rate_select_speed(struct ngbe_hw *hw,
				      u32 speed);
void ngbe_add_uc_addr(struct ngbe_hw *hw, u8 *addr_list, u32 vmdq);
void ngbe_set_mta(struct ngbe_hw *hw, u8 *mc_addr);
s32 ngbe_negotiate_fc(struct ngbe_hw *hw, u32 adv_reg, u32 lp_reg,
			u32 adv_sym, u32 adv_asm, u32 lp_sym, u32 lp_asm);
s32 ngbe_init_shared_code(struct ngbe_hw *hw);
s32 ngbe_set_mac_type(struct ngbe_hw *hw);
s32 ngbe_init_ops_pf(struct ngbe_hw *hw);
s32 ngbe_get_link_capabilities_raptor(struct ngbe_hw *hw,
				      u32 *speed, bool *autoneg);
u32 ngbe_get_media_type(struct ngbe_hw *hw);
void ngbe_disable_tx_laser_multispeed_fiber(struct ngbe_hw *hw);
void ngbe_enable_tx_laser_multispeed_fiber(struct ngbe_hw *hw);
void ngbe_flap_tx_laser_multispeed_fiber(struct ngbe_hw *hw);
void ngbe_set_hard_rate_select_speed(struct ngbe_hw *hw,
					u32 speed);
s32 ngbe_setup_mac_link_smartspeed(struct ngbe_hw *hw,
				    u32 speed,
				    bool autoneg_wait_to_complete);
s32 ngbe_start_mac_link_raptor(struct ngbe_hw *hw,
			       bool autoneg_wait_to_complete);
s32 ngbe_setup_mac_link(struct ngbe_hw *hw, u32 speed,
			       bool autoneg_wait_to_complete);
s32 ngbe_setup_sfp_modules(struct ngbe_hw *hw);
s32 ngbe_init_mac_ops(struct ngbe_hw *hw);
s32 ngbe_reset_hw(struct ngbe_hw *hw);
s32 ngbe_start_hw(struct ngbe_hw *hw);
s32 ngbe_init_phy(struct ngbe_hw *hw);
u64 ngbe_get_supported_physical_layer_raptor(struct ngbe_hw *hw);
s32 ngbe_enable_rx_dma_raptor(struct ngbe_hw *hw, u32 regval);
s32 ngbe_prot_autoc_read_raptor(struct ngbe_hw *hw, bool *locked, u64 *value);
s32 ngbe_prot_autoc_write_raptor(struct ngbe_hw *hw, bool locked, u64 value);
void ngbe_atr_compute_perfect_hash_raptor(struct ngbe_atr_input *input,
					  struct ngbe_atr_input *mask);
u32 ngbe_atr_compute_sig_hash_raptor(union ngbe_atr_hash_dword input,
				     union ngbe_atr_hash_dword common);
bool ngbe_verify_lesm_fw_enabled_raptor(struct ngbe_hw *hw);
void ngbe_set_fdir_drop_queue_raptor(struct ngbe_hw *hw, u8 dropqueue);

void ngbe_map_device_id(struct ngbe_hw *hw);

void ngbe_read_efuse(struct ngbe_hw *hw);
u32 ngbe_fmgr_cmd_op(struct ngbe_hw *hw, u32 cmd, u32 cmd_addr);
u32 ngbe_flash_read_dword(struct ngbe_hw *hw, u32 addr);

#endif /* _NGBE_HW_H_ */
