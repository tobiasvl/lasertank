(*******************************************************
 **             Laser Tank ver 2.0a                   **
 **               By Jim Kindley                      **
 **               (c) 1998,1997,1996                  **
 **                  Sound Unit                       **
 *******************************************************)

 { All Sounds are Wav files loaded into the lt_sfx.res resource file.
   I created a new type of resource in the resource workshop, called "wSound".
   Then you can compile the wave files into the resource
}
unit lt_sfx;
 
interface
{$R lt_sfx.res }
uses mmsystem,winprocs,wintypes;

const
 MaxSounds         = 20; { the maximum number of Sounds }
 LastSFWord : word = 0;
 SFXError: Word    = 0;
 snd_NoStop        = $0010;  { don't stop any currently playing sound }
 ResTypeName       = 'wSound';
 Sound_On: Boolean = true;   { true when Sound is On }

var
 SFx: Array[1..MaxSounds] of THandle;

 Procedure SoundPlay( sn: word);  { SoundPlay(5) Play sound 5 }
 Procedure SFxInit;                      { Load all sounds }
 Procedure SoundKill;                    { This Unloads All Data }

implementation

Procedure SoundPlay( sn: Word);
var p: pointer;
begin
 if Not Sound_On then exit;
 p := LockResource(SFx[sn]);
 sndplaySound(p,5);  { 5 = memory & async }
 UnLockResource(SFx[sn]);
end;

Procedure SoundLoad(S: pChar);
var h: Thandle;
begin
  h := LoadResource(hInstance,FindResource(hInstance,s,ResTypeName));
  if h = 0 then SFXError := LastSFWord+1
  else begin
    inc(LastSFWord);
    SFx[LastSFWord] := h; { put resource handle in Sound Array }
  end;
end;

Procedure SFxInit;
begin
 SoundLoad('Explos');
 SoundLoad('Laser'); SoundLoad('Move');
 SoundLoad('Ping'); SoundLoad('Movet');
 SoundLoad('EndLev'); SoundLoad('Die');
 if SFXError <> 0 then messagebox(0,'Loading Error','Error',mb_ok);
end;

Procedure SoundKill;
          { run this to release all the memory used to store the sounds }
var i: word;
begin
 For i := 1 to LastSFWord do FreeResource(SFx[i]);
 LastSFWord := 0;
end;

end.