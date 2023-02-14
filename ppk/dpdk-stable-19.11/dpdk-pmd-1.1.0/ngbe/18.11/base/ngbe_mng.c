/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "ngbe_type.h"
#include "ngbe_phy.h"
#include "ngbe_mng.h"

/**
 *  ngbe_calculate_checksum - Calculate checksum for buffer
 *  @buffer: pointer to EEPROM
 *  @length: size of EEPROM to calculate a checksum for
 *  Calculates the checksum for some buffer on a specified length.  The
 *  checksum calculated is returned.
 **/
static u8
ngbe_calculate_checksum(u8 *buffer, u32 length)
{
	u32 i;
	u8 sum = 0;

	for (i = 0; i < length; i++)
		sum += buffer[i];

	return (u8) (0 - sum);
}

/**
 *  ngbe_hic_unlocked - Issue command to manageability block unlocked
 *  @hw: pointer to the HW structure
 *  @buffer: command to write and where the return status will be placed
 *  @length: length of buffer, must be multiple of 4 bytes
 *  @timeout: time in ms to wait for command completion
 *
 *  Communicates with the manageability block. On success return 0
 *  else returns semaphore error when encountering an error acquiring
 *  semaphore or NGBE_ERR_HOST_INTERFACE_COMMAND when command fails.
 *
 *  This function assumes that the NGBE_GSSR_SW_MNG_SM semaphore is held
 *  by the caller.
 **/
static s32
ngbe_hic_unlocked(struct ngbe_hw *hw, u32 *buffer, u32 length, u32 timeout)
{
	u32 value, loop;
	u16 i, dword_len;

	DEBUGFUNC("ngbe_hic_unlocked");

	if (!length || length > NGBE_PMMBX_BSIZE) {
		DEBUGOUT("Buffer length failure buffersize=%d.\n", length);
		return NGBE_ERR_HOST_INTERFACE_COMMAND;
	}

	/* Calculate length in DWORDs. We must be DWORD aligned */
	if (length % sizeof(u32)) {
		DEBUGOUT("Buffer length failure, not aligned to dword");
		return NGBE_ERR_INVALID_ARGUMENT;
	}

	dword_len = length >> 2;

	/* The device driver writes the relevant command block
	 * into the ram area.
	 */
	for (i = 0; i < dword_len; i++) {
		wr32a(hw, NGBE_MNGMBX, i, cpu_to_le32(buffer[i]));
		buffer[i] = rd32a(hw, NGBE_MNGMBX, i); /* flush */
	}
	ngbe_flush(hw);

	/* Setting this bit tells the ARC that a new command is pending. */
	wr32m(hw, NGBE_MNGMBXCTL,
	      NGBE_MNGMBXCTL_SWRDY, NGBE_MNGMBXCTL_SWRDY);

	/* Check command completion */
	loop = po32m(hw, NGBE_MNGMBXCTL,
		NGBE_MNGMBXCTL_FWRDY, NGBE_MNGMBXCTL_FWRDY,
		&value, timeout, 1000);
	if (!loop || !(value & NGBE_MNGMBXCTL_FWACK)) {
		DEBUGOUT("Command has failed with no status valid.\n");
		return NGBE_ERR_HOST_INTERFACE_COMMAND;
	}

	return 0;
}

/**
 *  ngbe_host_interface_command - Issue command to manageability block
 *  @hw: pointer to the HW structure
 *  @buffer: contains the command to write and where the return status will
 *   be placed
 *  @length: length of buffer, must be multiple of 4 bytes
 *  @timeout: time in ms to wait for command completion
 *  @return_data: read and return data from the buffer (true) or not (false)
 *   Needed because FW structures are big endian and decoding of
 *   these fields can be 8 bit or 16 bit based on command. Decoding
 *   is not easily understood without making a table of commands.
 *   So we will leave this up to the caller to read back the data
 *   in these cases.
 *
 *  Communicates with the manageability block. On success return 0
 *  else returns semaphore error when encountering an error acquiring
 *  semaphore or NGBE_ERR_HOST_INTERFACE_COMMAND when command fails.
 **/
