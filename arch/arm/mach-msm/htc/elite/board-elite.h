/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_ARM_MACH_MSM_BOARD_ELITE_H
#define __ARCH_ARM_MACH_MSM_BOARD_ELITE_H

#include <mach/irqs.h>
#include <mach/rpm-regulator.h>
#include <linux/mfd/pm8xxx/pm8921.h>
#include <mach/msm_memtypes.h>
#ifdef CONFIG_MSM_RTB
#include <mach/msm_rtb.h>
#endif
#ifdef CONFIG_MSM_CACHE_DUMP
#include <mach/msm_cache_dump.h>
#endif


/* Macros assume PMIC GPIOs and MPPs start at 1 */
#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)

#define ELITE_LAYOUTS		{ \
		{ { 0,  1, 0}, { 1,  0,  0}, {0, 0, -1} }, \
		{ { 0, -1, 0}, { 1,  0,  0}, {0, 0, -1} }, \
		{ { 1,  0, 0}, { 0, -1,  0}, {0, 0, -1} }, \
		{ {-1,  0, 0}, { 0,  0, -1}, {0, 1,  0} }  \
					}

extern struct regulator_init_data msm_saw_regulator_pdata_s5;
extern struct regulator_init_data msm_saw_regulator_pdata_s6;

extern struct rpm_regulator_platform_data elite_rpm_regulator_pdata __devinitdata;

extern struct platform_device msm8960_device_ext_5v_vreg __devinitdata;
extern struct platform_device msm8960_device_ext_l2_vreg __devinitdata;
extern struct platform_device msm8960_device_rpm_regulator __devinitdata;
extern struct pm8xxx_regulator_platform_data
	msm_pm8921_regulator_pdata[] __devinitdata;

extern int msm_pm8921_regulator_pdata_len __devinitdata;

#define GPIO_VREG_ID_EXT_5V		0
#define GPIO_VREG_ID_EXT_L2		1
#define GPIO_VREG_ID_EXT_3P3V           2

#define PMGPIO(x) (x)

