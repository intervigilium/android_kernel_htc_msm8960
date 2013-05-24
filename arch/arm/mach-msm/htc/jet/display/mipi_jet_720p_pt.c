#include "../../../drivers/video/msm/msm_fb.h"
#include "../../../drivers/video/msm/mipi_dsi.h"
#include "mipi_jet.h"
#include <mach/panel_id.h>

static struct mipi_dsi_phy_ctrl dsi_fig_cmd_mode_phy_db = {

/* DSI_BIT_CLK at 482MHz, 2 lane, RGB888 */
  /* regulator */
  {0x03, 0x0a, 0x04, 0x00, 0x20},
  /* timing */
  /* clk_rate:482MHz */
  {0xb7, 0x28, 0x1f, 0x00, 0x22, 0x95, 0x22, 0x28, 0x22,
   0x03, 0x04, 0xa0},
  /* phy ctrl */
  {0x5f, 0x00, 0x00, 0x10},
  /* strength */
  {0xff, 0x00, 0x06, 0x00},
  /* pll control */
  {0x0, 0xf9, 0xb0, 0xda, 0x00, 0x50, 0x48, 0x63,
   0x41, 0x0f, 0x01,
   0x00, 0x14, 0x03, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01 },
};

static struct mipi_dsi_phy_ctrl nova_dsi_video_mode_phy_db = {
  /* DSI_BIT_CLK at 569MHz, 3 lane, RGB888 */
  /* regulator *//* off=0x0500 */
  {0x03, 0x08, 0x05, 0x00, 0x20},
  /* timing *//* off=0x0440 */
  {0x9B, 0x38, 0x18, 0x00, 0x4B, 0x51, 0x1C,
   0x3B, 0x29, 0x03, 0x04, 0xA0},
  /* phy ctrl *//* off=0x0470 */
  {0x5F, 0x00, 0x00, 0x10},
  /* strength *//* off=0x0480 */
  {0xFF, 0x00, 0x06, 0x00},
  /* pll control *//* off=0x0204 */
  {0x0, 0x38, 0x32, 0xDA, 0x00, 0x10, 0x0F, 0x61,
   0x41, 0x0F, 0x01,
   0x00, 0x1A, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x02 },
};

static struct msm_panel_info pinfo;

static int __init mipi_cmd_novatek_blue_qhd_pt_init(void)
{
  int ret;
  
  pinfo.xres = 540;
  pinfo.yres = 960;
  pinfo.type = MIPI_CMD_PANEL;
  pinfo.pdest = DISPLAY_1;
  pinfo.wait_cycle = 0;
  pinfo.bpp = 24;
  
  pinfo.lcdc.h_back_porch = 64;
  pinfo.lcdc.h_front_porch = 96;
  pinfo.lcdc.h_pulse_width = 32;
  pinfo.lcdc.v_back_porch = 16;
  pinfo.lcdc.v_front_porch = 16;
  pinfo.lcdc.v_pulse_width = 4;
  pinfo.lcd.v_back_porch = 16;
  pinfo.lcd.v_front_porch = 16;
  pinfo.lcd.v_pulse_width = 4;
  pinfo.lcdc.border_clr = 0;	/* blk */
  pinfo.lcdc.underflow_clr = 0xff;	/* blue */
  pinfo.lcdc.hsync_skew = 0;
  pinfo.bl_max = 255;
  pinfo.bl_min = 1;
  pinfo.fb_num = 2;
  pinfo.clk_rate = 482000000;
  pinfo.lcd.vsync_enable = TRUE;
  pinfo.lcd.hw_vsync_mode = TRUE;
  pinfo.lcd.refx100 = 6096; /* adjust refx100 to prevent tearing */
  
  pinfo.mipi.mode = DSI_CMD_MODE;
  pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
  pinfo.mipi.vc = 0;
  pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
  pinfo.mipi.data_lane0 = TRUE;
  pinfo.mipi.data_lane1 = TRUE;
  pinfo.mipi.t_clk_post = 0x0a;
  pinfo.mipi.t_clk_pre = 0x20;
  pinfo.mipi.stream = 0;	/* dma_p */
  pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;
  pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
  pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
  pinfo.mipi.interleave_max = 1;
  pinfo.mipi.insert_dcs_cmd = TRUE;
  pinfo.mipi.wr_mem_continue = 0x3c;
  pinfo.mipi.wr_mem_start = 0x2c;
  pinfo.mipi.dsi_phy_db = &dsi_fig_cmd_mode_phy_db;
#ifdef CONFIG_FB_MSM_SELF_REFRESH
  jet_panel_data.self_refresh_switch = NULL;
#endif
  ret = mipi_jet_device_register(&pinfo, MIPI_DSI_PRIM,
				 MIPI_DSI_PANEL_WVGA_PT);
  if (ret)
    printk(KERN_ERR "%s: failed to register device!\n", __func__);
  
  return ret;
}

