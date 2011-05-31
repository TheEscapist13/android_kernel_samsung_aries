/*
 * wm8994.c  --  WM8994 ALSA Soc Audio driver
 *
 * Copyright 2010 Wolfson Microelectronics PLC.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 * Notes:
 *  The WM8994 is a multichannel codec with S/PDIF support, featuring six
 *  DAC channels and two ADC channels.
 *
 *  Currently only the primary audio interface is supported - S/PDIF and
 *  the secondary audio interfaces are not.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
<<<<<<< HEAD
=======
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/jack.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
<<<<<<< HEAD
#include <sound/soc-dapm.h>
#include <sound/tlv.h>
#include <sound/initval.h>
#include <asm/div64.h>
#include <asm/io.h>
#include <plat/map-base.h>
#include <mach/regs-clock.h> 
#include "wm8994.h"
=======
#include <sound/initval.h>
#include <sound/tlv.h>
#include <trace/events/asoc.h>
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

#define WM8994_VERSION "0.1"
#define SUBJECT "wm8994.c"

#if defined(CONFIG_VIDEO_TV20) && defined(CONFIG_SND_S5P_WM8994_MASTER) 
#define HDMI_USE_AUDIO
#endif

<<<<<<< HEAD

//------------------------------------------------
// Definitions of clock related.
//------------------------------------------------
static struct {
	int ratio;
	int clk_sys_rate;
} clk_sys_rates[] = {
	{ 64,   0 },
	{ 128,  1 },
	{ 192,  2 },
	{ 256,  3 },
	{ 384,  4 },
	{ 512,  5 },
	{ 768,  6 },
	{ 1024, 7 },
	{ 1408, 8 },
	{ 1536, 9 },
};

static struct {
	int rate;
	int sample_rate;
} sample_rates[] = {
	{ 8000,  0  },
	{ 11025, 1  },
	{ 12000, 2  },
	{ 16000, 3  },
	{ 22050, 4  },
	{ 24000, 5  },
	{ 32000, 6  },
	{ 44100, 7  },
	{ 48000, 8  },
	{ 88200, 9  },
	{ 96000, 10  },
=======
#define WM8994_NUM_DRC 3
#define WM8994_NUM_EQ  3

static int wm8994_drc_base[] = {
	WM8994_AIF1_DRC1_1,
	WM8994_AIF1_DRC2_1,
	WM8994_AIF2_DRC_1,
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
};

static struct {
	int div; /* *10 due to .5s */
	int bclk_div;
} bclk_divs[] = {
#if 0
	{ 10,  0  },
	{ 15,  1  },
	{ 20,  2  },
	{ 30,  3  },
	{ 40,  4  },
	{ 55,  5  },
	{ 60,  6  },
	{ 80,  7  },
	{ 110, 8  },
	{ 120, 9  },
	{ 160, 10 },
	{ 220, 11 },
	{ 240, 12 },
	{ 320, 13 },
	{ 440, 14 },
	{ 480, 15 },
#endif
	{ 1,   0  },
	{ 2,   1  },
	{ 4,   2  },
	{ 6,   3  },
	{ 8,   4  },
	{ 12,  5  },
	{ 16,  6  },
	{ 24,  7  },
	{ 32,  8  },
	{ 48,  9  },
};

<<<<<<< HEAD
extern flush_fifo(void);//I2S FIFO-flush function . Implemented as to match TN requirement to remove ringtone noise during call.

#if defined ATTACH_ADDITINAL_PCM_DRIVER
int vtCallActive = 0;
#endif
//------------------------------------------------
// Definitions of sound path
//------------------------------------------------
select_route universal_wm8994_playback_paths[] = 
	{wm8994_set_off, wm8994_set_playback_receiver,
	wm8994_set_playback_speaker, wm8994_set_playback_headset, wm8994_set_off, wm8994_set_playback_speaker_headset};

select_route universal_wm8994_voicecall_paths[] = 
	{wm8994_set_off, wm8994_set_voicecall_receiver, 
	wm8994_set_voicecall_speaker, wm8994_set_voicecall_headset, wm8994_set_voicecall_bluetooth}; 

select_mic_route universal_wm8994_mic_paths[] = {wm8994_record_main_mic,wm8994_record_headset_mic};


//------------------------------------------------
// Implementation of I2C functions
//------------------------------------------------
static unsigned int wm8994_read_hw(struct snd_soc_codec *codec, u16 reg)
{
	struct i2c_msg xfer[2];
	u16 data;
	int ret;
	struct i2c_client *i2c = codec->control_data;

	data = ((reg & 0xff00) >> 8) | ((reg & 0xff) << 8);
	
	/* Write register */
	xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = 2;  //1
	//xfer[0].buf = &reg;
	xfer[0].buf = (void *)&data;

	/* Read data */
	xfer[1].addr = i2c->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 2;
	xfer[1].buf = (u8 *)&data;
	ret = i2c_transfer(i2c->adapter, xfer, 2);
	if (ret != 2) {
		dev_err(codec->dev, "Failed to read 0x%x: %d\n", reg, ret);
		return 0;
	}

	return (data >> 8) | ((data & 0xff) << 8);
}

int wm8994_write(struct snd_soc_codec *codec, unsigned int reg, unsigned int value)
{
	u8 data[4];
	int ret;
	//BUG_ON(reg > WM8993_MAX_REGISTER);

	/* data is
	 *   D15..D9 WM8993 register offset
	 *   D8...D0 register data
	 */
	data[0] = (reg & 0xff00 ) >> 8;
	data[1] = reg & 0x00ff;
	data[2] = value >> 8;
	data[3] = value & 0x00ff;
	ret = codec->hw_write(codec->control_data, data, 4);

	if (ret == 4)
=======
static int wm8994_readable(struct snd_soc_codec *codec, unsigned int reg)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994 *control = wm8994->control_data;

	switch (reg) {
	case WM8994_GPIO_1:
	case WM8994_GPIO_2:
	case WM8994_GPIO_3:
	case WM8994_GPIO_4:
	case WM8994_GPIO_5:
	case WM8994_GPIO_6:
	case WM8994_GPIO_7:
	case WM8994_GPIO_8:
	case WM8994_GPIO_9:
	case WM8994_GPIO_10:
	case WM8994_GPIO_11:
	case WM8994_INTERRUPT_STATUS_1:
	case WM8994_INTERRUPT_STATUS_2:
	case WM8994_INTERRUPT_RAW_STATUS_2:
		return 1;

	case WM8958_DSP2_PROGRAM:
	case WM8958_DSP2_CONFIG:
	case WM8958_DSP2_EXECCONTROL:
		if (control->type == WM8958)
			return 1;
		else
			return 0;

	default:
		break;
	}

	if (reg >= WM8994_CACHE_SIZE)
		return 0;
	return wm8994_access_masks[reg].readable != 0;
}

static int wm8994_volatile(struct snd_soc_codec *codec, unsigned int reg)
{
	if (reg >= WM8994_CACHE_SIZE)
		return 1;

	switch (reg) {
	case WM8994_SOFTWARE_RESET:
	case WM8994_CHIP_REVISION:
	case WM8994_DC_SERVO_1:
	case WM8994_DC_SERVO_READBACK:
	case WM8994_RATE_STATUS:
	case WM8994_LDO_1:
	case WM8994_LDO_2:
	case WM8958_DSP2_EXECCONTROL:
	case WM8958_MIC_DETECT_3:
		return 1;
	default:
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
		return 0;
	//if (ret < 0)
	else{ 
		printk("i2c write problem occured\n");
		return ret;
	}
#if 0
	struct i2c_msg wmsg;
	unsigned char data[100];
	int ret,i;
	struct i2c_client *i2c_client = codec->control_data;
	
	wmsg.addr = i2c_client->addr;
	wmsg.flags = 0;
	wmsg.len = 4;
	wmsg.buf = data;
	
	data[0] = reg & 0x00ff;
	data[1] = (reg & 0xff00 ) >> 8;
	data[2] = value & 0x00ff;
	data[3] = value >> 8;
	
	ret = i2c_transfer(i2c_client->adapter,&wmsg,1);
	if(ret!=1)
	     printk("wm8994 i2c write problem occured\n");

	return ret;	

	return -EIO;
#endif
}

inline unsigned int wm8994_read(struct snd_soc_codec *codec, unsigned int reg)
{
<<<<<<< HEAD
	return wm8994_read_hw(codec, reg); 
}
=======
	int ret;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

//------------------------------------------------
// Functions related volume.
//------------------------------------------------
static const DECLARE_TLV_DB_SCALE(dac_tlv, -12750, 50, 1);

<<<<<<< HEAD
static int wm899x_outpga_put_volsw_vu(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
        int ret;
        u16 val;

        struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
        struct soc_mixer_control *mc =
                (struct soc_mixer_control *)kcontrol->private_value;
        int reg = mc->reg;
=======
	if (!wm8994_volatile(codec, reg)) {
		ret = snd_soc_cache_write(codec, reg, value);
		if (ret != 0)
			dev_err(codec->dev, "Cache write to %x failed: %d\n",
				reg, ret);
	}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	DEBUG_LOG("");

<<<<<<< HEAD
        ret = snd_soc_put_volsw_2r(kcontrol, ucontrol);
        if (ret < 0)
                return ret;
=======
static unsigned int wm8994_read(struct snd_soc_codec *codec,
				unsigned int reg)
{
	unsigned int val;
	int ret;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

        /* now hit the volume update bits (always bit 8) */
        val = wm8994_read(codec, reg);

<<<<<<< HEAD
        return wm8994_write(codec, reg, val | 0x0100);
=======
	if (!wm8994_volatile(codec, reg) && wm8994_readable(codec, reg) &&
	    reg < codec->driver->reg_cache_size) {
		ret = snd_soc_cache_read(codec, reg, &val);
		if (ret >= 0)
			return val;
		else
			dev_err(codec->dev, "Cache read from %x failed: %d\n",
				reg, ret);
	}

	return wm8994_reg_read(codec->control_data, reg);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
}

static int wm899x_inpga_put_volsw_vu(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
        struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
        struct soc_mixer_control *mc =
                (struct soc_mixer_control *)kcontrol->private_value;
        int reg = mc->reg;
        int ret;
        u16 val;

        ret = snd_soc_put_volsw(kcontrol, ucontrol);
        if (ret < 0)
                return ret;

	/* now hit the volume update bits (always bit 8) */
        val = wm8994_read(codec, reg);

        return wm8994_write(codec, reg, val | 0x0100);

}


//------------------------------------------------
// Implementation of sound path
//------------------------------------------------
#define MAX_PLAYBACK_PATHS 5
#define MAX_VOICECALL_PATH 4
static const char *playback_path[] = { "OFF", "RCV", "SPK", "HP", "BT", "SPK_HP"};
static const char *voicecall_path[] = { "OFF", "RCV", "SPK", "HP", "BT", };
static const char *fmradio_path[] = { "FMR_OFF", "FMR_SPK", "FMR_HP", "FMR_SPK_MIX", "FMR_HP_MIX", "FMR_SPK_HP_MIX"};
static const char *mic_path[] = { "Main Mic", "Hands Free Mic", };
static const char *codec_tuning_control[] = {"OFF", "ON"};
static const char *mic_state[] = {"MIC_NO_USE", "MIC_USE"};

static int wm8994_get_mic_path(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	DEBUG_LOG("");
#if 0
	ucontrol->value.integer.value[0] = wm8994_mic_path;
	return 0;
#endif
	return 0;
}

static int wm8994_set_mic_path(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = codec->drvdata;

	DEBUG_LOG("");
			
	if (ucontrol->value.integer.value[0] == 0) // MAIN MIC
                wm8994->rec_path = MAIN;
	else if (ucontrol->value.integer.value[0] == 1) // SUB MIC
                wm8994->rec_path = SUB;
	else
		return -EINVAL;
	
	audio_ctrl_mic_bias_gpio(1);
	 wm8994->universal_mic_path[wm8994->rec_path ](codec);
	return 0;
}

static int wm8994_get_path(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	DEBUG_LOG("");
#if 0
	int i = 0;

	while(audio_path[i] != NULL) {
		if(!strcmp(audio_path[i], kcontrol->id.name) && ((wm8994_path >> 4) == i)) {
			ucontrol->value.integer.value[0] = wm8994_path & 0xf;
			break;
		}
		i++;
	}
#endif
	return 0;
}

static int wm8994_set_path(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
        struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = codec->drvdata;
	struct soc_enum *mc =
                (struct soc_enum *)kcontrol->private_value;

	// Get path value
	int path_num = ucontrol->value.integer.value[0];

	if(strcmp(mc->texts[path_num],playback_path[path_num]) )
	{		
		DEBUG_LOG_ERR("Unknown path %s\n", mc->texts[path_num] );		
		return -ENODEV;
	}

	if(path_num > MAX_PLAYBACK_PATHS )
	{
		DEBUG_LOG_ERR("Unknown Path\n");
		return -ENODEV;
	}

	//select the requested path from the array of function pointers
	switch(path_num)
	{
		case OFF:
			DEBUG_LOG("Switching off output path\n");
			break;

		case RCV:
		case SPK:
		case HP:	
		case SPK_HP :
			DEBUG_LOG("routing to %s \n", mc->texts[path_num] );
			break;			

		case BT:	
		default:
			DEBUG_LOG_ERR("The audio path[%d] does not exists!! \n", path_num);
			return -ENODEV;
			break;
	}

<<<<<<< HEAD
	wm8994->cur_path = path_num;
	wm8994->call_state = DISCONNECT;
	wm8994->universal_playback_path[wm8994->cur_path](codec);
	
	if(wm8994->mic_state == MIC_NO_USE)
		audio_ctrl_mic_bias_gpio(0);
	
	return 0;
}
=======
	snd_soc_dapm_sync(&codec->dapm);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

static int wm8994_get_voice_path(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	DEBUG_LOG("");
#if 0
	while(audio_path[i] != NULL) {
		if(!strcmp(audio_path[i], kcontrol->id.name) && ((wm8994_path >> 4) == i)) {
			ucontrol->value.integer.value[0] = wm8994_path & 0xf;
			break;
		}
		i++;
	}
#endif
	return 0;
}

static int wm8994_set_voice_path(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
        struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = codec->drvdata;	
	struct soc_enum *mc =
		(struct soc_enum *)kcontrol->private_value;

	// Get path value
	int path_num = ucontrol->value.integer.value[0];	
	
	if(strcmp( mc->texts[path_num], voicecall_path[path_num]) )
	{		
		DEBUG_LOG_ERR("Unknown path %s\n", mc->texts[path_num] );
		return -ENODEV;
	}
	
	wm8994->cur_path = path_num;
	wm8994->call_state = CONNECT;
	
	switch(path_num)
	{
		case OFF :
			DEBUG_LOG("Switching off output path\n");
			break;
			
		case RCV :
		case SPK :
		case HP :
		case BT :
			DEBUG_LOG("routing  voice path to  %s \n", mc->texts[path_num] );
			break;
		
		default:
			DEBUG_LOG_ERR("The audio path[%d] does not exists!! \n", path_num);
			return -ENODEV;
			break;
	}

        wm8994->universal_voicecall_path[wm8994->cur_path](codec);

	return 0;
}


static int wm8994_get_fmradio_path(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
#if AUDIO_COMMON_DEBUG		// for removing warning on compliing.
	int pathnum = ucontrol->value.integer.value[0];
#endif

<<<<<<< HEAD
	DEBUG_LOG("wm8994_get_fmradio_path : %d", pathnum);
=======
static const char *adc_hpf_text[] = {
	"HiFi", "Voice 1", "Voice 2", "Voice 3"
};

static const struct soc_enum aif1adc1_hpf =
	SOC_ENUM_SINGLE(WM8994_AIF1_ADC1_FILTERS, 13, 4, adc_hpf_text);

static const struct soc_enum aif1adc2_hpf =
	SOC_ENUM_SINGLE(WM8994_AIF1_ADC2_FILTERS, 13, 4, adc_hpf_text);

static const struct soc_enum aif2adc_hpf =
	SOC_ENUM_SINGLE(WM8994_AIF2_ADC_FILTERS, 13, 4, adc_hpf_text);

static const DECLARE_TLV_DB_SCALE(aif_tlv, 0, 600, 0);
static const DECLARE_TLV_DB_SCALE(digital_tlv, -7200, 75, 1);
static const DECLARE_TLV_DB_SCALE(st_tlv, -3600, 300, 0);
static const DECLARE_TLV_DB_SCALE(wm8994_3d_tlv, -1600, 183, 0);
static const DECLARE_TLV_DB_SCALE(eq_tlv, -1200, 100, 0);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	return 0;
}

static int wm8994_set_fmradio_path(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct soc_enum *mc =
		(struct soc_enum *)kcontrol->private_value;
	struct wm8994_priv *wm8994 = codec->drvdata;
	
	int path_num = ucontrol->value.integer.value[0];	
	
	if(strcmp( mc->texts[path_num], fmradio_path[path_num]) )
	{		
		DEBUG_LOG_ERR("Unknown path %s\n", mc->texts[path_num] );		
	}
	
	if(path_num == wm8994->fmradio_path)
	{
		DEBUG_LOG("%s is already set. skip to set path.. \n", mc->texts[path_num]);
		return 0;
	}
		
	switch(path_num)
	{
		case FMR_OFF:
			DEBUG_LOG("Switching off output path\n");
			wm8994_disable_fmradio_path(codec, FMR_OFF);
			break;
			
		case FMR_SPK:
			DEBUG_LOG("routing  fmradio path to  %s \n", mc->texts[path_num] );
			wm8994_set_fmradio_speaker(codec);
			break;

		case FMR_HP:
			DEBUG_LOG("routing  fmradio path to  %s \n", mc->texts[path_num] );
			wm8994_set_fmradio_headset(codec);
			break;

		case FMR_SPK_MIX:
			DEBUG_LOG("routing  fmradio path to  %s \n", mc->texts[path_num]);
			wm8994_set_fmradio_speaker_mix(codec);
			break;

<<<<<<< HEAD
		case FMR_HP_MIX:
			DEBUG_LOG("routing  fmradio path to  %s \n", mc->texts[path_num]);
			wm8994_set_fmradio_headset_mix(codec);
			break;

		case FMR_SPK_HP_MIX :
			DEBUG_LOG("routing  fmradio path to  %s \n", mc->texts[path_num]);
			wm8994_set_fmradio_speaker_headset_mix(codec);
			break;			

		default:
			DEBUG_LOG_ERR("The audio path[%d] does not exists!! \n", path_num);
			return -ENODEV;
			break;
	}
	
	return 0;
=======
static void wm8994_set_drc(struct snd_soc_codec *codec, int drc)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int base = wm8994_drc_base[drc];
	int cfg = wm8994->drc_cfg[drc];
	int save, i;

	/* Save any enables; the configuration should clear them. */
	save = snd_soc_read(codec, base);
	save &= WM8994_AIF1DAC1_DRC_ENA | WM8994_AIF1ADC1L_DRC_ENA |
		WM8994_AIF1ADC1R_DRC_ENA;

	for (i = 0; i < WM8994_DRC_REGS; i++)
		snd_soc_update_bits(codec, base + i, 0xffff,
				    pdata->drc_cfgs[cfg].regs[i]);

	snd_soc_update_bits(codec, base, WM8994_AIF1DAC1_DRC_ENA |
			     WM8994_AIF1ADC1L_DRC_ENA |
			     WM8994_AIF1ADC1R_DRC_ENA, save);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
}

static int wm8994_get_codec_tuning(struct snd_kcontrol *kcontrol, 
	struct snd_ctl_elem_value *ucontrol)
{	
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = codec->drvdata;

	DEBUG_LOG("testmode_config_flag = [%d]", wm8994->testmode_config_flag);

	return wm8994->testmode_config_flag;
}

static int wm8994_set_codec_tuning(struct snd_kcontrol *kcontrol, 
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
<<<<<<< HEAD
	struct wm8994_priv *wm8994 = codec->drvdata;
=======
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int drc = wm8994_get_drc(kcontrol->id.name);
	int value = ucontrol->value.integer.value[0];
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	int control_flag = ucontrol->value.integer.value[0];	

	DEBUG_LOG("control flag =[%d]", control_flag); 

	wm8994->testmode_config_flag = control_flag;

	return 0;
}

static int wm8994_get_mic_status(struct snd_kcontrol *kcontrol, 
	struct snd_ctl_elem_value *ucontrol)
{	
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = codec->drvdata;

	DEBUG_LOG("mic_state = [%d]", wm8994->mic_state);

	return wm8994->testmode_config_flag;
}

static int wm8994_set_mic_status(struct snd_kcontrol *kcontrol, 
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = codec->drvdata;

	int control_flag = ucontrol->value.integer.value[0];	

	DEBUG_LOG("Changed mic state [%d] => [%d]", wm8994->mic_state, control_flag); 

	wm8994->mic_state = control_flag;

	return 0;
}

void wm8994_set_off(struct snd_soc_codec *codec)
{
	DEBUG_LOG("");
	
	audio_power(0);
}

<<<<<<< HEAD
//#define USE_INFINIEON_EC_FOR_VT 	//VT call flag used in TN platform
=======
static int wm8994_put_retune_mobile_enum(struct snd_kcontrol *kcontrol,
					 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_pdata *pdata = wm8994->pdata;
	int block = wm8994_get_retune_mobile_block(kcontrol->id.name);
	int value = ucontrol->value.integer.value[0];

	if (block < 0)
		return block;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

#if defined USE_INFINIEON_EC_FOR_VT
extern int s3c_pcmdev_clock_control(int enable);

static const char *clock_control[] = { "OFF", "ON"};

static const struct soc_enum clock_control_enum[] = {
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(clock_control),clock_control),
};

static int s3c_pcmdev_get_clock(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
	return 0;
}

static int s3c_pcmdev_set_clock(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
	// Get path value
	int enable = ucontrol->value.integer.value[0];

	s3c_pcmdev_clock_control(enable);

	return 0;
}
#endif

<<<<<<< HEAD
#define  SOC_WM899X_OUTPGA_DOUBLE_R_TLV(xname, reg_left, reg_right, xshift, xmax, xinvert, tlv_array) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname),\
	.access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |\
		 SNDRV_CTL_ELEM_ACCESS_READWRITE,\
	.tlv.p = (tlv_array), \
	.info = snd_soc_info_volsw_2r, \
	.get = snd_soc_get_volsw_2r, .put = wm899x_outpga_put_volsw_vu, \
	.private_value = (unsigned long)&(struct soc_mixer_control) \
		{.reg = reg_left, .rreg = reg_right, .shift = xshift, \
		.max = xmax, .invert = xinvert} }


#define SOC_WM899X_OUTPGA_SINGLE_R_TLV(xname, reg, shift, max, invert,\
         tlv_array) {\
        .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = (xname), \
        .access = SNDRV_CTL_ELEM_ACCESS_TLV_READ |\
                  SNDRV_CTL_ELEM_ACCESS_READWRITE,\
        .tlv.p = (tlv_array), \
        .info = snd_soc_info_volsw, \
        .get = snd_soc_get_volsw, .put = wm899x_inpga_put_volsw_vu, \
        .private_value = SOC_SINGLE_VALUE(reg, shift, max, invert) }

