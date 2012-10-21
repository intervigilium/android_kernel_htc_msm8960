/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/fb.h>
#include <asm/system.h>
#include <asm/mach-types.h>
#include <mach/hardware.h>

#include "mdp.h"
#include "msm_fb.h"
#include "mdp4.h"
#include <mach/debug_display.h>

#define DTV_BASE	0xD0000

extern int mdp4_check_dtv_pipe(void);

/*#define DEBUG*/
#ifdef DEBUG
static void __mdp_outp(uint32 port, uint32 value)
{
	uint32 in_val;

	outpdw(port, value);
	in_val = inpdw(port);
	printk(KERN_INFO "MDP-DTV[%04x] => %08x [%08x]\n",
		port-(uint32)(MDP_BASE + DTV_BASE), value, in_val);
}

#undef MDP_OUTP
#define MDP_OUTP(port, value)	__mdp_outp((uint32)(port), (value))
#endif

static int first_pixel_start_x;
static int first_pixel_start_y;

static struct mdp4_overlay_pipe *dtv_pipe;
static DECLARE_COMPLETION(dtv_comp);

static int mdp4_dtv_start(struct msm_fb_data_type *mfd)
{
	int dtv_width;
	int dtv_height;
	int dtv_bpp;
	int dtv_border_clr;
	int dtv_underflow_clr;
	int dtv_hsync_skew;

	int hsync_period;
	int hsync_ctrl;
	int vsync_period;
	int display_hctl;
	int display_v_start;
	int display_v_end;
	int active_hctl;
	int active_h_start;
	int active_h_end;
	int active_v_start;
	int active_v_end;
	int ctrl_polarity;
	int h_back_porch;
	int h_front_porch;
	int v_back_porch;
	int v_front_porch;
	int hsync_pulse_width;
	int vsync_pulse_width;
	int hsync_polarity;
	int vsync_polarity;
	int data_en_polarity;
	int hsync_start_x;
	int hsync_end_x;
	struct fb_info *fbi;
	struct fb_var_screeninfo *var;

	if (!mfd)
		return -ENODEV;

	if (mfd->key != MFD_KEY)
		return -EINVAL;

	if (dtv_pipe == NULL)
		return -EINVAL;

	fbi = mfd->fbi;
	var = &fbi->var;

	mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_ON, FALSE);
#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
	if (is_mdp4_hw_reset()) {
		mdp4_hw_init();
		outpdw(MDP_BASE + 0x0038, mdp4_display_intf);
	}
