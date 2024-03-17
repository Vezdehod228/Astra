#include "dllFunction.h"

Base::Utils::CVMTHookManager g_pClientVMT;
Base::Utils::CVMTHookManager g_pPanelVMT;
Base::Utils::CVMTHookManager g_pPredictionVMT;
Base::Utils::CVMTHookManager g_pModelRenderVMT;
Base::Utils::CVMTHookManager g_pInputVMT;

float flArrayCorrect[128][4];
CNoSpread g_NoSpread;
CAimbot g_Aimbot;
bool SendPacket = true;

ValveSDK::IMaterial* pNew = nullptr;
ValveSDK::IMaterial* pIgnoreNew = nullptr;
ValveSDK::IMaterial* pBrightNew = nullptr;
ValveSDK::IMaterial* pBrightIgnoreNew = nullptr;

KeyValues::KeyValues(const char* setName)
{
	typedef void(__thiscall* KeyValuesFn)(void*, const char*);

	static KeyValuesFn pKeyValues = nullptr;

	if (!pKeyValues)
	{
		pKeyValues = (KeyValuesFn)Base::Utils::PatternSearch("client.dll", (PBYTE)"\x33\xC0\x56\x8B\xF1\xC7\x06\x00\x00\x00\x00\x88\x46\x10", "xxxxxxx????xxx", NULL, NULL);

		if (!pKeyValues)
			return;
	}

	pKeyValues(this, setName);
}

bool KeyValues::LoadFromBuffer(const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID)
{
	typedef bool(__thiscall* LoadFromBufferFn)(void*, const char*, const char*, void*, const char*);

	static LoadFromBufferFn pLoadFromBuffer = nullptr;

	if (!pLoadFromBuffer)
	{
		pLoadFromBuffer = (LoadFromBufferFn)Base::Utils::PatternSearch("client.dll", (PBYTE)"\x64\xA1\x00\x00\x00\x00\x8B\x54\x24\x08\x6A\xFF", "xx????xxxxxx", NULL, NULL);

		if (!pLoadFromBuffer)
			return false;
	}

	return pLoadFromBuffer(this, resourceName, pBuffer, pFileSystem, pPathID);
}

void SetCurrentCmd(ValveSDK::CUserCmd* pDest, ValveSDK::CUserCmd* pSource)
{
	pDest->command_number = pSource->command_number;
	pDest->tick_count = pSource->tick_count;
	pDest->viewangles = pSource->viewangles;
	pDest->forwardmove = pSource->forwardmove;
	pDest->sidemove = pSource->sidemove;
	pDest->upmove = pSource->upmove;
	pDest->buttons = pSource->buttons;
	pDest->impulse = pSource->impulse;
	pDest->weaponselect = pSource->weaponselect;
	pDest->weaponsubtype = pSource->weaponsubtype;
	pDest->random_seed = pSource->random_seed;
	pDest->mousedx = pSource->mousedx;
	pDest->mousedy = pSource->mousedy;
	pDest->hasbeenpredicted = pSource->hasbeenpredicted;
}

void Prediction(ValveSDK::CUserCmd* pCmd, CBaseEntity* pLocalPlayer, float fCurTime)
{
	static HANDLE hClient = (HMODULE)GetModuleHandleA("client.dll");

	float curtime = g_Valve.pGlobalVars->curtime;
	float frametime = g_Valve.pGlobalVars->frametime;

	g_Valve.pGlobalVars->curtime = fCurTime;
	g_Valve.pGlobalVars->frametime = g_Valve.pGlobalVars->interval_per_tick;

	*(ValveSDK::CUserCmd**)((DWORD)pLocalPlayer + CURRENTCOMMANDOFFSET) = pCmd;
	*(CBaseEntity**)((DWORD)hClient + PREDICTIONPLAYEROFFSET) = pLocalPlayer;
	*(int*)((DWORD)hClient + PREIDCTIONSEEDOFFSET) = pCmd->random_seed;

	CMoveData Data;
	memset(&Data, 0, sizeof(Data));
	g_Valve.pPred->SetupMove(pLocalPlayer, pCmd, NULL, &Data);
	g_Valve.pGameMovement->ProcessMovement(pLocalPlayer, &Data);
	g_Valve.pPred->FinishMove(pLocalPlayer, pCmd, &Data);

	*(ValveSDK::CUserCmd**)((DWORD)pLocalPlayer + CURRENTCOMMANDOFFSET) = 0;
	*(CBaseEntity**)((DWORD)hClient + PREDICTIONPLAYEROFFSET) = 0;
	*(int*)((DWORD)hClient + PREIDCTIONSEEDOFFSET) = -1;

	g_Valve.pGlobalVars->curtime = curtime;
	g_Valve.pGlobalVars->frametime = frametime;
}

