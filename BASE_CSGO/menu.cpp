#include "dllFunction.h"

cMenu g_Menu(450,250,575,380);
cListItem ListItemArray[NUM_OF_TOTAL_LISTITEMS];

bool bSliderFix[50];

int iSliderIndex = -1;
int iCurrSlider;

//getasynckeystate fixing...
DWORD dwWait;

//menu colors (format: RGBA)
#define COLOR1 212, 212, 212, 255
#define COLOR2 222, 222, 222, 255
#define COLOR3 185, 185, 185, 255
#define COLOR4 233, 233, 233, 255
#define COLOR5 216, 216, 216, 255
#define COLOR6 100, 100, 100, 255
#define COLOR7 233, 233, 233, 255
#define COLOR8 170, 152, 162, 255
#define COLOR9 185, 171, 154, 255
#define COLOR10 233, 233, 233, 255
#define COLOR11 13, 151, 235, 255

#define COLOR_CFG_POINTERS COLOR1
#define COLOR_CFG_POINTERS_BORDER 140, 0, 0, 255

#define TEXTCOLOR1 255,255,255, 255

#define LISTITEM_TEXTCOLOR 255,255,255, 255

#define ELEMENT_SEPERATION 20
#define CHECKBOX_SEPERATION_FROM_TEXT 105
#define CHECKBOX_SIZE 17

#define ADDER_SIZE 16
#define ADDER_SEPERATE_FROM_BOXES 44 

#define DEFAULT_X_TO_ADD (CHECKBOX_SEPERATION_FROM_TEXT + CHECKBOX_SIZE)
#define SLIDER_X_TO_ADD 40

#define DROPDOWN_WIDTH 82
#define DROPDOWN_HEIGHT 20

#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT 20

cMenu::cMenu(int x, int y, int w, int h)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
}

char *pszAimspotNames[] = {"Pelvis","L Chest","Chest","Neck","Head"};
float fAimspotValues[] = {1, 9, 10, 11, 12};

char *pszAntiAimXNames[] = { "Off", "Down" };
float fAntiAimXValues[] = { 0, 1 };

char *pszAntiAimYNames[] = { "Off", "Backward", "Jitter", "Unhittable" };
float fAntiAimYValues[] = { 0, 1, 2, 3 };