#endif
	mdp4_overlay_dmae_cfg(mfd, 0);

	/*
	 * DTV timing setting
	 */
	h_back_porch = var->left_margin;
	h_front_porch = var->right_margin;
	v_back_porch = var->upper_margin;
	v_front_porch = var->lower_margin;
	hsync_pulse_width = var->hsync_len;
	vsync_pulse_width = var->vsync_len;
	dtv_border_clr = mfd->panel_info.lcdc.border_clr;
	dtv_underflow_clr = mfd->panel_info.lcdc.underflow_clr;
	dtv_hsync_skew = mfd->panel_info.lcdc.hsync_skew;

	pr_info("%s: <ID=%d %dx%d (%d,%d,%d), (%d,%d,%d) %dMHz>\n", __func__,
		var->reserved[3], var->xres, var->yres,
		var->right_margin, var->hsync_len, var->left_margin,
		var->lower_margin, var->vsync_len, var->upper_margin,
		var->pixclock/1000/1000);

	dtv_width = var->xres;
	dtv_height = var->yres;
	dtv_bpp = mfd->panel_info.bpp;

	hsync_period =
	    hsync_pulse_width + h_back_porch + dtv_width + h_front_porch;
	hsync_ctrl = (hsync_period << 16) | hsync_pulse_width;
	hsync_start_x = hsync_pulse_width + h_back_porch;
	hsync_end_x = hsync_period - h_front_porch - 1;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;

	vsync_period =
	    (vsync_pulse_width + v_back_porch + dtv_height +
	     v_front_porch) * hsync_period;
	display_v_start =
	    (vsync_pulse_width + v_back_porch) * hsync_period + dtv_hsync_skew;
	display_v_end =
	    vsync_period - (v_front_porch * hsync_period) + dtv_hsync_skew - 1;

	if (dtv_width != var->xres) {
		active_h_start = hsync_start_x + first_pixel_start_x;
		active_h_end = active_h_start + var->xres - 1;
		active_hctl =
		    ACTIVE_START_X_EN | (active_h_end << 16) | active_h_start;
	} else {
		active_hctl = 0;
	}

	if (dtv_height != var->yres) {
		active_v_start =
		    display_v_start + first_pixel_start_y * hsync_period;
		active_v_end = active_v_start + (var->yres) * hsync_period - 1;
		active_v_start |= ACTIVE_START_Y_EN;
	} else {
		active_v_start = 0;
		active_v_end = 0;
	}

	dtv_underflow_clr |= 0x80000000;	/* enable recovery */
	hsync_polarity = fbi->var.yres >= 720 ? 0 : 1;
	vsync_polarity = fbi->var.yres >= 720 ? 0 : 1;
	data_en_polarity = 0;

	ctrl_polarity =
	    (data_en_polarity << 2) | (vsync_polarity << 1) | (hsync_polarity);


	MDP_OUTP(MDP_BASE + DTV_BASE + 0x4, hsync_ctrl);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x8, vsync_period);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0xc, vsync_pulse_width * hsync_period);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x18, display_hctl);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x1c, display_v_start);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x20, display_v_end);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x40, dtv_border_clr);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x44, dtv_underflow_clr);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x48, dtv_hsync_skew);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x50, ctrl_polarity);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x2c, active_hctl);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x30, active_v_start);
	MDP_OUTP(MDP_BASE + DTV_BASE + 0x38, active_v_end);

	/* Test pattern 8 x 8 pixel */
	/* MDP_OUTP(MDP_BASE + DTV_BASE + 0x4C, 0x80000808); */

	/*MHL v1.1 compliance test, Test3.2.3.2 Test3.2.3.4*/
	if((var->yres == 480) || (var->yres == 576)){
		if(var->xres == 640 && var->yres == 480)
				mdp4_set_limit_range(false);
		else
				mdp4_set_limit_range(true);
	}
	else
		mdp4_set_limit_range(false);

	/* enable DTV block */
	MDP_OUTP(MDP_BASE + DTV_BASE, 1);
	mdp_pipe_ctrl(MDP_OVERLAY1_BLOCK, MDP_BLOCK_POWER_ON, FALSE);
	/* MDP cmd block disable */
	mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_OFF, FALSE);

	return 0;
}

static int mdp4_dtv_stop(struct msm_fb_data_type *mfd)
{
	if (dtv_pipe == NULL)
		return -EINVAL;

	/* MDP cmd block enable */
	mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_ON, FALSE);
	MDP_OUTP(MDP_BASE + DTV_BASE, 0);
	/* MDP cmd block disable */
	mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_OFF, FALSE);
	mdp_pipe_ctrl(MDP_OVERLAY1_BLOCK, MDP_BLOCK_POWER_OFF, FALSE);

	return 0;
}

int mdp4_dtv_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	int ret = 0;

	mfd = (struct msm_fb_data_type *)platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;

	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mdp4_overlay_panel_mode(MDP4_MIXER1, MDP4_PANEL_DTV);
	if (dtv_pipe != NULL)
		ret = mdp4_dtv_start(mfd);

	ret = panel_next_on(pdev);
	if (ret != 0)
		dev_warn(&pdev->dev, "mdp4_overlay_dtv: panel_next_on failed");

	dev_info(&pdev->dev, "mdp4_overlay_dtv: on");

	return ret;
}