static int __init mipi_video_auo_hd720p_init(void)
{
  int ret;
#ifdef JEL_CMD_MODE_PANEL
  printk(KERN_INFO "%s: CMD mode (AL)\n", __func__);
  pinfo.type = MIPI_CMD_PANEL;
  pinfo.mipi.mode = DSI_CMD_MODE;
  pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
  /*pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;*/
  pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
#ifdef CONFIG_FB_MSM_SELF_REFRESH
  jet_panel_data.self_refresh_switch = NULL; /* CMD or VIDEO mode only */
#endif
  pinfo.lcd.vsync_enable = TRUE;
  pinfo.lcd.hw_vsync_mode = TRUE;
  pinfo.lcd.refx100 = 6096; /* adjust refx100 to prevent tearing */
  pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
  pinfo.mipi.interleave_max = 1;
  pinfo.mipi.insert_dcs_cmd = TRUE;
  pinfo.mipi.wr_mem_continue = 0x3c;
  pinfo.mipi.wr_mem_start = 0x2c;
#else
  pinfo.type = MIPI_VIDEO_PANEL;/*MIPI_VIDEO_PANEL;*/
  pinfo.mipi.mode = DSI_VIDEO_MODE;
  pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
  pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
#ifdef CONFIG_FB_MSM_SELF_REFRESH
  printk(KERN_INFO "%s: VIDEO mode (AL)\n", __func__);
  jet_panel_data.self_refresh_switch = NULL; /* CMD or VIDEO mode only */
#else
  printk(KERN_INFO "%s: SWITCH mode (AL)\n", __func__);
#endif
  pinfo.mipi.pulse_mode_hsa_he = TRUE;
  pinfo.mipi.hfp_power_stop = TRUE;
  pinfo.mipi.hbp_power_stop = TRUE;
  pinfo.mipi.hsa_power_stop = TRUE;
  pinfo.mipi.eof_bllp_power_stop = TRUE;
  pinfo.mipi.bllp_power_stop = TRUE;
  pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_PULSE;
#endif
  
  pinfo.xres = 720;
  pinfo.yres = 1280;
  
  pinfo.pdest = DISPLAY_1;
  pinfo.wait_cycle = 0;
  pinfo.bpp = 24;
  
  pinfo.lcdc.h_back_porch = 104;	/* 660Mhz: 116 */
  pinfo.lcdc.h_front_porch = 95;	/* 660Mhz: 184 */
  pinfo.lcdc.h_pulse_width = 1;	/* 660Mhz: 24 */
  pinfo.lcdc.v_back_porch = 2;
  pinfo.lcdc.v_front_porch = 6;
  pinfo.lcdc.v_pulse_width = 1;
  
  pinfo.lcd.v_back_porch = 2;
  pinfo.lcd.v_front_porch = 6;
  pinfo.lcd.v_pulse_width = 1;
  
  pinfo.lcdc.border_clr = 0;	/* blk */
  pinfo.lcdc.underflow_clr = 0xff;	/* blue */
  pinfo.lcdc.hsync_skew = 0;
  pinfo.bl_max = 255;
  pinfo.bl_min = 1;
  pinfo.fb_num = 2;
  /*pinfo.clk_rate = 742500000;*/
  /*pinfo.clk_rate = 482000000;*/
  pinfo.clk_rate = 569000000;
  
  pinfo.mipi.vc = 0;
  pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
  pinfo.mipi.data_lane0 = TRUE;
  pinfo.mipi.data_lane1 = TRUE;
  pinfo.mipi.data_lane2 = TRUE;
  pinfo.mipi.tx_eot_append = TRUE;
  pinfo.mipi.t_clk_post = 0x10;	/* 660Mhz: 10 */
  pinfo.mipi.t_clk_pre = 0x21;	/* 660Mhz: 30 */
  pinfo.mipi.stream = 0;	/* dma_p */
  pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
  pinfo.mipi.frame_rate = 63;
  pinfo.mipi.dsi_phy_db = &nova_dsi_video_mode_phy_db;
  
  ret = mipi_jet_device_register(&pinfo, MIPI_DSI_PRIM,
				 MIPI_DSI_PANEL_WVGA_PT);
  if (ret)
    printk(KERN_ERR "%s: failed to register device!\n", __func__);
  
  return ret;
}

