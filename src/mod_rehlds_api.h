#pragma once

extern IRehldsApi* g_RehldsApi;
extern const RehldsFuncs_t* g_RehldsFuncs;
extern IRehldsHookchains* g_RehldsHookchains;
extern IRehldsServerStatic* g_RehldsServerStatic;
extern IRehldsServerData* g_RehldsServerData;

extern bool RehldsApi_Init();