//these are all factors of .01dB
static const DECLARE_TLV_DB_SCALE(digital_tlv, -7162, 37, 1);
static const DECLARE_TLV_DB_LINEAR(digital_tlv_spkr,-5700,600);
static const DECLARE_TLV_DB_LINEAR(digital_tlv_rcv,-5700,600);
static const DECLARE_TLV_DB_LINEAR(digital_tlv_headphone,-5700,600);
static const DECLARE_TLV_DB_LINEAR(digital_tlv_mic,-7162,7162);


static const struct soc_enum path_control_enum[] = {
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(playback_path),playback_path),
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(voicecall_path),voicecall_path),
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(mic_path),mic_path),
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(fmradio_path),fmradio_path), 
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(codec_tuning_control), codec_tuning_control), 
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(mic_state), mic_state),
=======
static const char *aif_chan_src_text[] = {
	"Left", "Right"
};

static const struct soc_enum aif1adcl_src =
	SOC_ENUM_SINGLE(WM8994_AIF1_CONTROL_1, 15, 2, aif_chan_src_text);

static const struct soc_enum aif1adcr_src =
	SOC_ENUM_SINGLE(WM8994_AIF1_CONTROL_1, 14, 2, aif_chan_src_text);

static const struct soc_enum aif2adcl_src =
	SOC_ENUM_SINGLE(WM8994_AIF2_CONTROL_1, 15, 2, aif_chan_src_text);

static const struct soc_enum aif2adcr_src =
	SOC_ENUM_SINGLE(WM8994_AIF2_CONTROL_1, 14, 2, aif_chan_src_text);

static const struct soc_enum aif1dacl_src =
	SOC_ENUM_SINGLE(WM8994_AIF1_CONTROL_2, 15, 2, aif_chan_src_text);

static const struct soc_enum aif1dacr_src =
	SOC_ENUM_SINGLE(WM8994_AIF1_CONTROL_2, 14, 2, aif_chan_src_text);

static const struct soc_enum aif2dacl_src =
	SOC_ENUM_SINGLE(WM8994_AIF2_CONTROL_2, 15, 2, aif_chan_src_text);

static const struct soc_enum aif2dacr_src =
	SOC_ENUM_SINGLE(WM8994_AIF2_CONTROL_2, 14, 2, aif_chan_src_text);

static const char *osr_text[] = {
	"Low Power", "High Performance",
};

static const struct soc_enum dac_osr =
	SOC_ENUM_SINGLE(WM8994_OVERSAMPLING, 0, 2, osr_text);

static const struct soc_enum adc_osr =
	SOC_ENUM_SINGLE(WM8994_OVERSAMPLING, 1, 2, osr_text);

static const struct snd_kcontrol_new wm8994_snd_controls[] = {
SOC_DOUBLE_R_TLV("AIF1ADC1 Volume", WM8994_AIF1_ADC1_LEFT_VOLUME,
		 WM8994_AIF1_ADC1_RIGHT_VOLUME,
		 1, 119, 0, digital_tlv),
SOC_DOUBLE_R_TLV("AIF1ADC2 Volume", WM8994_AIF1_ADC2_LEFT_VOLUME,
		 WM8994_AIF1_ADC2_RIGHT_VOLUME,
		 1, 119, 0, digital_tlv),
SOC_DOUBLE_R_TLV("AIF2ADC Volume", WM8994_AIF2_ADC_LEFT_VOLUME,
		 WM8994_AIF2_ADC_RIGHT_VOLUME,
		 1, 119, 0, digital_tlv),

SOC_ENUM("AIF1ADCL Source", aif1adcl_src),
SOC_ENUM("AIF1ADCR Source", aif1adcr_src),
SOC_ENUM("AIF2ADCL Source", aif2adcl_src),
SOC_ENUM("AIF2ADCR Source", aif2adcr_src),

SOC_ENUM("AIF1DACL Source", aif1dacl_src),
SOC_ENUM("AIF1DACR Source", aif1dacr_src),
SOC_ENUM("AIF2DACL Source", aif2dacl_src),
SOC_ENUM("AIF2DACR Source", aif2dacr_src),

SOC_DOUBLE_R_TLV("AIF1DAC1 Volume", WM8994_AIF1_DAC1_LEFT_VOLUME,
		 WM8994_AIF1_DAC1_RIGHT_VOLUME, 1, 96, 0, digital_tlv),
SOC_DOUBLE_R_TLV("AIF1DAC2 Volume", WM8994_AIF1_DAC2_LEFT_VOLUME,
		 WM8994_AIF1_DAC2_RIGHT_VOLUME, 1, 96, 0, digital_tlv),
SOC_DOUBLE_R_TLV("AIF2DAC Volume", WM8994_AIF2_DAC_LEFT_VOLUME,
		 WM8994_AIF2_DAC_RIGHT_VOLUME, 1, 96, 0, digital_tlv),

SOC_SINGLE_TLV("AIF1 Boost Volume", WM8994_AIF1_CONTROL_2, 10, 3, 0, aif_tlv),
SOC_SINGLE_TLV("AIF2 Boost Volume", WM8994_AIF2_CONTROL_2, 10, 3, 0, aif_tlv),

SOC_SINGLE("AIF1DAC1 EQ Switch", WM8994_AIF1_DAC1_EQ_GAINS_1, 0, 1, 0),
SOC_SINGLE("AIF1DAC2 EQ Switch", WM8994_AIF1_DAC2_EQ_GAINS_1, 0, 1, 0),
SOC_SINGLE("AIF2 EQ Switch", WM8994_AIF2_EQ_GAINS_1, 0, 1, 0),

WM8994_DRC_SWITCH("AIF1DAC1 DRC Switch", WM8994_AIF1_DRC1_1, 2),
WM8994_DRC_SWITCH("AIF1ADC1L DRC Switch", WM8994_AIF1_DRC1_1, 1),
WM8994_DRC_SWITCH("AIF1ADC1R DRC Switch", WM8994_AIF1_DRC1_1, 0),

WM8994_DRC_SWITCH("AIF1DAC2 DRC Switch", WM8994_AIF1_DRC2_1, 2),
WM8994_DRC_SWITCH("AIF1ADC2L DRC Switch", WM8994_AIF1_DRC2_1, 1),
WM8994_DRC_SWITCH("AIF1ADC2R DRC Switch", WM8994_AIF1_DRC2_1, 0),

WM8994_DRC_SWITCH("AIF2DAC DRC Switch", WM8994_AIF2_DRC_1, 2),
WM8994_DRC_SWITCH("AIF2ADCL DRC Switch", WM8994_AIF2_DRC_1, 1),
WM8994_DRC_SWITCH("AIF2ADCR DRC Switch", WM8994_AIF2_DRC_1, 0),

SOC_SINGLE_TLV("DAC1 Right Sidetone Volume", WM8994_DAC1_MIXER_VOLUMES,
	       5, 12, 0, st_tlv),
SOC_SINGLE_TLV("DAC1 Left Sidetone Volume", WM8994_DAC1_MIXER_VOLUMES,
	       0, 12, 0, st_tlv),
SOC_SINGLE_TLV("DAC2 Right Sidetone Volume", WM8994_DAC2_MIXER_VOLUMES,
	       5, 12, 0, st_tlv),
SOC_SINGLE_TLV("DAC2 Left Sidetone Volume", WM8994_DAC2_MIXER_VOLUMES,
	       0, 12, 0, st_tlv),
SOC_ENUM("Sidetone HPF Mux", sidetone_hpf),
SOC_SINGLE("Sidetone HPF Switch", WM8994_SIDETONE, 6, 1, 0),

SOC_ENUM("AIF1ADC1 HPF Mode", aif1adc1_hpf),
SOC_DOUBLE("AIF1ADC1 HPF Switch", WM8994_AIF1_ADC1_FILTERS, 12, 11, 1, 0),

SOC_ENUM("AIF1ADC2 HPF Mode", aif1adc2_hpf),
SOC_DOUBLE("AIF1ADC2 HPF Switch", WM8994_AIF1_ADC2_FILTERS, 12, 11, 1, 0),

SOC_ENUM("AIF2ADC HPF Mode", aif2adc_hpf),
SOC_DOUBLE("AIF2ADC HPF Switch", WM8994_AIF2_ADC_FILTERS, 12, 11, 1, 0),

SOC_ENUM("ADC OSR", adc_osr),
SOC_ENUM("DAC OSR", dac_osr),

SOC_DOUBLE_R_TLV("DAC1 Volume", WM8994_DAC1_LEFT_VOLUME,
		 WM8994_DAC1_RIGHT_VOLUME, 1, 96, 0, digital_tlv),
SOC_DOUBLE_R("DAC1 Switch", WM8994_DAC1_LEFT_VOLUME,
	     WM8994_DAC1_RIGHT_VOLUME, 9, 1, 1),

SOC_DOUBLE_R_TLV("DAC2 Volume", WM8994_DAC2_LEFT_VOLUME,
		 WM8994_DAC2_RIGHT_VOLUME, 1, 96, 0, digital_tlv),
SOC_DOUBLE_R("DAC2 Switch", WM8994_DAC2_LEFT_VOLUME,
	     WM8994_DAC2_RIGHT_VOLUME, 9, 1, 1),

SOC_SINGLE_TLV("SPKL DAC2 Volume", WM8994_SPKMIXL_ATTENUATION,
	       6, 1, 1, wm_hubs_spkmix_tlv),
SOC_SINGLE_TLV("SPKL DAC1 Volume", WM8994_SPKMIXL_ATTENUATION,
	       2, 1, 1, wm_hubs_spkmix_tlv),

SOC_SINGLE_TLV("SPKR DAC2 Volume", WM8994_SPKMIXR_ATTENUATION,
	       6, 1, 1, wm_hubs_spkmix_tlv),
SOC_SINGLE_TLV("SPKR DAC1 Volume", WM8994_SPKMIXR_ATTENUATION,
	       2, 1, 1, wm_hubs_spkmix_tlv),

SOC_SINGLE_TLV("AIF1DAC1 3D Stereo Volume", WM8994_AIF1_DAC1_FILTERS_2,
	       10, 15, 0, wm8994_3d_tlv),
SOC_SINGLE("AIF1DAC1 3D Stereo Switch", WM8994_AIF1_DAC1_FILTERS_2,
	   8, 1, 0),
SOC_SINGLE_TLV("AIF1DAC2 3D Stereo Volume", WM8994_AIF1_DAC2_FILTERS_2,
	       10, 15, 0, wm8994_3d_tlv),
SOC_SINGLE("AIF1DAC2 3D Stereo Switch", WM8994_AIF1_DAC2_FILTERS_2,
	   8, 1, 0),
SOC_SINGLE_TLV("AIF2DAC 3D Stereo Volume", WM8994_AIF2_DAC_FILTERS_2,
	       10, 15, 0, wm8994_3d_tlv),
SOC_SINGLE("AIF2DAC 3D Stereo Switch", WM8994_AIF2_DAC_FILTERS_2,
	   8, 1, 0),
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
};


<<<<<<< HEAD
static const struct snd_kcontrol_new wm8994_snd_controls[] = {
	SOC_WM899X_OUTPGA_DOUBLE_R_TLV("Playback Volume",  WM8994_LEFT_OPGA_VOLUME ,
		  		 WM8994_RIGHT_OPGA_VOLUME , 0, 0x3F, 0, digital_tlv_rcv),
	SOC_WM899X_OUTPGA_DOUBLE_R_TLV("Playback Spkr Volume", WM8994_SPEAKER_VOLUME_LEFT  ,
				 WM8994_SPEAKER_VOLUME_RIGHT  , 1,0x3F, 0, digital_tlv_spkr),
	SOC_WM899X_OUTPGA_DOUBLE_R_TLV("Playback Headset Volume",WM8994_LEFT_OUTPUT_VOLUME  ,
				 WM8994_RIGHT_OUTPUT_VOLUME   , 1,0x3F, 0, digital_tlv_headphone),
	SOC_WM899X_OUTPGA_SINGLE_R_TLV("Capture Volume",  WM8994_AIF1_ADC1_LEFT_VOLUME ,
                                         0, 0xEF, 0, digital_tlv_mic),
	 /* Path Control */
        SOC_ENUM_EXT("Playback Path", path_control_enum[0],
                wm8994_get_path, wm8994_set_path),

	SOC_ENUM_EXT("Voice Call Path", path_control_enum[1],
                wm8994_get_voice_path, wm8994_set_voice_path),

	SOC_ENUM_EXT("MIC Path", path_control_enum[2],
                wm8994_get_mic_path, wm8994_set_mic_path),

	SOC_ENUM_EXT("FM Radio Path", path_control_enum[3],
                wm8994_get_fmradio_path, wm8994_set_fmradio_path),

	SOC_ENUM_EXT("Codec Tuning", path_control_enum[4],
					wm8994_get_codec_tuning, wm8994_set_codec_tuning),

#if defined USE_INFINIEON_EC_FOR_VT	
	SOC_ENUM_EXT("Clock Control", clock_control_enum[0],
			s3c_pcmdev_get_clock, s3c_pcmdev_set_clock),
#endif
	SOC_ENUM_EXT("Mic Status", path_control_enum[5],
				wm8994_get_mic_status, wm8994_set_mic_status),
=======
static const struct snd_kcontrol_new wm8958_snd_controls[] = {
SOC_SINGLE_TLV("AIF3 Boost Volume", WM8958_AIF3_CONTROL_2, 10, 3, 0, aif_tlv),
};

static int clk_sys_event(struct snd_soc_dapm_widget *w,
			 struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

} ;//snd_ctrls


/* Add non-DAPM controls */
static int wm8994_add_controls(struct snd_soc_codec *codec)
{
	int err, i;

	for (i = 0; i < ARRAY_SIZE(wm8994_snd_controls); i++) {
		err = snd_ctl_add(codec->card,
				  snd_soc_cnew(&wm8994_snd_controls[i],
					       codec, NULL));
		if (err < 0)
			return err;
	}
	return 0;
}
static const struct snd_soc_dapm_widget wm8994_dapm_widgets[] = {
//SND_SOC_DAPM_DAC("DAC1", "Playback", WM8580_PWRDN1, 2, 1),
//SND_SOC_DAPM_DAC("DAC2", "Playback", WM8580_PWRDN1, 3, 1),
//SND_SOC_DAPM_DAC("DAC3", "Playback", WM8580_PWRDN1, 4, 1),

<<<<<<< HEAD
//SND_SOC_DAPM_OUTPUT("VOUT1L"),
//SND_SOC_DAPM_OUTPUT("VOUT1R"),
//SND_SOC_DAPM_OUTPUT("VOUT2L"),
//SND_SOC_DAPM_OUTPUT("VOUT2R"),
//SND_SOC_DAPM_OUTPUT("VOUT3L"),
//SND_SOC_DAPM_OUTPUT("VOUT3R"),
=======
static void wm8994_update_class_w(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	int enable = 1;
	int source = 0;  /* GCC flow analysis can't track enable */
	int reg, reg_r;

	/* Only support direct DAC->headphone paths */
	reg = snd_soc_read(codec, WM8994_OUTPUT_MIXER_1);
	if (!(reg & WM8994_DAC1L_TO_HPOUT1L)) {
		dev_vdbg(codec->dev, "HPL connected to output mixer\n");
		enable = 0;
	}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

//SND_SOC_DAPM_ADC("ADC", "Capture", WM8580_PWRDN1, 1, 1),

//SND_SOC_DAPM_INPUT("AINL"),
//SND_SOC_DAPM_INPUT("AINR"),
};

static const struct snd_soc_dapm_route audio_map[] = {
#if 0
	{ "VOUT1L", NULL, "DAC1" },
	{ "VOUT1R", NULL, "DAC1" },

<<<<<<< HEAD
	{ "VOUT2L", NULL, "DAC2" },
	{ "VOUT2R", NULL, "DAC2" },

	{ "VOUT3L", NULL, "DAC3" },
	{ "VOUT3R", NULL, "DAC3" },
=======
	if (enable) {
		dev_dbg(codec->dev, "Class W enabled\n");
		snd_soc_update_bits(codec, WM8994_CLASS_W_1,
				    WM8994_CP_DYN_PWR |
				    WM8994_CP_DYN_SRC_SEL_MASK,
				    source | WM8994_CP_DYN_PWR);
		wm8994->hubs.class_w = true;
		
	} else {
		dev_dbg(codec->dev, "Class W disabled\n");
		snd_soc_update_bits(codec, WM8994_CLASS_W_1,
				    WM8994_CP_DYN_PWR, 0);
		wm8994->hubs.class_w = false;
	}
}

static int late_enable_ev(struct snd_soc_dapm_widget *w,
			  struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		if (wm8994->aif1clk_enable) {
			snd_soc_update_bits(codec, WM8994_AIF1_CLOCKING_1,
					    WM8994_AIF1CLK_ENA_MASK,
					    WM8994_AIF1CLK_ENA);
			wm8994->aif1clk_enable = 0;
		}
		if (wm8994->aif2clk_enable) {
			snd_soc_update_bits(codec, WM8994_AIF2_CLOCKING_1,
					    WM8994_AIF2CLK_ENA_MASK,
					    WM8994_AIF2CLK_ENA);
			wm8994->aif2clk_enable = 0;
		}
		break;
	}

	/* We may also have postponed startup of DSP, handle that. */
	wm8958_aif_ev(w, kcontrol, event);

	return 0;
}

static int late_disable_ev(struct snd_soc_dapm_widget *w,
			   struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	switch (event) {
	case SND_SOC_DAPM_POST_PMD:
		if (wm8994->aif1clk_disable) {
			snd_soc_update_bits(codec, WM8994_AIF1_CLOCKING_1,
					    WM8994_AIF1CLK_ENA_MASK, 0);
			wm8994->aif1clk_disable = 0;
		}
		if (wm8994->aif2clk_disable) {
			snd_soc_update_bits(codec, WM8994_AIF2_CLOCKING_1,
					    WM8994_AIF2CLK_ENA_MASK, 0);
			wm8994->aif2clk_disable = 0;
		}
		break;
	}

	return 0;
}

static int aif1clk_ev(struct snd_soc_dapm_widget *w,
		      struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wm8994->aif1clk_enable = 1;
		break;
	case SND_SOC_DAPM_POST_PMD:
		wm8994->aif1clk_disable = 1;
		break;
	}

	return 0;
}

