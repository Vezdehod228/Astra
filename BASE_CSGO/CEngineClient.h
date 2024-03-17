namespace ValveSDK
{
	class CEngineClient
	{
	public:
		typedef struct
		{
			char			name[32];
			int				userID;
			char			guid[33];
			unsigned int	friendsID;
			char			friendsName[32];
			bool			fakeplayer;
			bool			ishltv;
			unsigned long	customFiles[4];
			unsigned char	filesDownloaded;
		} player_info_t;

		void GetScreenSize(int& width, int& height)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, int&, int&);
			return getvfunc<OriginalFn>(this, 5)(this, width, height);
		}

		bool GetPlayerInfo(int iIndex, player_info_t *pInfo)
		{
			typedef bool(__thiscall* OriginalFn)(PVOID, int, player_info_t*);
			return getvfunc<OriginalFn>(this, 8)(this, iIndex, pInfo);
		}

		int GetLocalPlayer(VOID)
		{
			typedef int(__thiscall* OriginalFn)(PVOID);
			return getvfunc<OriginalFn>(this, 12)(this);
		}

		const matrix3x4& WorldToScreenMatrix(void)
		{
			typedef const matrix3x4& (__thiscall* OriginalFn)(PVOID);
			return getvfunc<OriginalFn>(this, 38)(this);
		}

		void ExecuteClientCmd(char const* szCommand)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, char const* szCommand);
			getvfunc<OriginalFn>(this, 7)(this, szCommand);
		}

		void ExecuteClientCmdFunction(char const* szCommand, int hitboxBone)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, char const* szCommand, int hitboxBone);
			getvfunc<OriginalFn>(this, 7)(this, szCommand, hitboxBone);
		}

		void GetViewAngles(Vector& vAngles)
		{
			typedef void(__thiscall* OriginalFn)(PVOID, Vector&);
			return getvfunc< OriginalFn >(this, 19)(this, vAngles);
		}

		void SetViewAngles(Vector& vAngles)
		{
			typedef void(__thiscall* oSetViewAngles)(PVOID, Vector&);
			return getvfunc< oSetViewAngles >(this, 20)(this, vAngles);
		}
		
		bool IsConnected()
		{
			typedef bool(__thiscall* oGetScreenSize)(PVOID);
			return getvfunc< oGetScreenSize >(this, 27)(this);
		}

		bool IsInGame()
		{
			typedef bool(__thiscall* oLocal)(PVOID);
			return getvfunc< oLocal >(this, 26)(this);
		}

		int GetMaxClients()
		{
			typedef bool(__thiscall* oGetMaxClients)(PVOID);
			return getvfunc< oGetMaxClients >(this, 21)(this);
		}
	};
}