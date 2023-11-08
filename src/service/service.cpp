#include <cstring>
#include <string>
#include <thread>
#include "../config.hpp"
#include "log.c/log.h"

#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>
#include <unistd.h>
#include "block_keyboard.hpp"

int childPid = 0;

#if defined(__linux__)
void disableKeyboard(std::string keyboardPath) {
#else
void disableKeyboard() {
#endif
    if (childPid != 0) {
        log_warn("Keyboard is already disabled");
        return;
    }

    // Fork on linux and mac, because it's the easiest way to do it on mac,
    // and it can also be done this way on linux, even though it's not necessary
    int pid = fork();
    if (pid == -1) {
        log_error("Failed to fork process: %s", std::strerror(errno));
        return;
    } else if (pid == 0) {
        // Child process - disable keyboard and keep process running
#ifdef __linux__
        blockKeyboard(keyboardPath);
#else
        blockKeyboard();
#endif
    } else {
        // Parent process - save child pid
        childPid = pid;
    }
}
void enableKeyboard() {
    if (childPid == 0) {
        log_warn("Keyboard is already enabled");
        return;
    }

    if (kill(childPid, SIGTERM) == -1) {
        log_error("Failed to kill child process: %s", std::strerror(errno));
    } else {
        childPid = 0;
        log_info("Enabled keyboard");
    }
}
#elif defined(_WIN32)
#include <windows.h>

HHOOK keyboardHook = nullptr;
LRESULT CALLBACK Hook(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) {
        // Nonzero value means prevents the system from passing the message to the next
        // hook procedure in the current hook chain.
        return 1;
    } else {
        return CallNextHookEx(keyboardHook, code, wParam, lParam);
    }
}

void disableKeyboard() {
    keyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, Hook, NULL, 0);
    log_info("Disabled keyboard");
}
void enableKeyboard() {
    UnhookWindowsHookEx(keyboardHook);
    log_info("Enabled keyboard");
}
#endif

void runCommand(std::string command) {
    if (command[0] == *DISABLE) {
#ifdef __linux__
        disableKeyboard(command.substr(2));  // 1 is space, 2 is the first character of the path
#else
        disableKeyboard();
#endif
    } else if (command[0] == *ENABLE) {
        enableKeyboard();
    } else {
        log_error("Unknown command: '%s'", command.c_str());
    }
}

#ifdef __APPLE__
#include "pipe_reader.hpp"
int main() {
    log_info("Starting keyboard disable service...");

    if (std::atexit(&enableKeyboard) == -1) {
        log_error("Failed to register exit handler: %s", std::strerror(errno));
        return 1;
    }

    PipeReader pipeReader;
    if (pipeReader.open() == -1) {
        log_error("Failed to open named pipe: %s", std::strerror(errno));
        return 1;
    }
    log_debug("Opened named pipe");

    while (true) {
        std::string buf = pipeReader.read();
        if (!buf.empty()) {
            log_debug("Read from named pipe: '%s'", buf.c_str());
            runCommand(buf);
        }
        // For when the pipe is closed from the tray's side
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
#endif