void MyMovePacket(int sequence_number)
{
	g_Valve.pConVar->FindVar("cl_pred_optimize")->m_nValue = 0;

	if (Config.CvarList[svCheats]) 
		g_Valve.pConVar->FindVar("sv_cheats")->m_nValue = 1;

	if (Config.CvarList[lagFix])
	{
		if (Config.CvarList[svCheats])
		{
			g_Valve.pConVar->FindVar("cl_allowdownload")->m_nValue = 0;
			g_Valve.pConVar->FindVar("cl_autowepswitch")->m_nValue = 0;
			g_Valve.pConVar->FindVar("cl_forcepreload")->m_nValue = 1;
			g_Valve.pConVar->FindVar("cl_interpolate")->m_nValue = 1;
			g_Valve.pConVar->FindVar("cl_interp")->m_nValue = 0;
			g_Valve.pConVar->FindVar("cl_cmdrate")->m_nValue = 30;
			g_Valve.pConVar->FindVar("rate")->m_nValue = 57500;
			g_Valve.pConVar->FindVar("cl_lagcompensation")->m_nValue = 1;
			g_Valve.pConVar->FindVar("cl_extrapolate")->m_nValue = 1;
			g_Valve.pConVar->FindVar("cl_predict")->m_nValue = 1;
		}
	}

	ValveSDK::CUserCmd* pUserCmd = g_Valve.pInput->GetUserCmd(sequence_number);

	CBaseEntity* pMe = g_Valve.pEntList->GetClientEntity(g_Valve.pEngine->GetLocalPlayer());

	if (bMenu)
		pUserCmd->buttons &= ~IN_ATTACK;

	if (Config.CvarList[Bunnyhop])
		if (pUserCmd->buttons & IN_JUMP && !(pMe->GetFlags() & FL_ONGROUND))
			pUserCmd->buttons &= ~IN_JUMP;

	bool bCanFire = true;
	float fCurTime = g_Valve.pGlobalVars->interval_per_tick * (pMe->GetTickBase() + 1);
	Vector qOldAngle = pUserCmd->viewangles;

	if (Config.CvarList[Strafebot] && !(pMe->GetFlags() & FL_ONGROUND) && pUserCmd->sidemove == 0 && pUserCmd->forwardmove == 0)
	{
		static float fOldAbsoluteViewYaw;
		float fAbsoluteView = qOldAngle.y;

		float fDeltaAbsView = fAbsoluteView - fOldAbsoluteViewYaw;

		fOldAbsoluteViewYaw = fAbsoluteView;

		if (fDeltaAbsView > 0.0f)
			pUserCmd->sidemove = -400;
		else if (fDeltaAbsView < 0.0f)
			pUserCmd->sidemove = 400;
	}

	Prediction(pUserCmd, pMe, fCurTime);

	ValveSDK::CBaseCombatWeapon* pWeapon = pMe->GetActiveBaseCombatWeapon();

	//keep a tab on how many commands we queue
	static int iChokedCommands = 0;
	int iMaxChokedCommands = 14;

	//kolonote:
	//this way we wont crash if we have no active weapon
	if (pWeapon)
	{
		float fNextPrimary = pWeapon->GetNextPrimaryAttack();

		if (!pWeapon->IsMiscWeapon())
		{
			//for triggerseed/fakeseed
			static bool bRevertBack;
			static int iOldSeq;
			static int iOldCmdNum;

			//the tick we can fire on...beware this will fail if fps are smaller than 60
			static bool bOldBullet;

			if ((fNextPrimary > fCurTime) || bOldBullet)
				bCanFire = false;

			if (!(fNextPrimary > fCurTime))
				bOldBullet = false;

			if (pWeapon->HasAmmo())
			{

				//for rage aimbot we can check bCanFire here otherwise for legit one we dont
				if (Config.CvarList[EnableAim])
				{
					Config.CvarList[AimOnFire] = 1;
					if (Config.CvarList[AimOnFire])
					{
						if (bCanFire)
							g_Aimbot.Main(pUserCmd, pMe, pWeapon);
					}
					else
					{
						g_Aimbot.Main(pUserCmd, pMe, pWeapon);
					}
				}
			}

			//autopistol - will slow down firerate if fps < 60
			if ((pUserCmd->buttons & IN_ATTACK))
			{
				if (bCanFire)
					bOldBullet = true;
				else
				{
					if (Config.CvarList[Autopistol])
						pUserCmd->buttons &= ~IN_ATTACK;
				}
			}



			//by now checking in_attack we check the tick we can fire on
			if ((pUserCmd->buttons & IN_ATTACK))
			{
				pUserCmd->random_seed = 141;

				if (Config.CvarList[NoSpread])
					g_NoSpread.GetSpreadFix(pWeapon, pUserCmd->random_seed, pUserCmd->viewangles);
			}

			if (Config.CvarList[NoRecoil])
				if (pUserCmd->buttons & IN_ATTACK)
					pUserCmd->viewangles -= pMe->GetPunchAngle() * 2.0f;

			static Vector qClientViewAngles;
			if (Config.CvarList[PSilent] && !bCanFire)
			{
				if (pUserCmd->buttons & IN_ATTACK && pWeapon->IsSilencerOn())
					SendPacket = false;

				g_Valve.pEngine->GetViewAngles(qClientViewAngles);

				qClientViewAngles.x = g_NoSpread.AngleNormalize(qClientViewAngles.x);
				qClientViewAngles.y = g_NoSpread.AngleNormalize(qClientViewAngles.y);

				pUserCmd->viewangles.y = qClientViewAngles.y;

				if (!Config.CvarList[AntiAimX])
				{
					//make sure we dont modify z angle as that will ban us on mm	
					pUserCmd->viewangles.x = qClientViewAngles.x;
				}
			}

			if ((Config.CvarList[AntiAimX] || Config.CvarList[AntiAimY]) && !(pUserCmd->buttons & IN_USE))
			{
				if (!(pUserCmd->buttons & IN_ATTACK))
				{
					if (Config.CvarList[AntiAimX] == 1)
						pUserCmd->viewangles.x = 70.0f;

					if (Config.CvarList[AntiAimY] == 1)
						pUserCmd->viewangles.y -= 180.0f;

					if (Config.CvarList[AntiAimY] == 2)
					{
						static bool miniJitt = false;

						if (miniJitt)
							pUserCmd->viewangles.y -= 190.0f;
						else
							pUserCmd->viewangles.y -= 170.0f;

						miniJitt = !miniJitt;
					}

					if (Config.CvarList[AntiAimY] == 3)
					{
						static bool miniJitt = false;

						if (miniJitt)
							pUserCmd->viewangles.y -= 160.0f;
						else
							pUserCmd->viewangles.y -= 200.0f;

						miniJitt = !miniJitt;
					}
				}
			}

			if (Config.CvarList[PSilent] && bCanFire && (pUserCmd->buttons & IN_ATTACK))
			{
				SendPacket = false;
				iChokedCommands++;
			}

			g_Aimbot.FixMovement(pUserCmd, qOldAngle, ((Config.CvarList[AntiAimX] > 1) ? -1 : 1));
		}
	}
}

