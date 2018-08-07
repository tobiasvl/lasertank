/*******************************************************
 **             LaserTank ver 4.0                     **
 **               By Jim Kindley                      **
 **               (c) 2001                            **
 **         The Program and Source is Public Domain   **
 **                                                   **
 **   Dialog Code                                     **
 **                                                   **
 *******************************************************
 **       Release version 2005 by Yves Maingoy        **
 **               ymaingoy@free.fr                    **
 *******************************************************/
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <ole2.h>
#include <shlobj.h>
#include "ltank.h"
#include "ltank_d.h"

#define PHDC (pDIS->hDC)
#define PRC  (pDIS->rcItem)

const long DifCList[6] =  {0x00FFFFFF,0x0000FFFF,0x00FFFF00,0x0000FF00,0x00FF00FF,0x000000FF};

HBRUSH hbrBack;
DRAWITEMSTRUCT FAR *pDIS;
int HSClear,LastOfList,DispAll;
THSREC HS, GHS;
TSEARCH SearchRec;
HWND PBCountH,PlayH;
extern int QHELP;
DWORD OldProcEdit;

// Will Set the Starting Directory for the Browsing
INT CALLBACK BrowseCallBack (HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
  if (uMsg == BFFM_INITIALIZED)
    SendMessage(hwnd,BFFM_SETSELECTION, TRUE, (LPARAM)&GraphDN);
  return(0);
}

/************************************************************************************
Browse
Action: Uses SHBrowseForFolder to allow the user to browse their shell namespace
Paramaters: hwndOwner - handle to owner window
           lpszDir - pointer to string that will be filled with the user's selection
                     Assumed to be MAX_PATH
           lpszTitle - any text that you would like to be displayed in the window
Return value: TRUE if successful, FALSE if there was a problem
************************************************************************************/
BOOL Browse(HWND hwndOwner,LPTSTR lpszDir,LPCTSTR lpszTitle)
{
  BROWSEINFO bi;
  LPITEMIDLIST pidl;
  IMalloc* pMalloc;

  OleInitialize(NULL);

  if(FAILED(SHGetMalloc(&pMalloc))) return FALSE;

  // fill BROWSEINFO struct
  bi.hwndOwner = hwndOwner;   // owner window
  bi.pidlRoot = NULL;         // folder to start in - NULL for My Computer
  bi.pszDisplayName = lpszDir;  // pointer to string that gets folder display name
  bi.lpszTitle = lpszTitle;   // pointer to text to display in window
  bi.ulFlags = BIF_RETURNONLYFSDIRS;  // flags - see API docs for BROWSEINFO
  bi.lpfn = BrowseCallBack;       // callback function - usually not needed
  bi.lParam = 0;              // parameter to callback function

  pidl = SHBrowseForFolder(&bi);

  if(!pidl)
     return FALSE;

  // SHBrowseForFolder filled lpszDir with the name of the chosen item,
  // but it did not give us the full path to it. So, we need to get
  // that using SHGetPathFromIDList.
  if(!SHGetPathFromIDList(pidl,lpszDir)) return FALSE;
  pMalloc->lpVtbl->Free(pMalloc,pidl);
  pMalloc->lpVtbl->Release(pMalloc);
  return TRUE;
}


// Used By Load, HighScore & Global High Score ListBoxs
int TransListKey( HWND Dialog, INT Key  )
{
  int i;

  switch (Key)
  {
  case VK_HOME: SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,0,0);
    break;
  case VK_UP: i = SendDlgItemMessage(Dialog,ID_LISTBOX,LB_GETCURSEL,0,0) - 1;
        if (i > -1) SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,i,0);
    break;
  case VK_DOWN: i = SendDlgItemMessage(Dialog,ID_LISTBOX,LB_GETCURSEL,0,0) + 1;
        if (i <= LastOfList) SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,i,0);
    break;
  case VK_END:SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,LastOfList,0);
    break;
  case VK_PRIOR: i = SendDlgItemMessage(Dialog,ID_LISTBOX,LB_GETCURSEL,0,0) - 10;
        if (i > -1) SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,i,0);
    break;
  case VK_NEXT: i = SendDlgItemMessage(Dialog,ID_LISTBOX,LB_GETCURSEL,0,0) + 10;
        if (i <= LastOfList) SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,i,0);
    break;
  case VK_ESCAPE: EndDialog(Dialog,0);
  }
  return(-2);
}
// ---- About Box Dialog Procedure ----
//  This is an About box with an Edit window that has text in it from the
//  resource file. The text is stored as RCData.
LRESULT CALLBACK AboutBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  char *p;
  char AboutMsg[SIZE_ABOUTMSG*MAX_LANG_SIZE+1];
  int i;
  HGLOBAL h;

  switch (Message)
  {
       case WM_INITDIALOG:
            // Grab the Text for the About Edit window
            h = LoadResource(hInst,FindResource(hInst,"About",RT_RCDATA));
            if ( LANGText[OFFSET_ABOUTMSG][0] != '\0' )
            {
               AboutMsg[0]='\0';
               for (i=0; i<SIZE_ABOUTMSG; i++)
                    strcat(AboutMsg,LANGText[OFFSET_ABOUTMSG + i]);
               p = AboutMsg;
            }
            else
               p = LockResource(h);

            LoadWindowCaption(Dialog, ID_ABOUTBOX_00);
            LoadWindowText(Dialog,    ID_ABOUTBOX_OK);
            LoadWindowText(Dialog,    ID_ABOUTBOX_04);
            LoadWindowText(Dialog,    ID_ABOUTBOX_06);

            SetWindowText(GetDlgItem(Dialog,ID_ABOUTBOX_02), App_Title);
            SetWindowText(GetDlgItem(Dialog,ID_ABOUTBOX_03), App_Version);
            SetWindowText(GetDlgItem(Dialog,ID_ABOUTBOX_07), p);
            FreeResource(h);
            return(TRUE);
      case WM_COMMAND: if (wparam == ID_ABOUTBOX_OK) EndDialog(Dialog,1);
  }
  return(0);
}




// ---- DeadBox Dialog Procedure ----
// This dialog Proc is used for the DeadBox dialog.
// It returns the value of the button pressed.}
LRESULT CALLBACK DeadBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowText(Dialog,    ID_DEADBOX_UNDO);
      LoadWindowText(Dialog,    ID_DEADBOX_DEAD);
      LoadWindowText(Dialog,    ID_DEADBOX_RESTART);
      return(TRUE); // I need this or I don't kave [tab] key }
    case WM_COMMAND:
      if (Game.RecP > 1) EndDialog(Dialog,wparam);  // end dialog with button id }
      else EndDialog(Dialog, ID_DEADBOX_RESTART);     // reset if first turn
  }
  return(0);
}


// ---- RetBox Dialog Procedure ----
// This dialog Proc is used for "Return to Game".
// It returns the value of the button pressed.}
//function DefBox(Dialog: HWnd; Message, WParam: Word;LParam: Longint): Bool; export;
LRESULT CALLBACK RetBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowText(Dialog,    ID_RETBOX_00);
      return(TRUE);            // I need this or I don't kave [tab] key }
    case WM_COMMAND:
      if (Game.RecP > 1) EndDialog(Dialog,wparam);   // end dialog with button id }
      else EndDialog(Dialog,2);             // reset if first turn
  }
  return(0);
}


