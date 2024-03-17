enum
{
	WEAPON_NONE = 0,
	WEAPON_P228,
	WEAPON_GLOCK,
	WEAPON_SCOUT,
	WEAPON_HEGRENADE,
	WEAPON_XM1014,
	WEAPON_C4,
	WEAPON_MAC10,
	WEAPON_AUG,
	WEAPON_SMOKEGRENADE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_UMP45,
	WEAPON_SG550,
	WEAPON_GALIL,
	WEAPON_FAMAS,
	WEAPON_USP,
	WEAPON_AWP,
	WEAPON_MP5NAVY,
	WEAPON_M249,
	WEAPON_M3,
	WEAPON_M4A1,
	WEAPON_TMP,
	WEAPON_G3SG1,
	WEAPON_FLASHBANG,
	WEAPON_DEAGLE,
	WEAPON_SG552,
	WEAPON_AK47,
	WEAPON_KNIFE,
	WEAPON_P90,
	WEAPON_SHIELD,
	WEAPON_KEVLAR,
	WEAPON_ASSAULTSUIT,
	WEAPON_NIGHTVISION,

	WEAPON_MAX,
};

namespace ValveSDK
{
	class CBaseCombatWeapon
	{
	public:
		float GetNextPrimaryAttack()
		{
			static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BaseCombatWeapon*/XorStr<0x0C,20,0xCCB89A4B>("\x48\x59\x51\x4D\x71\x62\x77\x50\x7B\x78\x74\x76\x6C\x4E\x7F\x7A\x6C\x72\x70"+0xCCB89A4B).s, /*m_flNextPrimaryAttack*/XorStr<0x0D,22,0x423C055C>("\x60\x51\x69\x7C\x5F\x77\x6B\x60\x45\x64\x7E\x75\x78\x68\x62\x5D\x69\x6A\x7E\x43\x4A"+0x423C055C).s);
			return *(float*)((DWORD)this + iOffset);
		}

		bool HasAmmo()
		{
			static int iOffset = g_NetworkedVariableManager.GetOffset(/*DT_BaseCombatWeapon*/XorStr<0x5E,20,0xBED67F8D>("\x1A\x0B\x3F\x23\x03\x10\x01\x26\x09\x0A\x0A\x08\x1E\x3C\x09\x0C\x1E\x00\x1E"+0xBED67F8D).s,/*m_iClip1*/XorStr<0x7B,9,0xEBD79649>("\x16\x23\x14\x3D\x13\xE9\xF1\xB3"+0xEBD79649).s);
			return ((*(PINT)((DWORD)this + iOffset)) > 0);
		}

		bool IsSilencerOn()
		{
			static int iOffsetPistol = g_NetworkedVariableManager.GetOffset(/*DT_WeaponUSP*/XorStr<0x88,13,0xD9DE2C14>("\xCC\xDD\xD5\xDC\xE9\xEC\xFE\xE0\xFE\xC4\xC1\xC3"+0xD9DE2C14).s,/*m_bSilencerOn*/XorStr<0xFD,14,0x2902C67C>("\x90\xA1\x9D\x53\x68\x6E\x66\x6A\x66\x63\x75\x47\x67"+0x2902C67C).s);
			static int iOffsetRifle = g_NetworkedVariableManager.GetOffset(/*DT_WeaponM4A1*/XorStr<0x80,14,0x78EF406B>("\xC4\xD5\xDD\xD4\xE1\xE4\xF6\xE8\xE6\xC4\xBE\xCA\xBD"+0x78EF406B).s,/*m_bSilencerOn*/XorStr<0xFD,14,0x2902C67C>("\x90\xA1\x9D\x53\x68\x6E\x66\x6A\x66\x63\x75\x47\x67"+0x2902C67C).s);
		
			int iWeaponID = GetWeaponID();

			if(iWeaponID == WEAPON_USP)
				return (*(PBOOL)((DWORD)this + iOffsetPistol));
			else if(iWeaponID == WEAPON_M4A1)
				return (*(PBOOL)((DWORD)this + iOffsetRifle));

			return false;
		}

