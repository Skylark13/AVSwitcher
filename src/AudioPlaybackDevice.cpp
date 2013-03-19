#include "AudioPlaybackDevice.h"

#include "AudioPlaybackDeviceManager.h"

AudioPlaybackDevice::AudioPlaybackDevice(IMMDevice* device)
    : _id(NULL)
    , _device(device)
    , _default(false)
{
    HRESULT hr = _device->GetId(&_id);
    if (SUCCEEDED(hr))
    {
        IPropertyStore *pStore;
        hr = _device->OpenPropertyStore(STGM_READ, &pStore);
        if (SUCCEEDED(hr))
        {
            _name;
            PropVariantInit(&_name);
            hr = pStore->GetValue(PKEY_Device_FriendlyName, &_name);
            pStore->Release();
        }
    }
}

AudioPlaybackDevice::~AudioPlaybackDevice()
{
    PropVariantClear(&_name);
    _device->Release();
}

void AudioPlaybackDevice::setDefault()
{
    IPolicyConfigVista *pPolicyConfig;
    ERole role = eMultimedia;

    printf("Trying to set default device to %ws", _name.pwszVal);

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), 
        NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
    if (SUCCEEDED(hr))
    {
        hr = pPolicyConfig->SetDefaultEndpoint(_id, role);
        pPolicyConfig->Release();

        if (SUCCEEDED(hr))
        {
            printf("... ok\n");
        }
        else
        {
            printf("... failed\n");
        }
    }

    AudioPlaybackDeviceManager::getInstance()->onSetDefaultDevice(this);
}

