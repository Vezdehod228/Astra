#pragma once

#define MAX_CVARS 136

enum
{
	NameESP, HealthESP, lagFix, hitscan, hitboxesEnabled, DrawCrosshair, thirdPerson, svCheats, NoRecoil, FakelagEnabled, NoSpread, RoundSay, KillSay, Fakelag, EnableAntiAim, Watermark, EnableAim, EnableESP, Bunnyhop, Autopistol, AimOnFire,
	Autoshoot, SilentAim, MinDamage, AimTeam, AimFOV, PSilent, FakelagAmount, Triggerbot, PTrigger, TriggerBurst,
	TriggerHead, TriggerAll, TriggerWall, NoVisRecoil, RCS, RCSFOV, RCSSmooth, RCSSpot, SCS, AimSpot,
	RCSAimTeam, TriggerSeed, AimHeight, Hitscans, Autowall, HitscanAutowall, Chams, BrightChams,
	ESPEnemyOnly, ChamsEnemyOnly, Glow, GlowEnemyOnly, AimKey, TriggerKey, SmoothAim, AimTime,
	TargetSelection, AntiDM, IgnoreLegsAndArms, Knifebot, BackstabOnly, AntiAimX, AntiAimY,
	Strafebot, XQZ, NoHands, Asus, AsusAmount, NoSky, HeadESP, BoxESP, Chams_T_Vis, RESERVED0,
	RESERVED1, Chams_T_InVis, RESERVED3, RESERVED4, Chams_CT_Vis, RESERVED6,
	RESERVED7, Chams_CT_InVis, RESERVED9, RESERVED10, Glow_T_Vis, RESERVED12,
	RESERVED35, Glow_T_InVis, RESERVED24, RESERVED23, Glow_CT_Vis, RESERVED13,
	RESERVED34, Glow_CT_InVis, RESERVED25, RESERVED22, Esp_T_Vis, RESERVED14,
	RESERVED33, RESERVED30, Esp_T_InVis, RESERVED26, RESERVED21, RESERVED18, Esp_CT_Vis, RESERVED15,
	RESERVED32, RESERVED31, Esp_CT_InVis, RESERVED27, RESERVED20, RESERVED19, ModelAlpha, VisChecks,
	SmartAim, SMACBot, HelmetESP, BombESP, BombDetonation, GrenadeESP, EntESP, WeaponESP, FlashedESP,
	DefuseESP, Radar, RadarX, RadarY, RadarHeight, RadarWorld, RadarVisuals, RadarRange, NoSmoke,
	NoFlash, AutoReload
};

enum
{
	SaveConfig,
	LoadConfig
};

class cCVARS
{
public:
	void SetDefaultConfig()
	{
		CvarList[AutoReload] = 0;
		CvarList[hitscan] = 0;
		CvarList[hitboxesEnabled] = 0;
		CvarList[thirdPerson] = 0;
		CvarList[FakelagEnabled] = 0;
		CvarList[svCheats] = 0;
		CvarList[lagFix] = 0;
		CvarList[DrawCrosshair] = 0;
		CvarList[Fakelag] = 0;
		CvarList[Watermark] = 1;
		CvarList[MinDamage] = 0;
		CvarList[TargetSelection] = 1;
		CvarList[EnableAntiAim] = 0;
		CvarList[NameESP] = 0;
		CvarList[HealthESP] = 0;
		CvarList[ESPEnemyOnly] = 1;
		CvarList[Bunnyhop] = 0;
		CvarList[Autopistol] = 0;
		CvarList[EnableESP] = 0;
		CvarList[BoxESP] = 0;
		CvarList[EnableESP] = 0;
		CvarList[RCSSpot] = 0;
		CvarList[RCSFOV] = 0;
		CvarList[RCSSmooth] = 0;
		CvarList[RadarRange] = 31;
		CvarList[BrightChams] = 1.f;
		CvarList[NoVisRecoil] = 0;
		CvarList[Triggerbot] = 0;
		CvarList[PTrigger] = 1;
		CvarList[TriggerSeed] = 0;
		CvarList[TriggerHead] = 1;
		CvarList[ChamsEnemyOnly] = 0;
		CvarList[AimSpot] = 12;
		//CvarList[MinDamage] = 30;
		CvarList[EnableAim] = 0;
		CvarList[NoSpread] = 1;
		CvarList[NoRecoil] = 0;
		//CvarList[Autowall] = 1;
		CvarList[PSilent] = 0;
		//CvarList[Chams] = 2;
		//CvarList[Glow] = 1;
		CvarList[AntiAimX] = 0;
		CvarList[AntiAimY] = 0;
		CvarList[Hitscans] = 0;
		CvarList[BoxESP] = 0;
		CvarList[AsusAmount] = 255;
		CvarList[NoSky] = 0;
		CvarList[Autowall] = 0;
		CvarList[NoHands] = 0;
		CvarList[Autoshoot] = 0;
		CvarList[Knifebot] = 0;
		CvarList[AimFOV] = 360.0f;
		CvarList[ModelAlpha] = 255.f;
		CvarList[GlowEnemyOnly] = 1;
		CvarList[AntiDM] = 0;
		CvarList[AimKey] = 0;//lbutton todo: virtual key binder inside inkeyevent (when menu is done)
		CvarList[TriggerKey] = 69;//e
		CvarList[RadarVisuals] = 0;
		CvarList[AimHeight] = 5.2f;

		CvarList[Chams_T_InVis] = 255;
		CvarList[Chams_CT_InVis + 2] = 255;

		CvarList[Chams_T_Vis] = 255;
		CvarList[Chams_T_Vis + 1] = 255;
		CvarList[Chams_CT_Vis + 1] = 255;

		CvarList[Esp_T_InVis] = 255;
		CvarList[Esp_T_InVis + 1] = 255;
		CvarList[Esp_CT_InVis + 1] = 255;

		CvarList[Esp_T_Vis] = 255;
		CvarList[Esp_CT_Vis + 2] = 255;
		CvarList[Esp_T_InVis + 3] = CvarList[Esp_T_Vis + 3] = CvarList[Esp_CT_InVis + 3] = CvarList[Esp_CT_Vis + 3] = 255;

		CvarList[Glow_T_InVis] = 255;
		CvarList[Glow_CT_InVis + 2] = 255;

		CvarList[Glow_T_Vis] = 255;
		CvarList[Glow_T_Vis + 1] = 255;
		CvarList[Glow_CT_Vis + 1] = 255;

	}

	void HandleConfig(const char *pszConfigName, int iType);

	void Init(HMODULE hinstDLL);

	float CvarList[MAX_CVARS];

private:

	char szIniFilePath[2048];
};

extern cCVARS Config;