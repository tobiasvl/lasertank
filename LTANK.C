/*******************************************************
 **             LaserTank ver 4.0                     **
 **               By Jim Kindley                      **
 **               (c) 2001                            **
 **         The Program and Source is Public Domain   **
 *******************************************************
 **       Release version 2002 by Yves Maingoy        **
 **               ymaingoy@free.fr                    **
 *******************************************************/
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <mmsystem.h>
#include "ltank.h"
#include "ltank_d.h"
#include "lt_sfx.h"

const int GetNextBMArray[MaxObjects+1] = {0,1,2,3,4,5,6,8,9,10,7,12,13,14,11,16,17,18,15,19,21,22,23,20,24};
const int OpeningBMA[16] = {4,6,1,9,56,57,33,19,16,13,14,45,15,55,21,47};
int EditorOn   	= FALSE;		// true when in editor mode
int QHELP		= FALSE;		// True when Quick Help is On
extern const long DifCList[6];
HINSTANCE hInst;					// Globally Defined Instance
HBITMAP OpenScreen;		 			// Instruction Bitmap
HFONT MyFont;
int FileHand,RB_TOS,PBHold,VHSOn;
HWND MainH,Ed1,Ed2,BT1,BT2,BT3,BT4,BT5,BT6,BT7,BT8,BT9;
TPLAYFIELD ShiftPF,ShiftBMF;
HMENU EMenu,MMenu;
char PrintJobName[100];
char HelpFile[MAX_PATH];


extern int Backspace[10];			// Backspace Buffer
extern int BS_SP;					// StackPointer for Backspace
extern int OKtoSave;


OPENFILENAME OFN = {sizeof(OPENFILENAME),
					0,
					0,
					NULL,
					NULL,
					0,
					1,
					FileName,
					MAX_PATH,
					NULL,
					0,
					NULL,
					txt004,
					OFN_HIDEREADONLY,
					0,
					0,
					"lvl"};

OPENFILENAME PBfn = {sizeof(OPENFILENAME),
					0,
					0,
					NULL,
					NULL,
					0,
					1,
					PBFileName,
					MAX_PATH,
					NULL,
					0,
					NULL,
					txt005,
					OFN_HIDEREADONLY,
					0,
					0,
					"lpb"};

// Callback function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
void EditDiffSet( int );
BOOL LoadPlayback();
void VHSPlayback();

// Local Work
void EditDiffSet(int t)
{
  CheckMenuItem(EMenu,701,0);
  CheckMenuItem(EMenu,702,0);
  CheckMenuItem(EMenu,703,0);
  CheckMenuItem(EMenu,704,0);
  CheckMenuItem(EMenu,705,0);
  switch (t)
  {
    case 1: CheckMenuItem(EMenu,701,MF_CHECKED);
			break;
    case 2: CheckMenuItem(EMenu,702,MF_CHECKED);
			break;
    case 4: CheckMenuItem(EMenu,703,MF_CHECKED);
			break;
    case 8: CheckMenuItem(EMenu,704,MF_CHECKED);
			break;
    case 16: CheckMenuItem(EMenu,705,MF_CHECKED);
  }
  CurRecData.SDiff = t;
}

BOOL LoadPlayback()
{
	HANDLE F;
	char temps[200];

	if (Recording) SendMessage(MainH,WM_COMMAND,123,0); // Turn Off Recording
	if ((F = CreateFile(PBFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_FLAG_SEQUENTIAL_SCAN, NULL)) == INVALID_HANDLE_VALUE) return(FALSE);
	PBOpen = TRUE;		// this will also tell autorecord not to turn on
	ReadFile(F, &PBRec, sizeof(PBRec), &BytesMoved, NULL);
	if (RecBufSize <= PBRec.Size)
	{										// RecBuffer needs to be bigger
		RecBufSize = PBRec.Size+1;
		RecBuffer = GlobalReAlloc(RecBuffer,RecBufSize,GMEM_MOVEABLE);
	}
	ReadFile(F, RecBuffer, PBRec.Size, &BytesMoved, NULL);			// Load RecBuffer W data
	CloseHandle(F);
	CurLevel = PBRec.Level - 1;
	// this will error if the levels have moved
	if ((!LoadNextLevel(TRUE,TRUE)) || (strcmp(CurRecData.LName,PBRec.LName) != 0))
	{
		// Do a hard file search for the level name
		F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		CurLevel = 0;
		ReadFile(F, &CurRecData, sizeof(CurRecData), &BytesMoved, NULL);
		while ((BytesMoved == sizeof(CurRecData)) && (strcmp(CurRecData.LName,PBRec.LName) != 0))
		{
			CurLevel++;
			ReadFile(F, &CurRecData, sizeof(CurRecData), &BytesMoved, NULL);
		}
		CloseHandle(F1);
		if (BytesMoved == sizeof(CurRecData))
		{
			LoadNextLevel(TRUE,TRUE);
		}
	}
	Game.RecP = 0;
	RB_TOS = PBRec.Size;
	if (strcmp(CurRecData.LName,PBRec.LName) != 0)
	{
		strcpy(temps,txt013);
		strcat(temps,PBRec.LName);
		strcat(temps,txt015);
		strcat(temps,FileName); strcat(temps," >");
		MessageBox(MainH,temps,txt007,MB_OK | MB_ICONERROR);
		PlayBack = FALSE;
		PBOpen = FALSE;
		PBHold = FALSE;
		SendMessage(MainH,WM_COMMAND,101,0);
		return(FALSE);
	}
	return(TRUE);
}

void VHSPlayback()
{
	gDC = GetDC(MainH);
	GameOn(FALSE);
	if (FindTank)
	{
		FindTank = FALSE;
		PutLevel();
	}
	VHSOn = TRUE;
	while (Game.RecP < RB_TOS)
	{
		if (Game.Tank.Firing) MoveLaser();   	// Move laser if one was fired
		// Check Key Press }
		if (!(Game.Tank.Firing || ConvMoving || SlideO.s || SlideT.s ))
		{
			switch (RecBuffer[Game.RecP])
			{
			case VK_UP:
				MoveTank(1); // Move tank Up one
				break;
			case VK_RIGHT:
				MoveTank(2);
				break;
			case VK_DOWN:
				MoveTank(3);
				break;
			case VK_LEFT:
				MoveTank(4);
				break;
			case VK_SPACE:
				{
					UpdateUndo();
					Game.ScoreShot++;  // do here Not in FireLaser
					FireLaser(Game.Tank.X,Game.Tank.Y,Game.Tank.Dir,S_Fire); // Bang
				}
			}
			Game.RecP++;	// Point to next charecter
			AntiTank();  // give the Anti-Tanks a turn to play
		}
		if (SlideO.s) IceMoveO();
		if (SlideT.s) IceMoveT();
		ConvMoving = FALSE;    // used to disable Laser on the conveyor
		switch (Game.PF[Game.Tank.X][Game.Tank.Y])
		{
		case 2:
			if (Game_On)                   // Reached the Flag
			{
				GameOn(FALSE);
				ReleaseDC(gDC,MainH);
				VHSOn= FALSE;
				return;					// We shouldn't be here

			}
			break;
		case 3:
			PostMessage(MainH,WM_Dead,0,0);  // Water
			break;
		case 15:
			if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) // Conveyor Up
				ConvMoveTank(0,-1,TRUE);
			break;
		case 16:
			if (CheckLoc(Game.Tank.X+1,Game.Tank.Y))
				ConvMoveTank(1,0,TRUE);
			break;
		case 17:
			if (CheckLoc(Game.Tank.X,Game.Tank.Y+1))
				ConvMoveTank(0,1,TRUE);
			break;
		case 18:
			if (CheckLoc(Game.Tank.X-1,Game.Tank.Y))
				ConvMoveTank(-1,0,TRUE);
		}
	}
	ReleaseDC(gDC,MainH);
	UpDateTank();
	GameOn(TRUE);
	VHSOn= FALSE;
}

