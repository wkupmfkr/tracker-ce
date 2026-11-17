// PTFX_NAME: Tape
// PTFX_AUTHOR: JHNY
// PTFX_FLAGS: EFFECT, STEREO
// PTFX_SCRATCH: 4096
#include "jhny_fx_common.h"
const PluginParam PLUGIN_PARAMS[6]={
 PTFX_PARAM_INT("Drive",0,24,6,1,"dB"),
 PTFX_PARAM_INT("Tone",0,100,65,1,"%"),
 PTFX_PARAM_INT("Wow",0,100,15,1,"%"),
 PTFX_PARAM_INT("Flutter",0,100,10,1,"%"),
 PTFX_PARAM_INT("Mix",0,100,100,1,"%"),
 PTFX_PARAM_INT("Output",-18,6,0,1,"dB")
};
typedef struct{float line[2][256];float low[2];uint32_t pos;}TapeState;
_Static_assert(sizeof(TapeState)<=4096,"scratch");
static TapeState*s;static const PtfxHostApi*h;static float sr,drive,tone,wow,flutter,mix,output,wp,fp,lp,amount;
static void coeff(void){amount=fx_gain(h,drive)-1;float cutoff=2000+tone*160;float a=6.2831853f*cutoff/sr;lp=a/(1+a);}
void plugin_init(void*mem,uint32_t bytes,const PtfxHostApi*api){h=api;sr=fx_rate(h);s=bytes>=sizeof(TapeState)?(TapeState*)mem:0;drive=6;tone=65;wow=.15f;flutter=.1f;mix=1;output=1;wp=fp=0;if(s){uint8_t*b=(uint8_t*)s;for(uint32_t i=0;i<sizeof(TapeState);i++)b[i]=0;}coeff();if(h&&h->set_extra_samples)h->set_extra_samples(0,0);}
void plugin_set_param(uint8_t i,float v){switch(i){case 0:drive=fx_bound(v,0,24);break;case 1:tone=fx_bound(v,0,100);break;case 2:wow=fx_bound(v,0,100)*.01f;break;case 3:flutter=fx_bound(v,0,100)*.01f;break;case 4:mix=fx_bound(v,0,100)*.01f;break;case 5:output=fx_gain(h,fx_bound(v,-18,6));break;default:return;}coeff();}
int32_t plugin_process_block(const int16_t**in,int16_t**out,uint32_t n,uint8_t channels){if(!fx_valid(out,channels))return 0;for(uint32_t i=0;i<n;i++){
 float delay=sr*(.003f+.0015f*wow*fx_sin(wp)+.0002f*flutter*fx_sin(fp));
 float read=(float)(s?s->pos:0)-delay;if(read<0)read+=256;uint32_t j=(uint32_t)read;float frac=read-j;
 for(uint32_t ch=0;ch<channels;ch++){float dry=in&&in[ch]?in[ch][i]:0;float x=dry;
 if(s){s->line[ch][s->pos]=dry;if(wow>0||flutter>0)x=s->line[ch][j]*(1-frac)+s->line[ch][(j+1)%256]*frac;
 x/=32768;x=x*(1+amount)/(1+amount*fx_abs(x));x*=32768;
 s->low[ch]+=lp*(x-s->low[ch]);if(tone<100)x=s->low[ch];}
 out[ch][i]=fx_pcm((dry*(1-mix)+x*mix)*output);}
 if(s)s->pos=(s->pos+1)%256;wp=fx_wrap(wp+.35f/sr);fp=fx_wrap(fp+6.1f/sr);
 }return (int32_t)n;}
