// PTFX_NAME: Phaser
// PTFX_AUTHOR: JHNY
// PTFX_FLAGS: EFFECT, STEREO
// PTFX_SCRATCH: 0
#include "jhny_fx_common.h"
const PluginParam PLUGIN_PARAMS[6]={
 PTFX_PARAM_FLOAT("Rate",0.05,8,0.4,0.05,"Hz"),
 PTFX_PARAM_INT("Depth",0,100,75,1,"%"),
 PTFX_PARAM_INT("Center",200,4000,1000,50,"Hz"),
 PTFX_PARAM_INT("Fdbk",-75,75,25,1,"%"),
 PTFX_PARAM_INT("Stereo",0,180,90,5,"deg"),
 PTFX_PARAM_INT("Mix",0,100,50,1,"%")
};
static const PtfxHostApi*h;static float sr,rate,depth,center,feedback,stereo,mix,phase,z[2][6],last[2];
void plugin_init(void*m,uint32_t b,const PtfxHostApi*api){(void)m;(void)b;h=api;sr=fx_rate(h);rate=.4f;depth=.75f;center=1000;feedback=.25f;stereo=.25f;mix=.5f;phase=0;for(int ch=0;ch<2;ch++){last[ch]=0;for(int j=0;j<6;j++)z[ch][j]=0;}if(h&&h->set_extra_samples)h->set_extra_samples(0,0);}
void plugin_set_param(uint8_t i,float v){switch(i){case 0:rate=fx_bound(v,.05f,8);break;case 1:depth=fx_bound(v,0,100)*.01f;break;case 2:center=fx_bound(v,200,4000);break;case 3:feedback=fx_bound(v,-75,75)*.01f;break;case 4:stereo=fx_bound(v,0,180)/360;break;case 5:mix=fx_bound(v,0,100)*.01f;break;default:return;}}
int32_t plugin_process_block(const int16_t**in,int16_t**out,uint32_t n,uint8_t channels){if(!fx_valid(out,channels))return 0;for(uint32_t i=0;i<n;i++){
 for(uint32_t ch=0;ch<channels;ch++){float dry=in&&in[ch]?in[ch][i]:0;float freq=fx_bound(center*(1+.85f*depth*fx_sin(fx_wrap(phase+ch*stereo))),20,sr*.2f);
 float t=3.14159265f*freq/sr;float t2=t*t;float tangent=t*(1+t2*(.333333333f+t2*.133333333f));float a=(tangent-1)/(tangent+1);
 float x=dry*(1-fx_abs(feedback))+last[ch]*feedback;
 for(int j=0;j<6;j++){float y=a*x+z[ch][j];z[ch][j]=x-a*y;x=y;}
 last[ch]=x;out[ch][i]=fx_pcm(dry*(1-mix)+x*mix);}
 phase=fx_wrap(phase+rate/sr);
 }return (int32_t)n;}
