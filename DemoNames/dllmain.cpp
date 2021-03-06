#include "vmt.h"
#include "demonames.h"
#include "console.h"

CEngine* g_pEngine;
CEntList* g_pEntList;
CVMTHookManager* g_pEngineHook;

CreateInterface_t EngineFactory, ClientFactory;

typedef bool(__thiscall* GetPlayerInfoFn)(void*, int, player_info_t*);
GetPlayerInfoFn orgGetPlayerInfo;

bool __fastcall hkGetPlayerInfo(void* ecx, void* edx, int entity_id, player_info_t *info) 
{
	bool original = orgGetPlayerInfo(ecx, entity_id, info);

	std::map<int, std::string>::iterator it = demonames::table.find(entity_id);

	if (it != demonames::table.end())
	{
		strcpy(info->name, it->second.c_str());
	}
	if (demonames::hide_avatars)
	{
		info->friendsID = 0;
	}
	if (demonames::hide_clantags)
	{
		demonames::clean_player_resources(entity_id);
	}
	 
	return original;
} 

DWORD WINAPI DllAttach(LPVOID hInst)
{
	EngineFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
	ClientFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");
	g_pEngine = (CEngine*)EngineFactory("VEngineClient013", 0);
	g_pEntList = (CEntList*)ClientFactory("VClientEntityList003", 0);

	if (g_pEngine && g_pEntList)
	{
		g_pEngineHook = new CVMTHookManager((PDWORD*)g_pEngine);
		orgGetPlayerInfo = (GetPlayerInfoFn)g_pEngineHook->dwHookMethod((DWORD)hkGetPlayerInfo, 8);
		demonames::list();
	}
	else 
	{
		std::cout << "this version isn't supported";
		Sleep(1250);
	} 

	console.destroy();
	if (g_pEngineHook != nullptr)
		g_pEngineHook->UnHook();
	Sleep(125);
	FreeLibraryAndExitThread(static_cast<HINSTANCE>(hInst), 0);
} 

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwreason, LPVOID lpreserved)
{
	if (dwreason == DLL_PROCESS_ATTACH)
	{
		console.setup();
		DisableThreadLibraryCalls(hInst);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DllAttach, hInst, 0, 0);
	}
	return TRUE; 
}