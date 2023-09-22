/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2001-2018
 */

#include "ngbe_phy.h"

#ifndef _NGBE_PHY_MTD_H_
#define _NGBE_PHY_MTD_H_

/* PHY IDs*/
#define NGBE_PHYID_MTD3310             0x00000000U

/* PHY MMIO Registers */
#define SR_XS_PCS_MMD_STATUS1           0x030001
#define SR_XS_PCS_CTRL2                 0x030007
#define   SR_PCS_CTRL2_TYPE_SEL         MS16(0, 0x3)
#define   SR_PCS_CTRL2_TYPE_SEL_R       LS16(0, 0, 0x3)
#define   SR_PCS_CTRL2_TYPE_SEL_X       LS16(1, 0, 0x3)
#define   SR_PCS_CTRL2_TYPE_SEL_W       LS16(2, 0, 0x3)
#define SR_PMA_CTRL1                    0x010000
#define   SR_PMA_CTRL1_SS13             MS16(13, 0x1)
#define   SR_PMA_CTRL1_SS13_KX          LS16(0, 13, 0x1)
#define   SR_PMA_CTRL1_SS13_KX4         LS16(1, 13, 0x1)
#define   SR_PMA_CTRL1_LB               MS16(0, 0x1)
#define SR_MII_MMD_CTL                  0x1F0000
#define   SR_MII_MMD_CTL_AN_EN              0x1000
#define   SR_MII_MMD_CTL_RESTART_AN         0x0200
#define SR_MII_MMD_DIGI_CTL             0x1F8000
#define SR_MII_MMD_AN_CTL               0x1F8001
#define SR_MII_MMD_AN_ADV               0x1F0004
#define   SR_MII_MMD_AN_ADV_PAUSE(v)    ((0x3 & (v)) << 7)
#define   SR_MII_MMD_AN_ADV_PAUSE_ASM   0x80
#define   SR_MII_MMD_AN_ADV_PAUSE_SYM   0x100
#define SR_MII_MMD_LP_BABL              0x1F0005
#define SR_AN_CTRL                      0x070000
#define   SR_AN_CTRL_RSTRT_AN           MS16(9, 0x1)
#define   SR_AN_CTRL_AN_EN              MS16(12, 0x1)
#define SR_AN_MMD_ADV_REG1                0x070010
#define   SR_AN_MMD_ADV_REG1_PAUSE(v)      ((0x3 & (v)) << 10)
#define   SR_AN_MMD_ADV_REG1_PAUSE_SYM      0x400
#define   SR_AN_MMD_ADV_REG1_PAUSE_ASM      0x800
#define SR_AN_MMD_ADV_REG2                0x070011
#define   SR_AN_MMD_ADV_REG2_BP_TYPE_KX4    0x40
#define   SR_AN_MMD_ADV_REG2_BP_TYPE_KX     0x20
#define   SR_AN_MMD_ADV_REG2_BP_TYPE_KR     0x80
#define   SR_AN_MMD_ADV_REG2_BP_TYPE_MASK   0xFFFF
#define SR_AN_MMD_LP_ABL1                 0x070013
#define VR_AN_KR_MODE_CL                  0x078003
#define VR_XS_OR_PCS_MMD_DIGI_CTL1        0x038000
#define   VR_XS_OR_PCS_MMD_DIGI_CTL1_ENABLE 0x1000
#define   VR_XS_OR_PCS_MMD_DIGI_CTL1_VR_RST 0x8000
#define VR_XS_OR_PCS_MMD_DIGI_STATUS      0x038010
#define   VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_MASK            0x1C
#define   VR_XS_OR_PCS_MMD_DIGI_STATUS_PSEQ_POWER_GOOD      0x10

