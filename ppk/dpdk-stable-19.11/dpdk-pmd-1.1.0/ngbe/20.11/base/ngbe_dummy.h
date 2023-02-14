/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#ifndef _NGBE_TYPE_DUMMY_H_
#define _NGBE_TYPE_DUMMY_H_

#ifdef TUP
#elif defined(__GNUC__)
#define TUP(x) x##_unused ngbe_unused
#elif defined(__LCLINT__)
#define TUP(x) x /*@unused@*/
#else
#define TUP(x) x
#endif /*TUP*/
#define TUP0 TUP(p0)
#define TUP1 TUP(p1)
#define TUP2 TUP(p2)
#define TUP3 TUP(p3)
#define TUP4 TUP(p4)
#define TUP5 TUP(p5)
#define TUP6 TUP(p6)
#define TUP7 TUP(p7)
#define TUP8 TUP(p8)
#define TUP9 TUP(p9)

/* struct ngbe_bus_operations */
static inline s32 ngbe_bus_get_bus_info_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_bus_set_lan_id_dummy(struct ngbe_hw *TUP0) {
	return;
}
/* struct ngbe_rom_operations */
static inline s32 ngbe_rom_init_params_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_read16_dummy(struct ngbe_hw *TUP0, u32 TUP1, u16 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_readw_buffer_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, void *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_readw_sw_dummy(struct ngbe_hw *TUP0, u32 TUP1, u16 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_read32_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_read_buffer_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, void *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_write16_dummy(struct ngbe_hw *TUP0, u32 TUP1, u16 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_writew_buffer_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, void *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_writew_sw_dummy(struct ngbe_hw *TUP0, u32 TUP1, u16 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_write32_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_write_buffer_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, void *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_validate_checksum_dummy(struct ngbe_hw *TUP0, u16 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_update_checksum_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_rom_calc_checksum_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}

/* struct ngbe_mac_operations */
static inline s32 ngbe_mac_init_params_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_init_hw_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_reset_hw_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_start_hw_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_stop_hw_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_clear_hw_cntrs_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_enable_relaxed_ordering_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline u64 ngbe_mac_get_supported_physical_layer_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_mac_addr_dummy(struct ngbe_hw *TUP0, u8 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_san_mac_addr_dummy(struct ngbe_hw *TUP0, u8 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_san_mac_addr_dummy(struct ngbe_hw *TUP0, u8 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_device_caps_dummy(struct ngbe_hw *TUP0, u16 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_wwn_prefix_dummy(struct ngbe_hw *TUP0, u16 *TUP1, u16 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_fcoe_boot_status_dummy(struct ngbe_hw *TUP0, u16 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_read_analog_reg8_dummy(struct ngbe_hw *TUP0, u32 TUP1, u8 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_write_analog_reg8_dummy(struct ngbe_hw *TUP0, u32 TUP1, u8 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_setup_sfp_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_enable_rx_dma_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_disable_sec_rx_path_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_enable_sec_rx_path_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_disable_sec_tx_path_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_enable_sec_tx_path_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_acquire_swfw_sync_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_release_swfw_sync_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return;
}
static inline void ngbe_mac_init_swfw_sync_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline u64 ngbe_mac_autoc_read_dummy(struct ngbe_hw *TUP0) {
	return 0;
}
static inline void ngbe_mac_autoc_write_dummy(struct ngbe_hw *TUP0, u64 TUP1) {
	return;
}
static inline s32 ngbe_mac_prot_autoc_read_dummy(struct ngbe_hw *TUP0, bool *TUP1, u64 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_prot_autoc_write_dummy(struct ngbe_hw *TUP0, bool TUP1, u64 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_negotiate_api_version_dummy(struct ngbe_hw *TUP0, int TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_disable_tx_laser_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline void ngbe_mac_enable_tx_laser_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline void ngbe_mac_flap_tx_laser_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline s32 ngbe_mac_setup_link_dummy(struct ngbe_hw *TUP0, u32 TUP1, bool TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_setup_mac_link_dummy(struct ngbe_hw *TUP0, u32 TUP1, bool TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_check_link_dummy(struct ngbe_hw *TUP0, u32 *TUP1, bool *TUP3, bool TUP4) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_link_capabilities_dummy(struct ngbe_hw *TUP0, u32 *TUP1, bool *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_set_rate_select_speed_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return;
}
static inline void ngbe_mac_setup_pba_dummy(struct ngbe_hw *TUP0, int TUP1, u32 TUP2, int TUP3) {
	return;
}
static inline s32 ngbe_mac_led_on_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_led_off_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_blink_led_start_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_blink_led_stop_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_init_led_link_act_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_rar_dummy(struct ngbe_hw *TUP0, u32 TUP1, u8 *TUP2, u32 TUP3, u32 TUP4) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_uc_addr_dummy(struct ngbe_hw *TUP0, u32 TUP1, u8 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_clear_rar_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_insert_mac_addr_dummy(struct ngbe_hw *TUP0, u8 *TUP1, u32 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_vmdq_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_vmdq_san_mac_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_clear_vmdq_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_init_rx_addrs_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_update_uc_addr_list_dummy(struct ngbe_hw *TUP0, u8 *TUP1, u32 TUP2, ngbe_mc_addr_itr TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_update_mc_addr_list_dummy(struct ngbe_hw *TUP0, u8 *TUP1, u32 TUP2, ngbe_mc_addr_itr TUP3, bool TUP4) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_enable_mc_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_disable_mc_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_clear_vfta_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_vfta_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, bool TUP3, bool TUP4) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_vlvf_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, bool TUP3, u32 *TUP4, u32 TUP5, bool TUP6) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_init_uta_tables_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_set_mac_anti_spoofing_dummy(struct ngbe_hw *TUP0, bool TUP1, int TUP2) {
	return;
}
static inline void ngbe_mac_set_vlan_anti_spoofing_dummy(struct ngbe_hw *TUP0, bool TUP1, int TUP2) {
	return;
}
static inline s32 ngbe_mac_update_xcast_mode_dummy(struct ngbe_hw *TUP0, int TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_set_rlpml_dummy(struct ngbe_hw *TUP0, u16 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_fc_enable_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_setup_fc_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_fc_autoneg_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline s32 ngbe_mac_set_fw_drv_ver_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 TUP2, u8 TUP3, u8 TUP4, u16 TUP5, const char * TUP6) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_get_thermal_sensor_data_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_init_thermal_sensor_thresh_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_get_rtrup2tc_dummy(struct ngbe_hw *TUP0, u8 *TUP1) {
	return;
}
static inline void ngbe_mac_disable_rx_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline void ngbe_mac_enable_rx_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline void ngbe_mac_set_source_address_pruning_dummy(struct ngbe_hw *TUP0, bool TUP1, unsigned int TUP2) {
	return;
}
static inline void ngbe_mac_set_ethertype_anti_spoofing_dummy(struct ngbe_hw *TUP0, bool TUP1, int TUP2) {
	return;
}
static inline s32 ngbe_mac_dmac_update_tcs_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_dmac_config_tcs_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_dmac_config_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_setup_eee_dummy(struct ngbe_hw *TUP0, bool TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_read_iosf_sb_reg_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u32 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mac_write_iosf_sb_reg_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u32 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_mac_disable_mdd_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline void ngbe_mac_enable_mdd_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline void ngbe_mac_mdd_event_dummy(struct ngbe_hw *TUP0, u32 *TUP1) {
	return;
}
static inline void ngbe_mac_restore_mdd_vf_dummy(struct ngbe_hw *TUP0, u32 TUP1) {
	return;
}
static inline bool ngbe_mac_fw_recovery_mode_dummy(struct ngbe_hw *TUP0) {
	return false;
}

/* struct ngbe_phy_operations */
static inline s32 ngbe_phy_identify_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_identify_sfp_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_init_hw_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_reset_hw_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_read_reg_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u16 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_write_reg_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_read_reg_unlocked_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u16 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_write_reg_unlocked_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}

static inline s32 ngbe_read_phy_reg_internal_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u16 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}

static inline s32 ngbe_write_phy_reg_internal_dummy(struct ngbe_hw *TUP0, u32 TUP1, u32 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}

static inline s32 ngbe_phy_setup_link_dummy(struct ngbe_hw *TUP0, u32 TUP1, bool TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_check_link_dummy(struct ngbe_hw *TUP0, u32 *TUP1, bool *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_get_firmware_version_dummy(struct ngbe_hw *TUP0, u32 *TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_read_i2c_byte_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 TUP2, u8 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_write_i2c_byte_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 TUP2, u8 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_read_i2c_sff8472_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_read_i2c_eeprom_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 *TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_write_i2c_eeprom_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 TUP2) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline void ngbe_phy_i2c_bus_clear_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline s32 ngbe_phy_check_overtemp_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_set_phy_power_dummy(struct ngbe_hw *TUP0, bool TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_enter_lplu_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_handle_lasi_dummy(struct ngbe_hw *TUP0) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_read_i2c_byte_unlocked_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 TUP2, u8 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_phy_write_i2c_byte_unlocked_dummy(struct ngbe_hw *TUP0, u8 TUP1, u8 TUP2, u8 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}

/* struct ngbe_link_operations */
static inline s32 ngbe_link_read_link_dummy(struct ngbe_hw *TUP0, u8 TUP1, u16 TUP2, u16 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_link_read_link_unlocked_dummy(struct ngbe_hw *TUP0, u8 TUP1, u16 TUP2, u16 *TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_link_write_link_dummy(struct ngbe_hw *TUP0, u8 TUP1, u16 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_link_write_link_unlocked_dummy(struct ngbe_hw *TUP0, u8 TUP1, u16 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}

/* struct ngbe_mbx_operations */
static inline void ngbe_mbx_init_params_dummy(struct ngbe_hw *TUP0) {
	return;
}
static inline s32 ngbe_mbx_read_dummy(struct ngbe_hw *TUP0, u32 *TUP1, u16 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mbx_write_dummy(struct ngbe_hw *TUP0, u32 *TUP1, u16 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mbx_read_posted_dummy(struct ngbe_hw *TUP0, u32 *TUP1, u16 TUP2, u16 TUP3) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mbx_write_posted_dummy(struct ngbe_hw *TUP0, u32 *TUP1, u16 TUP2, u16 TUP4) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mbx_check_for_msg_dummy(struct ngbe_hw *TUP0, u16 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mbx_check_for_ack_dummy(struct ngbe_hw *TUP0, u16 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}
static inline s32 ngbe_mbx_check_for_rst_dummy(struct ngbe_hw *TUP0, u16 TUP1) {
	return NGBE_ERR_OPS_DUMMY;
}


static inline void ngbe_init_ops_dummy(struct ngbe_hw *hw)
{
	hw->bus.get_bus_info = ngbe_bus_get_bus_info_dummy;
	hw->bus.set_lan_id = ngbe_bus_set_lan_id_dummy;
	hw->rom.init_params = ngbe_rom_init_params_dummy;
	hw->rom.read16 = ngbe_rom_read16_dummy;
	hw->rom.readw_buffer = ngbe_rom_readw_buffer_dummy;
	hw->rom.readw_sw = ngbe_rom_readw_sw_dummy;
	hw->rom.read32 = ngbe_rom_read32_dummy;
	hw->rom.read_buffer = ngbe_rom_read_buffer_dummy;
	hw->rom.write16 = ngbe_rom_write16_dummy;
	hw->rom.writew_buffer = ngbe_rom_writew_buffer_dummy;
	hw->rom.writew_sw = ngbe_rom_writew_sw_dummy;
	hw->rom.write32 = ngbe_rom_write32_dummy;
	hw->rom.write_buffer = ngbe_rom_write_buffer_dummy;
	hw->rom.validate_checksum = ngbe_rom_validate_checksum_dummy;
	hw->rom.update_checksum = ngbe_rom_update_checksum_dummy;
	hw->rom.calc_checksum = ngbe_rom_calc_checksum_dummy;
	hw->mac.init_params = ngbe_mac_init_params_dummy;
	hw->mac.init_hw = ngbe_mac_init_hw_dummy;
	hw->mac.reset_hw = ngbe_mac_reset_hw_dummy;
	hw->mac.start_hw = ngbe_mac_start_hw_dummy;
	hw->mac.stop_hw = ngbe_mac_stop_hw_dummy;
	hw->mac.clear_hw_cntrs = ngbe_mac_clear_hw_cntrs_dummy;
	hw->mac.enable_relaxed_ordering = ngbe_mac_enable_relaxed_ordering_dummy;
	hw->mac.get_supported_physical_layer = ngbe_mac_get_supported_physical_layer_dummy;
	hw->mac.get_mac_addr = ngbe_mac_get_mac_addr_dummy;
	hw->mac.get_san_mac_addr = ngbe_mac_get_san_mac_addr_dummy;
	hw->mac.set_san_mac_addr = ngbe_mac_set_san_mac_addr_dummy;
	hw->mac.get_device_caps = ngbe_mac_get_device_caps_dummy;
	hw->mac.get_wwn_prefix = ngbe_mac_get_wwn_prefix_dummy;
	hw->mac.get_fcoe_boot_status = ngbe_mac_get_fcoe_boot_status_dummy;
	hw->mac.read_analog_reg8 = ngbe_mac_read_analog_reg8_dummy;
	hw->mac.write_analog_reg8 = ngbe_mac_write_analog_reg8_dummy;
	hw->mac.setup_sfp = ngbe_mac_setup_sfp_dummy;
	hw->mac.enable_rx_dma = ngbe_mac_enable_rx_dma_dummy;
	hw->mac.disable_sec_rx_path = ngbe_mac_disable_sec_rx_path_dummy;
	hw->mac.enable_sec_rx_path = ngbe_mac_enable_sec_rx_path_dummy;
	hw->mac.disable_sec_tx_path = ngbe_mac_disable_sec_tx_path_dummy;
	hw->mac.enable_sec_tx_path = ngbe_mac_enable_sec_tx_path_dummy;
	hw->mac.acquire_swfw_sync = ngbe_mac_acquire_swfw_sync_dummy;
	hw->mac.release_swfw_sync = ngbe_mac_release_swfw_sync_dummy;
	hw->mac.init_swfw_sync = ngbe_mac_init_swfw_sync_dummy;
	hw->mac.autoc_read = ngbe_mac_autoc_read_dummy;
	hw->mac.autoc_write = ngbe_mac_autoc_write_dummy;
	hw->mac.prot_autoc_read = ngbe_mac_prot_autoc_read_dummy;
	hw->mac.prot_autoc_write = ngbe_mac_prot_autoc_write_dummy;
	hw->mac.negotiate_api_version = ngbe_mac_negotiate_api_version_dummy;
	hw->mac.disable_tx_laser = ngbe_mac_disable_tx_laser_dummy;
	hw->mac.enable_tx_laser = ngbe_mac_enable_tx_laser_dummy;
	hw->mac.flap_tx_laser = ngbe_mac_flap_tx_laser_dummy;
	hw->mac.setup_link = ngbe_mac_setup_link_dummy;
	hw->mac.setup_mac_link = ngbe_mac_setup_mac_link_dummy;
	hw->mac.check_link = ngbe_mac_check_link_dummy;
	hw->mac.get_link_capabilities = ngbe_mac_get_link_capabilities_dummy;
	hw->mac.set_rate_select_speed = ngbe_mac_set_rate_select_speed_dummy;
	hw->mac.setup_pba = ngbe_mac_setup_pba_dummy;
	hw->mac.led_on = ngbe_mac_led_on_dummy;
	hw->mac.led_off = ngbe_mac_led_off_dummy;
	hw->mac.blink_led_start = ngbe_mac_blink_led_start_dummy;
	hw->mac.blink_led_stop = ngbe_mac_blink_led_stop_dummy;
	hw->mac.init_led_link_act = ngbe_mac_init_led_link_act_dummy;
	hw->mac.set_rar = ngbe_mac_set_rar_dummy;
	hw->mac.set_uc_addr = ngbe_mac_set_uc_addr_dummy;
	hw->mac.clear_rar = ngbe_mac_clear_rar_dummy;
	hw->mac.insert_mac_addr = ngbe_mac_insert_mac_addr_dummy;
	hw->mac.set_vmdq = ngbe_mac_set_vmdq_dummy;
	hw->mac.set_vmdq_san_mac = ngbe_mac_set_vmdq_san_mac_dummy;
	hw->mac.clear_vmdq = ngbe_mac_clear_vmdq_dummy;
	hw->mac.init_rx_addrs = ngbe_mac_init_rx_addrs_dummy;
	hw->mac.update_uc_addr_list = ngbe_mac_update_uc_addr_list_dummy;
	hw->mac.update_mc_addr_list = ngbe_mac_update_mc_addr_list_dummy;
	hw->mac.enable_mc = ngbe_mac_enable_mc_dummy;
	hw->mac.disable_mc = ngbe_mac_disable_mc_dummy;
	hw->mac.clear_vfta = ngbe_mac_clear_vfta_dummy;
	hw->mac.set_vfta = ngbe_mac_set_vfta_dummy;
	hw->mac.set_vlvf = ngbe_mac_set_vlvf_dummy;
	hw->mac.init_uta_tables = ngbe_mac_init_uta_tables_dummy;
	hw->mac.set_mac_anti_spoofing = ngbe_mac_set_mac_anti_spoofing_dummy;
	hw->mac.set_vlan_anti_spoofing = ngbe_mac_set_vlan_anti_spoofing_dummy;
	hw->mac.update_xcast_mode = ngbe_mac_update_xcast_mode_dummy;
	hw->mac.set_rlpml = ngbe_mac_set_rlpml_dummy;
	hw->mac.fc_enable = ngbe_mac_fc_enable_dummy;
	hw->mac.setup_fc = ngbe_mac_setup_fc_dummy;
	hw->mac.fc_autoneg = ngbe_mac_fc_autoneg_dummy;
	hw->mac.set_fw_drv_ver = ngbe_mac_set_fw_drv_ver_dummy;
	hw->mac.get_thermal_sensor_data = ngbe_mac_get_thermal_sensor_data_dummy;
	hw->mac.init_thermal_sensor_thresh = ngbe_mac_init_thermal_sensor_thresh_dummy;
	hw->mac.get_rtrup2tc = ngbe_mac_get_rtrup2tc_dummy;
	hw->mac.disable_rx = ngbe_mac_disable_rx_dummy;
	hw->mac.enable_rx = ngbe_mac_enable_rx_dummy;
	hw->mac.set_source_address_pruning = ngbe_mac_set_source_address_pruning_dummy;
	hw->mac.set_ethertype_anti_spoofing = ngbe_mac_set_ethertype_anti_spoofing_dummy;
	hw->mac.dmac_update_tcs = ngbe_mac_dmac_update_tcs_dummy;
	hw->mac.dmac_config_tcs = ngbe_mac_dmac_config_tcs_dummy;
	hw->mac.dmac_config = ngbe_mac_dmac_config_dummy;
	hw->mac.setup_eee = ngbe_mac_setup_eee_dummy;
	hw->mac.read_iosf_sb_reg = ngbe_mac_read_iosf_sb_reg_dummy;
	hw->mac.write_iosf_sb_reg = ngbe_mac_write_iosf_sb_reg_dummy;
	hw->mac.disable_mdd = ngbe_mac_disable_mdd_dummy;
	hw->mac.enable_mdd = ngbe_mac_enable_mdd_dummy;
	hw->mac.mdd_event = ngbe_mac_mdd_event_dummy;
	hw->mac.restore_mdd_vf = ngbe_mac_restore_mdd_vf_dummy;
	hw->mac.fw_recovery_mode = ngbe_mac_fw_recovery_mode_dummy;
	hw->phy.identify = ngbe_phy_identify_dummy;
	hw->phy.identify_sfp = ngbe_phy_identify_sfp_dummy;
	hw->phy.init_hw = ngbe_phy_init_hw_dummy;
	hw->phy.reset_hw = ngbe_phy_reset_hw_dummy;
	hw->phy.read_reg = ngbe_phy_read_reg_dummy;
	hw->phy.write_reg = ngbe_phy_write_reg_dummy;
	hw->phy.read_reg_unlocked = ngbe_phy_read_reg_unlocked_dummy;
	hw->phy.write_reg_unlocked = ngbe_phy_write_reg_unlocked_dummy;
	hw->phy.setup_link = ngbe_phy_setup_link_dummy;
	hw->phy.check_link = ngbe_phy_check_link_dummy;
	hw->phy.get_firmware_version = ngbe_phy_get_firmware_version_dummy;
	hw->phy.read_i2c_byte = ngbe_phy_read_i2c_byte_dummy;
	hw->phy.write_i2c_byte = ngbe_phy_write_i2c_byte_dummy;
	hw->phy.read_i2c_sff8472 = ngbe_phy_read_i2c_sff8472_dummy;
	hw->phy.read_i2c_eeprom = ngbe_phy_read_i2c_eeprom_dummy;
	hw->phy.write_i2c_eeprom = ngbe_phy_write_i2c_eeprom_dummy;
	hw->phy.i2c_bus_clear = ngbe_phy_i2c_bus_clear_dummy;
	hw->phy.check_overtemp = ngbe_phy_check_overtemp_dummy;
	hw->phy.set_phy_power = ngbe_phy_set_phy_power_dummy;
	hw->phy.enter_lplu = ngbe_phy_enter_lplu_dummy;
	hw->phy.handle_lasi = ngbe_phy_handle_lasi_dummy;
	hw->phy.read_i2c_byte_unlocked = ngbe_phy_read_i2c_byte_unlocked_dummy;
	hw->phy.write_i2c_byte_unlocked = ngbe_phy_write_i2c_byte_unlocked_dummy;
	hw->link.read_link = ngbe_link_read_link_dummy;
	hw->link.read_link_unlocked = ngbe_link_read_link_unlocked_dummy;
	hw->link.write_link = ngbe_link_write_link_dummy;
	hw->link.write_link_unlocked = ngbe_link_write_link_unlocked_dummy;
	hw->mbx.init_params = ngbe_mbx_init_params_dummy;
	hw->mbx.read = ngbe_mbx_read_dummy;
	hw->mbx.write = ngbe_mbx_write_dummy;
	hw->mbx.read_posted = ngbe_mbx_read_posted_dummy;
	hw->mbx.write_posted = ngbe_mbx_write_posted_dummy;
	hw->mbx.check_for_msg = ngbe_mbx_check_for_msg_dummy;
	hw->mbx.check_for_ack = ngbe_mbx_check_for_ack_dummy;
	hw->mbx.check_for_rst = ngbe_mbx_check_for_rst_dummy;
}

#endif /* _NGBE_TYPE_DUMMY_H_ */