static int __init mipi_video_sony_hd720p_init(void)
{
  int ret;
  
  /* 1:VIDEO MODE, 0:CMD MODE */
#ifdef JEL_CMD_MODE_PANEL
  printk(KERN_INFO "%s: CMD mode (AL)\n", __func__);
  pinfo.type = MIPI_CMD_PANEL;
  pinfo.mipi.mode = DSI_CMD_MODE;
  pinfo.mipi.dst_format = DSI_CMD_DST_FORMAT_RGB888;
  /*pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_NONE;*/
  pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
#ifdef CONFIG_FB_MSM_SELF_REFRESH
  jet_panel_data.self_refresh_switch = NULL; /* CMD or VIDEO mode only */
#endif
  pinfo.lcd.vsync_enable = TRUE;
  pinfo.lcd.hw_vsync_mode = TRUE;
  pinfo.lcd.refx100 = 5700; /* adjust refx100 to prevent tearing */
  pinfo.mipi.te_sel = 1; /* TE from vsycn gpio */
  pinfo.mipi.interleave_max = 1;
  pinfo.mipi.insert_dcs_cmd = TRUE;
  pinfo.mipi.wr_mem_continue = 0x3c;
  pinfo.mipi.wr_mem_start = 0x2c;
  
#else
  pinfo.type = MIPI_VIDEO_PANEL;
  pinfo.mipi.mode = DSI_VIDEO_MODE;
  pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
  pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
#ifdef CONFIG_FB_MSM_SELF_REFRESH
  printk(KERN_INFO "%s: VIDEO mode (AL)\n", __func__);
  jet_panel_data.self_refresh_switch = NULL; /* CMD or VIDEO mode only */
#else
  printk(KERN_INFO "%s: SWITCH mode (AL)\n", __func__);
#endif
  
  pinfo.mipi.pulse_mode_hsa_he = TRUE;
  pinfo.mipi.hfp_power_stop = TRUE;
  pinfo.mipi.hbp_power_stop = TRUE;
  pinfo.mipi.hsa_power_stop = TRUE;
  pinfo.mipi.eof_bllp_power_stop = TRUE;
  pinfo.mipi.bllp_power_stop = TRUE;
  pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_PULSE;
  
#endif
  
  pinfo.xres = 720;
  pinfo.yres = 1280;
  
  pinfo.pdest = DISPLAY_1;
  pinfo.wait_cycle = 0;
  pinfo.bpp = 24;
  
  pinfo.lcdc.h_back_porch = 104;
  pinfo.lcdc.h_front_porch = 95;
  pinfo.lcdc.h_pulse_width = 1;
  pinfo.lcdc.v_back_porch = 2;
  pinfo.lcdc.v_front_porch = 6;
  pinfo.lcdc.v_pulse_width = 1;
  
  pinfo.lcd.v_back_porch = 2;
  pinfo.lcd.v_front_porch = 6;
  pinfo.lcd.v_pulse_width = 1;
  
  pinfo.lcdc.border_clr = 0;	/* blk */
  pinfo.lcdc.underflow_clr = 0xff;	/* blue */
  pinfo.lcdc.hsync_skew = 0;
  pinfo.bl_max = 255;
  pinfo.bl_min = 1;
  pinfo.fb_num = 2;
  pinfo.clk_rate = 569000000;
  
  pinfo.mipi.vc = 0;
  pinfo.mipi.rgb_swap = DSI_RGB_SWAP_RGB;
  pinfo.mipi.data_lane0 = TRUE;
  pinfo.mipi.data_lane1 = TRUE;
  pinfo.mipi.data_lane2 = TRUE;
  pinfo.mipi.tx_eot_append = TRUE;
  pinfo.mipi.t_clk_post = 0x10;
  pinfo.mipi.t_clk_pre = 0x21;
  pinfo.mipi.stream = 0;	/* dma_p */
  pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
  pinfo.mipi.frame_rate = 59;
  pinfo.mipi.dsi_phy_db = &nova_dsi_video_mode_phy_db;
  
  ret = mipi_jet_device_register(&pinfo, MIPI_DSI_PRIM,
				 MIPI_DSI_PANEL_WVGA_PT);
  if (ret)
    printk(KERN_ERR "%s: failed to register device!\n", __func__);
  
  return ret;
}

static int __init mipi_jet_panel_init(void)
{
  int rc;

  if (panel_type == PANEL_ID_NONE)
    {
      printk(KERN_INFO "No panel detected.\n");
      return -EINVAL;
    }
  if (panel_type == PANEL_ID_JET_SONY_NT || panel_type == PANEL_ID_JET_SONY_NT_C1 ||
      panel_type == PANEL_ID_JET_SONY_NT_C2)
    rc = mipi_video_sony_hd720p_init();
  else if (panel_type == PANEL_ID_JET_AUO_NT || panel_type ==
	   PANEL_ID_JET_AUO_NT_C2 || panel_type == PANEL_ID_JET_AUO_NT_C3 ||
	   panel_type == PANEL_ID_JET_AUO_NT_C3_1)
    rc = mipi_video_auo_hd720p_init();
  else
    rc = mipi_cmd_novatek_blue_qhd_pt_init();

  return rc;
}

late_initcall(mipi_jet_panel_init);
