// EndPointController.cpp : Defines the entry point for the console application.
//

#include "AudioPlaybackDeviceManager.h"
#include "AudioPlaybackDevice.h"
#include "NvAPISupport.h"

#include <stdio.h>
#include <tchar.h>



// swapAudioDevices Speakers VSX-925
// swapDisplays 2147881095 2147881088
int _tmain(int argc, _TCHAR* argv[])
{

    if (argc > 1)
    {

        for (unsigned int i = 1; i < (unsigned int)argc; ++i)
        {
            std::wstring arg(argv[i]);
            if (arg == L"swapAudioDevices")
            {
                if (i+2 < (unsigned int)argc)
                {
                    ++i;
                    std::wstring arg1(argv[i]);
                    ++i;
                    std::wstring arg2(argv[i]);

                    AudioPlaybackDevice* device1 = AudioPlaybackDeviceManager::getInstance()->findDeviceFromNameSubString(arg1);
                    AudioPlaybackDevice* device2 = AudioPlaybackDeviceManager::getInstance()->findDeviceFromNameSubString(arg2);
                    if (device1 && device2)
                    {
                        if (device1->_default)
                        {
                            device2->setDefault();
                        }
                        else
                        {
                            device1->setDefault();
                        }
                    }
                    else
                    {
                        if (!device1)
                        {
                            printf("Error: Could not find audio device with substring %s in name.\n", arg1);
                        }
                        if (!device2)
                        {
                            printf("Error: Could not find audio device with substring %s in name.\n", arg2);
                        }
                    }
                }
                else
                {
                    printf("Error: Must specify two device name substrings for swapAudioDevices.\n");
                }
            }
            else if (arg == L"switchToAudioDevice")
            {
                if (i+1 < (unsigned int)argc)
                {
                    ++i;
                    std::wstring arg1(argv[i]);

                    AudioPlaybackDevice* device1 = AudioPlaybackDeviceManager::getInstance()->findDeviceFromNameSubString(arg1);
                    if (device1)
                    {
                        if (device1->_default)
                        {
                            printf("Audio device with substring %s was already the default device.\n", arg1);
                        }
                        else
                        {
                            device1->setDefault();
                        }
                    }
                    else
                    {
                        printf("Error: Could not find audio device with substring %s in name.\n", arg1);
                    }
                }
                else
                {
                    printf("Error: Must specify a device name substring for switchToAudioDevice.\n");
                }
            }
            else if (arg == L"swapDisplays")
            {
                if (i+2 < (unsigned int)argc)
                {
                    ++i;
                    NvU32 displayId1 = 0;
                    swscanf(argv[i], L"%u", &displayId1);
                    ++i;
                    NvU32 displayId2 = 0;
                    swscanf(argv[i], L"%u", &displayId2);

                    NVApiManager::getInstance()->swapDisplays(displayId1, displayId2);
                }
                else
                {
                    printf("Error: Must specify two displayIds for swapDisplays.\n");
                }
            }
            else if (arg == L"switchToDisplay")
            {
                if (i+3 < (unsigned int)argc)
                {
                    ++i;
                    NvU32 display = 0;
                    swscanf(argv[i], L"%u", &display);
                    ++i;
                    NvU32 target = 0;
                    swscanf(argv[i], L"%u", &target);
                    ++i;
                    NvU32 displayId1 = 0;
                    swscanf(argv[i], L"%u", &displayId1);

                    NVApiManager::getInstance()->switchToDisplay(display, target, displayId1);
                }
                else
                {
                    printf("Error: Must specify a display, target and displayId for switchToDisplay.\n");
                }
            }
            else if (arg == L"cloneDisplay")
            {
                if (i+2 < (unsigned int)argc)
                {
                    ++i;
                    NvU32 displayId1 = 0;
                    swscanf(argv[i], L"%u", &displayId1);
                    ++i;
                    NvU32 displayId2 = 0;
                    swscanf(argv[i], L"%u", &displayId2);

                    NVApiManager::getInstance()->cloneDisplay(displayId1, displayId2);
                }
                else
                {
                    printf("Error: Must specify two displayIds for cloneDisplay.\n");
                }
            }
            else
            {
                printf("Error: unrecognized command %s \n", argv[i]);
            }
        }

    }
    else
    {
        // No arguments, dump all the info.

        printf("\nAudio Devices:\n");

        AudioPlaybackDeviceManager::getInstance()->dumpAllDeviceInfo();

        printf("\nAttached Displays:\n");

        NVApiManager::getInstance()->dumpAttachedDisplays();

        printf("\nUnAttached Displays:\n");

        NVApiManager::getInstance()->dumpUnAttachedDisplays();

        printf("\nDisplay IDs and connectors:\n");

        NVApiManager::getInstance()->dumpAllDisplayIds();

        printf("\nDisplay Config:\n");

        NVApiManager::getInstance()->dumpDisplayConfig();

    }

}