int mdp4_dtv_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	int ret = 0;
	unsigned int flags;

	mfd = (struct msm_fb_data_type *)platform_get_drvdata(pdev);

	if (mdp4_check_dtv_pipe()) {
		PR_DISP_DEBUG("%s dtv still enable\n", __func__);
		return -1;
	}

	if (dtv_pipe != NULL) {
		mdp4_mixer_stage_down(dtv_pipe);
		flags = dtv_pipe->flags;
		dtv_pipe->flags &= ~MDP_OV_PLAY_NOWAIT;
		mdp4_overlay_dtv_ov_done_push(mfd, dtv_pipe);
		dtv_pipe->flags = flags;
		mdp4_dtv_stop(mfd);
		mdp4_overlay_pipe_free(dtv_pipe);
		mdp4_iommu_unmap(dtv_pipe);
		dtv_pipe = NULL;
	}
	mdp4_overlay_panel_mode_unset(MDP4_MIXER1, MDP4_PANEL_DTV);

	ret = panel_next_off(pdev);
	mdp4_iommu_detach();
	mdp_footswitch_ctrl(FALSE);

	dev_info(&pdev->dev, "mdp4_overlay_dtv: off");
	return ret;
}

void mdp4_overlay_dtv_vsync_push(struct msm_fb_data_type *mfd,
			struct mdp4_overlay_pipe *pipe)
{
	mdp4_overlay_reg_flush(pipe, 1);
	if (pipe->flags & MDP_OV_PLAY_NOWAIT)
		return;

	mdp4_overlay_dtv_wait4vsync();

	/* change mdp clk while mdp is idle` */
	mdp4_set_perf_level();
}

static void mdp4_overlay_dtv_alloc_pipe(struct msm_fb_data_type *mfd,
		int32 ptype)
{
	int ret = 0;
	struct fb_info *fbi = mfd->fbi;
	struct mdp4_overlay_pipe *pipe;

	if (dtv_pipe != NULL)
		return;

	pr_debug("%s: ptype=%d\n", __func__, ptype);

	pipe = mdp4_overlay_pipe_alloc(ptype, MDP4_MIXER1);
	if (pipe == NULL) {
		pr_err("%s: pipe_alloc failed\n", __func__);
		return;
	}
	pipe->pipe_used++;
	pipe->mixer_stage = MDP4_MIXER_STAGE_BASE;
	pipe->mixer_num = MDP4_MIXER1;

	if (ptype == OVERLAY_TYPE_BF) {
		mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_ON, FALSE);
		/* LSP_BORDER_COLOR */
		MDP_OUTP(MDP_BASE + MDP4_OVERLAYPROC1_BASE + 0x5004,
			((0x0 & 0xFFF) << 16) |	/* 12-bit B */
			(0x0 & 0xFFF));		/* 12-bit G */
		/* MSP_BORDER_COLOR */
		MDP_OUTP(MDP_BASE + MDP4_OVERLAYPROC1_BASE + 0x5008,
			(0x0 & 0xFFF));		/* 12-bit R */
		mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_OFF, FALSE);
	} else {
		switch (mfd->ibuf.bpp) {
		case 2:
			pipe->src_format = MDP_RGB_565;
			break;
		case 3:
			pipe->src_format = MDP_RGB_888;
			break;
		case 4:
		default:
#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
			pipe->src_format = MSMFB_DEFAULT_TYPE;
#else
			pipe->src_format = MDP_ARGB_8888;
#endif
			break;
		}
	}

	pipe->src_height = fbi->var.yres;
	pipe->src_width = fbi->var.xres;
	pipe->src_h = fbi->var.yres;
	pipe->src_w = fbi->var.xres;
	pipe->src_y = 0;
	pipe->src_x = 0;
	pipe->srcp0_ystride = fbi->fix.line_length;

	ret = mdp4_overlay_format2pipe(pipe);
	if (ret < 0)
		pr_warn("%s: format2type failed\n", __func__);

	mdp4_overlay_dmae_xy(pipe);	/* dma_e */
	mdp4_overlayproc_cfg(pipe);

	if (pipe->pipe_type == OVERLAY_TYPE_RGB) {
		pipe->srcp0_addr = (uint32) mfd->ibuf.buf;
		mdp4_overlay_rgb_setup(pipe);
	}

	mdp4_mixer_stage_up(pipe);

	dtv_pipe = pipe; /* keep it */
}

