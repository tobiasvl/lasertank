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
#include <windowsx.h>
#include <commdlg.h>
#include <string.h>
#include <mmsystem.h>
#include <stdio.h>
#include "ltank.h"
#include "ltank_d.h"
#include "lt_sfx.h"

// Declare the Global Varables

int GFXError      	= 0;            // error used for load
int GFXOn			= FALSE;		// True when Graphics are loaded
int TankDirty		= FALSE;		// if true then we need to repaint the tank
int NoLevel     	= TRUE;         // if true Main Paint will show Openning
int Game_On     	= FALSE;        // true when game is running
int Ani_On      	= TRUE;         // true when Animation is On
int RLL         	= TRUE;         // remember last level
int ConvMoving   	= FALSE;        // true when moving on the conveyor belts
int OKtoHS			= TRUE;			// true if OK to Set HighScore
int OKtoSave		= FALSE;		// true if OK to Set HighScore
int Recording		= FALSE;		// true if Recording
int PlayBack		= FALSE;		// true if PlayBack is recording
int PBOpen			= FALSE;		// true when Playback window is open
int ARecord			= FALSE;		// AutoRecord is On/Off
int SkipCL			= FALSE;		// true if Skip Complete Level is on
int DWarn			= FALSE;		// Disable Warning
int CurLevel       	= 0;            // Used to Figure out the Current Level
int AniLevel        = 0;            // Used for Animation Position
int AniCount        = 0;            // counter for animation
int CurSelBM_L     	= 3;            // current selected bm in editor
int CurSelBM_R     	= 0;            // current selected bm in editor
int SpBm_Width		= 32;			// Width of Sprite
int SpBm_Height		= 32;			// Height of Sprite
int LaserOffset		= 10;			// Offset of Laser Size
int ContXPos		= 540;			// Position of Control Side
int EditBMWidth		= 5;			// # of bitmaps across edit select area
int Speed			= 1;			// Playback speed
int SlowPB			= 1;
int RecBufSize		= 10000;		// Size of recording buffer
int UndoBufSize		= 3200;			// Size of Undo Buffer ( * sizeof(TGAMEREC))
int Difficulty		= 0;			// Difficulty Enable ( use Bits )
int GraphM			= 0;			// Graphics Mode 0=int; 1=ext; 2=ltg
int FindTank		= FALSE;		// True when First starting a level
int BlackHole		= FALSE;		// True if we TunnleTranslae to a Black Hole


TGAMEREC Game,SaveGame;		  // The Level Data
TLEVEL CurRecData;
HBRUSH LaserColor,LaserColorR, LaserColorG;
HDC gDC;                      		  // Use this game dc for all ops
char FileName[MAX_PATH], HFileName[MAX_PATH], GHFileName[MAX_PATH];
char PBFileName[MAX_PATH], GraphFN[MAX_PATH], GraphDN[MAX_PATH],INIFile[MAX_PATH];
int Modified;
TICEREC SlideO,SlideT;
TICEMEM SlideMem; // MGY - mem up MAX_TICEMEM sliding objects
int wasIce;				// CheckLoc will set this to true if Ice
int WaitToTrans;

// Global Varables

TXYREC BMA[MaxBitMaps+1]; 		// Bit Map Array
TTANKREC laser;
HDC BuffDC,MaskDC;              // used to bitblat all sprites
HBITMAP BuffBMH,MaskBMH;		// Handle to Bitmaps in above DC's
const int GetOBMArray[MaxObjects+1] = {1,2,6,9,13,14,15,16,36,39,42,20,21,22,23,24,27,30,33,45,47,48,49,50,56,57,55};
const int CheckArray[MaxObjects] = {1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1};
// Pad the beggining with junk array was [1..MaxBitMaps]
const int BMSTA[MaxBitMaps+1] = {0,0,1,1,1,1,0,0,0,0,0,0,1,0,1,0,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,0,0,0};
const long ColorList[8] =  {0x000000FF,0x0000FF00,0x00FF0000,0x00FFFF00,0x00FFFF,
							0x00FF00FF,0x00FFFFFF,0x00808080};
int UndoP;
TRECORDREC PBRec, PBSRec;
char *RecBuffer;
PGAMEREC UndoBuffer;
THSREC HS;
HANDLE F1,F2,F3;
DWORD BytesMoved;
TXYZREC MBuffer[MaxMBuffer];
int MB_TOS, MB_SP;			// TOS = Top of Stack ; SP = Stack Pointer
int findmap[16][16];

int Backspace[10];			// Backspace Buffer
int BS_SP	= 0;			// StackPointer for Backspace
int UndoRollOver;

// Local function declaration

void SetButtons( int);
void SpriteLoad(char *);
static void FindTarget( int, int, int);

//
// Start the code section
//

// ---------------------------------------------------
// Move   a Sliding Object FROM stack
// ---------------------------------------------------
void Mem_to_SlideO( int iSlideObj ) // MGY
{
	if (iSlideObj <= SlideMem.count ) {
	    SlideO.x  = SlideMem.Objects[iSlideObj].x;
		SlideO.y  = SlideMem.Objects[iSlideObj].y;
		SlideO.dx = SlideMem.Objects[iSlideObj].dx;
		SlideO.dy = SlideMem.Objects[iSlideObj].dy;
		SlideO.s  = SlideMem.Objects[iSlideObj].s;
	}
}

// ---------------------------------------------------
// Move a Sliding Object ON TO stack
// Update SlideMem with an object SlideO
// ---------------------------------------------------
void SlideO_to_Mem( int iSlideObj ) // MGY
{
 	if (iSlideObj <= SlideMem.count ) {
	    SlideMem.Objects[iSlideObj].x  = SlideO.x ;
		SlideMem.Objects[iSlideObj].y  = SlideO.y ;
		SlideMem.Objects[iSlideObj].dx = SlideO.dx;
		SlideMem.Objects[iSlideObj].dy = SlideO.dy;
		SlideMem.Objects[iSlideObj].s  = SlideO.s ;
	}
}


// ---------------------------------------------------
// Add an object in the stack for slidings objects
// But, if this object is already in this stack,
// just change dir and don't increase the counter.
// ---------------------------------------------------
void add_SlideO_to_Mem() // MGY
{
	int iSlideObj;

	if (SlideMem.count < MAX_TICEMEM-1) {
	    for ( iSlideObj = 1 ; iSlideObj <= SlideMem.count; iSlideObj++) {
			if ( (SlideMem.Objects[iSlideObj].x  == SlideO.x) &&
				 (SlideMem.Objects[iSlideObj].y  == SlideO.y) )
			{
				SlideO_to_Mem( iSlideObj ); // Update the stack
				return; // don't inc the counter
			}
		}
		// Add this object to the stack
		SlideMem.count++;
		SlideO_to_Mem(SlideMem.count);
		SlideO.s = (SlideMem.count>0);
	}
}

// ---------------------------------------------------
// Delete a Sliding Object from stack
// ---------------------------------------------------
void sub_SlideO_from_Mem( int iSlideObj ) // MGY
{
	int i;
	for (i = iSlideObj ; i < SlideMem.count; i++) {
		Mem_to_SlideO( i+1 );
	    SlideO_to_Mem( i );
	}
	SlideMem.count--;
	SlideO.s = (SlideMem.count>0);
}

// ---------------------------------------------------
// If an object is sliding and is hit by a laser,
// delete it from stack.
// ---------------------------------------------------
void del_SlideO_from_Mem( int x, int y ) // MGY
{
	int iSlideObj;
    for ( iSlideObj = SlideMem.count; iSlideObj >=1 ; iSlideObj--) {
		if ( (SlideMem.Objects[iSlideObj].x  == x) &&
			 (SlideMem.Objects[iSlideObj].y  == y) )
		{
			// remove this object
		    sub_SlideO_from_Mem( iSlideObj );

			return;
		}
	}
	SlideO.s = (SlideMem.count>0);
}

// ---------------------------------------------------
// Used to handle a bug :  the speed bug
// MGY - 22-nov-2002
// Return True if the tank is on Convoyor.
// ---------------------------------------------------
int TestIfConvCanMoveTank()
{
	switch (Game.PF[Game.Tank.X][Game.Tank.Y])
	{
	case 15:
		if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) // Conveyor Up
			return(TRUE);
		break;
	case 16:
		if (CheckLoc(Game.Tank.X+1,Game.Tank.Y))
			return(TRUE);
		break;
	case 17:
		if (CheckLoc(Game.Tank.X,Game.Tank.Y+1))
			return(TRUE);
		break;
	case 18:
		if (CheckLoc(Game.Tank.X-1,Game.Tank.Y))
			return(TRUE);
		break;
	}
	return( FALSE );
}