void cMenu::InitMenuElements()
{
	//all menu sections
	static cMenuSection msAimbot;
	static cMenuSection msVisualsESP;
	static cMenuSection msAntiAim;
	static cMenuSection msMisc;
	static cMenuSection msColors;

	int iRowTwo = DEFAULT_X_TO_ADD;
	int iRowThree = (DEFAULT_X_TO_ADD) * 2;

	//the usual x position for sections/itemlists
	int iUsualX = m_x + 109;
	int iListY = m_y + 45;


	//playerlist stuff
	static bool bDoOnce = true;
	static int iPlayerIndex;
	static float fResetHandle;
	std::string sCurrPlayer;

	//blabla
	iCurrSlider = 0;
	 
	switch(GetTabIndex())
	{
	case AIMBOT:

		msAimbot.ClearSection();

		msAimbot.Draw(iUsualX, m_y + 35, m_w - 111, 344);

		msAimbot.AddElement(ONOFF,0,"Aimbot","",&Config.CvarList[EnableAim]);
		if (Config.CvarList[EnableAim])
		{
			msAimbot.AddElement(ONOFF, 0, "Auto Shoot", "", &Config.CvarList[Autoshoot]);
			msAimbot.AddElement(ONOFF, 0, "Silent Aim", "", &Config.CvarList[SilentAim]);
			msAimbot.AddElement(ONOFF, 0, "Lag Fix", "", &Config.CvarList[lagFix]);

			msAimbot.SetSlider(iCurrSlider, true);
			msAimbot.AddElement(SLIDER, 6, "Min Damage", "", &Config.CvarList[MinDamage], 0, 100);
		}

		msAimbot.SetValueNames(pszAimspotNames, fAimspotValues, 5);
		msAimbot.AddElement(DROPDOWN, 0, "Hitbox", "", &Config.CvarList[AimSpot]);

		msAimbot.PostSection();
		break;
	case VISUALS:
		msVisualsESP.ClearSection();
		msVisualsESP.Draw(iUsualX, m_y + 35, m_w - 111, 344);

		msVisualsESP.AddElement(ONOFF, 0, "ESP", "", &Config.CvarList[EnableESP]);
		if (Config.CvarList[EnableESP])
		{
			msVisualsESP.AddElement(ONOFF, 0, "Health ESP", "", &Config.CvarList[HealthESP]);
			msVisualsESP.AddElement(ONOFF, 0, "Box ESP", "", &Config.CvarList[BoxESP]);
			msVisualsESP.AddElement(ONOFF, 0, "Name ESP", "", &Config.CvarList[NameESP]);
		}
		msVisualsESP.RestartSection();

		//msVisualsESP.SetValueNames(pszChamsNames, fChamsValues, 3);
		msVisualsESP.AddElement(ONOFF, 230, "Chams", "", &Config.CvarList[Chams]);
		if (Config.CvarList[Chams])
		{
			msVisualsESP.AddElement(ONOFF, 230, "Bright Chams", "", &Config.CvarList[BrightChams]);
			msVisualsESP.AddElement(ONOFF, 230, "XYZ Chams", "", &Config.CvarList[XQZ]);
			msVisualsESP.AddElement(ONOFF, 230, "Enemy Only", "", &Config.CvarList[ChamsEnemyOnly]);
		}

		msVisualsESP.PostSection();
		break;
	case ANTIAIMS:
		msAntiAim.ClearSection();
		msAntiAim.Draw(iUsualX, m_y + 35, m_w - 111, 344);

		msAntiAim.AddElement(ONOFF, 0, "Anti Aim", "", &Config.CvarList[EnableAntiAim]);

		if (Config.CvarList[EnableAntiAim])
		{
			msAntiAim.SetValueNames(pszAntiAimXNames, fAntiAimXValues, 2);
			msAntiAim.AddElement(DROPDOWN, 0, "Pitch", "", &Config.CvarList[AntiAimX]);
			msAntiAim.SetValueNames(pszAntiAimYNames, fAntiAimYValues, 4);
			msAntiAim.AddElement(DROPDOWN, 0, "Yaw", "", &Config.CvarList[AntiAimY]);
		}

		msAntiAim.PostSection();
		break;
	case MISC:
		msMisc.ClearSection();
		msMisc.Draw(iUsualX, m_y + 35, m_w - 111, 344);

		msMisc.AddElement(ONOFF, 0, "No Spread", "", &Config.CvarList[NoSpread]);
		msMisc.AddElement(ONOFF, 0, "No Recoil", "", &Config.CvarList[NoRecoil]);
		msMisc.AddElement(ONOFF, 0, "No Visual Recoil", "", &Config.CvarList[NoVisRecoil]);
		msMisc.AddElement(ONOFF, 0, "Bunny Hop", "", &Config.CvarList[Bunnyhop]);
		msMisc.AddElement(ONOFF, 0, "Strafe Bot", "", &Config.CvarList[Strafebot]);
		msMisc.AddElement(ONOFF, 0, "Auto Pistol", "", &Config.CvarList[Autopistol]);
		msMisc.AddElement(ONOFF, 0, "sv_cheats", "", &Config.CvarList[svCheats]);
		//msMisc.AddElement(ONOFF, 0, "Watermark", "", &Config.CvarList[Watermark]);
		msMisc.AddElement(ONOFF, 0, "Round Say", "", &Config.CvarList[RoundSay]);

		msMisc.PostSection();
		break;
	}
}

int iLoop[] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

void cMenu::DrawSmallTab(int index, int &setindex, int x, int y, const char *pszTitle)
{
	static int iWidth = 100;
	static int iHeight = 19;

	bool bOverTri = false;

	bool bOverBack = g_Mouse.IsOver(x + iHeight, y - iHeight, iWidth + 1 - iHeight, iHeight + 1);

	bool bSame = setindex == index;

	for (int ax = 0; ax <= iHeight; ax++)
	{
		if (g_Mouse.IsOver(x + ax, y - ax, 2, ax) || g_Mouse.IsOver(x + iWidth + ax, y - iHeight, 2, iLoop[ax]))
		{
			bOverTri = true;
			break;
		}
	}

	for (int i = 0; i <= iHeight; i++)
	{
		if (bSame)
		{
			if (i == iHeight)
				g_Draw.FillRGBA(x + iHeight, y - iHeight, iWidth - iHeight, iHeight + 1, 108, 96, 240, 255);
		}
		else
		{
			if (bOverBack || bOverTri)
				g_Valve.pSurface->DrawSetColor(174, 194, 255, 255);
			else
				g_Valve.pSurface->DrawSetColor(COLOR1);

			if (g_Mouse.HasMouseOneJustBeenReleased() && (bOverBack || bOverTri))
				setindex = index;

			if (i == iHeight)
			{
				if (bOverBack || bOverTri)
					g_Draw.FillRGBA(x + iHeight, y - iHeight, iWidth - iHeight, iHeight + 1, 108, 96, 240, 255 );
				else
					g_Draw.FillRGBA(x + iHeight, y - iHeight, iWidth - iHeight, iHeight + 1, 233, 233, 233, 255);
			}
		}
	}


	g_Valve.pSurface->DrawSetColor(COLOR2);

	g_Draw.DrawStringA(g_Draw.ForMiniTabPanel ,true,x + (iWidth + iHeight) * 0.5,y - 4 - iHeight,TEXTCOLOR1,pszTitle);
}

