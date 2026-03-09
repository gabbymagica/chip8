#include "beep.h"
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>

static ma_device device;
static ma_waveform beep_wave;
volatile u_int8_t beep_active = 0;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_waveform* pWave = (ma_waveform*)pDevice->pUserData;
    
    if (beep_active) {
        ma_waveform_read_pcm_frames(pWave, pOutput, frameCount, NULL);
    } else {
        ma_silence_pcm_frames(pOutput, frameCount, pDevice->playback.format, pDevice->playback.channels);
    }
}

int audio_init() {
    ma_waveform_config waveConfig = ma_waveform_config_init(
        ma_format_f32, 1, 48000, ma_waveform_type_square, 0.1, 440.0
    );
    
    ma_waveform_init(&waveConfig, &beep_wave);

    ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = ma_format_f32;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate        = 48000;
    deviceConfig.dataCallback      = data_callback; 
    deviceConfig.pUserData         = &beep_wave; 

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        return -1;
    }   
    
    ma_device_start(&device);
    return 0;
}

void audio_set_beep(bool play) {
    beep_active = play;
}