static s32
ngbe_host_interface_command(struct ngbe_hw *hw, u32 *buffer,
				 u32 length, u32 timeout, bool return_data)
{
	u32 hdr_size = sizeof(struct ngbe_hic_hdr);
	struct ngbe_hic_hdr *resp = (struct ngbe_hic_hdr *)buffer;
	u16 buf_len;
	s32 err;
	u32 bi;
	u32 dword_len;

	DEBUGFUNC("ngbe_host_interface_command");

	if (length == 0 || length > NGBE_PMMBX_BSIZE) {
		DEBUGOUT("Buffer length failure buffersize=%d.\n", length);
		return NGBE_ERR_HOST_INTERFACE_COMMAND;
	}

	/* Take management host interface semaphore */
	err = hw->mac.acquire_swfw_sync(hw, NGBE_MNGSEM_SWMBX);
	if (err)
		return err;

	err = ngbe_hic_unlocked(hw, buffer, length, timeout);
	if (err)
		goto rel_out;

	if (!return_data)
		goto rel_out;

	/* Calculate length in DWORDs */
	dword_len = hdr_size >> 2;

	/* first pull in the header so we know the buffer length */
	for (bi = 0; bi < dword_len; bi++) {
		buffer[bi] = rd32a(hw, NGBE_MNGMBX, bi);
	}

	/*
	 * If there is any thing in data position pull it in
	 * Read Flash command requires reading buffer length from
	 * two bytes instead of one byte
	 */
	if (resp->cmd == 0x30) {
		for (; bi < dword_len + 2; bi++) {
			buffer[bi] = rd32a(hw, NGBE_MNGMBX, bi);
		}
		buf_len = (((u16)(resp->cmd_or_resp.ret_status) << 3)
				  & 0xF00) | resp->buf_len;
		hdr_size += (2 << 2);
	} else {
		buf_len = resp->buf_len;
	}
	if (!buf_len)
		goto rel_out;

	if (length < buf_len + hdr_size) {
		DEBUGOUT("Buffer not large enough for reply message.\n");
		err = NGBE_ERR_HOST_INTERFACE_COMMAND;
		goto rel_out;
	}

	/* Calculate length in DWORDs, add 3 for odd lengths */
	dword_len = (buf_len + 3) >> 2;

	/* Pull in the rest of the buffer (bi is where we left off) */
	for (; bi <= dword_len; bi++) {
		buffer[bi] = rd32a(hw, NGBE_MNGMBX, bi);
	}

rel_out:
	hw->mac.release_swfw_sync(hw, NGBE_MNGSEM_SWMBX);

	return err;
}

/**
 *  ngbe_hic_sr_read - Read EEPROM word using a host interface cmd
 *  assuming that the semaphore is already obtained.
 *  @hw: pointer to hardware structure
 *  @offset: offset of  word in the EEPROM to read
 *  @data: word read from the EEPROM
 *
 *  Reads a 16 bit word from the EEPROM using the hostif.
 **/
s32 ngbe_hic_sr_read(struct ngbe_hw *hw, u32 addr, u8 *buf, int len)
{
	struct ngbe_hic_read_shadow_ram command;
	u32 value;
	int err, i = 0, j = 0;

	if (len > NGBE_PMMBX_DATA_SIZE)
		return NGBE_ERR_HOST_INTERFACE_COMMAND;

	memset(&command, 0, sizeof(command));
	command.hdr.req.cmd = FW_READ_SHADOW_RAM_CMD;
	command.hdr.req.buf_lenh = 0;
	command.hdr.req.buf_lenl = FW_READ_SHADOW_RAM_LEN;
	command.hdr.req.checksum = FW_DEFAULT_CHECKSUM;
	command.address = cpu_to_be32(addr);
	command.length = cpu_to_be16(len);

	err = ngbe_hic_unlocked(hw, (u32 *)&command,
			sizeof(command), NGBE_HI_COMMAND_TIMEOUT);
	if (err)
		return err;

	while (i < (len >> 2)) {
		value = rd32a(hw, NGBE_MNGMBX, FW_NVM_DATA_OFFSET + i);
		((u32 *)buf)[i] = value;
		i++;
	}

	value = rd32a(hw, NGBE_MNGMBX, FW_NVM_DATA_OFFSET + i);
	for (i <<= 2; i < len; i++) {
		((u8 *)buf)[i] = ((u8 *)&value)[j++];
	}

	return 0;
}