static int aif2clk_ev(struct snd_soc_dapm_widget *w,
		      struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		wm8994->aif2clk_enable = 1;
		break;
	case SND_SOC_DAPM_POST_PMD:
		wm8994->aif2clk_disable = 1;
		break;
	}

	return 0;
}

static int adc_mux_ev(struct snd_soc_dapm_widget *w,
		      struct snd_kcontrol *kcontrol, int event)
{
	late_enable_ev(w, kcontrol, event);
	return 0;
}

static int micbias_ev(struct snd_soc_dapm_widget *w,
		      struct snd_kcontrol *kcontrol, int event)
{
	late_enable_ev(w, kcontrol, event);
	return 0;
}

static int dac_ev(struct snd_soc_dapm_widget *w,
		  struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	unsigned int mask = 1 << w->shift;

	snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_5,
			    mask, mask);
	return 0;
}

static const char *hp_mux_text[] = {
	"Mixer",
	"DAC",
};
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	{ "ADC", NULL, "AINL" },
	{ "ADC", NULL, "AINR" },
#endif
};

static int wm8994_add_widgets(struct snd_soc_codec *codec)
{
<<<<<<< HEAD
	snd_soc_dapm_new_controls(codec, wm8994_dapm_widgets,
				  ARRAY_SIZE(wm8994_dapm_widgets));
=======
	struct snd_soc_dapm_widget_list *wlist = snd_kcontrol_chip(kcontrol);
	struct snd_soc_dapm_widget *w = wlist->widgets[0];
	struct snd_soc_codec *codec = w->codec;
	int ret;

	ret = snd_soc_dapm_put_enum_double(kcontrol, ucontrol);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	snd_soc_dapm_new_widgets(codec);
	return 0;
}

static int configure_clock(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = codec->drvdata;
	unsigned int reg;

	DEBUG_LOG("");

	reg = wm8994_read(codec,WM8994_AIF1_CLOCKING_1);
        reg &= ~WM8994_AIF1CLK_ENA ; //disable the clock
	reg &= ~WM8994_AIF1CLK_SRC_MASK; 
        wm8994_write(codec, WM8994_AIF1_CLOCKING_1, reg);

	/* This should be done on init() for bypass paths */
	switch (wm8994->sysclk_source) {
		case WM8994_SYSCLK_MCLK:
			dev_dbg(codec->dev, "Using %dHz MCLK\n", wm8994->mclk_rate);

			reg = wm8994_read(codec,WM8994_AIF1_CLOCKING_1);
			reg &= ~WM8994_AIF1CLK_ENA ; //disable the clock 
			wm8994_write(codec, WM8994_AIF1_CLOCKING_1, reg);

			reg = wm8994_read(codec,WM8994_AIF1_CLOCKING_1);
			reg &= 0x07; //clear clksrc bits ..now it is for MCLK
			
			if(wm8994->mclk_rate > 13500000)
			{
				reg |= WM8994_AIF1CLK_DIV ; 
				wm8994->sysclk_rate = wm8994->mclk_rate / 2;
			}
			else
			{
				reg &= ~WM8994_AIF1CLK_DIV;
				wm8994->sysclk_rate = wm8994->mclk_rate;
			}
			reg |= WM8994_AIF1CLK_ENA ; //enable the clocks
			wm8994_write(codec, WM8994_AIF1_CLOCKING_1, reg);

			//Enable clocks to the Audio core and sysclk of wm8994
			reg = wm8994_read(codec, WM8994_CLOCKING_1 );		
			reg &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK|WM8994_DSP_FS1CLK_ENA_MASK);
			reg |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
			wm8994_write(codec,WM8994_CLOCKING_1 ,reg);
			break;

		case WM8994_SYSCLK_FLL:
			switch(wm8994->fs )
			{
				case  8000: 
					wm8994_write(codec, WM8994_FLL1_CONTROL_2, 0x2F00);
					wm8994_write(codec, WM8994_FLL1_CONTROL_3, 0x3126);
					wm8994_write(codec, WM8994_FLL1_CONTROL_4, 0x0100);
					wm8994_write(codec, WM8994_FLL1_CONTROL_5, 0x0C88);
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  11025: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x1F00 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x86C2 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x00e0 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  12000: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x1F00 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x3126);
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x0100 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  16000: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x1900 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0xE23E );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x0100 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  22050: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x0F00 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x86C2 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x00E0 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  24000: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x0F00 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x3126 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x0100 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  32000: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x0C00 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0xE23E );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x0100 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  44100: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x0700 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x86C2 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x00E0 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				case  48000: 
					wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x0700 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x3126 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
					wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x0100 );
					wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);
					break;

				default:
					DEBUG_LOG_ERR("Unsupported Frequency\n");
					break;
			}

			reg = wm8994_read(codec,WM8994_AIF1_CLOCKING_1);
			reg |= WM8994_AIF1CLK_ENA ; //enable the clocks
			reg |= WM8994_AIF1CLK_SRC_FLL1;//selecting FLL1
			wm8994_write(codec, WM8994_AIF1_CLOCKING_1, reg);

			//Enable clocks to the Audio core and sysclk of wm8994	
			reg = wm8994_read(codec, WM8994_CLOCKING_1 );
			reg &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK|WM8994_DSP_FS1CLK_ENA_MASK  );
			reg |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA  );
			wm8994_write(codec,WM8994_CLOCKING_1 ,reg);		
			break;

		default:
			dev_err(codec->dev, "System clock not configured\n");
			return -EINVAL;
		}

	dev_dbg(codec->dev, "CLK_SYS is %dHz\n", wm8994->sysclk_rate);

	return 0;
}

static int wm8994_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	//struct wm8994_priv *wm8994 = codec->drvdata;
	DEBUG_LOG("");

	switch (level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
		/* VMID=2*40k */
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
				    WM8994_VMID_SEL_MASK, 0x2);
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_2,
				    WM8994_TSHUT_ENA, WM8994_TSHUT_ENA);
		break;

	case SND_SOC_BIAS_STANDBY:
		if (codec->bias_level == SND_SOC_BIAS_OFF) {
			/* Bring up VMID with fast soft start */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_2,
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    WM8994_VMID_RAMP_MASK |
					    WM8994_BIAS_SRC,
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    WM8994_VMID_RAMP_MASK |
				    WM8994_BIAS_SRC);
		#if 0
			/* If either line output is single ended we
			 * need the VMID buffer */
			if (!wm8993->pdata.lineout1_diff ||
			    !wm8993->pdata.lineout2_diff)
				snd_soc_update_bits(codec, WM8994_ANTIPOP1,
						 WM8994_LINEOUT_VMID_BUF_ENA,
						 WM8994_LINEOUT_VMID_BUF_ENA);
#endif //if 0 shaju
			/* VMID=2*40k */
			snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
					    WM8994_VMID_SEL_MASK |
					    WM8994_BIAS_ENA,
					    WM8994_BIAS_ENA | 0x2);
			//msleep(32);//commented as without sleep() also it behaves properly in SLSI platform

			/* Switch to normal bias */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_2,
					    WM8994_BIAS_SRC |
					    WM8994_STARTUP_BIAS_ENA, 0);
		}

		/* VMID=2*240k */
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
				    WM8994_VMID_SEL_MASK, 0x4);

		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_2,
				    WM8994_TSHUT_ENA, 0);
		break;

	case SND_SOC_BIAS_OFF:
		snd_soc_update_bits(codec, WM8994_ANTIPOP_1,
				    WM8994_LINEOUT_VMID_BUF_ENA, 0);

<<<<<<< HEAD
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
				    WM8994_VMID_SEL_MASK | WM8994_BIAS_ENA,
				    0);
		break;
	}
=======
static const struct snd_kcontrol_new aif1adc2l_mix[] = {
SOC_DAPM_SINGLE("DMIC Switch", WM8994_AIF1_ADC2_LEFT_MIXER_ROUTING,
		1, 1, 0),
SOC_DAPM_SINGLE("AIF2 Switch", WM8994_AIF1_ADC2_LEFT_MIXER_ROUTING,
		0, 1, 0),
};

static const struct snd_kcontrol_new aif1adc2r_mix[] = {
SOC_DAPM_SINGLE("DMIC Switch", WM8994_AIF1_ADC2_RIGHT_MIXER_ROUTING,
		1, 1, 0),
SOC_DAPM_SINGLE("AIF2 Switch", WM8994_AIF1_ADC2_RIGHT_MIXER_ROUTING,
		0, 1, 0),
};

static const struct snd_kcontrol_new aif2dac2l_mix[] = {
SOC_DAPM_SINGLE("Right Sidetone Switch", WM8994_DAC2_LEFT_MIXER_ROUTING,
		5, 1, 0),
SOC_DAPM_SINGLE("Left Sidetone Switch", WM8994_DAC2_LEFT_MIXER_ROUTING,
		4, 1, 0),
SOC_DAPM_SINGLE("AIF2 Switch", WM8994_DAC2_LEFT_MIXER_ROUTING,
		2, 1, 0),
SOC_DAPM_SINGLE("AIF1.2 Switch", WM8994_DAC2_LEFT_MIXER_ROUTING,
		1, 1, 0),
SOC_DAPM_SINGLE("AIF1.1 Switch", WM8994_DAC2_LEFT_MIXER_ROUTING,
		0, 1, 0),
};
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	codec->bias_level = level;

	return 0;
}

static int wm8994_set_sysclk(struct snd_soc_dai *codec_dai,
			     int clk_id, unsigned int freq, int dir)
{
<<<<<<< HEAD
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8994_priv *wm8994 = codec->drvdata;
=======
	struct snd_soc_dapm_widget_list *wlist = snd_kcontrol_chip(kcontrol);
	struct snd_soc_dapm_widget *w = wlist->widgets[0];
	struct snd_soc_codec *codec = w->codec;
	int ret;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	DEBUG_LOG("clk_id =%d ", clk_id);

	switch (clk_id) {
	case WM8994_SYSCLK_MCLK:
		wm8994->mclk_rate = freq;
		wm8994->sysclk_source = clk_id;
		break;
	case WM8994_SYSCLK_FLL:
		wm8994->sysclk_rate = freq;
		wm8994->sysclk_source = clk_id;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}


static int wm8994_set_dai_fmt(struct snd_soc_dai *dai,
			      unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct wm8994_priv *wm8994 = codec->drvdata;

	unsigned int aif1 = wm8994_read(codec,WM8994_AIF1_CONTROL_1);
	unsigned int aif2 = wm8994_read(codec,WM8994_AIF1_MASTER_SLAVE );

	DEBUG_LOG("");

	aif1 &= ~(WM8994_AIF1_LRCLK_INV |WM8994_AIF1_BCLK_INV |
		   WM8994_AIF1_WL_MASK | WM8994_AIF1_FMT_MASK);

<<<<<<< HEAD
	aif2 &= ~( WM8994_AIF1_LRCLK_FRC_MASK| WM8994_AIF1_CLK_FRC| WM8994_AIF1_MSTR ) ; //to enable LRCLK and bclk in master mode
=======
static const struct snd_kcontrol_new sidetone2_mux =
	SOC_DAPM_ENUM("Right Sidetone Mux", sidetone2_enum);

static const char *aif1dac_text[] = {
	"AIF1DACDAT", "AIF3DACDAT",
};

static const struct soc_enum aif1dac_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 0, 2, aif1dac_text);

static const struct snd_kcontrol_new aif1dac_mux =
	SOC_DAPM_ENUM("AIF1DAC Mux", aif1dac_enum);

static const char *aif2dac_text[] = {
	"AIF2DACDAT", "AIF3DACDAT",
};

static const struct soc_enum aif2dac_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 1, 2, aif2dac_text);

static const struct snd_kcontrol_new aif2dac_mux =
	SOC_DAPM_ENUM("AIF2DAC Mux", aif2dac_enum);

static const char *aif2adc_text[] = {
	"AIF2ADCDAT", "AIF3DACDAT",
};

static const struct soc_enum aif2adc_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 2, 2, aif2adc_text);

static const struct snd_kcontrol_new aif2adc_mux =
	SOC_DAPM_ENUM("AIF2ADC Mux", aif2adc_enum);

static const char *aif3adc_text[] = {
	"AIF1ADCDAT", "AIF2ADCDAT", "AIF2DACDAT", "Mono PCM",
};

static const struct soc_enum wm8994_aif3adc_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 3, 3, aif3adc_text);

static const struct snd_kcontrol_new wm8994_aif3adc_mux =
	SOC_DAPM_ENUM("AIF3ADC Mux", wm8994_aif3adc_enum);

static const struct soc_enum wm8958_aif3adc_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 3, 4, aif3adc_text);

static const struct snd_kcontrol_new wm8958_aif3adc_mux =
	SOC_DAPM_ENUM("AIF3ADC Mux", wm8958_aif3adc_enum);

static const char *mono_pcm_out_text[] = {
	"None", "AIF2ADCL", "AIF2ADCR", 
};

static const struct soc_enum mono_pcm_out_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 9, 3, mono_pcm_out_text);

static const struct snd_kcontrol_new mono_pcm_out_mux =
	SOC_DAPM_ENUM("Mono PCM Out Mux", mono_pcm_out_enum);

static const char *aif2dac_src_text[] = {
	"AIF2", "AIF3",
};

/* Note that these two control shouldn't be simultaneously switched to AIF3 */
static const struct soc_enum aif2dacl_src_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 7, 2, aif2dac_src_text);

static const struct snd_kcontrol_new aif2dacl_src_mux =
	SOC_DAPM_ENUM("AIF2DACL Mux", aif2dacl_src_enum);

static const struct soc_enum aif2dacr_src_enum =
	SOC_ENUM_SINGLE(WM8994_POWER_MANAGEMENT_6, 8, 2, aif2dac_src_text);

static const struct snd_kcontrol_new aif2dacr_src_mux =
	SOC_DAPM_ENUM("AIF2DACR Mux", aif2dacr_src_enum);