// ---- Search Box Dialog Procedure ----
//  The Load Box uses this proc to select a Search term and type
LRESULT CALLBACK SearchBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  int i,j;

  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_SEARCH_00);
      LoadWindowText(Dialog,    ID_SEARCH_01);
      LoadWindowText(Dialog,    ID_SEARCH_02);
      LoadWindowText(Dialog,    ID_SEARCH_03);
      LoadWindowText(Dialog,    ID_SEARCH_04);
      LoadWindowText(Dialog,    ID_SEARCH_05);
      LoadWindowText(Dialog,    ID_SEARCH_06);

      LoadWindowText(Dialog,    ID_SEARCH_08);
      LoadWindowText(Dialog,    ID_SEARCH_09);
      LoadWindowText(Dialog,    ID_SEARCH_10);
      LoadWindowText(Dialog,    ID_SEARCH_11);
      LoadWindowText(Dialog,    ID_SEARCH_12);
      LoadWindowText(Dialog,    ID_SEARCH_13);
      LoadWindowText(Dialog,    ID_SEARCH_14);

      SendMessage(GetDlgItem(Dialog,ID_SEARCH_03),BM_SETCHECK,1,0);
      SearchRec.mode = 1;
      return(TRUE);
    case WM_COMMAND:
      switch (LOWORD(wparam))
      {
        case ID_SEARCH_06:
        case 1:
          GetWindowText(GetDlgItem(Dialog,ID_SEARCH_07),SearchRec.data,60);
          strupr(SearchRec.data);
          if (SendMessage(GetDlgItem(Dialog,ID_SEARCH_03),BM_GETCHECK,0,0) != 0) SearchRec.mode = 1;
          else SearchRec.mode = 2;
          if (SendMessage(GetDlgItem(Dialog,ID_SEARCH_09),BM_GETCHECK,0,0))
          {
            SearchRec.Diff = 0;
            if (SendDlgItemMessage(Dialog,ID_SEARCH_10,BM_GETCHECK,0,0) == 1) SearchRec.Diff++;
            if (SendDlgItemMessage(Dialog,ID_SEARCH_11,BM_GETCHECK,0,0) == 1) SearchRec.Diff = SearchRec.Diff + 2;
            if (SendDlgItemMessage(Dialog,ID_SEARCH_12,BM_GETCHECK,0,0) == 1) SearchRec.Diff = SearchRec.Diff + 4;
            if (SendDlgItemMessage(Dialog,ID_SEARCH_13,BM_GETCHECK,0,0) == 1) SearchRec.Diff = SearchRec.Diff + 8;
            if (SendDlgItemMessage(Dialog,ID_SEARCH_14,BM_GETCHECK,0,0) == 1) SearchRec.Diff = SearchRec.Diff + 16;
          }
          else SearchRec.Diff = 255;
          SearchRec.SkipComp = SendMessage(GetDlgItem(Dialog,200),BM_GETCHECK,0,0);
          EndDialog(Dialog,1);
          break;
        case 2:
        case ID_SEARCH_05: EndDialog(Dialog,2);
          break;
        case ID_SEARCH_09:
          j = SendMessage(GetDlgItem(Dialog,ID_SEARCH_09),BM_GETCHECK,0,0);
          for (i=ID_SEARCH_10; i<= ID_SEARCH_14; i++) EnableWindow(GetDlgItem(Dialog,i),j);
      }
  }
  return(0);
}



// ---- Difficulty Box Dialog Procedure ----
LRESULT CALLBACK DiffBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  char temps[40];
  switch (Message)
  {
    case WM_INITDIALOG:
       if ((Difficulty & 1) == 1)   SendDlgItemMessage(Dialog,ID_DIFFBOX_02,BM_SETCHECK,1,0);
       if ((Difficulty & 2) == 2)   SendDlgItemMessage(Dialog,ID_DIFFBOX_03,BM_SETCHECK,1,0);
       if ((Difficulty & 4) == 4)   SendDlgItemMessage(Dialog,ID_DIFFBOX_04,BM_SETCHECK,1,0);
       if ((Difficulty & 8) == 8)   SendDlgItemMessage(Dialog,ID_DIFFBOX_05,BM_SETCHECK,1,0);
       if ((Difficulty & 16) == 16) SendDlgItemMessage(Dialog,ID_DIFFBOX_06,BM_SETCHECK,1,0);

       LoadWindowCaption(Dialog, ID_DIFFBOX_00);
       LoadWindowText(Dialog,    ID_DIFFBOX_01);
       LoadWindowText(Dialog,    ID_DIFFBOX_02);
       LoadWindowText(Dialog,    ID_DIFFBOX_03);
       LoadWindowText(Dialog,    ID_DIFFBOX_04);
       LoadWindowText(Dialog,    ID_DIFFBOX_05);
       LoadWindowText(Dialog,    ID_DIFFBOX_06);
       LoadWindowText(Dialog,    ID_DIFFBOX_07);
       LoadWindowText(Dialog,    ID_DIFFBOX_08);
       return(TRUE);
    case WM_COMMAND:
     switch (wparam)
     {
       case 1:
       case ID_DIFFBOX_01:
          Difficulty = 0;
          if (SendDlgItemMessage(Dialog,ID_DIFFBOX_02,BM_GETCHECK,0,0) == 1) Difficulty++;
          if (SendDlgItemMessage(Dialog,ID_DIFFBOX_03,BM_GETCHECK,0,0) == 1) Difficulty = Difficulty + 2;
          if (SendDlgItemMessage(Dialog,ID_DIFFBOX_04,BM_GETCHECK,0,0) == 1) Difficulty = Difficulty + 4;
          if (SendDlgItemMessage(Dialog,ID_DIFFBOX_05,BM_GETCHECK,0,0) == 1) Difficulty = Difficulty + 8;
          if (SendDlgItemMessage(Dialog,ID_DIFFBOX_06,BM_GETCHECK,0,0) == 1) Difficulty = Difficulty + 16;
          itoa(Difficulty,temps,10);
          WritePrivateProfileString("DATA",psDiff,temps,INIFile);
          if (Difficulty > 0) EndDialog(Dialog,0);
          break;
       case ID_DIFFBOX_07:
          SendDlgItemMessage(Dialog,ID_DIFFBOX_02,BM_SETCHECK,1,0);
          SendDlgItemMessage(Dialog,ID_DIFFBOX_03,BM_SETCHECK,1,0);
          SendDlgItemMessage(Dialog,ID_DIFFBOX_04,BM_SETCHECK,1,0);
          SendDlgItemMessage(Dialog,ID_DIFFBOX_05,BM_SETCHECK,1,0);
          SendDlgItemMessage(Dialog,ID_DIFFBOX_06,BM_SETCHECK,1,0);
    }
  }
  return(0);
}


