#if defined(__linux__)
#include <linux/input.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <cstring>

// TODO: Select device
#define DEVICE_PATH "/dev/input/event14"

#elif defined(__APPLE__)
#include <Carbon/Carbon.h>
#endif

#include <iostream>
#include "log.c/log.h"

void blockKeys() {
#if defined(__linux__)
    int fd = open(DEVICE_PATH, O_WRONLY);
    if (fd == -1) {
        log_error("Failed to open device: %s", std::strerror(errno));
        close(fd);
        return;
    }

    if (ioctl(fd, EVIOCGRAB, 1) == -1) {
        log_error("Failed to disable keyboard: %s", std::strerror(errno));
        close(fd);
        return;
    }

    log_info("Disabled keyboard");
#elif defined(__APPLE__)
    // https://chat.openai.com/share/40df0dc9-dd46-4203-bb44-c5eaf312e82a
    CFMachPortRef eventTap;
    CGEventMask mask = kCGEventMaskForAllEvents;

    eventTap = CGEventTapCreate(
        kCGHIDEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, mask,
        // Lambda function looks better than multiple #ifdefs
        [](CGEventTapProxy proxy, CGEventType type, CGEventRef event, void* refcon) -> CGEventRef {
            (void)proxy;
            (void)refcon;

            // If keyboard event, consume it
            if (type == kCGEventKeyDown || type == kCGEventKeyUp || type == kCGEventFlagsChanged) {
                return nullptr;  // Consumes the event, preventing it from being processed further
            } else {
                return event;
            }
        },
        nullptr);

    if (!eventTap) {
        log_error("Failed to create event tap");
        return;
    }
    log_info("Disabled keyboard");

    CFRunLoopSourceRef runLoopSource =
        CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);

    CFRunLoopRun();

    // Clean up
    CFRelease(runLoopSource);
    CFRelease(eventTap);
#endif
}