/**
 *  ngbe_hic_sr_write - Write EEPROM word using hostif
 *  @hw: pointer to hardware structure
 *  @offset: offset of  word in the EEPROM to write
 *  @data: word write to the EEPROM
 *
 *  Write a 16 bit word to the EEPROM using the hostif.
 **/
s32 ngbe_hic_sr_write(struct ngbe_hw *hw, u32 addr, u8 *buf, int len)
{
	struct ngbe_hic_write_shadow_ram command;
	u32 value;
	int err, i = 0, j = 0;

	if (len > NGBE_PMMBX_DATA_SIZE)
		return NGBE_ERR_HOST_INTERFACE_COMMAND;

	memset(&command, 0, sizeof(command));
	command.hdr.req.cmd = FW_WRITE_SHADOW_RAM_CMD;
	command.hdr.req.buf_lenh = 0;
	command.hdr.req.buf_lenl = FW_WRITE_SHADOW_RAM_LEN;
	command.hdr.req.checksum = FW_DEFAULT_CHECKSUM;
	command.address = cpu_to_be32(addr);
	command.length = cpu_to_be16(len);

	while(i < (len >> 2)) {
		value = ((u32 *)buf)[i];
		wr32a(hw, NGBE_MNGMBX, FW_NVM_DATA_OFFSET + i, value);
		i++;
	}

	for (i <<= 2; i < len; i++) {
		((u8 *)&value)[j++] = ((u8 *)buf)[i];
	}
	wr32a(hw, NGBE_MNGMBX, FW_NVM_DATA_OFFSET + (i >> 2), value);

#if 0
	err = ngbe_hic_unlocked(hw, (u32 *)&command,
			sizeof(command), NGBE_HI_COMMAND_TIMEOUT);
#else
	err = 0; //fixme
	UNREFERENCED_PARAMETER(&command);
#endif
	return err;
}

s32 ngbe_hic_pcie_read(struct ngbe_hw *hw, u16 addr, u32 *buf, int len)
{
	struct ngbe_hic_read_pcie command;
	u32 value = 0;
	int err, i = 0;

	if (len > NGBE_PMMBX_DATA_SIZE)
		return NGBE_ERR_HOST_INTERFACE_COMMAND;

	memset(&command, 0, sizeof(command));
	command.hdr.cmd = FW_PCIE_READ_CMD;
	command.hdr.buf_len = sizeof(command) - sizeof(command.hdr);
	command.hdr.checksum = FW_DEFAULT_CHECKSUM;
	command.lan_id = hw->bus.lan_id;
	command.addr = addr;

	err = ngbe_host_interface_command(hw, (u32 *)&command,
			sizeof(command), NGBE_HI_COMMAND_TIMEOUT, false);
	if (err)
		return err;

	while (i < (len >> 2)) {
		value = rd32a(hw, NGBE_MNGMBX, FW_PCIE_BUSMASTER_OFFSET + i);
		((u32 *)buf)[i] = value;
		i++;
	}

	return 0;
}

s32 ngbe_hic_pcie_write(struct ngbe_hw *hw, u16 addr, u32 *buf, int len)
{
	struct ngbe_hic_write_pcie command;
	u32 value = 0;
	int err, i = 0;

	while(i < (len >> 2)) {
		value = ((u32 *)buf)[i];
		i++;
	}

	memset(&command, 0, sizeof(command));
	command.hdr.cmd = FW_PCIE_WRITE_CMD;
	command.hdr.buf_len = sizeof(command) - sizeof(command.hdr);
	command.hdr.checksum = FW_DEFAULT_CHECKSUM;
	command.lan_id = hw->bus.lan_id;
	command.addr = addr;
	command.data = value;

	err = ngbe_host_interface_command(hw, (u32 *)&command,
			sizeof(command), NGBE_HI_COMMAND_TIMEOUT, false);
	if (err)
		return err;

	return 0;
}