// ---- Load Box Dialog Procedure ----
//  List out all levels in the current data file, by number & title.
//  return with the level # + 101 or 0 if canceled}
LRESULT CALLBACK LoadBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  TLEVEL TempRecData;
  THSREC TempHSData;
  char temps[80],t2[90];
  char *P;
  int i,j;


  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_LOADLEV_00);
      LoadWindowText(Dialog,    ID_LOADLEV_01);
      LoadWindowText(Dialog,    ID_LOADLEV_03);
      LoadWindowText(Dialog,    ID_LOADLEV_04);
      LoadWindowText(Dialog,    ID_LOADLEV_05);
      LoadWindowText(Dialog,    ID_LOADLEV_06);
      LoadWindowText(Dialog,    ID_LOADLEV_07);
      F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (F1 == INVALID_HANDLE_VALUE) return(TRUE); // File NOT Found
        P = strrchr(FileName,'\\');
      if (P ==NULL) P = FileName;
      else P++;
      SetWindowText(GetDlgItem(Dialog,ID_LOADLEV_05),P);
      i = SetFilePointer(F1,0,NULL,FILE_END) / sizeof(TLEVEL);
      SendDlgItemMessage(Dialog, ID_LOADLEV_08,LB_INITSTORAGE,i,72);
      SetFilePointer(F1,0,NULL,FILE_BEGIN);
      i = 1;
      ReadFile(F1, &TempRecData, sizeof(TLEVEL), &BytesMoved, NULL);
      while (BytesMoved == sizeof(TLEVEL))
      {
        sprintf(temps,"%4d   %-30.30s %s",i,TempRecData.LName,TempRecData.Author);
        switch (TempRecData.SDiff)
        {
          case 1: t2[0] = '1'; break;
          case 2: t2[0] = '2'; break;
          case 4: t2[0] = '3'; break;
          case 8: t2[0] = '4'; break;
          case 16: t2[0]= '5'; break;
          default: t2[0]= '0';
        }
        strcpy(t2+1,temps);
        SendDlgItemMessage(Dialog,ID_LOADLEV_08,LB_ADDSTRING,0,(LPARAM)t2);
        i++;
        ReadFile(F1, &TempRecData, sizeof(TLEVEL), &BytesMoved, NULL);
      }
      LastOfList = i-2;
      CloseHandle(F1);
      SendDlgItemMessage(Dialog,ID_LOADLEV_08,LB_SETCURSEL,CurLevel-1,0);
      return(TRUE);
    case WM_CTLCOLORLISTBOX:
      return ((int)GetStockObject(BLACK_BRUSH));
      break;
    case WM_MEASUREITEM:
      ((MEASUREITEMSTRUCT FAR *)lparam)->itemHeight = 14;
      break;
    case WM_DRAWITEM:
      DrawLevels(lparam);
      return TRUE;
    case WM_VKEYTOITEM:
      return (TransListKey(Dialog,LOWORD(wparam)));
    case WM_COMMAND:
      switch (LOWORD(wparam))
      {
        case 1: // Pressing Enter will get you here
          GetWindowText(GetDlgItem(Dialog,ID_LOADLEV_02),temps,10);
          i = atoi(temps);
          if (i)
          {
            EndDialog(Dialog,i+100);
            break;
          }
        case ID_LOADLEV_08:
          if (((HIWORD(wparam) != LBN_SELCHANGE)) && (wparam != 1)) return(0);
          i = SendDlgItemMessage(Dialog,ID_LOADLEV_08,LB_GETCURSEL,0,0);
          SendDlgItemMessage(Dialog,ID_LOADLEV_08,LB_GETTEXT,i,(LPARAM)temps);
          temps[5] = (char)0;
          i = 1;
          while (temps[i] == ' ') i++; // strip leading spaces
          i = atoi(&temps[i]);
          EndDialog(Dialog,i+100);    // Add 100 to the Select #
          break;
        case ID_LOADLEV_01:
          EndDialog(Dialog,0);
          break;
        case ID_LOADLEV_03: // Search
          i = DialogBox(hInst, "Search", Dialog, (DLGPROC)SearchBox);
          if (i== 1) // idOk
          {
            SendDlgItemMessage(Dialog,ID_LOADLEV_08,LB_RESETCONTENT,0,0);
            F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
            FILE_FLAG_SEQUENTIAL_SCAN, NULL);
            if (SearchRec.SkipComp) F2 = CreateFile(HFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
            else F2 = INVALID_HANDLE_VALUE;
            if (F2 == INVALID_HANDLE_VALUE) SearchRec.SkipComp = FALSE;
            ReadFile(F1, &TempRecData, sizeof(TLEVEL), &BytesMoved, NULL);
            LastOfList = -1;
            while (BytesMoved == sizeof(TLEVEL))
            {
              if (SearchRec.SkipComp) ReadFile(F2,&TempHSData,sizeof(THSREC), &BytesMoved, NULL);
              if (BytesMoved < sizeof(THSREC)) TempHSData.moves = 0;
              if (SearchRec.mode == 1) strcpy(temps,TempRecData.LName);
              else strcpy(temps,TempRecData.Author);
              if (TempRecData.SDiff == 0) TempRecData.SDiff = 255;
              if (strstr(strupr(temps),SearchRec.data) && (TempRecData.SDiff & SearchRec.Diff )
                 && (( SearchRec.SkipComp == 0) || ( TempHSData.moves == 0)) )
              {
                sprintf(temps,"%4d   %-30.30s %s",i,TempRecData.LName,TempRecData.Author);
                switch (TempRecData.SDiff)
                {
                  case 1: t2[0] = '1'; break;
                  case 2: t2[0] = '2';break;
                  case 4: t2[0] = '3';break;
                  case 8: t2[0] = '4';break;
                  case 16: t2[0] = '5';break;
                  default: t2[0] = '0';
                }
                strcpy(t2+1,temps);
                SendDlgItemMessage(Dialog,ID_LOADLEV_08,LB_ADDSTRING,0,(LPARAM)t2);
                LastOfList++;
              }
              i++;
              ReadFile(F1, &TempRecData, sizeof(TLEVEL), &BytesMoved, NULL);
            }
            CloseHandle(F1);
            if (SearchRec.SkipComp) CloseHandle(F2);
          }
        }
      }
      return(0);
}



// ---- Hint Box Dialog Procedure ----
//  The editor uses this proc to Display & Edit the Hint. If the text is modified
//  the 'Modified' varable is set.}
//function HintBox(Dialog: HWnd; Message, WParam: Word; LParam: Longint): Bool; export;
LRESULT CALLBACK HintBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  switch (Message)
  {
    case WM_INITDIALOG:
         LoadWindowCaption(Dialog, ID_HINTBOX_00);
         LoadWindowText(Dialog,    ID_HINTBOX_01);
         LoadWindowText(Dialog,    ID_HINTBOX_02);
         SetWindowText(GetDlgItem(Dialog,ID_HINTBOX_03),CurRecData.Hint);
         return(TRUE);
    case WM_COMMAND:
         switch (wparam)
     {
       case 1:
       case ID_HINTBOX_01:
          GetWindowText(GetDlgItem(Dialog,ID_HINTBOX_03),CurRecData.Hint,255);
          if (SendMessage(GetDlgItem(Dialog,ID_HINTBOX_03),EM_GETMODIFY,0,0)) Modified = TRUE;
       case 2:
       case ID_HINTBOX_02:
          EndDialog(Dialog,2);
     }
  }
  return(0);
}


