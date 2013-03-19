#include "NvAPISupport.h"

#include "nvapi.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define BYTETOBINARYPATTERN "%u%u%u%u%u%u%u%u"
#define BYTETOBINARY(byte)  \
    (byte & 0x80 ? 1 : 0), \
    (byte & 0x40 ? 1 : 0), \
    (byte & 0x20 ? 1 : 0), \
    (byte & 0x10 ? 1 : 0), \
    (byte & 0x08 ? 1 : 0), \
    (byte & 0x04 ? 1 : 0), \
    (byte & 0x02 ? 1 : 0), \
    (byte & 0x01 ? 1 : 0) 

#define U32TOBINARYPATTERN BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN
#define U32TOBINARY(val) \
    BYTETOBINARY((val & 0xFF000000) >> 24), \
    BYTETOBINARY((val & 0x00FF0000) >> 16), \
    BYTETOBINARY((val & 0x0000FF00) >>  8), \
    BYTETOBINARY((val & 0x000000FF) >>  0)

#define BOOLTOSTRING(val) val  ? "true" : "false"

namespace
{

    struct NvAPIDisplayIDs
    {
        NvAPIDisplayIDs(NvU32 gpuIndex = 0)
            : m_gpuIndex(gpuIndex)
            , m_gpuCount(0)
            , m_displayIDs(NULL)
            , m_displayIdCount(0)
        {
            NvAPI_EnumPhysicalGPUs(m_gpuHandles, &m_gpuCount);

            // Get number of displayIDs.
            NvAPI_GPU_GetAllDisplayIds(m_gpuHandles[m_gpuIndex], NULL, &m_displayIdCount);

            m_displayIDs = (NV_GPU_DISPLAYIDS*)malloc(sizeof(NV_GPU_DISPLAYIDS) * m_displayIdCount);
            memset(m_displayIDs, 0, sizeof(NV_GPU_DISPLAYIDS) * m_displayIdCount);
            for (unsigned int i = 0; i < m_displayIdCount; ++i)
            {
                m_displayIDs[i].version = NV_GPU_DISPLAYIDS_VER;
            }

            NvAPI_GPU_GetAllDisplayIds(m_gpuHandles[m_gpuIndex], m_displayIDs, &m_displayIdCount);
        }

        ~NvAPIDisplayIDs()
        {
            free(m_displayIDs);
        }

        NvU32 m_gpuIndex;
        NvPhysicalGpuHandle m_gpuHandles[NVAPI_MAX_PHYSICAL_GPUS];
        NvU32 m_gpuCount;
        NV_GPU_DISPLAYIDS* m_displayIDs;
        NvU32 m_displayIdCount;
    };

    struct NvAPIDisplayConfig
    {
        NvAPIDisplayConfig(bool includeSourceModeInfo = false)
            : m_pathInfoCount(0)
            , m_pathInfos(NULL)
        {
            NvAPI_DISP_GetDisplayConfig(&m_pathInfoCount, NULL);

            m_pathInfos = (NV_DISPLAYCONFIG_PATH_INFO*)malloc(sizeof(NV_DISPLAYCONFIG_PATH_INFO) * m_pathInfoCount);
            memset(m_pathInfos, 0, sizeof(NV_DISPLAYCONFIG_PATH_INFO) * m_pathInfoCount);
            for (unsigned int i = 0; i < m_pathInfoCount; ++i)
            {
                m_pathInfos[i].version = NV_DISPLAYCONFIG_PATH_INFO_VER;
                if (includeSourceModeInfo)
                {
                    m_pathInfos[i].sourceModeInfo = (NV_DISPLAYCONFIG_SOURCE_MODE_INFO*)malloc(sizeof(NV_DISPLAYCONFIG_SOURCE_MODE_INFO));
                }
            }

            NvAPI_DISP_GetDisplayConfig(&m_pathInfoCount, m_pathInfos);

            for (unsigned int i = 0; i < m_pathInfoCount; ++i)
            {
                m_pathInfos[i].targetInfo = (NV_DISPLAYCONFIG_PATH_TARGET_INFO*)malloc(sizeof(NV_DISPLAYCONFIG_PATH_TARGET_INFO) * m_pathInfos[i].targetInfoCount);
                memset(m_pathInfos[i].targetInfo, 0, sizeof(NV_DISPLAYCONFIG_PATH_TARGET_INFO) * m_pathInfos[i].targetInfoCount);
            }

            NvAPI_DISP_GetDisplayConfig(&m_pathInfoCount, m_pathInfos);
        }

