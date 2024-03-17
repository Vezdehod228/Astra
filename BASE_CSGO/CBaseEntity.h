/*
CBaseEntity rebuild
22.08.2014
*/

class CBaseEntity{
public:
	virtual void init(){};
	void* GetNetworkable()
	{
		typedef void*(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 4)(this);
	}
	
	void* GetRenderable()
	{
		typedef void*(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 5)(this);
	}

	void* GetCollideable()
	{
		typedef void*(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 3)(this);
	}

	const Vector& GetAbsOrigin()
	{
		typedef const Vector& ( __thiscall *GetAbsOrg_t )(PVOID);
		return getvfunc<GetAbsOrg_t>(this, 9)(this);
	}

	int GetFOV()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset("DT_BasePlayer", "m_iFOV");
		return *(int*)((DWORD)this + iOffset);
	}

	Vector GetEyeAngles()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0xAA,12,0x6F938DD5>("\xEE\xFF\xF3\xEE\xFD\xFF\xDC\xD0\xCB\xD6\xC6"+0x6F938DD5).s, /*m_angEyeAngles[0]*/XorStr<0x7E,18,0x0D7A3609>("\x13\x20\xE1\xEF\xE5\xC6\xFD\xE0\xC7\xE9\xEF\xE5\xEF\xF8\xD7\xBD\xD3"+0x0D7A3609).s);
		return *(Vector*)((DWORD)this + iOffset);
	}

	color32 GetModelColor()
	{
		unsigned long offset = g_NetworkedVariableManager.GetOffset("DT_BaseEntity", "m_clrRender");
		color32 color;
		if (offset)
		{
			color.r = offset + 0x4;
			color.g = offset + 0x8;
			color.b = offset + 0xC;
			color.a = offset + 0x10;
		}
		return color;
	}

	bool IsSpawnProtectedPlayer()
	{
		return (this->GetModelColor().a < 255);
	}

	ValveSDK::CBaseCombatWeapon* GetActiveBaseCombatWeapon();

	int GetIndex()
	{
		auto net = GetNetworkable();

		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(net, 9)(net);
	}

	bool IsDormant()
	{
		auto net = GetNetworkable();

		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(net, 8)(net);
	}

	bool IsPlayer()
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 131)(this); //118
	}

	model_t* GetModel()
	{
		auto ren = GetRenderable();

		typedef model_t*(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(ren, 8)(ren);
	}

	int GetHealth()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2,14,0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC"+0xCFD8B52B).s, /*m_iHealth*/XorStr<0x7A,10,0x7B6EFF44>("\x17\x24\x15\x35\x1B\x1E\xEC\xF5\xEA"+0x7B6EFF44).s);
		return *(int*)((DWORD)this + iOffset);
	}

	int GetEntity()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2, 14, 0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC" + 0xCFD8B52B).s, "dwLocalPlayer");
		return *(int*)((DWORD)this + iOffset);
	}

	char GetLifeState()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2,14,0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC"+0xCFD8B52B).s, /*m_LifeState*/XorStr<0x8E,12,0x5618CCB6>("\xE3\xD0\xDC\xF8\xF4\xF6\xC7\xE1\xF7\xE3\xFD"+0x5618CCB6).s);
		return *(char*)((DWORD)this + iOffset);
	}

	int GetTeamNum()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2,14,0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC"+0xCFD8B52B).s, /*m_iTeamNum*/XorStr<0xBC,11,0x9EF53877>("\xD1\xE2\xD7\xEB\xA5\xA0\xAF\x8D\xB1\xA8"+0x9EF53877).s);
		return *(int*)((DWORD)this + iOffset);
	}

	int GetFlags()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2,14,0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC"+0xCFD8B52B).s, /*m_fFlags*/XorStr<0xC3,9,0x8951F943>("\xAE\x9B\xA3\x80\xAB\xA9\xAE\xB9"+0x8951F943).s);
		return *(int*)((DWORD)this + iOffset);
	}

	float GetFallVelocity()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0xAA,12,0x6F938DD5>("\xEE\xFF\xF3\xEE\xFD\xFF\xDC\xD0\xCB\xD6\xC6"+0x6F938DD5).s, /*m_flFallVelocity*/XorStr<0xC4,17,0x8454FEF5>("\xA9\x9A\xA0\xAB\x8E\xA8\xA6\xA7\x9A\xA8\xA2\xA0\xB3\xB8\xA6\xAA"+0x8454FEF5).s);
		return *(float*)((DWORD)this + iOffset);
	}

	Vector GetBaseVelocity()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0x34,12,0x959D9DAB>("\x70\x61\x69\x74\x6B\x69\x56\x5A\x45\x58\x4C"+0x959D9DAB).s,/*m_vecBaseVelocity*/XorStr<0x44,18,0xF97A7797>("\x29\x1A\x30\x22\x2B\x0B\x2B\x38\x29\x1B\x2B\x23\x3F\x32\x3B\x27\x2D"+0xF97A7797).s);
		return *(Vector*)((DWORD)this + iOffset);
	}

	Vector GetNetworkedOrigin()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2,14,0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC"+0xCFD8B52B).s, /*m_vecOrigin*/XorStr<0x1D,12,0x2333782C>("\x70\x41\x69\x45\x42\x6D\x51\x4D\x42\x4F\x49"+0x2333782C).s);
		return *(Vector*)((DWORD)this + iOffset);
	}

	Vector GetVecViewOffset()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BasePlayer*/XorStr<0xC2,14,0xCFD8B52B>("\x86\x97\x9B\x87\xA7\xB4\xAD\x99\xA6\xAA\xB5\xA8\xBC"+0xCFD8B52B).s, /*m_vecViewOffset[0]*/XorStr<0xF0,19,0xB93B59BD>("\x9D\xAE\x84\x96\x97\xA3\x9F\x92\x8F\xB6\x9C\x9D\x8F\x98\x8A\xA4\x30\x5C"+0xB93B59BD).s);
		return *(Vector*)((DWORD)this + iOffset);
	}

	Vector GetEyePosition()
	{
		return GetNetworkedOrigin() + GetVecViewOffset();
	}

	Vector GetPunchAngle()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0x34,12,0x959D9DAB>("\x70\x61\x69\x74\x6B\x69\x56\x5A\x45\x58\x4C"+0x959D9DAB).s, /*m_vecPunchAngle*/XorStr<0xFC,16,0xFF541491>("\x91\xA2\x88\x9A\x63\x51\x77\x6D\x67\x6D\x47\x69\x6F\x65\x6F"+0xFF541491).s);
		return *(Vector*)((DWORD)this + iOffset);
	}

	Vector GetVelocity()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0xAA,12,0x6F938DD5>("\xEE\xFF\xF3\xEE\xFD\xFF\xDC\xD0\xCB\xD6\xC6"+0x6F938DD5).s,/*m_vecVelocity[0]*/XorStr<0x60,17,0x44C61C52>("\x0D\x3E\x14\x06\x07\x33\x03\x0B\x07\x0A\x03\x1F\x15\x36\x5E\x32"+0x44C61C52).s);
		return *(Vector*)((DWORD)this + iOffset);
	}

	int GetTickBase()
	{
		static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_CSPlayer*/XorStr<0x34,12,0x959D9DAB>("\x70\x61\x69\x74\x6B\x69\x56\x5A\x45\x58\x4C"+0x959D9DAB).s, /*m_nTickBase*/XorStr<0x5E,12,0xDB7AF0A6>("\x33\x00\x0E\x35\x0B\x00\x0F\x27\x07\x14\x0D"+0xDB7AF0A6).s);
		return *(int*)((DWORD)this + iOffset);
	}

	bool isValidPlayer()
	{
		//kolonote:
		//there are no dormant player ents in csgo...there are some other ents tho like grenades
		return (GetLifeState() == LIFE_ALIVE && !IsDormant());
	}

	UCHAR GetMoveType()
	{
		return *(UCHAR*)((DWORD)this + 0x134);
	}

	bool SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		auto ren = GetRenderable();

		typedef bool(__thiscall* OriginalFn)(void*, matrix3x4_t*, int, int, int);
		return getvfunc<OriginalFn>(ren, 15)(ren, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	//later usage
	bool IsUsingAntiAim()
	{
		Vector qAngles = GetEyeAngles();

		return (qAngles.x >= 68.0f && qAngles.x <= 90.0f);
	}
};