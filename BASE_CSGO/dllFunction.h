#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib" )

//HELPING INCLUDES
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <psapi.h>
#include <time.h>
#include <process.h>
#include <vector>
#include <map>
#include <ostream>
#include <Shlobj.h>
#include <math.h>
#include <intrin.h>
#include <cstdint>

//MENU/VECTORS
#include "xor.h"
#include "vector.h"
#include "CMath.h"
#include "Valve/checksum_crc.h"
#include "sdk.h"
#include "Utils.h"
#include "CValve.h"
#include "CDraw.h"
#include "CWinsock.h"
#include "mouse.h"
#include "menu.h"

//CSGO
#include "Surface.h"
#include "client.h"
#include "Esp.h"
#include "CNoSpread.h"
#include "CGameEventManager.h"
#include "Aimbot.h"
#include "CVARS.h"

typedef struct 
{
	int damage;
	int targetId;
	DWORD animTimer;
} healthInfo_t;

extern std::vector<healthInfo_t>g_healthInfo;

#pragma warning( disable : 4409 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4101 )

//#define DEBUGMODE

typedef void(__thiscall* PaintTraverseFn)(void*, unsigned int, bool, bool);
extern PaintTraverseFn oPaintTraverse;

typedef void(__thiscall* RunCommandFn)(void*, CBaseEntity*, ValveSDK::CUserCmd*, void*);
extern RunCommandFn oRunCommand;

extern CValve g_Valve;
extern CDraw g_Draw;
extern CMath g_Math;