#ifndef __NVAPI_SUPPORT_H__
#define __NVAPI_SUPPORT_H__

#include "nvapi.h"

class NVApiManager
{
public:
    static NVApiManager* getInstance();

    void dumpAttachedDisplays();
    void dumpUnAttachedDisplays();
    void dumpAllDisplayIds();
    void dumpDisplayConfig();

    /// Switch the display that currently has displayId1 to displayId2, or vice versa.
    void swapDisplays(NvU32 displayId1, NvU32 displayId2);

    /// Switch display to use a given displayId.
    void switchToDisplay(NvU32 display, NvU32 displayId1);

private:
    NVApiManager();
    ~NVApiManager();
};


#endif  // __NVAPI_SUPPORT_H__