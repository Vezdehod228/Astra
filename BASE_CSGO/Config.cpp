#include "dllFunction.h"

cCVARS Config;

std::string CvarNames[MAX_CVARS];

void cCVARS::Init(HMODULE hinstDLL)
{
	GetModuleFileName(hinstDLL, Config.szIniFilePath,2048);

	for(int i = strlen(Config.szIniFilePath); i > 0; i--)
	{ 
		if(Config.szIniFilePath[i] == '\\')
		{ 
			Config.szIniFilePath[i+1] = 0;
			break;//break loop so we dont delete everything
		} 
	}

	for(int i = 0; i < MAX_CVARS; i++)
	{
		char szName[10];
		sprintf(szName,"[var%i]",i);
		CvarNames[i] = szName;
	}
}

//kolonote:
//ok time to use my skillz ehi ehi
//note that this is pretty slow soooo
float FindVarValue(const char *szTextArray, const char *pszVarName)
{
	int iLen = strlen(pszVarName) - 1;

	for(int i = 0; i < strlen(szTextArray); i++)
	{
		//first character matches but we gotta check if whole word matches
		if(szTextArray[i] == pszVarName[0])
		{
			bool bSkipThis = false;

			for(int i2 = 0; i2 <= iLen; i2++)
			{
				//we already checked for first character in the array
				if(i2 == 0)
					continue;

				if(szTextArray[i+i2] != pszVarName[i2])
				{
					bSkipThis = true;
					break;//ok not our string lets skip this one
				}
			}

			//something didnt match so lets skip to this one
			if(bSkipThis)
				continue;

			//everything matches meaning we can retrieve the value now
			std::string sOwn = &szTextArray[i+iLen+1];//move 1 more spot as thats where the space is

			return atof(sOwn.data());
		}
	}

	return 0.0f;
}

void cCVARS::HandleConfig(const char *pszConfigName, int iType)
{
	//kolonote:
	//to save shit were gonna generate a config file
	std::string sSavedConfig;
	//our file loaded from server
	std::string sLoadResponse;

	HW_PROFILE_INFO hwProfileInfo;

	//REMEMBER: TEMPORARY
	//this is utter shit but i didnt find a quicker way for unique identification and i need it to store configs for different users on server
	GetCurrentHwProfile(&hwProfileInfo);

	for(int varindex = 0;varindex < MAX_CVARS; varindex++)
	{
		bool bLastInLoop = (varindex == (MAX_CVARS-1));
		bool bFirstInLoop = (varindex == 0);
	}
}