BOOL CALLBACK CancelProc( HDC hdc, int nCode)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(TRUE);
}

void Print()
{
	PRINTDLG PrintInfo;
	char temps[100];
	TEXTMETRIC TextInfo;
	int XLog,YLog,x,y,ch;
	HDC tempDC;
	HPALETTE hPal;
	LPLOGPALETTE lpLogPal;
	BITMAPINFO bmi;
	HBITMAP hbm;
	LPBYTE pBits;
	DIBSECTION ds;
	DOCINFO DocInfo = {sizeof(DOCINFO),
						0,
						0,
						0,
						0};

	PrintInfo.hwndOwner = MainH;
 	PrintInfo.hDevMode = 0;
 	PrintInfo.hDevNames = 0;
 	PrintInfo.lStructSize = sizeof(PrintInfo);
	PrintInfo.Flags =  PD_RETURNDC | PD_NOPAGENUMS | PD_NOSELECTION | PD_USEDEVMODECOPIES;
	if (!PrintDlg(&PrintInfo)) return;

	UpdateWindow(MainH);		// We need to redraw it first
	gDC = GetDC(MainH);

	ZeroMemory(&bmi,sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 16* SpBm_Width;
	bmi.bmiHeader.biHeight = 16 * SpBm_Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	// Create intermediate drawing surface
	hbm = CreateDIBSection(gDC,&bmi,DIB_RGB_COLORS,&pBits,NULL,0);
	// Prep surface for drawing
	tempDC = CreateCompatibleDC(gDC);
	SelectObject(tempDC,hbm);

	if ((GetDeviceCaps(gDC, RASTERCAPS) & RC_PALETTE))	// Are we Palette Based ?
	{
		// YES we are - Get Palette
		lpLogPal = GlobalAlloc(GPTR, sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY));
		lpLogPal->palVersion = 0x300;
		lpLogPal->palNumEntries = 256;
		GetSystemPaletteEntries(gDC,0,256,(LPPALETTEENTRY)(lpLogPal->palPalEntry));
		hPal = CreatePalette(lpLogPal);
		GlobalFree(lpLogPal);

		// Apply the Palette to Game WIndow
		SelectPalette(gDC, hPal, FALSE);
		RealizePalette(gDC);
		// Apply the Palette to the tempDC
		SelectPalette(tempDC, hPal, FALSE);
		RealizePalette(tempDC);
		DeleteObject(hPal);
	}
	// Copy Bits to the memory DC
	BitBlt(tempDC,0,0,16 * SpBm_Width, 16 * SpBm_Height, gDC,XOffset,YOffset,SRCCOPY);
	ReleaseDC(MainH,gDC);

	EnableWindow(MainH,FALSE);
	sprintf(PrintJobName,"%s %d",txt036,CurLevel);
	DocInfo.lpszDocName = PrintJobName;
	if (!StartDoc(PrintInfo.hDC,&DocInfo)) return;

	SetAbortProc(PrintInfo.hDC,CancelProc);
	StartPage(PrintInfo.hDC);
	GetTextMetrics(PrintInfo.hDC,&TextInfo);
	ch = TextInfo.tmHeight;
	XLog = GetDeviceCaps(PrintInfo.hDC,LOGPIXELSX);		// pixles per inch
	YLog = GetDeviceCaps(PrintInfo.hDC,LOGPIXELSY);

		// Now we can start to print
	SetBkMode(PrintInfo.hDC,TRANSPARENT);
	SetTextAlign(PrintInfo.hDC,TA_CENTER);
	TextOut(PrintInfo.hDC,(XLog * 3),(YLog / 2),PrintJobName,strlen(PrintJobName));
	if (CurLevel)		// This is if we print the opening Screen
	{
		sprintf(temps,"( %s )",CurRecData.LName);
		TextOut(PrintInfo.hDC,(XLog * 3),(YLog / 2)+ch,temps,strlen(temps));
	}
	y = (YLog * 5)+ch;
	SetTextAlign(PrintInfo.hDC,TA_LEFT);
	sprintf(temps,"Moves = %d    Shots = %d",Game.ScoreMove,Game.ScoreShot);
	TextOut(PrintInfo.hDC,XLog,y,temps,strlen(temps));

	GetObject(hbm,sizeof(DIBSECTION),&ds);
	StretchDIBits(PrintInfo.hDC,XLog,YLog,XLog * 4,YLog * 4,
					0,0,(SpBm_Width * 16),(SpBm_Height * 16),
					ds.dsBm.bmBits,(LPBITMAPINFO)&ds.dsBmih,DIB_RGB_COLORS,SRCCOPY);

	EndPage(PrintInfo.hDC);					// Send Page
	EndDoc(PrintInfo.hDC);
	DeleteDC(tempDC);
	DeleteDC(PrintInfo.hDC);
	DeleteObject(hbm);
	EnableWindow(MainH,TRUE);
}


void ToggleOpt( int ID, HMENU Menu, int *Opt, LPCTSTR psText)
{
	if (GetMenuState(Menu,ID,0) & MF_CHECKED)
	{
		CheckMenuItem(Menu,ID,0);
		*Opt = FALSE;
		WritePrivateProfileString("OPT",psText,"No",INIFile);
	}
	else {
		CheckMenuItem(Menu,ID,MF_CHECKED);
		*Opt = TRUE;
		WritePrivateProfileString("OPT",psText,"Yes",INIFile);
	}
}

void BuildPB_Name ()
{
	char temps[100];
	char *P;

	PBSRec.Level = CurLevel;
	sprintf(temps,"_%04d.lpb",PBSRec.Level);
	P = strrchr(PBFileName,'_');
	if (P) strcpy(P,temps);			// add to name
}

