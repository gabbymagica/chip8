#ifndef BEEP_H
#define BEEP_H
#include "miniaudio.h"
#include <stdint.h> 
#include <stdbool.h>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
int audio_init();
void audio_set_beep(bool play);

#endif // BEEP_H