static const struct snd_soc_dapm_widget wm8994_lateclk_revd_widgets[] = {
SND_SOC_DAPM_SUPPLY("AIF1CLK", SND_SOC_NOPM, 0, 0, aif1clk_ev,
	SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_SUPPLY("AIF2CLK", SND_SOC_NOPM, 0, 0, aif2clk_ev,
	SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

SND_SOC_DAPM_PGA_E("Late DAC1L Enable PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
	late_enable_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_PGA_E("Late DAC1R Enable PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
	late_enable_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_PGA_E("Late DAC2L Enable PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
	late_enable_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_PGA_E("Late DAC2R Enable PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
	late_enable_ev, SND_SOC_DAPM_PRE_PMU),

SND_SOC_DAPM_POST("Late Disable PGA", late_disable_ev)
};

static const struct snd_soc_dapm_widget wm8994_lateclk_widgets[] = {
SND_SOC_DAPM_SUPPLY("AIF1CLK", WM8994_AIF1_CLOCKING_1, 0, 0, NULL, 0),
SND_SOC_DAPM_SUPPLY("AIF2CLK", WM8994_AIF2_CLOCKING_1, 0, 0, NULL, 0)
};

static const struct snd_soc_dapm_widget wm8994_dac_revd_widgets[] = {
SND_SOC_DAPM_DAC_E("DAC2L", NULL, SND_SOC_NOPM, 3, 0,
	dac_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_DAC_E("DAC2R", NULL, SND_SOC_NOPM, 2, 0,
	dac_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_DAC_E("DAC1L", NULL, SND_SOC_NOPM, 1, 0,
	dac_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_DAC_E("DAC1R", NULL, SND_SOC_NOPM, 0, 0,
	dac_ev, SND_SOC_DAPM_PRE_PMU),
};

static const struct snd_soc_dapm_widget wm8994_dac_widgets[] = {
SND_SOC_DAPM_DAC("DAC2L", NULL, WM8994_POWER_MANAGEMENT_5, 3, 0),
SND_SOC_DAPM_DAC("DAC2R", NULL, WM8994_POWER_MANAGEMENT_5, 2, 0),
SND_SOC_DAPM_DAC("DAC1L", NULL, WM8994_POWER_MANAGEMENT_5, 1, 0),
SND_SOC_DAPM_DAC("DAC1R", NULL, WM8994_POWER_MANAGEMENT_5, 0, 0),
};

static const struct snd_soc_dapm_widget wm8994_adc_revd_widgets[] = {
SND_SOC_DAPM_MUX_E("ADCL Mux", WM8994_POWER_MANAGEMENT_4, 1, 0, &adcl_mux,
		   adc_mux_ev, SND_SOC_DAPM_PRE_PMU),
SND_SOC_DAPM_MUX_E("ADCR Mux", WM8994_POWER_MANAGEMENT_4, 0, 0, &adcr_mux,
		   adc_mux_ev, SND_SOC_DAPM_PRE_PMU),
};

static const struct snd_soc_dapm_widget wm8994_adc_widgets[] = {
SND_SOC_DAPM_MUX("ADCL Mux", WM8994_POWER_MANAGEMENT_4, 1, 0, &adcl_mux),
SND_SOC_DAPM_MUX("ADCR Mux", WM8994_POWER_MANAGEMENT_4, 0, 0, &adcr_mux),
};

static const struct snd_soc_dapm_widget wm8994_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("DMIC1DAT"),
SND_SOC_DAPM_INPUT("DMIC2DAT"),
SND_SOC_DAPM_INPUT("Clock"),

SND_SOC_DAPM_MICBIAS("MICBIAS", WM8994_MICBIAS, 2, 0),
SND_SOC_DAPM_SUPPLY_S("MICBIAS Supply", 1, SND_SOC_NOPM, 0, 0, micbias_ev,
		      SND_SOC_DAPM_PRE_PMU),

SND_SOC_DAPM_SUPPLY("CLK_SYS", SND_SOC_NOPM, 0, 0, clk_sys_event,
		    SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

SND_SOC_DAPM_SUPPLY("DSP1CLK", WM8994_CLOCKING_1, 3, 0, NULL, 0),
SND_SOC_DAPM_SUPPLY("DSP2CLK", WM8994_CLOCKING_1, 2, 0, NULL, 0),
SND_SOC_DAPM_SUPPLY("DSPINTCLK", WM8994_CLOCKING_1, 1, 0, NULL, 0),

SND_SOC_DAPM_AIF_OUT("AIF1ADC1L", NULL,
		     0, WM8994_POWER_MANAGEMENT_4, 9, 0),
SND_SOC_DAPM_AIF_OUT("AIF1ADC1R", NULL,
		     0, WM8994_POWER_MANAGEMENT_4, 8, 0),
SND_SOC_DAPM_AIF_IN_E("AIF1DAC1L", NULL, 0,
		      WM8994_POWER_MANAGEMENT_5, 9, 0, wm8958_aif_ev,
		      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_AIF_IN_E("AIF1DAC1R", NULL, 0,
		      WM8994_POWER_MANAGEMENT_5, 8, 0, wm8958_aif_ev,
		      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),

SND_SOC_DAPM_AIF_OUT("AIF1ADC2L", NULL,
		     0, WM8994_POWER_MANAGEMENT_4, 11, 0),
SND_SOC_DAPM_AIF_OUT("AIF1ADC2R", NULL,
		     0, WM8994_POWER_MANAGEMENT_4, 10, 0),
SND_SOC_DAPM_AIF_IN_E("AIF1DAC2L", NULL, 0,
		      WM8994_POWER_MANAGEMENT_5, 11, 0, wm8958_aif_ev,
		      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_AIF_IN_E("AIF1DAC2R", NULL, 0,
		      WM8994_POWER_MANAGEMENT_5, 10, 0, wm8958_aif_ev,
		      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),

SND_SOC_DAPM_MIXER("AIF1ADC1L Mixer", SND_SOC_NOPM, 0, 0,
		   aif1adc1l_mix, ARRAY_SIZE(aif1adc1l_mix)),
SND_SOC_DAPM_MIXER("AIF1ADC1R Mixer", SND_SOC_NOPM, 0, 0,
		   aif1adc1r_mix, ARRAY_SIZE(aif1adc1r_mix)),

SND_SOC_DAPM_MIXER("AIF1ADC2L Mixer", SND_SOC_NOPM, 0, 0,
		   aif1adc2l_mix, ARRAY_SIZE(aif1adc2l_mix)),
SND_SOC_DAPM_MIXER("AIF1ADC2R Mixer", SND_SOC_NOPM, 0, 0,
		   aif1adc2r_mix, ARRAY_SIZE(aif1adc2r_mix)),

SND_SOC_DAPM_MIXER("AIF2DAC2L Mixer", SND_SOC_NOPM, 0, 0,
		   aif2dac2l_mix, ARRAY_SIZE(aif2dac2l_mix)),
SND_SOC_DAPM_MIXER("AIF2DAC2R Mixer", SND_SOC_NOPM, 0, 0,
		   aif2dac2r_mix, ARRAY_SIZE(aif2dac2r_mix)),

SND_SOC_DAPM_MUX("Left Sidetone", SND_SOC_NOPM, 0, 0, &sidetone1_mux),
SND_SOC_DAPM_MUX("Right Sidetone", SND_SOC_NOPM, 0, 0, &sidetone2_mux),

SND_SOC_DAPM_MIXER("DAC1L Mixer", SND_SOC_NOPM, 0, 0,
		   dac1l_mix, ARRAY_SIZE(dac1l_mix)),
SND_SOC_DAPM_MIXER("DAC1R Mixer", SND_SOC_NOPM, 0, 0,
		   dac1r_mix, ARRAY_SIZE(dac1r_mix)),

SND_SOC_DAPM_AIF_OUT("AIF2ADCL", NULL, 0,
		     WM8994_POWER_MANAGEMENT_4, 13, 0),
SND_SOC_DAPM_AIF_OUT("AIF2ADCR", NULL, 0,
		     WM8994_POWER_MANAGEMENT_4, 12, 0),
SND_SOC_DAPM_AIF_IN_E("AIF2DACL", NULL, 0,
		      WM8994_POWER_MANAGEMENT_5, 13, 0, wm8958_aif_ev,
		      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_AIF_IN_E("AIF2DACR", NULL, 0,
		      WM8994_POWER_MANAGEMENT_5, 12, 0, wm8958_aif_ev,
		      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

SND_SOC_DAPM_AIF_IN("AIF1DACDAT", "AIF1 Playback", 0, SND_SOC_NOPM, 0, 0),
SND_SOC_DAPM_AIF_IN("AIF2DACDAT", "AIF2 Playback", 0, SND_SOC_NOPM, 0, 0),
SND_SOC_DAPM_AIF_OUT("AIF1ADCDAT", "AIF1 Capture", 0, SND_SOC_NOPM, 0, 0),
SND_SOC_DAPM_AIF_OUT("AIF2ADCDAT", "AIF2 Capture", 0, SND_SOC_NOPM, 0, 0),

SND_SOC_DAPM_MUX("AIF1DAC Mux", SND_SOC_NOPM, 0, 0, &aif1dac_mux),
SND_SOC_DAPM_MUX("AIF2DAC Mux", SND_SOC_NOPM, 0, 0, &aif2dac_mux),
SND_SOC_DAPM_MUX("AIF2ADC Mux", SND_SOC_NOPM, 0, 0, &aif2adc_mux),

SND_SOC_DAPM_AIF_IN("AIF3DACDAT", "AIF3 Playback", 0, SND_SOC_NOPM, 0, 0),
SND_SOC_DAPM_AIF_IN("AIF3ADCDAT", "AIF3 Capture", 0, SND_SOC_NOPM, 0, 0),

SND_SOC_DAPM_SUPPLY("TOCLK", WM8994_CLOCKING_1, 4, 0, NULL, 0),

SND_SOC_DAPM_ADC("DMIC2L", NULL, WM8994_POWER_MANAGEMENT_4, 5, 0),
SND_SOC_DAPM_ADC("DMIC2R", NULL, WM8994_POWER_MANAGEMENT_4, 4, 0),
SND_SOC_DAPM_ADC("DMIC1L", NULL, WM8994_POWER_MANAGEMENT_4, 3, 0),
SND_SOC_DAPM_ADC("DMIC1R", NULL, WM8994_POWER_MANAGEMENT_4, 2, 0),

/* Power is done with the muxes since the ADC power also controls the
 * downsampling chain, the chip will automatically manage the analogue
 * specific portions.
 */
SND_SOC_DAPM_ADC("ADCL", NULL, SND_SOC_NOPM, 1, 0),
SND_SOC_DAPM_ADC("ADCR", NULL, SND_SOC_NOPM, 0, 0),

SND_SOC_DAPM_MUX("Left Headphone Mux", SND_SOC_NOPM, 0, 0, &hpl_mux),
SND_SOC_DAPM_MUX("Right Headphone Mux", SND_SOC_NOPM, 0, 0, &hpr_mux),

SND_SOC_DAPM_MIXER("SPKL", WM8994_POWER_MANAGEMENT_3, 8, 0,
		   left_speaker_mixer, ARRAY_SIZE(left_speaker_mixer)),
SND_SOC_DAPM_MIXER("SPKR", WM8994_POWER_MANAGEMENT_3, 9, 0,
		   right_speaker_mixer, ARRAY_SIZE(right_speaker_mixer)),

SND_SOC_DAPM_POST("Debug log", post_ev),
};

static const struct snd_soc_dapm_widget wm8994_specific_dapm_widgets[] = {
SND_SOC_DAPM_MUX("AIF3ADC Mux", SND_SOC_NOPM, 0, 0, &wm8994_aif3adc_mux),
};

static const struct snd_soc_dapm_widget wm8958_dapm_widgets[] = {
SND_SOC_DAPM_MUX("Mono PCM Out Mux", SND_SOC_NOPM, 0, 0, &mono_pcm_out_mux),
SND_SOC_DAPM_MUX("AIF2DACL Mux", SND_SOC_NOPM, 0, 0, &aif2dacl_src_mux),
SND_SOC_DAPM_MUX("AIF2DACR Mux", SND_SOC_NOPM, 0, 0, &aif2dacr_src_mux),
SND_SOC_DAPM_MUX("AIF3ADC Mux", SND_SOC_NOPM, 0, 0, &wm8958_aif3adc_mux),
};

static const struct snd_soc_dapm_route intercon[] = {
	{ "CLK_SYS", NULL, "AIF1CLK", check_clk_sys },
	{ "CLK_SYS", NULL, "AIF2CLK", check_clk_sys },

	{ "DSP1CLK", NULL, "CLK_SYS" },
	{ "DSP2CLK", NULL, "CLK_SYS" },
	{ "DSPINTCLK", NULL, "CLK_SYS" },

	{ "AIF1ADC1L", NULL, "AIF1CLK" },
	{ "AIF1ADC1L", NULL, "DSP1CLK" },
	{ "AIF1ADC1R", NULL, "AIF1CLK" },
	{ "AIF1ADC1R", NULL, "DSP1CLK" },
	{ "AIF1ADC1R", NULL, "DSPINTCLK" },

	{ "AIF1DAC1L", NULL, "AIF1CLK" },
	{ "AIF1DAC1L", NULL, "DSP1CLK" },
	{ "AIF1DAC1R", NULL, "AIF1CLK" },
	{ "AIF1DAC1R", NULL, "DSP1CLK" },
	{ "AIF1DAC1R", NULL, "DSPINTCLK" },

	{ "AIF1ADC2L", NULL, "AIF1CLK" },
	{ "AIF1ADC2L", NULL, "DSP1CLK" },
	{ "AIF1ADC2R", NULL, "AIF1CLK" },
	{ "AIF1ADC2R", NULL, "DSP1CLK" },
	{ "AIF1ADC2R", NULL, "DSPINTCLK" },

	{ "AIF1DAC2L", NULL, "AIF1CLK" },
	{ "AIF1DAC2L", NULL, "DSP1CLK" },
	{ "AIF1DAC2R", NULL, "AIF1CLK" },
	{ "AIF1DAC2R", NULL, "DSP1CLK" },
	{ "AIF1DAC2R", NULL, "DSPINTCLK" },

	{ "AIF2ADCL", NULL, "AIF2CLK" },
	{ "AIF2ADCL", NULL, "DSP2CLK" },
	{ "AIF2ADCR", NULL, "AIF2CLK" },
	{ "AIF2ADCR", NULL, "DSP2CLK" },
	{ "AIF2ADCR", NULL, "DSPINTCLK" },

	{ "AIF2DACL", NULL, "AIF2CLK" },
	{ "AIF2DACL", NULL, "DSP2CLK" },
	{ "AIF2DACR", NULL, "AIF2CLK" },
	{ "AIF2DACR", NULL, "DSP2CLK" },
	{ "AIF2DACR", NULL, "DSPINTCLK" },

	{ "DMIC1L", NULL, "DMIC1DAT" },
	{ "DMIC1L", NULL, "CLK_SYS" },
	{ "DMIC1R", NULL, "DMIC1DAT" },
	{ "DMIC1R", NULL, "CLK_SYS" },
	{ "DMIC2L", NULL, "DMIC2DAT" },
	{ "DMIC2L", NULL, "CLK_SYS" },
	{ "DMIC2R", NULL, "DMIC2DAT" },
	{ "DMIC2R", NULL, "CLK_SYS" },

	{ "ADCL", NULL, "AIF1CLK" },
	{ "ADCL", NULL, "DSP1CLK" },
	{ "ADCL", NULL, "DSPINTCLK" },

	{ "ADCR", NULL, "AIF1CLK" },
	{ "ADCR", NULL, "DSP1CLK" },
	{ "ADCR", NULL, "DSPINTCLK" },

	{ "ADCL Mux", "ADC", "ADCL" },
	{ "ADCL Mux", "DMIC", "DMIC1L" },
	{ "ADCR Mux", "ADC", "ADCR" },
	{ "ADCR Mux", "DMIC", "DMIC1R" },

	{ "DAC1L", NULL, "AIF1CLK" },
	{ "DAC1L", NULL, "DSP1CLK" },
	{ "DAC1L", NULL, "DSPINTCLK" },

	{ "DAC1R", NULL, "AIF1CLK" },
	{ "DAC1R", NULL, "DSP1CLK" },
	{ "DAC1R", NULL, "DSPINTCLK" },

	{ "DAC2L", NULL, "AIF2CLK" },
	{ "DAC2L", NULL, "DSP2CLK" },
	{ "DAC2L", NULL, "DSPINTCLK" },

	{ "DAC2R", NULL, "AIF2DACR" },
	{ "DAC2R", NULL, "AIF2CLK" },
	{ "DAC2R", NULL, "DSP2CLK" },
	{ "DAC2R", NULL, "DSPINTCLK" },

	{ "TOCLK", NULL, "CLK_SYS" },

	/* AIF1 outputs */
	{ "AIF1ADC1L", NULL, "AIF1ADC1L Mixer" },
	{ "AIF1ADC1L Mixer", "ADC/DMIC Switch", "ADCL Mux" },
	{ "AIF1ADC1L Mixer", "AIF2 Switch", "AIF2DACL" },

	{ "AIF1ADC1R", NULL, "AIF1ADC1R Mixer" },
	{ "AIF1ADC1R Mixer", "ADC/DMIC Switch", "ADCR Mux" },
	{ "AIF1ADC1R Mixer", "AIF2 Switch", "AIF2DACR" },

	{ "AIF1ADC2L", NULL, "AIF1ADC2L Mixer" },
	{ "AIF1ADC2L Mixer", "DMIC Switch", "DMIC2L" },
	{ "AIF1ADC2L Mixer", "AIF2 Switch", "AIF2DACL" },

	{ "AIF1ADC2R", NULL, "AIF1ADC2R Mixer" },
	{ "AIF1ADC2R Mixer", "DMIC Switch", "DMIC2R" },
	{ "AIF1ADC2R Mixer", "AIF2 Switch", "AIF2DACR" },

	/* Pin level routing for AIF3 */
	{ "AIF1DAC1L", NULL, "AIF1DAC Mux" },
	{ "AIF1DAC1R", NULL, "AIF1DAC Mux" },
	{ "AIF1DAC2L", NULL, "AIF1DAC Mux" },
	{ "AIF1DAC2R", NULL, "AIF1DAC Mux" },

	{ "AIF1DAC Mux", "AIF1DACDAT", "AIF1DACDAT" },
	{ "AIF1DAC Mux", "AIF3DACDAT", "AIF3DACDAT" },
	{ "AIF2DAC Mux", "AIF2DACDAT", "AIF2DACDAT" },
	{ "AIF2DAC Mux", "AIF3DACDAT", "AIF3DACDAT" },
	{ "AIF2ADC Mux", "AIF2ADCDAT", "AIF2ADCL" },
	{ "AIF2ADC Mux", "AIF2ADCDAT", "AIF2ADCR" },
	{ "AIF2ADC Mux", "AIF3DACDAT", "AIF3ADCDAT" },

	/* DAC1 inputs */
	{ "DAC1L Mixer", "AIF2 Switch", "AIF2DACL" },
	{ "DAC1L Mixer", "AIF1.2 Switch", "AIF1DAC2L" },
	{ "DAC1L Mixer", "AIF1.1 Switch", "AIF1DAC1L" },
	{ "DAC1L Mixer", "Left Sidetone Switch", "Left Sidetone" },
	{ "DAC1L Mixer", "Right Sidetone Switch", "Right Sidetone" },

	{ "DAC1R Mixer", "AIF2 Switch", "AIF2DACR" },
	{ "DAC1R Mixer", "AIF1.2 Switch", "AIF1DAC2R" },
	{ "DAC1R Mixer", "AIF1.1 Switch", "AIF1DAC1R" },
	{ "DAC1R Mixer", "Left Sidetone Switch", "Left Sidetone" },
	{ "DAC1R Mixer", "Right Sidetone Switch", "Right Sidetone" },

	/* DAC2/AIF2 outputs  */
	{ "AIF2ADCL", NULL, "AIF2DAC2L Mixer" },
	{ "AIF2DAC2L Mixer", "AIF2 Switch", "AIF2DACL" },
	{ "AIF2DAC2L Mixer", "AIF1.2 Switch", "AIF1DAC2L" },
	{ "AIF2DAC2L Mixer", "AIF1.1 Switch", "AIF1DAC1L" },
	{ "AIF2DAC2L Mixer", "Left Sidetone Switch", "Left Sidetone" },
	{ "AIF2DAC2L Mixer", "Right Sidetone Switch", "Right Sidetone" },

	{ "AIF2ADCR", NULL, "AIF2DAC2R Mixer" },
	{ "AIF2DAC2R Mixer", "AIF2 Switch", "AIF2DACR" },
	{ "AIF2DAC2R Mixer", "AIF1.2 Switch", "AIF1DAC2R" },
	{ "AIF2DAC2R Mixer", "AIF1.1 Switch", "AIF1DAC1R" },
	{ "AIF2DAC2R Mixer", "Left Sidetone Switch", "Left Sidetone" },
	{ "AIF2DAC2R Mixer", "Right Sidetone Switch", "Right Sidetone" },

	{ "AIF1ADCDAT", NULL, "AIF1ADC1L" },
	{ "AIF1ADCDAT", NULL, "AIF1ADC1R" },
	{ "AIF1ADCDAT", NULL, "AIF1ADC2L" },
	{ "AIF1ADCDAT", NULL, "AIF1ADC2R" },

	{ "AIF2ADCDAT", NULL, "AIF2ADC Mux" },

	/* AIF3 output */
	{ "AIF3ADCDAT", "AIF1ADCDAT", "AIF1ADC1L" },
	{ "AIF3ADCDAT", "AIF1ADCDAT", "AIF1ADC1R" },
	{ "AIF3ADCDAT", "AIF1ADCDAT", "AIF1ADC2L" },
	{ "AIF3ADCDAT", "AIF1ADCDAT", "AIF1ADC2R" },
	{ "AIF3ADCDAT", "AIF2ADCDAT", "AIF2ADCL" },
	{ "AIF3ADCDAT", "AIF2ADCDAT", "AIF2ADCR" },
	{ "AIF3ADCDAT", "AIF2DACDAT", "AIF2DACL" },
	{ "AIF3ADCDAT", "AIF2DACDAT", "AIF2DACR" },

	/* Sidetone */
	{ "Left Sidetone", "ADC/DMIC1", "ADCL Mux" },
	{ "Left Sidetone", "DMIC2", "DMIC2L" },
	{ "Right Sidetone", "ADC/DMIC1", "ADCR Mux" },
	{ "Right Sidetone", "DMIC2", "DMIC2R" },

	/* Output stages */
	{ "Left Output Mixer", "DAC Switch", "DAC1L" },
	{ "Right Output Mixer", "DAC Switch", "DAC1R" },

	{ "SPKL", "DAC1 Switch", "DAC1L" },
	{ "SPKL", "DAC2 Switch", "DAC2L" },

	{ "SPKR", "DAC1 Switch", "DAC1R" },
	{ "SPKR", "DAC2 Switch", "DAC2R" },

	{ "Left Headphone Mux", "DAC", "DAC1L" },
	{ "Right Headphone Mux", "DAC", "DAC1R" },
};

static const struct snd_soc_dapm_route wm8994_lateclk_revd_intercon[] = {
	{ "DAC1L", NULL, "Late DAC1L Enable PGA" },
	{ "Late DAC1L Enable PGA", NULL, "DAC1L Mixer" },
	{ "DAC1R", NULL, "Late DAC1R Enable PGA" },
	{ "Late DAC1R Enable PGA", NULL, "DAC1R Mixer" },
	{ "DAC2L", NULL, "Late DAC2L Enable PGA" },
	{ "Late DAC2L Enable PGA", NULL, "AIF2DAC2L Mixer" },
	{ "DAC2R", NULL, "Late DAC2R Enable PGA" },
	{ "Late DAC2R Enable PGA", NULL, "AIF2DAC2R Mixer" }
};

static const struct snd_soc_dapm_route wm8994_lateclk_intercon[] = {
	{ "DAC1L", NULL, "DAC1L Mixer" },
	{ "DAC1R", NULL, "DAC1R Mixer" },
	{ "DAC2L", NULL, "AIF2DAC2L Mixer" },
	{ "DAC2R", NULL, "AIF2DAC2R Mixer" },
};

static const struct snd_soc_dapm_route wm8994_revd_intercon[] = {
	{ "AIF1DACDAT", NULL, "AIF2DACDAT" },
	{ "AIF2DACDAT", NULL, "AIF1DACDAT" },
	{ "AIF1ADCDAT", NULL, "AIF2ADCDAT" },
	{ "AIF2ADCDAT", NULL, "AIF1ADCDAT" },
	{ "MICBIAS", NULL, "CLK_SYS" },
	{ "MICBIAS", NULL, "MICBIAS Supply" },
};

static const struct snd_soc_dapm_route wm8994_intercon[] = {
	{ "AIF2DACL", NULL, "AIF2DAC Mux" },
	{ "AIF2DACR", NULL, "AIF2DAC Mux" },
};

static const struct snd_soc_dapm_route wm8958_intercon[] = {
	{ "AIF2DACL", NULL, "AIF2DACL Mux" },
	{ "AIF2DACR", NULL, "AIF2DACR Mux" },

	{ "AIF2DACL Mux", "AIF2", "AIF2DAC Mux" },
	{ "AIF2DACL Mux", "AIF3", "AIF3DACDAT" },
	{ "AIF2DACR Mux", "AIF2", "AIF2DAC Mux" },
	{ "AIF2DACR Mux", "AIF3", "AIF3DACDAT" },

	{ "Mono PCM Out Mux", "AIF2ADCL", "AIF2ADCL" },
	{ "Mono PCM Out Mux", "AIF2ADCR", "AIF2ADCR" },

	{ "AIF3ADC Mux", "Mono PCM", "Mono PCM Out Mux" },
};

/* The size in bits of the FLL divide multiplied by 10
 * to allow rounding later */
#define FIXED_FLL_SIZE ((1 << 16) * 10)

struct fll_div {
	u16 outdiv;
	u16 n;
	u16 k;
	u16 clk_ref_div;
	u16 fll_fratio;
};

static int wm8994_get_fll_config(struct fll_div *fll,
				 int freq_in, int freq_out)
{
	u64 Kpart;
	unsigned int K, Ndiv, Nmod;

	pr_debug("FLL input=%dHz, output=%dHz\n", freq_in, freq_out);

	/* Scale the input frequency down to <= 13.5MHz */
	fll->clk_ref_div = 0;
	while (freq_in > 13500000) {
		fll->clk_ref_div++;
		freq_in /= 2;

		if (fll->clk_ref_div > 3)
			return -EINVAL;
	}
	pr_debug("CLK_REF_DIV=%d, Fref=%dHz\n", fll->clk_ref_div, freq_in);

	/* Scale the output to give 90MHz<=Fvco<=100MHz */
	fll->outdiv = 3;
	while (freq_out * (fll->outdiv + 1) < 90000000) {
		fll->outdiv++;
		if (fll->outdiv > 63)
			return -EINVAL;
	}
	freq_out *= fll->outdiv + 1;
	pr_debug("OUTDIV=%d, Fvco=%dHz\n", fll->outdiv, freq_out);

	if (freq_in > 1000000) {
		fll->fll_fratio = 0;
	} else if (freq_in > 256000) {
		fll->fll_fratio = 1;
		freq_in *= 2;
	} else if (freq_in > 128000) {
		fll->fll_fratio = 2;
		freq_in *= 4;
	} else if (freq_in > 64000) {
		fll->fll_fratio = 3;
		freq_in *= 8;
	} else {
		fll->fll_fratio = 4;
		freq_in *= 16;
	}
	pr_debug("FLL_FRATIO=%d, Fref=%dHz\n", fll->fll_fratio, freq_in);

	/* Now, calculate N.K */
	Ndiv = freq_out / freq_in;

	fll->n = Ndiv;
	Nmod = freq_out % freq_in;
	pr_debug("Nmod=%d\n", Nmod);

	/* Calculate fractional part - scale up so we can round. */
	Kpart = FIXED_FLL_SIZE * (long long)Nmod;

	do_div(Kpart, freq_in);

	K = Kpart & 0xFFFFFFFF;

	if ((K % 10) >= 5)
		K += 5;

	/* Move down to proper range now rounding is done */
	fll->k = K / 10;

	pr_debug("N=%x K=%x\n", fll->n, fll->k);

	return 0;
}

static int _wm8994_set_fll(struct snd_soc_codec *codec, int id, int src,
			  unsigned int freq_in, unsigned int freq_out)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	int reg_offset, ret;
	struct fll_div fll;
	u16 reg, aif1, aif2;

	aif1 = snd_soc_read(codec, WM8994_AIF1_CLOCKING_1)
		& WM8994_AIF1CLK_ENA;

	aif2 = snd_soc_read(codec, WM8994_AIF2_CLOCKING_1)
		& WM8994_AIF2CLK_ENA;

	switch (id) {
	case WM8994_FLL1:
		reg_offset = 0;
		id = 0;
		break;
	case WM8994_FLL2:
		reg_offset = 0x20;
		id = 1;
		break;
	default:
		return -EINVAL;
	}

	switch (src) {
	case 0:
		/* Allow no source specification when stopping */
		if (freq_out)
			return -EINVAL;
		src = wm8994->fll[id].src;
		break;
	case WM8994_FLL_SRC_MCLK1:
	case WM8994_FLL_SRC_MCLK2:
	case WM8994_FLL_SRC_LRCLK:
	case WM8994_FLL_SRC_BCLK:
		break;
	default:
		return -EINVAL;
	}

	/* Are we changing anything? */
	if (wm8994->fll[id].src == src &&
	    wm8994->fll[id].in == freq_in && wm8994->fll[id].out == freq_out)
		return 0;

	/* If we're stopping the FLL redo the old config - no
	 * registers will actually be written but we avoid GCC flow
	 * analysis bugs spewing warnings.
	 */
	if (freq_out)
		ret = wm8994_get_fll_config(&fll, freq_in, freq_out);
	else
		ret = wm8994_get_fll_config(&fll, wm8994->fll[id].in,
					    wm8994->fll[id].out);
	if (ret < 0)
		return ret;

	/* Gate the AIF clocks while we reclock */
	snd_soc_update_bits(codec, WM8994_AIF1_CLOCKING_1,
			    WM8994_AIF1CLK_ENA, 0);
	snd_soc_update_bits(codec, WM8994_AIF2_CLOCKING_1,
			    WM8994_AIF2CLK_ENA, 0);

	/* We always need to disable the FLL while reconfiguring */
	snd_soc_update_bits(codec, WM8994_FLL1_CONTROL_1 + reg_offset,
			    WM8994_FLL1_ENA, 0);

	reg = (fll.outdiv << WM8994_FLL1_OUTDIV_SHIFT) |
		(fll.fll_fratio << WM8994_FLL1_FRATIO_SHIFT);
	snd_soc_update_bits(codec, WM8994_FLL1_CONTROL_2 + reg_offset,
			    WM8994_FLL1_OUTDIV_MASK |
			    WM8994_FLL1_FRATIO_MASK, reg);

	snd_soc_write(codec, WM8994_FLL1_CONTROL_3 + reg_offset, fll.k);

	snd_soc_update_bits(codec, WM8994_FLL1_CONTROL_4 + reg_offset,
			    WM8994_FLL1_N_MASK,
				    fll.n << WM8994_FLL1_N_SHIFT);

	snd_soc_update_bits(codec, WM8994_FLL1_CONTROL_5 + reg_offset,
			    WM8994_FLL1_REFCLK_DIV_MASK |
			    WM8994_FLL1_REFCLK_SRC_MASK,
			    (fll.clk_ref_div << WM8994_FLL1_REFCLK_DIV_SHIFT) |
			    (src - 1));

	/* Enable (with fractional mode if required) */
	if (freq_out) {
		if (fll.k)
			reg = WM8994_FLL1_ENA | WM8994_FLL1_FRAC;
		else
			reg = WM8994_FLL1_ENA;
		snd_soc_update_bits(codec, WM8994_FLL1_CONTROL_1 + reg_offset,
				    WM8994_FLL1_ENA | WM8994_FLL1_FRAC,
				    reg);
	}

	wm8994->fll[id].in = freq_in;
	wm8994->fll[id].out = freq_out;
	wm8994->fll[id].src = src;

	/* Enable any gated AIF clocks */
	snd_soc_update_bits(codec, WM8994_AIF1_CLOCKING_1,
			    WM8994_AIF1CLK_ENA, aif1);
	snd_soc_update_bits(codec, WM8994_AIF2_CLOCKING_1,
			    WM8994_AIF2CLK_ENA, aif2);

	configure_clock(codec);

	return 0;
}


static int opclk_divs[] = { 10, 20, 30, 40, 55, 60, 80, 120, 160 };

static int wm8994_set_fll(struct snd_soc_dai *dai, int id, int src,
			  unsigned int freq_in, unsigned int freq_out)
{
	return _wm8994_set_fll(dai->codec, id, src, freq_in, freq_out);
}

static int wm8994_set_dai_sysclk(struct snd_soc_dai *dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	int i;

	switch (dai->id) {
	case 1:
	case 2:
		break;

	default:
		/* AIF3 shares clocking with AIF1/2 */
		return -EINVAL;
	}

	switch (clk_id) {
	case WM8994_SYSCLK_MCLK1:
		wm8994->sysclk[dai->id - 1] = WM8994_SYSCLK_MCLK1;
		wm8994->mclk[0] = freq;
		dev_dbg(dai->dev, "AIF%d using MCLK1 at %uHz\n",
			dai->id, freq);
		break;

	case WM8994_SYSCLK_MCLK2:
		/* TODO: Set GPIO AF */
		wm8994->sysclk[dai->id - 1] = WM8994_SYSCLK_MCLK2;
		wm8994->mclk[1] = freq;
		dev_dbg(dai->dev, "AIF%d using MCLK2 at %uHz\n",
			dai->id, freq);
		break;

	case WM8994_SYSCLK_FLL1:
		wm8994->sysclk[dai->id - 1] = WM8994_SYSCLK_FLL1;
		dev_dbg(dai->dev, "AIF%d using FLL1\n", dai->id);
		break;

	case WM8994_SYSCLK_FLL2:
		wm8994->sysclk[dai->id - 1] = WM8994_SYSCLK_FLL2;
		dev_dbg(dai->dev, "AIF%d using FLL2\n", dai->id);
		break;

	case WM8994_SYSCLK_OPCLK:
		/* Special case - a division (times 10) is given and
		 * no effect on main clocking. 
		 */
		if (freq) {
			for (i = 0; i < ARRAY_SIZE(opclk_divs); i++)
				if (opclk_divs[i] == freq)
					break;
			if (i == ARRAY_SIZE(opclk_divs))
				return -EINVAL;
			snd_soc_update_bits(codec, WM8994_CLOCKING_2,
					    WM8994_OPCLK_DIV_MASK, i);
			snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_2,
					    WM8994_OPCLK_ENA, WM8994_OPCLK_ENA);
		} else {
			snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_2,
					    WM8994_OPCLK_ENA, 0);
		}

	default:
		return -EINVAL;
	}

	configure_clock(codec);

	return 0;
}

static int wm8994_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct wm8994 *control = codec->control_data;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;

	case SND_SOC_BIAS_PREPARE:
		/* VMID=2x40k */
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
				    WM8994_VMID_SEL_MASK, 0x2);
		break;

	case SND_SOC_BIAS_STANDBY:
		if (codec->dapm.bias_level == SND_SOC_BIAS_OFF) {
			pm_runtime_get_sync(codec->dev);

			switch (control->type) {
			case WM8994:
				if (wm8994->revision < 4) {
					/* Tweak DC servo and DSP
					 * configuration for improved
					 * performance. */
					snd_soc_write(codec, 0x102, 0x3);
					snd_soc_write(codec, 0x56, 0x3);
					snd_soc_write(codec, 0x817, 0);
					snd_soc_write(codec, 0x102, 0);
				}
				break;

			case WM8958:
				if (wm8994->revision == 0) {
					/* Optimise performance for rev A */
					snd_soc_write(codec, 0x102, 0x3);
					snd_soc_write(codec, 0xcb, 0x81);
					snd_soc_write(codec, 0x817, 0);
					snd_soc_write(codec, 0x102, 0);

					snd_soc_update_bits(codec,
							    WM8958_CHARGE_PUMP_2,
							    WM8958_CP_DISCH,
							    WM8958_CP_DISCH);
				}
				break;
			}

			/* Discharge LINEOUT1 & 2 */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_1,
					    WM8994_LINEOUT1_DISCH |
					    WM8994_LINEOUT2_DISCH,
					    WM8994_LINEOUT1_DISCH |
					    WM8994_LINEOUT2_DISCH);

			/* Startup bias, VMID ramp & buffer */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_2,
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    WM8994_VMID_RAMP_MASK,
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    (0x11 << WM8994_VMID_RAMP_SHIFT));

			/* Main bias enable, VMID=2x40k */
			snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
					    WM8994_BIAS_ENA |
					    WM8994_VMID_SEL_MASK,
					    WM8994_BIAS_ENA | 0x2);

			msleep(20);
		}

		/* VMID=2x500k */
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
				    WM8994_VMID_SEL_MASK, 0x4);

		break;

	case SND_SOC_BIAS_OFF:
		if (codec->dapm.bias_level == SND_SOC_BIAS_STANDBY) {
			/* Switch over to startup biases */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_2,
					    WM8994_BIAS_SRC |
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    WM8994_VMID_RAMP_MASK,
					    WM8994_BIAS_SRC |
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    (1 << WM8994_VMID_RAMP_SHIFT));

			/* Disable main biases */
			snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_1,
					    WM8994_BIAS_ENA |
					    WM8994_VMID_SEL_MASK, 0);

			/* Discharge line */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_1,
					    WM8994_LINEOUT1_DISCH |
					    WM8994_LINEOUT2_DISCH,
					    WM8994_LINEOUT1_DISCH |
					    WM8994_LINEOUT2_DISCH);

			msleep(5);

			/* Switch off startup biases */
			snd_soc_update_bits(codec, WM8994_ANTIPOP_2,
					    WM8994_BIAS_SRC |
					    WM8994_STARTUP_BIAS_ENA |
					    WM8994_VMID_BUF_ENA |
					    WM8994_VMID_RAMP_MASK, 0);

			wm8994->cur_fw = NULL;

			pm_runtime_put(codec->dev);
		}
		break;
	}
	codec->dapm.bias_level = level;
	return 0;
}

