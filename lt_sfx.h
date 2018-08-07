/*******************************************************
 **             LaserTank ver 4.0                     **
 **               By Jim Kindley                      **
 **               (c) 2001                            **
 **         The Program and Source is Public Domain   **
 *******************************************************/

 // This is the header file for the Sound unit of Lasertank


#define	 	MaxSounds	20

typedef enum 	LT_Sound_Types {	S_Bricks = 1, S_Fire, S_Move, S_Head, S_Turn,
					S_EndLev, S_Die, S_Anti1, S_Anti2, S_Deflb,
					S_LaserHit, S_Push2, S_Push1, S_Rotate, S_Push3,
					S_Sink };

extern int Sound_On;

// Function Prototypes

void SoundPlay ( int );
void SFxInit ( void );