// Dont Process Space key
LRESULT CALLBACK EditProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  if ((iMsg == WM_CHAR) && (wParam == VK_SPACE)) return(0);
  return( CallWindowProc( (WNDPROC)OldProcEdit, hwnd, iMsg, wParam, lParam));
}


// ---- Win Box Dialog Procedure ----
//  This is an Win box with an Edit window that has text in it from the
//  resource file. The text is stored as RCData.}
LRESULT CALLBACK WinBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  HGLOBAL H;
  char *P;
  int x,y,i;
  RECT Box;
  THSREC TempHSData;
  char temps[100];
  char AboutMsg[SIZE_ABOUTMSG*MAX_LANG_SIZE+1];

    switch (Message)
  {
    case WM_INITDIALOG:
    LoadWindowText(Dialog,    ID_WINBOX_00);
    LoadWindowText(Dialog,    ID_WINBOX_01);
    LoadWindowText(Dialog,    ID_WINBOX_02);
    LoadWindowText(Dialog,    ID_WINBOX_03);

    H = LoadResource(hInst,FindResource(hInst,"About",RT_RCDATA));
    if ( LANGText[OFFSET_ABOUTMSG][0] != '\0' )
    {
       AboutMsg[0]='\0';
       for (i=0; i<SIZE_ABOUTMSG; i++)
            strcat(AboutMsg,LANGText[OFFSET_ABOUTMSG + i]);
       P = AboutMsg;
    }
    else
       P = LockResource(H);
    SetWindowText(GetDlgItem(Dialog,ID_WINBOX_06),P);

    P = strrchr(FileName,'\\');
    if (P) P++;
    else P = FileName;
    SetWindowText(GetDlgItem(Dialog,ID_WINBOX_03),P);
    FreeResource(H);
    SetFocus(GetDlgItem(Dialog,ID_WINBOX_00));
    F2 = CreateFile(HFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (F2 == INVALID_HANDLE_VALUE) return(TRUE);
    y = 0;
    ReadFile(F2, &TempHSData, sizeof(THSREC), &BytesMoved, NULL);
    while (BytesMoved == sizeof(THSREC))
    {
      if (TempHSData.moves > 0) y++;
      ReadFile(F2, &TempHSData, sizeof(THSREC), &BytesMoved, NULL);
    }
    CloseHandle(F2);

    F1 = CreateFile(FileName,GENERIC_READ ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
            FILE_FLAG_SEQUENTIAL_SCAN,NULL);
    x = SetFilePointer(F1,0,NULL,FILE_END) / sizeof(TLEVEL);
    CloseHandle(F1);

    sprintf(temps, txt042,y,x);  // "Completed %d of %d levels"
    SetWindowText(GetDlgItem(Dialog,ID_WINBOX_04),temps);
    if ((Difficulty & 0x1F) != 0x1F)
    {
      strcpy(temps, txt043); // "Difficulty Set ="
      P = temps + strlen(temps);
      strcat(temps,  "         ");
      if ((Difficulty & 1) == 1)  { P[0] = txt044[0]; P+=2; } // txt044="KEMHD"
      if ((Difficulty & 2) == 2)  { P[0] = txt044[1]; P+=2; }
      if ((Difficulty & 4) == 4)  { P[0] = txt044[2]; P+=2; }
      if ((Difficulty & 8) == 8)  { P[0] = txt044[3]; P+=2; }
      if ((Difficulty & 16) == 16){ P[0] = txt044[4];       }
      SetWindowText(GetDlgItem(Dialog,ID_WINBOX_05),temps);
    }
     return(TRUE);
  case WM_ERASEBKGND:
    GetClientRect(Dialog,&Box);
    SelectObject((HDC)wparam,GetStockObject(BLACK_BRUSH));
    Rectangle((HDC)wparam,Box.left,Box.top,Box.right,Box.bottom);
    return(TRUE);
  case WM_CTLCOLORSTATIC:
    SetTextColor((HDC)wparam,0x0000FFFF);
    SetBkColor((HDC)wparam,0);
    return((LONG)GetStockObject(BLACK_BRUSH));
  case WM_COMMAND: if ( (wparam == ID_WINBOX_00)||(wparam == 1) ) EndDialog(Dialog,1);
    }
  return(0);
}

// ---- Load Tunnel ID Dialog Procedure ----
// The editor uses this to pick which level ID number to assign
// to the current tunnel.
LRESULT CALLBACK LoadTID (HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  char temps[40];
  int i;

  switch (Message)
  {
  case WM_INITDIALOG:
    LoadWindowCaption(Dialog, ID_LOADTID_00);
    LoadWindowText(Dialog,    ID_LOADTID_03);
    LoadWindowText(Dialog,    ID_LOADTID_04);
    return(TRUE);
  case WM_COMMAND:
    if ((wparam == ID_LOADTID_03) || (wparam == 1) ) // idOk
    {
      GetWindowText(GetDlgItem(Dialog,ID_LOADTID_01),temps,10);
      i = atoi(temps);
      if (i > 7) i = 0;
      EndDialog(Dialog,i);
      return(TRUE);
    }
  }
  return(0);
}

// ---- HS Box Dialog Procedure ---- HighBox
//  This is the New High Score box
LRESULT CALLBACK HSBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  char temps[20], old[40];

  switch(Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_HIGHBOX_00);
      LoadWindowText(Dialog,    ID_HIGHBOX_01);
      LoadWindowText(Dialog,    ID_HIGHBOX_02);
      LoadWindowText(Dialog,    ID_HIGHBOX_03);
      LoadWindowText(Dialog,    ID_HIGHBOX_04);
      LoadWindowText(Dialog,    ID_HIGHBOX_05);
      LoadWindowText(Dialog,    ID_HIGHBOX_10);
      LoadWindowText(Dialog,    ID_HIGHBOX_12);
      LoadWindowText(Dialog,    ID_HIGHBOX_13);

      itoa(CurLevel,temps,10);
      SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_08),temps);
      SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_09),CurRecData.LName);
      itoa(Game.ScoreMove,temps,10);
      SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_06),temps);
      itoa(Game.ScoreShot,temps,10);
      SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_07),temps);
      if (HS.moves > 0)
      {
        strcpy(old,txt008);
        itoa(HS.moves,temps,10); strcat(old,temps); strcat(old,txt010);
        itoa(HS.shots,temps,10); strcat(old,temps); strcat(old,txt011);
        strcat(old,HS.name);
        ShowWindow(GetDlgItem(Dialog,ID_HIGHBOX_15),SW_SHOW);
        SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_15),old);
      }
      GetPrivateProfileString("DATA",psUser,"",HS.name,5,INIFile);
      SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_14),HS.name);
      F3 = CreateFile(GHFileName,GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
      SetFilePointer(F3,(CurLevel-1)*sizeof(GHS),NULL,FILE_BEGIN);
      ReadFile(F3, &GHS, sizeof(GHS), &BytesMoved, NULL);
      if (BytesMoved == sizeof(GHS))
      {
      if ((GHS.moves == 0) || (Game.ScoreMove < GHS.moves) ||
          ((Game.ScoreMove == GHS.moves) && (Game.ScoreShot < GHS.shots)))
      {
        SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_13),txt012);
        EnableWindow(GetDlgItem(Dialog,ID_HIGHBOX_13),TRUE);
      } else
      {
        strcpy(old,txt009);
        itoa(GHS.moves,temps,10); strcat(old,temps); strcat(old,txt010);
        itoa(GHS.shots,temps,10); strcat(old,temps); strcat(old,txt011);
        strcat(old,GHS.name);
        SetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_13),old);
      }
    }
    CloseHandle(F3);
    SetFocus(GetDlgItem(Dialog,ID_HIGHBOX_01));
    return(TRUE);
  case WM_COMMAND:
    if ((wparam == ID_HIGHBOX_01) || (wparam == 1))
    {
      GetWindowText(GetDlgItem(Dialog,ID_HIGHBOX_14),temps,5);
      HS.moves = Game.ScoreMove;
      HS.shots = Game.ScoreShot;
      if (stricmp(temps,HS.name) != 0)
      {
        strcpy(HS.name,temps);
        WritePrivateProfileString("DATA",psUser,HS.name,INIFile);
      }
      EndDialog(Dialog,1);
    }
  }
  return(0);
}