        ~NvAPIDisplayConfig()
        {
            for (unsigned int i = 0; i < m_pathInfoCount; ++i)
            {
                free(m_pathInfos[i].targetInfo);
                if (m_pathInfos[i].sourceModeInfo)
                {
                    free(m_pathInfos[i].sourceModeInfo);
                }
            }

            free(m_pathInfos);
        }

        void apply()
        {
            NvAPI_Status retval = NVAPI_OK;
            retval = NvAPI_DISP_SetDisplayConfig(m_pathInfoCount, m_pathInfos, NV_DISPLAYCONFIG_VALIDATE_ONLY);
            if (retval == NVAPI_OK)
            {
                printf("Validation successful\n");

                retval = NvAPI_DISP_SetDisplayConfig(m_pathInfoCount, m_pathInfos, NV_DISPLAYCONFIG_SAVE_TO_PERSISTENCE | NV_DISPLAYCONFIG_DRIVER_RELOAD_ALLOWED);

                if (retval == NVAPI_OK)
                {
                    printf("Apply successful\n");
                }
                else
                {
                    printf("Apply failed: %u\n", retval);
                }
            }
            else
            {
                printf("Validation failed: %u\n", retval);
            }
        }

        NvU32 m_pathInfoCount;
        NV_DISPLAYCONFIG_PATH_INFO* m_pathInfos;
    };

}



NVApiManager::NVApiManager()
{
    NvAPI_Initialize();
}

NVApiManager::~NVApiManager()
{
    NvAPI_Unload();
}

NVApiManager* NVApiManager::getInstance() { static NVApiManager s_instance; return &s_instance; }

void NVApiManager::dumpAttachedDisplays()
{
    NvDisplayHandle handle = NULL;

    NvAPI_Status ret = NVAPI_OK;
    NvU32 i = 0;
    while (NvAPI_EnumNvidiaDisplayHandle(i, &handle) != NVAPI_END_ENUMERATION)
    {
        NvAPI_ShortString name;
        NvAPI_GetAssociatedNvidiaDisplayName(handle, name);
        NvU32 outputId = 0;
        NvAPI_GetAssociatedDisplayOutputId(handle, &outputId);
        printf("Attached Display %u : %s - Output "U32TOBINARYPATTERN"\n", i, name, U32TOBINARY(outputId));
        ++i;
    }
}

void NVApiManager::dumpUnAttachedDisplays()
{
    NvUnAttachedDisplayHandle uhandle = NULL;

    NvAPI_Status ret = NVAPI_OK;
    NvU32 i = 0;
    while(NvAPI_EnumNvidiaUnAttachedDisplayHandle(i, &uhandle) != NVAPI_END_ENUMERATION)
    {
        NvAPI_ShortString name;
        NvAPI_GetUnAttachedAssociatedDisplayName(uhandle, name);
        printf("Unattached Display %u : %s\n", i, name);
        ++i;
    }
}

void NVApiManager::dumpAllDisplayIds()
{
    NvAPIDisplayIDs displayIDs;

    const char* nvConnTypeConverter[] = {
        "NV_MONITOR_CONN_TYPE_UNINITIALIZED",
        "NV_MONITOR_CONN_TYPE_VGA",
        "NV_MONITOR_CONN_TYPE_COMPONENT",
        "NV_MONITOR_CONN_TYPE_SVIDEO",
        "NV_MONITOR_CONN_TYPE_HDMI",
        "NV_MONITOR_CONN_TYPE_DVI",
        "NV_MONITOR_CONN_TYPE_LVDS",
        "NV_MONITOR_CONN_TYPE_DP",
        "NV_MONITOR_CONN_TYPE_COMPOSITE"
    };

    for (unsigned int i = 0; i < displayIDs.m_displayIdCount; ++i)
    {
        const NV_GPU_DISPLAYIDS& displayID = displayIDs.m_displayIDs[i];
        printf("Display %u  DisplayId %u IsActive %s IsOSVisible %s", 
            i, displayID.displayId, BOOLTOSTRING(displayID.isActive), BOOLTOSTRING(displayID.isOSVisible));
        printf("  Connector type: ");
        if (displayID.connectorType != -1)
        {
            printf("%s", nvConnTypeConverter[displayID.connectorType]);
        }
        else
        {
            printf("unknown");
        }
        printf("\n");
    }
}