int mdp4_overlay_dtv_set(struct msm_fb_data_type *mfd,
			struct mdp4_overlay_pipe *pipe)
{
	if (dtv_pipe != NULL)
		return 0;

	if (pipe != NULL && pipe->mixer_stage == MDP4_MIXER_STAGE_BASE &&
			pipe->pipe_type == OVERLAY_TYPE_RGB)
		dtv_pipe = pipe; /* keep it */
	else if (mdp4_overlay_borderfill_supported())
		mdp4_overlay_dtv_alloc_pipe(mfd, OVERLAY_TYPE_BF);
	else
		mdp4_overlay_dtv_alloc_pipe(mfd, OVERLAY_TYPE_RGB);
	if (dtv_pipe == NULL)
		return -ENODEV;

	mdp4_init_writeback_buf(mfd, MDP4_MIXER1);
	dtv_pipe->blt_addr = 0;

	return mdp4_dtv_start(mfd);
}

int mdp4_overlay_dtv_unset(struct msm_fb_data_type *mfd,
			struct mdp4_overlay_pipe *pipe)
{
	unsigned int flags;
	int result = 0;

	flags = pipe->flags;
	pipe->flags &= ~MDP_OV_PLAY_NOWAIT;
	mdp4_overlay_dtv_ov_done_push(mfd, pipe);
	pipe->flags = flags;

	if (pipe->mixer_stage == MDP4_MIXER_STAGE_BASE &&
			pipe->pipe_type == OVERLAY_TYPE_RGB) {
		result = mdp4_dtv_stop(mfd);
		dtv_pipe = NULL;
	}
	return result;
}

static void mdp4_overlay_dtv_ov_start(struct msm_fb_data_type *mfd)
{
	unsigned long flag;

	/* enable irq */
	if (mfd->ov_start)
		return;

	spin_lock_irqsave(&mdp_spin_lock, flag);
	mdp_enable_irq(MDP_OVERLAY1_TERM);
	INIT_COMPLETION(dtv_pipe->comp);
	mfd->dma->waiting = TRUE;
	outp32(MDP_INTR_CLEAR, INTR_OVERLAY1_DONE);
	mdp_intr_mask |= INTR_OVERLAY1_DONE;
	outp32(MDP_INTR_ENABLE, mdp_intr_mask);
	spin_unlock_irqrestore(&mdp_spin_lock, flag);
	mfd->ov_start = true;
}

static void mdp4_overlay_dtv_wait4dmae(struct msm_fb_data_type *mfd)
{
	unsigned long flag;

	if (!dtv_pipe) {
		pr_debug("%s: no mixer1 base layer pipe allocated!\n",
			 __func__);
		return;
	}
	/* enable irq */
	spin_lock_irqsave(&mdp_spin_lock, flag);
	mdp_enable_irq(MDP_DMA_E_TERM);
	INIT_COMPLETION(dtv_pipe->comp);
	mfd->dma->waiting = TRUE;
	outp32(MDP_INTR_CLEAR, INTR_DMA_E_DONE);
	mdp_intr_mask |= INTR_DMA_E_DONE;
	outp32(MDP_INTR_ENABLE, mdp_intr_mask);
	spin_unlock_irqrestore(&mdp_spin_lock, flag);
	wait_for_completion_killable(&dtv_pipe->comp);
	mdp_disable_irq(MDP_DMA_E_TERM);
}

static void mdp4_overlay_dtv_wait4_ov_done(struct msm_fb_data_type *mfd,
	struct mdp4_overlay_pipe *pipe)
{
	u32 data = inpdw(MDP_BASE + DTV_BASE);

	if (mfd->ov_start)
		mfd->ov_start = false;
	else
		return;
	if (!(data & 0x1) || (pipe == NULL))
		return;

	if(mfd->fbi->var.yres == 1080)
		wait_for_completion_timeout(&dtv_pipe->comp,
			msecs_to_jiffies(VSYNC_PERIOD_1080P24*2));
	else
		wait_for_completion_timeout(&dtv_pipe->comp,
			msecs_to_jiffies(VSYNC_PERIOD*2));
	mdp_disable_irq(MDP_OVERLAY1_TERM);

