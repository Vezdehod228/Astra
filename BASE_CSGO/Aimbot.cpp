#include "dllFunction.h"

//optimizing code so we call setupbones only once
Vector GetBoneFromMatrix(matrix3x4_t* MatrixArray, int iBone)
{
	return Vector(MatrixArray[iBone][0][3], MatrixArray[iBone][1][3], MatrixArray[iBone][2][3]);
}

static void GetMaterialParameters(int iMaterial, float& flPenetrationModifier, float& flDamageModifier)
{
	switch (iMaterial)
	{
	case CHAR_TEX_METAL:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.3;
		break;
	case CHAR_TEX_DIRT:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.3;
		break;
	case CHAR_TEX_CONCRETE:
		flPenetrationModifier = 0.4;
		flDamageModifier = 0.25;
		break;
	case CHAR_TEX_GRATE:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.99;
		break;
	case CHAR_TEX_VENT:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.45;
		break;
	case CHAR_TEX_TILE:
		flPenetrationModifier = 0.65;
		flDamageModifier = 0.3;
		break;
	case CHAR_TEX_COMPUTER:
		flPenetrationModifier = 0.4;
		flDamageModifier = 0.45;
		break;
	case CHAR_TEX_WOOD:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.6;
		break;
	default:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.5;
		break;
	}

	Assert(flPenetrationModifier > 0);
	Assert(flDamageModifier < 1.0f);
}

typedef void (*ClipTraceToPlayers_t)(const Vector&, const Vector&, unsigned int, ValveSDK::CTrace::ITraceFilter*, ValveSDK::CTrace::trace_t*);

#define	CS_MASK_SHOOT (MASK_SOLID|CONTENTS_DEBRIS)

bool CAimbot::AutoWall(WeaponInfo_t wiWeaponInfo, Vector vStart, Vector vEnd)
{
	ValveSDK::CTrace::trace_t tr;
	ValveSDK::CTrace::Ray_t ray;
	ValveSDK::CTrace::ITraceFilter* pTr = (ValveSDK::CTrace::ITraceFilter*)&g_Aimbot.tfNoPlayers;

	Vector vecSrc = vStart, vDir = (vEnd - vStart);
	Vector vecDirShooting, vecRight, vecUp;

	vDir = vecDirShooting +
		vStart * vecSrc * vecRight +
		vEnd * vecSrc * vecUp;

	//Vector( vDir ).NormalizeInPlace();

	Vector vEndPos2;
	float flDistance = 0;
	float flPenetrationModifier;
	float fDamageModifier;

	int fPinetrationDistance = wiWeaponInfo.fPenetrationDistance;
	float fPenetrationPower = wiWeaponInfo.fPenetrationPower;
	int iPenetration = wiWeaponInfo.iPenetration;
	float iCurrentDamage = wiWeaponInfo.iDamage;

	static DWORD dwCliptracetoplayers = NULL;
	if (dwCliptracetoplayers == NULL)
		dwCliptracetoplayers = Base::Utils::PatternSearch("client.dll", (PBYTE)"\x81\xEC\x00\x00\x00\x00\x8B\x84\x24\x00\x00\x00\x00\x8B\x48\x2C", "xx????xxx????xxx", NULL, NULL);

	static ClipTraceToPlayers_t ClipTraceToPlayersCall = (ClipTraceToPlayers_t)dwCliptracetoplayers;
	ValveSDK::surfacedata_t* DataSurface = g_Valve.pPhysics->GetSurfaceData(tr.surface.surfaceProps);
	float fRange = Vector( vDir ).Length();

	while ( iCurrentDamage > 0 )
	{
		int main = CS_MASK_SHOOT | CONTENTS_HITBOX;
		ray.Init(vecSrc, vEnd);
		g_Valve.pEngineTrace->TraceRay(ray, main, pTr, &tr);
		ClipTraceToPlayersCall(vecSrc, vEnd + vecSrc * 40.0f, main, pTr, &tr);

		//if (tr.fraction < 1.0f)
			//tr.fraction = 1.0f;

		if (tr.fraction == 1.0f)
			return true;

		flDistance += tr.fraction * fRange;
		iCurrentDamage *= pow(wiWeaponInfo.fRangeModifier, (flDistance / 500));

		bool hitGrate = (tr.contents & CONTENTS_GRATE);
		int iMaterial = DataSurface->game.material;

		GetMaterialParameters( iMaterial, flPenetrationModifier, fDamageModifier );

		if ( hitGrate )
		{
			flPenetrationModifier = 1.0f;
			fDamageModifier = 0.99f;
		}

		if ( flDistance > fPinetrationDistance && iPenetration > 0 )
			iPenetration = 0;

		if ( iPenetration == 0 && !hitGrate)
			break;

		if ( iPenetration < 0 )
			break;

		bool intGrate = hitGrate && (tr.contents & CONTENTS_GRATE);
		if (intGrate == CHAR_TEX_WOOD)
		{
			if (intGrate == CHAR_TEX_DIRT)
			{
				fDamageModifier *= 2;
			}
		}

		ValveSDK::CTrace::trace_t exitTr;
		g_Valve.pEngineTrace->TraceRay(ray, main, NULL, &exitTr);
		if ((exitTr.m_pEnt != tr.m_pEnt) && exitTr.m_pEnt != NULL)
		{
			ValveSDK::CTrace::CSimpleTraceFilter pTr2 = exitTr.m_pEnt;
			g_Valve.pEngineTrace->TraceRay(ray, main, &pTr2, &exitTr);
		}

		float flNewDistance = Vector(exitTr.startpos - exitTr.endpos).Length();//Vector(exitTr.endpos - tr.endpos).Length();
		if (flNewDistance > (fPenetrationPower * flPenetrationModifier))
			break;

		fPenetrationPower -= (flNewDistance / flPenetrationModifier) * 0.5;
		vecSrc = exitTr.endpos;
		iCurrentDamage *= fDamageModifier;
		flDistance = (flDistance - flNewDistance) * 0.5f;
		iPenetration--;
	}

	return false;
}