void NVApiManager::dumpDisplayConfig()
{
    NvAPIDisplayConfig displayConfig(true);

    for (unsigned int i = 0; i < displayConfig.m_pathInfoCount; ++i)
    {
        printf("PathInfo %u IsNonNVidia %s\n", i, BOOLTOSTRING(displayConfig.m_pathInfos[i].IsNonNVIDIAAdapter));
        if (displayConfig.m_pathInfos[i].sourceModeInfo)
        {
            const NV_DISPLAYCONFIG_SOURCE_MODE_INFO& info = *displayConfig.m_pathInfos[i].sourceModeInfo;
            printf("    Resolution: %u x %u x %u\n", info.resolution.width, info.resolution.height, info.resolution.colorDepth);

            const char* colorFormatName;
            switch (info.colorFormat)
            {
            case NV_FORMAT_UNKNOWN      : colorFormatName = "NV_FORMAT_UNKNOWN";        break;
            case NV_FORMAT_P8           : colorFormatName = "NV_FORMAT_P8";             break;
            case NV_FORMAT_R5G6B5       : colorFormatName = "NV_FORMAT_R5G6B5";         break;
            case NV_FORMAT_A8R8G8B8     : colorFormatName = "NV_FORMAT_A8R8G8B8";       break;
            case NV_FORMAT_A16B16G16R16F: colorFormatName = "NV_FORMAT_A16B16G16R16F";  break;
            default                     : colorFormatName = "NV_FORMAT_UNKNOWN";        break;
            };

            printf("    Color format: %s\n", colorFormatName);
            printf("    Position: %u %u\n", info.position.x, info.position.y);
            printf("    GDIPrimary: %s  SLIFocus: %s\n", info.bGDIPrimary ? "true" : "false", info.bSLIFocus ? "true" : "false");
        }

        printf("    Targets: \n");
        for (unsigned int j = 0; j < displayConfig.m_pathInfos[i].targetInfoCount; ++j)
        {
            NV_DISPLAYCONFIG_PATH_TARGET_INFO& target = displayConfig.m_pathInfos[i].targetInfo[j];
            printf("      %u DisplayId: %u\n", j, target.displayId);
        }
    }
}

/// Switch the display that currently has displayId1 to displayId2, or vice versa.
void NVApiManager::swapDisplays(NvU32 displayId1, NvU32 displayId2)
{
    NvAPIDisplayConfig displayConfig;
    bool changesToApply = false;

    printf("Swap displayId %u --> displayId %u\n", displayId1, displayId2);
    for (unsigned int i = 0; i < displayConfig.m_pathInfoCount; ++i)
    {
        for (unsigned int j = 0; j < displayConfig.m_pathInfos[i].targetInfoCount; ++j)
        {
            NV_DISPLAYCONFIG_PATH_TARGET_INFO& target = displayConfig.m_pathInfos[i].targetInfo[j];

            // Do the swap.
            if (target.displayId == displayId1)
            {
                target.displayId = displayId2;
                changesToApply = true;
            }
            else if (target.displayId == displayId2)
            {
                target.displayId = displayId1;
                changesToApply = true;
            }
        }
    }

    if (changesToApply)
    {
        displayConfig.apply();
    }
}

/// Switch display to use a given displayId.
void NVApiManager::switchToDisplay(NvU32 display, NvU32 displayId1)
{
    NvAPIDisplayConfig displayConfig;
    bool changesToApply = false;

    printf("Switch display %u --> displayId %u\n", display, displayId1);
    for (unsigned int i = 0; i < displayConfig.m_pathInfoCount; ++i)
    {
        if (i != display) continue;

        for (unsigned int j = 0; j < displayConfig.m_pathInfos[i].targetInfoCount; ++j)
        {
            NV_DISPLAYCONFIG_PATH_TARGET_INFO& target = displayConfig.m_pathInfos[i].targetInfo[j];

            // Do the switch.
            target.displayId = displayId1;
            changesToApply = true;
        }
    }

    if (changesToApply)
    {
        displayConfig.apply();
    }
}