/**
 *  ngbe_hic_set_drv_ver - Sends driver version to firmware
 *  @hw: pointer to the HW structure
 *  @maj: driver version major number
 *  @min: driver version minor number
 *  @build: driver version build number
 *  @sub: driver version sub build number
 *  @len: unused
 *  @driver_ver: unused
 *
 *  Sends driver version number to firmware through the manageability
 *  block.  On success return 0
 *  else returns NGBE_ERR_SWFW_SYNC when encountering an error acquiring
 *  semaphore or NGBE_ERR_HOST_INTERFACE_COMMAND when command fails.
 **/
s32 ngbe_hic_set_drv_ver(struct ngbe_hw *hw, u8 maj, u8 min,
				 u8 build, u8 sub, u16 len,
				 const char *driver_ver)
{
	struct ngbe_hic_drv_info fw_cmd;
	int i;
	s32 ret_val = 0;

	DEBUGFUNC("ngbe_hic_set_drv_ver");
	UNREFERENCED_PARAMETER(len, driver_ver);

	fw_cmd.hdr.cmd = FW_CEM_CMD_DRIVER_INFO;
	fw_cmd.hdr.buf_len = FW_CEM_CMD_DRIVER_INFO_LEN;
	fw_cmd.hdr.cmd_or_resp.cmd_resv = FW_CEM_CMD_RESERVED;
	fw_cmd.port_num = (u8)hw->bus.func;
	fw_cmd.ver_maj = maj;
	fw_cmd.ver_min = min;
	fw_cmd.ver_build = build;
	fw_cmd.ver_sub = sub;
	fw_cmd.hdr.checksum = 0;
	fw_cmd.pad = 0;
	fw_cmd.pad2 = 0;
	fw_cmd.hdr.checksum = ngbe_calculate_checksum((u8 *)&fw_cmd,
				(FW_CEM_HDR_LEN + fw_cmd.hdr.buf_len));

	for (i = 0; i <= FW_CEM_MAX_RETRIES; i++) {
		ret_val = ngbe_host_interface_command(hw, (u32 *)&fw_cmd,
						       sizeof(fw_cmd),
						       NGBE_HI_COMMAND_TIMEOUT,
						       true);
		if (ret_val != 0)
			continue;

		if (fw_cmd.hdr.cmd_or_resp.ret_status ==
		    FW_CEM_RESP_STATUS_SUCCESS)
			ret_val = 0;
		else
			ret_val = NGBE_ERR_HOST_INTERFACE_COMMAND;

		break;
	}

	return ret_val;
}

/**
 *  ngbe_hic_reset - send reset cmd to fw
 *  @hw: pointer to hardware structure
 *
 *  Sends reset cmd to firmware through the manageability
 *  block.  On success return 0
 *  else returns NGBE_ERR_SWFW_SYNC when encountering an error acquiring
 *  semaphore or NGBE_ERR_HOST_INTERFACE_COMMAND when command fails.
 **/
s32
ngbe_hic_reset(struct ngbe_hw *hw)
{
	struct ngbe_hic_reset reset_cmd;
	int i;
	s32 err = 0;

	DEBUGFUNC("\n");

	reset_cmd.hdr.cmd = FW_RESET_CMD;
	reset_cmd.hdr.buf_len = FW_RESET_LEN;
	reset_cmd.hdr.cmd_or_resp.cmd_resv = FW_CEM_CMD_RESERVED;
	reset_cmd.lan_id = hw->bus.lan_id;
	reset_cmd.reset_type = (u16)hw->reset_type;
	reset_cmd.hdr.checksum = 0;
	reset_cmd.hdr.checksum = ngbe_calculate_checksum((u8 *)&reset_cmd,
				(FW_CEM_HDR_LEN + reset_cmd.hdr.buf_len));

	for (i = 0; i <= FW_CEM_MAX_RETRIES; i++) {
		err = ngbe_host_interface_command(hw, (u32 *)&reset_cmd,
						       sizeof(reset_cmd),
						       NGBE_HI_COMMAND_TIMEOUT,
						       true);
		if (err != 0)
			continue;

		if (reset_cmd.hdr.cmd_or_resp.ret_status ==
		    FW_CEM_RESP_STATUS_SUCCESS)
			err = 0;
		else
			err = NGBE_ERR_HOST_INTERFACE_COMMAND;

		break;
	}

	return err;
}