bool CAimbot::MultiPoints(CBaseEntity* pPlayer, Vector& vPos, int iHitBox)
{
	Vector vMax;
	Vector vMin;

	matrix3x4_t matrix[128];
	studiohdr_t* hdr = g_Valve.pModel->GetStudiomodel(pPlayer->GetModel());
	mstudiobbox_t* hitbox = hdr->pHitboxSet(NULL)->pHitbox(iHitBox);


	if (!pPlayer->SetupBones(matrix, 0x080, 0x100, NULL))
		return false;

	if (!hdr)
		return false;

	//if (!hitbox)
		//return false;

	g_Math.VectorTransform(hitbox->bbmin, matrix[hitbox->bone], vMin);
	g_Math.VectorTransform(hitbox->bbmax, matrix[hitbox->bone], vMax);

	vPos = (vMin + vMax) * 0.5f;
	vPos.z += 4.5f;

	if (!Config.CvarList[EnableAim] && IsVisible(vEyePos, vPos, CS_MASK_SHOOT, (ValveSDK::CTrace::ITraceFilter*)&g_Aimbot.tfNoPlayers))
		return true;
	else if (Config.CvarList[EnableAim] && CAimbot::AutoWall(wiWeaponInfo, vEyePos, vPos))
		return true;

	return false;
}

void CalcAngle(Vector& vSource, Vector& vDestination, Vector& qAngle)
{
	Vector vDelta = vSource - vDestination;

	float fHyp = (vDelta.x * vDelta.x) + (vDelta.y * vDelta.y);

	float fRoot;

	__asm
	{
		sqrtss xmm0, fHyp
		movss fRoot, xmm0
	}

	qAngle.x = RAD2DEG(atan(vDelta.z / fRoot));
	qAngle.y = RAD2DEG(atan(vDelta.y / vDelta.x));

	if (vDelta.x >= 0.0f)
		qAngle.y += 180.0f;

	//qAngle.x = g_NoSpread.AngleNormalize(qAngle.x);
	//qAngle.y = g_NoSpread.AngleNormalize(qAngle.y);
}

