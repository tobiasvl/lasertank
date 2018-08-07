/*******************************************************
 **             LaserTank ver 4.0                     **
 **               By Jim Kindley                      **
 **               (c) 2001                            **
 **                  Sound Unit                       **
 *******************************************************)

  All Sounds are Wav files loaded into the resource file.
  Because lcc-win32 can't handle user defined resources, the wav files
  are converted into a charecter stream and imported as RCData
*/
#include <windows.h>            // required for all Windows applications
#include <mmsystem.h>
#include "lt_sfx.h"            // prototypes specific to this application

HGLOBAL SFx[MaxSounds];
int LastSFWord	= 0;
int SFXError = 0;
int Sound_On = 1;				// true when sound is on
extern HINSTANCE hInst;			// Globally Defined Instance


// Declare Local Function
static void SoundLoad ( char *s );

void SoundPlay( int sn)
{
char *p;
 if (!Sound_On) return;
 if (SFXError) return;
 p = LockResource(SFx[sn]);
 PlaySound(p,0,5);  // 5 = memory & async
}

static void SoundLoad(char *s)
{
HGLOBAL h;

  if (SFXError) return;
  h = LoadResource(hInst,FindResource(hInst, s, RT_RCDATA));
  if (h == 0)
		SFXError = LastSFWord+1;
  else {
    LastSFWord++;
    SFx[LastSFWord] = h; // put resource handle in Sound Array
  }
}

void SFxInit( void)
{
 SoundLoad("BRICKS"); SoundLoad("FIRE");
 SoundLoad("MOVE"); SoundLoad("HEAD");
 SoundLoad("TURN"); SoundLoad("ENDLEV");
 SoundLoad("DIE"); SoundLoad("ANTI1");
 SoundLoad("ANTI2"); SoundLoad("DEFLB");
 SoundLoad("LASER2"); SoundLoad("PUSH2");
 SoundLoad("PUSH1"); SoundLoad("ROTATE");
 SoundLoad("PUSH3"); SoundLoad("SINK");
 if (SFXError) MessageBox(0,"Loading Error","SFx Error",MB_OK);
}


