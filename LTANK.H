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

/*

Obj			BitM	Description
---     	----    -----------

0 			1		Dirt
1 			2		Tank Up ( Primary )
-			3		Tank Right
-			4		Tank Down
-			5		Tank left
2			6		Base ( Primary)
-			7		Base Alt ( animation )
-			8		Base Alt2
3			9		Water ( Primary )
-			10		Water Alt
-			11		Water Alt 2
-       	12  	Blown Up Anti Tank (down)
4			13		Solid Block NOT movable
5			14		Movable Block
6			15		Bricks
7			16		Anti-Tank Gun Up
-			17		Anti-Tank Alt
-			18		Anti-Tank Alt 2
-			19	    Movable block after pushing into water
11			20		Mirror Up-Lt
12			21		Mirror Up-Rt
13			22		Mirror Dn-Rt
14			23		Mirror Dn-Lt
15			24		One Way Up (Primary)
-			25		One Way Up Alt
-			26		One Way Up Alt2
16			27		One Way Rt (Primary)
-			28		One Way Rt Alt
-			29		One Way Rt Alt2
17			30		One Way Dn (Primary)
-			31		One Way Dn Alt
-			32		One Way Dn Alt2
18			33		One Way Lt (Primary)
-			34		One Way Lt Alt
-			35		One Way Lt Alt2
8 	      	36      Anti-Tank facing right
- 	      	37
- 	      	38
9       	39      Anti-Tank facing down
-       	40
-       	41
10      	42      Anti-Tank facing left
-       	43
-       	44
19      	45      Crystal Block
-       	46      Crystal when hit by tank
20			47		Roto Mirror Up-Lt
21			48		Roto Mirror Up-Rt
22			49		Roto Mirror Dn-Rt
23			50		Roto Mirror Dn-Lt
-       	51      Crystal when hit by Anti-tank
-       	52      Blown Up Anti Tank (right)
-       	53      Blown Up Anti Tank (left)
-       	54      Blown Up Anti Tank (up)
24			56		Ice
25			57		Thin Ice
01dddddx	55		Worm Hole / Tunnel

*/

// Language Section for Lasertank
// Import this from its own file

#include "lt32l_us.h"

// Game Defaults
#define LevelData       "LaserTank.lvl"    // Default Level Data File
#define INIFileName     "LaserTank.ini"
// MAX text size for a lang_text
#define MAX_LANG_SIZE    300



#define MaxObjects	    26              // Maximum number of game objects used
#define MaxBitMaps      58             	// the maximum number of BitMaps ( + 1 )
#define XOffset         17             	// Game Board Offset from Left
#define YOffset         17             	// Game Board Offset from top
#define ani_delay       4              	// animation Delay in GameDelay units
#define GameDelay       50             	// Main Operation Loop
#define WM_GameOver     WM_USER +1     	// Send Message if there are no more levels
#define WM_Dead         WM_USER +2     	// Send Message when you die
#define WM_NewHS	    WM_USER +3      // Send to Display New High Score Dialog
#define WM_SaveRec      WM_USER +4      // Send to Save Recording
#define App_Class		"LaserTC2"		// The Class Name
#define UndoBufStep		200				// Groth Amount of Undo Buffer
#define UndoMax			10000			// Max Amount of Undo Buffer
#define RecBufStep		10000			// Groth amount of Rec Buffer
#define RecMax			65500			// Max recording steps saved to file
#define SlowPBSet		5				// Delay of Slow Playback setting
#define MaxMBuffer		20				// Size of Mouse Buffer
#define Obj_Water		3				// Object Number 3 is water
#define Obj_Ice			24				// Ice Object
#define Obj_ThinIce		25				// Object Number of Thin Ice
#define Obj_Tunnel		0x40			// Object 01dddddX = Tunnel
#define LTG_ID			"LTG1"			// ID field of LTG file
#define psRLLOn			"RLL"          // Profile String for remember last level
#define psRLLN			"RLLFilename"  // remember last level file name
#define psRLLL			"RLLLevel"     // Level number
#define psAni			"Animation"
#define psSound			"Sound"
#define psSize			"Size"			// 1 = small, 3 = large
#define psXpos			"PosX"
#define psYpos			"PosY"
#define psUser 			"Player"
#define psPBA 			"Record Author"
#define psARec			"Auto_Record"
#define psSCL			"SkipComLev"
#define psDiff			"Diff_Setting"
#define psGM			"Graphics_Mode"
#define psGFN			"Graphics_File"
#define psGDN			"Graphics_Dir"
#define psDW			"DisableWarnings"
#define psYes			"Yes"
#define psNET			"NETWORK_INI"	// 1 = Use INI file from C:\Windows
#define BADMOVE 		256