// --- Main Window Procedure --- }
// This is used for both the editor and the game }
// function MainWindowProc(Window: HWnd; Message, WParam: Word; LParam: Longint): Longint; export;
LRESULT CALLBACK WndProc(HWND Window, UINT Message, WPARAM wparam, LPARAM lparam)
{
	char temps[100];
	HDC tDC,pdc;
	HBITMAP tBM;
	RECT Box;
	HMENU Menu;
	PAINTSTRUCT PI;
	int i,j,x,y,LastLevel;
	char *P;
	WINDOWPLACEMENT twp;

	switch (Message)
	{
	case WM_CREATE:
		strcpy(FileName,LevelData);    // set up default file name
		Menu = GetMenu(Window);
		GetPrivateProfileString("OPT",psAni,psYes,temps,5,INIFile);
		// Check if Animation is enabled or disabled }
		if (strcmp(temps,psYes))
		{
			Ani_On = FALSE;
			CheckMenuItem(Menu,104,0);	// Yes I Know we should write MF_BYCOMMAND | MF_UNCHECKED
		}
		GetPrivateProfileString("OPT",psSound,psYes,temps,5,INIFile);
		// Check if Sound is enabled or disabled
		if (strcmp(temps,psYes))
		{
			Sound_On = FALSE;
			CheckMenuItem(Menu,102,0);
		}
		GetPrivateProfileString("OPT",psSCL,"No",temps,5,INIFile);
		// Check if Skip Complete Level is enabled or disabled
		if (strcmp(temps,psYes) == 0)
		{
			CheckMenuItem(Menu,116,MF_CHECKED);
			SkipCL = TRUE;
		}
		GetPrivateProfileString("OPT",psARec,"No",temps,5,INIFile);
		// Check if AutoRecord is enabled or disabled
		if (strcmp(temps,psYes) == 0)
		{
			CheckMenuItem(Menu,115,MF_CHECKED);
			ARecord = TRUE;
		}
		GetPrivateProfileString("OPT",psDW,"No",temps,5,INIFile);
		// Check if DisableWarnings is enabled or disabled
		if (strcmp(temps,psYes) == 0)
		{
			CheckMenuItem(Menu,127,MF_CHECKED);
			DWarn = TRUE;
		}
		Difficulty = GetPrivateProfileInt("DATA",psDiff,0,INIFile);
		GetPrivateProfileString("OPT",psRLLOn,psYes,temps,5,INIFile);
		// Check if Remember Last Level is enabled or disabled
		if (strcmp(temps,psYes))
		{
			RLL = FALSE;
			CheckMenuItem(Menu,109,0);
		}
		else GetPrivateProfileString("DATA",psRLLN,LevelData,FileName,100,INIFile);
		PBSRec.Author[0] = (char)0;
		LaserColorG = CreateSolidBrush(0x0000FF00);
		LaserColorR = CreateSolidBrush(0x000000FF);
		InitBuffers();
		SFxInit();
		PBHold = FALSE;							// used by playback to hold charecters
		VHSOn = FALSE;
		AssignHSFile();
		break;
	case WM_PAINT:
		pdc = BeginPaint(Window,&PI);
		gDC = pdc;							// we use gDC for most graphics stuff
		SelectObject(gDC,MyFont);
		GetClientRect(Window,&Box);
		// draw 3D frames }
		JK3dFrame(pdc,XOffset-1,YOffset-1,(SpBm_Width*16)+XOffset,(SpBm_Height*16)+YOffset,FALSE);
		JK3dFrame(pdc,XOffset-2,YOffset-2,(SpBm_Width*16)+XOffset+1,(SpBm_Height*16)+YOffset+1,FALSE);
		JK3dFrame(pdc,1,1,ContXPos-5,Box.bottom-2,TRUE);
		JK3dFrame(pdc,ContXPos-1,1,ContXPos+181,Box.bottom-2,TRUE);
		if (!EditorOn) JK3dFrame(pdc,ContXPos+10,250,ContXPos+165,405,FALSE);
		tDC = CreateCompatibleDC(pdc);
		OpenScreen = LoadImageFile(hInst,"CONTROLBM",CONTROL_BMP);
		tBM = SelectObject(tDC,OpenScreen);
		// put up control bitmap }
		BitBlt (pdc,ContXPos,2,180,245,tDC,0,0,SRCCOPY);
		SelectObject (tDC,tBM);
		DeleteObject(OpenScreen);
		SetBkMode(pdc,TRANSPARENT);
		SetTextAlign(pdc,TA_CENTER);
		SetTextColor(pdc,0x00808080);
		if ((CurLevel == 0) || QHELP )
		{
			// come here in the beggining before a level is loaded
			OpenScreen = LoadImageFile(hInst,"OPENING", OPENING_BMP);
			tBM = SelectObject(tDC,OpenScreen);
			StretchBlt (gDC,XOffset,YOffset,SpBm_Width*16,SpBm_Height*16,tDC,0,0,384,384,SRCCOPY);
			SelectObject (tDC,tBM);
			DeleteObject(OpenScreen);
			x = XOffset+3;
			y = YOffset + (SpBm_Height*8);
			j = 1;
			for (i=0;i<16;i++)
			{
				PutSprite(OpeningBMA[i],x,y);
   			 	x += (SpBm_Width*4);
				j++;
				if (j > 4)
				{
					x = XOffset+3; y += (SpBm_Height*2); j = 1;
  			   	}
			}
			// desactive  2004/05/09 - mgy
			// TextOut(pdc,(SpBm_Width*13),(SpBm_Height*16),App_Version,strlen(App_Version));
		}
		else {
			// Lable Game Grid
			x = SpBm_Width / 2;
			y = (SpBm_Height-15) /2;
			for (i=1; i<17; i++)
			{
				TextOut(pdc,8,YOffset+y+((i-1) * SpBm_Height),itoa(i,temps,10),strlen(temps));
				if ( i<10 )
				{
					TextOut(pdc,8+XOffset+(16*SpBm_Width) ,YOffset+y+((i-1) * SpBm_Height),itoa(i,temps,10),strlen(temps));
				}
				else
				{
					strcpy(temps, "1 ");
					TextOut(pdc,-1+8+XOffset+(16*SpBm_Width) ,YOffset+y+((i-1) * SpBm_Height),temps, strlen(temps));
					itoa(i-10,temps,10);
					TextOut(pdc,3+8+XOffset+(16*SpBm_Width) ,YOffset+y+((i-1) * SpBm_Height),temps,strlen(temps));
				}


				strcpy(temps,"@"); temps[0] = temps[0] + i;
				TextOut(pdc,XOffset+x+((i-1) * SpBm_Width),1,temps,strlen(temps));
				TextOut(pdc,XOffset+x+((i-1) * SpBm_Width),YOffset+1+(16 * SpBm_Height),temps,strlen(temps));

			}
			PutLevel();
			if (EditorOn)
			{
				PutSelectors();
				ShowTunnelID();
			}
			else {
				SetTextColor(pdc,DifCList[0]);
				itoa(CurLevel,temps,10);
				switch (CurRecData.SDiff)
				{
				case 1:
					strcat(temps,txt023);
					SetTextColor(pdc,DifCList[1]);
					break;
				case 2:
					strcat(temps,txt024);
					SetTextColor(pdc,DifCList[2]);
					break;
				case 4:
					strcat(temps,txt025);
					SetTextColor(pdc,DifCList[3]);
					break;
				case 8:
					strcat(temps,txt026);
					SetTextColor(pdc,DifCList[4]);
					break;
				case 16:
					strcat(temps,txt027);
					SetTextColor(pdc,DifCList[5]);
					break;
				}
				TextOut(pdc,ContXPos+91,43,temps,strlen(temps));
				TextOut(pdc,ContXPos+91,100,CurRecData.LName,strlen(CurRecData.LName));
				TextOut(pdc,ContXPos+91,150,CurRecData.Author,strlen(CurRecData.Author));
				SetTextColor(pdc,0x0000FF00);
				TextOut(pdc,ContXPos+48,207,itoa(Game.ScoreMove,temps,10),strlen(temps));
				TextOut(pdc,ContXPos+134,207,itoa(Game.ScoreShot,temps,10),strlen(temps));
			}
		}
		DeleteDC(tDC);
		EndPaint(Window,&PI);
		return(0);
	case WM_KEYDOWN:
		if (!EditorOn)
		{
			if ((wparam < 32) || (wparam > 40)) return(0);
			if ((RB_TOS > Game.RecP) && (lparam & 0x40000000)) return(0);
			AddKBuff(wparam);
			return(0);
		}
		break;
	case WM_TIMER:
		if (QHELP) return(0);
		gDC = GetDC(Window);
		SelectObject(gDC,MyFont);
		if (FindTank)
			{
				FindTank = FALSE;
				PutLevel();
				SetTimer(MainH,1,GameDelay,NULL);
			}
		if (Ani_On) AniCount++;
		if (AniCount == ani_delay) Animate(); 	// Do Animation
		if (Game.Tank.Firing)
			MoveLaser();   	// Move laser if one was fired

		if (PBOpen)
		{
			if (Speed == 2)
			{
				SlowPB++;
				if (SlowPB == SlowPBSet) SlowPB = 1;
			}
			if (PlayBack && (!( ConvMoving || SlideO.s || SlideT.s))
				&& ((Speed != 2) || ((Speed == 2) && (SlowPB == 1))))
			{
				PBHold = FALSE;
				itoa(Game.RecP,temps,10);
				SendMessage(PBCountH,WM_SETTEXT,0,(long)(temps));
				if (Speed == 3) SendMessage(PlayH,WM_COMMAND,ID_PLAYBOX_02,0);
			}
			else PBHold = TRUE;
		}
		// Check Key Press }

		if ((Game.RecP < RB_TOS) && // (speedBug) &&
			(!(Game.Tank.Firing || ConvMoving || SlideO.s || SlideT.s || PBHold)))
		{
			switch (RecBuffer[Game.RecP])
			{
			case VK_UP:
				MoveTank(1); // Move tank Up one
				break;
			case VK_RIGHT:
				MoveTank(2);
				break;
			case VK_DOWN:
				MoveTank(3);
				break;
			case VK_LEFT:
				MoveTank(4);
				break;
			case VK_SPACE:
				{
					UpdateUndo();
					Game.ScoreShot++;  // do here Not in FireLaser
					FireLaser(Game.Tank.X,Game.Tank.Y,Game.Tank.Dir,S_Fire); // Bang
				}
			}
			Game.RecP++;	// Point to next charecter
			AntiTank();  // give the Anti-Tanks a turn to play
		}
		if (SlideO.s) IceMoveO();
		if (SlideT.s) IceMoveT();
		if (TankDirty) UpDateTank();	// I know we do this again later.
		ConvMoving = FALSE;    // used to disable Laser on the conveyor
		switch (Game.PF[Game.Tank.X][Game.Tank.Y])
		{
		case 2:
			if (Game_On)                   // Reached the Flag
			{
				GameOn(FALSE);
				SoundPlay(S_EndLev);
				if (!PBOpen)
				{
					if (Recording) SendMessage( Window,WM_SaveRec,0,0);
					CheckHighScore();
					LoadNextLevel(FALSE,FALSE);
				}
			}
			break;
		case 3:
			PostMessage(Window,WM_Dead,0,0);  // Water
			break;
		case 15:
			if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) // Conveyor Up
				ConvMoveTank(0,-1,TRUE);
			break;
		case 16:
			if (CheckLoc(Game.Tank.X+1,Game.Tank.Y))
				ConvMoveTank(1,0,TRUE);
			break;
		case 17:
			if (CheckLoc(Game.Tank.X,Game.Tank.Y+1))
				ConvMoveTank(0,1,TRUE);
			break;
		case 18:
			if (CheckLoc(Game.Tank.X-1,Game.Tank.Y))
			ConvMoveTank(-1,0,TRUE);
		}

		// Check the mouse Buffer
		if ((Game.RecP == RB_TOS) && (MB_TOS != MB_SP) &&
			(!(Game.Tank.Firing || ConvMoving || SlideO.s || SlideT.s)))
		{
			if (MouseOperation(MB_SP))				// Turn Mouse Operation into KB chars
			{
				MB_SP++;
				if (MB_SP == MaxMBuffer) MB_SP = 0;
			} else {
				MB_SP = MB_TOS;						// error so clear the rest
				MessageBeep(0);
			}
		}
		if (TankDirty) UpDateTank();
		ReleaseDC(Window,gDC);
		return(0);
	case WM_GameOver:
		DialogBox(hInst, "WinBox", Window, (DLGPROC)WinBox);
		return(0);

	case WM_NewHS:
		DialogBox(hInst, "HighBox", Window, (DLGPROC)HSBox);
		return(0);
	case WM_SaveRec:
		if (PBSRec.Author[0] == (char)0)
			DialogBox(hInst, "RecBox", Window, (DLGPROC)RecordBox);
     	PBfn.lpstrTitle = txt006;
    	PBfn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		strcpy(PBSRec.LName,CurRecData.LName);
		BuildPB_Name();
		if (GetSaveFileName(&PBfn))
		{
			if (strchr(PBFileName,'.') == 0) strcat(PBFileName,".lpb");
			if (Game.RecP < RecMax) PBSRec.Size = Game.RecP;			// Set Playback size
			else PBSRec.Size = RecMax;								// WOW Big Level
            SavePBFile();
 		}
 		return(0);
	case WM_Dead:
		GameOn(FALSE);
		if (VHSOn)
		{
			RB_TOS = Game.RecP;
			return(0);
		}
		SoundPlay(S_Die);
		i = DialogBox(hInst, "DeadBox", Window, (DLGPROC)DeadBox);
		switch (i)
		{
		case ID_DEADBOX_UNDO:
			SendMessage(Window,WM_COMMAND,110,0);
			GameOn(TRUE);
			break;
		case ID_DEADBOX_RESTART:
		case 2: // Cancel
			UndoStep();		// We have to undo the error first ( incase we Restart Undo)
			PostMessage(Window,WM_COMMAND,105,0);
		}
		return(0);
	case WM_CLOSE:
		if (GameInProg)
		{
			i = MessageBox(MainH,txt039,txt038,MB_YESNO | MB_ICONQUESTION);
			if (i == IDYES)	SendMessage(MainH,WM_SaveRec,0,0);
		}
		DestroyWindow(Window);
		return(0);
	case WM_DESTROY:
		if (Game_On) GameOn(FALSE); // Kill Timer
		GFXKill();
		KillBuffers();
		DeleteObject(LaserColorR);
		DeleteObject(LaserColorG);
		WinHelp(Window,HelpFile,HELP_QUIT,0);
 		PostQuitMessage(0);
		return(0);
	case WM_MOVE:
		twp.length = sizeof(twp);
		GetWindowPlacement(Window,&twp);
		if (twp.showCmd != SW_SHOWNORMAL) break;
		itoa(twp.rcNormalPosition.top,temps,10);
		WritePrivateProfileString("SCREEN",psYpos,temps,INIFile);
		itoa(twp.rcNormalPosition.left,temps,10);
		WritePrivateProfileString("SCREEN",psXpos,temps,INIFile);
		break;
	case WM_MOUSEMOVE:
		if (EditorOn)  // if we are editing then check mouse
		{
			if ((!(wparam & (MK_RBUTTON | MK_LBUTTON)))
				|| (wparam & MK_SHIFT)) return(0); // Mouse Not Down or Shift pressed

			y = ((HIWORD(lparam) - XOffset) / SpBm_Height);
			x = ((LOWORD(lparam) - YOffset) / SpBm_Width);
			if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
			if (wparam == MK_LBUTTON)
			{
				if ((Game.PF[x][y] != CurSelBM_L) && (CurSelBM_L != MaxObjects))
					ChangeGO(x,y,CurSelBM_L);
			}
			else {
				if ((Game.PF[x][y] != CurSelBM_R) && (CurSelBM_R != MaxObjects))
				    ChangeGO(x,y,CurSelBM_R);
			}
			return(0);
		}
		break;
	case WM_LBUTTONDOWN:
		if (EditorOn)
		{
			SetFocus(Window);				// We need to get the focus off of the Edits
			if (LOWORD(lparam) > ContXPos)
			{
				// we are on the selector window area
				y = (HIWORD(lparam) - 250) / (SpBm_Height + 4);
				x = (LOWORD(lparam) - (ContXPos+5)) / (SpBm_Width + 4);
				i = x + (y*EditBMWidth);
				if ((i > MaxObjects+1) || (i < 0)) return(0);
				CurSelBM_L = i;
				gDC = GetDC(Window);
				PutSelectors();
				ReleaseDC(Window,gDC);
			}
			else {
				// we are in the Game window area - Edit Mode
				y = ((HIWORD(lparam) - XOffset) / SpBm_Height);
				x = ((LOWORD(lparam) - YOffset) / SpBm_Width);
				if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
				if ((wparam & MK_SHIFT) == MK_SHIFT) ChangeGO(x,y,GetNextBMArray[Game.PF[x][y]]); // Rotate Object
				else {
					if (Game.PF[x][y] != CurSelBM_L) ChangeGO(x,y,CurSelBM_L);
				}
				Modified = TRUE;
			}
			return(0);
		}
		else {
				y = ((HIWORD(lparam) - XOffset) / SpBm_Height);
				x = ((LOWORD(lparam) - YOffset) / SpBm_Width);
				if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
				MBuffer[MB_TOS].X = x;
				MBuffer[MB_TOS].Y = y;
				MBuffer[MB_TOS].Z = 1;	// 1 = mouse clicked
				MB_TOS++;
				if (MB_TOS == MaxMBuffer) MB_TOS = 0;
		}
		break;
	case WM_RBUTTONDOWN:
		if (EditorOn)
		{
			SetFocus(Window);				// We need to get the focus off of the Edits
			if (LOWORD(lparam) > ContXPos)
			{
				y = (HIWORD(lparam) - 250) / (SpBm_Height + 4);
				x = (LOWORD(lparam) - (ContXPos+5)) / (SpBm_Width + 4);
				i = x + (y*EditBMWidth);
				if ((i > MaxObjects+1) || (i < 0)) return(0);
				CurSelBM_R = i;
				gDC = GetDC(Window);
				PutSelectors();
				ReleaseDC(Window,gDC);
			}
			else {
				y = ((HIWORD(lparam) - XOffset) / SpBm_Height);
				x = ((LOWORD(lparam) - YOffset) / SpBm_Width);
				if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
				if (Game.PF[x][y] != CurSelBM_R) ChangeGO(x,y,CurSelBM_R);
				Modified = TRUE;
			}
			return(0);
		}
		else {
				y = ((HIWORD(lparam) - XOffset) / SpBm_Height);
				x = ((LOWORD(lparam) - YOffset) / SpBm_Width);
				if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
				MBuffer[MB_TOS].X = x;
				MBuffer[MB_TOS].Y = y;
				MBuffer[MB_TOS].Z = 2;	// 2 = mouse R clicked
 				MB_TOS++;
				if (MB_TOS == MaxMBuffer) MB_TOS = 0;
		}
		break;
	case WM_COMMAND:
		if (!EditorOn) SetFocus(Window);		// We need this for the buttons
		switch (LOWORD(wparam))
		{
		case 101:  // New Game

			LastLevel = CurLevel; CurLevel = 0;
			if (RLL)   // Remember Last Level
				{
				GetPrivateProfileString("DATA",psRLLL,"1",temps,5,INIFile);
				CurLevel = atoi(temps);
				CurLevel --;
			}
			if (!LoadNextLevel(TRUE,FALSE)) CurLevel = LastLevel;
			return(0);
		case 102: 		 // Toggle Sound Option
			ToggleOpt( 102, MMenu, &Sound_On, psSound);
			return(0);
		case 103:  // Exit }
			if (GameInProg)
			{
				i = MessageBox(MainH,txt039,txt038,MB_YESNO | MB_ICONQUESTION);
				if (i == IDYES)	SendMessage(MainH,WM_SaveRec,0,0);
			}
			DestroyWindow(Window);
			return(0);
		case 104: 		 // Toggle Animation Option
			ToggleOpt( 104, MMenu, &Ani_On, psAni);
			return(0);
		case 105:
			if (CurLevel > 0)		// ReStart
				{
				if (UndoP > 0 ) UpdateUndo();		// Without this we loose the last move
				memcpy(Game.PF,CurRecData.PF,sizeof(TPLAYFIELD)); // Game.PF = CurRecData.PF;
				BuildBMField();
				InvalidateRect(Window,NULL,FALSE);
				GameOn(TRUE);
				Game.RecP = 0;
				RB_TOS = 0;
				SlideO.s = 0;						// stop any sliding
				SlideMem.count = 0;					// MGY --- stop any sliding
				SlideT.s = 0;
				// Lets also init the Mouse Buffer
				MB_TOS = MB_SP = 0;
			}
			return(0);
		case 106:							 // Load Level
			x = Game_On;
			LastLevel = CurLevel;
			GameOn(FALSE);
			i = DialogBox(hInst, "LoadLev", Window, (DLGPROC)LoadBox);
			if (i > 100)
			{
				CurLevel = i - 101;
				if (!LoadNextLevel(TRUE,TRUE))
				{
					CurLevel = LastLevel;
					GameOn(x);
				}
			}
			else GameOn(x);
			return(0);
		case 107: 						 // Skip Level
			LoadNextLevel(FALSE,FALSE);
			return(0);
		case 108: 						 // Open Data File
			x = Game_On;
			GameOn(FALSE);
			OFN.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
			strcpy(temps,FileName);
			if (GetOpenFileName(&OFN))
			{
				AssignHSFile();
				CurLevel = 0;
				Backspace[BS_SP] = 0;	// Clear Backspace stack
				EnableMenuItem(MMenu,118,MF_BYCOMMAND | MF_GRAYED ); // Disable Menu Item
				LoadNextLevel(TRUE,FALSE);
			}
			else {
				GameOn(x);
				strcpy(FileName,temps);
			}
			return(0);
		case 109: 		 				// Toggle Remember Last Level Option
			ToggleOpt( 109, MMenu, &RLL, psRLLOn);
			return(0);
		case 110: 					 	// Undo
			UndoStep();
			if (UndoBuffer[UndoP].Tank.Dir == 0)
			{
				EnableMenuItem(MMenu,110,MF_GRAYED); // disable Undo
				EnableWindow(BT1,SW_HIDE);
			}
			InvalidateRect(Window,NULL,FALSE);
			return(0);
		case 111: 					  // Save Position
			SaveGame = Game;
			EnableMenuItem(MMenu,112,MF_ENABLED);
			EnableWindow(BT3,SW_SHOWNA);
			return(0);
		case 112: 					  // Restore Position
			Game = SaveGame;
			RB_TOS = Game.RecP;						// clear all keys Past this Pos
			MB_TOS = MB_SP = 0;						// we need to cancle all mouse inputs
			InvalidateRect(Window,NULL,FALSE);
			return(0);
		case 113:					 // High Score
				DialogBox(hInst, "HighList", Window, (DLGPROC)HSList);
			return(0);
		case 114:					 // PlayBack Recording
			BuildPB_Name();
			PBfn.lpstrTitle = txt020;
			PBfn.Flags= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
      if (GetOpenFileName(&PBfn))
			{
				if (LoadPlayback())	DialogBox(hInst, "PlayBox", Window, (DLGPROC)PBWindow);
			}
 			return(0);
		case 115: 		 				// Toggle AutoRecord Option
			if (GetMenuState(MMenu,115,0) & MF_CHECKED)
			{
				CheckMenuItem(MMenu,115,0);
				ARecord = FALSE;
				WritePrivateProfileString("OPT",psARec,"No",INIFile);
	      if (GetMenuState(MMenu,123,0) & MF_CHECKED)
					PostMessage(Window,WM_COMMAND,123,0); 	// Record Off
			}
			else {
				CheckMenuItem(MMenu,115,MF_CHECKED);
				ARecord = TRUE;
				WritePrivateProfileString("OPT",psARec,"Yes",INIFile);
	      if (!(GetMenuState(MMenu,123,0) & MF_CHECKED) )
					PostMessage(Window,WM_COMMAND,123,0); 	// Record On
				}
			return(0);
		case 116: 		 				// Toggle Skip Complete Levels Option
			ToggleOpt( 116, MMenu, &SkipCL, psSCL);
			return(0);
		case 117: if (Recording) PostMessage(Window,WM_SaveRec,0,0);	// F6 - Save Rec
			break;
		case 118:							// Backspace Level
			if (Backspace[BS_SP])
			{
				Backspace[BS_SP] = 0;					// this is so we dont loop around
				BS_SP--;
				if (BS_SP < 0) BS_SP = 9;
				if (Backspace[BS_SP] == 0) return(0);	// error
				i = BS_SP - 1;
				if (i < 0) i = 9;
				if (Backspace[i] == 0)
					EnableMenuItem(MMenu,118,MF_BYCOMMAND | MF_GRAYED ); // Disable Menu Item
				LastLevel = CurLevel;
				CurLevel = Backspace[BS_SP]-1;
				if (!LoadNextLevel(TRUE,FALSE))	CurLevel = LastLevel;
			}
			return(0);
		case 119:							// Previous Level
			LoadLastLevel();
			return(0);
		case 120: 							// small size
			SetGameSize(1);
			InvalidateRect(Window,NULL,TRUE);
			return(0);
		case 121: 							// medium size
			SetGameSize(2);
			InvalidateRect(Window,NULL,TRUE);
			return(0);
		case 122: 							// large size
			SetGameSize(3);
			InvalidateRect(Window,NULL,TRUE);
			return(0);
		case 123:
			if (GetMenuState(MMenu,123,MF_BYCOMMAND) && MF_CHECKED != 0)
			{
				CheckMenuItem(MMenu,123,0);
				Recording = FALSE;
        EnableMenuItem(MMenu,117,MF_GRAYED);
				SetWindowText(MainH,App_Title);
      }
			else
			{
				CheckMenuItem(MMenu,123,MF_CHECKED);
				Recording = TRUE;
				EnableMenuItem(MMenu,117,0 ); 				// enable Save Recording
				SetWindowText(MainH,REC_Title);
 			}
			return(0);
		case 124:
			if (CurLevel > 0)		// RePlay
				{
				memcpy(Game.PF,CurRecData.PF,sizeof(TPLAYFIELD)); // Game.PF = CurRecData.PF;
				BuildBMField();
				InvalidateRect(Window,NULL,FALSE);
				ResetUndoBuffer();					//We don't need the old data
				GameOn(TRUE);
				Game.RecP = 0;
			}
			return(0);
		case 125:					// Resume Recording
			PBfn.lpstrTitle = txt033;
			PBfn.Flags= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
      BuildPB_Name();
			if (GetOpenFileName(&PBfn))
			{
				x = LoadPlayback();
				PBOpen = FALSE;
				PBHold = FALSE;
				if (x)
				{
					UpdateWindow(Window);
					VHSPlayback();							// Playback File
					SendMessage(MainH,WM_COMMAND,123,0);	// Start Recording
				}
			}
			return(0);
		case 126:
				x = Game_On;
				GameOn(FALSE);
				Print();
				GameOn(x);
			return(0);
		case 127:
			ToggleOpt( 127, MMenu, &DWarn, psDW);
			return(0);
		case 201: 					 // Editor
			EditorOn = TRUE;
			GameOn(FALSE);           // Shut down game engine
			strcpy(temps,App_Title);
			strcat(temps,txt028);
			SetWindowText(Window,temps);  // Change window name
			if (Recording) SendMessage(MainH,WM_COMMAND,123,0);
   			OKtoHS = FALSE;
			SetMenu(Window,EMenu);        // Set up editor menu's
			ShowWindow(BT1, SW_HIDE);
			ShowWindow(BT2, SW_HIDE);
			ShowWindow(BT3, SW_HIDE);
			ShowWindow(BT4, SW_HIDE);
			ShowWindow(BT5, SW_HIDE);
			ShowWindow(BT6, SW_HIDE);
			ShowWindow(BT7, SW_HIDE);
			ShowWindow(BT8, SW_HIDE);
			ShowWindow(BT9, SW_HIDE);
			ShowWindow(Ed1, SW_SHOWNA);   // setup title window
			SetWindowText(Ed1,CurRecData.LName);
			SendMessage(Ed1,EM_SETMODIFY,0,0);
			ShowWindow(Ed2, SW_SHOWNA);   // setup Author window
			SendMessage(Ed2,EM_SETMODIFY,0,0);
			SetWindowText(Ed2,CurRecData.Author);
			EditDiffSet(CurRecData.SDiff);
			InvalidateRect(Window,NULL,TRUE);  // redraw in editor mode
			Modified = FALSE;
			// Strip off the Tunnel Indexers
			for (x=0; x < 16; x++) for (y=0; y < 16; y++)
				if (ISTunnel(x,y)) Game.PF[x][y] &= 0xFE;
			if (CurLevel == 0) PostMessage(Window,WM_COMMAND,601,0); // New Level
			return(0);
		case 225:					 // Difficulty
			x = Game_On;
			GameOn(FALSE);
			DialogBox(hInst, "DiffBox", Window, (DLGPROC)DiffBox);
			GameOn(x);
			return(0);
		case 226:								// Graphics Change
			DialogBox(hInst, "GRAPHBOX", Window, (DLGPROC)GraphBox);
			return(0);
		case 301:
			if (Game_On)						// Hint
			{
				GameOn(FALSE);
				if (strlen(CurRecData.Hint) > 0)
					MessageBox(Window,CurRecData.Hint,txt021,MB_OK | MB_ICONINFORMATION);
				else MessageBox(Window,txt035,txt021,MB_OK | MB_ICONHAND);
				GameOn(TRUE);
			}
			return(0);
		case 601: 							// New Level  [Editor]
			for (x=0; x<16; x++) for (y=0; y<16; y++)
			{
				Game.PF[x][y] = 0;
				Game.BMF[x][y] = 1;
				Game.BMF2[x][y] = 1;
				Game.PF2[x][y] = 0;
			}
			Game.Tank.X = 7;
			Game.Tank.Y = 15;
			Game.Tank.Dir = 1;
			Game.Tank.Firing = FALSE;
			SetWindowText(Ed1,"");
			SendMessage(Ed1,EM_SETMODIFY,0,0);
			SetWindowText(Ed2,"");
			SendMessage(Ed2,EM_SETMODIFY,0,0);
			CurRecData.Hint[0] = 0;       // Clear Out Hint
			InvalidateRect(Window,NULL,TRUE);
			CurLevel = 1;
			Modified = TRUE;
			OKtoSave = FALSE;
			return(0);
		case 602: 						   // Load Level [Editor]
			if (SendMessage(Ed1,EM_GETMODIFY,0,0) ||
					SendMessage(Ed2,EM_GETMODIFY,0,0)) Modified = TRUE;
			if (Modified) if (MessageBox(Window,txt022,txt029,MB_YESNO | MB_ICONQUESTION) == IDYES)
				SendMessage(Window,WM_COMMAND,603,0);
			strcpy(temps,FileName);  // Save old name in case we cancel
			OFN.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
			if (GetOpenFileName(&OFN))
			{
				i = DialogBox(hInst, "LoadLev", Window, (DLGPROC)LoadBox);
				if (i > 100)
				{
					i -= 101;
					if (!(F1 = CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
										FILE_FLAG_RANDOM_ACCESS,NULL))) return(0);
					SetFilePointer(F1,i * sizeof(TLEVEL),NULL,FILE_BEGIN);
					ReadFile(F1,&CurRecData,sizeof(TLEVEL),&BytesMoved,NULL);
					if ( BytesMoved == sizeof(TLEVEL))
					{
						SetWindowText(Ed1,CurRecData.LName);
						SendMessage(Ed1,EM_SETMODIFY,0,0);
						SetWindowText(Ed2,CurRecData.Author);
						SendMessage(Ed2,EM_SETMODIFY,0,0);
						CurLevel = i + 1;
						memcpy(Game.PF,CurRecData.PF,sizeof(TPLAYFIELD)); // Game.PF = CurRecData.PF;
						BuildBMField();
					}
					CloseHandle(F1);
					InvalidateRect(Window,NULL,TRUE);
					OKtoSave = TRUE;
				}
				else strcpy(FileName,temps);
			}
			return(0);
		case 603:   // Save Level [Editor] }
			if (OKtoSave)
			{
				F1 = CreateFile(FileName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
							FILE_FLAG_RANDOM_ACCESS,NULL);
				GetWindowText(Ed1,CurRecData.LName,30);
				GetWindowText(Ed2,CurRecData.Author,30);
				memcpy(CurRecData.PF,Game.PF,sizeof(TPLAYFIELD));
				CurRecData.PF[Game.Tank.X][Game.Tank.Y] = 1; // save Tank in File
				SetFilePointer(F1,((CurLevel-1) * sizeof(TLEVEL)),NULL,FILE_BEGIN);
				if (WriteFile(F1,&CurRecData,sizeof(TLEVEL),&BytesMoved,NULL))
				{
					Modified = FALSE;
					SendMessage(Ed1,EM_SETMODIFY,0,0);
					SendMessage(Ed2,EM_SETMODIFY,0,0);
				}
				else FileError();
				CloseHandle(F1);
				if (HSClear)
				{
					AssignHSFile();
					if (F2 = CreateFile(HFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,
										FILE_FLAG_RANDOM_ACCESS,NULL))
					{
						HS.moves = 0;
						SetFilePointer(F2,(CurLevel-1)* sizeof(THSREC),NULL,FILE_BEGIN);
						WriteFile(F2,&HS,sizeof(THSREC),&BytesMoved,NULL);
						CloseHandle(F2);
					}
				}
			}
			else SendMessage(MainH,WM_COMMAND,606,0);		// Save As if new Level
			return(0);
		case 606:				// Save As
			OFN.Flags = OFN_HIDEREADONLY;
			if (GetSaveFileName(&OFN))
			{
				AssignHSFile();
				F1 = CreateFile(FileName,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
							FILE_FLAG_RANDOM_ACCESS,NULL);
				i = DialogBox(hInst, "PickLevel", Window, (DLGPROC)PickBox);
				CloseHandle(F1);
				if (i>0)
				{
					CurLevel = i;
					OKtoSave = TRUE;
					SendMessage(MainH,WM_COMMAND,603,0);
				}
			}
			return(0);
		case 604: 		  		// Leave Editor
			if (SendMessage(Ed1,EM_GETMODIFY,0,0) ||
					SendMessage(Ed2,EM_GETMODIFY,0,0)) Modified = TRUE;
			if (Modified) if (MessageBox(Window,txt022,txt029,MB_YESNO | MB_ICONQUESTION) == IDYES)
				SendMessage(Window,WM_COMMAND,603,0);
			EditorOn = FALSE;
			SetWindowText(Window,App_Title);
			SetMenu(Window,MMenu);
			ShowWindow(Ed1, SW_HIDE);
			ShowWindow(Ed2, SW_HIDE);
			ShowWindow(BT1, SW_SHOWNA);
			ShowWindow(BT2, SW_SHOWNA);
			ShowWindow(BT3, SW_SHOWNA);
			ShowWindow(BT4, SW_SHOWNA);
			ShowWindow(BT5, SW_SHOWNA);
			ShowWindow(BT6, SW_SHOWNA);
			ShowWindow(BT7, SW_SHOWNA);
			ShowWindow(BT8, SW_SHOWNA);
			ShowWindow(BT9, SW_SHOWNA);
			memcpy(CurRecData.PF,Game.PF,sizeof(TPLAYFIELD));	// Update CurRec
			CurRecData.PF[Game.Tank.X][Game.Tank.Y] = 1; // save Tank in Playfield's
			InvalidateRect(Window,NULL,TRUE);
			if (CurLevel > 0) GameOn(TRUE);
			return(0);
		case 605:					 // Hint [Editor]
					DialogBox(hInst, "HintBox", Window, (DLGPROC)HintBox);
					return(0);
	    case 701:	EditDiffSet(1);  // Edit Difficulty Set - Kids
					return(0);
        case 702:	EditDiffSet(2);  // Edit Difficulty Set - Easy
					return (0);
        case 703:	EditDiffSet(4);  // Edit Difficulty Set - Medium
					return(0);
        case 704:	EditDiffSet(8);  // Edit Difficulty Set - Hard
					return(0);
        case 705:	EditDiffSet(16); // Edit Difficulty Set - Deadly
					return(0);
		case 710:					// Shift Game Board Right
			for (x=1; x <16; x++) for (y=0; y < 16; y++)
			{
				ShiftPF[x][y] = Game.PF[x-1][y];
				ShiftBMF[x][y] = Game.BMF[x-1][y];
			}
			for (y=0; y < 16; y++)
			{
				ShiftPF[0][y] = Game.PF[15][y];
				ShiftBMF[0][y] = Game.BMF[15][y];
			}
			Game.Tank.X++;
			if (Game.Tank.X == 16) Game.Tank.X = 0;
			Modified = TRUE;
			memcpy(Game.PF,ShiftPF,sizeof(TPLAYFIELD));	// Update PF
			memcpy(Game.BMF,ShiftBMF,sizeof(TPLAYFIELD));	// Update BMF
			InvalidateRect(Window,NULL,TRUE);
			return(0);
	case 711:					// Shift Game Board Left
			for (x=0; x <15; x++) for (y=0; y < 16; y++)
			{
				ShiftPF[x][y] = Game.PF[x+1][y];
				ShiftBMF[x][y] = Game.BMF[x+1][y];
			}
			for (y=0; y < 16; y++)
			{
				ShiftPF[15][y] = Game.PF[0][y];
				ShiftBMF[15][y] = Game.BMF[0][y];
			}
			Game.Tank.X--;
			if (Game.Tank.X < 0) Game.Tank.X = 15;
			Modified = TRUE;
			memcpy(Game.PF,ShiftPF,sizeof(TPLAYFIELD));	// Update PF
			memcpy(Game.BMF,ShiftBMF,sizeof(TPLAYFIELD));	// Update BMF
			InvalidateRect(Window,NULL,TRUE);
			return(0);
	case 712:					// Shift Game Board Up
			for (x=0; x <16; x++) for (y=0; y < 15; y++)
			{
				ShiftPF[x][y] = Game.PF[x][y+1];
				ShiftBMF[x][y] = Game.BMF[x][y+1];
			}
			for (x=0; x < 16; x++)
			{
				ShiftPF[x][15] = Game.PF[x][0];
				ShiftBMF[x][15] = Game.BMF[x][0];
			}
			Game.Tank.Y--;
			if (Game.Tank.Y < 0) Game.Tank.Y = 15;
			Modified = TRUE;
			memcpy(Game.PF,ShiftPF,sizeof(TPLAYFIELD));	// Update PF
			memcpy(Game.BMF,ShiftBMF,sizeof(TPLAYFIELD));	// Update BMF
			InvalidateRect(Window,NULL,TRUE);
			return(0);
	case 713:					// Shift Game Board Down
			for (x=0; x <16; x++) for (y=1; y < 16; y++)
			{
				ShiftPF[x][y] = Game.PF[x][y-1];
				ShiftBMF[x][y] = Game.BMF[x][y-1];
			}
			for (x=0; x < 16; x++)
			{
				ShiftPF[x][0] = Game.PF[x][15];
				ShiftBMF[x][0] = Game.BMF[x][15];
			}
			Game.Tank.Y++;
			if (Game.Tank.Y == 16) Game.Tank.Y = 0;
			Modified = TRUE;
			memcpy(Game.PF,ShiftPF,sizeof(TPLAYFIELD));	// Update PF
			memcpy(Game.BMF,ShiftBMF,sizeof(TPLAYFIELD));	// Update BMF
			InvalidateRect(Window,NULL,TRUE);
			return(0);
		case 901: 	 				// About Box
			x = Game_On;
			GameOn(FALSE);
			DialogBox(hInst, "AboutBox", Window, (DLGPROC)AboutBox);
			GameOn(x);
			return(0);
		case 902:
			WinHelp(Window,HelpFile,HELP_INDEX,0);
			return(0);
		case 903:
			WinHelp(Window,HelpFile,HELP_KEY,(DWORD)&help01);
			return(0);
		case 904:
			WinHelp(Window,HelpFile,HELP_KEY,(long)&help02);
			return(0);
		case 905:
			WinHelp(Window,HelpFile,HELP_KEY,(long)&help03);
			return(0);
		case 907: 	 				// Quick About Box
			x = Game_On;
			GameOn(FALSE);
			QHELP = TRUE;
			InvalidateRect(Window,NULL,FALSE);
			DialogBox(hInst, "RETBOX", Window, (DLGPROC)RetBox);
			QHELP = FALSE;
			InvalidateRect(Window,NULL,FALSE);
			GameOn(x);
			return(0);
		case 906:
			DialogBox(hInst, "GHighList", Window, (DLGPROC)GHSList);
			return(0);
		default:
			return(DefWindowProc(Window, Message, wparam, lparam));
		} 		// switch(wparam) of WM_COMMAND
	default:
		return(DefWindowProc(Window, Message, wparam, lparam));
	}	 // switch (Message)
	return(0);
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	MSG msg;
	HANDLE hAccelTable1, hAccelTable2;
	int tacc;
	char *P;

	// Other instances of app running?
	if (!hPrevInstance)
	{
		// Initialize shared things
		if (!InitApplication(hInstance)) return FALSE;		// Exits if unable to initialize
	}
	hAccelTable1 = LoadAccelerators(hInst,"ACC1");
	hAccelTable2 = LoadAccelerators(hInst,"ACC2");

	// Create Full INI file name
	// Help file name is created in InitInstance()
	GetModuleFileName (NULL,INIFile,MAX_PATH);
	strcpy(HelpFile,INIFile);
	P = strrchr(INIFile,'\\');
	if (P)
	{
		P++;
		strcpy(P,INIFileName);			// add name
		if (GetPrivateProfileInt("DATA",psNET,0,INIFile) == 1) strcpy(INIFile,INIFileName);
	} else strcpy(INIFile,INIFileName);	// Error so put in Windows Directory

	strcpy(LANGFile,INIFile);
	P = strrchr(LANGFile,'\\');
	if (P)
	{
		P++;
		strcpy(P,LANGFileName);			// add name
	} else strcpy(LANGFile,LANGFileName);	// Error so put in Default Directory

	// Perform initializations that apply to a specific instance
	if (!InitInstance(hInstance, nCmdShow)) return FALSE;
	// Get Command line stuff
	if (lpCmdLine[0] != 0)
	{
		strcpy(FileName,lpCmdLine);
		if (strstr(strlwr(lpCmdLine),".lvl"))
		{
			AssignHSFile();
			CurLevel = 0;
			LoadNextLevel(FALSE,FALSE);
		}
	}
	// Acquire and dispatch messages until a WM_QUIT message is received.
	while (GetMessage(&msg, NULL, 0, 0))
	{	// dont translateAccelerators if editor is on

		if (EditorOn) tacc = TranslateAccelerator(msg.hwnd,hAccelTable2,&msg); // Editor Accelerator
			else tacc = TranslateAccelerator(msg.hwnd,hAccelTable1,&msg);

		if (!tacc)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	DestroyMenu(EMenu);
	DeleteObject(MyFont);
	// Returns the value from PostQuitMessage
	return msg.wParam;
}