#define ELITE_GPIO_LCD_TE                      (0)
#define ELITE_GPIO_NC_1                        (1)
#define ELITE_GPIO_NC_2                        (2)
#define ELITE_GPIO_HAPTIC_EN                   (3)
#define ELITE_GPIO_CAM_MCLK1                   (4)
#define ELITE_GPIO_CAM_MCLK0                   (5)
#define ELITE_GPIO_NFC_DL_MODE                 (6)
#define ELITE_GPIO_NC_6                        (6)
#define ELITE_GPIO_TP_ATTz                     (7)
#define ELITE_GPIO_TP_RSTz                     (8)
#define ELITE_GPIO_CAM_PWDN                    (9)
#define ELITE_GPIO_RAFT_UP_EN_CPU              (10)
#define ELITE_GPIO_NC_10                       (10)
#define ELITE_GPIO_RESOUTz_CONTROL             (11)
#define ELITE_GPIO_VP_I2C_DAT                  (12)
#define ELITE_GPIO_VP_I2C_CLK                  (13)
#define ELITE_GPIO_CHG_INT                     (14)
#define ELITE_GPIO_AUD_1WIRE_DO                (14)
#define ELITE_GPIO_NC_14                       (14)
#define ELITE_GPIO_COVER_DETz                  (15)
#define ELITE_GPIO_AUD_1WIRE_DI                (15)
#define ELITE_GPIO_NC_15                       (15)
#define ELITE_GPIO_TP_I2C_DAT                  (16)
#define ELITE_GPIO_TP_I2C_CLK                  (17)
#define ELITE_GPIO_V_HSMIC_2v85_EN             (18)
#define ELITE_GPIO_NC_18                       (18)
#define ELITE_GPIO_USB_ID1                     (19)
#define ELITE_GPIO_CAM_I2C_DAT                 (20)
#define ELITE_GPIO_CAM_I2C_CLK                 (21)
#define ELITE_GPIO_NC_22                       (22)
#define ELITE_GPIO_NC_23                       (23)
#define ELITE_GPIO_NFC_I2C_SDA                 (24)
#define ELITE_GPIO_NC_24                       (24)
#define ELITE_GPIO_NFC_I2C_SCL                 (25)
#define ELITE_GPIO_NC_25                       (25)
#define ELITE_GPIO_FM_SSBI                     (26)
#define ELITE_GPIO_FM_DATA                     (27)
#define ELITE_GPIO_BT_STROBE                   (28)
#define ELITE_GPIO_BT_DATA                     (29)
#define ELITE_GPIO_UIM1_DATA_MSM               (30)
#define ELITE_GPIO_UIM1_CLK_MSM                (31)
#define ELITE_GPIO_TORCH_FLASHz                (32)
#define ELITE_GPIO_DRIVER_EN                   (33)
#define ELITE_GPIO_DEBUG_UART_TX               (34)
#define ELITE_GPIO_DEBUG_UART_RX               (35)
#define ELITE_GPIO_MHL_I2C_DAT                 (36)
#define ELITE_GPIO_MC_I2C_DAT                  (36)
#define ELITE_GPIO_NC_36                       (36)
#define ELITE_GPIO_MHL_I2C_CLK                 (37)
#define ELITE_GPIO_MC_I2C_CLK                  (37)
#define ELITE_GPIO_NC_37                       (37)
#define ELITE_GPIO_MSM_SPI_DO                  (38)
#define ELITE_GPIO_NC_39                       (39)
#define ELITE_GPIO_MSM_SPI_CS0                 (40)
#define ELITE_GPIO_MSM_SPI_CLK                 (41)
#define ELITE_GPIO_VOL_UPz                     (42)
#define ELITE_GPIO_VOL_DOWNz                   (43)
#define ELITE_GPIO_SR_I2C_DAT                  (44)
#define ELITE_GPIO_SR_I2C_CLK                  (45)
#define ELITE_GPIO_PWR_KEYz                    (46)
#define ELITE_GPIO_MAIN_CAM_ID                 (47)
#define ELITE_GPIO_LCD_RSTz                    (48)
#define ELITE_GPIO_CAM_VCM_PD                  (49)
#define ELITE_GPIO_NFC_VEN                     (50)
#define ELITE_GPIO_NC_50                       (50)
#define ELITE_GPIO_RAW_RSTN                    (51)
#define ELITE_GPIO_RAW_INTR0                   (52)
#define ELITE_GPIO_SEC_CAM_ID                  (53)
#define ELITE_GPIO_NC_53                       (53)
#define ELITE_GPIO_REGION_ID                   (54)
#define ELITE_GPIO_NC_55                       (55)
#define ELITE_GPIO_NC_56                       (56)
#define ELITE_GPIO_V_3G_PA0_EN                 (57)
#define ELITE_GPIO_RAW_INTR1                   (58)
#define ELITE_GPIO_AUD_WCD_MCLK                (59)
#define ELITE_GPIO_AUD_WCD_SB_CLK              (60)
#define ELITE_GPIO_AUD_WCD_SB_DATA             (61)
#define ELITE_GPIO_AUD_WCD_INTR_OUT            (62)
#define ELITE_GPIO_NC_63                       (63)
#define ELITE_GPIO_NC_64                       (64)
#define ELITE_GPIO_NC_65                       (65)
#define ELITE_GPIO_NC_66                       (66)
#define ELITE_GPIO_MHL_USB_SELz_XB             (66)
#define ELITE_GPIO_GSENSOR_INT                 (67)
#define ELITE_GPIO_CAM2_RSTz                   (68)
#define ELITE_GPIO_GYRO_INT                    (69)
#define ELITE_GPIO_NC_69                       (69)
#define ELITE_GPIO_COMPASS_INT                 (70)
#define ELITE_GPIO_MCAM_SPI_DO                 (71)
#define ELITE_GPIO_MCAM_SPI_DI                 (72)
#define ELITE_GPIO_MCAM_SPI_CS0                (73)
#define ELITE_GPIO_MCAM_SPI_CLK                (74)
#define ELITE_GPIO_NC_75                       (75)
#define ELITE_GPIO_CAM2_STANDBY                (76)
#define ELITE_GPIO_OTG_EN                      (77)
#define ELITE_GPIO_NC_77                       (77)
#define ELITE_GPIO_MHL_INT                     (78)
#define ELITE_GPIO_NC_78                       (78)
#define ELITE_GPIO_V_RAW_1V8_EN                (79)
#define ELITE_GPIO_MHL_RSTz                    (80)
#define ELITE_GPIO_NC_80                       (80)
#define ELITE_GPIO_V_TP_3V3_EN                 (81)
#define ELITE_GPIO_MHL_USB_SELz                (82)
#define ELITE_GPIO_NC_82                       (82)
#define ELITE_GPIO_WCN_BT_SSBI                 (83)
#define ELITE_GPIO_WCN_CMD_DATA2               (84)
#define ELITE_GPIO_WCN_CMD_DATA1               (85)
#define ELITE_GPIO_WCN_CMD_DATA0               (86)
#define ELITE_GPIO_WCN_CMD_SET                 (87)
#define ELITE_GPIO_WCN_CMD_CLK                 (88)
#define ELITE_GPIO_MHL_USB_ENz                 (89)
#define ELITE_GPIO_NC_89                       (89)
#define ELITE_GPIO_AUD_A1028_WAKE              (90)
#define ELITE_GPIO_AUD_A1028_RSTz              (91)
#define ELITE_GPIO_AUD_A1028_INT               (92)
#define ELITE_GPIO_V_LCMIO_1V8_EN              (93)
#define ELITE_GPIO_MBAT_IN                     (94)
#define ELITE_GPIO_V_CAM_D1V2_EN               (95)
#define ELITE_GPIO_NC_95                       (95)
#define ELITE_GPIO_V_BOOST_5V_EN               (96)
#define ELITE_GPIO_NC_97                       (97)
#define ELITE_GPIO_RIVA_TX                     (98)
#define ELITE_GPIO_NC_99                       (99)
#define ELITE_GPIO_HDMI_DDC_CLK                (100)
#define ELITE_GPIO_NC_100                      (100)
#define ELITE_GPIO_HDMI_DDC_DATA               (101)
#define ELITE_GPIO_NC_101                      (101)
#define ELITE_GPIO_HDMI_HPD                    (102)
#define ELITE_GPIO_NC_102                      (102)
#define ELITE_GPIO_PM_SEC_INTz                 (103)
#define ELITE_GPIO_PM_USR_INTz                 (104)
#define ELITE_GPIO_PM_MDM_INTz                 (105)
#define ELITE_GPIO_NFC_IRQ                     (106)
#define ELITE_GPIO_NC_106                      (106)
#define ELITE_GPIO_NC_107                      (107)
#define ELITE_GPIO_PS_HOLD                     (108)
#define ELITE_GPIO_NC_109                      (109)
#define ELITE_GPIO_NC_110                      (110)
#define ELITE_GPIO_PRX_LB_SW_SEL               (111)
#define ELITE_GPIO_BOOT_COINIG_6               (112)
#define ELITE_GPIO_NC_103                      (113)
#define ELITE_GPIO_DRX_MODE_SELB               (114)
#define ELITE_GPIO_DRX_MODE_SELA               (115)
#define ELITE_GPIO_ANT_SW_SEL3                 (116)
#define ELITE_GPIO_ANT_SW_SEL2                 (117)
#define ELITE_GPIO_ANT_SW_SEL1                 (118)
#define ELITE_GPIO_ANT_SW_SEL0                 (119)
#define ELITE_GPIO_NC_120                      (120)
#define ELITE_GPIO_NC_121                      (121)
#define ELITE_GPIO_PA0_R0                      (122)
#define ELITE_GPIO_PA0_R1                      (123)
#define ELITE_GPIO_NC_124                      (124)
#define ELITE_GPIO_RTR0_RF_ON                  (125)
#define ELITE_GPIO_RTR_RX_ON                   (126)
#define ELITE_GPIO_TX_AGC_ADJ_CPU              (127)
#define ELITE_GPIO_PA_ON8_U900                 (128)
#define ELITE_GPIO_PA_ON7_U700                 (129)
#define ELITE_GPIO_PA_ON6_AWS                  (130)
#define ELITE_GPIO_NC_131                      (131)
#define ELITE_GPIO_PA_ON4_MODE                 (132)
#define ELITE_GPIO_NC_133                      (133)
#define ELITE_GPIO_NC_134                      (134)
#define ELITE_GPIO_PA_ON1_GSMHB                (135)
#define ELITE_GPIO_PA_ON0_GSMLB                (136)
#define ELITE_GPIO_EXT_GPS_LNA_EN              (137)
#define ELITE_GPIO_NC_138                      (138)
#define ELITE_GPIO_NC_139                      (139)
#define ELITE_GPIO_NC_140                      (140)
#define ELITE_GPIO_NC_141                      (141)
#define ELITE_GPIO_RTR0_SSBI2                  (142)
#define ELITE_GPIO_RTR0_SSBI1                  (143)
#define ELITE_GPIO_RTR0_GP_CLK                 (144)
#define ELITE_GPIO_RTR0_GPRSSYNC               (145)
#define ELITE_GPIO_RTR0_GPDATA2                (146)
#define ELITE_GPIO_RTR0_GPDATA1                (147)
#define ELITE_GPIO_RTR0_GPDATA0                (148)
#define ELITE_GPIO_NC_149                      (149)
#define ELITE_GPIO_NC_150                      (150)
#define ELITE_GPIO_NC_151                      (151)