float GetFov(Vector vLocalOrigin, Vector vPosition, Vector vForward)
{
	Vector vLocal;

	VectorSubtract(vPosition, vLocalOrigin, vLocal);
	vLocal.NormalizeInPlace();
	float fValue = vForward.Dot(vLocal);

	if (fValue < -1.0f)
		fValue = -1.0f;

	if (fValue > 1.0f)
		fValue = 1.0f;

	return RAD2DEG(acos(fValue));
}

bool CAimbot::IsVisible(Vector vStart, Vector vEnd, unsigned int nMask, ValveSDK::CTrace::ITraceFilter* pTraceFilter)
{
	ValveSDK::CTrace::Ray_t ray;
	ValveSDK::CTrace::trace_t tr;

	ray.Init(vStart, vEnd);
	g_Valve.pEngineTrace->TraceRay(ray, nMask, pTraceFilter, &tr);

	return (tr.fraction == 1.0f && tr.m_pEnt && tr.m_pEnt->IsPlayer());
}

void CAimbot::FixMovement(ValveSDK::CUserCmd* c, Vector& qOld, int iPositive)
{
	Vector vMove(c->forwardmove, c->sidemove, c->upmove);
	float fSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y);
	static Vector qMove;
	g_NoSpread.vectorAngles(vMove, qMove);

	float fYaw = DEG2RAD(c->viewangles.y - qOld.y + qMove.y);

	c->forwardmove = cos(fYaw) * fSpeed * iPositive;
	c->sidemove = sin(fYaw) * fSpeed;
}

void GetWeaponInfo(ValveSDK::CBaseCombatWeapon* pWeapon, WeaponInfo_t& wiInfo, int iWeaponID, bool bSilencer)
{
	switch (iWeaponID)
	{
	case WEAPON_AK47:
	{
		wiInfo.iBulletType = 2.2;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.91f;
		break;
	}

	case WEAPON_AUG:
	{
		wiInfo.iBulletType = 2;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.96f;
		break;
	}

	case WEAPON_AWP:
	{
		wiInfo.iBulletType = 5;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.79f;
		break;
	}

	case WEAPON_DEAGLE:
	{
		wiInfo.iBulletType = 1;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.81f;
		break;
	}

	case WEAPON_ELITE:
	{
		wiInfo.iBulletType = 6;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.75f;
		break;
	}

	case WEAPON_FAMAS:
	{
		wiInfo.iBulletType = 3;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.96f;
		break;
	}

	case WEAPON_FIVESEVEN:
	{
		wiInfo.iBulletType = 10;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.885f;
		break;
	}

	case WEAPON_G3SG1:
	{
		wiInfo.iBulletType = 2.6;
		wiInfo.iPenetration = 2.8;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.86f;
		break;
	}

	case WEAPON_GALIL:
	{
		wiInfo.iBulletType = 3;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.98f;
		break;
	}

	case WEAPON_GLOCK:
	{
		wiInfo.iBulletType = 6;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.75f;
		break;
	}

	case WEAPON_M249:
	{
		wiInfo.iBulletType = 4;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.97f;
		break;
	}

	case WEAPON_M4A1:
	{
		wiInfo.iBulletType = 3;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = (bSilencer ? 0.95f : 0.97f);
		break;
	}

	case WEAPON_MAC10:
	{
		wiInfo.iBulletType = 8;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.82f;
		break;
	}

	case WEAPON_MP5NAVY:
	{
		wiInfo.iBulletType = 6;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.84f;
		break;
	}

	case WEAPON_P228:
	{
		wiInfo.iBulletType = 9;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.8f;
		break;
	}

	case WEAPON_P90:
	{
		wiInfo.iBulletType = 10;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.84f;
		break;
	}

	case WEAPON_SCOUT:
	{
		wiInfo.iBulletType = 2;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.98f;
		break;
	}

	case WEAPON_SG550:
	{
		wiInfo.iBulletType = 3;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.77f;
		break;
	}

	case WEAPON_SG552:
	{
		wiInfo.iBulletType = 3;
		wiInfo.iPenetration = 2;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 8192.0f;
		wiInfo.fRangeModifier = 0.955f;
		break;
	}

	case WEAPON_TMP:
	{
		wiInfo.iBulletType = 6;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.84f;
		break;
	}

	case WEAPON_UMP45:
	{
		wiInfo.iBulletType = 8;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.82f;
		break;
	}

	case WEAPON_USP:
	{
		wiInfo.iBulletType = 8;
		wiInfo.iPenetration = 1;
		wiInfo.fMaxRange = 4096.0f;
		wiInfo.fRangeModifier = 0.79f;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		break;
	}
	case WEAPON_XM1014:
	{
		wiInfo.iBulletType = 7;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = Config.CvarList[MinDamage];
		wiInfo.fRangeModifier = 0.7f;
		wiInfo.fMaxRange = 3000.0f;
		break;
	}
	case WEAPON_M3:
	{
		wiInfo.iBulletType = 7;
		wiInfo.iPenetration = 1;
		wiInfo.iDamage = 26;
		wiInfo.fRangeModifier = 0.7f;
		wiInfo.fMaxRange = 3000.0f;
		break;
	}
	default:
	{
		break;
	}
	}

	static DWORD dwBulletParams = NULL;

	if (dwBulletParams == NULL)
	{
		dwBulletParams = Base::Utils::PatternSearch("client.dll", (PBYTE)"\x56\x8B\x74\x24\x08\x68\x00\x00\x00\x00\x56\xE8\x00\x00\x00\x00\x83\xC4\x08\x84\xC0", "xxxxxx????xx????xxxxx", NULL, NULL);
	}

	float fPenDist;
	float fPenPow;

	int iBulletTup = wiInfo.iBulletType;

	__asm
	{
		LEA ECX, fPenDist
		PUSH ECX
		LEA EDX, fPenPow
		PUSH EDX
		PUSH iBulletTup
		CALL dwBulletParams
	}

	wiInfo.fPenetrationDistance = fPenDist;
	wiInfo.fPenetrationPower = fPenPow;
}