DWORD dwOriginCreateMove;
void __declspec(naked) __fastcall hkdCreateMove(void* edx, void* ecx, int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebp
		mov ebp, esp

		mov SendPacket, bl

		movzx	eax, active
		push	eax
		mov		eax, input_sample_frametime
		push	eax
		mov		eax, sequence_number
		push	eax
		call dwOriginCreateMove
	}

	MyMovePacket(sequence_number);

	__asm
	{
		mov bl, SendPacket

		mov ebp, esp
		pop ebp

		retn 0xC
	}
}

void StorePunchAngle(CBaseEntity* player)
{
	int nTickBase = player->GetTickBase();

	int iStorage = 9 * nTickBase % 128;

	flArrayCorrect[iStorage][0] = nTickBase;

	//at this point punchangle isnt compressed and we can retrieve it then just predict it when we call processmovement inside createmove
	//Vector* qPunchangle = reinterpret_cast< Vector* >(reinterpret_cast< int >(player)+g_NetworkedVariableManager->GetOffset("DT_BasePlayer", "m_aimPunchAngle"));

	Vector qPunchangle = player->GetPunchAngle();
	flArrayCorrect[iStorage][1] = qPunchangle.x;
	flArrayCorrect[iStorage][2] = qPunchangle.y;
	flArrayCorrect[iStorage][3] = qPunchangle.z;
}

