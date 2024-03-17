#include "dllFunction.h"

ValveSDK::CBaseCombatWeapon* CBaseEntity::GetActiveBaseCombatWeapon()
{
	static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0x8F, 12, 0xB341DDC2>("\xCB\xC4\xCE\xD1\xC0\xC4\xF9\xF7\xEE\xFD\xEB" + 0xB341DDC2).s,/*m_hActiveWeapon*/XorStr<0xA4, 16, 0x547EFF2E>("\xC9\xFA\xCE\xE6\xCB\xDD\xC3\xDD\xC9\xFA\xCB\xCE\xC0\xDE\xDC" + 0x547EFF2E).s);
	ULONG pWeepEhandle = *(PULONG)((DWORD)this + iOffset);
	return (ValveSDK::CBaseCombatWeapon*)(g_Valve.pEntList->GetClientEntityFromHandle(pWeepEhandle));
}

template< class T > T* GetInterface(std::string szModuleName, std::string szInterfaceName, bool bSkip = false, int iSkips = 0)
{
	if (szModuleName.empty() || szInterfaceName.empty())
		return nullptr;
	typedef PVOID(*CreateInterfaceFn)(const char* pszName, int* piReturnCode);
	CreateInterfaceFn hInterface = nullptr;
	while (!hInterface)
	{
		hInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(szModuleName.c_str()), "CreateInterface");
		Sleep(5);
	}

	char pszBuffer[256];
	for (int i = 0; i < 100; i++)
	{
		sprintf_s(pszBuffer, "%s%0.3d", szInterfaceName.c_str(), i);
		PVOID pInterface = hInterface(pszBuffer, nullptr);

		if (pInterface && pInterface != NULL)
		{
			if (bSkip)
				sprintf_s(pszBuffer, "%s%0.3d", szInterfaceName.c_str(), i + iSkips);

			Sleep(5);
			break;
		}
	}

#ifdef DEBUGMODE
	char szDebugString[1024];
	sprintf(szDebugString, "Found interface! [%s] -> [0x%X]", pszBuffer, (T*)hInterface(pszBuffer, nullptr));
	Base::Debug::LOG(szDebugString);
#endif
	return (T*)hInterface(pszBuffer, nullptr);
}

VOID CValve::initSDK()
{
#ifdef DEBUGMODE
	Base::Debug::LOG("Initiate Thread");
	char szDebugString[1024];
#endif

	pClient = GetInterface<ValveSDK::HLCLient>("client.dll", "VClient");
	pEntList = GetInterface<ValveSDK::CEntityList>("client.dll", "VClientEntityList");
	pPred = GetInterface<ValveSDK::CPrediction>("client.dll", "VClientPrediction");
	pGameMovement = GetInterface<ValveSDK::CGameMovement>("client.dll", "GameMovement");
	pEngine = GetInterface<ValveSDK::CEngineClient>("engine.dll", "VEngineClient", true, 1);
	pModel = GetInterface<ValveSDK::CModelInfo>("engine.dll", "VModelInfoClient");
	pDebugOverlay = GetInterface<ValveSDK::CDebugOverlay>("engine.dll", "VDebugOverlay");
	pEngineTrace = GetInterface<ValveSDK::CTrace>("engine.dll", "EngineTraceClient");
	pSurface = GetInterface<ValveSDK::ISurface>("vguimatsurface.dll", "VGUI_Surface", true, 2);
	pPanel = GetInterface<ValveSDK::IPanel>("vgui2.dll", "VGUI_Panel");
	pPhysics = GetInterface<ValveSDK::IPhysicsSurfaceProps>("vphysics.dll", "VPhysicsSurfaceProps");
	pModelRender = GetInterface<ValveSDK::IVModelRender>("engine.dll", "VEngineModel");
	pRenderView = GetInterface<ValveSDK::IVRenderView>("engine.dll", "VEngineRenderView", true, 1);
	pMaterialSystem = GetInterface<ValveSDK::IMaterialSystem>("materialsystem.dll", "VMaterialSystem");
	pConVar = GetInterface<ValveSDK::ICvar>("engine.dll", "VEngineCvar", true, 1);
	pGameEventManager = GetInterface<ValveSDK::IGameEventManager>("engine.dll", "GAMEEVENTSMANAGER", true, 1);

#ifdef DEBUGMODE
	sprintf(szDebugString, "g_pGameEventManager: 0x%x", (DWORD)pGameEventManager);
	Base::Debug::LOG(szDebugString);
#endif

	pGlobalVars = *(ValveSDK::CGlobalVars**)(((*(PDWORD*)g_Valve.pClient)[0]) + GLOBALSOFFSET);

	pGlobalVars = (ValveSDK::CGlobalVars*)*(PDWORD)pGlobalVars;

#ifdef DEBUGMODE
	sprintf(szDebugString, "g_pGlobalVars: 0x%x", (DWORD)pGlobalVars);
	Base::Debug::LOG(szDebugString);
#endif

	m_bInitiated = TRUE;
}

BOOL CValve::isInitiated()
{
	return this->m_bInitiated;
}

bool CValve::WorldToScreen(const Vector& vOrigin, Vector& vScreen)
{
	auto ScreenTransform = [](const Vector& point, Vector& screen) -> bool {
		float w;
		const matrix3x4& worldToScreen = g_Valve.pEngine->WorldToScreenMatrix();
		screen.x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
		screen.y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
		w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];
		screen.z = 0.0f;

		bool behind = false;

		if (w < 0.001f)
		{
			behind = true;
			screen.x *= 100000;
			screen.y *= 100000;
		}
		else
		{
			behind = false;
			float invw = 1.0f / w;
			screen.x *= invw;
			screen.y *= invw;
		}
		return behind;
	};

	if (!ScreenTransform(vOrigin, vScreen))
	{
		int iScreenWidth, iScreenHeight;

		g_Valve.pEngine->GetScreenSize(iScreenWidth, iScreenHeight);
		float x = iScreenWidth / 2;
		float y = iScreenHeight / 2;
		x += 0.5 * vScreen.x * iScreenWidth + 0.5;
		y -= 0.5 * vScreen.y * iScreenHeight + 0.5;
		vScreen.x = x;
		vScreen.y = y;
		return true;
	}
	return false;
}