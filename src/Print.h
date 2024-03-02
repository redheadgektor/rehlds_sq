#pragma once
#include "precompiled.h"

extern char g_GameDir[256];

extern void Utils_Init();
extern void UTIL_LogPrintf(char* fmt, ...);
extern void UTIL_LogPrintf2(char* fmt, ...);
extern void LCPrintf(bool critical, const char* fmt, ...);