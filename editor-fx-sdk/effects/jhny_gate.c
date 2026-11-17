// PTFX_NAME: Gate
// PTFX_AUTHOR: JHNY
// PTFX_FLAGS: EFFECT, STEREO
// PTFX_SCRATCH: 0

#include "tracker_plugin.h"
const PluginParam PLUGIN_PARAMS[5] = {
    PTFX_PARAM_INT("Thresh", -80, 0, -40, 1, "dB"),
    PTFX_PARAM_FLOAT("Attack", 0.1, 50, 0.5, 0.1, "ms"),
    PTFX_PARAM_INT("Hold", 0, 500, 30, 1, "ms"),
    PTFX_PARAM_INT("Release", 5, 1000, 80, 1, "ms"),
    PTFX_PARAM_INT("Range", -80, 0, -80, 1, "dB")
};
static const PtfxHostApi* host;
static float rate,threshold_db,attack_ms,hold_ms,release_ms,range_db;
static float threshold,floor_gain,attack,release,detector_release;
static float envelope,gain;
static uint32_t hold_frames,remaining;
static uint8_t opened;
static float bounded(float x,float lo,float hi) {
    if(!(x>=lo))return lo;
    return x>hi?hi:x;
}
static float smoothing(float ms) {
    float n=ms*0.001f*rate;
    return host && host->expf ? 1.0f-host->expf(-1.0f/n) : 1.0f/(1.0f+n);
}
static float dbgain(float db) {
    if(host && host->powf)return host->powf(10.0f,db/20.0f);
    // Safe fallback for incomplete hosts; production provides powf.
    float result=1.0f;
    for(int i=0;i<(int)(-db);++i)result*=0.89125094f;
    return result;
}
static void coefficients(void) {
    threshold=dbgain(threshold_db);
    floor_gain=range_db<=-80 ? 0.0f : dbgain(range_db);
    attack=smoothing(attack_ms);release=smoothing(release_ms);
    detector_release=1.0f-smoothing(10.0f);
    hold_frames=(uint32_t)(hold_ms*0.001f*rate+0.5f);
}
void plugin_init(void* scratch,uint32_t bytes,const PtfxHostApi* api) {
    (void)scratch;(void)bytes;host=api;
    rate=api && api->sample_rate>=8000 && api->sample_rate<=48000 ? (float)api->sample_rate : 44100.0f;
    threshold_db=-40;attack_ms=0.5f;hold_ms=30;release_ms=80;range_db=-80;
    envelope=gain=0;remaining=0;opened=0;
    coefficients();
    if(host && host->set_extra_samples)host->set_extra_samples(0,0);
}
void plugin_set_param(uint8_t index,float value) {
    switch(index){
        case 0:threshold_db=bounded(value,-80,0);break;
        case 1:attack_ms=bounded(value,0.1f,50);break;
        case 2:hold_ms=bounded(value,0,500);break;
        case 3:release_ms=bounded(value,5,1000);break;
        case 4:range_db=bounded(value,-80,0);break;
        default:return;
    }
    coefficients();
    // Before processing, start at the closed-gate floor (Range 0 is exact bypass).
    if(envelope==0 && !opened && remaining==0)gain=floor_gain;
}
int32_t plugin_process_block(const int16_t** in,int16_t** out,uint32_t samples,uint8_t channels) {
    if(!out || channels<1 || channels>2 || !out[0] || (channels==2 && !out[1]))return 0;
    for(uint32_t i=0;i<samples;++i){
        int32_t dry[2]={in && in[0]?in[0][i]:0,channels==2 && in && in[1]?in[1][i]:0};
        float peak=0;
        for(uint32_t ch=0;ch<channels;++ch){float a=(float)(dry[ch]<0?-dry[ch]:dry[ch])/32768.0f;if(a>peak)peak=a;}
        envelope*=detector_release;if(peak>envelope)envelope=peak;
        // Linked stereo peak and fixed 3 dB hysteresis avoid image movement/chatter.
        if(envelope>=threshold){opened=1;remaining=hold_frames;}
        else if(opened){
            if(envelope>=threshold*0.7079458f)remaining=hold_frames;
            else if(remaining) --remaining;
            else opened=0;
        }
        float target=opened?1.0f:floor_gain;
        gain+=(target-gain)*(target>gain?attack:release);
        gain=bounded(gain,0,1);
        for(uint32_t ch=0;ch<channels;++ch)out[ch][i]=(int16_t)((float)dry[ch]*gain);
    }
    return (int32_t)samples;
}