/**
 * ngbe_mng_present - returns true when management capability is present
 * @hw: pointer to hardware structure
 */
bool
ngbe_mng_present(struct ngbe_hw *hw)
{
	if (hw->mac.type == ngbe_mac_unknown)
		return false;

	return !!rd32m(hw, NGBE_STAT, NGBE_STAT_MNGINIT);
}

s32 ngbe_hic_check_cap(struct ngbe_hw *hw)
{
	struct ngbe_hic_read_shadow_ram command;
	s32 err;
	int i;

	DEBUGFUNC("\n");

	command.hdr.req.cmd = FW_EEPROM_CHECK_STATUS;
	command.hdr.req.buf_lenh = 0;
	command.hdr.req.buf_lenl = 0;
	command.hdr.req.checksum = FW_DEFAULT_CHECKSUM;

	/* convert offset from words to bytes */
	command.address = 0;
	/* one word */
	command.length = 0;

	for (i = 0; i <= FW_CEM_MAX_RETRIES; i++) {
		err = ngbe_host_interface_command(hw, (u32 *)&command,
				sizeof(command),
				NGBE_HI_COMMAND_TIMEOUT, true);
		if (err)
			continue;

		command.hdr.rsp.ret_status &= 0x1F;
		if (command.hdr.rsp.ret_status !=
			FW_CEM_RESP_STATUS_SUCCESS)
			err = NGBE_ERR_HOST_INTERFACE_COMMAND;

		break;
	}

	if (!err && command.address != FW_CHECKSUM_CAP_ST_PASS) {
		err = NGBE_ERR_EEPROM_CHECKSUM;
	}

	return err;
}

s32 ngbe_phy_led_oem_chk(struct ngbe_hw *hw, u32 *data)
{
	struct ngbe_hic_read_shadow_ram command;
	s32 err;
	int i;

	DEBUGFUNC("\n");

	command.hdr.req.cmd = FW_PHY_LED_CONF;
	command.hdr.req.buf_lenh = 0;
	command.hdr.req.buf_lenl = 0;
	command.hdr.req.checksum = FW_DEFAULT_CHECKSUM;

	/* convert offset from words to bytes */
	command.address = 0;
	/* one word */
	command.length = 0;

	for (i = 0; i <= FW_CEM_MAX_RETRIES; i++) {
		err = ngbe_host_interface_command(hw, (u32 *)&command,
				sizeof(command),
				NGBE_HI_COMMAND_TIMEOUT, true);
		if (err)
			continue;

		command.hdr.rsp.ret_status &= 0x1F;
		if (command.hdr.rsp.ret_status !=
			FW_CEM_RESP_STATUS_SUCCESS)
			err = NGBE_ERR_HOST_INTERFACE_COMMAND;

		break;
	}

	if (err)
		return err;

	if (command.address == FW_CHECKSUM_CAP_ST_PASS) {
		*data = ((u32 *)&command)[2];
		err = 0;
	} else if (command.address == FW_CHECKSUM_CAP_ST_FAIL) {
		*data = FW_CHECKSUM_CAP_ST_FAIL;
		err = -1;
	} else {
		err = NGBE_ERR_EEPROM_CHECKSUM;
	}

	return err;
}

s32 ngbe_phy_signal_set(struct ngbe_hw *hw)
{
	s32 status;
	struct ngbe_hic_read_shadow_ram buffer;

	DEBUGFUNC("\n");
	buffer.hdr.req.cmd = FW_PHY_SIGNAL;
	buffer.hdr.req.buf_lenh = 0;
	buffer.hdr.req.buf_lenl = 0;
	buffer.hdr.req.checksum = FW_DEFAULT_CHECKSUM;

	/* convert offset from words to bytes */
	buffer.address = 0;
	/* one word */
	buffer.length = 0;

	status = ngbe_host_interface_command(hw, (u32 *)&buffer,
			sizeof(buffer), NGBE_HI_COMMAND_TIMEOUT, false);

	if (status)
		return status;

	return status;
}