void DrawLevels( LPARAM lparam)
{
  int crBack,crText;
  char temps[100];

  pDIS = (DRAWITEMSTRUCT FAR *)lparam;

  /* Draw the focus rectangle for an empty list box or an
  empty combo box to indicate that the control has the
  focus
  */
  if ((int)(pDIS->itemID) < 0)
  {
    if ((pDIS->itemAction) & (ODA_FOCUS)) DrawFocusRect (PHDC, &PRC);
    return;
  }

  /* Get the string */
  SendMessage ( pDIS->hwndItem,LB_GETTEXT,pDIS->itemID,(LPARAM)(LPSTR)temps);

  if ((pDIS->itemState) & (ODS_SELECTED))
    /* Set background colors for selected item */
    crBack = 0x00404080;
  else
    /* Set background colors for unselected item */
    crBack = 0;

  crText = DifCList[0];
  switch (temps[0])
  {
    case '1':
      crText = DifCList[1];
      break;
    case '2':
      crText = DifCList[2];
      break;
    case '3':
      crText = DifCList[3];
      break;
    case '4':
      crText = DifCList[4];
      break;
    case '5':
      crText = DifCList[5];
      break;
  }

  // Fill item rectangle with background color
  hbrBack = CreateSolidBrush (crBack);
  FillRect (PHDC, &PRC, hbrBack);
  DeleteObject (hbrBack);

  // Set current font, background and text colors
  SetBkColor (PHDC, crBack);
  SetTextColor (PHDC, crText);

  // TextOut uses current background and text colors
  TextOut ( PHDC,PRC.left,PRC.top,temps+1,lstrlen(temps)-1);

  /* If enabled item has the input focus, call
  DrawFocusRect to set or clear the focus
  rectangle */
  if ((pDIS->itemState) & (ODS_FOCUS)) DrawFocusRect (PHDC, &PRC);
}

// For :
// ---- HS List Dialog Procedure ----
// ---- Global HS List Dialog Procedure ----
// So, the ID of the Dialog Item ID_LISTBOX must be the same
// for the two dialogs !!
//
void GetGameLevel (HWND Dialog)
{
  int i,j;
  char temps[100];

  i = SendDlgItemMessage(Dialog,ID_LISTBOX,LB_GETCURSEL,0,0);
  SendDlgItemMessage(Dialog,ID_LISTBOX,LB_GETTEXT,i,(LPARAM)temps);
  temps[5] = (char)0;
  i = 1;
  while (temps[i] == ' ') i++; // strip leading spaces
    j = atoi(&temps[i])-1;
  if (CurLevel - 1 != j)
  {
    CurLevel = j;
    LoadNextLevel(TRUE,FALSE);
  }
    EndDialog(Dialog,1);
}

// ---- HS List Dialog Procedure ----
//  List out all levels in the current data file, by number ,title, moves, shots & initials
LRESULT CALLBACK HSList(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{

  TLEVEL TempRecData;
  THSREC TempHSData;
  char temps[80], t2[100];
  int i,j;

  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_HIGHLIST_00);
      LoadWindowText(Dialog,    ID_HIGHLIST_01);
      LoadWindowText(Dialog,    ID_HIGHLIST_02);
      LoadWindowText(Dialog,    ID_HIGHLIST_03);

      F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (F1 == INVALID_HANDLE_VALUE) return(TRUE); // File NOT Found
      F2 = CreateFile(HFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (F2 == INVALID_HANDLE_VALUE)
      {
        CloseHandle(F1);
        return(TRUE);
      }
      SetWindowText(GetDlgItem(Dialog,ID_HIGHLIST_02),HFileName);
      i = SetFilePointer(F2,0,NULL,FILE_END) / sizeof(THSREC);
      SendDlgItemMessage(Dialog,ID_LISTBOX,LB_INITSTORAGE,i,54);
      SetFilePointer(F2,0,NULL,FILE_BEGIN);
      i = 1;
      ReadFile(F2, &TempHSData, sizeof(THSREC), &BytesMoved, NULL);
      while (BytesMoved == sizeof(THSREC))
      {
        ReadFile(F1, &TempRecData, sizeof(TLEVEL), &BytesMoved, NULL);
        sprintf(temps,"%4d %-30.30s",i,TempRecData.LName);
        switch (TempRecData.SDiff)
        {
          case 1: t2[0] = '1'; break;
          case 2: t2[0] = '2'; break;
          case 4: t2[0] = '3'; break;
          case 8: t2[0] = '4'; break;
          case 16: t2[0] ='5'; break;
          default: t2[0] ='0';
        }
        strcpy(t2+1,temps);
        if (TempHSData.moves > 0)
        {
          sprintf(temps,"%5d  %5d  %s",TempHSData.moves,TempHSData.shots,TempHSData.name);
          strcat(t2,temps);
        }
        SendDlgItemMessage(Dialog,ID_LISTBOX,LB_ADDSTRING,0,(LPARAM)t2);
        i++;
        ReadFile(F2, &TempHSData, sizeof(THSREC), &BytesMoved, NULL);
      }
      LastOfList = i -2;
      CloseHandle(F1); CloseHandle(F2);
      SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,CurLevel-1,0);
      OldProcEdit = SetWindowLong(GetDlgItem(Dialog,ID_LISTBOX), GWL_WNDPROC, (LONG) (WNDPROC) EditProc);
      return(TRUE);
    case WM_CTLCOLORLISTBOX:
      return ((int)GetStockObject(BLACK_BRUSH));
      break;
    case WM_MEASUREITEM:
      ((MEASUREITEMSTRUCT FAR *)lparam)->itemHeight = 13;
      break;
    case WM_DRAWITEM:
      DrawLevels(lparam);
      return TRUE;
    case WM_VKEYTOITEM:
      return (TransListKey(Dialog,LOWORD(wparam)));
    case WM_COMMAND:
      if ((wparam == ID_HIGHLIST_01)||(wparam == 2) ) EndDialog(Dialog,0);
      if (((LOWORD(wparam) == ID_LISTBOX) && (HIWORD(wparam) == LBN_SELCHANGE))
          || (wparam == 1)) GetGameLevel(Dialog);
  }
  return(0);
}

