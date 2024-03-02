#include "precompiled.h"
#include "Print.h"

cvar_t* pcv_mp_logecho;
char g_GameDir[256];

void Utils_Init() {
	pcv_mp_logecho = g_engfuncs.pfnCVarGetPointer("mp_logecho");
}

void UTIL_LogPrintf(char* fmt, ...) {
	va_list			argptr;
	static char		string[1024];

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT(at_logged, "[SourceQuery] %s", string);
}

void UTIL_LogPrintf2(char* fmt, ...) {
	va_list			argptr;
	static char		string[1024];

	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT(at_logged, "%s", string);
}

void LCPrintf(bool critical, const char* fmt, ...) {
	va_list			argptr;
	static char		string[2048];
	static const char prefix[] = "[SourceQuery]: ";

	va_start(argptr, fmt);
	vsnprintf((sizeof(prefix) - 1) + string, sizeof(string) - (sizeof(prefix) - 1), fmt, argptr);
	va_end(argptr);
	memcpy(string, prefix, (sizeof(prefix) - 1));

	bool bNeedWriteInConsole = true, bNeedWriteInLog = true;
	if (bNeedWriteInConsole && bNeedWriteInLog)
	{
		if (pcv_mp_logecho && pcv_mp_logecho->value != 0.0)
			bNeedWriteInConsole = false;
	}

	if (bNeedWriteInConsole)
		SERVER_PRINT(string);

	if (bNeedWriteInLog)
		ALERT(at_logged, string);
}