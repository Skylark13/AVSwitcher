#ifndef __AUDIO_PLAYBACK_DEVICE_MANAGER_H__
#define __AUDIO_PLAYBACK_DEVICE_MANAGER_H__

#include "AudioPlaybackDevice.h"

#include <vector>

class AudioPlaybackDeviceManager
{
public:
    static AudioPlaybackDeviceManager* getInstance() { static AudioPlaybackDeviceManager s_instance; return &s_instance; }

    void dumpAllDeviceInfo();

    AudioPlaybackDevice* findDeviceFromNameSubString(const std::wstring& substring);

    void onSetDefaultDevice(AudioPlaybackDevice* defaultDevice);

private:
    AudioPlaybackDeviceManager();
    ~AudioPlaybackDeviceManager();

    void initDevices();

    std::vector<AudioPlaybackDevice*> m_devices;
};



#endif  // __AUDIO_PLAYBACK_DEVICE_MANAGER_H__