BOOL InitApplication(HINSTANCE hInstance)
// Initialize the Application ( Only once  )
{
	WNDCLASS wc;
	// Fill in window class structure with parameters that describe the
	// main window.
	wc.style		 = CS_BYTEALIGNWINDOW | CS_SAVEBITS;
	wc.lpfnWndProc   = (WNDPROC)WndProc;        		// Window Procedure
	wc.cbClsExtra    = 0;                       		// No per-class extra data.
	wc.cbWndExtra    = 0;                       		// No per-window extra data.
	wc.hInstance     = hInstance;               		// Owner of this class
	wc.hIcon         = LoadIcon(hInstance, "icon1"); 	// Icon name from .RC
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 	// Cursor
	wc.hbrBackground = GetStockObject(LTGRAY_BRUSH); 	// Default color
	wc.lpszMenuName  = "MAIN";               			// Menu name from .RC
	wc.lpszClassName = App_Class;               	    // Name to register as
	// Register the window class and return FALSE if unsuccesful.
	if (!RegisterClass(&wc)) return FALSE;
	return TRUE;
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND Window; // Main window handle.
	int xp, yp;
	// <LangFile>
    TCHAR *p;
    TCHAR buffer[50];
	// </LangFile>

	hInst = hInstance; // Store instance handle in our global variable
	xp = GetPrivateProfileInt("SCREEN",psXpos,CW_USEDEFAULT,INIFile);
	yp = GetPrivateProfileInt("SCREEN",psYpos,CW_USEDEFAULT,INIFile);
	// Create a main window for this application instance.
	Window = CreateWindow(App_Class,App_Title,
		WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX),
		xp, yp, 725, 579, 0, 0, hInst, NULL);
	if (!Window) return FALSE;
	MainH = Window;
	MMenu = GetMenu(Window);
	OFN.hwndOwner = Window;
	PBfn.hwndOwner = Window;
	EMenu = LoadMenu(hInst,"MENU2");



	// <LangFile>
    InitLanguage(MMenu, EMenu );
    // Buit HelpFile file name here, after InitLanguage()
    p = strrchr(HelpFile,'\\');
    if (p)
    {
    	p++;
    	strcpy(p,HelpFileName);			// add name
    } else strcpy(HelpFile,HelpFileName);	// Error so put in Default Directory

    // Build up the correct filter strings for OPENFILENAME structure
    p = szFilterOFN;
    lstrcpy (buffer,txt002);
    lstrcpy (p,buffer);
    p += lstrlen (buffer) +1;
    lstrcpy (buffer,TEXT("*.LVL"));
    lstrcpy (p,buffer);
    p += lstrlen (buffer) +1;
    strcpy (p,TEXT("\0"));
    OFN.lpstrFilter = szFilterOFN;

    // Build up the correct filter strings for OPENFILENAME structure
    p = szFilterPBfn;
    lstrcpy (buffer,txt005);
    lstrcpy (p,buffer);
    p += lstrlen (buffer) +1;
    lstrcpy (buffer,TEXT("*.LPB"));
    lstrcpy (p,buffer);
    p += lstrlen (buffer) +1;
    strcpy (p,TEXT("\0"));
    PBfn.lpstrFilter = szFilterPBfn;
	// </LangFile>

	// Title Edit Box - Visable only in Editor Mode
	Ed1 = CreateWindow("EDIT","", WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		418,99,163,20,Window,(HMENU)501,hInst,NULL);
	// Author Edit Box - Visable only in Editor Mode }
	Ed2 = CreateWindow("EDIT","",WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
		418,148,163,20,Window,(HMENU)502,hInst,NULL);

	BT1 = CreateWindow("BUTTON",ButText1,WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_DISABLED,
		418,260,70,20,Window,(HMENU)ButID1,hInst,NULL);
	BT2 = CreateWindow("BUTTON",ButText2,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON | WS_DISABLED,
		418,280,145,20,Window,(HMENU)ButID2,hInst,NULL);
	BT3 = CreateWindow("BUTTON",ButText3,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON | WS_DISABLED,
		418,280,145,20,Window,(HMENU)ButID3,hInst,NULL);
	BT4 = CreateWindow("BUTTON",ButText4,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON,
		418,280,70,20,Window,(HMENU)ButID4,hInst,NULL);
	BT5 = CreateWindow("BUTTON",ButText5,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON,
		418,280,70,20,Window,(HMENU)ButID5,hInst,NULL);
	BT6 = CreateWindow("BUTTON",ButText6,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON,
		418,280,70,20,Window,(HMENU)ButID6,hInst,NULL);
	BT7 = CreateWindow("BUTTON",ButText7,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON | WS_DISABLED,
		418,280,70,20,Window,(HMENU)ButID7,hInst,NULL);
	BT8 = CreateWindow("BUTTON",ButText8,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON | WS_DISABLED,
		418,280,70,20,Window,(HMENU)ButID8,hInst,NULL);
	BT9 = CreateWindow("BUTTON",ButText9,WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON,
		418,280,145,20,Window,(HMENU)ButID9,hInst,NULL);
	MyFont = CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,"Arial");
	SetGameSize(GetPrivateProfileInt("SCREEN",psSize,1,INIFile));
	ShowWindow(Window, nCmdShow); // Show the window
	UpdateWindow(Window);         // Sends WM_PAINT message
	return TRUE;                // We succeeded...

}
