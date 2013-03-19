#include "AudioPlaybackDeviceManager.h"

#include <stdio.h>

#define BOOLTOSTRING(val) val  ? "true" : "false"

AudioPlaybackDeviceManager::AudioPlaybackDeviceManager()
{
    initDevices();
}

AudioPlaybackDeviceManager::~AudioPlaybackDeviceManager()
{
    for (unsigned int i = 0; i < m_devices.size(); ++i)
    {
        delete m_devices[i];
    }
}

void AudioPlaybackDeviceManager::dumpAllDeviceInfo()
{
    for (unsigned int i = 0; i < m_devices.size(); ++i)
    {
        printf("Audio Device %u (default: %s): %ws -- %ws\n", i, BOOLTOSTRING(m_devices[i]->_default), m_devices[i]->_id, m_devices[i]->_name.pwszVal);
    }
}

AudioPlaybackDevice* AudioPlaybackDeviceManager::findDeviceFromNameSubString(const std::wstring& substring)
{
    for (unsigned int i = 0; i < m_devices.size(); ++i)
    {
        std::wstring name(m_devices[i]->_name.pwszVal);
        if (name.find(substring) != std::wstring::npos)
        {
            return m_devices[i];
        }
    }

    return nullptr;
}

void AudioPlaybackDeviceManager::onSetDefaultDevice(AudioPlaybackDevice* defaultDevice)
{
    for (unsigned int i = 0; i < m_devices.size(); ++i)
    {
        m_devices[i]->_default = (m_devices[i] == defaultDevice);
    }
}

void AudioPlaybackDeviceManager::initDevices()
{
    HRESULT hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        IMMDeviceEnumerator *pEnum = NULL;
        // Create a multimedia device enumerator.
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
            CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
        if (SUCCEEDED(hr))
        {
            IMMDeviceCollection *pDevices;
            // Enumerate the output devices.
            //hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
            hr = pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &pDevices);
            if (SUCCEEDED(hr))
            {
                UINT count;
                pDevices->GetCount(&count);
                if (SUCCEEDED(hr))
                {
                    for (unsigned int i = 0; i < count; i++)
                    {
                        IMMDevice *pDevice;
                        hr = pDevices->Item(i, &pDevice);
                        if (SUCCEEDED(hr))
                        {
                            m_devices.push_back(new AudioPlaybackDevice(pDevice));
                        }
                    }
                }
                pDevices->Release();
            }

            IMMDevice *pDefaultEndpoint;
            hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDefaultEndpoint);
            if (SUCCEEDED(hr))
            {
                LPWSTR id;
                HRESULT hr = pDefaultEndpoint->GetId(&id);
                if (SUCCEEDED(hr))
                {
                    for (unsigned int i = 0; i < m_devices.size(); ++i)
                    {
                        
                        m_devices[i]->_default = (_wcsicmp(m_devices[i]->_id, id) == 0);
                    }
                }
            }

            pEnum->Release();
        }
    }
}

