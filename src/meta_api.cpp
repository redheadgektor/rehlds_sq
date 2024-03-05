// meta_api.cpp - minimal implementation of metamod's plugin interface

// This is intended to illustrate the (more or less) bare minimum code
// required for a valid metamod plugin, and is targeted at those who want
// to port existing HL/SDK DLL code to run as a metamod plugin.

/*
 * Copyright (c) 2001-2003 Will Day <willday@hpgx.net>
 *
 *    This file is part of Metamod.
 *
 *    Metamod is free software; you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation; either version 2 of the License, or (at
 *    your option) any later version.
 *
 *    Metamod is distributed in the hope that it will be useful, but
 *    WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Metamod; if not, write to the Free Software Foundation,
 *    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    In addition, as a special exception, the author gives permission to
 *    link the code of this program with the Half-Life Game Engine ("HL
 *    Engine") and Modified Game Libraries ("MODs") developed by Valve,
 *    L.L.C ("Valve").  You must obey the GNU General Public License in all
 *    respects for all of the code used other than the HL Engine and MODs
 *    from Valve.  If you modify this file, you may extend this exception
 *    to your version of the file, but you are not obligated to do so.  If
 *    you do not wish to do so, delete this exception statement from your
 *    version.
 *
 */
#include "precompiled.h"
#include <list>
 // Must provide at least one of these..
static META_FUNCTIONS gMetaFunctionTable = {
	NULL,			// pfnGetEntityAPI				HL SDK; called before game DLL
	NULL,			// pfnGetEntityAPI_Post			META; called after game DLL
	GetEntityAPI2,	// pfnGetEntityAPI2				HL SDK2; called before game DLL
	NULL,			// pfnGetEntityAPI2_Post		META; called after game DLL
	NULL,			// pfnGetNewDLLFunctions		HL SDK2; called before game DLL
	NULL,			// pfnGetNewDLLFunctions_Post	META; called after game DLL
	GetEngineFunctions,	// pfnGetEngineFunctions	META; called before HL engine
	NULL,			// pfnGetEngineFunctions_Post	META; called after HL engine
};

// Description of plugin
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	"REHLDS-SourceQuery",	// name
	"0.0.3",	// version
	__DATE__,	// date
	"t.me/redheadgektor",	// author
	"",	// url
	"SourceQuery",	// logtag, all caps please
	PT_STARTUP,	// (when) loadable
	PT_NEVER,	// (when) unloadable
};

// Global vars from metamod:
meta_globals_t* gpMetaGlobals;		// metamod globals
gamedll_funcs_t* gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t* gpMetaUtilFuncs;		// metamod utility functions

// Metamod requesting info about this plugin:
//  ifvers			(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod
C_DLLEXPORT int Meta_Query(char* /*ifvers */, plugin_info_t** pPlugInfo, mutil_funcs_t* pMetaUtilFuncs)
{
	*pPlugInfo = &Plugin_info;
	gpMetaUtilFuncs = pMetaUtilFuncs;
	return(TRUE);
}


/* SourceQuery */
const int RESPONCE_AS_SINGLE_PACKET = 0xFFFFFFFF;
const int RESPONCE_AS_MULTIPLE_PACKET = 0xFFFFFFFE;

enum QueryID
{
	/* Requests from clients */
	A2S_INFO = 'T',
	A2S_PLAYER = 'U',
	A2S_RULES = 'V',
	A2S_SERVERQUERY_GETCHALLENGE = 'W',

	/* Responce from server to clients */
	S2C_CHALLENGE = 'A',
	S2A_PLAYER = 'D',
	S2A_RULES = 'E',
	S2A_INFO_SRC = 'I',
	S2A_INFO_OLD = 'm'
};

#ifdef _WIN32
const char default_env = 'w';
#elif defined(linux)
const char default_env = 'l';
#endif

//cvars
cvar_t* cvarp_hostname;
cvar_t* cvarp_password;
cvar_t* cvarp_tags;
cvar_t* cvarp_version;

