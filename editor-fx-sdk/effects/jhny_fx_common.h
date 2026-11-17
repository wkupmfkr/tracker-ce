#pragma once
#include "tracker_plugin.h"
static float fx_bound(float x,float lo,float hi){if(!(x>=lo))return lo;return x>hi?hi:x;}
static float fx_abs(float x){return x<0?-x:x;}
static float fx_rate(const PtfxHostApi*h){return h&&h->sample_rate>=8000&&h->sample_rate<=48000?(float)h->sample_rate:44100.0f;}
static float fx_gain(const PtfxHostApi*h,float db){return h&&h->powf?h->powf(10,db*0.05f):1.0f;}
static float fx_smooth(float ms,float sr){return 1.0f/(1.0f+ms*0.001f*sr);}
static float fx_sin(float phase){float x=phase<0.5f?phase*2:phase*2-2;float y=4*x*(1-fx_abs(x));return y+0.225f*(y*fx_abs(y)-y);}
static float fx_wrap(float p){if(p>=1)p-=1;return p;}
static int16_t fx_pcm(float v){return (int16_t)fx_bound(v,-32768,32767);}
static int fx_valid(int16_t**o,uint8_t c){return o&&c>=1&&c<=2&&o[0]&&(c==1||o[1]);}
