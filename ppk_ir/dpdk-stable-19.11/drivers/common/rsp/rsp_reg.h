#ifndef _RSP_REG_H_
#define _RSP_REG_H_


#define        RSP_MAX_NPUB_BANK_PER_DEV    1
#define        RSP_MAX_RPU_QPS_NUM          4
#define        RSP_MAX_BULK_QPS_NUM         2
#define        RSP_MAX_PUB_QPS_NUM          2


#define        NPUB_RING_TYPE_BULK          0
#define        NPUB_RING_TYPE_PUB           1

#define RSP_RING_INFO_REG                       (0x00798024)

#define S10_RPU_REG_BASE                        0x200000
#define RPU_REG_BASE                            0x300000
#define RPU_CORE_MASK(model)                    (((model==RSP_MODEL_S10)?(S10_RPU_REG_BASE+0x1c):(RPU_REG_BASE+0x40)))
#define RPU_START_REG(model)                    (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0)
#define RPU_STOP_REG(model)                     (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x04)
#define RPU_IDLE_RING_REG(model)                (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x08)
#define RPU_ENDIAN_REG(model)                   (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x10)

#define RPU_TX_RING_BASE_ADDR_L_REG(model, n)   (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0100 + 0x04*(n))
#define RPU_TX_RING_BASE_ADDR_H_REG(model, n)   (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0300 + 0x04*(n))
#define RPU_TX_RING_SIZE(model, n)              (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0500 + 0x04*(n))
#define RPU_TX_RING_HEAD_REG(model, n)          (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0700 + 0x04*(n))
#define RPU_TX_RING_TAIL_REG(model, n)          (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0900 + 0x04*(n))
#define RPU_TX_RING_STATUS_REG(model, n)        (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0b00 + 0x04*(n))

#define RPU_RX_RING_BASE_ADDR_L_REG(model, n)   (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0d00 + 0x04*(n))
#define RPU_RX_RING_BASE_ADDR_H_REG(model, n)   (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x0f00 + 0x04*(n))
#define RPU_RX_RING_SIZE(model, n)              (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x1100 + 0x04*(n))
#define RPU_RX_RING_HEAD_REG(model, n)          (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x1300 + 0x04*(n))
#define RPU_RX_RING_TAIL_REG(model, n)          (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x1500 + 0x04*(n))
#define RPU_RX_RING_STATUS_REG(model, n)        (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x1700 + 0x04*(n))

#define VF_NUM_REG(model)                       (((model==RSP_MODEL_S10)?S10_RPU_REG_BASE:RPU_REG_BASE) + 0x00f000)

#define RSP_COMMOM_CFG3                         0x00520560

#define NPUB_RING_BASE                          0x00500000
#define NPUB_RING_REG_SIZE                      0x100
#define NPUB_RING_REQUEST_BASE_LO(bk,n)         (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x00)
#define NPUB_RING_REQUEST_BASE_HI(bk,n)         (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x10)
#define NPUB_RING_RESPONSE_BASE_LO(bk,n)        (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x20)
#define NPUB_RING_RESPONSE_BASE_HI(bk,n)        (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x30)
#define NPUB_RING_SIZE(bk,n)                    (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x40)
#define NPUB_RING_TYPE(bk,n)                    (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x50)
#define NPUB_RING_PLUS(bk,n)                    (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x60)
#define NPUB_RING_CLEAR(bk,n)                   (NPUB_RING_BASE+NPUB_RING_REG_SIZE*(4*(bk)+(n))+0x70)


#define RSP_MSI0_MODE                    0x00760000
#define RSP_MSI1_MODE                    0x00760004

#define RSP_TIMER0_SET                   0x00760100
#define RSP_TIMER1_SET                   0x00760104

#define RSP_BANK0_SET                    0x00760300
#define RSP_BANK1_SET                    0x00760304

#define RSP_FORCE_TRG_PF                 0x00760400
#define RSP_FORCE_TRG_VF0                0x00760500
#define RSP_FORCE_TRG_VF1                0x00760504


#define BANK_VF_NUM_ADDR_OFFSET          0x400
#define BANK0_VF_NUM_ADDR                0x5000C0
#define BANK1_VF_NUM_ADDR                0x5004C0
#define BANK2_VF_NUM_ADDR                0x5008C0
#define BANK3_VF_NUM_ADDR                0x500CC0
#define BANK4_VF_NUM_ADDR                0x5010C0
#define BANK5_VF_NUM_ADDR                0x5014C0
#define BANK6_VF_NUM_ADDR                0x5018C0
#define BANK7_VF_NUM_ADDR                0x501CC0
#define BANK8_VF_NUM_ADDR                0x5020C0
#define BANK9_VF_NUM_ADDR                0x5024C0
#define BANK10_VF_NUM_ADDR               0x5028C0
#define BANK11_VF_NUM_ADDR               0x502CC0
#define BANK12_VF_NUM_ADDR               0x5030C0
#define BANK13_VF_NUM_ADDR               0x5034C0
#define BANK14_VF_NUM_ADDR               0x5038C0
#define BANK15_VF_NUM_ADDR               0x503CC0
#define BANK16_VF_NUM_ADDR               0x5040C0
#define BANK17_VF_NUM_ADDR               0x5044C0
#define BANK18_VF_NUM_ADDR               0x5048C0
#define BANK19_VF_NUM_ADDR               0x504CC0
#define BANK20_VF_NUM_ADDR               0x5050C0
#define BANK21_VF_NUM_ADDR               0x5054C0
#define BANK22_VF_NUM_ADDR               0x5058C0
#define BANK23_VF_NUM_ADDR               0x505CC0
#define BANK24_VF_NUM_ADDR               0x5060C0
#define BANK25_VF_NUM_ADDR               0x5064C0
#define BANK26_VF_NUM_ADDR               0x5068C0
#define BANK27_VF_NUM_ADDR               0x506CC0
#define BANK28_VF_NUM_ADDR               0x5070C0
#define BANK29_VF_NUM_ADDR               0x5074C0
#define BANK30_VF_NUM_ADDR               0x5078C0
#define BANK31_VF_NUM_ADDR               0x507CC0

#define RSP_MSI_CLEAR                    0x00624024
#define RSP_CMDQ_MSI_IRQ_TO_RC_DATA0     0x00624000
#define RSP_VF_MSI_CLEAR                 0x00760500

#define write_reg(reg, val)              iowrite32(val, reg)
#define read_reg(reg)                    ioread32(reg)

#define RSP_REG_READ(addr)              *(volatile uint32_t *)(addr)
#define RSP_REG_WRITE(addr,val)         *((volatile uint32_t *)(addr)) = val
#define RSP_REG_READQ(addr)             *(volatile uint64_t *)(addr)
#endif// _RSP_REG_H_