cvar_t sq_showpackets = { "sq_showpackets", "0", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_showpackets;

cvar_t sq_query = { "sq_query", "1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_query;

cvar_t sq_hostname = { "sq_hostname", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_hostname;

cvar_t sq_hideplayers = { "sq_hideplayers", "0", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_hideplayers;

cvar_t sq_hideplayersname = { "sq_hideplayersname", "0", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_hideplayersname;


cvar_t sq_hiderules = { "sq_hiderules", "0", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_hiderules;

//cvars for A2S_INFO, S2A_INFO_SRC
cvar_t sq_protocol = { "sq_protocol", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_protocol;

cvar_t sq_map = { "sq_map", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_map;

cvar_t sq_gamedir = { "sq_gamedir", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_gamedir;

cvar_t sq_gamename = { "sq_gamename", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_gamename;

cvar_t sq_appid = { "sq_appid", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_appid;

cvar_t sq_players = { "sq_players", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_players;

cvar_t sq_maxplayers = { "sq_maxplayers", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_maxplayers;

cvar_t sq_bots = { "sq_bots", "0", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_bots;

cvar_t sq_servertype = { "sq_servertype", "0", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_servertype;

cvar_t sq_serverenv = { "sq_serverenv", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_serverenv;

cvar_t sq_password = { "sq_password", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_password;

cvar_t sq_vac = { "sq_vac", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_vac;

cvar_t sq_gameversion = { "sq_gameversion", "1.1.2.7/Stdio", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_gameversion;

cvar_t sq_tags = { "sq_tags", "-1", FCVAR_EXTDLL, 0.0f, NULL };
cvar_t* p_sq_tags;

void SQ_HELP()
{
	UTIL_LogPrintf2("\n\n------- SQ HELP -------\nsq_query (def. 1) - mode of processing queries\n	0 - disabled (handling by Reunion, Dproto or Steam)\n	1 - Query sets params from cvars and instant response\n	2 - Only Steam Query\n	3 - Ignore all Queries\nsq_showpackets (def. 0) - show all queries packets (receive/send)\nsq_hideplayerslist (def. 0) - hide all players in list from query\nsq_hiderules (def. 0) - hide all rules/cvars from query\nsq_tags (def. -1) - override 'sv_tags' cvar (work only Steam)\n\n\n");
}

void InitCvars()
{
	g_engfuncs.pfnAddServerCommand("sq_help", SQ_HELP);

	//cache cvars
	cvarp_hostname = g_engfuncs.pfnCVarGetPointer("hostname");
	cvarp_password = g_engfuncs.pfnCVarGetPointer("sv_password");
	cvarp_tags = g_engfuncs.pfnCVarGetPointer("sv_tags");
	cvarp_version = g_engfuncs.pfnCVarGetPointer("sv_version");

	//register cvars
	g_engfuncs.pfnCvar_RegisterVariable(&sq_showpackets);
	p_sq_showpackets = g_engfuncs.pfnCVarGetPointer(sq_showpackets.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_query);
	p_sq_query = g_engfuncs.pfnCVarGetPointer(sq_query.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_hostname);
	p_sq_hostname = g_engfuncs.pfnCVarGetPointer(sq_hostname.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_hideplayers);
	p_sq_hideplayers = g_engfuncs.pfnCVarGetPointer(sq_hideplayers.name);
	
	g_engfuncs.pfnCvar_RegisterVariable(&sq_hideplayersname);
	p_sq_hideplayersname = g_engfuncs.pfnCVarGetPointer(sq_hideplayersname.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_hiderules);
	p_sq_hiderules = g_engfuncs.pfnCVarGetPointer(sq_hiderules.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_protocol);
	p_sq_protocol = g_engfuncs.pfnCVarGetPointer(sq_protocol.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_map);
	p_sq_map = g_engfuncs.pfnCVarGetPointer(sq_map.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_gamedir);
	p_sq_gamedir = g_engfuncs.pfnCVarGetPointer(sq_gamedir.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_gamename);
	p_sq_gamename = g_engfuncs.pfnCVarGetPointer(sq_gamename.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_appid);
	p_sq_appid = g_engfuncs.pfnCVarGetPointer(sq_appid.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_players);
	p_sq_players = g_engfuncs.pfnCVarGetPointer(sq_players.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_maxplayers);
	p_sq_maxplayers = g_engfuncs.pfnCVarGetPointer(sq_maxplayers.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_bots);
	p_sq_bots = g_engfuncs.pfnCVarGetPointer(sq_bots.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_servertype);
	p_sq_servertype = g_engfuncs.pfnCVarGetPointer(sq_servertype.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_serverenv);
	p_sq_serverenv = g_engfuncs.pfnCVarGetPointer(sq_serverenv.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_password);
	p_sq_password = g_engfuncs.pfnCVarGetPointer(sq_password.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_vac);
	p_sq_vac = g_engfuncs.pfnCVarGetPointer(sq_vac.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_gameversion);
	p_sq_gameversion = g_engfuncs.pfnCVarGetPointer(sq_gameversion.name);

	g_engfuncs.pfnCvar_RegisterVariable(&sq_tags);
	p_sq_tags = g_engfuncs.pfnCVarGetPointer(sq_tags.name);

}


enum {
	GAME_APPID_CSTRIKE = 10,
	GAME_APPID_TFC = 20,
	GAME_APPID_DOD = 30,
	GAME_APPID_DMC = 40,
	GAME_APPID_GEARBOX = 50,
	GAME_APPID_RICOCHET = 60,
	GAME_APPID_VALVE = 70,
	GAME_APPID_CZERO = 80,
	GAME_APPID_CZEROR = 100,
	GAME_APPID_BSHIFT = 130,
	GAME_APPID_CSTRIKE_BETA = 150,
};

typedef struct GameToAppIDMapItem_s
{
	size_t appID;
	const char* dir;
} GameToAppIDMapItem_t;

GameToAppIDMapItem_t m_GameToAppIDMap[] =
{
	{ GAME_APPID_CSTRIKE,      "cstrike" },
	{ GAME_APPID_TFC,          "tfc" },
	{ GAME_APPID_DOD,          "dod" },
	{ GAME_APPID_DMC,          "dmc" },
	{ GAME_APPID_GEARBOX,      "gearbox" },
	{ GAME_APPID_RICOCHET,     "ricochet" },
	{ GAME_APPID_VALVE,        "valve" },
	{ GAME_APPID_CZERO,        "czero" },
	{ GAME_APPID_CZEROR,       "czeror" },
	{ GAME_APPID_BSHIFT,       "bshift" },
	{ GAME_APPID_CSTRIKE_BETA, "cstrike_beta" }
};

int GetGameAppID(const char* gamedir)
{
	for (auto& game : m_GameToAppIDMap) {
		if (!strcmp(game.dir, gamedir)) {
			return game.appID;
		}
	}

	return GAME_APPID_VALVE;
}

BinaryWriter stream(10000);

int ActivePlayersCount()
{
	int clients = 0;
	for (int i = 0; i < g_RehldsServerStatic->GetMaxClients(); i++)
	{
		IGameClient* client = g_RehldsServerStatic->GetClient(i);
		if (client != 0)
		{
			if (client->IsActive() && client->IsConnected())
			{
				clients++;
			}
		}
	}

	return clients;
}

void Info_SourceQuery()
{
	stream.pointer = 0;
	stream.write4(0xFFFFFFFF);
	stream.write1((char)S2A_INFO_SRC);
	if (p_sq_protocol->value != -1)
	{
		stream.write1(p_sq_protocol->value);
	}
	else
	{
		stream.write1(0x30);
	}

	//hostname
	if (p_sq_hostname->value != -1)
	{
		stream.writeS(p_sq_hostname->string);
	}
	else
	{

		stream.writeS(cvarp_hostname->string);
	}

	//map
	if (p_sq_map->value != -1)
	{
		stream.writeS(p_sq_map->string);
	}
	else
	{

		stream.writeS(g_RehldsServerData->GetName());
	}

	//game dir
	if (p_sq_gamedir->value != -1)
	{
		stream.writeS(p_sq_gamedir->string);
	}
	else
	{
		stream.writeS(g_GameDir);
	}

	//game name
	if (p_sq_gamename->value != -1)
	{
		stream.writeS(p_sq_gamename->string);
	}
	else
	{
		stream.writeS(MDLL_GetGameDescription());
	}

	//appid
	if (p_sq_appid->value != -1)
	{
		stream.write2(p_sq_appid->value);
	}
	else
	{
		stream.write2(GetGameAppID(g_GameDir));
	}

	//players
	if (p_sq_players->value == -1)
	{
		stream.write1(ActivePlayersCount());
	}
	else
	{
		stream.write1(p_sq_players->value);
	}

	if (p_sq_maxplayers->value == -1)
	{
		stream.write1(gpGlobals->maxClients);
	}
	else
	{
		stream.write1(p_sq_maxplayers->value);
	}

	//bots
	stream.write1(p_sq_bots->value);

	//server type
	if (p_sq_servertype->value <= 0)
	{
		stream.write1('d');//dedicated
	}
	else if (p_sq_servertype->value == 1)
	{
		stream.write1('l');//listen
	}
	else if (p_sq_servertype->value > 1)
	{
		stream.write1('p');//proxy (hltv or sourceTV)
	}

	//server environment (OS)
	if (p_sq_serverenv->value == -1)
	{
		stream.write1(default_env);
	}
	else if (p_sq_serverenv->value == 0)
	{
		stream.write1('w');
	}
	else if (p_sq_serverenv->value == 1)
	{
		stream.write1('l');
	}
	else if (p_sq_serverenv->value == 2)
	{
		stream.write1('m');
	}

	//has password
	if (p_sq_password->value != -1)
	{
		stream.write1(p_sq_password->string ? 1 : 0);
	}
	else
	{
		stream.write1(strlen(cvarp_password->string) ? 1 : 0);
	}

	//has vac
	if (p_sq_vac->value != -1)
	{
		stream.write1(p_sq_vac->value ? 1 : 0);
	}
	else
	{
		stream.write1(g_RehldsFuncs->GSBSecure());
	}

	//Game Version
	if (p_sq_gameversion->value != -1)
	{
		stream.writeS(p_sq_gameversion->string);
	}
	else
	{
		stream.writeS(cvarp_version->string);
	}
}


/* Players */

typedef struct S2A_Player_s
{
	byte index;
	const char* name;
	int score;
	int time;
} S2A_Player_t;

void Players_SourceQuery() 
{
	stream.pointer = 0;
	stream.write4(0xFFFFFFFF);
	stream.write1((char)S2A_PLAYER);

	int count = 0;
	int count_ptr = stream.pointer;
	stream.pointer++;

	//FIXED: FATAL ERROR (shutting down): GetClient: invalid id provided: 32
	for (int i = 0; i < g_RehldsServerStatic->GetMaxClientsLimit(); i++)
	{
		IGameClient* client = g_RehldsServerStatic->GetClient(i);

		if (client->IsActive() && client->IsConnected()) 
		{
			client_t* cl = g_RehldsServerStatic->GetClient_t(i);
			stream.write1(i);
			stream.writeS(p_sq_hideplayersname->value > 0 ? "Hidden" : client->GetName());
			stream.write4(client->GetEdict()->v.frags);

			if (!client->IsFakeClient()) {
				stream.write4float(g_RehldsApi->GetFuncs()->GetRealTime() - cl->netchan.connect_time);
			}
			else 
			{
				stream.write4float(RANDOM_FLOAT(0,10000));
			}
			count++;
		}
	}

	int old = stream.pointer;
	stream.pointer = count_ptr;
	stream.write1(count);
	stream.pointer = old;
}

void Rules_SourceQuery()
{
	stream.pointer = 0;
	stream.write4(0xFFFFFFFF);
	stream.write1((char)S2A_RULES);

	int count = 1;
	int count_ptr = stream.pointer;
	stream.pointer +=2;

	stream.writeS("Cvars hidden!");
	stream.writeS("-");

	int old = stream.pointer;
	stream.pointer = count_ptr;
	stream.write2(count);
	stream.pointer = old;
}

void SendQueryData(const netadr_t& srcAddr)
{
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = srcAddr.port;
	memcpy(&saddr.sin_addr, srcAddr.ip, 4);

	g_RehldsFuncs->NET_SendPacket(stream.pointer, stream.buffer, srcAddr);
}

bool PreProcessHook(IRehldsHook_PreprocessPacket* chain, uint8* data, unsigned int len, const netadr_t& srcAddr)
{
	if (p_sq_query->value > 0)
	{
		if (len >= 5)
		{
			if (p_sq_query->value == 1)
			{
				if (data[0] == 0xFF && data[1] == 0xFF && data[3] == 0xFF)
				{
					if (data[4] == A2S_INFO)
					{
						if (p_sq_showpackets->value > 0)
						{
							UTIL_LogPrintf("In -> A2S_INFO | sz %i | from %i.%i.%i.%i:%i\n", len, srcAddr.ip[0], srcAddr.ip[1], srcAddr.ip[2], srcAddr.ip[3], srcAddr.port);
						}

						Info_SourceQuery();
						SendQueryData(srcAddr);

						if (p_sq_showpackets->value > 0)
						{
							UTIL_LogPrintf("Out -> S2A_INFO_SRC send %i to %i.%i.%i.%i:%i\n", stream.pointer, srcAddr.ip[0], srcAddr.ip[1], srcAddr.ip[2], srcAddr.ip[3], srcAddr.port);
						}

						return false;
					}

					if (data[4] == A2S_PLAYER)
					{
						if (p_sq_hideplayers->value != 0)
						{
							return false;
						}

						if (p_sq_showpackets->value > 0) {
							UTIL_LogPrintf("In -> A2S_PLAYER | sz %i | from %i.%i.%i.%i:%i\n", len, srcAddr.ip[0], srcAddr.ip[1], srcAddr.ip[2], srcAddr.ip[3], srcAddr.port);
						}

						Players_SourceQuery();
						SendQueryData(srcAddr);

						if (p_sq_showpackets->value > 0)
						{
							UTIL_LogPrintf("Out -> S2A_PLAYER send %i to %i.%i.%i.%i:%i\n", stream.pointer, srcAddr.ip[0], srcAddr.ip[1], srcAddr.ip[2], srcAddr.ip[3], srcAddr.port);
						}
						
						return false;
					}

					if (data[4] == A2S_RULES)
					{
						if (p_sq_hiderules->value > 0)
						{
							return false;
						}
						if (p_sq_showpackets->value > 0) {
							UTIL_LogPrintf("In -> A2S_RULES | sz %i | from %i.%i.%i.%i:%i\n", len, srcAddr.ip[0], srcAddr.ip[1], srcAddr.ip[2], srcAddr.ip[3], srcAddr.port);
						}

						Rules_SourceQuery();
						SendQueryData(srcAddr);

						if (p_sq_showpackets->value > 0)
						{
							UTIL_LogPrintf("Out -> S2A_RULES send %i to %i.%i.%i.%i:%i\n", stream.pointer, srcAddr.ip[0], srcAddr.ip[1], srcAddr.ip[2], srcAddr.ip[3], srcAddr.port);
						}
						
						return false;
					}

				}
			}

			//pass to steam handler
			if (p_sq_query->value == 2)
			{
				uint32_t ip;
				memcpy(&ip, srcAddr.ip, 4);
				if (g_RehldsServerData->GetSteamGameServer()->HandleIncomingPacket(data, len, ip, srcAddr.port))
				{
					if (p_sq_showpackets->value > 0)
					{
						UTIL_LogPrintf("Handling query by Steam\n");
					}
				}
				return true;
			}

			//ignore all
			if (p_sq_query->value == 3)
			{
				return false;
			}
		}

		return true;
	}
	return chain->callNext(data, len, srcAddr);
}

bool Query_Init() {
	GET_GAME_DIR(g_GameDir);

	Utils_Init();

	if (!RehldsApi_Init())
	{
		LCPrintf(true, "Failed to locate REHLDS API\n");
		return false;
	}
	else
	{
		UTIL_LogPrintf("REHLDS API Hooked! %s | Steam App ID: %i\n", g_GameDir, GetGameAppID(g_GameDir));
	}

	InitCvars();

	g_RehldsHookchains->PreprocessPacket()->registerHook(&PreProcessHook, HC_PRIORITY_UNINTERRUPTABLE);

	return true;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME, META_FUNCTIONS* pFunctionTable, meta_globals_t* pMGlobals, gamedll_funcs_t* pGamedllFuncs)
{
	if (!pMGlobals) {
		LOG_ERROR(PLID, "Meta_Attach called with null pMGlobals");
		return(FALSE);
	}
	gpMetaGlobals = pMGlobals;
	if (!pFunctionTable) {
		LOG_ERROR(PLID, "Meta_Attach called with null pFunctionTable");
		return(FALSE);
	}
	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
	gpGamedllFuncs = pGamedllFuncs;
	return Query_Init() ? (TRUE) : (FALSE);
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME /* now */,
	PL_UNLOAD_REASON /* reason */)
{
	return(TRUE);
}