void SetButtons( int ButtonX)
{
	EnableWindow(BT1,(ButtonX & 1) == 1);
	EnableWindow(BT2,(ButtonX & 2) == 2);
	EnableWindow(BT3,(ButtonX & 4) == 4);
	EnableWindow(BT4,(ButtonX & 8) == 8);
	EnableWindow(BT5,(ButtonX & 16) == 16);
	EnableWindow(BT6,(ButtonX & 32) == 32);
	EnableWindow(BT7,(ButtonX & 64) == 64);
	EnableWindow(BT8,(ButtonX & 128) == 128);
}

void FileError()
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0, NULL );
	// Display the string.
	MessageBox( MainH, lpMsgBuf, "System Error", MB_OK|MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

void AddKBuff( char zz)
{
	int i;
	RecBuffer[RB_TOS] = zz;
	RB_TOS++;
	if (RB_TOS >= RecBufSize)
	{

		i = RecBufSize + RecBufStep;
		RecBuffer = GlobalReAlloc(RecBuffer,i,GMEM_MOVEABLE);
		if (RecBuffer == NULL)
		{			// Recorder Buffer Overflow
			FileError();
			if (Recording) SendMessage(MainH,WM_SaveRec,0,0);
			RB_TOS = 0;
		}
		else RecBufSize = i;
	}
}


/* find the shortest path to the target via a fill search algorithm */
static void FindTarget(int px, int py, int pathlen)
{
	if ((px<0) || (px>15) || (py<0) || (py>15)) return; // outer edges
	// if we hit something AND we are not at the tank then return
	if((Game.PF[px][py] != 0) && (!((Game.Tank.X == px) && (Game.Tank.Y == py))))
		return;		// we hit something - ouch


	if(findmap[px][py] <= pathlen) return;

	findmap[px][py] = pathlen++;

	if((px == Game.Tank.X) && (py == Game.Tank.Y)) return; // speed's us up

	FindTarget(px - 1, py, pathlen);
	FindTarget(px + 1, py, pathlen);
	FindTarget(px, py - 1, pathlen);
	FindTarget(px, py + 1, pathlen);
}

int MouseOperation (int sp)
{
	int dx,dy,XBigger,cx,cy,ltdir;

	dx = MBuffer[sp].X - Game.Tank.X;
	dy = MBuffer[sp].Y - Game.Tank.Y;
	XBigger = abs(dx) > abs(dy);		// true if x is bigger than y

	if (MBuffer[sp].Z == 1)
	{
		// Mouse Move
		/* Fill the trace map */
		for(cx = 0; cx < 16; cx++)
			for (cy = 0; cy < 16; cy++)
				findmap[cx][cy] = BADMOVE;
		// We will test the destination manually
		dx = Game.PF[MBuffer[sp].X][MBuffer[sp].Y];		// temp store in dx
		if (!( (dx < 3) || ((dx > 14) && (dx < 19)) ||
			    (dx > 23) || ((Obj_Tunnel & dx) == Obj_Tunnel)))
			return(FALSE);
		findmap[MBuffer[sp].X][MBuffer[sp].Y] = 0;		// destination
		/* flood fill search to find a shortest path to the push point. */
		FindTarget(MBuffer[sp].X - 1, MBuffer[sp].Y, 1);
		FindTarget(MBuffer[sp].X + 1, MBuffer[sp].Y, 1);
		FindTarget(MBuffer[sp].X, MBuffer[sp].Y - 1, 1);
		FindTarget(MBuffer[sp].X, MBuffer[sp].Y + 1, 1);

		/* if we didn't make it back to the players position, there is no valid path
		* to that place.
		*/
		if(findmap[Game.Tank.X][Game.Tank.Y] == BADMOVE) {
			return(FALSE);
		} else {
			/* we made it back, so let's walk the path we just built up */
			cx = Game.Tank.X;
			cy = Game.Tank.Y;
			ltdir = Game.Tank.Dir;			// we need to keep track of direction
			while(findmap[cx][cy]) {
				if((cx > 0) && (findmap[cx - 1][cy] == (findmap[cx][cy] - 1))) {
					if (ltdir != 4) AddKBuff(VK_LEFT);
					AddKBuff(VK_LEFT);
					cx--;
					ltdir = 4;
				} else if((cx < 15) && (findmap[cx + 1][cy] == (findmap[cx][cy] - 1))) {
					if (ltdir != 2) AddKBuff(VK_RIGHT);
					AddKBuff(VK_RIGHT);
					cx++;
					ltdir = 2;
				} else if((cy > 0) && (findmap[cx][cy - 1] == (findmap[cx][cy] - 1))) {
					if (ltdir != 1) AddKBuff(VK_UP);
					AddKBuff(VK_UP);
					cy--;
					ltdir = 1;
				} else if((cy < 15) && (findmap[cx][cy + 1] == (findmap[cx][cy] - 1))) {
					if (ltdir != 3) AddKBuff(VK_DOWN);
					AddKBuff(VK_DOWN);
					cy++;
					ltdir = 3;
				} else {
					/* if we get here, something is SERIOUSLY wrong, so we should abort */
					abort();
				}
			}
		}

	}
	else {
		// Mouse Shot
		if (XBigger)
		{
			if (dx > 0)
			{
				if (Game.Tank.Dir != 2) AddKBuff(VK_RIGHT);		// Turn Right
			}
			else if (Game.Tank.Dir != 4) AddKBuff(VK_LEFT);		// Turn Left
		}
		else {
			if (dy > 0)
			{
				if (Game.Tank.Dir != 3) AddKBuff(VK_DOWN);		// Turn Down
			}
			else if (Game.Tank.Dir != 1) AddKBuff(VK_UP);		// Turn Up
		}
		AddKBuff(VK_SPACE);
	}
	return(TRUE);
}

void InitBuffers()
{
	int x;

	UndoP = 0;
	UndoBuffer = GlobalAlloc(GMEM_FIXED,UndoBufSize * sizeof(TGAMEREC));// Undo Buffer
	RecBuffer = GlobalAlloc(GMEM_FIXED,RecBufSize);						// Recording Buffer
	Backspace[BS_SP] = 0;												// Clear Backspace stack
	MB_TOS = MB_SP = 0;
	SlideT.s = 0;														// nothing sliding
	SlideO.s = 0;
	SlideMem.count = 0; // MGY

	UndoRollOver = UndoMax;
}

void ResetUndoBuffer()
// Reset Undo Buffer to 1 block only; all variable = 0
{
	UndoBuffer = GlobalReAlloc(UndoBuffer,UndoBufStep * sizeof(TGAMEREC),GMEM_MOVEABLE);
	if (UndoBuffer == NULL)
	{			// Undo Buffer Allocation Error
		FileError();
	}
	UndoBufSize = UndoBufStep;
	UndoP = 0;
	UndoBuffer->Tank.Dir = 0;
	// Lets also init the Mouse Buffer
	MB_TOS = MB_SP = 0;
}

void KillBuffers()
{
	GlobalFree(UndoBuffer);
	GlobalFree(RecBuffer);
}

void UpdateUndo()		// Come here whenever we move or shoot
{
	int i;
	PGAMEREC tmpUndoArray;

	UndoP++;
	if (UndoP >= UndoBufSize)
	{
		if (UndoP >= UndoMax)
		{
			UndoRollOver = (UndoP - 1);			// Save Where we rolled Over
			UndoP = 0;
		}
		else {
			i = UndoBufSize + UndoBufStep;
			tmpUndoArray = GlobalReAlloc(UndoBuffer,i * sizeof(TGAMEREC),GMEM_MOVEABLE);
			if (tmpUndoArray == NULL)
			{			// Undo Buffer Allocation Error
				MessageBox(MainH,txt019,txt007,MB_OK | MB_ICONERROR);
				UndoRollOver = (UndoP - 1);			// Save Where we rolled Over
				UndoP = 0;
			} else 	{
				UndoBufSize = i;
				UndoBuffer = tmpUndoArray;
			}
		}
	}
	UndoBuffer[UndoP] = Game;
	EnableMenuItem(MMenu,110,MF_BYCOMMAND | MF_ENABLED); // enable Undo Command
	EnableWindow(BT1,SW_SHOWNA);
}

void UndoStep()
{
	if (UndoBuffer[UndoP].Tank.Dir == 0)	 return;	// out of data
	Game = UndoBuffer[UndoP];
	RB_TOS = Game.RecP;						// clear all keys not processed
	SlideT.s = 0;							// stop any sliding
	SlideO.s = 0;
	SlideMem.count = 0; // MGY
	UndoBuffer[UndoP].Tank.Dir = 0;
	UndoP--;
	if (UndoP < 0)
	{
		UndoP = UndoRollOver;
	}
	MB_TOS = MB_SP = 0;						// we need to cancle all mouse inputs
}

void PutSprite(char bmn, int x,int y)
// paint sprite ( bitmap ) at screen location x,y
// add grass behind if transparent
{
	if (BMSTA[bmn] == 1)
	{
		BitBlt(gDC,x,y,SpBm_Width,SpBm_Height,BuffDC,BMA[1].X,BMA[1].Y,SRCCOPY);
		SetTextColor(gDC,RGB(0,0,0));
		SetBkColor(gDC,RGB(255,255,255));
		BitBlt(gDC,x,y,SpBm_Width,SpBm_Height,MaskDC,BMA[bmn].X,BMA[bmn].Y,SRCAND);
		BitBlt(gDC,x,y,SpBm_Width,SpBm_Height,BuffDC,BMA[bmn].X,BMA[bmn].Y,SRCPAINT);
	}
	else BitBlt(gDC,x,y,SpBm_Width,SpBm_Height,BuffDC,BMA[bmn].X,BMA[bmn].Y,SRCCOPY);
}

void UpDateSprite(int x, int y)
// re-paint sprite at Game Board Location x,y
{
	int bmn,bmn2,x1,y1;
	HBRUSH Bpen,penO;

	bmn = Game.BMF[x][y];
	x1 = XOffset + (x * SpBm_Width);
	y1 = YOffset + (y * SpBm_Height);

	if (bmn == 55)
	{
		Bpen = CreateSolidBrush(ColorList[GetTunnelID(x,y)]);
		penO = SelectObject(gDC,Bpen);
		Rectangle(gDC,x1,y1,x1+SpBm_Width,y1+SpBm_Height);
		SetTextColor(gDC,RGB(0,0,0));
		SetBkColor(gDC,RGB(255,255,255));
		BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,MaskDC,BMA[55].X,BMA[55].Y,SRCAND);
		BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,BuffDC,BMA[55].X,BMA[55].Y,SRCPAINT);
		SelectObject(gDC,penO);
		DeleteObject(Bpen);
	}
	else
	    {
		if (BMSTA[bmn] == 1 )
		{
			bmn2 = Game.BMF2[x][y];
			if (bmn2 == 55)
			{
				Bpen = CreateSolidBrush(ColorList[((Game.PF2[x][y] & 0x0F)  >> 1)]);
				penO = SelectObject(gDC,Bpen);
				Rectangle(gDC,x1,y1,x1+SpBm_Width,y1+SpBm_Height);
				SetTextColor(gDC,RGB(0,0,0));
				SetBkColor(gDC,RGB(255,255,255));
				BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,MaskDC,BMA[55].X,BMA[55].Y,SRCAND);
				BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,BuffDC,BMA[55].X,BMA[55].Y,SRCPAINT);
				SelectObject(gDC,penO);
				DeleteObject(Bpen);
			}
			else
			    BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,BuffDC,BMA[bmn2].X,BMA[bmn2].Y,SRCCOPY);
			SetTextColor(gDC,RGB(0,0,0));
			SetBkColor(gDC,RGB(255,255,255));
			BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,MaskDC,BMA[bmn].X,BMA[bmn].Y,SRCAND);
			BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,BuffDC,BMA[bmn].X,BMA[bmn].Y,SRCPAINT);
		}
		else BitBlt (gDC,x1,y1,SpBm_Width,SpBm_Height,BuffDC,BMA[bmn].X,BMA[bmn].Y,SRCCOPY);
	}
}

