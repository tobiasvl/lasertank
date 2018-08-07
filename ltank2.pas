(*******************************************************
 **             Laser Tank ver 3.1.8                 **
 **               By Jim Kindley                      **
 **               (c) 1999,1998,1997,1996             **
 **         The Program and Source is Public Domain   **
 **             Graphics & Varable Unit               **
 *******************************************************)

unit ltank2;

interface
{$R lt_gfx.res}
{$I- }

uses winprocs,wintypes,strings,commdlg,lt_sfx;
(*

Obj	BitM	BMName		Description
---     ----    ------          -----------

0 	1	Dirt		Dirt
1 	2	Tank1		Tank Up ( Primary )
-	3	Tank2		Tank Right
-	4	Tank3		Tank Down
-	5	Tank4		Tank left
2	6	Base1		Base ( Primary)
-	7	Base2		Base Alt ( animation )
-	8	Base3		Base Alt2
3	9	Water1		Water ( Primary )
-	10	Water2		Water Alt
-	11	Water3		Water Alt 2
-       12      Junk            Blown Up Anti Tank
4	13	SolidB		Solid Block NOT movable
5	14	MovableB	Movable Block
6	15	Brick		Bricks
7	16	atank1		Anti-Tank Gun Up
-	17	atank1b		Anti-Tank Alt
-	18	atank1c		Anti-Tank Alt 2
-	19	Bridge          Movable block after pushing into water
11	20	Mirror1		Mirror Up-Lt
12	21	Mirror2		Mirror Up-Rt
13	22	Mirror3		Mirror Dn-Rt
14	23	Mirror4		Mirror Dn-Lt
15	24	OW1A		One Way Up (Primary)
-	25	OW1B		One Way Up Alt
-	26	OW1C		One Way Up Alt2
16	27	OW2A		One Way Rt (Primary)
-	28	OW2B		One Way Rt Alt
-	29	OW2C		One Way Rt Alt2
17	30	OW3A		One Way Dn (Primary)
-	31	OW3B		One Way Dn Alt
-	32	OW3C		One Way Dn Alt2
18	33	OW4A		One Way Lt (Primary)
-	34	OW4B		One Way Lt Alt
-	35	OW4C		One Way Lt Alt2
8       36      atank2          Anti-Tank facing right
-       37      atank2b
-       38      atank2c
9       39      atank3          Anti-Tank facing down
-       40      atank3b
-       41      atank3c
10      42      atank4          Anti-Tank facing left
-       43      atank4b
-       44      atank4c
19      45      crystal         Crystal Block
-       46      crystalh        Crystal when hit
20	47	RMirror1	Roto Mirror Up-Lt
21	48	RMirror2	Roto Mirror Up-Rt
22	49	RMirror3	Roto Mirror Dn-Rt
23	50	RMirror4	Roto Mirror Dn-Lt
*)
const
 App_Title            = 'LaserTank';
 REC_Title            = 'LaserTank ** RECORDING **';
 App_Version          = '3.1';
 psRLLOn              = 'RLL';          { Profile String for remember last level}
 psRLLN               = 'RLLFilename';  { remember last level file name }
 psRLLL               = 'RLLLevel';     { Level number }
 psAni                = 'Animation';
 psSound              = 'Sound';
 psSize               = 'Size';         { 1 = small, 3 = large }
 psXpos               = 'PosX';
 psYpos               = 'PosY';
 psUser               = 'Player';
 psARec               = 'Auto_Record';
 psSCL                = 'SkipComLev';
 psDiff               = 'Diff_Setting';
 LevelData            = 'LTANK.LVL';     { Default Level Data File }
 INIFile              = 'LTANK.INI';
 MaxObjects           = 23;             { Maximum number of Objects used }
 MaxBitMaps           = 50;             { the number of Game BitMaps }
 XOffset              = 11;             { Game Board Offset from Left }
 YOffset              = 11;             { Game Board Offset from top }
 ani_delay            = 4;              { animation Delay in GameDelay units }
 GameDelay            = 50;             { Main Operation Loop }
 WM_GameOver          = WM_user +1;     { Send Message if there are no more levels}
 WM_Dead              = WM_user +2;     { Send Message when you die }
 WM_NewHS             = WM_user +3;     { Send to Display New High Score Dialog}
 WM_SaveRec           = WM_user +4;     { Send to Save Recording }
 UndoDepth            = 50;             { How deep the undo buffer is }
 RecBufMax            = 60000;          { Maximum depth of record buffer }
 RecBufStep           = 5000;           { Groth amount of Rec Buffer }
 SlowPBSet            = 5;              { Delay of slow playback setting }

 Error_msg1 : PChar = 'The Level file can not be found.'+
                         #13+#10+'Please select a new one.';
 PCRLF: PChar = #13#10#0;
 LastBMWord : word    = 0;              { The Last BMA Handle }
 GFXError : word      = 0;              { error used for load }
 GFXOn : Boolean      = false;          { true when GFX is Initialized }
 NoLevel: Boolean     = true;           { if true Main Paint will show Openning }
 Game_On: Boolean     = false;          { true when game is running }
 Ani_On: Boolean      = true;           { true when Animation is On }
 RLL: Boolean         = true;           { remember last level }
 ConvMoving :Boolean  = false;          { true when moving on the conveyor belts }
 EditorOn: Boolean    = false;          { true when in editor mode }
 OktoHS: Boolean      = true;           { true if OK to Set High Score }
 Recording: Boolean   = false;          { true if recording }
 PlayBack: Boolean    = false;          { true if Playing Back a Recording }
 PBOpen: Boolean      = false;          { true if Playing Back window is Open }
 ARecord: Boolean     = false;          { true if AutoRecord is enabled }
 SkipCL: Boolean      = false;          { true if Skip Completed Levels is On }

 CurLevel: Word       = 0;              { Used to Figure out the Current Level }
 AniLevel:word        = 0;              { Used for Animation Position }
 AniCount:word        = 0;              { counter for animation }
 CurSelBM_L:integer   = 2;              { current selected bm in editor LEFT Button}
 CurSelBM_R:integer   = 0;              { current selected bm in editor RIGHT}
 LastKeyPress: word   = 0;              { Keyboard buffer }
 SpBm_Width:word      = 24;             { Width of Sprites }
 SpBm_Height:Word     = 24;             { Height of Sprite }
 ContXPos:Word        = 409;            { X position where Control BitMap go's }
 EditBMWidth: word    = 5;              { number of bitmaps across edit select area }
 LaserOffset:word     = 10;             { Offset for laser size }
 Speed:word           = 1;              { Playback speed }
 SlowPB:word          = 1;
 RecBufSize:word      = 5000;           { Size of the Recorder Buffer }
 Difficulty:word      = 0;              { Difficulty enables }

type
 tPlayField = array[0..15,0..15] of Byte; { Matrix of G.O. types }

 tLevel = record                { Level Data from File }
  PF: TPlayField;               { Object Grid }
  LName: Array[0..30] of Char;  { Level Name }
  Hint: Array[0..255] of Char;  { Hint for this level }
  Author: Array[0..30] of Char; { the Author of the Level }
  SDiff: word;                  { Score Difficulty - NOT USED -}
 end;

 tRecordRec = record
  LName: Array[0..30] of Char;  { Level Name }
  Author: Array[0..30] of Char; { the Author of the Recording }
  Level: word;                  { Level Number }
  Size: word;                   { Size of Data  -- Data falows record}
 end;

 tSearch = record
   mode: integer;               { 1 = title, 2 = author }
   data: array[0..60] of Char;  { the string }
 end;

 tLevelFile = File of tLevel;

 tTankRec = record              { Store the Tank & Laser information }
  X: word;
  Y: word;
  Dir: word;
  Firing: Boolean;
 end;

 tGameRec = record
  PF,                     { Store Game Objects }
  PF2,                    { Store Objects Under Stuff ( Ground, conveyor) }
  BMF,                    { Bitmaps for Objects }
  BMF2: TPLayField;       { Bitmaps for Under Stuff ( Bridges ) }
  ScoreMove,              { Move Counter }
  ScoreShot: word;        { Shot Counter }
  RecP: word;             { Recording Pointer }
  Tank: tTankRec;         { Tank Data }
 end;

 tXYRec = record
  X,                      { x position in Game Bitmap }
  Y: word;                { y position in Game Bitmap }
 end;

 tUndoBuf =  array[1..UndoDepth] of tGameRec;
 pUndoBuf = ^tUndoBuf;

 tRecBuf = Array[1..RecBufMax] of Byte;
 pRecBuf = ^tRecBuf;

 tHSRec = record
  moves,
  shots: word;
  name: array[0..5] of Char;
 end;

tHSFile = File of tHSRec;

var
 BMA: Array[1..MaxBitMaps] of tXYRec; { Bit Map Location Array }
 Game,SaveGame: tGameRec;
 BackUp: pUndoBuf;
 PBRec: tRecordRec;
 RecBuffer: pRecBuf;
 UndoP: word;
 CurRecData:TLevel;
 laser: tTankRec;
 File1: TLevelFile;
 File2,File3: THSFile;
 MainH,Ed1,Ed2,PBCountH,PlayH : HWnd;
 LaserColor : HBrush;
 MyFont: HFont;
 BuffDC,MaskDC: HDC;            { used to bitblat all sprites }
 BuffBMH,MaskBMH: HBitmap;     { Bitmap Handles to above DC's }
 gDC: HDC;                      { Use this game dc for all ops }
 FileName,HFileName,PBFileName: Array[0..80] of Char;
 HS,GHS: tHSRec;
 EMenu,MMenu: HMenu;
 Drawing,Modified,NewGHS,HSClear: Boolean;
 SearchRec: tSearch;

 const
  OFN: tOpenFileName = (
      lStructSize: Sizeof(tOpenFileName);
      hWndOwner: 0;
      hInstance: 0;
      lpstrFilter: 'Level Files (*.LVL)'+#0+'*.LVL'+#0+'All Files (*.*)'+#0+'*.*'+#0+#0;
      lpstrCustomFilter: nil;
      nMaxCustFilter: 0;
      nFilterIndex: 1;
      lpstrFile: @FileName;
      nMaxFile: 80;
      lpstrFileTitle: nil;
      nMaxFileTitle: 0;
      lpstrInitialDir: nil;
      lpstrTitle: 'Pick Laser Tank Level File';
      Flags: ofn_HideReadOnly;
      nFileOffset: 0;
      nFileExtension: 0;
      lpstrDefExt: nil;
      lCustData: 0);

  PBFN: tOpenFileName = (
      lStructSize: Sizeof(tOpenFileName);
      hWndOwner: 0;
      hInstance: 0;
      lpstrFilter: 'Playback Files (*.LPB)'+#0+'*.LPB'+#0+'All Files (*.*)'+#0+'*.*'+#0+#0;
      lpstrCustomFilter: nil;
      nMaxCustFilter: 0;
      nFilterIndex: 1;
      lpstrFile: @PBFileName;
      nMaxFile: 80;
      lpstrFileTitle: nil;
      nMaxFileTitle: 0;
      lpstrInitialDir: nil;
      lpstrTitle: 'Save Playback File';
      Flags: ofn_HideReadOnly;
      nFileOffset: 0;
      nFileExtension: 0;
      lpstrDefExt: nil;
      lCustData: 0);


 Procedure SetGameSize(i: integer);                 { Set Up Graphics System }
 Procedure GFXInit;                                 { Load All the Bitmaps }
 Procedure GFXKill;                                 { Release All the Bitmaps }
 Procedure PutLevel;                                { Put All Bitmaps }
 procedure GameOn(b:Boolean);                       { Turns on/off game timer }
 Procedure JK3dFrame(dc:HDC; x,y,x2,y2: integer; up: boolean); { Draw 3D frame }
 Procedure JKSelFrame(dc:HDC; x,y,x2,y2,pnum: integer);             { Draw Red frame }
 Procedure LoadNextLevel( DirectLoad: boolean);
 Procedure AssignHSFile;
 Procedure CheckHighScore;
 Procedure Animate;
 Procedure MoveTank(d: word);
 function  CheckLoc(x,y: word): Boolean;
 procedure MoveLaser;
 procedure FireLaser(x,y,d: word);
 procedure AntiTank;
 Function  GetOBM(bm: word): word;                  { Translate Bitmap to Object }
 procedure BuildBMField;                            { Convert Objects to bitmaps }
 Procedure PutSprite(bmn,x,y: Word);
 Procedure UpDateSprite(x,y: word);                 { Repaint bitmap at x,y }
 Procedure UpDateTank;                              { Repaint Tank }
 Procedure PutSelectors;                            { Editor - paint objects to edit }
 Procedure ChangeGO(x,y,CurSelBM: word);            { Change Game Object }
 function Pad (s: PChar; L: Byte): PChar;
 Procedure SavePBFile;

implementation 

function Pad (s: PChar; L: Byte): PChar;
var x: Byte;
begin
 if strlen(s) < l then for x := strlen(s) to l do s[x] := ' ';
 s[l] := #0;
 Pad := s;
end; 

Procedure PutSprite(bmn,x,y: Word);
{ paint sprite ( bitmap ) at screen location x,y }
begin
 BitBlt (gDC,X,Y,spbm_width,spbm_height,BuffDC,BMA[bmn].x,BMA[bmn].y,SrcCopy);
end;

Procedure UpDateSprite(x,y: word);
var bmn: word;
{ re-paint sprite at Game Board Location x,y }
begin
 bmn := Game.BMF[x,y];
 BitBlt (gDC,XOffset +(x*SpBm_Width),YOffset +(y*SpBm_Height),
         spbm_width,spbm_height,BuffDC,BMA[bmn].x,BMA[bmn].y,SrcCopy);
end;

Procedure UpDateTank;
{ paint mask then tank sprite, we mask the cell so the back ground will show }
begin
 SetTextColor(gDC,RGB(0,0,0));
 SetBkColor(gDC,RGB(255,255,255));
 BitBlt (gDC,XOffset +(Game.tank.x*SpBm_Width),YOffset +(Game.tank.y*SpBm_Height),
         spbm_width,spbm_height,MaskDC,((Game.tank.Dir-1) * SpBm_Width),0,SrcAnd);
 BitBlt (gDC,XOffset +(Game.tank.x*SpBm_Width),YOffset +(Game.tank.y*SpBm_Height),
         spbm_width,spbm_height,BuffDC,BMA[1 + Game.tank.Dir].x,BMA[1 + Game.tank.Dir].y,SrcPaint);
end;

Procedure UpDateLaser;
{ paint laser }
var OB: hBrush;
    w,x,y: integer;
begin
 ob := SelectObject(gdc,LaserColor);
 w := LaserOffset;
 x := xoffset + (laser.x*SpBm_Width);
 y := yoffset + (laser.y*SpBm_height);
 if (laser.dir and 1) = 1 then  Rectangle(gdc,x+w,y,x+SpBm_Width-w,y+SpBm_height)
                          else  Rectangle(gdc,x,y+w,x+SpBm_width,y+SpBm_Height-w);
 SelectObject(gdc,ob);
end;

Procedure UpDateLaserBounce(a,b: word);
{ paint laser bouncing off of a mirror }
var OB: hBrush;
    h,w,x,y: integer;
begin
 ob := SelectObject(gdc,LaserColor);
 w := LaserOffset;
 x := xoffset + (laser.x*SpBm_Width);
 y := yoffset + (laser.y*SpBm_height);
 h := SpBm_Width div 2;
 case a of
  1: rectangle(gdc,x+W,y+h,x+SpBm_Width-w,y+SpBm_Height);
  2: rectangle(gdc,x,y+W,x+h,y+SpBm_Height-w);
  3: rectangle(gdc,x+W,y,x+SpBm_Width-w,y+h);
  4: rectangle(gdc,x+h,y+W,x+SpBm_Width,y+SpBm_Height-w);
 end;
 case b of
  1: rectangle(gdc,x+W,y,x+SpBm_Width-w,y+h);
  2: rectangle(gdc,x+h,y+W,x+SpBm_Width,y+SpBm_Height-w);
  3: rectangle(gdc,x+W,y+h,x+SpBm_Width-w,y+SpBm_Height);
  4: rectangle(gdc,x,y+W,x+h,y+SpBm_Height-w);
 end;
 SelectObject(gdc,ob);
end;

Procedure PutLevel;
{ paint all game cell's and the tank }
var x,y: integer;
begin
 For y := 0 to 15 do
   for x := 0 to 15 do UpdateSprite(x,y);
 UpdateTank;
end;

Procedure GFXInit;
var
 dc,tdc: hdc;
 xh,xh2: hbitmap;
 x,y,i : integer;

begin
 dc := GetDC(MainH);
 BuffDC := CreateCompatibleDC(dc);  { Hold All sprites Here }
 MaskDC := CreateCompatibleDC(dc);  { Hold All Masks Here }
 tDC := CreateCompatibleDC(dc);     { temp memory dc }

 xh := LoadBitmap(hInstance,'MaskBM');
 if xh = 0 then GFXError := 1
 else begin
   SelectObject(MaskDC,xh);
   MaskBMH := CreateCompatibleBitmap(MaskDC,spbm_width * 5,spbm_height * 2);
   selectobject(MaskDC,MaskBMH);          { select new Mask Bitmap dc }
   xh2 := selectobject(tDC,xh);           { select mask into temp dc }
   StretchBlt(MaskDC,0,0,spbm_width * 4,spbm_height,tDC,0,0,96,24,SrcCopy);
   selectobject(tDC,xh2); deleteobject(xh); 
 end;

 xh := LoadBitmap(hInstance,'GameBM');
 if xh = 0 then GFXError := 1
 else begin
   Selectobject(BuffDC,xh);   { makes the next line create a color bitmap }
   BuffBMH := CreateCompatibleBitmap(BuffDC,spbm_width * 10,spbm_height * 5);
   SelectObject(BuffDC,BuffBMH);
   xh2 := SelectObject(tDC,xh);
   StretchBlt(BuffDC,0,0,spbm_width * 10,spbm_height * 5,tDC,0,0,240,120,SrcCopy);
   SelectObject(tDC,xh2); DeleteObject(xh);
   x := 0; y := 0;
   for i := 1 to MaxBitMaps do begin
    BMA[i].x := x;
    BMA[i].y := y;
    x := x + spbm_width;
    if x >= (spbm_width*10) then begin
     x := 0;
     y := y + spbm_height;
    end;
   end;
 end;
 DeleteDC(tDC);
 ReleaseDC(MainH,dc);
 if GFXError <> 0 then messagebox(0,'Loading Error','Error',mb_ok);
 GFXOn := true;
end;

Procedure GFXKill;
var i: word;
begin
 DeleteObject(MaskBMH);
 DeleteObject(BuffBMH);
 DeleteDC(BuffDC);
 DeleteDC(MaskDC);
 GFXOn := false;
end;

Function GetOBM(bm: word): word; { Translates Game Object to Bitmap }
begin
 case bm of
  1: GetOBM := 2;
  2: GetOBM := 6;
  3: GetOBM := 9;
  4: GetOBM := 13;
  5: GetOBM := 14;
  6: GetOBM := 15;
  7: GetOBM := 16;
  8: GetOBM := 36;
  9: GetOBM := 39;
  10: GetOBM := 42;
  11: GetOBM := 20;
  12: GetOBM := 21;
  13: GetOBM := 22;
  14: GetOBM := 23;
  15: GetOBM := 24;
  16: GetOBM := 27;
  17: GetOBM := 30;
  18: GetOBM := 33;
  19: GetOBM := 45;
  20: GetOBM := 47;
  21: GetOBM := 48;
  22: GetOBM := 49;
  23: GetOBM := 50;
  else GetOBM := 1;
 end;
end;

Procedure ChangeGO(x,y,CurSelBM: word);  { Change Game Object }
begin
  gdc := GetDC(MainH);
  if CurSelBM = 1 then begin  { Tank }
    UpdateSprite(Game.tank.x,Game.tank.y); { repaint under old tank}
    Game.tank.x := x; Game.tank.y := y;
    Game.BMF[x,y] := 1;
    UpdateTank;
  end else begin
    Game.PF[x,y] := CurSelBM;
    Game.BMF[x,y] := GetOBM(CurSelBM);
    UpdateSprite(x,y);
  end;
  ReleaseDC(MainH,gDC);
end;

Procedure BuildBMField;
var x,y,i: integer;
begin
 Game.tank.x := 7; Game.tank.y := 15;
 Game.tank.dir := 1; Game.tank.firing := false;
 for x := 0 to 15 do for y := 0 to 15 do begin
   case Game.PF[x,y] of
     1: begin
         i := 1;
         Game.tank.x := x;
         Game.tank.y := y;
         Game.PF[x,y] := 0;
        end;
    else i := GetOBM(Game.PF[x,y]);
   end;
   Game.BMF[x,y] := i;
   Game.BMF2[x,y] := 1;
   Game.PF2[x,y] := 0;
 end;
 { this is a good place to reset the score counters }
 Game.ScoreMove := 0;
 Game.ScoreShot := 0;
end;

procedure GameOn(b:Boolean);
begin
 if b then begin
   SetTimer(MainH,1,GameDelay,nil);
 end else begin
   KillTimer(MainH,1);
 end;
 Game_On := b;
 LastKeyPress:= 0;
end;

Procedure JK3dFrame(dc:HDC; x,y,x2,y2: integer; up: boolean);
var  Wpen,Bpen,penO : HPen;

begin
 Wpen := CreatePen(ps_Solid,1,$00FFFFFF);
 Bpen := CreatePen(ps_Solid,1,$00808080);
 if up then PenO := SelectObject(dc,wpen)
       else PenO := SelectObject(dc,bpen);
 Moveto(dc,x,y2);
 Lineto(dc,x,y);
 Lineto(dc,x2,y);
 if up then SelectObject(dc,bpen)
       else SelectObject(dc,wpen);
 Lineto(dc,x2,y2);
 Lineto(dc,x,y2);
 selectObject(dc,peno);
 deleteobject(bpen); deleteobject(wpen);
end;

Procedure JKSelFrame(dc:HDC; x,y,x2,y2,pnum: integer);
var  Wpen,Bpen,penO : HPen;
     color: longint;

begin
 if pnum = 1 then color := $000000FF else color := $0000FF00;
 Wpen := CreatePen(ps_Solid,2,color);
 PenO := SelectObject(dc,wpen);
 Moveto(dc,x+1,y2);
 Lineto(dc,x+1,y+1);
 Lineto(dc,x2,y+1);
 Lineto(dc,x2,y2);
 Lineto(dc,x+1,y2);
 selectObject(dc,peno);
 deleteobject(wpen);
end;

Procedure LoadNextLevel(DirectLoad: boolean);
var temps,T2: array[0..30] of Char;
    File2Ok: Boolean;
    TempHSData:tHSRec;
    i: integer;
begin
 GameOn(False);
 if Difficulty = 0 then sendmessage(MainH,wm_Command,225,0); { Set Diff Window}
 if Recording then sendmessage(MainH,wm_Command,123,0); { Turn Off Recording }
 Assign(File1,FileName); Reset(File1); i := ioresult;
 if i <> 0 then begin
    str(i,t2); strcopy(temps,'File Error - '); strcat(temps,t2);
    messagebox(MainH,Error_msg1,temps,mb_ok);
    postmessage(MainH,wm_Command,108,0);
   exit;
 end;
 Reset(File2);
 File2Ok := ioresult = 0;
 repeat
   if CurLevel >= FileSize(file1) then begin
     sendMessage(MainH,wm_GameOver,0,0);
     close (File1); if File2Ok then close(File2);
     Exit;
   end;
   seek(File1,CurLevel); Read(File1,CurRecData);
   if SkipCL and File2OK and (not DirectLoad) then begin
     seek(File2,CurLevel);
     if not eof(File2) then read(File2,TempHSData) else TempHSData.moves := 0;
     if TempHSData.moves > 0 then CurRecData.SDiff := 128; { error out diff }
   end;
   Inc(CurLevel);
 until DirectLoad or ((CurRecData.SDiff = 0) or (difficulty and CurRecData.SDiff <> 0));
 Close(File1); if File2Ok then Close(File2);
 Game.PF := CurRecData.PF;
 BuildBMField;
 GameOn(true);
 OKtoHS := true;
 EnableMenuItem(MMenu,110,mf_ByCommand or mf_Grayed); { disable Undo }
 EnableMenuItem(MMenu,112,mf_ByCommand or mf_Grayed); { disable Restore }
 BackUp^[UndoP].Tank.Dir := 0;                        { Block Undo Buffer }
 InvalidateRect(MainH,nil,true);
 if RLL then begin
    WritePrivateProfileString('DATA',psRLLN,FileName,INIFile);
    str(CurLevel,temps);
    WritePrivateProfileString('DATA',psRLLL,temps,INIFile);
 end;
 if ARecord and ( not PBOpen) then sendmessage(MainH,wm_Command,123,0);
end;

Procedure AssignHSFile;
var p,p2: pchar;
begin
  strcopy(HFileName,FileName);
  p := strscan(HFileName,'.');
  strcopy(p,'.ghs');
  Assign(File3,HFileName);
  strcopy(p,'.hs');
  Assign(File2,HFileName);

  { also set up Record Default file name }
  strcopy(PBFilename,FileName);
  p := strscan(PBFileName,'\');
  p2 := PBFilename;
  while p <> nil do begin
    if p <> nil then p2 := p+1;
    p := strscan(p2,'\');
  end;
  p := strscan(P2,'.');
  if p <> nil then p^ := #0;
  if strlen(p2) > 4 then  begin
    p2 := p2+4;
    p2^ := #0;
  end;
  strcat(PBfilename,'_000.lpb');
end;

Procedure CheckHighScore;
var
 i,x: integer;
begin
 if NOT OKtoHS then exit;
 Reset(File2);
 if ioresult <> 0 then Rewrite(File2);
 if CurLevel > FileSize(File2) then begin     { PAD file }
  i := filesize(File2); HS.moves := 0;
  seek(File2,i);
  for x := i to CurLevel-1 do write(File2,HS);
 end;
 seek(File2,CurLevel-1); Read(File2,HS);
 if (HS.moves = 0) or (Game.ScoreMove < HS.Moves) or
    ((Game.ScoreMove = HS.Moves) and (Game.ScoreShot < HS.Shots)) then begin
    SendMessage(MainH,wm_NewHS,0,0);
    seek(File2,CurLevel-1); Write (File2,HS);
 end;
 Close(File2);
end;

Procedure Animate;
var x,y,i: integer;
begin
 inc(AniLevel);
 AniCount := 0;
 if AniLevel = 3 then AniLevel := 0;
 for x := 0 to 15 do for y := 0 to 15 do
 if (x<> Game.tank.x) or (y <> Game.tank.y) then 
   case Game.PF[x,y] of
     2: begin
         Game.BMF[x,y] := 6 + AniLevel;
         UpdateSprite(x,y);
        end;
     3: begin
         Game.BMF[x,y] := 9 + AniLevel;
         UpdateSprite(x,y);
        end;
     7: begin
         Game.BMF[x,y] := 16 + AniLevel;
         UpdateSprite(x,y);
        end;
     8: begin
         Game.BMF[x,y] := 36 + AniLevel;
         UpdateSprite(x,y);
        end;
     9: begin
         Game.BMF[x,y] := 39 + AniLevel;
         UpdateSprite(x,y);
        end;
    10: begin
         Game.BMF[x,y] := 42 + AniLevel;
         UpdateSprite(x,y);
        end;
    15: begin
         Game.BMF[x,y] := 24 + AniLevel;
         UpdateSprite(x,y);
        end;
    16: begin
         Game.BMF[x,y] := 27 + AniLevel;
         UpdateSprite(x,y);
        end;
    17: begin
         Game.BMF[x,y] := 30 + AniLevel;
         UpdateSprite(x,y);
        end;
    18: begin
         Game.BMF[x,y] := 33 + AniLevel;
         UpdateSprite(x,y);
        end;
   end;
end;

procedure UpdateTankPos(x,y:integer);
var temps: array[0..30] of Char;
begin
      SoundPlay(5);
      inc(UndoP); if UndoP > UndoDepth then UndoP := 1;
      Backup^[UndoP] := Game;  { Save Undo Data }
      EnableMenuItem(MMenu,110,mf_ByCommand or mf_Enabled); { enable Undo Command}
      inc(Game.ScoreMove);
      SetTextAlign(gdc,TA_Center);
      SetTextColor(gdc,$0000FF00);
      SetBkColor(gdc,0);
      str(Game.ScoreMove,temps);
      TextOut(gDC,ContXPos+48,207,temps,strlen(temps));
      UpdateSprite(Game.tank.x,Game.tank.y);
      Game.tank.y := Game.tank.y + y;
      Game.tank.x := Game.tank.x + x;
      UpdateTank;
end;

procedure MoveTank(d:word);
begin
 if Game.tank.Dir <> d then begin
  Game.tank.Dir := d;
  UpDateSprite(Game.tank.x,Game.tank.y);
  UpDatetank;
  exit;
 end;
 case d of
  1: if CheckLoc(Game.Tank.x,Game.Tank.y-1) then UpdateTankPos(0,-1);
  2: if CheckLoc(Game.Tank.x+1,Game.Tank.y) then UpdateTankPos(1,0);
  3: if CheckLoc(Game.Tank.x,Game.Tank.y+1) then UpdateTankPos(0,1);
  4: if CheckLoc(Game.Tank.x-1,Game.Tank.y) then UpdateTankPos(-1,0);
 end;
end;

function CheckLLoc(x,y,dx,dy: integer): Boolean;
{ this is were the laser does it's damage }

   procedure MoveObj( obt,bm:word);
   begin
      Game.PF[x,y] := Game.PF2[x,y];  { Return Saved State }
      Game.BMF[x,y] := Game.BMF2[x,y];
      UpdateSprite(x,y);
      x := x + dx; y := y + dy;
      Game.PF2[x,y] := Game.PF[x,y];  { Save Return State }
      Game.BMF2[x,y] := Game.BMF[x,y];
      if Game.PF[x,y] <> 3 then begin
        Game.PF[x,y] := obt; Game.BMF[x,y] := bm;
      end else if obt = 5 then begin
        Game.PF[x,y] := 0; Game.PF2[x,y] := 0;  { Pushing Block into Water }
        Game.BMF[x,y] := 19; Game.BMF2[x,y] := 19;
      end;
      updateSprite(x,y);
      if (x= Game.Tank.x) and (y=Game.Tank.y) then UpdateTank;
      SoundPlay(3);
   end;

   procedure KillAtank;
   begin
    Game.PF[x,y] := 4; { Solid Object}
    Game.BMF[x,y] := 12; { Junk Bitmap}
    UpdateSprite(x,y);
    SoundPlay(1);
   end;


begin
 CheckLLoc := false;
 if (x<0) or (x>15) or (y<0) or (y>15) then begin
    SoundPlay(4);
    exit;
 end;
 if (x = Game.Tank.x) and (y = Game.Tank.y) then begin
   SendMessage(MainH,wm_Dead,0,0);
   exit;
 end;
 case Game.PF[x,y] of
  0,2,3,15,16,17,18: CheckLLoc := true;
  4: SoundPlay(4);
  5: if CheckLoc(x+dx,y+dy) then MoveObj(5,14);
  6: begin
      Game.PF[x,y] := 0; Game.BMF[x,y] := 1; UpdateSprite(x,y);
      SoundPlay(1);
     end;
  7: if dy = 1 then KillAtank else if CheckLoc(x+dx,y+dy) then MoveObj(7,16);
  8: if dx = -1 then KillAtank else if CheckLoc(x+dx,y+dy) then MoveObj(8,36);
  9: if dy = -1 then KillAtank else if CheckLoc(x+dx,y+dy) then MoveObj(9,39);
  10: if dx = 1 then KillAtank else if CheckLoc(x+dx,y+dy) then MoveObj(10,42);
  11: if (laser.dir <>2) and (laser.dir <> 3) then begin
        if CheckLoc(x+dx,y+dy) then MoveObj(11,20);
      end else CheckLLoc := true;
  12: if (laser.dir <>3) and (laser.dir <> 4) then begin
        if CheckLoc(x+dx,y+dy) then MoveObj(12,21);
      end else CheckLLoc := true;
  13: if (laser.dir <>1) and (laser.dir <> 4) then begin
        if CheckLoc(x+dx,y+dy) then MoveObj(13,22);
      end else CheckLLoc := true;
  14: if (laser.dir <>1) and (laser.dir <> 2) then begin
        if CheckLoc(x+dx,y+dy) then MoveObj(14,23);
      end else CheckLLoc := true;
  19: begin
        PutSprite(46,XOffset +(x*SpBm_Width),YOffset +(y*SpBm_Height));
        CheckLLoc := true;
      end;
  20: if (laser.dir <>2) and (laser.dir <> 3) then begin
        Game.PF[x,y] := 21; Game.BMF[x,y] := 48; UpdateSprite(x,y);
      end else CheckLLoc := true;
  21: if (laser.dir <>3) and (laser.dir <> 4) then begin
        Game.PF[x,y] := 22; Game.BMF[x,y] := 49; UpdateSprite(x,y);
      end else CheckLLoc := true;
  22: if (laser.dir <>1) and (laser.dir <> 4) then begin
        Game.PF[x,y] := 23; Game.BMF[x,y] := 50; UpdateSprite(x,y);
      end else CheckLLoc := true;
  23: if (laser.dir <>1) and (laser.dir <> 2) then begin
        Game.PF[x,y] := 20; Game.BMF[x,y] := 47; UpdateSprite(x,y);
      end else CheckLLoc := true;
 end;
end;

function MirrorBounce: Boolean;
var odir: word;
begin
 if (Game.PF[laser.x,laser.y] in [11..14]) or
    (Game.PF[laser.x,laser.y] in [20..23]) then begin
   odir := laser.dir;
   case Game.PF[laser.x,laser.y] of
     11,20: if laser.dir = 2 then laser.dir := 1
                          else laser.Dir := 4;
     12,21: if laser.Dir = 3 then laser.Dir := 2
                          else laser.Dir := 1;
     13,22: if laser.Dir = 1 then laser.Dir := 2
                          else laser.Dir := 3;
     14,23: if laser.Dir = 1 then laser.Dir := 4
                          else laser.Dir := 3;
   end;
   UpdateLaserBounce(odir,laser.Dir);
   MirrorBounce := true;
   SoundPlay(2);
 end else MirrorBounce := false;
end;

procedure MoveLaser;
var x,y: integer;
begin
 x := 0; y := 0;
 case laser.Dir of
  1: y := -1;
  2: x := +1;
  3: y := +1;
  4: x := -1;
 end;
 if CheckLLoc(laser.x+x,laser.y+y,x,y) then begin
      if laser.firing then UpdateSprite(laser.x,laser.y);
      laser.y := laser.y + y;
      laser.x := laser.x + x;
      if Not MirrorBounce then UpdateLaser;
      laser.firing := true;
 end else begin
   Game.Tank.firing := false;
   if laser.firing then UpdateSprite(laser.x,laser.y);
   if Game_On then AntiTank
 end;
end;

procedure FireLaser(x,y,d: word);
var temps: array[0..30] of Char;
begin
     Game.Tank.firing := true;
     laser.Dir := d;
     laser.x := x;
     laser.y := y;
     SoundPlay(2);
     laser.firing := false; { true if laser has been moved }
     SetTextAlign(gdc,TA_Center);
     SetTextColor(gdc,$0000FF00);
     SetBkColor(gdc,0);
     str(Game.ScoreShot,temps);   { we incremented it in wm_timer }
     TextOut(gDC,ContXPos+134,207,temps,strlen(temps));
     MoveLaser;
end;

function CheckLoc(x,y: word): Boolean;
begin
 CheckLoc := false;
 if (x<0) or (x>15) or (y<0) or (y>15) then exit;
 case Game.PF[x,y] of
  0,2,3,15,16,17,18: CheckLoc := true;
 end;
end;

procedure AntiTank;
var x,y: integer;
begin
    { Program Anti tank seek }
    if not Game.Tank.firing then begin
      x := Game.Tank.x;
      while CheckLoc(x,Game.Tank.y) do inc(x);
      if (x<16) and (Game.PF[x,Game.Tank.y] = 10) and (Game.Tank.x <> x) then
        begin FireLaser(x,Game.Tank.y,4); exit end;
      x := Game.Tank.x;
      while CheckLoc(x,Game.Tank.y) do dec(x);
      if (x>=0) and (Game.PF[x,Game.Tank.y] = 8) and (Game.Tank.x <> x)then
        begin FireLaser(x,Game.Tank.y,2); exit end;
      y := Game.Tank.y;
      while CheckLoc(Game.Tank.x,y) do inc(y);
      if (y<16) and (Game.PF[Game.Tank.x,y] = 7) and (Game.Tank.y <> y)then
        begin Firelaser(Game.Tank.x,y,1); exit end;
      y := Game.Tank.y;
      while CheckLoc(Game.Tank.x,y) do dec(y);
      if (y>=0) and (Game.PF[Game.Tank.x,y] = 9) and (Game.Tank.y <> y)then
        begin firelaser(Game.Tank.x,y,3); exit end;
    end;
end;

procedure PutSelectors;
var x,y,i: integer;
begin
 x := ContXPos+5; y := 250;
 for i := 0 to MaxObjects do begin
   jk3dframe(gdc,x-1,y-1,x+SpBm_Width,y+SpBM_Height,true);
   if i = CurSelBM_L then jkSelframe(gdc,x-1,y-1,x+SpBm_Width,y+SpBM_Height,1);
   if i = CurSelBM_R then jkSelframe(gdc,x-1,y-1,x+SpBm_Width,y+SpBM_Height,2);
   PutSprite(GetOBM(i),x,y);
   inc(x,SpBm_Width+4);
   if (i+1) mod EditBMWidth = 0 then begin
     x := ContXPos+5; inc(y,SpBm_Height+4);
   end;
 end;
end;

procedure SetGameSize(i: integer);
var
 temps: array[0..10] of Char;
begin
  CheckMenuItem(MMenu,120,mf_ByCommand);
  CheckMenuItem(MMenu,121,mf_ByCommand);
  CheckMenuItem(MMenu,122,mf_ByCommand);
  CheckMenuItem(MMenu,119+i,mf_ByCommand or mf_Checked);
  str(i,temps);
  WritePrivateProfileString('SCREEN',psSize,temps,INIFile);
  if GFXon then GFXKill;
  case i of
    1: begin
        spbm_Width := 24;
        spbm_Height := 24;
        ContXPos := 409;
        EditBMWidth := 6;
        SetWindowPos(MainH,0,0,0,ContXPos+185,447,SWP_NOMOVE or SWP_NOZORDER);
        LaserOffset := 10;
    end;
    2: begin
        spbm_Width := 30;
        spbm_Height := 30;
        ContXPos := 505;
        EditBMWidth := 5;
        SetWindowPos(MainH,0,0,0,ContXPos+185,544,SWP_NOMOVE or SWP_NOZORDER);
        LaserOffset := 12;
    end;
    3: begin
        spbm_Width := 40;
        spbm_Height := 40;
        ContXPos := 670;
        EditBMWidth := 4;
        SetWindowPos(MainH,0,0,0,ContXPos+185,704,SWP_NOMOVE or SWP_NOZORDER);
        LaserOffset := 17;
    end;
  end;
  SetWindowPos(Ed1,0,ContXPos+9,99,0,0,SWP_NOSIZE or SWP_NOZORDER);
  SetWindowPos(Ed2,0,ContXPos+9,148,0,0,SWP_NOSIZE or SWP_NOZORDER);
  GFXInit;
end;

Procedure SavePBFile;
var
 Book: file;
begin
  assign(Book,pbfilename); ReWrite(Book,1);
  BlockWrite(Book,PBRec,SizeOf(PBRec)); { Write Header }
  BlockWrite(Book,RecBuffer^,PBRec.Size);
  close(Book);
end;

end.