// PTFX_NAME: Reverb
// PTFX_AUTHOR: JHNY
// PTFX_FLAGS: EFFECT, STEREO
// PTFX_SCRATCH: 131072

#include "tracker_plugin.h"

const PluginParam PLUGIN_PARAMS[6] = {
    PTFX_PARAM_INT("Size", 20, 100, 60, 1, "%"),
    PTFX_PARAM_FLOAT("Decay", 0.2, 8.0, 1.8, 0.1, "s"),
    PTFX_PARAM_INT("Damp", 0, 90, 45, 1, "%"),
    PTFX_PARAM_INT("PreDly", 0, 100, 15, 1, "ms"),
    PTFX_PARAM_INT("Mix", 0, 100, 25, 1, "%"),
    PTFX_PARAM_LIST("Tail", 0, "Ring Out|Cutoff")
};

// Four damped parallel combs and two allpass diffusers per channel.
// Delay lines and all render state are reset at every Preview/Apply.
typedef struct {
    float comb[2][4][2048];
    float allpass[2][2][1024];
    float predelay[2][4801];
    float lowpass[2][4], previous[2], highpass[2];
    uint32_t comb_pos[2][4], ap_pos[2][2], pre_pos;
} ReverbState;
_Static_assert(sizeof(ReverbState) <= 131072, "Reverb scratch exceeds descriptor");
static const PtfxHostApi* host;
static ReverbState* state;
static float rate, room, decay, damping, mix, feedback[2][4];
static uint32_t comb_len[2][4], ap_len[2][2], pre_len;
static uint8_t cut_tail;

static float bounded(float x, float lo, float hi) {
    if (!(x >= lo)) return lo;
    return x > hi ? hi : x;
}
static void coefficients(void) {
    const uint16_t bases[4] = {1117,1277,1423,1559};
    const uint16_t diffusers[2] = {223,557};
    float scale = (0.4f + 0.0075f * room) * rate / 44100.0f;
    for (uint32_t ch=0; ch<2; ++ch) {
        for (uint32_t j=0; j<4; ++j) {
            uint32_t n = (uint32_t)((bases[j] + ch*23) * scale + 0.5f);
            if (n<1) n=1;
            if (n>2048) n=2048;
            comb_len[ch][j]=n;
            float exponent=(float)n/(rate*decay);
            feedback[ch][j] = host && host->powf ? host->powf(0.001f, exponent) :
                                 1.0f/(1.0f+6.907755f*exponent);
            feedback[ch][j]=bounded(feedback[ch][j],0.0f,0.9995f);
        }
        for(uint32_t j=0;j<2;++j) {
            uint32_t n=(uint32_t)((diffusers[j]+ch*17)*scale+0.5f);
            if(n<1)n=1;
            if(n>1024)n=1024;
            ap_len[ch][j]=n;
        }
    }
    if(host && host->set_extra_samples) {
        // Allow diffusion to settle after the nominal -60 dB comb decay.
        uint32_t tail=(uint32_t)((decay*1.2f+0.2f)*rate)+pre_len;
        if(tail>(uint32_t)(10.0f*rate))tail=(uint32_t)(10.0f*rate);
        host->set_extra_samples(0, state && !cut_tail && mix>0.0f ? tail : 0);
    }
}
void plugin_init(void* scratch, uint32_t bytes, const PtfxHostApi* api) {
    host=api; state=0;
    rate=api && api->sample_rate>=8000 && api->sample_rate<=48000 ? (float)api->sample_rate : 44100.0f;
    room=60; decay=1.8f; damping=0.45f; mix=0.25f; cut_tail=0;
    pre_len=(uint32_t)(0.015f*rate+0.5f);
    if(scratch && bytes>=sizeof(ReverbState)) {
        state=(ReverbState*)scratch;
        // Do not require libc or a non-null host memset function.
        uint8_t* p=(uint8_t*)state;
        for(uint32_t i=0;i<sizeof(ReverbState);++i)p[i]=0;
    }
    coefficients();
}
void plugin_set_param(uint8_t index,float value) {
    switch(index) {
        case 0:room=bounded(value,20,100);break;
        case 1:decay=bounded(value,0.2f,8);break;
        case 2:damping=bounded(value,0,90)*0.01f;break;
        case 3:pre_len=(uint32_t)(bounded(value,0,100)*rate*0.001f+0.5f);break;
        case 4:mix=bounded(value,0,100)*0.01f;break;
        case 5:cut_tail=value>=0.5f;break;
        default:return;
    }
    if(pre_len>4800)pre_len=4800;
    coefficients();
}
int32_t plugin_process_block(const int16_t** in,int16_t** out,uint32_t samples,uint8_t channels) {
    if(!out || channels<1 || channels>2 || !out[0] || (channels==2 && !out[1]))return 0;
    for(uint32_t i=0;i<samples;++i) {
        float dry[2]={in && in[0] ? (float)in[0][i] : 0,
                      channels==2 && in && in[1] ? (float)in[1][i] : 0};
        if(!state || mix==0.0f) {
            for(uint32_t ch=0;ch<channels;++ch)out[ch][i]=(int16_t)dry[ch];
            continue;
        }
        for(uint32_t ch=0;ch<channels;++ch) {
            float x=channels==2 ? dry[ch]*0.8f+dry[1-ch]*0.2f : dry[ch];
            // DC rejection before the feedback network.
            float hp=x-state->previous[ch]+0.995f*state->highpass[ch];
            state->previous[ch]=x;state->highpass[ch]=hp;
            uint32_t read=(state->pre_pos+4801-pre_len)%4801;
            x=pre_len ? state->predelay[ch][read] : hp;
            state->predelay[ch][state->pre_pos]=hp;
            float wet=0;
            for(uint32_t j=0;j<4;++j) {
                uint32_t pos=state->comb_pos[ch][j];
                if(pos>=comb_len[ch][j])pos=0;
                float delayed=state->comb[ch][j][pos];
                float lp=delayed*(1.0f-damping)+state->lowpass[ch][j]*damping;
                state->lowpass[ch][j]=lp;
                state->comb[ch][j][pos]=x*0.35f+lp*feedback[ch][j];
                state->comb_pos[ch][j]=(pos+1==comb_len[ch][j])?0:pos+1;
                wet+=delayed*0.25f;
            }
            for(uint32_t j=0;j<2;++j) {
                uint32_t pos=state->ap_pos[ch][j];
                if(pos>=ap_len[ch][j])pos=0;
                float delayed=state->allpass[ch][j][pos];
                // Unity-magnitude allpass, stable coefficient 0.5.
                float v=wet+0.5f*delayed;
                wet=delayed-0.5f*v;
                state->allpass[ch][j][pos]=v;
                state->ap_pos[ch][j]=(pos+1==ap_len[ch][j])?0:pos+1;
            }
            float y=dry[ch]*(1.0f-mix)+wet*mix;
            y=bounded(y,-32768.0f,32767.0f);
            out[ch][i]=(int16_t)y;
        }
        if(++state->pre_pos==4801)state->pre_pos=0;
    }
    return (int32_t)samples;
}
