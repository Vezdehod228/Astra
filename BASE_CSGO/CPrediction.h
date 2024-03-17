namespace ValveSDK
{
	class CPrediction
	{
	public:
		void SetupMove(CBaseEntity *player, ValveSDK::CUserCmd *ucmd, PVOID movehelper, PVOID moveData)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity*, ValveSDK::CUserCmd*, PVOID, PVOID);
			getvfunc<OriginalFn>(this, 20)(this, player, ucmd, movehelper, moveData);
		}

		void FinishMove(CBaseEntity *player, ValveSDK::CUserCmd *ucmd, PVOID moveData)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, CBaseEntity*, ValveSDK::CUserCmd*, PVOID);
			getvfunc<OriginalFn>(this, 21)(this, player, ucmd, moveData);
		}
	};
}