void CAimbot::Main(ValveSDK::CUserCmd* cmd, CBaseEntity* m_pLocal, ValveSDK::CBaseCombatWeapon* pWeapon)
{
	GetWeaponInfo(pWeapon, wiWeaponInfo, pWeapon->GetWeaponID(), pWeapon->IsSilencerOn());

	vEyePos = m_pLocal->GetEyePosition();
	Reset();

	static Vector vClientViewAngles;
	g_Valve.pEngine->GetViewAngles(vClientViewAngles);

	static Vector vAngle;
	g_NoSpread.angleVectors(vClientViewAngles, vAngle);

	if (Config.CvarList[EnableAim])
	{
		for (INT ax = 1; ax <= g_Valve.pEngine->GetMaxClients(); ax++)
		{
			CBaseEntity* pBaseEntity = g_Valve.pEntList->GetClientEntity(ax);

			if (!pBaseEntity
				|| pBaseEntity == m_pLocal
				|| !pBaseEntity->isValidPlayer()
				|| fWhiteList[ax]
				|| (!Config.CvarList[AimTeam] && pBaseEntity->GetTeamNum() == m_pLocal->GetTeamNum())
				|| (Config.CvarList[AntiDM] && pBaseEntity->IsSpawnProtectedPlayer())
				|| !MultiPoints(pBaseEntity, vTarget, (fSpecialAimspot[ax] != -1 ? fSpecialAimspot[ax] : Config.CvarList[AimSpot])))
				continue;

			vFinal = vTarget;
			iTarget = ax;
		}

		if (HasTarget())
		{
			CalcAngle(vEyePos, vFinal, cmd->viewangles);

			if (!Config.CvarList[SilentAim])
				g_Valve.pEngine->SetViewAngles(cmd->viewangles);

			if (Config.CvarList[Autoshoot])
				cmd->buttons |= IN_ATTACK;
		}
	}
}