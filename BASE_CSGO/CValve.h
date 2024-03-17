class CValve
{
public:
	VOID initSDK();
	BOOL isInitiated();
	bool WorldToScreen(const Vector& vOrigin, Vector& vScreen);

	ValveSDK::HLCLient* pClient;
	ValveSDK::CEngineClient* pEngine;
	ValveSDK::ISurface* pSurface;
	ValveSDK::IPanel* pPanel;
	ValveSDK::CEntityList* pEntList;
	ValveSDK::CInput* pInput;
	ValveSDK::CPrediction* pPred;
	ValveSDK::CGameMovement* pGameMovement;
	ValveSDK::CGlobalVars* pGlobalVars;
	ValveSDK::CModelInfo* pModel;
	ValveSDK::CDebugOverlay* pDebugOverlay;
	ValveSDK::CTrace* pEngineTrace;
	ValveSDK::ImoveHelper* pMoveHelper;
	ValveSDK::IGameEventManager* pGameEventManager;
	ValveSDK::IPhysicsSurfaceProps* pPhysics;
	ValveSDK::IVModelRender* pModelRender;
	ValveSDK::IVRenderView* pRenderView;
	ValveSDK::IMaterialSystem* pMaterialSystem;
	ValveSDK::ICvar* pConVar;

private:
	BOOL m_bInitiated;

};