#include "sdk.h"
#include "vmt.h"
#include "demonames.h"

CEngine* g_pEngine;
CEntList* g_pEntList;
CVMTHookManager* g_pEngineHook;

CreateInterface_t g_pEngineFactory, g_ClientFactory;

typedef bool(__thiscall* GetPlayerInfoFn)(void*, int, player_info_t*);
GetPlayerInfoFn oPlayerInfo;

bool __fastcall hkGetPlayerInfo(void* ecx, void* edx, int ent_id, player_info_t *info) 
{
	bool rt = oPlayerInfo(ecx, ent_id, info);

	std::map<int, std::string>::iterator it = demonames::table.find(ent_id);
	if (it != demonames::table.end())
		strcpy(info->name, it->second.c_str());
	if (demonames::hide_avatars)
		info->friendsID = 0;

	return rt; 
} 

DWORD WINAPI DllAttach(LPVOID base)  
{
	g_pEngineFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
	g_ClientFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");
	g_pEngine = (CEngine*)g_pEngineFactory("VEngineClient013", 0); // VEngineClient011 for v34
	g_pEntList = (CEntList*)g_ClientFactory("VClientEntityList003", 0);

	if (g_pEntList && g_pEntList)
	{
		g_pEngineHook = new CVMTHookManager((PDWORD*)g_pEngine);
		oPlayerInfo = (GetPlayerInfoFn)g_pEngineHook->dwHookMethod((DWORD)hkGetPlayerInfo, 8);
		demonames::list();
	}

	system("cls");
	std::cout << "close me!";
	EnableMenuItem(GetSystemMenu(GetConsoleWindow(), 0), SC_CLOSE, MF_ENABLED);
	FreeConsole();
	g_pEngineHook->UnHook();
	Sleep(125);
	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

	return 0; 
} 

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwR, LPVOID lpR)
{
	if (dwR == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		HWND hw = GetConsoleWindow();
		MoveWindow(hw, 70, 210, 500, 600, 1);
		EnableMenuItem(GetSystemMenu(hw, 0), SC_CLOSE, MF_DISABLED);
		SetConsoleTitleA("DemoNames");
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		DisableThreadLibraryCalls(hInst);
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DllAttach, hInst, 0, 0);
	}
	return TRUE;
}