void __fastcall hkdRunCommand(void* ecx, void* edx, CBaseEntity* pEntity, ValveSDK::CUserCmd* pUserCmd, void* moveHelper)
{
	oRunCommand(ecx, pEntity, pUserCmd, moveHelper);

	StorePunchAngle(pEntity);
}

void __stdcall hkdFrameStageNotify(ClientFrameStage_t curStage)
{
	static DWORD dwOriginFrameStageNotify = g_pClientVMT.dwGetMethodAddress(32);

	static int iAimPunchOffset = g_NetworkedVariableManager.GetOffset("DT_CSPlayer", "m_vecPunchAngle");
	static int iFallVelocityOffset = g_NetworkedVariableManager.GetOffset("DT_CSPlayer", "m_flFallVelocity");
	static int iBaseVelocityOffset = g_NetworkedVariableManager.GetOffset("DT_CSPlayer", "m_vecBaseVelocity");

	static Vector qOldPunch;

	if (Config.CvarList[NoVisRecoil] && g_Valve.pEngine->IsInGame() && curStage == FRAME_RENDER_START)
	{
		CBaseEntity* pMe = g_Valve.pEntList->GetClientEntity(g_Valve.pEngine->GetLocalPlayer());
		Vector* qPunch = (Vector*)((DWORD)pMe + iAimPunchOffset);

		qOldPunch = *qPunch;
		qPunch->Init(0.0f, 0.0f, 0.0f);

		_asm
		{
			PUSH curStage
			CALL dwOriginFrameStageNotify
		}

		*qPunch = qOldPunch;
	}
	else
	{
		_asm
		{
			PUSH curStage
			CALL dwOriginFrameStageNotify
		}
	}

	if (curStage == FRAME_NET_UPDATE_END && g_Valve.pEngine->GetLocalPlayer() > 0)
	{
		CBaseEntity* pMe = g_Valve.pEntList->GetClientEntity(g_Valve.pEngine->GetLocalPlayer());

		if (pMe)
		{
			int nTickBase = pMe->GetTickBase();
			static bool bKinda[4];
			static bool bRecoil, bCheckit, bChecknow;
			static float flIsee;
			float* flTouche = flArrayCorrect[9 * nTickBase % 128];

			bKinda[0] = flTouche[0] == nTickBase;

			Vector qPunch = pMe->GetPunchAngle();

			static Vector vPunch;

			vPunch.x = qPunch.x;
			vPunch.y = qPunch.y;
			vPunch.z = qPunch.z;

			static Vector vStoredPunch;

			vStoredPunch.x = flTouche[1];
			vStoredPunch.y = flTouche[2];
			vStoredPunch.z = flTouche[3];

			float flTesting = Vector(vStoredPunch - vPunch).Length();

			bKinda[1] = 0.33000001f > flTesting;

			if (bKinda[0])
				bRecoil = bKinda[1];
			else
			{
				int iNewTickbase = nTickBase - 1;
				int v5 = 1;
				int iUpdatetick = nTickBase - 1;

				bRecoil = bChecknow;
			}
			if (bRecoil)
			{
				Vector* pPunchangle = (Vector*)((DWORD)pMe + iAimPunchOffset);

				pPunchangle->x = flTouche[1];
				pPunchangle->y = flTouche[2];
				pPunchangle->z = flTouche[3];
			}
		}
	}
}

#define T_INVIS Config.CvarList[Chams_T_InVis],Config.CvarList[Chams_T_InVis + 1], Config.CvarList[Chams_T_InVis + 2]
#define CT_INVIS Config.CvarList[Chams_CT_InVis],Config.CvarList[Chams_CT_InVis + 1], Config.CvarList[Chams_CT_InVis + 2]

#define T_VIS 0,255,0
#define CT_VIS 0,255,0