#define NGBE_PHY_MPLLA_CTL0                    0x018071
#define NGBE_PHY_MPLLA_CTL3                    0x018077
#define NGBE_PHY_MISC_CTL0                     0x018090
#define NGBE_PHY_VCO_CAL_LD0                   0x018092
#define NGBE_PHY_VCO_CAL_LD1                   0x018093
#define NGBE_PHY_VCO_CAL_LD2                   0x018094
#define NGBE_PHY_VCO_CAL_LD3                   0x018095
#define NGBE_PHY_VCO_CAL_REF0                  0x018096
#define NGBE_PHY_VCO_CAL_REF1                  0x018097
#define NGBE_PHY_RX_AD_ACK                     0x018098
#define NGBE_PHY_AFE_DFE_ENABLE                0x01805D
#define NGBE_PHY_DFE_TAP_CTL0                  0x01805E
#define NGBE_PHY_RX_EQ_ATT_LVL0                0x018057
#define NGBE_PHY_RX_EQ_CTL0                    0x018058
#define NGBE_PHY_RX_EQ_CTL                     0x01805C
#define NGBE_PHY_TX_EQ_CTL0                    0x018036
#define NGBE_PHY_TX_EQ_CTL1                    0x018037
#define NGBE_PHY_TX_RATE_CTL                   0x018034
#define NGBE_PHY_RX_RATE_CTL                   0x018054
#define NGBE_PHY_TX_GEN_CTL2                   0x018032
#define NGBE_PHY_RX_GEN_CTL2                   0x018052
#define NGBE_PHY_RX_GEN_CTL3                   0x018053
#define NGBE_PHY_MPLLA_CTL2                    0x018073
#define NGBE_PHY_RX_POWER_ST_CTL               0x018055
#define NGBE_PHY_TX_POWER_ST_CTL               0x018035
#define NGBE_PHY_TX_GENCTRL1                   0x018031