#define ELITE_PMGPIO_NC_1                      PMGPIO(1)
#define ELITE_PMGPIO_AUD_CRADLE_EN             PMGPIO(1)
#define ELITE_PMGPIO_LED_3V3_EN                PMGPIO(1)
#define ELITE_PMGPIO_NC_2                      PMGPIO(2)
#define ELITE_PMGPIO_HAPTIC_3V3_EN             PMGPIO(2)
#define ELITE_PMGPIO_AUD_STEREO_REC            PMGPIO(3)
#define ELITE_PMGPIO_NC_4                      PMGPIO(4)
#define ELITE_PMGPIO_USB_ID_ADC_PMIC           PMGPIO(4)
#define ELITE_PMGPIO_PM_SPI_CS0                PMGPIO(5)
#define ELITE_PMGPIO_RAFT_uP_SPI_CS0           PMGPIO(6)
#define ELITE_PMGPIO_AUD_REMO_PRESz            PMGPIO(7)
#define ELITE_PMGPIO_NC_8                      PMGPIO(8)
#define ELITE_PMGPIO_AUD_1WIRE_DO              PMGPIO(9)
#define ELITE_PMGPIO_NC_9                      PMGPIO(9)
#define ELITE_PMGPIO_RAFT_UP_RSTz              PMGPIO(10)
#define ELITE_PMGPIO_AUD_1WIRE_O               PMGPIO(10)
#define ELITE_PMGPIO_NC_10                     PMGPIO(10)
#define ELITE_PMGPIO_PM_SPI_CLK                PMGPIO(11)
#define ELITE_PMGPIO_RAFT_uP_SPI_CLK           PMGPIO(12)
#define ELITE_PMGPIO_PM_SPI_DO                 PMGPIO(13)
#define ELITE_PMGPIO_RAFT_uP_SPI_DO            PMGPIO(14)
#define ELITE_PMGPIO_PM_RAFT_UP_EN_CPU         PMGPIO(15)
#define ELITE_PMGPIO_AUD_REMO_PRES             PMGPIO(15)
#define ELITE_PMGPIO_NC_15                     PMGPIO(15)
#define ELITE_PMGPIO_RAFT_UP_EN                PMGPIO(16)
#define ELITE_PMGPIO_AUD_1WIRE_DI              PMGPIO(16)
#define ELITE_PMGPIO_NC_16                     PMGPIO(16)
#define ELITE_PMGPIO_PROXIMITY_INTz            PMGPIO(17)
#define ELITE_PMGPIO_AUD_SPK_EN                PMGPIO(18)
#define ELITE_PMGPIO_AUD_HAC_SD                PMGPIO(19)
#define ELITE_PMGPIO_NC_19                     PMGPIO(19)
#define ELITE_PMGPIO_EARPHONE_DETz             PMGPIO(20)
#define ELITE_PMGPIO_CHG_STAT                  PMGPIO(21)
#define ELITE_PMGPIO_NC_21                     PMGPIO(21)
#define ELITE_PMGPIO_NC_22                     PMGPIO(22)
#define ELITE_PMGPIO_NC_23                     PMGPIO(23)
#define ELITE_PMGPIO_SDC3_CDz                  PMGPIO(23)
#define ELITE_PMGPIO_GREEN_LED                 PMGPIO(24)
#define ELITE_PMGPIO_NC_24                     PMGPIO(24)
#define ELITE_PMGPIO_AMBER_LED                 PMGPIO(25)
#define ELITE_PMGPIO_NC_25                     PMGPIO(25)
#define ELITE_PMGPIO_HAPTIC_PWM                PMGPIO(26)
#define ELITE_PMGPIO_UIM1_RST                  PMGPIO(27)
#define ELITE_PMGPIO_NC_28                     PMGPIO(28)
#define ELITE_PMGPIO_UIM1_CLK_MSM              PMGPIO(29)
#define ELITE_PMGPIO_UIM_CLK                   PMGPIO(30)
#define ELITE_PMGPIO_NC_31                     PMGPIO(31)
#define ELITE_PMGPIO_NC_32                     PMGPIO(32)
#define ELITE_PMGPIO_LCD_ID0                   PMGPIO(33)
#define ELITE_PMGPIO_AUD_CODEC_RSTz            PMGPIO(34)
#define ELITE_PMGPIO_LCD_ID1                   PMGPIO(35)
#define ELITE_PMGPIO_NC_36                     PMGPIO(36)
#define ELITE_PMGPIO_NC_37                     PMGPIO(37)
#define ELITE_PMGPIO_NC_38                     PMGPIO(38)
#define ELITE_PMGPIO_SSBI_PMIC_FWD_CLK         PMGPIO(39)
#define ELITE_PMGPIO_NC_40                     PMGPIO(40)
#define ELITE_PMGPIO_NC_41                     PMGPIO(41)
#define ELITE_PMGPIO_NC_42                     PMGPIO(42)
#define ELITE_PMGPIO_V_RAW_1V2_EN              PMGPIO(43)
#define ELITE_PMGPIO_NC_44                     PMGPIO(44)

void elite_lcd_id_power(int pull);

extern struct msm_camera_board_info elite_camera_board_info;

void __init elite_init_camera(void);
void elite_init_fb(void);
void __init elite_init_pmic(void);
void elite_init_mmc(void);
int __init elite_gpiomux_init(void);
void __init msm8960_allocate_fb_region(void);
void __init elite_pm8921_gpio_mpp_init(void);
void msm8960_mdp_writeback(struct memtype_reserve *reserve_table);
int __init elite_init_keypad(void);

#ifdef CONFIG_MSM_CACHE_DUMP
extern struct msm_cache_dump_platform_data msm8960_cache_dump_pdata;
#endif

#ifdef CONFIG_FB_MSM_HDMI_MSM_PANEL
int hdmi_enable_5v(int on);
void hdmi_hpd_feature(int on);
#endif

#endif