static int wm8994_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct wm8994 *control = codec->control_data;
	int ms_reg;
	int aif1_reg;
	int ms = 0;
	int aif1 = 0;

	switch (dai->id) {
	case 1:
		ms_reg = WM8994_AIF1_MASTER_SLAVE;
		aif1_reg = WM8994_AIF1_CONTROL_1;
		break;
	case 2:
		ms_reg = WM8994_AIF2_MASTER_SLAVE;
		aif1_reg = WM8994_AIF2_CONTROL_1;
		break;
	default:
		return -EINVAL;
	}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		wm8994->master = 0;
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
		aif2 |= (WM8994_AIF1_MSTR|WM8994_AIF1_LRCLK_FRC);
		wm8994->master = 1;
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		aif2 |= (WM8994_AIF1_MSTR|WM8994_AIF1_CLK_FRC) ;
		wm8994->master = 1;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		aif2 |= (WM8994_AIF1_MSTR|WM8994_AIF1_CLK_FRC| WM8994_AIF1_LRCLK_FRC);
		//aif2 |= (WM8994_AIF1_MSTR);
		wm8994->master = 1;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_DSP_B:
		aif1 |=WM8994_AIF1_LRCLK_INV;
	case SND_SOC_DAIFMT_DSP_A:
		aif1 |= 0x18;
		break;
	case SND_SOC_DAIFMT_I2S:
		aif1 |= 0x10;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		aif1 |= 0x8;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_DSP_A:
	case SND_SOC_DAIFMT_DSP_B:
		/* frame inversion not valid for DSP modes */
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			break;
		case SND_SOC_DAIFMT_IB_NF:
			aif1 |=  WM8994_AIF1_BCLK_INV;
			break;
		default:
			return -EINVAL;
		}
		break;

	case SND_SOC_DAIFMT_I2S:
	case SND_SOC_DAIFMT_RIGHT_J:
	case SND_SOC_DAIFMT_LEFT_J:
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			break;
		case SND_SOC_DAIFMT_IB_IF:
			aif1 |= WM8994_AIF1_BCLK_INV |WM8994_AIF1_LRCLK_INV;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			aif1 |=    WM8994_AIF1_BCLK_INV;
			break;
		case SND_SOC_DAIFMT_NB_IF:
			aif1 |= WM8994_AIF1_LRCLK_INV;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

<<<<<<< HEAD
	aif1 |= 0x4000;
	wm8994_write(codec,WM8994_AIF1_CONTROL_1, aif1);
	wm8994_write(codec,WM8994_AIF1_MASTER_SLAVE, aif2);
	wm8994_write( codec,WM8994_AIF1_CONTROL_2, 0x4000);
	
=======
	/* The AIF2 format configuration needs to be mirrored to AIF3
	 * on WM8958 if it's in use so just do it all the time. */
	if (control->type == WM8958 && dai->id == 2)
		snd_soc_update_bits(codec, WM8958_AIF3_CONTROL_1,
				    WM8994_AIF1_LRCLK_INV |
				    WM8958_AIF3_FMT_MASK, aif1);

	snd_soc_update_bits(codec, aif1_reg,
			    WM8994_AIF1_BCLK_INV | WM8994_AIF1_LRCLK_INV |
			    WM8994_AIF1_FMT_MASK,
			    aif1);
	snd_soc_update_bits(codec, ms_reg, WM8994_AIF1_MSTR,
			    ms);

>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
	return 0;
}

static int wm8994_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
<<<<<<< HEAD
	struct wm8994_priv *wm8994 = codec->drvdata;
	int ret, i, best, best_val, cur_val;
	unsigned int clocking1, clocking3, aif1, aif4,aif5;

	DEBUG_LOG("");

	clocking1 = wm8994_read(codec,WM8994_AIF1_BCLK);
	clocking1 &= ~ WM8994_AIF1_BCLK_DIV_MASK ;

	clocking3 = wm8994_read(codec, WM8994_AIF1_RATE);
	clocking3 &= ~(WM8994_AIF1_SR_MASK | WM8994_AIF1CLK_RATE_MASK);

	aif1 = wm8994_read(codec, WM8994_AIF1_CONTROL_1);
	aif1 &= ~WM8994_AIF1_WL_MASK;
	aif4 = wm8994_read(codec,WM8994_AIF1ADC_LRCLK);
	aif4 &= ~WM8994_AIF1ADC_LRCLK_DIR ;
	aif5 = wm8994_read(codec,WM8994_AIF1DAC_LRCLK);
	aif5 &= ~WM8994_AIF1DAC_LRCLK_DIR_MASK;

	/* What BCLK do we need? */
	wm8994->fs = params_rate(params);
	wm8994->bclk = 2 * wm8994->fs;