int __stdcall hkdDrawModelEx(ValveSDK::ModelRenderInfo_t& pInfo)
{
	//idgaf cba to use inline asm here when i can just unhook and clean call with 1 line
	g_pModelRenderVMT.UnHook();

	const char *pszModelName = g_Valve.pModel->GetModelName(pInfo.pModel);

	bool bPlayerModel = strstr(pszModelName, "models/player");

	static ValveSDK::IMaterial *pPlayerMaterial[32];
	static CBaseEntity *pEntity = NULL;
	static bool bTeamCheck;
	auto* hdr = g_Valve.pModel->GetStudiomodel(pInfo.pModel);
	if (hdr)
	{
		pEntity = g_Valve.pEntList->GetClientEntity(pInfo.entity_index);
		if (pEntity)
		{
			CBaseEntity* pLocal = g_Valve.pEntList->GetClientEntity(g_Valve.pEngine->GetLocalPlayer());

			if (Config.CvarList[XQZ] && bPlayerModel)
			{

				bTeamCheck = ((pEntity->GetHealth() >= 1) && (!Config.CvarList[ChamsEnemyOnly] || (Config.CvarList[ChamsEnemyOnly] && pEntity->GetTeamNum() != pLocal->GetTeamNum())));

				if (bTeamCheck)
				{
					g_Valve.pModel->GetModelMaterials(pInfo.pModel, 1, pPlayerMaterial);

					if (pPlayerMaterial[0])
						pPlayerMaterial[0]->SetMaterialVarFlag(ValveSDK::MATERIAL_VAR_IGNOREZ, true);
				}
			}

			if (Config.CvarList[Chams] && bPlayerModel)
			{
				if (Config.CvarList[Chams])
					FullCham(pIgnoreNew, pInfo, 69, 26, 190, 69, 26, 190); //первые 3 это для теров, последние 3 это для ктшников

				g_Valve.pModelRender->DrawModelExecute(pInfo);
				g_Valve.pModelRender->ForcedMaterialOverride(NULL);
			}
			else
			{
			    g_Valve.pModelRender->DrawModelExecute(pInfo);
			    g_Valve.pModelRender->ForcedMaterialOverride(NULL);
			}

			if (Config.CvarList[XQZ] && bPlayerModel && pPlayerMaterial[0])
				pPlayerMaterial[0]->SetMaterialVarFlag(ValveSDK::MATERIAL_VAR_IGNOREZ, false);
		}
	}

	auto ret = g_Valve.pModelRender->DrawModelExecute(pInfo);

	g_pModelRenderVMT.ReHook();

	return ret;
}

int __stdcall hkdIN_KeyEvent(int eventcode, ValveSDK::ButtonCode_t keynum, const char* pszCurrentBinding)
{
	int iRet;

	static DWORD dwOriginalAddress = g_pClientVMT.dwGetMethodAddress(20);

	_asm
	{
		PUSH pszCurrentBinding
		PUSH keynum
		PUSH eventcode
		CALL dwOriginalAddress
		MOV iRet, EAX
	}

	if (keynum == ValveSDK::MOUSE_WHEEL_UP)
	{
		ListItemArray[g_Menu.GetSmallTabIndex(1)].MoveUp(1);
		return 0;
	}

	if (keynum == ValveSDK::MOUSE_WHEEL_DOWN)
	{
		ListItemArray[g_Menu.GetSmallTabIndex(1)].MoveDown(1);
		return 0;
	}

	return iRet;
}

ValveSDK::CUserCmd* __stdcall hkdGetUserCmd(int sequence_number)
{
	return g_Valve.pInput->GetUserCmd(sequence_number);
}

void __stdcall hkdFinishMove(CBaseEntity* player, ValveSDK::CUserCmd* ucmd, PVOID move)
{
	static DWORD dwOriginalAddress = g_pPredictionVMT.dwGetMethodAddress(21);

	_asm
	{
		PUSH move
		PUSH ucmd
		PUSH player
		CALL dwOriginalAddress
	}

	Vector vNewAbsVelocity = player->GetVelocity() + player->GetBaseVelocity();

	//fix gay ass strafe spread (damn you valve)
	*(Vector*)((DWORD)player + 0x12C) = vNewAbsVelocity;
}