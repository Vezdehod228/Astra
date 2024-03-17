void __fastcall hkdCreateMove(void* edx, void* ecx, int sequence_number, float input_sample_frametime, bool active);
void __fastcall hkdRunCommand(void* ecx, void* edx, CBaseEntity* pEntity, ValveSDK::CUserCmd* pUserCmd, void* moveHelper);
void __stdcall hkdFrameStageNotify(ClientFrameStage_t curStage);
void __fastcall hkdPaintTraverse(void* ecx, void* edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
int __stdcall hkdDrawModelEx(ValveSDK::ModelRenderInfo_t &pInfo);
ValveSDK::CUserCmd* __stdcall hkdGetUserCmd(int sequence_number);
int __stdcall hkdIN_KeyEvent(int eventcode,ValveSDK::ButtonCode_t keynum,const char *pszCurrentBinding);
void __stdcall hkdFinishMove(CBaseEntity *player, ValveSDK::CUserCmd *ucmd, PVOID move);

extern DWORD dwOriginCreateMove;
extern Base::Utils::CVMTHookManager g_pClientVMT;
extern Base::Utils::CVMTHookManager g_pPanelVMT;
extern Base::Utils::CVMTHookManager g_pPredictionVMT;
extern Base::Utils::CVMTHookManager g_pModelRenderVMT;
extern Base::Utils::CVMTHookManager g_pInputVMT;

extern ValveSDK::IMaterial *pNew;
extern ValveSDK::IMaterial *pIgnoreNew;
extern ValveSDK::IMaterial *pBrightNew;
extern ValveSDK::IMaterial *pBrightIgnoreNew;

extern ValveSDK::IMaterial* m_pVertexIn;
extern ValveSDK::IMaterial* m_pVertexOut;

extern ValveSDK::IMaterial* m_pMatIn;
extern ValveSDK::IMaterial* m_pMatOut;

extern ValveSDK::IMaterial* m_pOut;
extern ValveSDK::IMaterial* m_pIn;

class KeyValues
{
public:
	KeyValues(const char* setName);
	bool LoadFromBuffer(const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = nullptr);

private:
	char pad[0x25];
};