=======
	struct wm8994 *control = codec->control_data;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	int aif1_reg;
	int aif2_reg;
	int bclk_reg;
	int lrclk_reg;
	int rate_reg;
	int aif1 = 0;
	int aif2 = 0;
	int bclk = 0;
	int lrclk = 0;
	int rate_val = 0;
	int id = dai->id - 1;

	int i, cur_val, best_val, bclk_rate, best;

	switch (dai->id) {
	case 1:
		aif1_reg = WM8994_AIF1_CONTROL_1;
		aif2_reg = WM8994_AIF1_CONTROL_2;
		bclk_reg = WM8994_AIF1_BCLK;
		rate_reg = WM8994_AIF1_RATE;
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK ||
		    wm8994->lrclk_shared[0]) {
			lrclk_reg = WM8994_AIF1DAC_LRCLK;
		} else {
			lrclk_reg = WM8994_AIF1ADC_LRCLK;
			dev_dbg(codec->dev, "AIF1 using split LRCLK\n");
		}
		break;
	case 2:
		aif1_reg = WM8994_AIF2_CONTROL_1;
		aif2_reg = WM8994_AIF2_CONTROL_2;
		bclk_reg = WM8994_AIF2_BCLK;
		rate_reg = WM8994_AIF2_RATE;
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK ||
		    wm8994->lrclk_shared[1]) {
			lrclk_reg = WM8994_AIF2DAC_LRCLK;
		} else {
			lrclk_reg = WM8994_AIF2ADC_LRCLK;
			dev_dbg(codec->dev, "AIF2 using split LRCLK\n");
		}
		break;
	case 3:
		switch (control->type) {
		case WM8958:
			aif1_reg = WM8958_AIF3_CONTROL_1;
			break;
		default:
			return 0;
		}
	default:
		return -EINVAL;
	}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			wm8994->bclk *= 16;
			break;

		case SNDRV_PCM_FORMAT_S20_3LE:
			wm8994->bclk *= 20;
			aif1 |= (0x01<< WM8994_AIF1_WL_SHIFT);
			break;

		case SNDRV_PCM_FORMAT_S24_LE:
			wm8994->bclk *= 24;
			aif1 |= (0x10 << WM8994_AIF1_WL_SHIFT);
			break;

<<<<<<< HEAD
		case SNDRV_PCM_FORMAT_S32_LE:
			wm8994->bclk *= 32;
			aif1 |= (0x11 << WM8994_AIF1_WL_SHIFT);
			break;

		default:
			return -EINVAL;
=======
	if (params_channels(params) == 1 &&
	    (snd_soc_read(codec, aif1_reg) & 0x18) == 0x18)
		aif2 |= WM8994_AIF1_MONO;

	if (wm8994->aifclk[id] == 0) {
		dev_err(dai->dev, "AIF%dCLK not configured\n", dai->id);
		return -EINVAL;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
	}

	ret = configure_clock(codec);
	if (ret != 0)
		return ret;

	dev_dbg(codec->dev, "Target BCLK is %dHz\n", wm8994->bclk);

	/* Select nearest CLK_SYS_RATE */
	if(wm8994->fs == 8000)		// Force to select clck_sys_rate 192 on using 8KHz.
		best = 3;
	else
	{
		best = 0;
		best_val = abs((wm8994->sysclk_rate / clk_sys_rates[0].ratio) - wm8994->fs);

		for (i = 1; i < ARRAY_SIZE(clk_sys_rates); i++) {
			cur_val = abs((wm8994->sysclk_rate /clk_sys_rates[i].ratio) - wm8994->fs);

			if (cur_val < best_val) {
				best = i;
				best_val = cur_val;
				}
		}
		dev_dbg(codec->dev, "Selected CLK_SYS_RATIO of %d\n", clk_sys_rates[best].ratio);
	}

	clocking3 |= (clk_sys_rates[best].clk_sys_rate << WM8994_AIF1CLK_RATE_SHIFT);

	/* SAMPLE_RATE */
	best = 0;
	best_val = abs(wm8994->fs - sample_rates[0].rate);
	for (i = 1; i < ARRAY_SIZE(sample_rates); i++) {
		/* Closest match */
		cur_val = abs(wm8994->fs - sample_rates[i].rate);
		if (cur_val < best_val) {
			best = i;
			best_val = cur_val;
		}
	}
	dev_dbg(codec->dev, "Selected SAMPLE_RATE of %dHz\n",
		sample_rates[best].rate);

	clocking3 |= (sample_rates[best].sample_rate << WM8994_AIF1_SR_SHIFT);

	/* BCLK_DIV */
	best = 0;
	best_val = INT_MAX;
	for (i = 0; i < ARRAY_SIZE(bclk_divs); i++) {
<<<<<<< HEAD
		cur_val = ((wm8994->sysclk_rate ) / bclk_divs[i].div)
			- wm8994->bclk;
		if (cur_val < 0) /* Table is sorted */
=======
		cur_val = (wm8994->aifclk[id] * 10 / bclk_divs[i]) - bclk_rate;
		if (cur_val < 0) /* BCLK table is sorted */
			break;
		best = i;
	}
	bclk_rate = wm8994->aifclk[id] * 10 / bclk_divs[best];
	dev_dbg(dai->dev, "Using BCLK_DIV %d for actual BCLK %dHz\n",
		bclk_divs[best], bclk_rate);
	bclk |= best << WM8994_AIF1_BCLK_DIV_SHIFT;

	lrclk = bclk_rate / params_rate(params);
	dev_dbg(dai->dev, "Using LRCLK rate %d for actual LRCLK %dHz\n",
		lrclk, bclk_rate / lrclk);

	snd_soc_update_bits(codec, aif1_reg, WM8994_AIF1_WL_MASK, aif1);
	snd_soc_update_bits(codec, aif2_reg, WM8994_AIF1_MONO, aif2);
	snd_soc_update_bits(codec, bclk_reg, WM8994_AIF1_BCLK_DIV_MASK, bclk);
	snd_soc_update_bits(codec, lrclk_reg, WM8994_AIF1DAC_RATE_MASK,
			    lrclk);
	snd_soc_update_bits(codec, rate_reg, WM8994_AIF1_SR_MASK |
			    WM8994_AIF1CLK_RATE_MASK, rate_val);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		switch (dai->id) {
		case 1:
			wm8994->dac_rates[0] = params_rate(params);
			wm8994_set_retune_mobile(codec, 0);
			wm8994_set_retune_mobile(codec, 1);
			break;
		case 2:
			wm8994->dac_rates[1] = params_rate(params);
			wm8994_set_retune_mobile(codec, 2);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
			break;
		if (cur_val < best_val) {
			best = i;
			best_val = cur_val;
		}
	}
	wm8994->bclk = (wm8994->sysclk_rate ) / bclk_divs[best].div;
	dev_dbg(codec->dev, "Selected BCLK_DIV of %d for %dHz BCLK\n",
		bclk_divs[best].div, wm8994->bclk);

	clocking1 |= bclk_divs[best].bclk_div << WM8994_AIF1_BCLK_DIV_SHIFT;

	/* LRCLK is a simple fraction of BCLK */
	dev_dbg(codec->dev, "LRCLK_RATE is %d\n", wm8994->bclk / wm8994->fs);

	aif4 |= wm8994->bclk / wm8994->fs;
	aif5 |= wm8994->bclk / wm8994->fs;

#ifdef HDMI_USE_AUDIO
	if(wm8994->fs == 44100)
		wm8994_write(codec,WM8994_AIF1_BCLK,0x70); //set bclk to 32fs for 44.1kHz 16 bit playback.
#endif

//TODO...we need to set proper BCLK & LRCLK to support different frequency songs..In modifying 
//BCLK & LRCLK , its giving noisy and improper frequency sound..this has to be checked
#ifndef CONFIG_SND_S5P_WM8994_MASTER 
	//wm8994_write(codec,WM8994_AIF1_BCLK, clocking1);
	//wm8994_write(codec,WM8994_AIF1ADC_LRCLK, aif4);
	//wm8994_write(codec,WM8994_AIF1DAC_LRCLK, aif5);
#endif	
	wm8994_write(codec,WM8994_AIF1_RATE, clocking3);
	wm8994_write(codec, WM8994_AIF1_CONTROL_1, aif1);
	
	return 0;
}

<<<<<<< HEAD


static int wm8994_digital_mute(struct snd_soc_dai *codec_dai, int mute)
=======
static int wm8994_aif3_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct wm8994 *control = codec->control_data;
	int aif1_reg;
	int aif1 = 0;

	switch (dai->id) {
	case 3:
		switch (control->type) {
		case WM8958:
			aif1_reg = WM8958_AIF3_CONTROL_1;
			break;
		default:
			return 0;
		}
	default:
		return 0;
	}

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		aif1 |= 0x20;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		aif1 |= 0x40;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		aif1 |= 0x60;
		break;
	default:
		return -EINVAL;
	}

	return snd_soc_update_bits(codec, aif1_reg, WM8994_AIF1_WL_MASK, aif1);
}

static int wm8994_aif_mute(struct snd_soc_dai *codec_dai, int mute)
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
{
//Implementation has to be tested properly.
	#if 0
	unsigned int reg;
	struct snd_soc_codec *codec = codec_dai->codec;
	reg = wm8994_read(codec, WM8994_DAC_SOFTMUTE);
	reg &= ~WM8994_DAC_SOFTMUTEMODE_MASK;

	if (mute)
		reg |= WM8994_DAC_SOFTMUTEMODE;
	else
		reg &= ~WM8994_DAC_SOFTMUTEMODE;

	wm8994_write(codec, WM8994_DAC_SOFTMUTEMODE_MASK, reg);
	#endif

#if AUDIO_COMMON_DEBUG		// for removing warning on compliing.
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8994_priv *wm8994 = codec->drvdata;
#endif

	DEBUG_LOG("Mute =[%d], current Path = [%d]\n", mute, wm8994->cur_path);

	return 0;
}

static bool play_en_dis = 0;
static bool rec_en_dis  = 0;

static int wm8994_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *codec_dai)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8994_priv *wm8994 = codec->drvdata;
#if 1 
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
	play_en_dis = 1;
	}
	else{
	rec_en_dis = 1;
	}
	
	
	if(wm8994->testmode_config_flag)
	{
		DEBUG_LOG_ERR("Testmode is activated!! Skip statup sequence!!");
		return 0;
	}

	if(wm8994->power_state == CODEC_OFF)
	{
		wm8994->power_state = CODEC_ON;
		DEBUG_LOG("Turn on codec!! Power state =[%d]", wm8994->power_state);

		// For initialize codec.	
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x3 << WM8994_VMID_SEL_SHIFT | WM8994_BIAS_ENA);
		msleep(10);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

		wm8994_write(codec,WM8994_OVERSAMPLING, 0x0000);
	}
	else
		DEBUG_LOG("Already turned on codec!!");
#endif
	return 0;
}

static void wm8994_shutdown(struct snd_pcm_substream *substream, struct snd_soc_dai *codec_dai)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8994_priv *wm8994 = codec->drvdata;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK){
	play_en_dis = 0;
	DEBUG_LOG("..inside..%s..for PLAYBACK_STREAM!!",__func__);
		if(wm8994->cur_path != OFF){
                                wm8994_disable_playback_path(codec, wm8994->cur_path);
                                wm8994->cur_path = OFF;
                }
	}
	else{
	rec_en_dis = 0;
	DEBUG_LOG("..inside..%s..for CAPTURE_STREAM!!",__func__);
		if(wm8994->mic_state == MIC_NO_USE && wm8994->rec_path != MIC_OFF)
                                wm8994_disable_rec_path(codec, wm8994->rec_path);
	}

	if(wm8994->call_state == DISCONNECT)
	{

		if(wm8994->testmode_config_flag)
		{
			DEBUG_LOG_ERR("Testmode is activated!! Don't shutdown(reset) sequence!!");
			return;
		}

		if(wm8994->fmradio_path != FMR_OFF)
                                wm8994_disable_fmradio_path(codec, FMR_OFF);

		if(!play_en_dis && !rec_en_dis){
			DEBUG_LOG("Turn off Codec!!");
                        wm8994->power_state = CODEC_OFF;
                        wm8994_write(codec,WM8994_SOFTWARE_RESET, 0x0000 );
#if defined ATTACH_ADDITINAL_PCM_DRIVER
			vtCallActive = 0;
#endif
                }
		

#if 0
		if(wm8994->mic_state == MIC_NO_USE)
		{
			DEBUG_LOG("Turn off Codec!!");
			wm8994->power_state = CODEC_OFF;
			wm8994_write(codec,WM8994_SOFTWARE_RESET, 0x0000 );
		}
		else
		{
			if(wm8994->cur_path != OFF)
			{
				wm8994_disable_playback_path(codec, wm8994->cur_path);
				wm8994->cur_path = OFF;
			}
			
			if(wm8994->fmradio_path != FMR_OFF)
				wm8994_disable_fmradio_path(codec, FMR_OFF);
			
			if(wm8994->mic_state == MIC_NO_USE && wm8994->rec_path != MIC_OFF)
				wm8994_disable_rec_path(codec, wm8994->rec_path);
		}

#endif


	}
	else
		DEBUG_LOG("Preserve codec state for call[%d].", wm8994->call_state);

		DEBUG_LOG("exiting ...%s...",__func__);
		
}

#if defined ATTACH_ADDITINAL_PCM_DRIVER
static int wm8994_pcm_startup(struct snd_pcm_substream *substream, struct snd_soc_dai *codec_dai)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8994_priv *wm8994 = codec->drvdata;
 
	int reg;
 
	 if(vtCallActive == 0)
	 {
		 vtCallActive = 1;
		wm8994->cur_path = OFF;
		//wm8994->codec_state = DEACTIVE;
		wm8994->power_state = CODEC_OFF;
		
		DEBUG_LOG("Turn on codec!! Power state =[%d]", wm8994->power_state);

		// For initialize codec.	
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x3 << WM8994_VMID_SEL_SHIFT | WM8994_BIAS_ENA);
		msleep(50);
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

		wm8994_write(codec,WM8994_OVERSAMPLING, 0x0000);

		wm8994_write( codec,WM8994_FLL1_CONTROL_2, 0x0700 );
		wm8994_write( codec,WM8994_FLL1_CONTROL_3, 0x86C2 );
		wm8994_write( codec,WM8994_FLL1_CONTROL_5, 0x0C88 );
		wm8994_write( codec,WM8994_FLL1_CONTROL_4, 0x00E0 );
		wm8994_write(codec, WM8994_FLL1_CONTROL_1, WM8994_FLL1_FRACN_ENA |WM8994_FLL1_ENA);

		reg = wm8994_read(codec,WM8994_AIF1_CLOCKING_1);
		reg |= (WM8994_AIF1CLK_SRC_FLL1 | WM8994_AIF1CLK_ENA); //enable the clocks
		wm8994_write(codec, WM8994_AIF1_CLOCKING_1, reg);
	
		//Enable clocks to the Audio core and sysclk of wm8994	
		reg = wm8994_read(codec, WM8994_CLOCKING_1 );
		reg &= ~(WM8994_SYSCLK_SRC_MASK | WM8994_DSP_FSINTCLK_ENA_MASK|WM8994_DSP_FS1CLK_ENA_MASK);
		reg |= (WM8994_DSP_FS1CLK_ENA | WM8994_DSP_FSINTCLK_ENA);
		wm8994_write(codec,WM8994_CLOCKING_1 ,reg);		
	}
	else
		DEBUG_LOG("Already turned on codec!!");

<<<<<<< HEAD
	return 0;
=======
	return snd_soc_update_bits(codec, reg, mask, val);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
}
#endif

static struct snd_soc_device *wm8994_socdev;
static struct snd_soc_codec  *wm8994_codec;

#define WM8994_RATES SNDRV_PCM_RATE_8000_96000
#define WM8994_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)
<<<<<<< HEAD
static struct snd_soc_dai_ops wm8994_ops = {
			 .startup = wm8994_startup,
			 .shutdown = wm8994_shutdown,
			 .set_sysclk = wm8994_set_sysclk,
			 .set_fmt = wm8994_set_dai_fmt,
			 .hw_params = wm8994_hw_params,
			 .digital_mute = wm8994_digital_mute,
};

struct snd_soc_dai wm8994_dai = {
	
