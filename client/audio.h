// client audio management

#pragma once

#include "types.h"

error audio_init();
bytestring audio_read();
void audio_write(bytestring sound);
void audio_close();
