#pragma once

#ifdef __linux__
#include <string>
void blockKeys(std::string keyboardPath);
#else
void blockKeys();
#endif