		.name = "WM8994 PAIFRX",
		.playback = {
			.stream_name = "Playback",
			.channels_min = 1,
			.channels_max = 6,
=======

static struct snd_soc_dai_ops wm8994_aif1_dai_ops = {
	.set_sysclk	= wm8994_set_dai_sysclk,
	.set_fmt	= wm8994_set_dai_fmt,
	.hw_params	= wm8994_hw_params,
	.digital_mute	= wm8994_aif_mute,
	.set_pll	= wm8994_set_fll,
	.set_tristate	= wm8994_set_tristate,
};

static struct snd_soc_dai_ops wm8994_aif2_dai_ops = {
	.set_sysclk	= wm8994_set_dai_sysclk,
	.set_fmt	= wm8994_set_dai_fmt,
	.hw_params	= wm8994_hw_params,
	.digital_mute   = wm8994_aif_mute,
	.set_pll	= wm8994_set_fll,
	.set_tristate	= wm8994_set_tristate,
};

static struct snd_soc_dai_ops wm8994_aif3_dai_ops = {
	.hw_params	= wm8994_aif3_hw_params,
	.set_tristate	= wm8994_set_tristate,
};

static struct snd_soc_dai_driver wm8994_dai[] = {
	{
		.name = "wm8994-aif1",
		.id = 1,
		.playback = {
			.stream_name = "AIF1 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = WM8994_RATES,
			.formats = WM8994_FORMATS,
		},
		.capture = {
			.stream_name = "AIF1 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = WM8994_RATES,
			.formats = WM8994_FORMATS,
		 },
		.ops = &wm8994_aif1_dai_ops,
	},
	{
		.name = "wm8994-aif2",
		.id = 2,
		.playback = {
			.stream_name = "AIF2 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = WM8994_RATES,
			.formats = WM8994_FORMATS,
		},
		.capture = {
			.stream_name = "AIF2 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = WM8994_RATES,
			.formats = WM8994_FORMATS,
		},
		.ops = &wm8994_aif2_dai_ops,
	},
	{
		.name = "wm8994-aif3",
		.id = 3,
		.playback = {
			.stream_name = "AIF3 Playback",
			.channels_min = 1,
			.channels_max = 2,
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
			.rates = WM8994_RATES,
			.formats = WM8994_FORMATS,
		},
		.capture = {
<<<<<<< HEAD
			.stream_name = "Capture",
=======
			.stream_name = "AIF3 Capture",
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
			.channels_min = 1,
			.channels_max = 2,
			.rates = WM8994_RATES,
			.formats = WM8994_FORMATS,
		},

		.ops = &wm8994_ops,
};

<<<<<<< HEAD
#if defined ATTACH_ADDITINAL_PCM_DRIVER
static struct snd_soc_dai_ops wm8994_pcm_ops = {
			 .startup = wm8994_pcm_startup,
};
=======
#ifdef CONFIG_PM
static int wm8994_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994 *control = codec->control_data;
	int i, ret;

	switch (control->type) {
	case WM8994:
		snd_soc_update_bits(codec, WM8994_MICBIAS, WM8994_MICD_ENA, 0);
		break;
	case WM8958:
		snd_soc_update_bits(codec, WM8958_MIC_DETECT_1,
				    WM8958_MICD_ENA, 0);
		break;
	}

	for (i = 0; i < ARRAY_SIZE(wm8994->fll); i++) {
		memcpy(&wm8994->fll_suspend[i], &wm8994->fll[i],
		       sizeof(struct wm8994_fll_config));
		ret = _wm8994_set_fll(codec, i + 1, 0, 0, 0);
		if (ret < 0)
			dev_warn(codec->dev, "Failed to stop FLL%d: %d\n",
				 i + 1, ret);
	}

	wm8994_set_bias_level(codec, SND_SOC_BIAS_OFF);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

struct snd_soc_dai wm8994_pcm_dai = {

                .name = "WM8994 PCM",
                .playback = {
                        .stream_name = "Playback",
                        .channels_min = 1,
                        .channels_max = 6,
                        .rates = WM8994_RATES,
                        .formats = WM8994_FORMATS,
                },
                .capture = {
                        .stream_name = "Capture",
                        .channels_min = 1,
                        .channels_max = 2,
                        .rates = WM8994_RATES,
                        .formats = WM8994_FORMATS,
                },
		.ops = &wm8994_pcm_ops,
};
EXPORT_SYMBOL_GPL(wm8994_pcm_dai);
#endif

<<<<<<< HEAD
/*
 * initialise the WM8994 driver
 * register the mixer and dsp interfaces with the kernel
 */
//static int wm8994_init(struct snd_soc_device *socdev)
static int wm8994_init(struct wm8994_priv *wm8994_private)
{
	struct snd_soc_codec *codec = &wm8994_private->codec;
	struct wm8994_priv *wm8994 ;
	int ret = 0;
	DEBUG_LOG("");
	 codec->drvdata = kzalloc(sizeof(struct wm8994_priv), GFP_KERNEL);
	 if (codec->drvdata == NULL)
	    return -ENOMEM;

	wm8994 = codec->drvdata;

	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);
	codec->name = "WM8994";
	codec->owner = THIS_MODULE;
	codec->read = wm8994_read;
	codec->write = wm8994_write;
	codec->set_bias_level = wm8994_set_bias_level;
	codec->dai = &wm8994_dai;
	codec->num_dai = 1;//ARRAY_SIZE(wm8994_dai);
	wm8994->universal_playback_path = universal_wm8994_playback_paths;
	wm8994->universal_voicecall_path = universal_wm8994_voicecall_paths;
	wm8994->universal_mic_path = universal_wm8994_mic_paths;
	wm8994->cur_path = OFF;
	wm8994->rec_path = MIC_OFF;
	wm8994->call_state = DISCONNECT;
	wm8994->fmradio_path = FMR_OFF;
	wm8994->testmode_config_flag = 0;
	wm8994->power_state = CODEC_OFF;
	wm8994->mic_state = MIC_NO_USE;

	wm8994_write(codec,WM8994_SOFTWARE_RESET, 0x0000);

	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x3 << WM8994_VMID_SEL_SHIFT | WM8994_BIAS_ENA);
	//msleep(10);//commented as sleep not required in SLSI platform
	wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

	wm8994->hw_version = wm8994_read(codec, 0x100);	// Read Wm8994 version.

	wm8994_socdev->card->codec = codec;
	wm8994_codec = codec;
	/* register pcms */
	ret = snd_soc_new_pcms(wm8994_socdev, SNDRV_DEFAULT_IDX1,
			       SNDRV_DEFAULT_STR1);
	if (ret < 0) {
		DEBUG_LOG_ERR("failed to create pcms\n");
		goto pcm_err;
	}
=======
static int wm8994_resume(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994 *control = codec->control_data;
	int i, ret;
	unsigned int val, mask;

	if (wm8994->revision < 4) {
		/* force a HW read */
		val = wm8994_reg_read(codec->control_data,
				      WM8994_POWER_MANAGEMENT_5);

		/* modify the cache only */
		codec->cache_only = 1;
		mask =  WM8994_DAC1R_ENA | WM8994_DAC1L_ENA |
			WM8994_DAC2R_ENA | WM8994_DAC2L_ENA;
		val &= mask;
		snd_soc_update_bits(codec, WM8994_POWER_MANAGEMENT_5,
				    mask, val);
		codec->cache_only = 0;
	}

	/* Restore the registers */
	ret = snd_soc_cache_sync(codec);
	if (ret != 0)
		dev_err(codec->dev, "Failed to sync cache: %d\n", ret);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	wm8994_add_controls(codec);
	wm8994_add_widgets(codec);

<<<<<<< HEAD
	return ret;

card_err:
	snd_soc_free_pcms(wm8994_socdev);
	snd_soc_dapm_free(wm8994_socdev);
pcm_err:
	//kfree(codec->reg_cache);
	return ret;
=======
	for (i = 0; i < ARRAY_SIZE(wm8994->fll); i++) {
		if (!wm8994->fll_suspend[i].out)
			continue;

		ret = _wm8994_set_fll(codec, i + 1,
				     wm8994->fll_suspend[i].src,
				     wm8994->fll_suspend[i].in,
				     wm8994->fll_suspend[i].out);
		if (ret < 0)
			dev_warn(codec->dev, "Failed to restore FLL%d: %d\n",
				 i + 1, ret);
	}

	switch (control->type) {
	case WM8994:
		if (wm8994->micdet[0].jack || wm8994->micdet[1].jack)
			snd_soc_update_bits(codec, WM8994_MICBIAS,
					    WM8994_MICD_ENA, WM8994_MICD_ENA);
		break;
	case WM8958:
		if (wm8994->jack_cb)
			snd_soc_update_bits(codec, WM8958_MIC_DETECT_1,
					    WM8958_MICD_ENA, WM8958_MICD_ENA);
		break;
	}

	return 0;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
}

#if defined ATTACH_ADDITINAL_PCM_DRIVER
static struct snd_soc_device *wm8994_pcm_socdev;
static struct snd_soc_codec  *wm8994_pcm_codec;

static int wm8994_pcm_init(struct wm8994_priv *wm8994_private)
{
<<<<<<< HEAD
	struct snd_soc_codec *codec = &wm8994_private->codec;
        struct wm8994_priv *wm8994;
        int ret = 0, val;

        DEBUG_LOG("");
	 codec->drvdata = kzalloc(sizeof(struct wm8994_priv), GFP_KERNEL);
	 if (codec->drvdata == NULL)
	    return -ENOMEM;
=======
	struct snd_soc_codec *codec = wm8994->codec;
	struct wm8994_pdata *pdata = wm8994->pdata;
	struct snd_kcontrol_new controls[] = {
		SOC_ENUM_EXT("AIF1.1 EQ Mode",
			     wm8994->retune_mobile_enum,
			     wm8994_get_retune_mobile_enum,
			     wm8994_put_retune_mobile_enum),
		SOC_ENUM_EXT("AIF1.2 EQ Mode",
			     wm8994->retune_mobile_enum,
			     wm8994_get_retune_mobile_enum,
			     wm8994_put_retune_mobile_enum),
		SOC_ENUM_EXT("AIF2 EQ Mode",
			     wm8994->retune_mobile_enum,
			     wm8994_get_retune_mobile_enum,
			     wm8994_put_retune_mobile_enum),
	};
	int ret, i, j;
	const char **t;

	/* We need an array of texts for the enum API but the number
	 * of texts is likely to be less than the number of
	 * configurations due to the sample rate dependency of the
	 * configurations. */
	wm8994->num_retune_mobile_texts = 0;
	wm8994->retune_mobile_texts = NULL;
	for (i = 0; i < pdata->num_retune_mobile_cfgs; i++) {
		for (j = 0; j < wm8994->num_retune_mobile_texts; j++) {
			if (strcmp(pdata->retune_mobile_cfgs[i].name,
				   wm8994->retune_mobile_texts[j]) == 0)
				break;
		}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	wm8994 = codec->drvdata;

	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);
        codec->name = "WM8994";
        codec->owner = THIS_MODULE;
        codec->read = wm8994_read;
        codec->write = wm8994_write;
        codec->set_bias_level = wm8994_set_bias_level;
        codec->dai = &wm8994_pcm_dai;
        codec->num_dai = 1;//ARRAY_SIZE(wm8994_pcm_dai);
        wm8994->universal_voicecall_path = universal_wm8994_voicecall_paths;
        wm8994->cur_path = OFF;

        val = wm8994_read(codec,WM8994_POWER_MANAGEMENT_1 );
        val &= ~(WM8994_BIAS_ENA_MASK | WM8994_VMID_SEL_MASK);
        val |= (WM8994_BIAS_ENA | WM8994_VMID_SEL_NORMAL);
        ret = wm8994_write(codec,WM8994_POWER_MANAGEMENT_1,val);

	wm8994->hw_version = wm8994_read(codec, 0x100);	// Read Wm8994 version.

	if(ret)
		printk("first wm8994_write failed in %s..\n",__func__);

	wm8994_pcm_socdev->card->codec = codec;
	wm8994_pcm_codec = codec;

#if 0
	ret = snd_soc_new_pcms(socdev, 1,"wm8994-pcm");
#else
	ret = snd_soc_new_pcms(wm8994_pcm_socdev,  1,"wm8994-pcm");
#endif
        if (ret < 0) {
                printk(KERN_ERR "wm8994: failed to create pcms\n");
                goto pcm_err;
        }

       wm8994_add_controls(codec);
       // wm8994_add_widgets(codec);

        ret = snd_soc_init_card(wm8994_pcm_socdev);
        if (ret < 0) {
                printk(KERN_ERR "wm8994: failed to register card\n");
                goto card_err;
        }
        return ret;

card_err:
        snd_soc_free_pcms(wm8994_pcm_socdev);
       // snd_soc_dapm_free(socdev);
pcm_err:
        kfree(codec->reg_cache);
	return ret;
}
#endif

/* If the i2c layer weren't so broken, we could pass this kind of data
   around */

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)

/*
 * WM8994 2 wire address is determined by GPIO5
 * state during powerup.
 *    low  = 0x1a
 *    high = 0x1b
 */
static void * control_data1;

static int wm8994_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct snd_soc_codec *codec ;
	struct wm8994_priv* wm8994_priv;
	int ret;

<<<<<<< HEAD
	DEBUG_LOG("");
#if 1
	 wm8994_priv = kzalloc(sizeof(struct wm8994_priv), GFP_KERNEL);
	 if (wm8994_priv == NULL)
	    return -ENOMEM;
#endif
	codec = &wm8994_priv->codec;
	#ifdef PM_DEBUG
	pm_codec = codec;
	#endif

	codec->hw_write = (hw_write_t)i2c_master_send; 
	i2c_set_clientdata(i2c, wm8994_priv);
	codec->control_data = i2c;
	codec->dev = &i2c->dev;
	control_data1 = i2c;
	ret = wm8994_init(wm8994_priv);
	if (ret < 0)
		dev_err(&i2c->dev, "failed to initialize WM8994\n");
	return ret;
=======
	ret = snd_soc_add_controls(wm8994->codec, controls,
				   ARRAY_SIZE(controls));
	if (ret != 0)
		dev_err(wm8994->codec->dev,
			"Failed to add ReTune Mobile controls: %d\n", ret);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
}

static int wm8994_i2c_remove(struct i2c_client *client)
{
<<<<<<< HEAD
	struct wm8994_priv* wm8994_priv = i2c_get_clientdata(client);
	kfree(wm8994_priv);
	return 0;
}
=======
	struct snd_soc_codec *codec = wm8994->codec;
	struct wm8994_pdata *pdata = wm8994->pdata;
	int ret, i;

	if (!pdata)
		return;

	wm_hubs_handle_analogue_pdata(codec, pdata->lineout1_diff,
				      pdata->lineout2_diff,
				      pdata->lineout1fb,
				      pdata->lineout2fb,
				      pdata->jd_scthr,
				      pdata->jd_thr,
				      pdata->micbias1_lvl,
				      pdata->micbias2_lvl);

	dev_dbg(codec->dev, "%d DRC configurations\n", pdata->num_drc_cfgs);

	if (pdata->num_drc_cfgs) {
		struct snd_kcontrol_new controls[] = {
			SOC_ENUM_EXT("AIF1DRC1 Mode", wm8994->drc_enum,
				     wm8994_get_drc_enum, wm8994_put_drc_enum),
			SOC_ENUM_EXT("AIF1DRC2 Mode", wm8994->drc_enum,
				     wm8994_get_drc_enum, wm8994_put_drc_enum),
			SOC_ENUM_EXT("AIF2DRC Mode", wm8994->drc_enum,
				     wm8994_get_drc_enum, wm8994_put_drc_enum),
		};

		/* We need an array of texts for the enum API */
		wm8994->drc_texts = kmalloc(sizeof(char *)
					    * pdata->num_drc_cfgs, GFP_KERNEL);
		if (!wm8994->drc_texts) {
			dev_err(wm8994->codec->dev,
				"Failed to allocate %d DRC config texts\n",
				pdata->num_drc_cfgs);
			return;
		}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

static const struct i2c_device_id wm8994_i2c_id[] = {
	{ "wm8994", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm8994_i2c_id);

static struct i2c_driver wm8994_i2c_driver = {
	.driver = {
		.name = "WM8994 I2C Codec",
		.owner = THIS_MODULE,
	},
	.probe =    wm8994_i2c_probe,
	.remove =   wm8994_i2c_remove,
	.id_table = wm8994_i2c_id,
};

<<<<<<< HEAD
struct i2c_board_info info;
struct i2c_adapter *adapter;
struct i2c_client *client;
=======
		ret = snd_soc_add_controls(wm8994->codec, controls,
					   ARRAY_SIZE(controls));
		if (ret != 0)
			dev_err(wm8994->codec->dev,
				"Failed to add DRC mode controls: %d\n", ret);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

static int wm8994_add_i2c_device(struct platform_device *pdev,
				 const struct wm8994_setup_data *setup)
{
	int ret;

	ret = i2c_add_driver(&wm8994_i2c_driver);
	if (ret != 0) {
		dev_err(&pdev->dev, "can't add i2c driver\n");
		return ret;
	}
	
	return 0;

<<<<<<< HEAD
err_driver:
	i2c_del_driver(&wm8994_i2c_driver);
	return -ENODEV;
}
#endif

static int wm8994_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct wm8994_setup_data *setup;
	struct snd_soc_codec *codec;
	struct wm8994_priv *wm8994;
	int ret = 0;

	pr_info("WM8994 Audio Codec %s\n", WM8994_VERSION);

	/* Board Specific Function */
	audio_init();
	audio_power(1);
	msleep(10);

	setup = socdev->codec_data;
	wm8994_socdev = socdev;
#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
	if (setup->i2c_address) {
		ret = wm8994_add_i2c_device(pdev, setup);
	}
#else
		/* Add other interfaces here */
#endif
	return ret;
}

/* power down chip */
static int wm8994_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = wm8994_codec;

	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
	i2c_unregister_device(codec->control_data);
	i2c_del_driver(&wm8994_i2c_driver);
#endif
=======
	if (pdata->num_retune_mobile_cfgs)
		wm8994_handle_retune_mobile_pdata(wm8994);
	else
		snd_soc_add_controls(wm8994->codec, wm8994_eq_controls,
				     ARRAY_SIZE(wm8994_eq_controls));

	for (i = 0; i < ARRAY_SIZE(pdata->micbias); i++) {
		if (pdata->micbias[i]) {
			snd_soc_write(codec, WM8958_MICBIAS1 + i,
				pdata->micbias[i] & 0xffff);
		}
	}
}

/**
 * wm8994_mic_detect - Enable microphone detection via the WM8994 IRQ
 *
 * @codec:   WM8994 codec
 * @jack:    jack to report detection events on
 * @micbias: microphone bias to detect on
 * @det:     value to report for presence detection
 * @shrt:    value to report for short detection
 *
 * Enable microphone detection via IRQ on the WM8994.  If GPIOs are
 * being used to bring out signals to the processor then only platform
 * data configuration is needed for WM8994 and processor GPIOs should
 * be configured using snd_soc_jack_add_gpios() instead.
 *
 * Configuration of detection levels is available via the micbias1_lvl
 * and micbias2_lvl platform data members.
 */
int wm8994_mic_detect(struct snd_soc_codec *codec, struct snd_soc_jack *jack,
		      int micbias, int det, int shrt)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994_micdet *micdet;
	struct wm8994 *control = codec->control_data;
	int reg;

	if (control->type != WM8994)
		return -EINVAL;

	switch (micbias) {
	case 1:
		micdet = &wm8994->micdet[0];
		break;
	case 2:
		micdet = &wm8994->micdet[1];
		break;
	default:
		return -EINVAL;
	}	

	dev_dbg(codec->dev, "Configuring microphone detection on %d: %x %x\n",
		micbias, det, shrt);

	/* Store the configuration */
	micdet->jack = jack;
	micdet->det = det;
	micdet->shrt = shrt;

	/* If either of the jacks is set up then enable detection */
	if (wm8994->micdet[0].jack || wm8994->micdet[1].jack)
		reg = WM8994_MICD_ENA;
	else 
		reg = 0;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	kfree(codec->drvdata);
	//kfree(codec);

	return 0;
}

#if defined ATTACH_ADDITINAL_PCM_DRIVER
static int wm8994_pcm_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
        struct snd_soc_device *socdev = platform_get_drvdata(pdev);
        struct wm8994_setup_data *setup;
        struct snd_soc_codec *codec;
        struct wm8994_priv *wm8994;
        int ret = 0;

        pr_info("WM8994 Audio Codec %s\n", WM8994_VERSION);

#if 1
        wm8994 = kzalloc(sizeof(struct wm8994_priv), GFP_KERNEL);
	 if (wm8994 == NULL)
                return -ENOMEM;
#endif
	//codec = &wm8994_priv->codec;
	codec = &wm8994->codec;

	setup = socdev->codec_data;
	wm8994_pcm_socdev = socdev;
=======
	struct wm8994_priv *priv = data;
	struct snd_soc_codec *codec = priv->codec;
	int reg;
	int report;

#ifndef CONFIG_SND_SOC_WM8994_MODULE
	trace_snd_soc_jack_irq(dev_name(codec->dev));
#endif

	reg = snd_soc_read(codec, WM8994_INTERRUPT_RAW_STATUS_2);
	if (reg < 0) {
		dev_err(codec->dev, "Failed to read microphone status: %d\n",
			reg);
		return IRQ_HANDLED;
	}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
        if (setup->i2c_address) {
                   codec->hw_write = (hw_write_t)i2c_master_send;
		codec->control_data = control_data1;
		wm8994_pcm_init(wm8994);
                //ret = wm8994_add_i2c_device(pdev, setup);
        }
#else
                /* Add other interfaces here */
#endif
        return ret;
}

<<<<<<< HEAD
/* power down chip */
static int wm8994_pcm_remove(struct platform_device *pdev)
{
        struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = wm8994_pcm_codec;

        snd_soc_free_pcms(socdev);
        snd_soc_dapm_free(socdev);

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)	// It's executed by I2S
      //  i2c_unregister_device(codec->control_data);
      //  i2c_del_driver(&wm8994_i2c_driver);
#endif

        kfree(codec->drvdata);
        kfree(codec);

        return 0;
}
#endif

#ifdef CONFIG_PM
static int wm8994_suspend(struct platform_device *pdev,pm_message_t msg )
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
        struct snd_soc_codec *codec = wm8994_codec;
        struct wm8994_priv *wm8994 = codec->drvdata;
=======
/* Default microphone detection handler for WM8958 - the user can
 * override this if they wish.
 */
static void wm8958_default_micdet(u16 status, void *data)
{
	struct snd_soc_codec *codec = data;
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	int report = 0;

	/* If nothing present then clear our statuses */
	if (!(status & WM8958_MICD_STS))
		goto done;

	report = SND_JACK_MICROPHONE;

	/* Everything else is buttons; just assign slots */
	if (status & 0x1c0)
		report |= SND_JACK_BTN_0;

done:
	snd_soc_jack_report(wm8994->micdet[0].jack, report,
			    SND_JACK_BTN_0 | SND_JACK_MICROPHONE);
}

/**
 * wm8958_mic_detect - Enable microphone detection via the WM8958 IRQ
 *
 * @codec:   WM8958 codec
 * @jack:    jack to report detection events on
 *
 * Enable microphone detection functionality for the WM8958.  By
 * default simple detection which supports the detection of up to 6
 * buttons plus video and microphone functionality is supported.
 *
 * The WM8958 has an advanced jack detection facility which is able to
 * support complex accessory detection, especially when used in
 * conjunction with external circuitry.  In order to provide maximum
 * flexiblity a callback is provided which allows a completely custom
 * detection algorithm.
 */
