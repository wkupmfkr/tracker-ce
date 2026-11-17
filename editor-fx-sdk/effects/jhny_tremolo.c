// PTFX_NAME: Tremolo
// PTFX_AUTHOR: JHNY
// PTFX_FLAGS: EFFECT, STEREO
// PTFX_SCRATCH: 0
#include "jhny_fx_common.h"
const PluginParam PLUGIN_PARAMS[6]={
 PTFX_PARAM_FLOAT("Rate",0.05,20,4,0.05,"Hz"),
 PTFX_PARAM_INT("Depth",0,100,60,1,"%"),
 PTFX_PARAM_LIST("Shape",0,"Sine|Triangle|Pulse"),
 PTFX_PARAM_INT("Stereo",0,180,0,5,"deg"),
 PTFX_PARAM_INT("Smooth",0,20,3,1,"ms"),
 PTFX_PARAM_INT("Mix",0,100,100,1,"%")
};
static const PtfxHostApi*h;static float sr,rate,depth,stereo,smooth,mix,phase,gain[2],coeff;static uint8_t shape;
void plugin_init(void*m,uint32_t b,const PtfxHostApi*api){(void)m;(void)b;h=api;sr=fx_rate(h);rate=4;depth=.6f;shape=0;stereo=0;smooth=3;mix=1;phase=0;gain[0]=gain[1]=1;coeff=fx_smooth(smooth,sr);if(h&&h->set_extra_samples)h->set_extra_samples(0,0);}
void plugin_set_param(uint8_t i,float v){switch(i){case 0:rate=fx_bound(v,.05f,20);break;case 1:depth=fx_bound(v,0,100)*.01f;break;case 2:shape=(uint8_t)fx_bound(v,0,2);break;case 3:stereo=fx_bound(v,0,180)/360;break;case 4:smooth=fx_bound(v,0,20);break;case 5:mix=fx_bound(v,0,100)*.01f;break;default:return;}coeff=fx_smooth(smooth,sr);}
int32_t plugin_process_block(const int16_t**in,int16_t**out,uint32_t n,uint8_t channels){if(!fx_valid(out,channels))return 0;for(uint32_t i=0;i<n;i++){
 for(uint32_t ch=0;ch<channels;ch++){float dry=in&&in[ch]?in[ch][i]:0;float p=fx_wrap(phase+ch*stereo);float wave=shape==0?.5f+.5f*fx_sin(p):shape==1?1-fx_abs(2*p-1):(p<.5f?1:0);
 float target=1-depth*wave;gain[ch]+=(target-gain[ch])*coeff;out[ch][i]=fx_pcm(dry*(1-mix+mix*gain[ch]));}
 phase=fx_wrap(phase+rate/sr);
 }return (int32_t)n;}