void UpDateTank()
// paint mask then tank sprite, we mask the cell so the back ground will show
{
	TankDirty = FALSE;
	SetTextColor(gDC,RGB(0,0,0));
	SetBkColor(gDC,RGB(255,255,255));
	BitBlt (gDC,XOffset +(Game.Tank.X*SpBm_Width),YOffset +(Game.Tank.Y*SpBm_Height),
		SpBm_Width,SpBm_Height,MaskDC,BMA[1 + Game.Tank.Dir].X,0,SRCAND);
	BitBlt (gDC,XOffset +(Game.Tank.X*SpBm_Width),YOffset +(Game.Tank.Y*SpBm_Height),
		SpBm_Width,SpBm_Height,BuffDC,BMA[1 + Game.Tank.Dir].X,0,SRCPAINT);
}

void UpDateLaser()
// paint laser
{
	HBRUSH ob;
	int   x,y;

	ob = SelectObject(gDC,LaserColor);
	x = XOffset + (laser.X*SpBm_Width);
	y = YOffset + (laser.Y*SpBm_Height);
	if ((laser.Dir & 1) == 1) Rectangle(gDC,x+LaserOffset,y,x+SpBm_Width-LaserOffset,y+SpBm_Height);
	else  Rectangle(gDC,x,y+LaserOffset,x+SpBm_Width,y+SpBm_Height-LaserOffset);
	SelectObject(gDC,ob);
}

int LaserBounceOnIce;

void UpDateLaserBounce(int a, int b)
// paint laser bouncing off of a mirror }
{
	HBRUSH ob;
	int   x,y,h;
	int iSlideObj; // MGY

	ob = SelectObject(gDC,LaserColor);
	x = XOffset + (laser.X*SpBm_Width);
	y = YOffset + (laser.Y*SpBm_Height);
	h = SpBm_Width / 2;

	// we need to stop advance the LaserShot if sliding on ice & hit
	for ( iSlideObj=1; iSlideObj<= SlideMem.count; iSlideObj++) // MGY
		// MGY
		if (SlideMem.Objects[iSlideObj].s
	        && (SlideMem.Objects[iSlideObj].x == laser.X)
			&& (SlideMem.Objects[iSlideObj].y == laser.Y)) LaserBounceOnIce = TRUE;
		//if (SlideO.s && (SlideO.x == laser.X) && (SlideO.y == laser.Y)) LaserBounceOnIce = TRUE;

	switch (a)
	{
	case 1: Rectangle(gDC,x+LaserOffset,y+h,x+SpBm_Width-LaserOffset,y+SpBm_Height);
			break;
	case 2: Rectangle(gDC,x,y+LaserOffset,x+h,y+SpBm_Height-LaserOffset);
			break;
	case 3:	Rectangle(gDC,x+LaserOffset,y,x+SpBm_Width-LaserOffset,y+h);
			break;
	case 4: Rectangle(gDC,x+h,y+LaserOffset,x+SpBm_Width,y+SpBm_Height-LaserOffset);
	}
	switch (b)
	{
	case 1: Rectangle(gDC,x+LaserOffset,y,x+SpBm_Width-LaserOffset,y+h);
		break;
	case 2: Rectangle(gDC,x+h,y+LaserOffset,x+SpBm_Width,y+SpBm_Height-LaserOffset);
		break;
	case 3: Rectangle(gDC,x+LaserOffset,y+h,x+SpBm_Width-LaserOffset,y+SpBm_Height);
		break;
	case 4: Rectangle(gDC,x,y+LaserOffset,x+h,y+SpBm_Height-LaserOffset);
	}
	SelectObject(gDC,ob);
}

void PutLevel()
// paint all game cell's and the tank
{
	HBRUSH ob;
	int x,y;

	for (y=0; y<16; y++) for (x=0;x<16;x++) UpDateSprite(x,y);
	UpDateTank();
	TankDirty = FALSE;
	if (FindTank)
	{
		ob = SelectObject(gDC,LaserColorR);
		SelectObject(gDC,GetStockObject(NULL_PEN));
		x = XOffset + (Game.Tank.X*SpBm_Width);
		y = YOffset + (Game.Tank.Y*SpBm_Height);
		Rectangle(gDC,x+LaserOffset,YOffset+2,x+SpBm_Width-LaserOffset,YOffset+(16*SpBm_Height));
		Rectangle(gDC,XOffset+2,y+LaserOffset,XOffset+(16*SpBm_Width),y+SpBm_Height-LaserOffset);
		SelectObject(gDC,ob);
	}
}