#define NGBE_PHY_MPLLA_CTL0_MULTIPLIER_1GBASEX_KX              32
#define NGBE_PHY_MPLLA_CTL0_MULTIPLIER_10GBASER_KR             33
#define NGBE_PHY_MPLLA_CTL0_MULTIPLIER_OTHER                   40
#define NGBE_PHY_MPLLA_CTL0_MULTIPLIER_MASK                    0xFF
#define NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_1GBASEX_KX           0x46
#define NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_10GBASER_KR          0x7B
#define NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_OTHER                0x56
#define NGBE_PHY_MPLLA_CTL3_MULTIPLIER_BW_MASK                 0x7FF
#define NGBE_PHY_MISC_CTL0_TX2RX_LB_EN_0                       0x1
#define NGBE_PHY_MISC_CTL0_TX2RX_LB_EN_3_1                     0xE
#define NGBE_PHY_MISC_CTL0_RX_VREF_CTRL                        0x1F00
#define NGBE_PHY_VCO_CAL_LD0_1GBASEX_KX                        1344
#define NGBE_PHY_VCO_CAL_LD0_10GBASER_KR                       1353
#define NGBE_PHY_VCO_CAL_LD0_OTHER                             1360
#define NGBE_PHY_VCO_CAL_LD0_MASK                              0x1000
#define NGBE_PHY_VCO_CAL_REF0_LD0_1GBASEX_KX                   42
#define NGBE_PHY_VCO_CAL_REF0_LD0_10GBASER_KR                  41
#define NGBE_PHY_VCO_CAL_REF0_LD0_OTHER                        34
#define NGBE_PHY_VCO_CAL_REF0_LD0_MASK                         0x3F
#define NGBE_PHY_AFE_DFE_ENABLE_DFE_EN0                        0x10
#define NGBE_PHY_AFE_DFE_ENABLE_AFE_EN0                        0x1
#define NGBE_PHY_AFE_DFE_ENABLE_MASK                           0xFF
#define NGBE_PHY_RX_EQ_CTL_CONT_ADAPT0                         0x1
#define NGBE_PHY_RX_EQ_CTL_CONT_ADAPT_MASK                     0xF
#define NGBE_PHY_TX_RATE_CTL_TX0_RATE_10GBASER_KR              0x0
#define NGBE_PHY_TX_RATE_CTL_TX0_RATE_RXAUI                    0x1
#define NGBE_PHY_TX_RATE_CTL_TX0_RATE_1GBASEX_KX               0x3
#define NGBE_PHY_TX_RATE_CTL_TX0_RATE_OTHER                    0x2
#define NGBE_PHY_TX_RATE_CTL_TX1_RATE_OTHER                    0x20
#define NGBE_PHY_TX_RATE_CTL_TX2_RATE_OTHER                    0x200
#define NGBE_PHY_TX_RATE_CTL_TX3_RATE_OTHER                    0x2000
#define NGBE_PHY_TX_RATE_CTL_TX0_RATE_MASK                     0x7
#define NGBE_PHY_TX_RATE_CTL_TX1_RATE_MASK                     0x70
#define NGBE_PHY_TX_RATE_CTL_TX2_RATE_MASK                     0x700
#define NGBE_PHY_TX_RATE_CTL_TX3_RATE_MASK                     0x7000
#define NGBE_PHY_RX_RATE_CTL_RX0_RATE_10GBASER_KR              0x0
#define NGBE_PHY_RX_RATE_CTL_RX0_RATE_RXAUI                    0x1
#define NGBE_PHY_RX_RATE_CTL_RX0_RATE_1GBASEX_KX               0x3
#define NGBE_PHY_RX_RATE_CTL_RX0_RATE_OTHER                    0x2
#define NGBE_PHY_RX_RATE_CTL_RX1_RATE_OTHER                    0x20
#define NGBE_PHY_RX_RATE_CTL_RX2_RATE_OTHER                    0x200
#define NGBE_PHY_RX_RATE_CTL_RX3_RATE_OTHER                    0x2000
#define NGBE_PHY_RX_RATE_CTL_RX0_RATE_MASK                     0x7
#define NGBE_PHY_RX_RATE_CTL_RX1_RATE_MASK                     0x70
#define NGBE_PHY_RX_RATE_CTL_RX2_RATE_MASK                     0x700
#define NGBE_PHY_RX_RATE_CTL_RX3_RATE_MASK                     0x7000
#define NGBE_PHY_TX_GEN_CTL2_TX0_WIDTH_10GBASER_KR             0x200
#define NGBE_PHY_TX_GEN_CTL2_TX0_WIDTH_10GBASER_KR_RXAUI       0x300
#define NGBE_PHY_TX_GEN_CTL2_TX0_WIDTH_OTHER                   0x100
#define NGBE_PHY_TX_GEN_CTL2_TX0_WIDTH_MASK                    0x300
#define NGBE_PHY_TX_GEN_CTL2_TX1_WIDTH_OTHER                   0x400
#define NGBE_PHY_TX_GEN_CTL2_TX1_WIDTH_MASK                    0xC00
#define NGBE_PHY_TX_GEN_CTL2_TX2_WIDTH_OTHER                   0x1000
#define NGBE_PHY_TX_GEN_CTL2_TX2_WIDTH_MASK                    0x3000
#define NGBE_PHY_TX_GEN_CTL2_TX3_WIDTH_OTHER                   0x4000
#define NGBE_PHY_TX_GEN_CTL2_TX3_WIDTH_MASK                    0xC000
#define NGBE_PHY_RX_GEN_CTL2_RX0_WIDTH_10GBASER_KR             0x200
#define NGBE_PHY_RX_GEN_CTL2_RX0_WIDTH_10GBASER_KR_RXAUI       0x300
#define NGBE_PHY_RX_GEN_CTL2_RX0_WIDTH_OTHER                   0x100
#define NGBE_PHY_RX_GEN_CTL2_RX0_WIDTH_MASK                    0x300
#define NGBE_PHY_RX_GEN_CTL2_RX1_WIDTH_OTHER                   0x400
#define NGBE_PHY_RX_GEN_CTL2_RX1_WIDTH_MASK                    0xC00
#define NGBE_PHY_RX_GEN_CTL2_RX2_WIDTH_OTHER                   0x1000
#define NGBE_PHY_RX_GEN_CTL2_RX2_WIDTH_MASK                    0x3000
#define NGBE_PHY_RX_GEN_CTL2_RX3_WIDTH_OTHER                   0x4000
#define NGBE_PHY_RX_GEN_CTL2_RX3_WIDTH_MASK                    0xC000
#define NGBE_PHY_MPLLA_CTL2_DIV_CLK_EN_8                       0x100
#define NGBE_PHY_MPLLA_CTL2_DIV_CLK_EN_10                      0x200
#define NGBE_PHY_MPLLA_CTL2_DIV_CLK_EN_16P5                    0x400
#define NGBE_PHY_MPLLA_CTL2_DIV_CLK_EN_MASK                    0x700

s32 ngbe_read_phy_reg_mtd(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			u16 *phy_data);
s32 ngbe_write_phy_reg_mtd(struct ngbe_hw *hw, u32 reg_addr, u32 device_type,
			u16 phy_data);

#endif /* _NGBE_PHY_MTD_H_ */