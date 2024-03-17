namespace ValveSDK
{
	class CGameMovement
	{
	public:
		void ProcessMovement(CBaseEntity *pPlayer, CMoveData* data)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity*, CMoveData*);
			getvfunc<OriginalFn>(this, 1)(this, pPlayer, data);
		}
	};
}