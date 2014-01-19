// GiDpiUtil.cpp
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>
#include <sys/sysctl.h>

int giGetScreenDpi()
{
    static int dpi = 0;
    
    if (dpi == 0) {
        size_t size = 15;
        char machine[15 + 1] = "";
        
        sysctlbyname("hw.machine", machine, &size, NULL, 0);
        
        if (strcmp(machine, "i386") == 0) {     // Simulator
            dpi = 72;
        } else {    // Identifier: http://theiphonewiki.com/wiki/Models
            bool iPadMini = (strcmp(machine, "iPad2,5") == 0 ||
                             strcmp(machine, "iPad2,6") == 0 ||
                             strcmp(machine, "iPad2,7") == 0 ||
                             strcmp(machine, "iPad4,4") == 0 ||
                             strcmp(machine, "iPad4,5") == 0);
            BOOL iPad = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
            
            dpi = (iPad && !iPadMini) ? 132 : 163;
        }
    }
    
    return dpi;
}
