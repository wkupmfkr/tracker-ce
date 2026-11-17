// PTFX_NAME: Transient
// PTFX_AUTHOR: JHNY
// PTFX_FLAGS: EFFECT, STEREO
// PTFX_SCRATCH: 0
#include "jhny_fx_common.h"
const PluginParam PLUGIN_PARAMS[5]={
 PTFX_PARAM_INT("Attack",-12,12,3,1,"dB"),
 PTFX_PARAM_INT("Sustain",-12,12,0,1,"dB"),
 PTFX_PARAM_INT("Time",10,200,50,1,"ms"),
 PTFX_PARAM_INT("Mix",0,100,100,1,"%"),
 PTFX_PARAM_INT("Output",-18,6,0,1,"dB")
};
static const PtfxHostApi*h;static float sr,attack,sustain,time,mix,output,fast,slow,gain,fa,fr,sa,ss,gs;
static void coeff(void){fa=fx_smooth(.5f,sr);fr=fx_smooth(time*.4f,sr);sa=fx_smooth(time*.3f,sr);ss=fx_smooth(time*2.4f,sr);gs=fx_smooth(1,sr);}
void plugin_init(void*m,uint32_t b,const PtfxHostApi*api){(void)m;(void)b;h=api;sr=fx_rate(h);attack=fx_gain(h,3);sustain=1;time=50;mix=output=gain=1;fast=slow=0;coeff();if(h&&h->set_extra_samples)h->set_extra_samples(0,0);}
void plugin_set_param(uint8_t i,float v){switch(i){case 0:attack=fx_gain(h,fx_bound(v,-12,12));break;case 1:sustain=fx_gain(h,fx_bound(v,-12,12));break;case 2:time=fx_bound(v,10,200);break;case 3:mix=fx_bound(v,0,100)*.01f;break;case 4:output=fx_gain(h,fx_bound(v,-18,6));break;default:return;}coeff();}
int32_t plugin_process_block(const int16_t**in,int16_t**out,uint32_t n,uint8_t channels){if(!fx_valid(out,channels))return 0;for(uint32_t i=0;i<n;i++){
 float dry[2]={in&&in[0]?in[0][i]:0,channels==2&&in&&in[1]?in[1][i]:0};float peak=fx_abs(dry[0])/32768;if(channels==2&&fx_abs(dry[1])/32768>peak)peak=fx_abs(dry[1])/32768;
 fast+=(peak-fast)*(peak>fast?fa:fr);slow+=(peak-slow)*(peak>slow?sa:ss);
 float transient=fx_bound((fast-slow)/(slow+.001f),0,1);float target=fast>.0001f?sustain+(attack-sustain)*transient:1;
 gain+=(target-gain)*gs;for(uint32_t ch=0;ch<channels;ch++)out[ch][i]=fx_pcm(dry[ch]*(1+mix*(gain-1))*output);
 }return (int32_t)n;}
