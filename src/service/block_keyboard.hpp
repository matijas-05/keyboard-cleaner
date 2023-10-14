#pragma once

#ifdef __linux__
#include <string>
void blockKeyboard(std::string keyboardPath);
#else
void blockKeyboard();
#endif
