Program LTank;
(*******************************************************
 **             Laser Tank ver 3.1                    **
 **               By Jim Kindley                      **
 **            (c) 2000,1999,1998,1997,1996           **
 **         The Program and Source is Public Domain   **
 *******************************************************)

uses winprocs,wintypes,ctl3d,strings,CommDlg,
     ltank2,Win31,lt_sfx;

{$R ltank.res}
{$D Laser Tank - By Jim Kindley }
{$I- }
Var
  OpenScreen : HBitmap;         { Handle to the instruction Bitmap }

{ ---- Difficulty Box Dialog Procedure ----  }
function DiffBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
var temps: array[0..40] of char;

begin
  DiffBox := True;
  case Message of
    wm_InitDialog: begin
       if Difficulty and 1 = 1 then senddlgitemmessage(Dialog,701,BM_SetCheck,1,0);
       if Difficulty and 2 = 2 then senddlgitemmessage(Dialog,702,BM_SetCheck,1,0);
       if Difficulty and 4 = 4 then senddlgitemmessage(Dialog,703,BM_SetCheck,1,0);
       if Difficulty and 8 = 8 then senddlgitemmessage(Dialog,704,BM_SetCheck,1,0);
       if Difficulty and 16 = 16 then senddlgitemmessage(Dialog,705,BM_SetCheck,1,0);
       Exit;
    end;
    wm_Command: begin
       case wparam of
         1:  begin
            Difficulty := 0;
            if senddlgitemmessage(Dialog,701,BM_GetCheck,0,0) = 1
               then Difficulty := Difficulty + 1;
            if senddlgitemmessage(Dialog,702,BM_GetCheck,0,0) = 1
               then Difficulty := Difficulty + 2;
            if senddlgitemmessage(Dialog,703,BM_GetCheck,0,0) = 1
               then Difficulty := Difficulty + 4;
            if senddlgitemmessage(Dialog,704,BM_GetCheck,0,0) = 1
               then Difficulty := Difficulty + 8;
            if senddlgitemmessage(Dialog,705,BM_GetCheck,0,0) = 1
               then Difficulty := Difficulty + 16;
            str(Difficulty,temps);
            WritePrivateProfileString('DATA',psDiff,temps,INIFile);
            if Difficulty > 0 then EndDialog(Dialog,0);
         end;
         706: begin
            senddlgitemmessage(Dialog,701,BM_SetCheck,1,0);
            senddlgitemmessage(Dialog,702,BM_SetCheck,1,0);
            senddlgitemmessage(Dialog,703,BM_SetCheck,1,0);
            senddlgitemmessage(Dialog,704,BM_SetCheck,1,0);
            senddlgitemmessage(Dialog,705,BM_SetCheck,1,0);
         end;
       end;
    end;
  end;
  DiffBox := False;
end;

{ ---- About Box Dialog Procedure ----
  This is an About box with an Edit window that has text in it from the
  resource file. The text is stored as RCData.}
function AboutBox(Dialog: HWnd; Message, WParam: Word; LParam: Longint): Bool; export;
Var
 H: THandle;
 P: PChar;
begin
  AboutBox := True;
  case Message of
    wm_InitDialog: begin
       { Grab the Text for the About Edit window }
       H := LoadResource(hInstance,FindResource(hInstance,'About',RT_RCData));
       P := LockResource(H);
       SetWindowText(GetDlgItem(Dialog,101),App_Title);
       SetWindowText(GetDlgItem(Dialog,102),App_Version);
       SetWindowText(GetDlgItem(Dialog,107),P);
       UnLockResource(H); FreeResource(H);
       Exit;
    end;
    wm_Command: if wparam = 1 then EndDialog(Dialog,1);
  end; { case Message }
  AboutBox := False;
end;

{ ---- Win Box Dialog Procedure ----
  This is an Win box with an Edit window that has text in it from the
  resource file. The text is stored as RCData.}
function WinBox(Dialog: HWnd; Message, WParam: Word; LParam: Longint): Bool; export;
Var
 H: THandle;
 P: PChar;
begin
  WinBox := True;
  case Message of
    wm_InitDialog: begin
       H := LoadResource(hInstance,FindResource(hInstance,'About',RT_RCData));
       P := LockResource(H);
       SetWindowText(GetDlgItem(Dialog,107),P);
       UnLockResource(H); FreeResource(H);
       Exit;
    end;
    wm_Command: if wparam = 1 then EndDialog(Dialog,1);
  end; { case Message }
  WinBox := False;
end;

{ ---- HS Box Dialog Procedure ----
  This is an High Score box .}
function HSBox(Dialog: HWnd; Message, WParam: Word; LParam: Longint): Bool; export;
Var
 temps: array[0..10] of Char;
 old: array[0..40] of Char;
begin
  HSBox := True;
  case Message of
    wm_InitDialog: begin
       NewGHS := false;
       str(CurLevel,temps);
       SetWindowText(GetDlgItem(Dialog,101),temps);
       SetWindowText(GetDlgItem(Dialog,102),CurRecData.LName);
       str(Game.ScoreMove,temps);
       SetWindowText(GetDlgItem(Dialog,103),temps);
       str(Game.ScoreShot,temps);
       SetWindowText(GetDlgItem(Dialog,104),temps);
       if hs.moves > 0 then begin
          strcopy(old,'Old High Score > M:');
          str(HS.moves,temps); strcat(old,temps); strcat(old,'  S:');
          str(HS.shots,temps); strcat(old,temps); strcat(old,'  I:');
          strcat(old,HS.name);
          ShowWindow(GetDlgItem(Dialog,106),SW_Show);
          SetWindowText(GetDlgItem(Dialog,106),old);
       end;
       GetPrivateProfileString('DATA',psUser,'',hs.name,5,INIFile);
       SetWindowText(GetDlgItem(Dialog,105),HS.Name);
       reset(File3);
       if ioresult = 0 then begin
          seek(File3,CurLevel-1); Read(File3,GHS);
          if (GHS.moves = 0) or (Game.ScoreMove < GHS.Moves) or
          ((Game.ScoreMove = GHS.Moves) and (Game.ScoreShot < GHS.Shots)) then begin
             SetWindowText(GetDlgItem(Dialog,107),'Congratulation''s You beat it !!');
             enablewindow(GetDlgItem(Dialog,107),true);
             NewGHS := true;
          end else begin
             strcopy(old,'M:');
             str(GHS.moves,temps); strcat(old,temps); strcat(old,'  S:');
             str(GHS.shots,temps); strcat(old,temps); strcat(old,'  I:');
             strcat(old,GHS.name);
             SetWindowText(GetDlgItem(Dialog,107),old);
          end;
          close(File3);
       end;
       Exit;
    end;
    wm_Command: if wparam = 1 then begin
       GetWindowText(GetDlgItem(Dialog,105),temps,5);
       HS.moves := Game.ScoreMove;
       HS.Shots := Game.ScoreShot;
       if stricomp(temps,HS.Name) <> 0 then begin
          strcopy(HS.Name,temps);
          WritePrivateProfileString('DATA',psUser,HS.Name,INIFile);
       end;
       if NewGHS then begin
          reset(File3);
          seek(File3,CurLevel-1); write(File3,HS);
          close(File3);
          if ioresult <> 0 then messagebeep(0);
       end;
       EndDialog(Dialog,1);
    end;
  end; { case Message }
  HSBox := False;
end;

{ ---- DefBox Dialog Procedure ----
 This dialog Proc is used for the Dead dialogs.
 It returns the value of the button pressed.}
function DefBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
begin
  DefBox := true;
  case Message of
    wm_InitDialog: Exit;                    { I need this or I don't have keyboard }
    wm_Command: EndDialog(Dialog,WParam);   { end dialog with button id }
  end; { case Message }
  DefBox := False;
end;

{ ---- Search Box Dialog Procedure ----
  The Load Box uses this proc to select a Search term and type.}
function SearchBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
begin
  SearchBox := True;
  case Message of
    wm_InitDialog: begin
       sendmessage(GetDlgItem(Dialog,102),bm_SetCheck,1,0);
       SearchRec.mode := 1;
       Exit;
    end;
    wm_Command: begin
       case wparam of
          1: begin
             GetWindowText(GetDlgItem(Dialog,101),SearchRec.Data,60);
             StrUpper(SearchRec.Data);
             if SendMessage(GetDlgItem(Dialog,102),bm_GetCheck,0,0) <> 0 then
                SearchRec.mode := 1 else SearchRec.mode := 2;
             EndDialog(Dialog,1);
          end;
          2: EndDialog(Dialog,2);
       end;
    end;
  end;
  SearchBox := False;
end;
{ ---- Load Box Dialog Procedure ----
  List out all levels in the current data file, by number & title.
  return with the level # + 101 or 0 if canceled}
function LoadBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
var TempRecData:TLevel;
    temps: array[0..80] of char;
    i,y: integer;
    DProc : TFarProc;
    P:PChar;

begin
  LoadBox := True;
  case Message of
    wm_InitDialog: begin
       Assign(File1,filename); Reset(File1);
       if IOResult <> 0 then exit;
       SetWindowText(GetDlgItem(Dialog,103),filename);
       i := 1;
       while not eof(File1) do begin
          read(File1,TempRecData);
          str(i:3,temps); strcat(temps,'  '); strcat(temps,Pad(TempRecData.LName,30));
          strcat(temps,' ');
          strcat(temps,TempRecData.Author);
          senddlgitemmessage(Dialog,101,LB_AddString,0,longint(@temps));
          inc(i);
       end;
       close(File1);
       senddlgitemmessage(Dialog,101,LB_SETCURSEL,CurLevel-1,0);
       Exit;
    end;
    wm_Command: begin
       case wparam of
          101: if HiWord(lparam) = lbn_selchange then begin
               i := senddlgitemmessage(Dialog,101,LB_GetCurSel,0,0);
               Senddlgitemmessage(Dialog,101,LB_GetText,i,longint(@temps));
               P := @temps; inc(p,3);P^ := #0;
               P := @temps; while P^ = ' ' do inc(P);
               val(p,i,y);
               EndDialog(Dialog,i+100);  { Add 100 to the Select # }
          end;
          102: EndDialog(Dialog,0);
          104: Begin   { Search }
               DProc := MakeProcInstance(@SearchBox,HInstance);
               i := DialogBox(HInstance, 'Search', Dialog, DProc);
               FreeProcInstance(DProc);
               if i = 1 then begin
                  senddlgitemmessage(Dialog,101,LB_ResetContent,0,0);
                  Reset(File1); i := 1;
                  while not eof(File1) do begin
                    read(File1,TempRecData);
                    if SearchRec.mode = 1 then strcopy(temps,TempRecData.LName)
                                          else strcopy(temps,TempRecData.Author);
                    if strpos(StrUpper(temps),SearchRec.Data) <> nil then begin
                       str(i:3,temps); strcat(temps,'  '); strcat(temps,Pad(TempRecData.LName,30));
                       strcat(temps,' ');
                       strcat(temps,TempRecData.Author);
                       senddlgitemmessage(Dialog,101,LB_AddString,0,longint(@temps));
                    end;
                    inc(i);
                  end;
                  close(File1);
               end;
          end;
       end;
    end;
  end;
  LoadBox := False;
end;

{ ---- HS List Dialog Procedure ----
  List out all levels in the current data file, by number ,title, moves, shots & initials}
function HSList(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
var TempRecData:TLevel;
    TempHSData:tHSRec;
    temps,T2: array[0..60] of char;
    i,y: integer;
    p: pchar;

begin
  HSList := True;
  case Message of
    wm_InitDialog: begin
       Assign(File1,filename); Reset(File1);
       if IOResult <> 0 then exit;
       Reset(File2);
       if IOResult <> 0 then begin
          close(File1);
          exit;
       end;
       SetWindowText(GetDlgItem(Dialog,103),hfilename);
       i := 1;
       while not eof(File2) do begin
         read(File1,TempRecData); read(File2,TempHSData);
         str(i:3,temps); strcat(temps,' ');
         strcat(temps,pad(TempRecData.LName,30));
         if TempHSData.moves > 0 then begin
           str(TempHSData.moves:5,t2); strcat (temps,' ');strcat(temps,t2);
           str(TempHSData.shots:5,t2); strcat (temps,'  ');strcat(temps,t2);
           strcat(temps,'  '); strcat(temps,tempHSData.name);
         end;
         senddlgitemmessage(Dialog,101,LB_AddString,0,longint(@temps));
         inc(i);
       end;
       close(File1); close(File2);
       senddlgitemmessage(Dialog,101,LB_SETCURSEL,CurLevel-1,0);
       Exit;
    end;
    wm_Command: begin
       if wparam = 1 then EndDialog(Dialog,0);
       if (wparam = 101) and (HiWord(lparam) = lbn_selchange) then begin
               i := senddlgitemmessage(Dialog,101,LB_GetCurSel,0,0);
               Senddlgitemmessage(Dialog,101,LB_GetText,i,longint(@temps));
               P := @temps; inc(p,3);P^ := #0;
               P := @temps; while P^ = ' ' do inc(P);
               val(p,i,y);
               CurLevel := i-1;
               LoadNextLevel(True);
               EndDialog(Dialog,1);
          end;
    end;
  end;
  HSList := False;
end;

{ ---- Global HS List Dialog Procedure ----
  List out all levels in the current data file, by number ,title, moves, shots & initials}
function GHSList(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
var TempRecData:TLevel;
    TempHSData,TempGHSData:tHSRec;
    temps,T2: array[0..60] of char;
    i,y: integer;
    p: pchar;
    File2Ok: Boolean;

begin
  GHSList := True;
  case Message of
    wm_InitDialog: begin
       Assign(File1,filename); Reset(File1);
       if IOResult <> 0 then exit;
       Reset(File2);
       File2Ok := ioresult = 0;
       Reset(File3);
       if IOResult <> 0 then begin
          close(File1);
          if File2Ok then close(File2);
          exit;
       end;
       i := 1;
       while not eof(File3) do begin
          read(File1,TempRecData);
          if File2Ok and (not eof(File2)) then read(File2,TempHSData);
          read(File3,TempGHSData);
          str(i:3,temps); strcat(temps,' ');
          strcat(temps,pad(TempRecData.LName,30));
          str(TempGHSData.moves:5,t2); strcat (temps,' ');strcat(temps,t2);
          str(TempGHSData.shots:5,t2); strcat (temps,' ');strcat(temps,t2);
          strcat(temps,' '); strcat(temps,pad(tempGHSData.name,4));
          if File2Ok and (TempHSData.moves > 0) then begin
            str(TempHSData.moves:5,t2); strcat (temps,' ');strcat(temps,t2);
            str(TempHSData.shots:5,t2); strcat (temps,' ');strcat(temps,t2);
            strcat(temps,' '); strcat(temps,pad(tempHSData.name,4));
          end;
          senddlgitemmessage(Dialog,101,LB_AddString,0,longint(@temps));
          inc(i);
          if File2Ok and (eof(File2)) then TempHSData.moves := 0;
       end;
       close(File1); close(File3); if File2Ok then close (File2);
       senddlgitemmessage(Dialog,101,LB_SETCURSEL,CurLevel-1,0);
       Exit;
    end;
    wm_Command: begin
       if wparam = 1 then EndDialog(Dialog,0);
       if (wparam = 101) and (HiWord(lparam) = lbn_selchange) then begin
               i := senddlgitemmessage(Dialog,101,LB_GetCurSel,0,0);
               Senddlgitemmessage(Dialog,101,LB_GetText,i,longint(@temps));
               P := @temps; inc(p,3);P^ := #0;
               P := @temps; while P^ = ' ' do inc(P);
               val(p,i,y);
               CurLevel := i-1;
               LoadNextLevel(True);
               EndDialog(Dialog,1);
          end;
    end;
  end;
  GHSList := False;
end;

{ ---- Hint Box Dialog Procedure ----
  The editor uses this proc to Display & Edit the Hint. If the text is modified
  the 'Modified' varable is set.}
function HintBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
begin
  HintBox := True;
  case Message of
    wm_InitDialog: begin
       SetWindowText(GetDlgItem(Dialog,101),CurRecData.Hint);
       Exit;
    end;
    wm_Command: begin
       case wparam of
          1: begin
             GetWindowText(GetDlgItem(Dialog,101),CurRecData.Hint,255);
             if SendMessage(GetDlgItem(Dialog,101),em_GetModify,0,0) <> 0 then
                Modified := true;
             EndDialog(Dialog,1);
          end;
          2: EndDialog(Dialog,2);
       end;
    end;
  end;
  HintBox := False;
end;
{ ---- Recorder Box Dialog Procedure ----
  The Recorder uses this to prompt for the Author's name. }
function RecordBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
begin
  RecordBox := True;
  case Message of
    wm_InitDialog: begin
       SetWindowText(GetDlgItem(Dialog,101),PBRec.Author);
       SetWindowText(GetDlgItem(Dialog,102),CurRecData.LName);
       Exit;
    end;
    wm_Command: begin
       case wparam of
         1: begin
            GetWindowText(GetDlgItem(Dialog,101),PBRec.Author,31);
            strcopy(PBRec.LName,CurRecData.LName);
            PBRec.Level := CurLevel;
            EndDialog(Dialog,1);
         end;
       end;
    end;
  end;
  RecordBox := False;
end;

{ ---- PlayBack Dialog Procedure ----
  This is the Playback Control window. }
function PBWindow(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
var
 Book: file;
 temps: array[0..100] of Char;
 t2: array[0..10] of Char;
 Box: tRect;
begin
  PBWindow := True;
  case Message of
    wm_InitDialog: begin
       if Recording then SendMessage(MainH,wm_Command,123,0); { Turn Off Recording }
       PBOpen := true;
       Assign(Book,PBFilename); Reset(Book,1); { Open PLayback file }
       BlockRead(Book,PBRec,sizeof(PBRec));
       Str(PBRec.Size,temps);
       SetWindowText(GetDlgItem(Dialog,104),temps);
       RecBufSize := PBRec.Size;
       Getmem(RecBuffer,RecBufSize);
       BlockRead(Book,RecBuffer^,PBRec.Size);  { Load RecBuffer W data }
       Close(Book);
       CurLevel := PBRec.Level - 1;
       LoadNextLevel(true); { this will error if the levels have moved }
       if strcomp(CurRecData.LName,PBRec.LName) <> 0 then begin
          { Do a hard file search for the level name }
          Reset(File1); CurLevel := 0;
          while (not eof(File1)) and (strcomp(CurRecData.LName,PBRec.LName) <> 0)
            do begin
            Read(File1,CurRecData);
            inc(CurLevel);
          end;
          dec(CurLevel);
          loadnextlevel(true);
       end;
       if strcomp(CurRecData.LName,PBRec.LName) = 0 then begin
          Game.RecP := 0;
          PBCountH := GetDlgItem(Dialog,103);
          PlayH := Dialog;
          strcopy (temps,'PlayBack Level : '); 
          strcat(temps,PBRec.LName); strcat(temps,PCRLF);
          strcat(temps,'Recorded by ');
          strcat(temps,PBRec.Author);
          sendmessage(GetDlgItem(Dialog,104+speed),bm_SetCheck,1,0);
          GetWindowRect(MainH,Box);
          SetWindowPos(Dialog,0,Box.left+ContXPos+2,Box.Top+280,0,0,
                               SWP_NOSIZE or SWP_NOZORDER);
          if messagebox(MainH,temps,'Laser Tank',mb_OKCANCEL) = 2 then begin
             Freemem(RecBuffer,RecBufSize);
             EndDialog(Dialog,1);
             Exit;
          end;
       end else begin
          strcopy(temps,PBRec.LName); strcat(temps,PCRLF);
          strcat(temps,'Not found in level file ');
          strcat(temps,filename);
          messagebox(Dialog,temps,'Error',mb_ok);
          Freemem(RecBuffer,RecBufSize);
          EndDialog(Dialog,1);
          PBOpen := false;
       end;
       Exit;
    end;
    wm_Command: begin
       case wparam of
         1: begin
            if Game_On and (Game.RecP = PBRec.Size) and
               (messagebox(Dialog,'Do You want to Continue Recording ?',
               'LaserTank',mb_YesNo) = IDYES) then begin
                CheckMenuItem(MMenu,123,mf_ByCommand or mf_Checked);
                Recording := true;
                SetWindowText(MainH,REC_title);
            end else begin
                GameOn(False);
                Freemem(RecBuffer,RecBufSize);
            end;
            PlayBack := false;
            PBOpen := false;
            EndDialog(Dialog,1);
         end;
         105: speed := 1;  { Fast }
         106: speed := 2;  { Slow }
         107: speed := 3; { Single Step }
         108: if PlayBack then begin
                SetWindowText(GetDlgItem(Dialog,108),'&Play');
                PlayBack := false;
              end else begin
                SetWindowText(GetDlgItem(Dialog,108),'&Pause');
                PlayBack := true;
              end;
         109: begin
              PlayBack := false;
              SetWindowText(GetDlgItem(Dialog,108),'&Play');
              SetWindowText(GetDlgItem(Dialog,103),'0');
              sendmessage(MainH,wm_command,105,0);
              Game.RecP := 0;
         end;
       end;
    end;
  end;
  PBWindow := False;
end;

{ ---- Pick Box Dialog Procedure ----
 The editor uses this to pick which level number to save the
 level with. Returns the number.}
function PickBox(Dialog: HWnd; Message, WParam: Word;
  LParam: Longint): Bool; export;
var temps,t2: array[0..40] of char;
    i,j: integer;

begin
  PickBox := True;
  case Message of
    wm_InitDialog: begin
                        i := FileSize(File1);
                        str(i,temps);
                        strcopy(t2,'1 - '); StrCat(t2,temps);
                        SetWindowText(GetDlgItem(Dialog,103),t2);
                        inc(i); str(i,temps);
                        SetWindowText(GetDlgItem(Dialog,104),temps);
                        if CurLevel < i then str(CurLevel,temps);
                        SetWindowText(GetDlgItem(Dialog,101),temps);
                        SendDlgItemMessage(Dialog,102,bm_SetCheck,1,0);
                        Exit;
                   end;
    wm_Command:    if wparam = 1 then begin
                            GetWindowText(GetDlgItem(Dialog,101),temps,10);
                            Val(temps,i,j);
                            HSClear := SendDlgItemMessage(Dialog,102,bm_GetCheck,0,0) > 0;
                            EndDialog(Dialog,i);
                            Exit;
                   end;
  end;
  PickBox := False;
end;

procedure EditDiffSet(t:word);
begin
  CheckMenuItem(EMenu,701,mf_ByCommand);
  CheckMenuItem(EMenu,702,mf_ByCommand);
  CheckMenuItem(EMenu,703,mf_ByCommand);
  CheckMenuItem(EMenu,704,mf_ByCommand);
  CheckMenuItem(EMenu,705,mf_ByCommand);
  case t of
    1: CheckMenuItem(EMenu,701,mf_ByCommand or mf_Checked);
    2: CheckMenuItem(EMenu,702,mf_ByCommand or mf_Checked);
    4: CheckMenuItem(EMenu,703,mf_ByCommand or mf_Checked);
    8: CheckMenuItem(EMenu,704,mf_ByCommand or mf_Checked);
    16: CheckMenuItem(EMenu,705,mf_ByCommand or mf_Checked);
  end;
  CurRecData.SDiff := t;
end;

{ --- Main Window Procedure --- }
{ This is used for both the editor and the game }
function MainWindowProc(Window: HWnd; Message, WParam: Word;
  LParam: Longint): Longint; export;
Var
 temps: array[0..100] of Char;
 tDC,pdc: HDC;
 tBM: HBitmap;
 Menu: HMenu;
 Box:TRect;
 PI: tPaintStruct;
 DProc : TFarProc;
 i,x,y: integer;
 twp: TWindowPlacement;
 P: pointer;

Begin
  MainWindowProc := 0;
  case Message of
    wm_create: begin
                    StrCopy(FileName,LevelData);    { set up default file name }
                    MainH  := Window;
                    MMenu := GetMenu(Window);
                    GetPrivateProfileString('OPT',psAni,'Yes',temps,5,INIFile);
                    { Check if Animation is enabled or disabled }
                    if strcomp(temps,'Yes') <> 0 then begin
                       Ani_On := false;
                       CheckMenuItem(MMenu,104,mf_ByCommand);
                    end;


                    GetPrivateProfileString('OPT',psSound,'Yes',temps,5,INIFile);
                    { Check if Sound is enabled or disabled }
                    if strcomp(temps,'Yes') <> 0 then begin
                       Sound_On := false;
                       CheckMenuItem(MMenu,102,mf_ByCommand);
                    end;

                    GetPrivateProfileString('OPT',psSCL,'No',temps,5,INIFile);
                    { Check if Sound is enabled or disabled }
                    if strcomp(temps,'Yes') = 0 then begin
                       SkipCL := true;
                       CheckMenuItem(MMenu,116,mf_ByCommand or mf_Checked);
                    end;

                    GetPrivateProfileString('OPT',psARec,'No',temps,5,INIFile);
                    { Check if AutoRecord is enabled or disabled }
                    if strcomp(temps,'Yes') = 0 then begin
                       ARecord := true;
                       CheckMenuItem(MMenu,115,mf_ByCommand or mf_Checked);
                    end;
                    Difficulty := GetPrivateProfileInt('DATA',psDiff,0,INIFile);
                    GetPrivateProfileString('OPT',psRLLOn,'Yes',temps,5,INIFile);
                    { Check if Remember Last Level is enabled or disabled }
                    if strcomp(temps,'Yes') <> 0 then begin
                       RLL := false;
                       CheckMenuItem(MMenu,109,mf_ByCommand);
                    end else GetPrivateProfileString('DATA',psRLLN,LevelData,FileName,100,INIFile);
                    LaserColor := CreateSolidBrush($0000FF00);
                    New(BackUp); UndoP := 1; Backup^[UndoP].Tank.Dir := 0;
                    AssignHSFile;
                    SFXInit;
    end;
    wm_Paint:  begin
                    pdc := BeginPaint(Window,PI);
                    gdc := pdc;       { we use gdc for most graphics stuff}
                    SelectObject(gDC,MyFont);
                    GetClientRect(Window,Box);
                    { draw 3D frames }
                    jk3dframe(pdc,XOffset-1,YOffset-1,(spbm_Width*16)+XOffset+2
                    ,(spbm_Height*16)+YOffset+2,false);
                    jk3dframe(pdc,XOffset-10,YOffset-10,ContXPos-5,Box.Bottom-2,true);
                    jk3dframe(pdc,ContXPos-1,1,Box.Right-2,Box.Bottom-2,true);
                    tDC := CreateCompatibleDC(PDC);
                    OpenScreen := LoadBitmap(HInstance,'Control');
                    tBM := SelectObject(tDC,OpenScreen);
                    { put up control bitmap }
                    BitBlt (PDC,ContXPos,2,180,245,tDC,0,0,SrcCopy);
                    SelectObject (tDC,tBM);
                    DeleteObject(OpenScreen);
                    if EditorOn then begin
                       PutSelectors;
                       PutLevel;
                    end else begin
                       if CurLevel > 0 then begin
                         SetBkMode(Pdc,transparent); SetTextAlign(pdc,TA_Center);
                         SetTextColor(pdc,$0000FFFF);
                         TextOut(pdc,ContXPos+91,100,CurRecData.LName,strlen(CurRecData.LName));
                         TextOut(pdc,ContXPos+91,150,CurRecData.Author,strlen(CurRecData.Author));
                         str(CurLevel,temps);
                         case CurRecData.SDiff of
                          0: strcat(temps,' - NR');
                          1: begin
                                  strcat(temps,' - Kids');
                                  SetTextColor(pdc,$008000FF);
                          end;
                          2: begin
                                  strcat(temps,' - Easy');
                                  SetTextColor(pdc,$00FF00FF);
                          end;
                          4: begin
                                  strcat(temps,' - Medium');
                                  SetTextColor(pdc,$0000FF00);
                          end;
                          8: begin
                                  strcat(temps,' - Hard');
                                  SetTextColor(pdc,$00FFFF00);
                          end;
                          16: begin
                                   strcat(temps,' - Deadly');
                                  SetTextColor(pdc,$000000FF);
                          end;
                         end;
                         TextOut(pdc,ContXPos+91,43,temps,strlen(temps));
                         SetTextColor(pdc,$0000FF00);
                         str(Game.ScoreMove,temps);
                         TextOut(pdc,ContXPos+48,207,temps,strlen(temps));
                         str(Game.ScoreShot,temps);
                         TextOut(pdc,ContXPos+134,207,temps,strlen(temps));
                         PutLevel;
                       end else begin
                         { come here in the beggining before a level is loaded }
                         OpenScreen := LoadBitmap(HInstance,'Opening');
                         tBM := SelectObject(tDC,OpenScreen);
                         StretchBlt (PDC,XOffset,YOffset,spbm_Width*16,spbm_Height*16,
                           tDC,0,0,384,384,SrcCopy);
                         SelectObject (tDC,tBM);
                         DeleteObject(OpenScreen);
                       end;
                    end;
                    DeleteDC(tDC);
                    EndPaint(Window,PI);
                    Exit;
    end;
    wm_KeyDown: if not EditorOn then begin
                     case wparam of
                          $41: postmessage(window,WM_Command,104,0);  { A - Animation }
                          $44: postmessage(window,WM_Command,225,0);  { D - Difficulty }
                          $48: postmessage(window,WM_Command,301,0);  { H - Hint }
                          $4C: postmessage(window,WM_Command,106,0);  { L - Load }
                          $4E: postmessage(window,WM_Command,102,0);  { N - Sound }
                          $4F: postmessage(window,WM_Command,108,0);  { O - Open }
                          $52: postmessage(window,WM_Command,105,0);  { R - Restart }
                          $53: postmessage(window,WM_Command,107,0);  { S - Skip Level }
                          $55: if GetMenuState(MMenu,110,mf_ByCommand) = 0 then
                               postmessage(window,WM_Command,110,0);  { U - Undo }
                          $56: postmessage(window,WM_Command,113,0);  { V - View High Score }
                          VK_F1:  WinHelp(Window,'LTank.Hlp',Help_Index,0);
                          VK_F2: postmessage(window,WM_Command,101,0);  { F2 - New Game }
                          VK_F5: postmessage(window,WM_Command,123,0);  { F5 - Record }
                          VK_F6: if Recording then
                                  postMessage(Window,wm_SaveRec,0,0);   { F6 - SaveRec }
                          VK_F7: postmessage(window,WM_Command,114,0);  { F7 - Playback }
                          else LastKeyPress := wparam;
                     end;
                     Exit;
    end;
    wm_Timer:  begin
                    gDC := GetDC(Window);
                    SelectObject(gDC,MyFont);
                    if Ani_On then inc(AniCount);
                    if AniCount = Ani_Delay then Animate; { Do Animation }
                    if Game.tank.firing then MoveLaser;   { Move laser if one was fired }
                    if PlayBack and (LastKeyPress = 0) and
                      (Game.RecP < PBRec.Size) then begin
                       if speed = 2 then begin
                         inc(SlowPB);
                         if SlowPB = SlowPBSet then SlowPB := 1;
                       end;
                       if (Not ConvMoving) and
                        ((speed <> 2) or ((speed = 2) and (slowPB = 1))) then begin
                          inc(Game.RecP);
                          LastKeyPress := RecBuffer^[Game.RecP];
                          Str(Game.RecP,temps);
                          SendMessage(PBCountH,wm_SetText,0,longint(@temps));
                          if speed = 3 then sendmessage(PlayH,wm_Command,108,0);
                       end;
                    end;
{if Game.RecP = 11270 then PlayBack := false;}
                    { Check Key Press }
                    if (LastKeyPress > 0) and (Not (Game.tank.firing or ConvMoving)) then begin
                       case LastKeyPress of
                            VK_Up:    Movetank(1); { Move tank Up one }
                            VK_Right: Movetank(2);
                            VK_Down:  Movetank(3);
                            VK_Left:  Movetank(4);
                            VK_space: begin
                               inc(UndoP); if UndoP > UndoDepth then UndoP := 1;
                               Backup^[UndoP] := Game;  { Save Undo Data }
                               EnableMenuItem(MMenu,110,mf_ByCommand or mf_Enabled);
                               inc(Game.ScoreShot);  { do here Not in FireLaser }
                               FireLaser(Game.tank.x,Game.tank.y,Game.tank.Dir); { Bang }
                            end;
                       end;
                       if recording then begin
                         inc(Game.RecP);
                         if Game.RecP > RecBufSize then begin
                           if RecBufSize >= RecBufMax then begin
                             messagebox(window,'Recorder Buffer Overflow','Error',mb_ok);
                             SendMessage(Window,wm_SaveRec,0,0);
                           end else begin
                             i := RecBufSize + RecBufStep;
                             getmem(P,i);
                             move(RecBuffer^,P^,RecBufSize);     { Move data }
                             freemem(RecBuffer,RecBufSize);
                             RecBufSize := i;
                             RecBuffer := P;
                           end;
                         end;
                         RecBuffer^[Game.RecP] := Byte(LastKeyPress);
                       end;
                       LastKeyPress := 0;
                       AntiTank;  { give the Anti-Tanks a turn to play }
                    end;
                    with Game do begin
                    ConvMoving := false;    { used to disable Laser on the conveyor }
                    Case Game.PF[tank.x,tank.y] of
                         2: if Game_On then begin   { Reached the Flag }
                               GameOn(False);
                               SoundPlay(6);
                               if not PBOpen then begin
                                 if Recording then SendMessage(Window,wm_SaveRec,0,0);
                                 CheckHighScore;
                                 LoadNextLevel(False);
                               end;
                         end;
                         3: PostMessage(Window,wm_Dead,0,0);  { Water }
                         15: if CheckLoc(tank.x,tank.y-1) then begin { Conveyor Up }
                             ConvMoving := true;
                             UpDateSprite(tank.x,tank.y);
                             Dec(tank.y);
                             UpDateTank;
                             AntiTank;
                         end;
                         16: if CheckLoc(tank.x+1,tank.y) then begin
                             ConvMoving := true;
                             UpDateSprite(tank.x,tank.y);
                             Inc(tank.x);
                             UpDateTank;
                             AntiTank;
                         end;
                         17: if CheckLoc(tank.x,tank.y+1) then begin
                             ConvMoving := true;
                             UpDateSprite(tank.x,tank.y);
                             Inc(tank.y);
                             UpDateTank;
                             AntiTank;
                         end;
                         18: if CheckLoc(tank.x-1,tank.y) then begin
                             ConvMoving := true;
                             UpDateSprite(tank.x,tank.y);
                             Dec(tank.x);
                             UpDateTank;
                             AntiTank;
                         end;
                       end;
                    end;
                    ReleaseDC(Window,gdc);
                    Exit;
    end;
    wm_GameOver: begin
                    DProc := MakeProcInstance(@WinBox,HInstance);
                    DialogBox(HInstance, 'WinBox', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
    end;
    wm_NewHS: begin
                    DProc := MakeProcInstance(@HSBox,HInstance);
                    DialogBox(HInstance, 'HighBox', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
    end;
    wm_SaveRec: begin
                    DProc := MakeProcInstance(@RecordBox,HInstance);
                    DialogBox(HInstance, 'RecBox', Window, DProc);
                    FreeProcInstance(DProc);
                    StrCopy(pbfn.lpstrTitle,'Save Playback File');
                    pbfn.Flags:= ofn_HideReadOnly or ofn_overwriteprompt;
                    str(PBRec.Level:3,temps);
                    i := strlen(PBfilename) - 7;
                    if temps[0] = ' ' then pbfilename[i] := '0' else pbfilename[i] := temps[0];
                    if temps[1] = ' ' then pbfilename[i+1] := '0' else pbfilename[i+1] := temps[1];
                    pbfilename[i+2] := temps[2];
                    if GetSaveFileName(PBfn) then begin
                       if StrScan(PBfilename,'.') = nil then Strcat(PBFileName,'.lpb');
                       PBRec.Size := game.RecP;              { Set Playback size }
                       SavePBFile;
                    end;
                    SendMessage(Window,wm_Command,123,0);
                    Exit;
    end;
    wm_Dead: begin
                  GameOn(False);
                  SoundPlay(7);
                  DProc := MakeProcInstance(@DefBox,HInstance);
                  i := DialogBox(HInstance, 'DeadBox', Window, DProc);
                  FreeProcInstance(DProc);
                  case i of
                   1: begin
                       sendmessage(Window,wm_Command,110,0);
                       GameOn(True);
                      end;
                   2: postmessage(Window,wm_Command,105,0);  { Restart }
                  end;
                  Exit;
    end;
    wm_Destroy: begin
                     GFXKill;
                     SoundKill;
                     DeleteObject(LaserColor);
                     Dispose(Backup);
                     if Game_On then GameOn(False); { Kill Timer }
                     WinHelp(Window,'LTank.Hlp',Help_Quit,0);
                     PostQuitMessage(0);
                     Exit;
     end;
     wm_Move: begin
                    twp.length := sizeof(twp);
                    GetWindowPlacement(window,@twp);
                    if twp.showCMD <> sw_shownormal then exit;
                    str(twp.rcNormalPosition.top,temps);
                    writePrivateprofilestring('SCREEN',psYPos,temps,INIFile);
                    str(twp.rcNormalPosition.left,temps);
                    writeprivateprofilestring('SCREEN',psXPos,temps,INIFile);
     end;
     wm_MouseMove: if EditorOn then begin  { if we are editing then check mouse }
                      if not drawing then exit;
                      y := ((HiWord(LParam) - XOffset) Div SpBm_Height);
                      x := ((LoWord(LParam) - YOffset) div SpBm_Width);
                      if (x<0) or (x>15) or (y<0) or (y>15) then exit;
                      if wParam and MK_LButton = MK_LButton then begin
                        if Game.PF[x,y] <> CurSelBM_L then ChangeGO(x,y,CurSelBM_L);
                      end else begin
                        if Game.PF[x,y] <> CurSelBM_R then ChangeGO(x,y,CurSelBM_R);
                      end;
                      Exit;
     end;
     wm_LButtonDown: if EditorOn then begin
                        if LoWord(lparam) > ContXPos then begin
                           y := (HiWord(LParam) - 250) Div (SpBm_Height+4);
                           x := (LoWord(LParam) - (ContXPos+5)) div (SpBm_Width+4);
                           i := x + (y*EditBMWidth);
                           if (i > MaxObjects) or (i < 0) then exit;
                           CurSelBM_L := i;
                           gDC := GetDC(Window);
                           PutSelectors;
                           ReleaseDC(Window,gDC);
                        end else begin
                           y := ((HiWord(LParam) - XOffset) Div SpBm_Height);
                           x := ((LoWord(LParam) - YOffset) div SpBm_Width);
                           if (x<0) or (x>15) or (y<0) or (y>15) then exit;
                           if Game.PF[x,y] <> CurSelBM_L then ChangeGO(x,y,CurSelBM_L);
                           Drawing := true;
                           Modified := true;
                        end;
                      Exit;
     end;
     wm_RButtonDown: if EditorOn then begin
                        if LoWord(lparam) > ContXPos then begin
                           y := (HiWord(LParam) - 250) Div (SpBm_Height+4);
                           x := (LoWord(LParam) - (ContXPos+5)) div (SpBm_Width+4);
                           i := x + (y*EditBMWidth);
                           if (i > MaxObjects) or (i < 0) then exit;
                           CurSelBM_R := i;
                           gDC := GetDC(Window);
                           PutSelectors;
                           ReleaseDC(Window,gDC);
                        end else begin
                           y := ((HiWord(LParam) - XOffset) Div SpBm_Height);
                           x := ((LoWord(LParam) - YOffset) div SpBm_Width);
                           if (x<0) or (x>15) or (y<0) or (y>15) then exit;
                           if Game.PF[x,y] <> CurSelBM_R then ChangeGO(x,y,CurSelBM_R);
                           Drawing := true;
                           Modified := true;
                        end;
                      Exit;
     end;
     wm_LButtonUp,
     wm_RButtonUp: if EditorOn then begin
                      Drawing := false;
                      Exit;
     end;
     wm_Command:
        case WParam of
          101: begin  { New Game }
                    CurLevel := 0;
                    if RLL then begin   { Remember Last Level }
                      GetPrivateProfileString('DATA',psRLLL,'1',temps,5,INIFile);
                      val(temps,CurLevel,i); dec(CurLevel);
                    end;
                    LoadNextLevel(False);
                    Exit;
               end;
          102: begin { Toggle Sound Option }
                    Menu := GetMenu(Window);
                    if GetMenuState(menu,102,mf_ByCommand) and mf_Checked <> 0 then begin
                      CheckMenuItem(menu,102,mf_ByCommand);
                      Sound_On := false;
                      WritePrivateProfileString('OPT',psSound,'No',INIFile);
                    end else begin
                      CheckMenuItem(menu,102,mf_ByCommand or mf_Checked);
                      Sound_On := true;
                      WritePrivateProfileString('OPT',psSound,'Yes',INIFile);
                    end;
                    Exit;
               end;
          103: begin { Exit }
                    DestroyWindow(Window);
                    Exit;
               end;
          104: begin { Toggle Animation Option }
                    Menu := GetMenu(Window);
                    if GetMenuState(menu,104,mf_ByCommand) and mf_Checked <> 0 then begin
                      CheckMenuItem(menu,104,mf_ByCommand);
                      Ani_On := false;
                      WritePrivateProfileString('OPT',psAni,'No',INIFile);
                    end else begin
                      CheckMenuItem(menu,104,mf_ByCommand or mf_Checked);
                      Ani_On := true;
                      WritePrivateProfileString('OPT',psAni,'Yes',INIFIle);
                    end;
                    Exit;
               end;
          105: begin { ReStart }
                    if CurLevel > 0 then begin
                       Game.PF := CurRecData.PF;
                       BuildBMField;
                       InvalidateRect(Window,nil,false);
                       GameOn(true);
                       Game.RecP := 0;
                       if Recording then BackUp^[UndoP].Tank.Dir := 0; { disable ReStart Undo }
                    end;
                    Exit;
               end;
          106: begin { Load Level }                  
                    DProc := MakeProcInstance(@LoadBox,HInstance);
                    i := DialogBox(HInstance, 'LoadLev', Window, DProc);
                    FreeProcInstance(DProc);
                    if i > 100 then begin
                       CurLevel := i - 101;
                       LoadNextLevel(True);
                    end;
                    Exit;
               end;
          107: begin { Skip Level }
                    LoadNextLevel(False);
                    Exit;
               end;
          108: begin { Load Data File }
                    if GetOpenFileName(OFN) then begin
                       AssignHSFile;
                       CurLevel := 0;
                       LoadNextLevel(true);
                    end;
                    Exit;
               end;
          109: begin { Toggle Remember LL Option }
                    Menu := GetMenu(Window);
                    if GetMenuState(menu,109,mf_ByCommand) and mf_Checked <> 0 then begin
                      CheckMenuItem(menu,109,mf_ByCommand);
                      RLL := false;
                      WritePrivateProfileString('OPT',psRLLOn,'No',INIFile);
                    end else begin
                      CheckMenuItem(menu,109,mf_ByCommand or mf_Checked);
                      RLL := true;
                      WritePrivateProfileString('OPT',psRLLOn,'Yes',INIFile);
                    end;
                    Exit;
               end;
          110: begin { Undo }
                    if BackUp^[UndoP].Tank.Dir = 0 then exit;
                    Game := Backup^[UndoP];
                    Backup^[UndoP].Tank.Dir := 0;
                    dec(UndoP); if UndoP = 0 then UndoP := UndoDepth;
                    if BackUp^[UndoP].Tank.Dir = 0 then
                      EnableMenuItem(MMenu,110,mf_ByCommand or mf_Grayed); { disable Undo }
                    InvalidateRect(Window,nil,false);
               end;
          111: begin  { Save Position }
                    SaveGame := Game;
                    EnableMenuItem(MMenu,112,mf_ByCommand or mf_Enabled);
               end;
          112: begin  { Restore Position }
                    Game := SaveGame;
                    InvalidateRect(Window,nil,false);
               end;
          113: begin { View High Scores }
                    DProc := MakeProcInstance(@HSList,HInstance);
                    DialogBox(HInstance, 'HighList', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
               end;
          114: begin  { PlayBack Recording }
                  StrCopy(pbfn.lpstrTitle,'Open Playback File');
                  pbfn.Flags:= ofn_HideReadOnly or ofn_filemustexist;
                  if GetOpenFileName(PBFN) then begin
                     DProc := MakeProcInstance(@PBWindow,HInstance);
                     DialogBox(HInstance, 'PlayBox', Window, DProc);
                     FreeProcInstance(DProc);
                  end;
                  exit;
              end;
          115: begin { Toggle AutoRecord Option }
                    Menu := GetMenu(Window);
                    if GetMenuState(menu,115,mf_ByCommand) and mf_Checked <> 0 then begin
                      CheckMenuItem(menu,115,mf_ByCommand);
                      ARecord := false;
                      WritePrivateProfileString('OPT',psARec,'No',INIFile);
                      if GetMenuState(MMenu,123,mf_ByCommand) and mf_Checked
                       <> 0 then postmessage(Window,wm_Command,123,0); { Record Off}
                    end else begin
                      CheckMenuItem(menu,115,mf_ByCommand or mf_Checked);
                      ARecord := true;
                      WritePrivateProfileString('OPT',psARec,'Yes',INIFile);
                      if GetMenuState(MMenu,123,mf_ByCommand) and mf_Checked
                       = 0 then postmessage(Window,wm_Command,123,0); { Record On}
                    end;
                    Exit;
               end;
           116: begin { Toggle SKip Completed Levels Option }
                    Menu := GetMenu(Window);
                    if GetMenuState(menu,116,mf_ByCommand) and mf_Checked <> 0 then begin
                      CheckMenuItem(menu,116,mf_ByCommand);
                      SkipCL := false;
                      WritePrivateProfileString('OPT',psSCL,'No',INIFile);
                    end else begin
                      CheckMenuItem(menu,116,mf_ByCommand or mf_Checked);
                      SkipCL := true;
                      WritePrivateProfileString('OPT',psSCL,'Yes',INIFile);
                    end;
                    Exit;
               end;
          117:if Recording then postMessage(Window,wm_SaveRec,0,0);   { F6 - SaveRec }
          120: begin  { small size }
                 SetGameSize(1);
                 InvalidateRect(Window,nil,true);
               end;
          121: begin  { medium size }
                 SetGameSize(2);
                 InvalidateRect(Window,nil,true);
               end;
          122: begin  { large size }
                 SetGameSize(3);
                 InvalidateRect(Window,nil,true);
               end;
          123: begin { Toggle Record Option }
                    if GetMenuState(MMenu,123,mf_ByCommand) and mf_Checked <> 0 then begin
                      CheckMenuItem(MMenu,123,mf_ByCommand);
                      Recording := false;
                      freemem(Recbuffer,RecBufSize);
                      EnableMenuItem(MMenu,117,mf_ByCommand or mf_Grayed);
                      SetWindowText(MainH,APP_title);
                    end else begin
                      CheckMenuItem(MMenu,123,mf_ByCommand or mf_Checked);
                      Recording := true;
                      Getmem(RecBuffer,RecBufSize);
                      postmessage(Window,wm_Command,105,0); { Restart Level }
                      EnableMenuItem(MMenu,117,mf_ByCommand ); { enable Save Recording }
                      SetWindowText(MainH,REC_title);
                    end;
                    Exit;
               end;
          201: begin { Editor }
                    EditorOn := true;
                    GameOn(False);       { Shut down game engine }
                    if Recording then sendmessage(MainH,wm_Command,123,0);
                    OKtoHS := false;
                    Strcopy(temps,APP_Title); strcat(temps,' - [Editor]');
                    SetWindowText(Window,temps);  { Change window name }
                    SetMenu(Window,EMenu);        { Set up editor menu's }
                    ShowWindow(Ed1, SW_ShowNA);   { setup title window }
                    SetWindowText(Ed1,CurRecData.LName);
                    SendMessage(Ed1,em_SetModify,0,0);
                    ShowWindow(Ed2, SW_ShowNA);   { setup Author window }
                    SendMessage(Ed2,em_SetModify,0,0);
                    SetWindowText(Ed2,CurRecData.Author);
                    EditDiffSet(CurRecData.SDiff);
                    InvalidateRect(Window,nil,true);  { redraw in editor mode }
                    Drawing := false;
                    Modified := false;
                    if CurLevel = 0 then begin
                      CurLevel := 1;  { we can't come back to 0 }
                      PostMessage(Window,WM_Command,601,0); { New Level }
                    end;
                    Exit;
               end;
          225: begin { Change Difficulty - Normal Mode}
                    DProc := MakeProcInstance(@DiffBox,HInstance);
                    DialogBox(HInstance, 'DiffBox', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
               end;
          301: begin { Hint }
                    if Game_On then begin
                       GameOn(false);
                       Messagebox(window,CurRecData.Hint,'Laser Tank Hint',mb_ok or mb_iconInformation);
                       GameOn(true);
                    end;
                   Exit;
               end;
          601: with Game do begin  { New Level  [Editor] }
                    for x := 0 to 15 do for y := 0 to 15 do begin
                      PF[x,y] := 0;
                      BMF[x,y] := 1;
                      BMF2[x,y] := 1;
                      PF2[x,y] := 0;
                    end;
                    Tank.x := 7; tank.y := 15;
                    tank.dir := 1; tank.firing := false;
                    SetWindowText(Ed1,''); SendMessage(Ed1,em_SetModify,0,0);
                    SetWindowText(Ed2,''); SendMessage(Ed2,em_SetModify,0,0);
                    CurRecData.Hint[0] := #0;       { Clear Out Hint }
                    InvalidateRect(Window,nil,true);
                    Modified := true;
               end;
          602: begin   { Load Level [Editor] }
                    if (SendMessage(ed1,em_GetModify,0,0) <> 0) or
                       (SendMessage(ed2,em_GetModify,0,0) <> 0) then Modified := true;   
                    if Modified and (messagebox(Window,'Level NOT Saved, Save Data ?',
                       'LTank Editor',mb_YesNo or mb_iconquestion) = id_Yes) then
                       SendMessage(Window,wm_Command,603,0); 
                    strcopy(temps,filename);  { Save old name in case we cancel }
                    if GetOpenFileName(Ofn) then begin
                       AssignHSFile;
                       DProc := MakeProcInstance(@LoadBox,HInstance);
                       i := DialogBox(HInstance, 'LoadLev', Window, DProc);
                       FreeProcInstance(DProc);
                       if i > 100 then begin
                          i := i - 101;
                          Assign(File1,filename); Reset(File1);
                          Seek(File1,i); Read(File1,CurRecData);
                          SetWindowText(Ed1,CurRecData.LName);
                          SendMessage(Ed1,em_SetModify,0,0);
                          SetWindowText(Ed2,CurRecData.Author);
                          SendMessage(Ed2,em_SetModify,0,0);
                          CurLevel := i + 1;
                          Game.PF := CurRecData.PF;
                          BuildBMField;
                          Modified := false;
                          Close(File1);
                          InvalidateRect(Window,nil,true);
                       end else strcopy(filename,temps);
                    end;
               end;
          603: begin  { Save Level [Editor] }
                    if GetSaveFileName(Ofn) then begin
                       if StrScan(filename,'.') = nil then Strcat(FileName,'.lvl');
                       Assign(File1,filename); Reset(File1);
                       If IOResult <> 0 then ReWrite(File1);
                       AssignHSFile;
                       DProc := MakeProcInstance(@PickBox,HInstance);
                       i := DialogBox(HInstance, 'PickLevel', Window, DProc);
                       FreeProcInstance(DProc);
                       if (i>0) and ((i-1) <= FileSize(File1)) then begin
                          GetWindowText(Ed1,CurRecData.LName,30);
                          CurRecData.PF := Game.PF;
                          CurRecData.PF[game.tank.x,Game.tank.y] := 1; { save Tank in File}
                          GetWindowText(Ed2,CurRecData.Author,30);
                          Seek(File1,i-1); Write(File1,CurRecData);
                          Modified := false;
                          SendMessage(ed1,em_SetModify,0,0);
                          SendMessage(ed2,em_SetModify,0,0);
                          CurLevel := i;
                          if RLL then begin   { Remember Last Level }
                            str(CurLevel,temps);
                            WritePrivateProfileString('DATA',psRLLL,temps,INIFile);
                            WritePrivateProfileString('DATA',psRLLN,FileName,INIFile);
                          end;
                       end;
                       Close(File1);
                       if HSClear then begin
                          AssignHSFile;
                          Reset(File2);
                          if ioresult = 0 then begin
                            if CurLevel <= FileSize(File2) then begin    
                              HS.moves := 0;
                              seek(File2,CurLevel-1);
                              write(File2,HS);
                            end;
                            close(File2);
                          end;
                       end;
                    end;
               end;
          604: begin  { Leave Editor }
                    if (SendMessage(ed1,em_GetModify,0,0) <> 0) or
                       (SendMessage(ed2,em_GetModify,0,0) <> 0) then Modified := true;   
                    CurRecData.PF := Game.PF;
                    CurRecData.PF[game.tank.x,Game.tank.y] := 1; { save Tank in File}
                    if Modified and (messagebox(Window,'Level NOT Saved, Save Data ?',
                       'LTank Editor',mb_YesNo or mb_iconquestion) = id_Yes) then
                       SendMessage(Window,wm_Command,603,0); 
                    ShowWindow(Ed1, SW_Hide);
                    ShowWindow(Ed2, SW_Hide);
                    EditorOn := false;
                    SetWindowText(Window,APP_Title);
                    SetMenu(Window,MMenu);
                    GameOn(True);
                    InvalidateRect(Window,nil,true);
                    Exit;
               end;
          605: begin { Hint [Editor] }
                    DProc := MakeProcInstance(@HintBox,HInstance);
                    DialogBox(HInstance, 'HintBox', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
               end;
          701: EditDiffSet(1);  { Edit Difficulty Set - Kids }
          702: EditDiffSet(2);  { Edit Difficulty Set - Easy }
          703: EditDiffSet(4);  { Edit Difficulty Set - Medium }
          704: EditDiffSet(8);  { Edit Difficulty Set - Hard }
          705: EditDiffSet(16); { Edit Difficulty Set - Deadly }
          901: begin { About Box }
                    DProc := MakeProcInstance(@AboutBox,HInstance);
                    DialogBox(HInstance, 'AboutBox', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
               end;
          902: begin
                    WinHelp(Window,'LTank.Hlp',Help_Index,0);
                    Exit;
               end;
          903: begin
                    StrCopy(temps,'Editor Index');
                    WinHelp(Window,'LTank.Hlp',Help_Key,longint(@temps));
                    Exit;
               end;
          904: begin
                    StrCopy(temps,'Revisions');
                    WinHelp(Window,'LTank.Hlp',Help_Key,longint(@temps));
                    Exit;
               end;
          905: begin
                    StrCopy(temps,'Writing Your Own Levels');
                    WinHelp(Window,'LTank.Hlp',Help_Key,longint(@temps));
                    Exit;
               end;
          906: begin { View Global High Scores }
                    DProc := MakeProcInstance(@GHSList,HInstance);
                    DialogBox(HInstance, 'GHighList', Window, DProc);
                    FreeProcInstance(DProc);
                    Exit;
               end;
     end; { case WParam of wm_Command }
   end; { case Message }
  MainWindowProc := DefWindowProc(Window, Message, WParam, LParam);
end;

procedure WinMain;
var
  Window: HWnd;
  Message: TMsg;
  gs,xp,yp: integer;
const
  WindowClass: TWndClass = (
    style: cs_ByteAlignWindow or cs_SaveBits;
    lpfnWndProc: @MainWindowProc;
    cbClsExtra: 0;
    cbWndExtra: 0;
    hInstance: 0;
    hIcon: 0;
    hCursor: 0;
    hbrBackground: 0;
    lpszMenuName: 'Main';
    lpszClassName: 'LaserTankClass');
begin
  Ctl3dRegister(HInstance);
  Ctl3dAutoSubclass(HInstance);
  if HPrevInst = 0 then begin
    WindowClass.hInstance := HInstance;
    WindowClass.hIcon := LoadIcon(HInstance, 'ICON1');
    WindowClass.hCursor := LoadCursor(0, idc_Arrow);
    WindowClass.hbrBackground := GetStockObject(LtGray_Brush);
    if not RegisterClass(WindowClass) then Halt(255);
  end;
  xp := GetPrivateProfileInt('SCREEN',psXpos,cw_UseDefault,INIFIle);
  yp := GetPrivateProfileInt('SCREEN',psYpos,cw_UseDefault,INIFile);

  Window := CreateWindow('LaserTankClass',APP_Title,
            ws_OverlappedWindow and (not (ws_sizebox or ws_Maximizebox)),
            xp,yp, 594, 447,
            0, 0, HInstance, nil);
  MainH := Window;
  MMenu := GetMenu(Window);
  Ofn.hWndOwner := Window;
  PBfn.hWndOwner := Window;
  EMenu := LoadMenu(Hinstance,'MENU2');
  { Title Edit Box - Visable only in Editor Mode }
  Ed1 := CreateWindow('EDIT','',ws_Child or ws_border or es_Left or es_AutoHScroll,
         418,99,163,20,Window,501,hinstance,nil);  
  { Author Edit Box - Visable only in Editor Mode }
  Ed2 := CreateWindow('EDIT','',ws_Child or ws_border or es_Left or es_AutoHScroll,
         418,148,163,20,Window,502,hinstance,nil); 
  MyFont := CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,'Arial');
  gs := GetPrivateProfileInt('SCREEN',psSize,1,INIFile);
  SetGameSize(gs);
  ShowWindow(Window, CmdShow);
  UpdateWindow(Window);
  if ParamCount > 0 then begin
     strpcopy(filename,ParamStr(1));
     AssignHSFile;
     CurLevel := 0;
     LoadNextLevel(False);
  end;
  while GetMessage(Message, 0, 0, 0) do
  begin
    TranslateMessage(Message);
    DispatchMessage(Message);
  end;
  Ctl3dUnregister(HInstance);
  DestroyMenu(EMenu);
  DeleteObject(MyFont);
  Halt(Message.wParam);
end;

begin
  WinMain;
end.
