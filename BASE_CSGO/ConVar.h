namespace ValveSDK
{
	class ConVar
	{
	public:
		void*		m_pVmt;
		ConVar*		m_pNext;
		BOOL		m_bRegistered;
		const char*	m_pszName;
		const char*	m_pszHelpString;
		int			m_nFlags;
		ConVar*		m_pParent;
		const char*	m_pszDefaultValue;
		char*		m_pszString;
		int			m_StringLength;
		float		m_fValue;
		int			m_nValue;
		bool		m_bHasMin;
		float		m_fMinVal;
		bool		m_bHasMax;
		float		m_fMaxVal;
		void*		m_fnChangeCallback;
	};

	class IAppSystem
	{
	public:
		// Here's where the app systems get to learn about each other 
		virtual void func00() = 0;
		virtual void func01() = 0;
		virtual void func02() = 0;
		virtual void func03() = 0;
		virtual void func04() = 0;
	};

	class ICvar
	{
	public:
		ConVar* FindVar(const char* var_name)
		{
			return getvfunc<ConVar*(__thiscall*)(void*, const char*)>(this, 7)(this, var_name);
		}
	};
}