int wm8958_mic_detect(struct snd_soc_codec *codec, struct snd_soc_jack *jack,
		      wm8958_micdet_cb cb, void *cb_data)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994 *control = codec->control_data;

	if (control->type != WM8958)
		return -EINVAL;

	if (jack) {
		if (!cb) {
			dev_dbg(codec->dev, "Using default micdet callback\n");
			cb = wm8958_default_micdet;
			cb_data = codec;
		}

		wm8994->micdet[0].jack = jack;
		wm8994->jack_cb = cb;
		wm8994->jack_cb_data = cb_data;

		snd_soc_update_bits(codec, WM8958_MIC_DETECT_1,
				    WM8958_MICD_ENA, WM8958_MICD_ENA);
	} else {
		snd_soc_update_bits(codec, WM8958_MIC_DETECT_1,
				    WM8958_MICD_ENA, 0);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(wm8958_mic_detect);

static irqreturn_t wm8958_mic_irq(int irq, void *data)
{
	struct wm8994_priv *wm8994 = data;
	struct snd_soc_codec *codec = wm8994->codec;
	int reg;

	reg = snd_soc_read(codec, WM8958_MIC_DETECT_3);
	if (reg < 0) {
		dev_err(codec->dev, "Failed to read mic detect status: %d\n",
			reg);
		return IRQ_NONE;
	}

	if (!(reg & WM8958_MICD_VALID)) {
		dev_dbg(codec->dev, "Mic detect data not valid\n");
		goto out;
	}

#ifndef CONFIG_SND_SOC_WM8994_MODULE
	trace_snd_soc_jack_irq(dev_name(codec->dev));
#endif

	if (wm8994->jack_cb)
		wm8994->jack_cb(reg, wm8994->jack_cb_data);
	else
		dev_warn(codec->dev, "Accessory detection with no callback\n");

out:
	return IRQ_HANDLED;
}

static int wm8994_codec_probe(struct snd_soc_codec *codec)
{
	struct wm8994 *control;
	struct wm8994_priv *wm8994;
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret, i;

	codec->control_data = dev_get_drvdata(codec->dev->parent);
	control = codec->control_data;

	wm8994 = kzalloc(sizeof(struct wm8994_priv), GFP_KERNEL);
	if (wm8994 == NULL)
		return -ENOMEM;
	snd_soc_codec_set_drvdata(codec, wm8994);

	wm8994->pdata = dev_get_platdata(codec->dev->parent);
	wm8994->codec = codec;

	if (wm8994->pdata && wm8994->pdata->micdet_irq)
		wm8994->micdet_irq = wm8994->pdata->micdet_irq;
	else if (wm8994->pdata && wm8994->pdata->irq_base)
		wm8994->micdet_irq = wm8994->pdata->irq_base +
				     WM8994_IRQ_MIC1_DET;

	pm_runtime_enable(codec->dev);
	pm_runtime_resume(codec->dev);

	/* Read our current status back from the chip - we don't want to
	 * reset as this may interfere with the GPIO or LDO operation. */
	for (i = 0; i < WM8994_CACHE_SIZE; i++) {
		if (!wm8994_readable(codec, i) || wm8994_volatile(codec, i))
			continue;

		ret = wm8994_reg_read(codec->control_data, i);
		if (ret <= 0)
			continue;

		ret = snd_soc_cache_write(codec, i, ret);
		if (ret != 0) {
			dev_err(codec->dev,
				"Failed to initialise cache for 0x%x: %d\n",
				i, ret);
			goto err;
		}
	}

	/* Set revision-specific configuration */
	wm8994->revision = snd_soc_read(codec, WM8994_CHIP_REVISION);
	switch (control->type) {
	case WM8994:
		switch (wm8994->revision) {
		case 2:
		case 3:
			wm8994->hubs.dcs_codes = -5;
			wm8994->hubs.hp_startup_mode = 1;
			wm8994->hubs.dcs_readback_mode = 1;
			break;
		default:
			wm8994->hubs.dcs_readback_mode = 1;
			break;
		}

	case WM8958:
		wm8994->hubs.dcs_readback_mode = 1;
		break;

	default:
		break;
	}

	switch (control->type) {
	case WM8994:
		if (wm8994->micdet_irq) {
			ret = request_threaded_irq(wm8994->micdet_irq, NULL,
						   wm8994_mic_irq,
						   IRQF_TRIGGER_RISING,
						   "Mic1 detect",
						   wm8994);
			if (ret != 0)
				dev_warn(codec->dev,
					 "Failed to request Mic1 detect IRQ: %d\n",
					 ret);
		}

		ret = wm8994_request_irq(codec->control_data,
					 WM8994_IRQ_MIC1_SHRT,
					 wm8994_mic_irq, "Mic 1 short",
					 wm8994);
		if (ret != 0)
			dev_warn(codec->dev,
				 "Failed to request Mic1 short IRQ: %d\n",
				 ret);

		ret = wm8994_request_irq(codec->control_data,
					 WM8994_IRQ_MIC2_DET,
					 wm8994_mic_irq, "Mic 2 detect",
					 wm8994);
		if (ret != 0)
			dev_warn(codec->dev,
				 "Failed to request Mic2 detect IRQ: %d\n",
				 ret);

		ret = wm8994_request_irq(codec->control_data,
					 WM8994_IRQ_MIC2_SHRT,
					 wm8994_mic_irq, "Mic 2 short",
					 wm8994);
		if (ret != 0)
			dev_warn(codec->dev,
				 "Failed to request Mic2 short IRQ: %d\n",
				 ret);
		break;

	case WM8958:
		if (wm8994->micdet_irq) {
			ret = request_threaded_irq(wm8994->micdet_irq, NULL,
						   wm8958_mic_irq,
						   IRQF_TRIGGER_RISING,
						   "Mic detect",
						   wm8994);
			if (ret != 0)
				dev_warn(codec->dev,
					 "Failed to request Mic detect IRQ: %d\n",
					 ret);
		}
	}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	DEBUG_LOG("%s..",__func__);
	
	if(wm8994->testmode_config_flag)
	{
		DEBUG_LOG_ERR("Testmode is activated!! Skip suspend sequence!!");
		return 0;
	}

	if(wm8994->call_state == DISCONNECT && wm8994->cur_path == OFF )
	{
		wm8994->power_state = OFF;
		
		audio_power(0);		
	}
		
	return 0;
}

<<<<<<< HEAD
static int wm8994_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = wm8994_codec;
	struct wm8994_priv *wm8994 = codec->drvdata;

	DEBUG_LOG("%s..",__func__);
	DEBUG_LOG_ERR("------WM8994 Revision = [%d]-------", wm8994->hw_version);

	if(wm8994->testmode_config_flag)
	{
		DEBUG_LOG_ERR("Testmode is activated!! Skip resume sequence!!");
		return 0;
	}

	if(wm8994->call_state == DISCONNECT && wm8994->cur_path == OFF)
	{
		// Turn on sequence by recommend Wolfson.
		audio_power(1);
		wm8994->power_state = CODEC_ON;
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, 0x3 << WM8994_VMID_SEL_SHIFT | WM8994_BIAS_ENA);
		//msleep(50);	// Wait to setup PLL. ////commented as without sleep() also it behaves properly in SLSI platform
		wm8994_write(codec, WM8994_POWER_MANAGEMENT_1, WM8994_VMID_SEL_NORMAL | WM8994_BIAS_ENA);

		wm8994_write(codec,WM8994_OVERSAMPLING, 0x0000);
	}
	return 0;
=======
	wm8994_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	/* Latch volume updates (right only; we always do left then right). */
	snd_soc_update_bits(codec, WM8994_AIF1_DAC1_LEFT_VOLUME,
			    WM8994_AIF1DAC1_VU, WM8994_AIF1DAC1_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_DAC1_RIGHT_VOLUME,
			    WM8994_AIF1DAC1_VU, WM8994_AIF1DAC1_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_DAC2_LEFT_VOLUME,
			    WM8994_AIF1DAC2_VU, WM8994_AIF1DAC2_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_DAC2_RIGHT_VOLUME,
			    WM8994_AIF1DAC2_VU, WM8994_AIF1DAC2_VU);
	snd_soc_update_bits(codec, WM8994_AIF2_DAC_LEFT_VOLUME,
			    WM8994_AIF2DAC_VU, WM8994_AIF2DAC_VU);
	snd_soc_update_bits(codec, WM8994_AIF2_DAC_RIGHT_VOLUME,
			    WM8994_AIF2DAC_VU, WM8994_AIF2DAC_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_ADC1_LEFT_VOLUME,
			    WM8994_AIF1ADC1_VU, WM8994_AIF1ADC1_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_ADC1_RIGHT_VOLUME,
			    WM8994_AIF1ADC1_VU, WM8994_AIF1ADC1_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_ADC2_LEFT_VOLUME,
			    WM8994_AIF1ADC2_VU, WM8994_AIF1ADC2_VU);
	snd_soc_update_bits(codec, WM8994_AIF1_ADC2_RIGHT_VOLUME,
			    WM8994_AIF1ADC2_VU, WM8994_AIF1ADC2_VU);
	snd_soc_update_bits(codec, WM8994_AIF2_ADC_LEFT_VOLUME,
			    WM8994_AIF2ADC_VU, WM8994_AIF1ADC2_VU);
	snd_soc_update_bits(codec, WM8994_AIF2_ADC_RIGHT_VOLUME,
			    WM8994_AIF2ADC_VU, WM8994_AIF1ADC2_VU);
	snd_soc_update_bits(codec, WM8994_DAC1_LEFT_VOLUME,
			    WM8994_DAC1_VU, WM8994_DAC1_VU);
	snd_soc_update_bits(codec, WM8994_DAC1_RIGHT_VOLUME,
			    WM8994_DAC1_VU, WM8994_DAC1_VU);
	snd_soc_update_bits(codec, WM8994_DAC2_LEFT_VOLUME,
			    WM8994_DAC2_VU, WM8994_DAC2_VU);
	snd_soc_update_bits(codec, WM8994_DAC2_RIGHT_VOLUME,
			    WM8994_DAC2_VU, WM8994_DAC2_VU);

	/* Set the low bit of the 3D stereo depth so TLV matches */
	snd_soc_update_bits(codec, WM8994_AIF1_DAC1_FILTERS_2,
			    1 << WM8994_AIF1DAC1_3D_GAIN_SHIFT,
			    1 << WM8994_AIF1DAC1_3D_GAIN_SHIFT);
	snd_soc_update_bits(codec, WM8994_AIF1_DAC2_FILTERS_2,
			    1 << WM8994_AIF1DAC2_3D_GAIN_SHIFT,
			    1 << WM8994_AIF1DAC2_3D_GAIN_SHIFT);
	snd_soc_update_bits(codec, WM8994_AIF2_DAC_FILTERS_2,
			    1 << WM8994_AIF2DAC_3D_GAIN_SHIFT,
			    1 << WM8994_AIF2DAC_3D_GAIN_SHIFT);

	/* Unconditionally enable AIF1 ADC TDM mode; it only affects
	 * behaviour on idle TDM clock cycles. */
	snd_soc_update_bits(codec, WM8994_AIF1_CONTROL_1,
			    WM8994_AIF1ADC_TDM, WM8994_AIF1ADC_TDM);

	wm8994_update_class_w(codec);

	wm8994_handle_pdata(wm8994);

	wm_hubs_add_analogue_controls(codec);
	snd_soc_add_controls(codec, wm8994_snd_controls,
			     ARRAY_SIZE(wm8994_snd_controls));
	snd_soc_dapm_new_controls(dapm, wm8994_dapm_widgets,
				  ARRAY_SIZE(wm8994_dapm_widgets));

	switch (control->type) {
	case WM8994:
		snd_soc_dapm_new_controls(dapm, wm8994_specific_dapm_widgets,
					  ARRAY_SIZE(wm8994_specific_dapm_widgets));
		if (wm8994->revision < 4) {
			snd_soc_dapm_new_controls(dapm, wm8994_lateclk_revd_widgets,
						  ARRAY_SIZE(wm8994_lateclk_revd_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_adc_revd_widgets,
						  ARRAY_SIZE(wm8994_adc_revd_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_dac_revd_widgets,
						  ARRAY_SIZE(wm8994_dac_revd_widgets));
		} else {
			snd_soc_dapm_new_controls(dapm, wm8994_lateclk_widgets,
						  ARRAY_SIZE(wm8994_lateclk_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_adc_widgets,
						  ARRAY_SIZE(wm8994_adc_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_dac_widgets,
						  ARRAY_SIZE(wm8994_dac_widgets));
		}
		break;
	case WM8958:
		snd_soc_add_controls(codec, wm8958_snd_controls,
				     ARRAY_SIZE(wm8958_snd_controls));
		snd_soc_dapm_new_controls(dapm, wm8958_dapm_widgets,
					  ARRAY_SIZE(wm8958_dapm_widgets));
		if (wm8994->revision < 1) {
			snd_soc_dapm_new_controls(dapm, wm8994_lateclk_revd_widgets,
						  ARRAY_SIZE(wm8994_lateclk_revd_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_adc_revd_widgets,
						  ARRAY_SIZE(wm8994_adc_revd_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_dac_revd_widgets,
						  ARRAY_SIZE(wm8994_dac_revd_widgets));
		} else {
			snd_soc_dapm_new_controls(dapm, wm8994_lateclk_widgets,
						  ARRAY_SIZE(wm8994_lateclk_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_adc_widgets,
						  ARRAY_SIZE(wm8994_adc_widgets));
			snd_soc_dapm_new_controls(dapm, wm8994_dac_widgets,
						  ARRAY_SIZE(wm8994_dac_widgets));
		}
		break;
	}
		

	wm_hubs_add_analogue_routes(codec, 0, 0);
	snd_soc_dapm_add_routes(dapm, intercon, ARRAY_SIZE(intercon));

	switch (control->type) {
	case WM8994:
		snd_soc_dapm_add_routes(dapm, wm8994_intercon,
					ARRAY_SIZE(wm8994_intercon));

		if (wm8994->revision < 4) {
			snd_soc_dapm_add_routes(dapm, wm8994_revd_intercon,
						ARRAY_SIZE(wm8994_revd_intercon));
			snd_soc_dapm_add_routes(dapm, wm8994_lateclk_revd_intercon,
						ARRAY_SIZE(wm8994_lateclk_revd_intercon));
		} else {
			snd_soc_dapm_add_routes(dapm, wm8994_lateclk_intercon,
						ARRAY_SIZE(wm8994_lateclk_intercon));
		}
		break;
	case WM8958:
		if (wm8994->revision < 1) {
			snd_soc_dapm_add_routes(dapm, wm8994_revd_intercon,
						ARRAY_SIZE(wm8994_revd_intercon));
			snd_soc_dapm_add_routes(dapm, wm8994_lateclk_revd_intercon,
						ARRAY_SIZE(wm8994_lateclk_revd_intercon));
		} else {
			snd_soc_dapm_add_routes(dapm, wm8994_lateclk_intercon,
						ARRAY_SIZE(wm8994_lateclk_intercon));
			snd_soc_dapm_add_routes(dapm, wm8958_intercon,
						ARRAY_SIZE(wm8958_intercon));
		}

		wm8958_dsp2_init(codec);
		break;
	}

	return 0;

err_irq:
	wm8994_free_irq(codec->control_data, WM8994_IRQ_MIC2_SHRT, wm8994);
	wm8994_free_irq(codec->control_data, WM8994_IRQ_MIC2_DET, wm8994);
	wm8994_free_irq(codec->control_data, WM8994_IRQ_MIC1_SHRT, wm8994);
	if (wm8994->micdet_irq)
		free_irq(wm8994->micdet_irq, wm8994);
err:
	kfree(wm8994);
	return ret;
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
}
#endif

<<<<<<< HEAD
struct snd_soc_codec_device soc_codec_dev_wm8994 = {
	.probe = 	wm8994_probe,
	.remove = 	wm8994_remove,
#ifdef CONFIG_PM
	.suspend= wm8994_suspend,
	.resume= wm8994_resume,
#endif
};
=======
static int  wm8994_codec_remove(struct snd_soc_codec *codec)
{
	struct wm8994_priv *wm8994 = snd_soc_codec_get_drvdata(codec);
	struct wm8994 *control = codec->control_data;

	wm8994_set_bias_level(codec, SND_SOC_BIAS_OFF);

	pm_runtime_disable(codec->dev);

	switch (control->type) {
	case WM8994:
		if (wm8994->micdet_irq)
			free_irq(wm8994->micdet_irq, wm8994);
		wm8994_free_irq(codec->control_data, WM8994_IRQ_MIC2_DET,
				wm8994);
		wm8994_free_irq(codec->control_data, WM8994_IRQ_MIC1_SHRT,
				wm8994);
		wm8994_free_irq(codec->control_data, WM8994_IRQ_MIC1_DET,
				wm8994);
		break;

	case WM8958:
		if (wm8994->micdet_irq)
			free_irq(wm8994->micdet_irq, wm8994);
		break;
	}
	if (wm8994->mbc)
		release_firmware(wm8994->mbc);
	if (wm8994->mbc_vss)
		release_firmware(wm8994->mbc_vss);
	if (wm8994->enh_eq)
		release_firmware(wm8994->enh_eq);
	kfree(wm8994->retune_mobile_texts);
	kfree(wm8994->drc_texts);
	kfree(wm8994);
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

EXPORT_SYMBOL_GPL(soc_codec_dev_wm8994);

<<<<<<< HEAD
#if defined ATTACH_ADDITINAL_PCM_DRIVER
struct snd_soc_codec_device soc_codec_dev_pcm_wm8994 = {
        .probe =        wm8994_pcm_probe,
        .remove =       wm8994_pcm_remove,
#ifdef CONFIG_PM
       // .suspend= wm8994_pcm_suspend,
       // .resume= wm8994_pcm_resume,
#endif
=======
static struct snd_soc_codec_driver soc_codec_dev_wm8994 = {
	.probe =	wm8994_codec_probe,
	.remove =	wm8994_codec_remove,
	.suspend =	wm8994_suspend,
	.resume =	wm8994_resume,
	.read =		wm8994_read,
	.write =	wm8994_write,
	.readable_register = wm8994_readable,
	.volatile_register = wm8994_volatile,
	.set_bias_level = wm8994_set_bias_level,

	.reg_cache_size = WM8994_CACHE_SIZE,
	.reg_cache_default = wm8994_reg_defaults,
	.reg_word_size = 2,
	.compress_type = SND_SOC_RBTREE_COMPRESSION,
};

static int __devinit wm8994_probe(struct platform_device *pdev)
{
	return snd_soc_register_codec(&pdev->dev, &soc_codec_dev_wm8994,
			wm8994_dai, ARRAY_SIZE(wm8994_dai));
}

static int __devexit wm8994_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver wm8994_codec_driver = {
	.driver = {
		   .name = "wm8994-codec",
		   .owner = THIS_MODULE,
		   },
	.probe = wm8994_probe,
	.remove = __devexit_p(wm8994_remove),
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
};

EXPORT_SYMBOL_GPL(soc_codec_dev_pcm_wm8994);
#endif

static int __init wm8994_modinit(void)
{
	int ret;
	ret = snd_soc_register_dai(&wm8994_dai);
	if(ret)
		printk(KERN_ERR "..dai registration failed..\n");

#if defined ATTACH_ADDITINAL_PCM_DRIVER
	ret = snd_soc_register_dai(&wm8994_pcm_dai);
	if(ret)
		printk(KERN_ERR "..pcm_dai registration failed..\n");
#endif

	return ret;
}
module_init(wm8994_modinit);

static void __exit wm8994_exit(void)
{
	snd_soc_unregister_dai(&wm8994_dai);
#if defined ATTACH_ADDITINAL_PCM_DRIVER
	snd_soc_unregister_dai(&wm8994_pcm_dai);
#endif
}
module_exit(wm8994_exit);

MODULE_DESCRIPTION("ASoC WM8994 driver");
MODULE_AUTHOR("Shaju Abraham shaju.abraham@samsung.com");
MODULE_LICENSE("GPL");