HBITMAP LoadBitmapFile (HDC dc, char *FileName)
// Load a WIndows style Bitmap file
{
	HANDLE F;
	LONG Size, n;
	LPBITMAPINFO P;
	BITMAPFILEHEADER Header;
	HBITMAP bm;
	LPVOID P2;

	bm = 0;										// set error
	F = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if (F == INVALID_HANDLE_VALUE) return(0);						// File not found
	ReadFile(F,&Header,sizeof(Header),&BytesMoved,NULL);
	if ((BytesMoved != sizeof(Header)) || (Header.bfType != 0x4d42))
		goto ErrExit1;
	Size = SetFilePointer(F,0,NULL,FILE_END) - sizeof(Header);		// Size = filesize(F) - Sizeof(header)
	SetFilePointer(F,sizeof(Header),NULL,FILE_BEGIN);				// Reset File Position
	P = GlobalAlloc(GMEM_FIXED, Size);		// Get Big lump of Memory
	if (P == 0) goto ErrExit1;						// Memory Error

	// This only works with 32bit LCC if size > 65K
	if (!(ReadFile(F,P,Size,&BytesMoved,NULL))) FileError();

	if (P->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
	{
		n = Header.bfOffBits - sizeof(BITMAPFILEHEADER);	//Compute image Location
		P2 = (char *)P + n;
		bm = CreateDIBitmap(dc,(LPBITMAPINFOHEADER)P,CBM_INIT,P2,P,DIB_RGB_COLORS);
	}
	GlobalFree(P);
ErrExit1:
	CloseHandle(F);
	return(bm);
}


HBITMAP LoadBitmapMem (HDC dc, LPBITMAPFILEHEADER Header)
// Load a WIndows style Bitmap from a memory block
{
	LPBITMAPINFO P;
	HBITMAP bm;
	LPVOID P2;


	if (Header->bfType != 0x4d42) return (0);

	P = (char *)Header + sizeof(BITMAPFILEHEADER);

	if (P->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
	{
		P2 = (char *)Header + Header->bfOffBits;
		bm = CreateDIBitmap(dc,(LPBITMAPINFOHEADER)P,CBM_INIT,P2,P,DIB_RGB_COLORS);
		return(bm);
	}
	return(0);
}

int LoadLTG( HDC dc, HBITMAP *GH, HBITMAP *MH, char *FN)
{
	HANDLE F;
	TLTGREC tmpLTG;
	DWORD GSize,MSize,LTSize;
	void *P;
	char temps[250];

	if ((F = CreateFile(FN, GENERIC_READ, FILE_SHARE_READ,NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,NULL)) == INVALID_HANDLE_VALUE) return(0);
	LTSize = SetFilePointer(F, 0, NULL, FILE_END);
	SetFilePointer(F, 0, NULL, FILE_BEGIN);

	if (!(ReadFile(F, &tmpLTG, sizeof(TLTGREC), &BytesMoved, NULL))) FileError();
	if (strcmp(tmpLTG.ID,LTG_ID) != 0)
	{
		MessageBox(MainH,txt031,txt007,MB_OK | MB_ICONERROR);
		return(FALSE);
	}
	GSize = tmpLTG.MaskOffset - sizeof(TLTGREC);
	MSize = LTSize - tmpLTG.MaskOffset;

	// transfer game bitmap from Filename -> GH
	P = GlobalAlloc(GMEM_FIXED, GSize);		// Get Big lump of Memory
	if (P != 0)
	{
		if (!(ReadFile(F,P,GSize,&BytesMoved,NULL))) FileError();
		*GH = LoadBitmapMem(dc,P);
	}
	GlobalFree(P);

	// transfer Mask bitmap from Filename -> MH
	P = GlobalAlloc(GMEM_FIXED, MSize);		// Get Big lump of Memory
	if (P != 0)
	{
		if (!(ReadFile(F,P,MSize,&BytesMoved,NULL))) FileError();
		*MH = LoadBitmapMem(dc,P);
	}
	GlobalFree(P);
	CloseHandle(F);
	return(TRUE);
}


void GFXInit()
{
	HDC dc,tdc;
	HBITMAP Gh, Mh ,xh2;
	int x,y,i;

	SetCurrentDirectory(GraphDN);
	dc = GetDC(MainH);
	BuffDC = CreateCompatibleDC(dc);	// DC for All Sprites
	MaskDC = CreateCompatibleDC(dc);	// DC for all Masks -- same size as Sprites
	tdc = CreateCompatibleDC(dc);		// Temp DC
	if (GraphM == 1)					// External
	{
		Gh = 0;							// need this if error
		Mh = LoadBitmapFile(dc,MASK_BMP);
		if (Mh) Gh = LoadBitmapFile(dc,GAME_BMP);
		if (!(Mh || Gh)) GraphM = 0;	// Error so Set Mode 0 - Internal
	}
	else if (GraphM == 2)				// LTG
	{
		if (!(LoadLTG(dc,&Gh,&Mh,GraphFN))) GraphM = 0;
	}
	if (GraphM == 0)					// Internal Graphics are used
	{
		Mh = LoadBitmap(hInst,"MASKBM");
		if (Mh) Gh = LoadBitmap(hInst,"GAMEBM");
	}
	if (!(Mh || Gh))					// last chance error check
	{
		GFXError = 1;
		if (GFXError != 0) MessageBox(0,"Graphics Loading Error",txt007,MB_OK | MB_ICONERROR);
		return;
	}
	// Create Mask Bitmap; Sized Properly for Screen
	SelectObject(MaskDC,Mh);			// This sets up the B&W
	MaskBMH = CreateCompatibleBitmap(MaskDC,SpBm_Width * 10,SpBm_Height * 6);
	SelectObject(MaskDC,MaskBMH); 		// Put New Blank Bitmap in MaskDC
	xh2 = SelectObject(tdc,Mh);			// Hold Old Value
	// Copy Bitmap to new size into MaskDC
	StretchBlt(MaskDC,0,0,SpBm_Width * 10, SpBm_Height * 6,tdc,0,0,320,192,SRCCOPY);
	SelectObject(tdc,xh2);				// Restore Old Value
	DeleteObject(Mh);					// Get Rid of original Bitmap

	// Create Game Bitmap; Sized Properly for Screen
	SelectObject(BuffDC,Gh);			// This sets up the color
	BuffBMH = CreateCompatibleBitmap(BuffDC,SpBm_Width * 10,SpBm_Height * 6);
	SelectObject(BuffDC,BuffBMH); 		// Put New Blank Bitmap in GameDC
	xh2 = SelectObject(tdc,Gh);			// Hold Old Value
	// Copy Bitmap to new size into GameDC
	StretchBlt(BuffDC,0,0,SpBm_Width * 10, SpBm_Height * 6,tdc,0,0,320,192,SRCCOPY);
	SelectObject(tdc,xh2);				// Restore Old Value
	DeleteObject(Gh);					// Get Rid of original Bitmap
	x = 0; y = 0;
	for (i = 1; i <= MaxBitMaps; i++)
	{
		BMA[i].X = x;
		BMA[i].Y = y;
		x += SpBm_Width;
		if ( x >= (SpBm_Width * 10))
		{
			x = 0; y += SpBm_Height;
		}
	}
	DeleteDC (tdc);
	ReleaseDC(MainH,dc);
	GFXOn = TRUE;
}

void GFXKill()
{
	DeleteObject(MaskBMH);
	DeleteObject(BuffBMH);
	DeleteDC(MaskDC);
	DeleteDC(BuffDC);
	GFXOn = FALSE;
}

void ChangeGO(int x, int y, int CurSelBM)  // Change Game Object
{
	int i;
	gDC = GetDC(MainH);
	if (CurSelBM == 1)  // Tank
	{
		UpDateSprite(Game.Tank.X,Game.Tank.Y); // repaint under old tank
		Game.BMF[x][y] = 1;
		Game.PF[x][y] = 0;
		Game.Tank.X = x; Game.Tank.Y = y;
		UpDateSprite(Game.Tank.X,Game.Tank.Y); // paint under new tank
		UpDateTank();
	}
	else
	    {
		if (CurSelBM == MaxObjects)
		{
			// Tunnel
			i = DialogBox(hInst,"LoadTID",MainH,(DLGPROC)LoadTID) << 1;
			Game.PF[x][y] = i + 0x40;
			Game.BMF[x][y] = 55;
			UpDateSprite(x,y);
			ShowTunnelID();
		}
		else {
			Game.PF[x][y] = CurSelBM;
			Game.BMF[x][y] = GetOBM(CurSelBM);
			UpDateSprite(x,y);
		}

	}
	ReleaseDC(MainH,gDC);
}

void BuildBMField()
{
	int x,y,i,j;
	unsigned char pt; // mgy 18-05-2003

	Game.Tank.X = 7; Game.Tank.Y = 15;
	Game.Tank.Dir = 1; Game.Tank.Firing = FALSE;
	for (x = 0; x<16; x++) for (y = 0; y<16; y++)
	{
		// --- mgy 18-05-2003 only legal pieces ---
		pt = Game.PF[x][y];
		if(pt>0x19)
		{
			pt = GetTunnelID(x,y);
			Game.PF[x][y] = (pt<<1) +0x40;
		}
		// --- end of 18-05-2003 ---

		if (Game.PF[x][y] == 1)
		{
			i = 1;
			Game.Tank.X = x;
			Game.Tank.Y = y;
			Game.PF[x][y] = 0;
		}
		else {
			if (Game.PF[x][y] < 64 ) i = GetOBM(Game.PF[x][y]);
			else if (ISTunnel(x,y)) i = 55;
		}
		Game.BMF[x][y] = i;
		Game.BMF2[x][y] = 1;
		Game.PF2[x][y] = 0;
	}
	// this is a good place to reset the score counters }
	Game.ScoreMove = 0;
	Game.ScoreShot = 0;
}

void GameOn(int b)
{
	if (b) SetTimer(MainH,1,GameDelay,NULL);
	else  KillTimer(MainH,1);
	Game_On = b;
#ifdef DEBUG
	if (b) DEBUG_Time = timeGetTime();
	DEBUG_Frames = 0;
#endif
}

void JK3dFrame(HDC dc, int x, int y, int x2, int y2, int up)
{
	HPEN  Wpen,Bpen,penO;

	Wpen = CreatePen(PS_SOLID,1,0x00FFFFFF);
	Bpen = CreatePen(PS_SOLID,1,0x00808080);
	if (up)  penO = SelectObject(dc,Wpen);
	else penO = SelectObject(dc,Bpen);
	MoveToEx(dc,x,y2,NULL);
	LineTo(dc,x,y);
	LineTo(dc,x2,y);
	if (up)  SelectObject(dc,Bpen);
	else SelectObject(dc,Wpen);
	LineTo(dc,x2,y2);
	LineTo(dc,x,y2);
	SelectObject(dc,penO);
	DeleteObject(Bpen);	DeleteObject(Wpen);
}

void JKSelFrame(HDC dc, int x, int y, int x2, int y2, int pnum)
{
	HPEN  Wpen,penO;
	long Color;

	if (pnum ==1) Color = 0x000000FF;
	else Color = 0x0000FF00;
	Wpen = CreatePen(PS_SOLID,2,Color);
	penO = SelectObject(dc,Wpen);
	MoveToEx(dc,x+1,y2,NULL);
	LineTo(dc,x+1,y+1);
	LineTo(dc,x2,y+1);
	LineTo(dc,x2,y2);
	LineTo(dc,x+1,y2);
	SelectObject(dc,penO);
	DeleteObject(Wpen);
}

char GetOBM ( char ob )
{
	if ((ob > -1) && (ob <= MaxObjects)) return (GetOBMArray[ob]);
	else return(1);
}

void LoadLastLevel()
{
	char temps[11];
	THSREC TempHSData;

	if (CurLevel < 2) return;
	GameOn(FALSE);
	F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (F1 == INVALID_HANDLE_VALUE)
	{
		MessageBox(MainH,txt001,txt007,MB_OK | MB_ICONERROR);
		PostMessage(MainH,WM_COMMAND,108,0);
		return;
	}
	F2 = CreateFile(HFileName,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	CurLevel--;
	do {
		CurLevel--;
		SetFilePointer(F1, CurLevel * sizeof(TLEVEL), NULL, FILE_BEGIN);
		ReadFile(F1, &CurRecData, sizeof(TLEVEL), &BytesMoved, NULL);
		if (BytesMoved < sizeof(TLEVEL))
		{
			CloseHandle(F1); if (F2 != INVALID_HANDLE_VALUE) CloseHandle(F2);
			SendMessage(MainH,WM_GameOver,0,0);
			return;
		}
		if ( SkipCL && (F2 != INVALID_HANDLE_VALUE))
		{
			SetFilePointer(F2, CurLevel * sizeof(THSREC), NULL, FILE_BEGIN);
			ReadFile(F2,&TempHSData,sizeof(THSREC), &BytesMoved, NULL);
			if (BytesMoved < sizeof(THSREC)) TempHSData.moves = 0;
			if (TempHSData.moves > 0) CurRecData.SDiff = 128; // Error SDiff
		}
	} while ((CurLevel > 0) && (CurRecData.SDiff > 0) && ((Difficulty & CurRecData.SDiff) == 0));
	CloseHandle(F1); if (F2 != INVALID_HANDLE_VALUE) CloseHandle(F2);
	// Load For Real
	LoadNextLevel(TRUE,FALSE);
}

BOOL LoadNextLevel(int DirectLoad, int Scanning)
// Directload is true if we shouldn't use difficulties & Completed Level check
// Scanning is true if we are searching and dont want any errors displayed
{
	char temps[11];
	THSREC TempHSData;
	int SavedLevelNum;

	if (GameInProg)
	{
		SavedLevelNum = MessageBox(MainH,txt039,txt038,MB_YESNOCANCEL | MB_ICONQUESTION);
		if (SavedLevelNum == IDCANCEL) return(FALSE);
		if (SavedLevelNum == IDYES)	SendMessage(MainH,WM_SaveRec,0,0);
	}
	GameOn(FALSE);
	SavedLevelNum = CurLevel;
	if ( Difficulty == 0) SendMessage(MainH,WM_COMMAND,225,0);	// Set Difficulty Window
	if ( Recording ) SendMessage(MainH,WM_COMMAND,123,0);		// Turn Off Recording if on
	F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (F1 == INVALID_HANDLE_VALUE)
	{
		MessageBox(MainH,txt001,txt007,MB_OK | MB_ICONERROR);
		PostMessage(MainH,WM_COMMAND,108,0);
		return FALSE;
	}
	F2 = CreateFile(HFileName,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	do {
		SetFilePointer(F1, CurLevel * sizeof(TLEVEL), NULL, FILE_BEGIN);
		ReadFile(F1, &CurRecData, sizeof(TLEVEL), &BytesMoved, NULL);
		if (BytesMoved < sizeof(TLEVEL))
		{
			CloseHandle(F1); if (F2 != INVALID_HANDLE_VALUE) CloseHandle(F2);
			CurLevel = SavedLevelNum;				// If eof use last Level Number
			if (!Scanning) SendMessage(MainH,WM_GameOver,0,0);
			return FALSE;
		}
		if ( SkipCL && (F2 != INVALID_HANDLE_VALUE)  && (!DirectLoad) )
		{
			SetFilePointer(F2, CurLevel * sizeof(THSREC), NULL, FILE_BEGIN);
			ReadFile(F2,&TempHSData,sizeof(THSREC), &BytesMoved, NULL);
			if (BytesMoved < sizeof(THSREC)) TempHSData.moves = 0;
			if (TempHSData.moves > 0) CurRecData.SDiff = 128; // Error SDiff
		}
		CurLevel++;
	} while ((!DirectLoad) && (CurRecData.SDiff > 0) && ((Difficulty & CurRecData.SDiff) == 0));
	CloseHandle(F1);
	if (F2 != INVALID_HANDLE_VALUE) CloseHandle(F2);
	//Game.PF = CurRecData.PF;
	memcpy(Game.PF,CurRecData.PF,sizeof(TPLAYFIELD));
	BuildBMField();
	GameOn(TRUE);
	OKtoHS = TRUE;
	OKtoSave = TRUE;
	EnableMenuItem(MMenu,110,MF_BYCOMMAND | MF_GRAYED); // disable Undo
	EnableMenuItem(MMenu,112,MF_BYCOMMAND | MF_GRAYED); // disable Restore
	SetButtons(0xFA);
	FindTank = TRUE;
	SetTimer(MainH,1,700,NULL);
	InvalidateRect(MainH,NULL,TRUE);
	ResetUndoBuffer();
	if (RLL)
	{
		WritePrivateProfileString("DATA",psRLLN,FileName,INIFile);
		WritePrivateProfileString("DATA",psRLLL,itoa(CurLevel,temps,10),INIFile);
	}
	Game.RecP = 0;
	RB_TOS = 0;
	SlideT.s = 0;							// Just in case
	SlideO.s = 0;
	SlideMem.count = 0; // MGY
	if (ARecord && ( !PBOpen)) SendMessage(MainH,WM_COMMAND,123,0);
	if (Backspace[BS_SP] != CurLevel)		// if CurLevel != LastLevel
	{
		if (Backspace[BS_SP]) EnableMenuItem(MMenu,118,MF_BYCOMMAND ); // Enable Menu Item
		BS_SP++;
		if (BS_SP > 9) BS_SP = 0;
		Backspace[BS_SP] = CurLevel;		// Add CurLevel to stack
	}
	return(TRUE);
}

void AssignHSFile()
{
	char *P;

	strcpy(GHFileName,FileName);
	strcpy(strrchr(GHFileName,'.'),".ghs");
	strcpy(HFileName,FileName);
	strcpy(strrchr(HFileName,'.'),".hs");

	// Set up Record Default file name
	strcpy(PBFileName,FileName);
	P = strrchr(PBFileName,'.');				// Set Up to truncate file name
	if (P)  P[0] = 0;						// Set end of string
	strcat(PBFileName,"_0000.lpb");			// add to name
}

void CheckHighScore()
{
	int x, i;

	if (!OKtoHS) return;
	F2 = CreateFile(HFileName,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,NULL,
		OPEN_ALWAYS, FILE_FLAG_RANDOM_ACCESS,NULL);

	if (F2 == INVALID_HANDLE_VALUE)	FileError();
	// Check if All lower scores are filled in, if not then set levels
	if ((CurLevel * sizeof(THSREC)) > (i = SetFilePointer(F2,0,NULL,FILE_END)))
	{
		HS.moves = 0;			// Sets field to blank
		for (x=(i / sizeof(THSREC)); x < CurLevel-1; x++)
			WriteFile(F2,&HS,sizeof(THSREC),&BytesMoved,NULL);
	}
	SetFilePointer(F2,(CurLevel-1) * sizeof(THSREC),NULL,FILE_BEGIN);
	ReadFile(F2,&HS,sizeof(THSREC),&BytesMoved,NULL);
	if ((HS.moves == 0) || (Game.ScoreMove < HS.moves) || ((Game.ScoreMove == HS.moves) && (Game.ScoreShot < HS.shots)))
	{
		SendMessage(MainH,WM_NewHS,0,0);
		SetFilePointer(F2,(CurLevel - 1) * sizeof(THSREC),NULL,FILE_BEGIN);
		WriteFile(F2,&HS,sizeof(THSREC),&BytesMoved,NULL);
	}
	CloseHandle(F2);
}

void Animate()
{
	int x,y;

	AniLevel++;
	AniCount = 0;
	if (AniLevel == 3) AniLevel = 0;
	for (x = 0; x<16; x++) for (y = 0; y<16; y++)
	{
		// Animate Conveyor Belts & Flag if under something
		switch (Game.PF2[x][y])
		{
		case 2:		Game.BMF2[x][y] = 6 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 15:	Game.BMF2[x][y] = 24 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 16:	Game.BMF2[x][y] = 27 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 17:	Game.BMF2[x][y] = 30 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 18:	Game.BMF2[x][y] = 33 + AniLevel;
					UpDateSprite(x,y);
					break;
		}
		// Now animate all top sprites
		switch (Game.PF[x][y])
		{
		case 2:		Game.BMF[x][y] = 6 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 3:		Game.BMF[x][y] = 9 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 7:		Game.BMF[x][y] = 16 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 8:		Game.BMF[x][y] = 36 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 9:		Game.BMF[x][y] = 39 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 10:	Game.BMF[x][y] = 42 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 15:	Game.BMF[x][y] = 24 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 16:	Game.BMF[x][y] = 27 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 17:	Game.BMF[x][y] = 30 + AniLevel;
					UpDateSprite(x,y);
					break;
		case 18:	Game.BMF[x][y] = 33 + AniLevel;
					UpDateSprite(x,y);
					break;
		}
	}
	TankDirty = TRUE;
}

void TranslateTunnel( int *x, int *y )
{
	int cx,cy,i;
	char bb;

	bb = Game.PF[*x][*y];		// bb is ID #
	WaitToTrans = FALSE;
	BlackHole = FALSE;
	for (cy=0; cy < 16; cy ++) for (cx = 0; cx < 16; cx ++)
		if ((Game.PF[cx][cy] == bb) && (!((*x == cx) && (*y == cy))))
		{
			*x = cx;			// We found an exit YEA !!!
			*y = cy;
			return;
		}
	// check for blocked hole - something is over the exit
	// Scan the 2nd layer any matches are blocked holes
	for (cy=0; cy < 16; cy ++) for (cx = 0; cx < 16; cx ++)
		if ((Game.PF2[cx][cy] == bb) && (!((*x == cx) && (*y == cy))))
		{
			// We found one so we will set the flag
			WaitToTrans = TRUE;
			return;						// Blocked so no warp
		}
	// There is no match, so it is a black hole
	BlackHole = TRUE;
}


void ConvMoveTank(int x, int y, int check)
{
	UpDateSprite(Game.Tank.X,Game.Tank.Y);
	Game.Tank.Y += y;
	Game.Tank.X += x;
	if (ISTunnel(Game.Tank.X,Game.Tank.Y))
	{
		TranslateTunnel(&Game.Tank.X,&Game.Tank.Y);		// We moved into a tunnel
		if (BlackHole) PostMessage(MainH,WM_Dead,0,0);	// The tunnel was a black hole
	}
	if (WaitToTrans) Game.Tank.Good = TRUE;
	TankDirty = TRUE;
	ConvMoving = TRUE;
	if (wasIce && check)
	{
		SlideT.x = Game.Tank.X;
		SlideT.y = Game.Tank.Y;
		SlideT.s = TRUE;
		SlideT.dx = x;
		SlideT.dy = y;
	}
	AntiTank();
}

void UpDateTankPos(int x, int y)
{
	char temps[50];

	SoundPlay(S_Move);
	UpdateUndo();
	Game.ScoreMove++;
	SetTextAlign(gDC,TA_CENTER);
	SetTextColor(gDC,0x0000FF00);
	SetBkColor(gDC,0);
	TextOut(gDC,ContXPos+48,207,itoa(Game.ScoreMove,temps,10),strlen(temps));
	UpDateSprite(Game.Tank.X,Game.Tank.Y);
	Game.Tank.Y += y;
	Game.Tank.X += x;
	Game.Tank.Good = FALSE;				// we need it somewhere if we move off a tunnel
	if (ISTunnel(Game.Tank.X,Game.Tank.Y))
	{
		TranslateTunnel(&Game.Tank.X,&Game.Tank.Y);		// We moved into a tunnel
		if (BlackHole) PostMessage(MainH,WM_Dead,0,0);	// The tunnel was a black hole
	}
	if (WaitToTrans) Game.Tank.Good = TRUE;
	TankDirty = TRUE;
}

void MoveTank(int d)
{
	if (Game.Tank.Dir != d)				// The Tank is Turning
	{
		Game.Tank.Dir = d;
		UpDateSprite(Game.Tank.X,Game.Tank.Y);
		TankDirty = TRUE;
		SoundPlay(S_Turn);
		return;
	}
	switch (d)
	{
	case 1:	if (CheckLoc(Game.Tank.X,Game.Tank.Y-1)) UpDateTankPos(0,-1);
			else SoundPlay(S_Head);		// Ouch we are hitting something hard
			SlideT.dy = -1;	SlideT.dx = 0;
			break;
	case 2:	if (CheckLoc(Game.Tank.X+1,Game.Tank.Y)) UpDateTankPos(1,0);
			else SoundPlay(S_Head);
			SlideT.dy = 0; SlideT.dx = 1;
			break;
	case 3:	if (CheckLoc(Game.Tank.X,Game.Tank.Y+1)) UpDateTankPos(0,1);
			else SoundPlay(S_Head);
			SlideT.dy = 1; SlideT.dx = 0;
			break;
	case 4:	if (CheckLoc(Game.Tank.X-1,Game.Tank.Y)) UpDateTankPos(-1,0);
			else SoundPlay(S_Head);
			SlideT.dy = 0; SlideT.dx = -1;
			break;
	}
	if (wasIce)
	{
		SlideT.x = Game.Tank.X;
		SlideT.y = Game.Tank.Y;
		SlideT.s = TRUE;
	}
}

int CheckLoc(int x, int y)
{
	// Check if Tank can move
	if ((x<0) || (x>15) || (y<0) || (y>15)) return(0);
	wasIce = ((Game.PF[x][y] == Obj_Ice) || (Game.PF[x][y] == Obj_ThinIce));
	if ((Game.PF[x][y] & Obj_Tunnel) == Obj_Tunnel) return(TRUE);
	return(CheckArray[Game.PF[x][y]]);
}

void MoveObj(int x, int y, int dx, int dy, int sf)
// used by CheckLLoc
{
	int obt, bm, i, bb, cx, cy, ok;

	obt = Game.PF[x][y];								// Get Object type
	bm = Game.BMF[x][y];
	if ((Game.PF2[x][y] & Obj_Tunnel) == Obj_Tunnel)	// Check if Tunnel
	{
		bb = Game.PF2[x][y] | 1;						// bb is ID # w/ 1 set
		ok = FALSE;
		for (cy=0; cy < 16; cy ++) for (cx = 0; cx < 16; cx ++)
			if ((Game.PF2[cx][cy] == bb) && (!((x == cx) && (y == cy))))
			{
				ok = TRUE;
				goto MoveObj1;
				// Ok if something wants to move here; cx & cy set to orig
			}
MoveObj1:
		if (ok)										// We are Moving an Object
		{
			Game.PF[x][y] = Game.PF[cx][cy];		// Transfer Blocked Object
			Game.BMF[x][y] = Game.BMF[cx][cy];
			Game.PF[cx][cy] = Game.PF2[cx][cy] & 0xFE;	// Return Saved State
			Game.PF2[cx][cy] = 0;
			Game.BMF[cx][cy] = Game.BMF2[cx][cy];
			UpDateSprite(cx,cy);
		}
		else {										// Not Blocked Anymore
			Game.PF[x][y] = Game.PF2[x][y] & 0xFE;	// Return Saved State strip
			Game.PF2[x][y] = 0;
			Game.BMF[x][y] = Game.BMF2[x][y];
			// We didn't find a match so maybe the tank is it
			if ((Game.PF[Game.Tank.X][Game.Tank.Y] == (bb & 0xFE)) && Game.Tank.Good)
			{
				Game.ScoreMove--; // MGY - 2003/05/18 - v408b15 -  Bartok Bug.lvl
				UpDateTankPos(0,0);
				UndoP--;
			}
		}
	}
	else {			// If not a tunnel
		Game.PF[x][y] = Game.PF2[x][y];				 	// Return Saved State
		Game.PF2[x][y] = 0;
		Game.BMF[x][y] = Game.BMF2[x][y];
	}
	UpDateSprite(x,y);
	x += dx;
	y += dy;
	if (ISTunnel(x,y))
	{
		TranslateTunnel(&x,&y);		// We moved into a tunnel
		if (BlackHole) return;		// The tunnel was a black hole
	}
	else WaitToTrans = FALSE;

	Game.PF2[x][y] = Game.PF[x][y];  				// Save Return State
	if (WaitToTrans) Game.PF2[x][y] |= 1;			// Set bit 1 if we are waiting to transport
	Game.BMF2[x][y] = Game.BMF[x][y];
	if (Game.PF[x][y] != 3)
	{
		Game.PF[x][y] = obt;
		Game.BMF[x][y] = bm;
	}
	else
	{
		sf = S_Sink;
		if (obt == 5)
		{
			Game.PF[x][y] = 0;
			Game.PF2[x][y] = 0;  // Pushing Block into Water }
			Game.BMF[x][y] = 19;
			Game.BMF2[x][y] = 19;
		}
	}
	UpDateSprite(x,y);
	if ((x == Game.Tank.X) && (y == Game.Tank.Y)) TankDirty = TRUE;
	SoundPlay(sf);
}

void IceMoveT()					// Move the tank on the Ice
{
	int savei;


	if (Game.PF[SlideT.x][SlideT.y] == Obj_ThinIce)
	{
		Game.BMF[SlideT.x][SlideT.y] = 9;
		Game.PF[SlideT.x][SlideT.y] = Obj_Water;	// Ice to Water
		UpDateSprite(SlideT.x,SlideT.y);
	}

	if (CheckLoc(SlideT.x + SlideT.dx, SlideT.y + SlideT.dy))
	{
		savei = wasIce;
		ConvMoveTank(SlideT.dx,SlideT.dy,FALSE);		// use this insted of UpDateTank
	}
	else {
		SlideT.s = FALSE;
		return;
	}

	SlideT.x += SlideT.dx;					// Update Position
	SlideT.y += SlideT.dy;					// Update Position
	if (!savei) SlideT.s = FALSE;			// The ride is over
}


void IceMoveO()					// Move an Object on the Ice
{
	int savei;
    int iSlideObj; // MGY

	for ( iSlideObj = SlideMem.count; iSlideObj>= 1; iSlideObj--) // MGY
    {
		Mem_to_SlideO( iSlideObj ); // Get from memory

		if ( iSlideObj <= SlideMem.count ) // just in case ... MGY
			{

			if (Game.PF2[SlideO.x][SlideO.y] == Obj_ThinIce)
			{
				Game.BMF2[SlideO.x][SlideO.y] = 9;
				Game.PF2[SlideO.x][SlideO.y] = Obj_Water;	// Ice to Water
			}

			if (CheckLoc(SlideO.x + SlideO.dx, SlideO.y + SlideO.dy) &&
			    (!((SlideO.x + SlideO.dx == Game.Tank.X) && (SlideO.y + SlideO.dy == Game.Tank.Y))))
			{
				savei = wasIce;
				MoveObj(SlideO.x,SlideO.y,SlideO.dx,SlideO.dy,S_Push2);
				AntiTank();

				SlideO.x += SlideO.dx;					// Update Position
				SlideO.y += SlideO.dy;					// Update Position
				if (!savei) {
			   		SlideO.s = FALSE;			// The ride is over
					SlideO_to_Mem( iSlideObj ); // update memory
					sub_SlideO_from_Mem( iSlideObj );
				}
				else {
					SlideO_to_Mem( iSlideObj ); // update memory
				}
			}
			else {

				if (Game.PF2[SlideO.x][SlideO.y] == Obj_Water)
					MoveObj(SlideO.x,SlideO.y,0,0,0);		// Drop Object in the water (was thin ice)
				SlideO.s = FALSE;
				SlideO_to_Mem( iSlideObj ); // update memory
				sub_SlideO_from_Mem( iSlideObj );
				AntiTank();										// incase an anti-tank is behind a block
				//return; // MGY
			}
		}


	}

	Mem_to_SlideO( SlideMem.count ); // Get from memory the last object of the list
	SlideO.s = ( SlideMem.count > 0 );
}

void KillAtank( int x, int y, char bm)
// used by CheckLLoc
{
	Game.PF[x][y] = 4; // Solid Object}
	Game.BMF[x][y] = bm; // Junk Bitmap}
	UpDateSprite(x,y);
	SoundPlay(S_Anti1);
}

int CheckLLoc(int x, int y, int dx, int dy)
// this is were the laser does it's damage
// returns true if laser didn't hit anything
{
	if ((x<0) || (x>15) || (y<0) || (y>15))
	{
		return(FALSE);
	}
	if ((x == Game.Tank.X) && (y == Game.Tank.Y))
	{
		SendMessage(MainH,WM_Dead,0,0);
		return(FALSE);
	}
	wasIce = FALSE;
	switch (Game.PF[x][y])
	{
	case 0:
	case 2:
	case 3:
	case 15:
	case 16:
	case 17:
	case 18:	return(TRUE);
	case 4:		SoundPlay(S_LaserHit);
				break;
	case 5:	if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push1);
			else SoundPlay(S_LaserHit);
			break;
	case 6:	Game.PF[x][y] = 0;
			Game.BMF[x][y] = 1;
			UpDateSprite(x,y);
			SoundPlay(S_Bricks);
			break;
	case 7:	if (dy == 1) { KillAtank(x,y,54); return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 8:	if (dx == -1) { KillAtank(x,y,52);return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 9:	if (dy == -1) { KillAtank(x,y,12);return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 10: if (dx == 1) { KillAtank(x,y,53);return(FALSE); }
			else if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push3);
			else SoundPlay(S_LaserHit);
			break;
	case 11: if ((laser.Dir == 2) || (laser.Dir == 3)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 12: if ((laser.Dir == 3) || (laser.Dir == 4)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 13: if ((laser.Dir == 1) || (laser.Dir == 4)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 14: if ((laser.Dir == 1) || (laser.Dir == 2)) return(TRUE);
			if (CheckLoc(x+dx,y+dy)) MoveObj(x,y,dx,dy,S_Push2);
			else SoundPlay(S_LaserHit);
			break;
	case 19: if (laser.Good) PutSprite(46,XOffset+(x*SpBm_Width),YOffset + (y*SpBm_Height));
			else PutSprite(51,XOffset+(x*SpBm_Width),YOffset + (y*SpBm_Height));
			return(TRUE);
	case 20: if ((laser.Dir == 2) || (laser.Dir == 3)) return(TRUE);
			Game.PF[x][y] = 21;	Game.BMF[x][y] = 48;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 21: if ((laser.Dir == 3) || (laser.Dir == 4)) return(TRUE);
			Game.PF[x][y] = 22;	Game.BMF[x][y] = 49;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 22: if ((laser.Dir == 1) || (laser.Dir == 4)) return(TRUE);
			Game.PF[x][y] = 23;	Game.BMF[x][y] = 50;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 23: if ((laser.Dir == 1) || (laser.Dir == 2)) return(TRUE);
			Game.PF[x][y] = 20;	Game.BMF[x][y] = 47;
			UpDateSprite(x,y);
			SoundPlay(S_Rotate);
			break;
	case 24:	// Ice
	case 25:	// thin Ice
			return(TRUE);
	default: if (ISTunnel(x,y)) return(TRUE);
	}
	if (wasIce)
	{
		// If is already sliding, del it !
		del_SlideO_from_Mem( x, y);
		// and add a new slide in a new dirrection
		SlideO.x = x+dx;
		SlideO.y = y+dy;
		SlideO.s = TRUE;
		SlideO.dx = dx;
		SlideO.dy = dy;
		add_SlideO_to_Mem();
	}
	// MGY
	else {
	    // SlideO.s = FALSE;		// in case we side hit off of the ice
		del_SlideO_from_Mem( x, y);
	}
	return (FALSE);
}

void MoveLaser()
{
	int x,y,oDir;

LaserMoveJump:
	LaserBounceOnIce = 0;
	x = 0;
	y = 0;
	switch (laser.Dir)
	{
	case 1:	y = -1;
		break;
	case 2:	x = +1;
		break;
	case 3:	y = +1;
		break;
	case 4:	x = -1;
	}
	if (CheckLLoc(laser.X+x,laser.Y+y,x,y))
	{
		if (laser.Firing) UpDateSprite(laser.X,laser.Y);
		laser.Y += y;
		laser.X += x;
		if (((Game.PF[laser.X][laser.Y] > 10) && (Game.PF[laser.X][laser.Y] < 15))
			    || ((Game.PF[laser.X][laser.Y] > 19) && (Game.PF[laser.X][laser.Y] < 24)))
		{
			oDir = laser.Dir;
			switch (Game.PF[laser.X][laser.Y])
			{
			case 11:
			case 20:	if (laser.Dir == 2) laser.Dir = 1;
						else laser.Dir = 4;
						break;
			case 12:
			case 21:	if (laser.Dir == 3) laser.Dir = 2;
						else laser.Dir = 1;
						break;
			case 13:
			case 22:	if (laser.Dir == 1) laser.Dir = 2;
						else laser.Dir = 3;
						break;
			case 14:
			case 23:	if (laser.Dir == 1) laser.Dir = 4;
						else laser.Dir = 3;
			}
			UpDateLaserBounce(oDir,laser.Dir);
			SoundPlay(S_Deflb);
		}
		else UpDateLaser();
		laser.Firing = TRUE;
	} else {
		Game.Tank.Firing = FALSE;
		if (laser.Firing) UpDateSprite(laser.X,laser.Y);
		if (Game_On || VHSOn) AntiTank();

		// SpeedBug - MGY - 22-11-2002
		if ( TestIfConvCanMoveTank() )
			ConvMoving = TRUE;
	}
	if (LaserBounceOnIce) goto LaserMoveJump;
}

void FireLaser(int x, int y, int d, int sf)
{
	char temps[30];

	Game.Tank.Firing = TRUE;
	laser.Dir = d;
	laser.X = x;
	laser.Y = y;
	laser.Firing = FALSE; // true if laser has been moved
	SoundPlay(sf);
	SetTextAlign(gDC,TA_CENTER);
	SetTextColor(gDC,0x0000FF00);
	SetBkColor(gDC,0);
	itoa(Game.ScoreShot,temps,10);   // we incremented it in wm_timer
	TextOut(gDC,ContXPos+134,207,temps,strlen(temps));
	if (sf == 2) LaserColor = LaserColorG;
	else LaserColor = LaserColorR;
	laser.Good = ( sf == 2);
	MoveLaser();
}

void AntiTank()
{
	int x,y;
	// Program Anti tank seek }

	if (Game.Tank.Firing) return;

	x = Game.Tank.X;	// Look to the right
	while (CheckLoc(x,Game.Tank.Y)) x++;
	if ((x<16) && (Game.PF[x][Game.Tank.Y] == 10) && (Game.Tank.X != x))
	{
		FireLaser(x,Game.Tank.Y,4,S_Anti2);
		return;
	}
	x = Game.Tank.X;	// Look to the left
	while (CheckLoc(x,Game.Tank.Y)) x--;
	if ((x>=0) && (Game.PF[x][Game.Tank.Y] == 8) && (Game.Tank.X != x))
	{
		FireLaser(x,Game.Tank.Y,2,S_Anti2);
		return;
	}
	y = Game.Tank.Y;	// Look Down
	while (CheckLoc(Game.Tank.X,y)) y++;
	if ((y<16) && (Game.PF[Game.Tank.X][y] == 7) && (Game.Tank.Y != y))
	{
		FireLaser(Game.Tank.X,y,1,S_Anti2);
		return;
	}
	y = Game.Tank.Y;	// Look Up
	while (CheckLoc(Game.Tank.X,y)) y--;
	if ((y>=0) && (Game.PF[Game.Tank.X][y] == 9) && (Game.Tank.Y != y))
	{
		FireLaser(Game.Tank.X,y,3,S_Anti2);
		return;
	}
}

void PutSelectors()
{
	int x,y,i,j;

	x = ContXPos + 5; y = 260; j = 1;
	for (i=0; i <= MaxObjects; i++)
	{
		JK3dFrame(gDC,x-1,y-1,x+SpBm_Width,y+SpBm_Height,TRUE);
		if (i == CurSelBM_L) JKSelFrame(gDC,x-1,y-1,x+SpBm_Width,y+SpBm_Height,1);
		if (i == CurSelBM_R) JKSelFrame(gDC,x-1,y-1,x+SpBm_Width,y+SpBm_Height,2);
		PutSprite(GetOBM(i),x,y);
		x += SpBm_Width+4;
		j++;
		if (j > EditBMWidth)
		{
			x = ContXPos + 5; y +=SpBm_Height+4; j = 1;
		}
	}
}

void ShowTunnelID()
{
	int x,y;
	char temps[20];

	// scan and add Tunnel ID #s
	SetBkMode(gDC,OPAQUE);
	SetTextAlign(gDC,TA_LEFT);
	SetTextColor(gDC,0);
	for (x=0; x < 16; x++) for (y=0; y < 16; y++)
		if (ISTunnel(x,y))
		{
			sprintf(temps,"(%1d)",GetTunnelOldID(x,y));
			TextOut(gDC,22 + (x * SpBm_Width),20 + (y * SpBm_Height),temps,strlen(temps));
		}
}

void SetGameSize(int i)
{
	char temps[11];
	CheckMenuItem(MMenu,120,0);
	CheckMenuItem(MMenu,121,0);
	CheckMenuItem(MMenu,122,0);
	CheckMenuItem(MMenu,119+i,MF_CHECKED);
	itoa(i,temps,10);
	WritePrivateProfileString("SCREEN",psSize,temps,INIFile);
	if (GFXOn) GFXKill;
	switch (i)
	{
	case 1:
		SpBm_Width = 24;
		SpBm_Height = 24;
		ContXPos = 419;
		EditBMWidth = 6;
		SetWindowPos(MainH,0,0,0,ContXPos+190,463,SWP_NOMOVE | SWP_NOZORDER);
		LaserOffset = 10;
		break;

	case 2:
		SpBm_Width = 32;
		SpBm_Height = 32;
		ContXPos = 550;
		EditBMWidth = 5;
		SetWindowPos(MainH,0,0,0,ContXPos+190,591,SWP_NOMOVE | SWP_NOZORDER);
		LaserOffset = 13;
		break;

	case 3:
		SpBm_Width = 40;
		SpBm_Height = 40;
		ContXPos = 680;
		EditBMWidth = 4;
		SetWindowPos(MainH,0,0,0,ContXPos+190,719,SWP_NOMOVE | SWP_NOZORDER);
		LaserOffset = 17;
		break;

	}
	SetWindowPos(Ed1,0,ContXPos+9,99,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(Ed2,0,ContXPos+9,148,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT1,0,ContXPos+15,255,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT2,0,ContXPos+15,280,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT3,0,ContXPos+15,305,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT4,0,ContXPos+15,330,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT5,0,ContXPos+15,380,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT6,0,ContXPos+90,380,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT7,0,ContXPos+90,255,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT8,0,ContXPos+90,330,0,0,SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(BT9,0,ContXPos+15,355,0,0,SWP_NOSIZE | SWP_NOZORDER);
	GraphM = GetPrivateProfileInt("SCREEN",psGM,0,INIFile);
	if (GraphM == 2)
		GetPrivateProfileString("SCREEN",psGFN,"",GraphFN,MAX_PATH,INIFile);
	GetPrivateProfileString("SCREEN",psGDN,"",GraphDN,MAX_PATH,INIFile);
	if (strlen(GraphDN) == 0)
	{
		GetCurrentDirectory(MAX_PATH,GraphDN);	//We only do this once
		WritePrivateProfileString("SCREEN",psGDN,GraphDN,INIFile);
	}
	GFXInit();
}


void SavePBFile()
{
	HANDLE Book;
	char temps[60];
	char SaveAuthor[31];

	// Get Hs.name
	GetPrivateProfileString("DATA",psUser,"",temps,5,INIFile);
    if (stricmp(temps,HS.name) != 0)
	{
        strcpy(HS.name,temps);
        WritePrivateProfileString("DATA",psUser,HS.name,INIFile);
    }

	if ( temps[0] != '\0' )
	{
		// Fill the name with spaces
		strcat( temps,"      ");
		temps[4] = '-';
		temps[5] = '\0';
	}
	// Add The Author's name
	strcat(temps,PBSRec.Author);
	// Limit the size
	temps[30] = '\0';


	// Save	PBSRec
    strcpy(SaveAuthor,PBSRec.Author);
	strcpy(PBSRec.Author,temps);

	// Write file
	Book = CreateFile (PBFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);// Create New PlayBack File
	WriteFile(Book,&PBSRec,sizeof(PBSRec),&BytesMoved,NULL);				 // Save Header Info
	WriteFile(Book,RecBuffer,PBSRec.Size,&BytesMoved,NULL);					 // Data
	CloseHandle(Book);

	// restore 	PBSRec
    strcpy(PBSRec.Author,SaveAuthor);
}


