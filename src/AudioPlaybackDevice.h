#ifndef __AUDIO_PLAYBACK_DEVICE_H__
#define __AUDIO_PLAYBACK_DEVICE_H__

#include <stdio.h>
#include <string>
#include <vector>

#include "windows.h"
#include "Mmdeviceapi.h"
#include "PolicyConfig.h"
#include "Propidl.h"
#include "Functiondiscoverykeys_devpkey.h"

struct AudioPlaybackDevice
{
    LPWSTR _id;
    PROPVARIANT _name;
    bool _default;
    IMMDevice* _device;

    AudioPlaybackDevice(IMMDevice* device);
    ~AudioPlaybackDevice();

    void setDefault();

};


#endif  // __AUDIO_PLAYBACK_DEVICE_H__