typedef char TPLAYFIELD [16][16]; 		// Matrix of G.O. types

typedef struct tLevel               // Level Data from File
{
  TPLAYFIELD PF;               // Object Grid
  char LName[31];  			// Level Name
  char Hint[256];  			// Hint for this level
  char Author[31]; 			// the Author of the Level
  WORD SDiff; 				// Score Difficulty
}TLEVEL;

typedef struct tRecordRec		// Recording Header
{
	char LName[31];				// Level Name
	char Author[31];			// Author of the recording
	WORD Level;					// Level Number
	WORD Size;					// Size of Data -- Data to fallow
}TRECORDREC;

typedef struct tSearch			// Search Record used in Level Load
{
	int mode,					// 1=title, 2 = author
		SkipComp;				// True = Skip Completed
	WORD Diff;					// Difficulty charecter
	char data[61];				// Search String
}TSEARCH;

typedef struct tTankRec              // Store the Tank & Laser information
{
  int X, Y, Dir, Firing, Good;		// Good is used for Tunnel Wait in Game.Tank
}TTANKREC;

typedef struct tGameRec
{
  TPLAYFIELD	PF,               // Store Game Objects
  				PF2,              // Store Objects Under Stuff ( Ground, conveyor)
  				BMF,              // Bitmaps for Objects
  				BMF2;             // Bitmaps for Under Stuff ( Bridges )
  WORD 			ScoreMove,        // Move Counter
  				ScoreShot;        // Shot Counter
  DWORD			RecP;			  // Recording Pointer
  TTANKREC		Tank;             // Tank Data
}TGAMEREC, *PGAMEREC;

typedef struct tXYRec			// Use in BMA
{
	int X,Y;					// X & Y Location in Big Butmap
}TXYREC;

typedef struct tXYZRec			// Use for Mouse Buffer
{
	int X,Y,Z;
}TXYZREC;

typedef struct tHSRec					// High Score Record
{
	WORD moves, shots;
	char name[6];						// Initials
}THSREC;

typedef struct tLTGRec			// Record for LTG graphics
{
	char Name[40];		// Name of Graphic Set
	char Author[30];	// Author of Graphics set
	char Info[245];		// 3 line Description of Graphics Set
	char ID[5];			// LTG ID = "LTG1"+0
	DWORD MaskOffset;	// Offset from the biggining of file to Mask Bitmap
}TLTGREC;

typedef struct tIceRec	// Record used for sliding on the Ice
{
	int x,y,			// Last XY position of object to move
		dx,dy,			// Direction to move in Delta Cords
		s;				// True if Sliding

}TICEREC;

/// ------------ MGY -----------
#define MAX_TICEMEM 16
typedef struct tIceMem	// Record used for sliding objects on the Ice
{
	TICEREC Objects[MAX_TICEMEM];  // MGY - mem up 16 sliding objects
	int count;          // number of current sliding objects
}TICEMEM;
extern TICEMEM SlideMem;
/// ------------ MGY -----------