	if (dtv_pipe->blt_addr)
		mdp4_overlay_dtv_wait4dmae(mfd);
}

void mdp4_overlay_dtv_ov_done_push(struct msm_fb_data_type *mfd,
			struct mdp4_overlay_pipe *pipe)
{
	mdp4_overlay_reg_flush(pipe, 1);
	mdp4_overlay_dtv_ov_start(mfd);

	if (pipe->flags & MDP_OV_PLAY_NOWAIT)
		return;

	mdp4_overlay_dtv_wait4_ov_done(mfd, pipe);

	/* change mdp clk while mdp is idle` */
	mdp4_set_perf_level();
}

void mdp4_overlay_dtv_wait_for_ov(struct msm_fb_data_type *mfd,
	struct mdp4_overlay_pipe *pipe)
{
	mdp4_overlay_dtv_wait4_ov_done(mfd, pipe);
	mdp4_set_perf_level();
}

void mdp4_external_vsync_dtv()
{
	complete_all(&dtv_comp);
}

/*
 * mdp4_overlay1_done_dtv: called from isr
 */
void mdp4_overlay1_done_dtv()
{
	complete_all(&dtv_pipe->comp);
}

#ifdef CONFIG_FB_MSM_HDMI_AS_PRIMARY
void mdp4_dtv_set_black_screen(void)
{
	char *rgb_base;
	/*Black color*/
	uint32 color = 0x00000000;
	uint32 temp_src_format;

	if (!dtv_pipe) {
		pr_err("dtv_pipe is not configured yet\n");
		return;
	}
	rgb_base = MDP_BASE + MDP4_RGB_BASE;
	rgb_base += (MDP4_RGB_OFF * dtv_pipe->pipe_num);

	mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_ON, FALSE);
	/*
	* RGB Constant Color
	*/
	MDP_OUTP(rgb_base + 0x1008, color);
	/*
	* MDP_RGB_SRC_FORMAT
	*/
	temp_src_format = inpdw(rgb_base + 0x0050);
	MDP_OUTP(rgb_base + 0x0050, temp_src_format | BIT(22));
	mdp4_overlay_reg_flush(dtv_pipe, 1);
	mdp_pipe_ctrl(MDP_CMD_BLOCK, MDP_BLOCK_POWER_OFF, FALSE);
}
#endif

void mdp4_overlay_dtv_wait4vsync(void)
{
	unsigned long flag;

	/* enable irq */
	spin_lock_irqsave(&mdp_spin_lock, flag);
	mdp_enable_irq(MDP_DMA_E_TERM);
	INIT_COMPLETION(dtv_comp);
	outp32(MDP_INTR_CLEAR, INTR_EXTERNAL_VSYNC);
	mdp_intr_mask |= INTR_EXTERNAL_VSYNC;
	outp32(MDP_INTR_ENABLE, mdp_intr_mask);
	spin_unlock_irqrestore(&mdp_spin_lock, flag);
	wait_for_completion_killable(&dtv_comp);
	mdp_disable_irq(MDP_DMA_E_TERM);
}

void mdp4_dtv_overlay(struct msm_fb_data_type *mfd)
{
	struct mdp4_overlay_pipe *pipe;

	if (!mfd->panel_power_on)
		return;

	mutex_lock(&mfd->dma->ov_mutex);
	if (dtv_pipe == NULL) {
		if (mdp4_overlay_dtv_set(mfd, NULL)) {
			pr_warn("%s: dtv_pipe == NULL\n", __func__);
			mutex_unlock(&mfd->dma->ov_mutex);
			return;
		}
	}

	pipe = dtv_pipe;
	if (pipe->pipe_type == OVERLAY_TYPE_RGB) {
		pipe->srcp0_addr = (uint32) mfd->ibuf.buf;
		mdp4_overlay_rgb_setup(pipe);
	}
	mdp4_overlay_dtv_ov_done_push(mfd, pipe);
	mdp4_iommu_unmap(pipe);
	mutex_unlock(&mfd->dma->ov_mutex);
}
