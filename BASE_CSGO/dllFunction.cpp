#include "dllFunction.h"
//#include <Windows.h>
//#include "c0gnito.h"
//#include <string>
//#pragma comment(lib, "c0gnito.lib")

CValve g_Valve;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

PaintTraverseFn oPaintTraverse;
RunCommandFn oRunCommand;

/*int baseCheck()
{
	if (Initialize("5WkJWddwiM2aCrMrVxaARyjsT2MiRte3a52OSlRf7Crtpq9J1JnApuRQsNFHkZyX"))
	{
		std::cout << "Database connected...\n";
	}
	else
	{
		std::cout << "No find database...\n";
		Sleep(2000);
		exit(1);
	}

	system("clr");
	std::cout << "Enter your key: ";
	std::string key;
	std::cin >> key;
	std::string hwid = GetHardwareID();

	if (Authenticate(key.c_str(), hwid.c_str()))
	{

	}
	else
		exit(1);
}*/

static HMODULE DllBaseAddress()
{
	MEMORY_BASIC_INFORMATION info;
	size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)DllBaseAddress, &info, sizeof(info));
	Assert(len == sizeof(info));
	return len ? (HMODULE)info.AllocationBase : NULL;
}

// stolen from keybode's src
void ApplySendPacket(void)
{
	auto CL_RunPrediction = (uintptr_t)(GetModuleHandleW(L"engine.dll")) + 0x464D4;

	MEMORY_BASIC_INFORMATION Memory = { 0 };
	VirtualQuery((LPCVOID)CL_RunPrediction, &Memory, sizeof(Memory));

	if (Memory.Protect != PAGE_NOACCESS)
	{
		DWORD Protect = 0;

		if (VirtualProtect((LPVOID)CL_RunPrediction, 5, PAGE_EXECUTE_READWRITE, &Protect) != FALSE)
		{
			memset((void*)CL_RunPrediction, 0x90, 5);
			VirtualProtect((LPVOID)CL_RunPrediction, 5, Protect, &Protect);
		}
	}
}

void InitDllThread(void* ptr)
{
	g_Valve.initSDK();

	if (!g_Valve.isInitiated())
		return;

	g_NetworkedVariableManager.Init();


	g_Valve.pInput = **(ValveSDK::CInput***)(Base::Utils::PatternSearch("client.dll", (PBYTE)"\x8B\x0D\x00\x00\x00\x00\x8B\x11\x50\x8B\x44\x24\x10", "xx????xxxxxxx", NULL, NULL) + 2);

	if (g_pPanelVMT.bInitialize((PDWORD*)g_Valve.pPanel))
		oPaintTraverse = (PaintTraverseFn)g_pPanelVMT.dwHookMethod((DWORD)hkdPaintTraverse, 40);

	g_pClientVMT.bInitialize((PDWORD*)g_Valve.pClient);
	dwOriginCreateMove = g_pClientVMT.dwGetMethodAddress(18);
	g_pClientVMT.dwHookMethod((DWORD)hkdCreateMove, 18);

	g_pInputVMT.bInitialize((PDWORD*)g_Valve.pInput);
	g_pInputVMT.dwHookMethod((DWORD)hkdGetUserCmd, 8);

	g_pClientVMT.dwHookMethod((DWORD)hkdFrameStageNotify, 32);

	if (g_pPredictionVMT.bInitialize((PDWORD*)g_Valve.pPred))
		oRunCommand = (RunCommandFn)g_pPredictionVMT.dwHookMethod((DWORD)hkdRunCommand, 19);

	g_pModelRenderVMT.bInitialize((PDWORD*)g_Valve.pModelRender);
	g_pModelRenderVMT.dwHookMethod((DWORD)hkdDrawModelEx, 19);

	g_NetworkedVariableManager.HookProp("DT_CSPlayer", "m_angEyeAngles[0]", PitchEyeAngleProxy);
	g_NetworkedVariableManager.HookProp("DT_CSPlayer", "m_angEyeAngles[1]", YawEyeAngleProxy);
	gGameEventManager.RegisterSelf();

	ApplySendPacket();

	if (g_Valve.pMaterialSystem)
	{
		if (!pNew)
			pNew = CreateMaterial(FALSE, FALSE);

		if (!pIgnoreNew)
			pIgnoreNew = CreateMaterial(FALSE, TRUE);

		if (!pBrightNew)
			pBrightNew = CreateMaterial(TRUE, FALSE);

		if (!pBrightIgnoreNew)
			pBrightIgnoreNew = CreateMaterial(TRUE, TRUE);
	}
}

#include "ReflectiveLoader.h"
#include "ReflectiveDLLInjection.h"

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitDllThread, hinstDLL, NULL, NULL);

		Config.Init(hinstDLL);
		Config.SetDefaultConfig();
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}