int iNubLoop[] = {22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

void cMenu::DrawTab(int index, int &setindex, int x, int y, const char *pszTitle)
{
	static int iWidth = 143;
	static int iHeight = 36;

	bool bOverTri = false;

	bool bOverBack = g_Mouse.IsOver(x + iHeight, y - iHeight,iWidth - iHeight,iHeight);

	bool bSame = setindex == index;

	for(int ax = 0; ax <= iHeight; ax++)
	{
		if(g_Mouse.IsOver(x + ax,y - ax,2,ax) || g_Mouse.IsOver(x + iWidth + ax,y - iHeight,2,iNubLoop[ax]))
		{
			bOverTri = true;
			break;
		}
	}

	for (int i = 0; i <= iHeight; i++)
	{
		if(bSame)
		{
			if (i == iHeight)
				g_Draw.FillRGBA(x + iHeight, y - iHeight, iWidth - iHeight, iHeight + 1, 69, 26, 190, 255);
		}
		else
		{
			if(bOverBack || bOverTri)
				g_Valve.pSurface->DrawSetColor(69, 26, 190, 255);
			else
				g_Valve.pSurface->DrawSetColor(COLOR1);

			if(g_Mouse.HasMouseOneJustBeenReleased() && (bOverBack || bOverTri))
				setindex = index;

			if(i == iHeight)
			{
				if (bOverBack || bOverTri)
				{
					g_Draw.FillRGBA(x + iHeight, y - iHeight, iWidth - iHeight, iHeight + 1, 24, 18, 39, 255);
				}
				else
					g_Draw.FillRGBA(x + iHeight, y - iHeight, iWidth - iHeight, iHeight + 1, 24, 24, 24, 255);
			}
		}
	}

	g_Valve.pSurface->DrawSetColor(COLOR2);

	g_Draw.DrawStringA(g_Draw.ForTabPanel, true, x + (iWidth + iHeight) * 0.5, y - iHeight + 10, TEXTCOLOR1, pszTitle);
}

void cMenu::DrawAConfig(int index, int cfgidx, int x, int y, std::string sString)
{
	for(int i = 0; i < sString.size(); i++)
	{
		std::string sCopy = sString;

		sCopy = &sCopy[i]; 
		sCopy[1] = 0;

		g_Draw.DrawStringA(g_Draw.m_MenuFont,false, x, y + (i * 10), TEXTCOLOR1, sCopy.data());
	}

	if(GetConfigIndex(cfgidx) != index)
		SetConfigIndex(index,cfgidx);
}

void OutlinedRectangle(int x,int y,int w,int h, int r, int g, int b, int a)
{
	g_Draw.FillRGBA(x,y,w,1,r,g,b,a);
	g_Draw.FillRGBA(x,y,1,h,r,g,b,a);
	g_Draw.FillRGBA(x+w,y,1,h+1,r,g,b,a);
	g_Draw.FillRGBA(x,y+h,w,1,r,g,b,a);
}

void SoftOutlinedRectangle(int x,int y,int w,int h, int r, int g, int b, int a)
{
	//top
	g_Draw.FillRGBA(x + 2,y,w - 3,1,r,g,b,a);
	g_Draw.FillRGBA(x + 1,y + 1,1,1,r,g,b,a);

	//left
	g_Draw.FillRGBA(x,y + 2,1,h - 3,r,g,b,a);
	g_Draw.FillRGBA(x + 1,y+h - 1,1,1,r,g,b,a);

	//right
	g_Draw.FillRGBA(x+w,y + 2,1,h - 3,r,g,b,a);
	g_Draw.FillRGBA(x+w - 1,y + 1,1,1,r,g,b,a);

	//bottom
	g_Draw.FillRGBA(x + 2,y+h,w - 3,1,r,g,b,a);
	g_Draw.FillRGBA(x+w - 1,y+h - 1,1,1,r,g,b,a);
}

#define ELEMENT_HEIGHT 15

void cListItem::Draw(int x, int y,int w, int h)
{
	int iNumOfItems = GetNumberOfItems();

	if(iNumOfItems == 0)
	{
		g_Draw.FillRGBA(x+w+2,y,8,h,COLOR2);

		OutlinedRectangle(x+w+2,y,8,h,COLOR6);

		OutlinedRectangle(x,y,w,h,COLOR2);

		return;
	}

	//we lost some old members probably... so we gotta make sure we still have some kind of selection
	if(iSelectedIndex >= iNumOfItems)
		iSelectedIndex = (iNumOfItems - 1);

	if(iTopIndex >= iNumOfItems)
		iTopIndex = 0;

	DisplayPerPage(((h / ELEMENT_HEIGHT) - 1));

	bScrollingNeeded = true;

	//we can display all items already so we dont have to change our top index
	if(iDisplayPerPage > iNumOfItems)
		bScrollingNeeded = false;

	//counter for items we will display/draw
	int iDisplayed = 0;

	for(int i = iTopIndex; i < iNumOfItems; i++)
	{
		//once display limit reached (if its even reached), we dont wanna display anymore
		if(iDisplayed > iDisplayPerPage)
			break;

		bool bBlackText = false;

		//current draw item
		//int iItemIndex = iTopIndex + i;
		ListItem_t liItem = GetItemFromIndex(i);

		int iDrawIndex = i - iTopIndex;
		
		//every second-item will have a different color (another optional rule for lists)
		if((iDrawIndex & 2) == 1)//the leftover when dividing (i) by 2 is zero
			g_Draw.FillRGBA(x,y + iDrawIndex * ELEMENT_HEIGHT,w,ELEMENT_HEIGHT,255,255,255,255);

		if(g_Mouse.IsOver(x,y + iDrawIndex * ELEMENT_HEIGHT,w,ELEMENT_HEIGHT + 1))
		{
			//mouse is hoovering over our item ????
			g_Draw.FillRGBA(x,y + iDrawIndex * ELEMENT_HEIGHT,w,ELEMENT_HEIGHT,COLOR9);

			//handle selection upon a mouse click
			if(g_Mouse.HasMouseOneJustBeenReleased())
				this->Select(i);

			bBlackText = true;
		}

		if(iSelectedIndex == i)
		{
			g_Draw.FillRGBA(x,y + iDrawIndex * ELEMENT_HEIGHT,w,ELEMENT_HEIGHT,COLOR9);

			bBlackText = true;
		}

		if(bBlackText)
			g_Draw.DrawStringA(g_Draw.m_ListItemFont,false,x + 5,y + iDrawIndex * ELEMENT_HEIGHT + 1,0,0,0,255,liItem.szName);
		else
			g_Draw.DrawStringA(g_Draw.m_ListItemFont,false,x + 5,y + iDrawIndex * ELEMENT_HEIGHT + 1,LISTITEM_TEXTCOLOR,liItem.szName);

		//we displayed the current item with the index (i), so lets try and display the next item
		iDisplayed++;
	}

	//is this list in focus aka active
	if(g_Mouse.IsOver(x,y,w,h))
	{
		if(!IsFocused())
			SetFocus(true);
	}
	else
	{
		if(!IsOnlyListOnPage())
			bScrollingNeeded = false;//why would we scroll if we list aint active + this isnt only list on menu page

		if(IsFocused())
			SetFocus(false);
	}

	if(bScrollingNeeded)
	{
		int iHowMany = iNumOfItems - iDisplayPerPage;
		int iScrollBar = (iHowMany * 3.5);
		int iWhen = 0;

		if(iScrollBar > (h - 20))
		{
			iScrollBar = h - 20;

			for(int nab = 0; ; nab++)
			{
				int iNew = (nab * 3.5);

				if(iNew > (h - 20))
				{
					iWhen = nab;
					break;
				}
			}
		}

		int iDiff = 0;
		
		if(iWhen > 0)
			iDiff = abs(iHowMany - iWhen) * 3.5;

		int iScrollBarHai = h + 3.5 - iScrollBar;
		int iScrollYAdd = iTopIndex * 3.5 * ((iWhen > 0) ? ((3.5 / iDiff) * 10 + 0.15) : 1);
		int iScrollYPos = y + iScrollYAdd;
		int iSavedScrollYPos = iScrollYPos;
		int iDummy;

		g_Mouse.Drag(g_Mouse.bDragged[1],true/*!g_Radar.IsHandlingItem()*/,
			g_Mouse.LeftClick(x+w+2,y,8,h),iDummy,iScrollYPos,g_Mouse.iDiffX[1],g_Mouse.iDiffY[1]);

		if(g_Mouse.bDragged[1] && !g_Menu.IsHandlingItem())//make sure we disable menu dragging
			g_Menu.AddMenuFlag(FL_DISABLEDRAG);

		if(iScrollYPos != iSavedScrollYPos)//slider was dragged
		{
			int iNewTopIndex = (iScrollYPos - y);
			iNewTopIndex /= 3.5;
			iNewTopIndex /= ((iWhen > 0) ? ((3.5 / iDiff) * 10 + 0.15) : 1);

			//min
			if(iNewTopIndex < 0)
				iNewTopIndex = 0;

			//max
			int iLastPossibleTopIndex = abs(iNumOfItems - iDisplayPerPage) - 1;

			if(iNewTopIndex > iLastPossibleTopIndex)
				iNewTopIndex = iLastPossibleTopIndex;
			//we clamped our new top index now we just gotta clamp our y pos
			iScrollYPos = y + (iNewTopIndex * 3.5 * ((iWhen > 0) ? ((3.5 / iDiff) * 10 + 0.15) : 1));

			//set the top index of the list
			iTopIndex = iNewTopIndex;
		}

		g_Draw.FillRGBA(x+w+2,iScrollYPos,8,iScrollBarHai,COLOR2);
	}
	else
		g_Draw.FillRGBA(x+w+2,y,8,h,COLOR2);
}

//SketchFlow Print Normal
void cMenu::DrawMenu()
{
	g_Draw.FillRGBA(m_x + 1, m_y + 2, m_w - 2, m_h - 2, 28, 28, 28, 255); //Main Menu
	g_Draw.FillRGBA(m_x + 1, m_y + 2, m_w - 466, m_h - 2, 24, 24, 24, 255); //Tab Left Menu
	g_Draw.FillRGBA(m_x + 1, m_y + 2, m_w - 2, m_h - 347, 22, 22, 22, 255);  //Tab Up Menu 
	g_Draw.DrawStringA(g_Draw.NameCheatFont, false, m_x + 15, m_y + 8.5, 255,255,255,255, "Evvware");
	g_Draw.DrawStringA(g_Draw.NameCheatFont, false, m_x + 490, m_y + 8.5, 31, 31, 31, 255, " Alpha v1.0"); //standard position x: m_x + 540, alpha user position x: m_x + 490

	DrawTab(AIMBOT, iTabIndex, m_x - 35, m_y + 71, "Aimbot");
	DrawTab(VISUALS, iTabIndex, m_x - 35, m_y + 107, "Visuals");
	DrawTab(ANTIAIMS, iTabIndex, m_x - 35, m_y + 143, "AntiAim");
	DrawTab(MISC, iTabIndex, m_x - 35, m_y + 179, "Misc");

	InitMenuElements();
}

void cMenuSection::Draw(int x, int y, int w, int h)
{
	g_Draw.FillRGBA(x,y,10,1, 255, 255, 255, 0);
	g_Draw.FillRGBA(x,y,1,h, 255, 255, 255, 0);
	g_Draw.FillRGBA(x,y+h,w,1, 255, 255, 255, 0);
	g_Draw.FillRGBA(x+w,y,1,h+1, 255, 255, 255, 0);

	SetSectionPos(x,y);
}

int iRed,iGreen,iBlue;
void GradientVertical(int x, int y, int w, int h, int r, int g, int b, int a,int r2, int g2, int b2)
{
	y += 1;

	for(int i = 1; i < h; i++)
	{
		iRed = (int)((float)i/h*(r - r2));
		iGreen = (int)((float)i/h*(g - g2));
		iBlue = (int)((float)i/h*(b - b2));
		g_Draw.FillRGBA(x,y+i-1,w,1,r-iRed,g-iGreen,b-iBlue,a);
	}
}

void GradientHorizontal(int x, int y, int w, int h, int r, int g, int b, int a, int r2, int g2, int b2)
{
	for(int i=1; i < w; i++)
	{
		iRed = (int)((float)i/w*(r - r2));
		iGreen = (int)((float)i/w*(g - g2));
		iBlue = (int)((float)i/w*(b - b2));
		g_Draw.FillRGBA(x+i-1,y,1,h,r-iRed,g-iGreen,b-iBlue,a);
	}
}

void cMenuSection::DrawAllDropDowns()
{
	int iNumOfDropDowns = vecDropDowns.size();

	int iWhich = -1;

	//AFTERCLICK FIX
	for(int i2 = (iNumOfDropDowns - 1); i2 >= 0; i2--)
	{
		bool bIsActive = bActiveDropDown[i2];

		if(bIsActive)
		{
			iWhich = i2;

			dwWait = GetTickCount() + 700;
			break;
		}
	}

	for(int i = (iNumOfDropDowns - 1); i >= 0; i--)
	{
		int e_x = vecDropDowns[i].x;
		int e_y = vecDropDowns[i].y;
		int iCount = vecDropDowns[i].iCount;

		bool bIsActive = bActiveDropDown[i];

		char **ppszNames = vecDropDowns[i].ppszNames;
		float *fValues  = vecDropDowns[i].fValues;
		float *cvar = vecDropDowns[i].cvar;

		if(bIsActive)
		{
			int string_x = e_x + (DROPDOWN_WIDTH * 0.5);

			for(int ax = 0; ax < iCount; ax++)
			{
				if(g_Mouse.IsOver(e_x,e_y + (ax * DROPDOWN_HEIGHT),DROPDOWN_WIDTH,DROPDOWN_HEIGHT))
				{
					if(!g_Menu.IsHandlingItem())
						g_Menu.AddMenuFlag(FL_DISABLEDRAG);

					g_Draw.FillRGBA(e_x + 1,e_y + (ax * DROPDOWN_HEIGHT),DROPDOWN_WIDTH - 1,DROPDOWN_HEIGHT - (ax == (iCount - 1) ? 1 : 0), 69, 26, 190, 255);
					
					if(g_Mouse.HasMouseOneJustBeenReleased())
					{
						*cvar = fValues[ax];

						bActiveDropDown[i] = false;
					}
				}
				else
				{
					g_Draw.FillRGBA(e_x + 1,e_y + (ax * DROPDOWN_HEIGHT),DROPDOWN_WIDTH - 1,DROPDOWN_HEIGHT - (ax == (iCount - 1) ? 1 : 0), 33, 33, 33, 255);

					if(g_Mouse.HasMouseOneJustBeenReleased())
					{
						bActiveDropDown[i] = false;
					}
				}

				//strings here
				if (*cvar == fValues[ax])
				{
					g_Draw.DrawStringA(g_Draw.TabPanelFont, true, string_x, e_y + (ax * DROPDOWN_HEIGHT) + 3, LISTITEM_TEXTCOLOR, ppszNames[ax]);
				}
				else
				{
					g_Draw.DrawStringA(g_Draw.TabPanelFont, true, string_x, e_y + (ax * DROPDOWN_HEIGHT) + 3, LISTITEM_TEXTCOLOR, ppszNames[ax]);
				}
			}
		}
		else//drop down not active
		{
			if(g_Mouse.IsOver(e_x,e_y,DROPDOWN_WIDTH,DROPDOWN_HEIGHT))
			{
				if(!g_Menu.IsHandlingItem())
					g_Menu.AddMenuFlag(FL_DISABLEDRAG);

				g_Draw.FillRGBA(e_x + 1,e_y,DROPDOWN_WIDTH,DROPDOWN_HEIGHT, 36, 36, 36, 255);

				if(g_Mouse.HasMouseOneJustBeenReleased() && (dwWait < GetTickCount() || iWhich == -1))
				{
					bActiveDropDown[i] = true;
				}
			}
			else
				g_Draw.FillRGBA(e_x + 1,e_y,DROPDOWN_WIDTH,DROPDOWN_HEIGHT, 33, 33, 33, 255);

			int string_x = e_x + (DROPDOWN_WIDTH * 0.5);
			int string_y = e_y;

			for(int ax = 0; ax < iCount; ax++)
			{
				if(*cvar == fValues[ax])//we display only the cvar matching the value in the list
				{
					g_Draw.DrawStringA(g_Draw.TabPanelFont,true,string_x,string_y + 3,LISTITEM_TEXTCOLOR,ppszNames[ax]);
					break;
				}
			}
		}
	}

	ClearAllElementSpecifics();
}

void cMenuSection::AddElement(int iType, int add_to_x, const char *pszElementName, const char *pszComment, float *cvar, float min, float max, float step, float fToValue, int increment)
{
	int e_x = s_x + 10 + add_to_x;
	int e_y = s_y + 15 + (iSection * ELEMENT_SEPERATION);

	bool bDropDownActive = false;

	for(int i2 = 0; i2 < 50; i2++)
	{
		bDropDownActive = bActiveDropDown[i2];

		if(bDropDownActive)
			break;
	}

	if(iType == ONOFF)
	{
		g_Draw.DrawStringA(g_Draw.Testing,false,e_x,e_y,LISTITEM_TEXTCOLOR,pszElementName);

		e_x += CHECKBOX_SEPERATION_FROM_TEXT + 10;

		if(g_Mouse.OneLeftClick(e_x + 18,e_y - 2,CHECKBOX_SIZE + 5,CHECKBOX_SIZE + 5) && !bDropDownActive)
			*cvar = !*cvar;

		g_Draw.FillRGBA(e_x + 18,e_y,CHECKBOX_SIZE,CHECKBOX_SIZE, 33, 33, 33, 255);

		if(*cvar)
			g_Draw.FillRGBA(e_x + 20, e_y + 2, 13, 13, 69, 26, 190, 255);

		if(g_Mouse.IsOver(e_x + 18,e_y,CHECKBOX_SIZE,CHECKBOX_SIZE))
		{	
			if(!g_Menu.IsHandlingItem())
				g_Menu.AddMenuFlag(FL_DISABLEDRAG);
		}
	}
	else if(iType == DROPDOWN)
	{
		e_y += 7;
		g_Draw.DrawStringA(g_Draw.Testing,false,e_x,e_y + 5,LISTITEM_TEXTCOLOR,pszElementName);

		DropDown_t ddNew;

		ddNew.x = e_x + CHECKBOX_SEPERATION_FROM_TEXT - 20;
		ddNew.y = e_y + 5;
		ddNew.fValues = fValueList;
		ddNew.cvar = cvar;
		ddNew.ppszNames = ppszValueNames;
		ddNew.iCount = iValueCount;

		AddDropDownToDrawList(ddNew);
	}
	else if(iType == SLIDER)
	{
		e_y += 3;

		int MaxFloat = max + 14;
		float Scale = max/MaxFloat;

		bool bOver = false;

		e_y += 8;

		if(bColored)
		{
			g_Draw.FillRGBA(e_x,e_y+4,fLength+2,5,fColDummy[0],fColDummy[1],fColDummy[2],fColDummy[3]);	
			g_Draw.FillRGBA(e_x - 7, e_y + 9, 124, 18, 33, 33, 33, 255);
		}
		else
		{
			g_Draw.FillRGBA(e_x - 7, e_y + 9, 124, 18, 33, 33, 33, 255);
		}

		int mouse_x, mouse_y;
		g_Mouse.GetMousePosition(mouse_x,mouse_y);

		if (mouse_x >= (e_x - 14) && mouse_x <= (e_x + fLength + 12) && mouse_y >= (e_y) && mouse_y <= (e_y + 24) && GetAsyncKeyState(VK_LBUTTON) && (iSliderIndex == -1) && dwWait < GetTickCount() && !bSliderFix[iCurrSlider])
		{
			iSliderIndex = iCurrSlider;
			bSliderFix[iCurrSlider] = true;

			if (!g_Menu.IsHandlingItem())
				g_Menu.AddMenuFlag(FL_DISABLEDRAG);
		}

		if (mouse_x >= (e_x - 102) && mouse_x <= (e_x + fLength + 114) && mouse_y >= (e_y - 1050) && mouse_y <= (e_y + 1050) && GetAsyncKeyState(VK_LBUTTON) && !bDropDownActive)
		{
			if (bSliderFix[iCurrSlider])
			{
				*cvar = min + (mouse_x - e_x) * Scale;

				bOver = true;

				if (!g_Menu.IsHandlingItem())
					g_Menu.AddMenuFlag(FL_DISABLEDRAG);
			}
		}
		else if(bSliderFix[iCurrSlider])
		{
			iSliderIndex = -1;
			bSliderFix[iCurrSlider] = false;
		}

		if(*cvar > max)
			*cvar = max;

		if(*cvar < min)
			*cvar = min;

		static int iSizeOf = 6;

		for(int nu = 0; nu <= iSizeOf; nu++)
		{
			g_Draw.FillRGBA(e_x+(*cvar/Scale)-iSizeOf+nu, e_y+iSizeOf+5, iSizeOf-4,14, 69, 26, 190, 255);
			g_Draw.FillRGBA(e_x+(*cvar/Scale)-iSizeOf+nu, e_y+iSizeOf+5, iSizeOf-4,14, 69, 26, 190, 255);
		}

		//if(g_Mouse.IsOver(e_x+(*cvar/Scale)-iSizeOf,e_y-1,10,14) || bOver)
			//g_Valve.pSurface->DrawSetColor(COLOR9);
		//else
			//g_Valve.pSurface->DrawSetColor(COLOR6);

		if(iColorIndex != -1)
		{
			Config.CvarList[iColorIndex] = fColDummy[0];
			Config.CvarList[iColorIndex + 1] = fColDummy[1];
			Config.CvarList[iColorIndex + 2] = fColDummy[2];
		}

		if (bShowValue)
		{
			char szCopy[256];
			char szCopy2[256];

			strcpy(szCopy2, pszElementName);
			sprintf(szCopy, "%2.f", *cvar);
			sprintf(szCopy2, "%s", pszElementName);

			g_Draw.FillRGBA(e_x + 124, e_y + 9, 34, 18, 33, 33, 33, 255);
			g_Draw.DrawStringA(g_Draw.m_ListItemFont, false, e_x + 129, e_y + 10, LISTITEM_TEXTCOLOR, szCopy);
			g_Draw.DrawStringA(g_Draw.Testing, false, e_x - 4, e_y - 8, LISTITEM_TEXTCOLOR, szCopy2);
		}
		else
			g_Draw.DrawStringA(g_Draw.Testing, false, e_x, e_y - 1, LISTITEM_TEXTCOLOR, pszElementName);
	}
	else if(iType == BUTTON)
	{
		bool bLeftClick = g_Mouse.LeftClick(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT);

		if(fToValue == -1)
		{
			if(*cvar)
				GradientVertical(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT, 233, 233, 233, 255,140,0,0);
			else
			{
				if(bLeftClick)
					GradientVertical(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT,COLOR9, 233, 233, 233);
				else
					GradientVertical(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT, 233, 233, 233, 255, 233, 233, 233);
			}
		}
		else
		{
			if(*cvar == fToValue)
				GradientVertical(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT,COLOR4,140,0,0);
			else
			{
				if(bLeftClick)
					GradientVertical(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT,COLOR9, 233, 233, 233);
				else
					GradientVertical(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT, 233, 233, 233, 255, 233, 233, 233);
			}
		}

		g_Draw.DrawStringA(g_Draw.m_ListItemFont,true,e_x + BUTTON_WIDTH * 0.5,e_y + 2,LISTITEM_TEXTCOLOR,pszElementName);

		if(g_Mouse.IsOver(e_x,e_y,BUTTON_WIDTH,BUTTON_HEIGHT))
		{

			if(g_Mouse.HasMouseOneJustBeenReleased() && !bDropDownActive)
			{
				if(fToValue == -1)
					*cvar = !*cvar;
				else
					*cvar = fToValue;
			}
		}
	}
	else if(iType == ADDER)
	{
		g_Draw.DrawStringA(g_Draw.Testing,false,e_x,e_y,LISTITEM_TEXTCOLOR,pszElementName);

		e_y += 1;

		e_x += (CHECKBOX_SEPERATION_FROM_TEXT - 20);

		g_Draw.FillRGBA(e_x,e_y,ADDER_SIZE,ADDER_SIZE, 233, 233, 233, 255);

		if(g_Mouse.IsOver(e_x,e_y,ADDER_SIZE,ADDER_SIZE))
		{

			if(g_Mouse.HasMouseOneJustBeenReleased() && !bDropDownActive)
				*cvar -= step;

			if(g_Mouse.IsMouseTwoBeingHeld() && !bDropDownActive)
				*cvar -= 20;
		}

		static std::string sLeft = "<";

		g_Draw.DrawStringA(g_Draw.Testing,true,e_x + ADDER_SIZE * 0.5,e_y - 3,COLOR10,sLeft.data());

		static std::string sValue = "%2.f";

		g_Draw.DrawStringA(g_Draw.Testing,true,e_x - 6 + ADDER_SIZE + ADDER_SEPERATE_FROM_BOXES * 0.5,e_y - 2,LISTITEM_TEXTCOLOR,sValue.data(),*cvar);

		e_x += ADDER_SEPERATE_FROM_BOXES;

		g_Draw.FillRGBA(e_x,e_y,ADDER_SIZE,ADDER_SIZE, 233, 233, 233,255);

		if(g_Mouse.IsOver(e_x,e_y,ADDER_SIZE,ADDER_SIZE))
		{

			if(g_Mouse.HasMouseOneJustBeenReleased() && !bDropDownActive)
				*cvar += step;

			if(g_Mouse.IsMouseTwoBeingHeld() && !bDropDownActive)
				*cvar += 20;
		}

		static std::string sRight = ">";

		g_Draw.DrawStringA(g_Draw.Testing,true,e_x + ADDER_SIZE * 0.5,e_y - 3,COLOR10,sRight.data());

		//clamp but go to min,max instead of not allowing to increase/decrease
		if(*cvar < min)
			*cvar = max;

		if(*cvar > max)
			*cvar = min;
	}

	PostElement((iType == SLIDER ? 2 : 1));
}