void GHSScan ( HWND Dialog )
{
  TLEVEL TempRecData;
  THSREC TempHSData, TempGHSData;
  char temps[80], t2[80];
  int i,j,BHS;

  LastOfList = 0;
  F1 = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (F1 == INVALID_HANDLE_VALUE) return; // File NOT Found
  F2 = CreateFile(HFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  F3 = CreateFile(GHFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
  if (F3 == INVALID_HANDLE_VALUE)
  {
    CloseHandle(F1);
    if (F2 != INVALID_HANDLE_VALUE) CloseHandle(F2);
    return;
  }
  i = SetFilePointer(F3,0,NULL,FILE_END) / sizeof(THSREC);
  SendDlgItemMessage(Dialog, ID_LISTBOX,LB_INITSTORAGE,i,74);
  SetFilePointer(F3,0,NULL,FILE_BEGIN);

  i = 1;
  ReadFile(F3, &TempGHSData, sizeof(THSREC), &BytesMoved, NULL);
  while (BytesMoved == sizeof(THSREC))
  {
    ReadFile(F1, &TempRecData, sizeof(TLEVEL), &BytesMoved, NULL);
    if (F2 != INVALID_HANDLE_VALUE) ReadFile(F2, &TempHSData, sizeof(THSREC), &BytesMoved, NULL);
    BHS = (
            (TempHSData.moves > 0) && (BytesMoved == sizeof(THSREC)) &&
            (
              (TempGHSData.moves == 0) || (TempHSData.moves < TempGHSData.moves) ||
              ((TempHSData.moves == TempGHSData.moves) && (TempHSData.shots < TempGHSData.shots))
            )
          );
    if (DispAll || (!DispAll && BHS))
    {
      if (BHS)
        sprintf(temps,"%4d** %-28.28s %5d %5d  %4s>",i,TempRecData.LName,TempGHSData.moves,
        TempGHSData.shots,TempGHSData.name);
      else
        sprintf(temps,"%4d %-30.30s %5d %5d  %4s ",i,TempRecData.LName,TempGHSData.moves,
        TempGHSData.shots,TempGHSData.name);
      switch (TempRecData.SDiff)
      {
        case 1: t2[0] = '1'; break;
        case 2: t2[0] = '2'; break;
        case 4: t2[0] = '3'; break;
        case 8: t2[0] = '4'; break;
        case 16: t2[0] ='5'; break;
        default: t2[0] ='0';
      }
      strcpy(t2+1,temps);
      if ((F2 != INVALID_HANDLE_VALUE) && (TempHSData.moves > 0)
      && (BytesMoved == sizeof(THSREC)))
      {
        sprintf(temps,"%5d  %5d  %s",TempHSData.moves,TempHSData.shots,TempHSData.name);
        strcat(t2,temps);
      }
      SendDlgItemMessage(Dialog, ID_LISTBOX,LB_ADDSTRING,0,(LPARAM)t2);
    }
    i++;
    ReadFile(F3, &TempGHSData, sizeof(THSREC), &BytesMoved, NULL);
  }
  LastOfList = i -2;
  CloseHandle(F1); CloseHandle(F2); CloseHandle(F3);
}




// ---- Global HS List Dialog Procedure ----
//  List out all levels in the current data file, by number ,title, moves, shots & initials
LRESULT CALLBACK GHSList(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_GHIGHLIST_00);
      LoadWindowText(Dialog,    ID_GHIGHLIST_01);
      LoadWindowText(Dialog,    ID_GHIGHLIST_02);
      LoadWindowText(Dialog,    ID_GHIGHLIST_03);
      LoadWindowText(Dialog,    ID_GHIGHLIST_04);
      LoadWindowText(Dialog,    ID_GHIGHLIST_05);
      LoadWindowText(Dialog,    ID_GHIGHLIST_06);

      SetWindowText(GetDlgItem(Dialog,ID_GHIGHLIST_05),GHFileName);
      DispAll = TRUE;
      GHSScan(Dialog);
      SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,CurLevel-1,0);
      OldProcEdit = SetWindowLong(GetDlgItem(Dialog,ID_LISTBOX), GWL_WNDPROC, (LONG) (WNDPROC) EditProc);
      return(TRUE);
    case WM_CTLCOLORLISTBOX:
      return ((int)GetStockObject(BLACK_BRUSH));
    case WM_MEASUREITEM:
      ((MEASUREITEMSTRUCT FAR *)lparam)->itemHeight = 14;
      break;
    case WM_DRAWITEM:
      DrawLevels(lparam);
      return TRUE;
    case WM_VKEYTOITEM:
      return (TransListKey(Dialog,LOWORD(wparam)));
    case WM_COMMAND:
      if ((wparam == 2) || (wparam == ID_GHIGHLIST_01)) EndDialog(Dialog,0);
      if (
          ((LOWORD(wparam) == ID_LISTBOX) || (wparam == 1))
          && (HIWORD(wparam) == LBN_SELCHANGE)
         )
      if (LastOfList > 0) GetGameLevel(Dialog);
      if (wparam == ID_GHIGHLIST_06)
      {
        SendDlgItemMessage(Dialog,ID_LISTBOX,LB_RESETCONTENT,0,0);
        DispAll = !DispAll;
        GHSScan(Dialog);
        if (DispAll)
        {
          SetWindowText(GetDlgItem(Dialog,ID_GHIGHLIST_06),txt040);
          SendDlgItemMessage(Dialog,ID_LISTBOX,LB_SETCURSEL,CurLevel-1,0);
        }
        else SetWindowText(GetDlgItem(Dialog,ID_GHIGHLIST_06),txt041);
      }
  }
  return(0);
}





// ---- Recorder Box Dialog Procedure ----
//  The Recorder uses this to prompt for the Author's name.
LRESULT CALLBACK RecordBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_RECBOX_00);
      LoadWindowText(Dialog,    ID_RECBOX_01);
      LoadWindowText(Dialog,    ID_RECBOX_02);

      GetPrivateProfileString("DATA",psPBA,"",PBSRec.Author,30,INIFile);
      SetWindowText(GetDlgItem(Dialog,ID_RECBOX_03),PBSRec.Author);
      SetFocus(GetDlgItem(Dialog,ID_RECBOX_03));
      break;
    case WM_COMMAND:
      if ((wparam ==1) || (wparam ==ID_RECBOX_02))
      {
        GetWindowText(GetDlgItem(Dialog,ID_RECBOX_03),PBSRec.Author,31);
        WritePrivateProfileString("DATA",psPBA,PBSRec.Author,INIFile);
        EndDialog(Dialog,1);
      }
 }
 return(0);
}




