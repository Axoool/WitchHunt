#include "FileDialog.hpp"

#if defined(TRMN_MACOS)

#import <Cocoa/Cocoa.h>
#include <string>

namespace Termina {
    std::string FileDialog::OpenFile()
    {
        std::string result;

        @autoreleasepool {
            NSOpenPanel* panel = [NSOpenPanel openPanel];
            panel.allowsMultipleSelection = NO;
            panel.canChooseFiles = YES;
            panel.canChooseDirectories = NO;

            NSInteger response = [panel runModal];
            if (response == NSModalResponseOK) {
                NSURL* url = [panel URL];
                NSString* path = [url path];
                if (path) {
                    const char* cstr = [path UTF8String];
                    if (cstr) result = std::string(cstr);
                }
            }
        }

        return result;
    }

    std::string FileDialog::SaveFile()
    {
        std::string result;

        @autoreleasepool {
            NSSavePanel* panel = [NSSavePanel savePanel];

            NSInteger response = [panel runModal];
            if (response == NSModalResponseOK) {
                NSURL* url = [panel URL];
                NSString* path = [url path];
                if (path) {
                    const char* cstr = [path UTF8String];
                    if (cstr) result = std::string(cstr);
                }
            }
        }

        return result;
    }
} // namespace Termina

#endif
