#ifdef C0GNITO_EXPORTS
#define C0GNITO_API __declspec(dllexport)
#else
#define C0GNITO_API __declspec(dllimport)
#endif

extern "C" C0GNITO_API bool Initialize(const char* publicKey);
extern "C" C0GNITO_API bool Authenticate(const char* license, const char* hwid = "");
extern "C" C0GNITO_API const char* GetReturnMessage();
extern "C" C0GNITO_API int GetExpirationTime();
extern "C" C0GNITO_API int GetLevel();
extern "C" C0GNITO_API const char* GetHardwareID();