// ---- PlayBack Dialog Procedure ----
//  This is the Playback Control window.
LRESULT CALLBACK PBWindow(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  char temps[200];
  RECT Box;
  char char1;

  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_PLAYBOX_00);
      LoadWindowText(Dialog,    ID_PLAYBOX_01);
      LoadWindowText(Dialog,    ID_PLAYBOX_02);
      LoadWindowText(Dialog,    ID_PLAYBOX_03);
      LoadWindowText(Dialog,    ID_PLAYBOX_04);
      LoadWindowText(Dialog,    ID_PLAYBOX_05);
      LoadWindowText(Dialog,    ID_PLAYBOX_06);
      LoadWindowText(Dialog,    ID_PLAYBOX_07);
      LoadWindowText(Dialog,    ID_PLAYBOX_08);
      LoadWindowText(Dialog,    ID_PLAYBOX_11);

      if (strcmp(CurRecData.LName,PBRec.LName) == 0)
      {
        SetWindowText(GetDlgItem(Dialog,ID_PLAYBOX_10),itoa(PBRec.Size,temps,10));
        PBCountH = GetDlgItem(Dialog,ID_PLAYBOX_09);
        PlayH = Dialog;
        strcpy (temps,txt013);
        strcat(temps,PBRec.LName);
        strcat(temps,txt014);
        strcat(temps,PBRec.Author);

        switch(Speed)
        {
        case 1: // Fast
          SendMessage(GetDlgItem(Dialog,ID_PLAYBOX_04),BM_SETCHECK,1,0);
          break;
        case 2: // Slow
          SendMessage(GetDlgItem(Dialog,ID_PLAYBOX_05),BM_SETCHECK,1,0);
          break;
        case 3: // Single Step
          SendMessage(GetDlgItem(Dialog,ID_PLAYBOX_06),BM_SETCHECK,1,0);
          break;
        }
        GetWindowRect(MainH,&Box);
        SetWindowPos(Dialog,0,Box.left+ContXPos+2,Box.top+280,0,0,SWP_NOSIZE | SWP_NOZORDER);
        if (MessageBox(MainH,temps,App_Title,MB_OKCANCEL | MB_ICONQUESTION) == 2)
        {
          EndDialog(Dialog,1);
          return(0);
        }
      }
      else EndDialog(Dialog,1);
      return(0);
    case WM_COMMAND:
      switch(wparam)
      {
        case ID_PLAYBOX_01://Close
        case ID_PLAYBOX_11://Speed
        case 1:   // Close Playback Dialog
          if (Game_On && (Game.RecP == PBRec.Size) &&
             (MessageBox(Dialog,txt016,App_Title,MB_YESNO | MB_ICONQUESTION) == IDYES))
          {
            CheckMenuItem(MMenu,123,MF_CHECKED);
            EnableMenuItem(MMenu,117,0 ); // enable Save Recording
            Recording = TRUE;
            SetWindowText(MainH,REC_Title);
          }
          PlayBack = FALSE;
          PBOpen = FALSE;
          PBHold = FALSE;
          RB_TOS = Game.RecP;     // incase we stop short
          EndDialog(Dialog,1);
          break;
        case ID_PLAYBOX_04: Speed = 1; // Fast
          break;
        case ID_PLAYBOX_05: Speed = 2; // Slow
          break;
        case ID_PLAYBOX_06: Speed = 3; // Single Step
          break;
        case ID_PLAYBOX_02:
          if (PlayBack)
          {
            SetWindowText(GetDlgItem(Dialog,ID_PLAYBOX_02),txt017);
            PlayBack = FALSE;
          }
          else {
            SetWindowText(GetDlgItem(Dialog,ID_PLAYBOX_02),txt018);
            PlayBack = TRUE;
          }
          break;
        case ID_PLAYBOX_03://Reset
          PlayBack = FALSE;
          SetWindowText(GetDlgItem(Dialog,ID_PLAYBOX_02),txt017);
          SetWindowText(GetDlgItem(Dialog,ID_PLAYBOX_09),"0");
          SendMessage(MainH,WM_COMMAND, 105,0); //ReStart
          Game.RecP = 0;
          RB_TOS = PBRec.Size;
          PBHold = FALSE;
      }
  }
  return(0);
}

static int LastLevelNum;

// ---- Pick Box Dialog Procedure ----
// The editor uses this to pick which level number to save the
// level with. Returns the number.}
//function PickBox(Dialog: HWnd; Message, WParam: Word; LParam: Longint): Bool; export;
LRESULT CALLBACK PickBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{
  char temps[40];
  int i,j;
  TLEVEL TempRecData;


  switch (Message)
  {
    case WM_INITDIALOG:
      LoadWindowCaption(Dialog, ID_PICKLEVEL_00);
      LoadWindowText(Dialog,    ID_PICKLEVEL_01);
      LoadWindowText(Dialog,    ID_PICKLEVEL_02);
      LoadWindowText(Dialog,    ID_PICKLEVEL_03);
      LoadWindowText(Dialog,    ID_PICKLEVEL_06);
      LoadWindowText(Dialog,    ID_PICKLEVEL_08);

      i = SetFilePointer(F1,0,NULL,FILE_END) / sizeof(TLEVEL);
      sprintf(temps,"1 - %d",i);
      SetWindowText(GetDlgItem(Dialog,ID_PICKLEVEL_04),temps);
      i++;
      LastLevelNum = i;   // save Last Level Number to check in exit
      itoa(i,temps,10);
      SetWindowText(GetDlgItem(Dialog,ID_PICKLEVEL_07),temps);
      if (CurLevel < i) itoa(CurLevel,temps,10);
      SetWindowText(GetDlgItem(Dialog,ID_PICKLEVEL_05),temps);
      SendDlgItemMessage(Dialog,ID_PICKLEVEL_08,BM_SETCHECK,1,0);
      return(TRUE);
    case WM_COMMAND:
      if ((wparam == 1) || (wparam == ID_PICKLEVEL_01) )
      {
        GetWindowText(GetDlgItem(Dialog,ID_PICKLEVEL_05),temps,10);
        i = atoi(temps);
        HSClear = SendDlgItemMessage(Dialog,ID_PICKLEVEL_08,BM_GETCHECK,0,0) > 0;
        if (i <= LastLevelNum) EndDialog(Dialog,i);
        return(TRUE);
      }
  }
  return(0);
}

// ---- Used by Graph Box Dialog Procedure ----
void SetUpGraphicsBox( HWND d, int C101, int C102, int BU)
{
  long DBU;
  RECT Box;

  SendDlgItemMessage(d,ID_GRAPHBOX_02,BM_SETCHECK,C101,0);
  //SendDlgItemMessage(d,ID_GRAPHBOX_03,BM_SETCHECK,C102,0);
  DBU = GetDialogBaseUnits();
  GetWindowRect(d,&Box);
  SetWindowPos(d,HWND_TOP,Box.left,Box.top,Box.right - Box.left,
          (BU * HIWORD(DBU))/8,SWP_NOZORDER);

    if (GFXOn) GFXKill();
  GFXInit();
  InvalidateRect(MainH,NULL,TRUE);
}


