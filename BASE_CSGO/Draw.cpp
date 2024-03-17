#include "dllFunction.h"

CDraw g_Draw;
ESP g_ESP;

bool bMenu = false;

std::vector<ValveSDK::IMaterial*> vecMapMaterials;
std::vector<ValveSDK::IMaterial*> vecOldMapMaterials;
std::vector<ValveSDK::IMaterial*> vecSkyMaterials;
ValveSDK::IMaterial *pHands = NULL;

bool bGetMaterials = true;

void __fastcall hkdPaintTraverse(void* ecx, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	oPaintTraverse(ecx, vguiPanel, forceRepaint, allowForce);

	static bool bDidOnce = false;

	if (bDidOnce == false)
	{
		g_Draw.InitFonts();

#ifdef DEBUGMODE
		Base::Debug::LOG("Font initalized");
#endif
		bDidOnce = true;
	}
	
	const char* pszPanelName = g_Valve.pPanel->GetName(vguiPanel);

	static std::string sPanel = "FocusOverlayPanel";

	if(!strstr(pszPanelName,sPanel.data()))
		return;

	//pointer address to convars never change so we only get it once
	static ValveSDK::ConVar *pMouseEnable = g_Valve.pConVar->FindVar("cl_mouseenable");

	if(GetAsyncKeyState(VK_INSERT) & 1)
		bMenu = !bMenu;

	if (g_Valve.pEngine->IsInGame() && Config.CvarList[EnableESP])
		g_ESP.draw();

	pMouseEnable->m_nValue = (bMenu ? 0 : 1);
	//make sure you got m_rawinput 1 ingame...
	if(bMenu)
	{
		g_Mouse.Update(pMouseEnable);

		int copy_x, copy_y;
		int copy_w, copy_h;

		g_Menu.GetMenuPos(copy_x,copy_y);
		g_Menu.GetMenuSize(copy_w,copy_h);

		g_Menu.DrawMenu();

		g_Mouse.Drag(g_Mouse.bDragged[0],
			!g_Menu.IsHandlingItem(),
			g_Mouse.LeftClick(copy_x,copy_y,copy_w,copy_h),copy_x,copy_y,g_Mouse.iDiffX[0],g_Mouse.iDiffY[0]);

		g_Menu.SetMenuPos(copy_x,copy_y);

		if(g_Menu.IsHandlingItem())
			g_Menu.RemoveMenuFlag(FL_DISABLEDRAG);//enable menu dragging

		//this comes at end so we overdraw menu
		g_Mouse.DrawMouse();
	} 
	else if(g_Mouse.bReturn)//do this once as buymenu enables/disables mouse same way we do
	{
		pMouseEnable->m_nValue = 1;
		g_Mouse.bReturn = false;
	}

	
	static bool bNosky;
	static bool bAsus;

	if(!g_Valve.pEngine->IsInGame() || !g_Valve.pEngine->IsConnected())
	{
		bGetMaterials = true;
		bNosky = false;
		bAsus = false;

		return;
	}

	if(bGetMaterials) 
	{
		vecSkyMaterials.clear();
		vecMapMaterials.clear();

		for(ValveSDK::MaterialHandle_t h = 0; h < g_Valve.pMaterialSystem->GetNumMaterials(); h++)
		{ 
			ValveSDK::IMaterial *pFirstMaterial = g_Valve.pMaterialSystem->GetMaterial(h);

			if(!pFirstMaterial)
				continue;

			if(strstr(pFirstMaterial->GetTextureGroupName(),XorStr<0xA4,7,0xFAF2E307>("\xF7\xCE\xDF\xE5\xC7\xD1"+0xFAF2E307).s))
				vecSkyMaterials.push_back(pFirstMaterial); 
			else if(strstr(pFirstMaterial->GetTextureGroupName(),XorStr<0x8A,6,0x3FB546ED>("\xDD\xE4\xFE\xE1\xEA"+0x3FB546ED).s))
				vecMapMaterials.push_back(pFirstMaterial);
			else if(strstr(pFirstMaterial->GetName(),XorStr<0x8D,8,0xFE3E79EF>("\xFB\xD1\xE7\xF1\xFF\xF6\xE0"+0xFE3E79EF).s))
				pHands = pFirstMaterial;
		}

		bGetMaterials = false;
	}

	static float fOldStrength;

	if(Config.CvarList[Asus])
	{
		if((!bAsus || Config.CvarList[AsusAmount] != fOldStrength) && !vecMapMaterials.empty())
		{
			for(int iTex = 0; iTex < vecMapMaterials.size(); iTex++)
			{
				if(!vecMapMaterials[iTex])
					continue;

				vecMapMaterials[iTex]->AlphaModulate((Config.CvarList[AsusAmount] / 255.f));

				if(iTex == (vecMapMaterials.size()-1))
				{
					bAsus = true;
					fOldStrength = Config.CvarList[AsusAmount];
				}
			}
		}
	}
	else
	{
		if(bAsus && !vecMapMaterials.empty())
		{
			for(int iTex = 0; iTex < vecMapMaterials.size(); iTex++)
			{
				if(!vecMapMaterials[iTex])
					continue;

				vecMapMaterials[iTex]->AlphaModulate(1.0f);

				if(iTex == (vecMapMaterials.size()-1))
					bAsus = false;
			}
		}
	}

	if(Config.CvarList[NoSky])
	{
		if(!bNosky && !vecSkyMaterials.empty())
		{
			for(int iTex = 0; iTex < vecSkyMaterials.size(); iTex++)
			{
				if(!vecSkyMaterials[iTex])
					continue;

				vecSkyMaterials[iTex]->ColorModulate(0.0f,0.0f,0.0f);

				if(iTex == (vecSkyMaterials.size()-1))
					bNosky = true;
			}
		}
	}
	else
	{
		if(bNosky && !vecSkyMaterials.empty())
		{
			for(int iTex = 0; iTex < vecSkyMaterials.size(); iTex++)
			{
				if(!vecSkyMaterials[iTex])
					continue;

				vecSkyMaterials[iTex]->ColorModulate(1.0f,1.0f,1.0f);

				if(iTex == (vecSkyMaterials.size()-1))
					bNosky = false;
			}
		}
	}

	//CSS only - we just use findmaterial
	static bool bHands;

	if(Config.CvarList[NoHands] && !bHands)
	{
		if(!pHands->GetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW))
			pHands->SetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW,true);

		bHands = true;
	}
	else if(!Config.CvarList[NoHands] && bHands)
	{
		if(pHands->GetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW))
			pHands->SetMaterialVarFlag(ValveSDK::MATERIAL_VAR_NO_DRAW,false);

		bHands = false;
	}
}


