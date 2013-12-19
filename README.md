# AVSwitcher

Utility to switch displays and audio devices on Windows

## Purpose

The goal of this little utility is to switch displays and audio devices.

If you have a PC which you use both for regular "work" and for watching movies or gaming on a TV, you may want to automate switching between different video and audio outputs. AVSwitcher allows you to do this using for example AutoHotKey to it to a key.

## Example

I normally use my PC with 2 1920x120\. monitors and with audio output through my PC's default audio chipset (either speakers or headphones).

I sometimes want to watch movies or TV series, or play games, on my big screen TV. The TV is 1920x1080\. and I want to view my primary monitor on it, but I do not want to always set the TV to clone the primary monitor, because it would then force that monitor to only output 1920x1080\. Also, in that case I want the audio output to go through the HDMI audio output, not the default audio chipset.

Having to go to NVidia control panel to enable and disable cloning of the primary monitor on the TV, and switching the audio output to HDMI and back, would involve a lot of clicking each time. Instead, I made this little utility which I can bind to Ctrl-Alt-F1 to switch to the TV, and Ctrl-Alt-F2 to switch back, and so when I get in front of my TV, I just grab my wireless keyboard, hit the keyboard shortcut, and voilà, everything is switched.

## Limitations

Audio switching uses Windows APIs so it should work on any hardware.

I currently only have video output switching implemented for NVidia hardware (which is what I use obviously).

If this should ever be made to work on both NVidia and AMD hardware, I suspect some of the inherent assumptions that made me design the program the way it is would no longer be true. It may require pretty deep changes. But I welcome any contributions to make that happen, as it will only make this utility more useful.

## Command line

* `AVSwitcher`

Without any command line arguments, it will simply output the list of audio and video devices and exit.

* `AVSwitcher swapAudioDevices [nameSubstr1] [nameSubstr2]`

Will swap the default attribute between two devices (if device1 was default, device2 will be set as default, and vice versa). You can give a substring of the name, just make sure it's unique enough because the first device in the list that matches that substring will be used.

Example: `AVSwitcher swapAudioDevices Speakers VSX-925`

* `AVSwitcher switchToAudioDevice [nameSubstr]`

Sets the device whose name matches the given substring as default. If it was already default, does nothing.

Example: `AVSwitcher switchToAudioDevice VSX-925`

* `AVSwitcher swapDisplays displayId1 displayId2`

Swaps the targets of the two given displays (if displayId1 was on the left monitor and displayId2 on the right, they will be inverted).

Example: `AVSwitcher swapDisplays 2147881090 2147881089`

* `AVSwitcher switchToDisplay display target displayId`

Switches the output identified as display:target to the given displayId. If the output is your primary display, then display is 0, else it's a higher number. If the output is not cloned, then target is 0.

Example: `AVSwitcher switchToDisplay 0 0 2147881090`

* `AVSwitcher cloneDisplay displayId1 displayId2`

Sets displayId2 to clone displayId1. If displayId2 is 0, all clones are removed and displayId1 is the only output left for that display.

Example: `AVSwitcher cloneDisplay 2147881090 2147881089`
Example: `AVSwitcher cloneDisplay 2147881090 0`

## Licensing

I have used the Apache license as I wanted a permissive license.

Note that I have included NVAPI in the source. IANAL, but having read the NVAPI license, it seems to imply that I have the right to copy and redistribute it as long as I don't modify it or remove the copyright notices, or try to pass it off as my own work. If this is not the case, I can remove it and provide instructions as to where to place the files so that people who want to build AVSwitcher can get it themselves. Just let me know. I have made no attempt to infringe on anything by this.