		float GetRange()
		{
			switch (GetWeaponID())
			{
			case WEAPON_AK47:
			case WEAPON_AUG:
			case WEAPON_AWP:
			case WEAPON_FAMAS:
			case WEAPON_G3SG1:
			case WEAPON_GALIL:
			case WEAPON_M249:
			case WEAPON_M4A1:
			case WEAPON_SCOUT:
			case WEAPON_SG550:
			case WEAPON_SG552:
				return 8192.f;
				break;
			case WEAPON_DEAGLE:
			case WEAPON_ELITE:
			case WEAPON_FIVESEVEN:
			case WEAPON_GLOCK:
			case WEAPON_MAC10:
			case WEAPON_MP5NAVY:
			case WEAPON_P228:
			case WEAPON_P90:
			case WEAPON_TMP:
			case WEAPON_UMP45:
			case WEAPON_USP:
				return 4096.f;
				break;
			case WEAPON_M3:
			case WEAPON_XM1014:
				return 768.f;
				break;
			default:
				break;
			}

			return 0.f;
		}

		float GetSpread(int iFlags, float m_fFov, float m_flAccuracy, bool m_RifleSilenced, bool m_PistolSilenced, Vector vVelocity, int iWeaponID)
		{
			float flSpread = 0.0f;

			int iSpeed = 0;

			if (vVelocity.Length() < 1)
				iSpeed = 0;
			else
				iSpeed = (int)vVelocity.Length2D();

			switch (iWeaponID) {
			case WEAPON_NONE:
			{
				flSpread = 0.0f;
				break;
			}
			case WEAPON_AK47:
			{

				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.03f + (0.045f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.045f + (0.055f) * (m_flAccuracy));
				else
					flSpread = (((0.035f) + 0.06f) * (m_flAccuracy));
				break;
			}
			case WEAPON_AUG:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.035f + (0.4f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.035f + (0.07f) * (m_flAccuracy));
				else
					flSpread = ((0.02f) * (m_flAccuracy));
				break;
			}
			case WEAPON_AWP:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = 0.95f;
				else if (iSpeed > 140)
					flSpread = 0.75f;
				else if (iSpeed > 10)
					flSpread = 0.8f;
				else if (iFlags & FL_DUCKING)
					flSpread = 0.5f;
				else
					flSpread = 0.001f;

				if (m_fFov == 0 || m_fFov == 90)
					flSpread += 0.08f;
				break;
			}
			case WEAPON_C4:
			{
				flSpread = 0.0f;
				break;
			}
			case WEAPON_DEAGLE:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (1.5 * (1.0 - m_flAccuracy));
				else if (iSpeed > 5)
					flSpread = (0.25 * (1.0 - m_flAccuracy));
				else if ((iFlags & FL_DUCKING))
					flSpread = (0.115 * (1.0 - m_flAccuracy));
				else
					flSpread = (0.13 * (1.0 - m_flAccuracy));
				break;
			}
			case WEAPON_ELITE:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((1.3f) * (1 - m_flAccuracy));
				else if (iSpeed > 5)
					flSpread = ((0.175f) * (1 - m_flAccuracy));
				else if ((iFlags & FL_DUCKING))
					flSpread = ((0.08f) * (1 - m_flAccuracy));
				else
					flSpread = ((0.1f) * (1 - m_flAccuracy));
				break;
			}
			case WEAPON_FAMAS:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.03f + (0.3f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.03f + (0.07f) * (m_flAccuracy));
				else
					flSpread = ((0.02f) * (m_flAccuracy));

				/*if (IsInSpecialMode())
				{
					if (GetFamasShoot() != 0.0f && GetFamasShoot() < Main::m_pServerData->curtime)
					{
						flSpread = GetFamasBurstSpread();
					}
				}
				else
				{
					flSpread += 0.01f;
				}*/
				flSpread += 0.01f;
				break;
			}
			case WEAPON_FIVESEVEN:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((1.5f) * (1 - m_flAccuracy));
				else if (iSpeed > 5)
					flSpread = ((0.255f) * (1 - m_flAccuracy));
				else if (iFlags & FL_DUCKING)
					flSpread = ((0.075f) * (1 - m_flAccuracy));
				else
					flSpread = ((0.15f) * (1 - m_flAccuracy));
				break;
			}
			case WEAPON_FLASHBANG:
			{
				flSpread = 0.0f;
				break;
			}
			case WEAPON_G3SG1:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.04f) * (1 - m_flAccuracy));
				else if (iSpeed > 5)
					flSpread = 0.01f;
				else if ((iFlags & FL_DUCKING))
					flSpread = ((0.065f) * (1 - m_flAccuracy));
				else
					flSpread = ((0.055f) * (1 - m_flAccuracy));

				if (m_fFov == 0 || m_fFov == 90)
					flSpread += 0.025f;
				break;
			}
			case WEAPON_GALIL:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.04f + (0.3f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.04f + (0.07f) * (m_flAccuracy));
				else
					flSpread = ((0.0375f) * (m_flAccuracy));
				break;
			}
			case WEAPON_GLOCK:
			{
				if (IsInSpecialMode())
				{
					/*if (GetGlockShoot() != 0.0f)
					{
						flSpread = 0.05f;
					}
					else
					{
						if (!(iFlags & FL_ONGROUND))
							flSpread = ((1.2f) * (1 - m_flAccuracy));
						else if (iSpeed > 5)
							flSpread = ((0.185f) * (1 - m_flAccuracy));
						else if ((iFlags & FL_DUCKING))
							flSpread = ((0.095f) * (1 - m_flAccuracy));
						else
							flSpread = ((0.3f) * (1 - m_flAccuracy));
					}*/
					if (!(iFlags & FL_ONGROUND))
						flSpread = ((0.85f) * (1 - m_flAccuracy));
					else if (iSpeed > 5)
						flSpread = ((0.185f) * (1 - m_flAccuracy));
					else if ((iFlags & FL_DUCKING))
						flSpread = ((0.095f) * (1 - m_flAccuracy));
					else
						flSpread = ((0.3f) * (1 - m_flAccuracy));
				}
				else
				{
					if (!(iFlags & FL_ONGROUND))
						flSpread = ((1.0f) * (1 - m_flAccuracy));
					else if (iSpeed > 5)
						flSpread = ((0.165f) * (1 - m_flAccuracy));
					else if ((iFlags & FL_DUCKING))
						flSpread = ((0.075f) * (1 - m_flAccuracy));
					else
						flSpread = ((0.1f) * (1 - m_flAccuracy));
				}
				break;
			}
			case WEAPON_HEGRENADE:
			{
				flSpread = 0.0f;
				break;
			}
			case WEAPON_KNIFE:
			{
				flSpread = 0.0f;
				break;
			}
			case WEAPON_M249:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.045f + (0.5f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.045f + (0.095f) * (m_flAccuracy));
				else
					flSpread = ((0.03f) * (m_flAccuracy));
				break;
			}
			case WEAPON_M3:
			{
				flSpread = 0.0675f;
				break;
			}
			case WEAPON_M4A1:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.035f + (0.4f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.035f + (0.07f) * (m_flAccuracy));
				else
				{
					if (IsInSpecialMode())
						flSpread = ((0.025f) * (m_flAccuracy));
					else
						flSpread = ((0.02f) * (m_flAccuracy));
				}
				break;
			}
			case WEAPON_MAC10:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.375f) * (m_flAccuracy));
				else
					flSpread = ((0.03f) * (m_flAccuracy));
				break;
			}
			case WEAPON_MP5NAVY:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.2f) * (m_flAccuracy));
				else
					flSpread = ((0.04f) * (m_flAccuracy));
				break;
			}
			case WEAPON_P228:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((1.5f) * (1 - m_flAccuracy));
				else if (iSpeed > 5)
					flSpread = ((0.255f) * (1 - m_flAccuracy));
				else if ((iFlags & FL_DUCKING))
					flSpread = ((0.075f) * (1 - m_flAccuracy));
				else
					flSpread = ((0.15f) * (1 - m_flAccuracy));
				break;
			}
			case WEAPON_P90:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.3f) * (m_flAccuracy));
				else if (iSpeed > 170)
					flSpread = ((0.115f) * (m_flAccuracy));
				else
					flSpread = ((0.045f) * (m_flAccuracy));
				break;
			}
			case WEAPON_SCOUT:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = 0.2f;
				else if (iSpeed > 170)
					flSpread = 0.075f;
				else if ((iFlags & FL_DUCKING))
					flSpread = 0.0f;
				else
					flSpread = 0.007f;

				if (m_fFov == 0 || m_fFov == 90)
					flSpread += 0.025f;
				break;
			}
			case WEAPON_SG550:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.45f) * (1 - m_flAccuracy));
				else if (iSpeed > 5)
					flSpread = 0.15f;
				else if ((iFlags & FL_DUCKING))
					flSpread = ((0.04f) * (1 - m_flAccuracy));
				else
					flSpread = ((0.05f) * (1 - m_flAccuracy));

				if (m_fFov == 0 || m_fFov == 90)
					flSpread += 0.025f;
				break;
			}
			case WEAPON_SG552:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = (0.035f + (0.45f) * (m_flAccuracy));
				else if (iSpeed > 140)
					flSpread = (0.035f + (0.075f) * (m_flAccuracy));
				else
					flSpread = ((0.02f) * (m_flAccuracy));
				break;
			}
			case WEAPON_SMOKEGRENADE:
			{
				flSpread = 0.0f;
				break;
			}
			case WEAPON_TMP:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.25f) * (m_flAccuracy));
				else
					flSpread = ((0.03f) * (m_flAccuracy));
				break;
			}
			case WEAPON_UMP45:
			{
				if (!(iFlags & FL_ONGROUND))
					flSpread = ((0.24f) * (m_flAccuracy));
				else
					flSpread = ((0.04f) * (m_flAccuracy));
				break;
			}
			case WEAPON_USP:
			{
				if (IsInSpecialMode())
				{
					if (!(iFlags & FL_ONGROUND))
						flSpread = ((1.3f) * (1 - m_flAccuracy));
					else if (iSpeed > 5)
						flSpread = ((0.25f) * (1 - m_flAccuracy));
					else if ((iFlags & FL_DUCKING))
						flSpread = ((0.125f) * (1 - m_flAccuracy));
					else
						flSpread = ((0.15f) * (1 - m_flAccuracy));
				}
				else
				{
					if (!(iFlags & FL_ONGROUND))
						flSpread = ((1.2f) * (1 - m_flAccuracy));
					else if (iSpeed > 5)
						flSpread = ((0.225f) * (1 - m_flAccuracy));
					else if ((iFlags & FL_DUCKING))
						flSpread = ((0.08f) * (1 - m_flAccuracy));
					else
						flSpread = ((0.1f) * (1 - m_flAccuracy));
				}
				break;
			}
			case WEAPON_XM1014:
			{
				flSpread = 0.0725f;
				break;
			}
			default:
			{
				flSpread = 0.0f;
				break;
			}
			}

			return flSpread;
		}

		bool IsInSpecialMode()
		{
			auto WeaponID = GetWeaponID();

			if (WeaponID == WEAPON_FAMAS)
			{
				static auto var = g_NetworkedVariableManager.GetOffset("DT_WeaponFamas", "m_bBurstMode");

				return *(bool*)(this + var);
			}
			else if (WeaponID == WEAPON_GLOCK)
			{
				static auto var = g_NetworkedVariableManager.GetOffset("DT_WeaponGlock", "m_bBurstMode");

				return *(bool*)(this + var);
			}
			else if (WeaponID == WEAPON_M4A1)
			{
				static auto var = g_NetworkedVariableManager.GetOffset("DT_WeaponM4A1", "m_bSilencerOn");

				return *(bool*)(this + var);
			}
			else if (WeaponID == WEAPON_USP)
			{
				static auto var = g_NetworkedVariableManager.GetOffset("DT_WeaponUSP", "m_bSilencerOn");

				return *(bool*)(this + var);
			}

			return false;
		}

		float GetCone()
		{
			return *(float*)(this + 0x760);
		}

		int GetWeaponID()
		{
			typedef int(__thiscall *GetWeaponID_t)(PVOID);
			return getvfunc<GetWeaponID_t>(this, 297)(this);
		}

		bool IsMiscWeapon()
		{
			int iWeaponID = GetWeaponID();

			if (iWeaponID == WEAPON_KNIFE || iWeaponID == WEAPON_C4 || iWeaponID == WEAPON_HEGRENADE
				|| iWeaponID == WEAPON_FLASHBANG || iWeaponID == WEAPON_SMOKEGRENADE)
				return true;

			return false;
		}
	};
}