void ChamModel( float r, float g, float b, ValveSDK::IMaterial *pMat)
{
	float fColor[4] = { r / 255, g / 255, b / 255, 255 };

	static float fGetColor[4] = { r / 255, g / 255, b / 255, 255 };

	g_Valve.pRenderView->GetColorModulation(fGetColor);
	g_Valve.pRenderView->SetColorModulation(fColor);

	g_Valve.pModelRender->ForcedMaterialOverride(pMat);
}

void FullCham( ValveSDK::IMaterial *pMat, const ValveSDK::ModelRenderInfo_t &pInfo, int r, int g, int b, int r2, int g2, int b2)
{
	CBaseEntity *pBaseEnt = g_Valve.pEntList->GetClientEntity(pInfo.entity_index);
	
	if (!pBaseEnt)
	{
		g_Valve.pModelRender->ForcedMaterialOverride(pMat);
		return;
	}

	if(pBaseEnt->GetHealth() >= 1)//regular lifestate checking didnt work -.-''''
	{
		int iTeamNum = pBaseEnt->GetTeamNum();

		CBaseEntity *pLocal = g_Valve.pEntList->GetClientEntity(g_Valve.pEngine->GetLocalPlayer());

		int iMyTeamNum = pLocal->GetTeamNum();

		if(Config.CvarList[ChamsEnemyOnly] && iTeamNum == iMyTeamNum)
			return;

		if( iTeamNum == 2 ) //TT
			ChamModel(r, g, b, pMat);
		else if( iTeamNum == 3 ) //CT
			ChamModel(r2, g2, b2, pMat);
		else
			g_Valve.pModelRender->ForcedMaterialOverride(pMat);
	}
}

void GenerateRandomString(char *s, const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; i++)
	{
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

	s[len] = 0;
}

#define MAT( _TYPE_ ) "\"" + _TYPE_ + "\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$ignorez\" \"0\"\n\t\"$nofog\" \"1\"\n\t\"$model\" \"1\"\n\t\"$halflambert\" \"1\"\n}"
#define MAT_IGNOREZ( _TYPE_ ) "\"" + _TYPE_ + "\"\n{\n\t\"$basetexture\" \"vgui/white_additive\"\n\t\"$ignorez\" \"1\"\n\t\"$nofog\" \"1\"\n\t\"$model\" \"1\"\n\t\"$halflambert\" \"1\"\n}"

ValveSDK::IMaterial *CreateMaterial(BOOL bFullbright, BOOL bIgnorez)
{
	static int z = 1;
	char szBuff[2048];

	std::string type = (bFullbright ? "UnlitGeneric" : "VertexLitGeneric");
	std::string tmp( (bIgnorez ? MAT_IGNOREZ(type) : MAT(type)) );
	strcpy( szBuff, tmp.c_str() );

	KeyValues *pKeyValues = new KeyValues(type.c_str());

	if (!pKeyValues) return 0;

	//generate some random file name for the .vmt (residing only in memory)
	char szMaterialName[128];
	sprintf(szMaterialName, "cu5t0m_m4ter1al_%i.vmt", z);
	z++;

	if (!pKeyValues->LoadFromBuffer(szMaterialName, szBuff))
	{
#ifdef DEBUGMODE
		Base::Debug::LOG("LoadFromBuffer failed!");
#endif
		return 0;
	}

	ValveSDK::IMaterial *pNew = g_Valve.pMaterialSystem->CreateMaterial(szMaterialName,(PVOID)pKeyValues);

	if(pNew)
		pNew->IncrementReferenceCount();

#ifdef DEBUGMODE
	char szNewMaterial[1024];
	sprintf(szNewMaterial,"Created new Valve Material Type[%s] Fullbright[%i] Ignorez[%i]",szMaterialName,bFullbright,bIgnorez);
	Base::Debug::LOG(szNewMaterial);
#endif

	return pNew;
}