// Extern's -- Complete Program Defined Global Variables
extern HINSTANCE hInst;					// Globally Defined Instance
extern char *RecBuffer;
extern int GFXOn, Game_On, Sound_On, Ani_On, RLL, ConvMoving, CurLevel,TankDirty, DWarn;
extern int AniCount, CurSelBM_L, CurSelBM_R, RB_TOS, PBHold, SpBm_Width,SpBm_Height,ContXPos;
extern int ARecord, Difficulty,SkipCL, Recording,PlayBack, EditBMWidth, Speed, SlowPB, RecBufSize;
extern HWND MainH, Ed1, Ed2, BT1, BT2, BT3, BT4, BT5, BT6, BT7,BT8, BT9;
extern TGAMEREC Game,BackUp,SaveGame;
extern TLEVEL CurRecData;
extern HBRUSH LaserColor, LaserColorG, LaserColorR;
extern HDC gDC;                      // Use this game dc for all ops
extern char FileName[], HFileName[], GHFileName[], PBFileName[], GraphFN[], GraphDN[],INIFile[];
extern HMENU MMenu;
extern int Modified,OKtoHS,VHSOn;
extern TRECORDREC PBRec, PBSRec;
extern char HelpFile[];
extern PGAMEREC UndoBuffer;
extern int UndoP,PBOpen;
extern THSREC HS;
extern HDC BuffDC;
extern HBITMAP BuffBMH;
extern TXYREC BMA[MaxBitMaps+1]; 			// Bit Map Array
extern TXYZREC MBuffer[MaxMBuffer];		// Mouse Buffer
extern int MB_TOS, MB_SP;				// TOS = Top of Stack ; SP = Stack Pointer
extern DWORD DEBUG_Time;
extern DWORD DEBUG_Frames;
extern HANDLE F1,F2,F3;					// File Handles
extern DWORD BytesMoved;				// use in ReadFile & WriteFile
extern int GraphM, FindTank;
extern TICEREC SlideO,SlideT;

// Variables for Language
extern char LANGText[SIZE_ALL][MAX_LANG_SIZE]; // All lines of Language.dat
extern char LANGFile[MAX_PATH];                // the dynamic file name for Language.dat
extern TCHAR szFilterOFN[MAX_PATH];
extern TCHAR szFilterPBfn[MAX_PATH];

// Global Function Prototypes

void FileError();
void GFXInit();									// Load All the Bitmaps
void GFXKill(void);								// Release All the Bitmaps
void PutLevel(void);							// Draw Game Board
void GameOn(int);								// Turns on/off game timer
void JK3dFrame(HDC, int, int, int, int, int);	// Draw 3D frame
void JKSelFrame(HDC, int, int, int, int, int);	// Draw Red/Green frame
void LoadLastLevel();
BOOL LoadNextLevel( int, int);
void CheckHighScore(void);						// Compare score to High Score
void Animate(void);								// Animate all Animated Objects
void MoveTank( int );
int  CheckLoc(int, int);
void MoveLaser();
void FireLaser(int, int, int, int);
void AntiTank();								// Target Anti-Tanks at Tank
char GetOBM(char);								// Translate Bitmap to Object
void BuildBMField();							// Convert Objects to bitmaps
void PutSprite (char, int, int);				// Paint BM at X,Y
void UpDateSprite(int, int);					// Repaint bitmap at x,y
void UpDateTank();								// Repaint Tank
void PutSelectors();							// Editor - paint objects to edit
void ChangeGO(int, int, int);					// Change Game Object
void SetGameSize(int);
void SavePBFile();
void InitBuffers();
void KillBuffers();
void AssignHSFile();
void UpdateUndo();
void UndoStep();
void ResetUndoBuffer();
int MouseOperation(int);						// Process Mouse Input
void ShowTunnelID();
void ConvMoveTank(int, int, int);
void IceMoveT();					// Move an tank on the Ice
void IceMoveO();					// Move an Object on the Ice
void AddKBuff(char);

// Function Prototypes for Language
void ChangeMenuText(HMENU, char*);
void InitLanguage(HMENU, HMENU);
void ConvertTabChar(char*);
HBITMAP LoadImageFile(HINSTANCE, char *, char*);
void LoadWindowText(HWND, int);
void LoadWindowCaption(HWND, int);


/* Lets try some Macros */
#define GetTunnelID(x,y) ((Game.PF[x][y] & 0x0F)  >> 1)			// 0 - 7

// mgy 18-05-2003 Tunnel is limited to 0-7.
#define GetTunnelOldID(x,y) ((Game.PF[x][y] & 0x0F)  >> 1)			// 0 - 7
//#define GetTunnelOldID(x,y) ((Game.PF[x][y] & 0x3F)  >> 1)		// 0 - 32

#define ISTunnel(x,y) ((Game.PF[x][y] & Obj_Tunnel) == Obj_Tunnel)
#define GameInProg Game.RecP && (Game.PF[Game.Tank.X][Game.Tank.Y] != 2) && (!DWarn)