// ---- Used by Graph Box Dialog Procedure ----
void GetLTGFiles(HWND Dialog,TLTGREC *ltgCur)
{
  WIN32_FIND_DATA ffdata;
  HANDLE ffdH,F1;
  TLTGREC ltg;

  // Scan for all LTG files
  SetCurrentDirectory(GraphDN);
  ffdH = FindFirstFile("*.ltg",&ffdata);
  if (ffdH)
  {
    do {
      F1 = CreateFile(ffdata.cFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (F1 != INVALID_HANDLE_VALUE)
      {
        if (!(ReadFile(F1, &ltg, sizeof(TLTGREC), &BytesMoved, NULL))) FileError();
        SendDlgItemMessage(Dialog, ID_GRAPHBOX_10,LB_ADDSTRING,0,(LPARAM)ltg.Name);
        // the next line will help setup the listbox if we are mode 2
        if ((GraphM == 2) && (strcmp(GraphFN,ffdata.cFileName) == 0))
          memcpy(ltgCur,&ltg,sizeof(TLTGREC));
      }
      CloseHandle(F1);
    } while (FindNextFile(ffdH,&ffdata));
  FindClose(ffdH);
  }
}

// ---- Graph Box Dialog Procedure ----
//  This Dialog is used to change the Graphics Set being used
LRESULT CALLBACK GraphBox(HWND Dialog, UINT Message, WPARAM wparam, LPARAM lparam)
{

  char temps[MAX_PATH];
  TLTGREC ltg,ltgCur;
  char Mode2N[40];
  int i;
  WIN32_FIND_DATA ffdata;
  HANDLE ffdH,F1;

  switch (Message)
  {
    case WM_INITDIALOG:

     LoadWindowCaption(Dialog, ID_GRAPHBOX_00);
     LoadWindowText(Dialog,    ID_GRAPHBOX_01);
     LoadWindowText(Dialog,    ID_GRAPHBOX_02);
     //LoadWindowText(Dialog,    ID_GRAPHBOX_03);
     LoadWindowText(Dialog,    ID_GRAPHBOX_04);
     LoadWindowText(Dialog,    ID_GRAPHBOX_05);
     LoadWindowText(Dialog,    ID_GRAPHBOX_06);
     LoadWindowText(Dialog,    ID_GRAPHBOX_08);
     LoadWindowText(Dialog,    ID_GRAPHBOX_09);

    GetLTGFiles(Dialog,&ltgCur);
    switch (GraphM)
    {
      case 1:
        SetUpGraphicsBox( Dialog, BST_UNCHECKED,BST_CHECKED,100);
      break;
      case 2:
        SetUpGraphicsBox( Dialog, BST_UNCHECKED,BST_UNCHECKED,160);
        i = SendDlgItemMessage(Dialog, ID_GRAPHBOX_10,LB_SELECTSTRING,-1,(LPARAM)ltgCur.Name);
        SetWindowText(GetDlgItem(Dialog,ID_GRAPHBOX_07),ltgCur.Author);
        SetWindowText(GetDlgItem(Dialog,ID_GRAPHBOX_11),ltgCur.Info);
      break;
      default:
        SetUpGraphicsBox( Dialog, BST_CHECKED,BST_UNCHECKED,100); // mode 0
    }
        return(TRUE);
    case WM_COMMAND:
    switch (LOWORD(wparam))
    {
      case ID_GRAPHBOX_04: // Close
      case 2: // Cancel
        itoa(GraphM,temps,10);
        WritePrivateProfileString("SCREEN",psGM,temps,INIFile);
        if (GraphM == 2) WritePrivateProfileString("SCREEN",psGFN,GraphFN,INIFile);
        QHELP = FALSE;
        InvalidateRect(MainH,NULL,FALSE);
        EndDialog(Dialog,1);
        break;
      case ID_GRAPHBOX_02:      // Internal Graphics
        if (GraphM == 2) SendDlgItemMessage(Dialog,ID_GRAPHBOX_10,LB_SETCURSEL,-1,0);
        GraphM = 0;
        SetUpGraphicsBox( Dialog, BST_CHECKED,BST_UNCHECKED,100);

        break;
      case ID_GRAPHBOX_03:      // External Graphics
        if (GraphM == 2) SendDlgItemMessage(Dialog,ID_GRAPHBOX_10,LB_SETCURSEL,-1,0);
        GraphM = 1;
        SetUpGraphicsBox( Dialog, BST_UNCHECKED,BST_CHECKED,100);
        if (GraphM == 0)
        {
          MessageBox(Dialog,txt032,txt007,MB_OK | MB_ICONERROR);
          SendDlgItemMessage(Dialog,ID_GRAPHBOX_02,BM_SETCHECK,BST_CHECKED,0);
          SendDlgItemMessage(Dialog,ID_GRAPHBOX_03,BM_SETCHECK,BST_UNCHECKED,0);
        }
      break;
      case ID_GRAPHBOX_10:      // Graphics Sets
        i = SendDlgItemMessage(Dialog,ID_GRAPHBOX_10,LB_GETCURSEL,0,0);
        if (i == -1) break; // Weed out junk ( Don't Ask )
        SendDlgItemMessage(Dialog,ID_GRAPHBOX_10,LB_GETTEXT,i,(LPARAM)Mode2N);
        // Find File Reference
        ffdH = FindFirstFile("*.ltg",&ffdata);
        if (ffdH)
        {
          do {
            F1 = CreateFile(ffdata.cFileName, GENERIC_READ, FILE_SHARE_READ,
              NULL, OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN, NULL);
            if (F1 != INVALID_HANDLE_VALUE)
            {
              if (!(ReadFile(F1, &ltg, sizeof(TLTGREC), &BytesMoved, NULL))) FileError();
              if (strcmp(Mode2N,ltg.Name) == 0)
              {
                CloseHandle(F1);
                FindClose(ffdH);
                SetWindowText(GetDlgItem(Dialog,ID_GRAPHBOX_07),ltg.Author);
                SetWindowText(GetDlgItem(Dialog,ID_GRAPHBOX_11),ltg.Info);
                strcpy(GraphFN,ffdata.cFileName);
                GraphM = 2;
                SetUpGraphicsBox( Dialog, BST_UNCHECKED,BST_UNCHECKED,160);
                return(0);
              }
            }
            CloseHandle(F1);
          } while (FindNextFile(ffdH,&ffdata));
          FindClose(ffdH);
        }
        //We should Never be here
        MessageBox(MainH, txt045,txt007,MB_OK);
        break;
      case ID_GRAPHBOX_08:
        QHELP = (SendDlgItemMessage(Dialog,ID_GRAPHBOX_08,BM_GETCHECK,0,0) > 0);
        InvalidateRect(MainH,NULL,FALSE);
      break;
      case ID_GRAPHBOX_09:
        Browse(Dialog,GraphDN,txt037);
        SendDlgItemMessage(Dialog,ID_GRAPHBOX_10,LB_RESETCONTENT,0,0);
        GetLTGFiles(Dialog,&ltgCur);
        WritePrivateProfileString("SCREEN",psGDN,GraphDN,INIFile);
      break;
         }
  }
  return(0);
}


