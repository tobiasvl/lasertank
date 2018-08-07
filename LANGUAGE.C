/*******************************************************
 **             LaserTank ver 4.0                     **
 **               By Jim Kindley                      **
 **               (c) 2001                            **
 **         The Program and Source is Public Domain   **
 *******************************************************
 **       Release version 2002 by Yves Maingoy        **
 **               ymaingoy@free.fr                    **
 *******************************************************/

// Language.c

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <string.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <stdio.h>
#include "ltank.h"

char LANGText[SIZE_ALL][MAX_LANG_SIZE]; // All lines of Language.dat
char LANGFile[MAX_PATH];                // the dynamic file name for Language.dat
TCHAR szFilterOFN[MAX_PATH];
TCHAR szFilterPBfn[MAX_PATH];


// ************************************************************************
// These are the originals texts for all messages.
// ************************************************************************
char *App_Strings[SIZE_BUTTON+SIZE_TEXT] = {
    // This is the Button Text
    "Undo",
    "Save Position",
    "Restore Position",
    "New",
    "<< Level",
    "Level >>",
    "Hint",
    "Restart",
    "Load Level",

    // This is others texts
    "The Level file can not be found\nPlease select a new one",
    "Level Files (*.LVL)",
    "Pick LaserTank Level File",
    "Playback Files (*.LPB)",
    "Save Playback File",
    "LaserTank Error",
    "Old High Score > M:",
    "M: ",
    " S: ",
    " I: ",
    "Congratulation's You beat it !!",
    "Playback Level : ",
    "\nRecorded by ",
    "\nCan Not be found in the current level file\n< ",
    "Do You want to Continue Recording ?",
    "&Play",
    "&Pause",
    "Out of Memory - Undo Roll Over",
    "Open Playback file",
    "LaserTank Hint",
    "Level NOT Saved, Save Data ?",
    " - Kids",
    " - Easy",
    " - Medium",
    " - Hard",
    " - Deadly",
    " - [Editor]",
    "LaserTank Editor",
    "NOT a Valid LaserTank Graphics Set",
    "game.bmp and/or mask.bmp Not Found",
    "Open Playback file for Resume",
    "Return to Game",
    "< NO HINT SUBMITTED FOR THIS LEVEL >",
    "LaserTank Level #",
    "Select LaserTank Graphics Directory",
    "Game in Progress ...",
    "You will lose game data.\nDo you want to save the game ?",
    "Beat",
    "All",
    "Completed %d of %d levels",
    "Difficulty Set =",
    "KEMHD",
    "LaserTank Graphics file Not Found !",
    "LaserTank *** RECORDING ***",
    "Editor Instructions",
    "Revisions",
    "Writing Your Own Levels",
    "LaserTank.hlp"
};

// ************************************************************************
// If Language File, set windowstext with this text
// ************************************************************************
//SetWindowText(GetDlgItem(Dialog,ID_ABOUTBOX_02),App_Title);

void LoadWindowText(HWND hDlg, int nIDDlgItem )
{
    if ( LANGText[nIDDlgItem][0] != '\0' )
        SetWindowText(GetDlgItem(hDlg, nIDDlgItem), LANGText[nIDDlgItem]);
}
void LoadWindowCaption(HWND hDlg, int nIDDlgItem )
{
    if ( LANGText[nIDDlgItem][0] != '\0' )
        SetWindowText(hDlg, LANGText[nIDDlgItem]);
}


// ************************************************************************
// Read the language file is exist
// fill the LANGText[],
// update the two menus,
// ************************************************************************
void InitLanguage(HMENU MMenu, HMENU EMenu)
{
    int i;
    FILE *fd;
    char szTmp[MAX_LANG_SIZE];

    // Initialyse with default values
    for( i = 0; i < SIZE_BUTTON+SIZE_TEXT; i++)
        strcpy( LANGText[i], App_Strings[i]);

    // Clear default messages dialogs
    for( i = OFFSET_DIALOGS; i < OFFSET_ABOUTMSG + SIZE_ABOUTMSG; i++)
        strcpy( LANGText[i], "\0");

    fd = fopen(LANGFile,"r");

    if ( fd != NULL )
    {
      i = 0;
      while(!feof(fd))
      {
        fgets( szTmp, MAX_LANG_SIZE, fd );
        if( szTmp[0] != '\0' )
        {
          szTmp[strlen(szTmp)-1]= '\0'; // remove the last char
          if ( (szTmp[0] != '\0' ) && (szTmp[0] != '#') )    // this is not a comment line
          {
            ConvertTabChar( szTmp );

            if      ( (i>=START_MMENU)
                        && (i <START_MMENU+SIZE_MMENU)  )
                            ChangeMenuText( MMenu, szTmp );

            else if ( (i>=START_EMENU)
                        && (i < START_EMENU+SIZE_EMENU)  )
                            ChangeMenuText( EMenu, szTmp );

            else if ( (i>=START_BUTTON)
                        && (i < (START_BUTTON+SIZE_BUTTON+SIZE_TEXT+SIZE_ABOUTMSG+SIZE_DIALOGS)) )
                            strcpy(LANGText[i-START_BUTTON], szTmp);
            i++;
          }
          }
      }
      fclose(fd);
    }
}

// Used by ChangeMenuText()
int DecodeMenuInt(char *MenuStr)
{
    if ( MenuStr[0] == '0' )
        return( (int)(MenuStr[1] - '0') );

    if ( MenuStr[0] == '1' )
        return( (int)(10 + (MenuStr[1] - '0')) );
    else
        return( -1 );
}


// ************************************************************************
// To change a menu text with this syntax :
// 00,01,02,&Text
//
// ************************************************************************
void ChangeMenuText(HMENU hMenu, char *MenuStr ) {

    MENUITEMINFO          ItemInfo;
    int nMenu;
    int nMenu1;
    int nMenu2;

    if ( (strlen(MenuStr) > 9) && (hMenu != NULL) )
    {
        nMenu  = DecodeMenuInt( (char*) (MenuStr + 0) );
        nMenu1 = DecodeMenuInt( (char*) (MenuStr + 3) );
        nMenu2 = DecodeMenuInt( (char*) (MenuStr + 6) );
        if ( nMenu2 >= 0 )
        {
            hMenu = GetSubMenu(hMenu, nMenu );
            hMenu = GetSubMenu(hMenu, nMenu1);
            nMenu = nMenu2;
        }
        else
        {
            if ( nMenu1 >= 0 )
            {
                hMenu = GetSubMenu(hMenu, nMenu);
                nMenu = nMenu1;
            }
        }

        memset(&ItemInfo, 0, sizeof(MENUITEMINFO));
        ItemInfo.cbSize = sizeof(MENUITEMINFO);
        ItemInfo.fMask = MIIM_TYPE;
        GetMenuItemInfo(hMenu, nMenu, TRUE, &ItemInfo);
        if ( ItemInfo.fType == MFT_STRING )
        {
            // Replace menu item.
            ItemInfo.dwTypeData = (char*)(MenuStr+9);
            ItemInfo.cch = lstrlen(ItemInfo.dwTypeData);
            SetMenuItemInfo(hMenu, nMenu, TRUE, &ItemInfo);
        }
    }
}

void ConvertTabChar(char *szBuffer)
{
  char *P;
  char *Q;

  P = strchr(szBuffer,'\\');
  while (P)
  {
      // case of \t
    if ( *(P+1) == 't' )
    {
      *P++='\t';
      for ( Q = P; *Q != 0; Q++)
          *Q = *(char*)(Q+1);
    }
    else
    {
      // case of \n
      if ( *(P+1) == 'n' )
      {
        *P++=0x0d;
        *P  =0x0a;
      }
	  else P++;
    }
      // Search the next occurence
      P = strchr(P,'\\');
  }

}

// ************************************************************************
// Load Resource Bitmap or File
// ************************************************************************
HBITMAP LoadImageFile(HINSTANCE hInst, char *szBitmapRes,char *szBitmapFile )
{

  HBITMAP             hbm;
  char                *P;
  char lpszName[MAX_PATH];

  GetModuleFileName (NULL,lpszName,MAX_PATH);
  P = strrchr(lpszName,'\\');
  if (P)
  {
    P++;
    strcpy(P, TEXT(LANGFilePath));      // add path
    P += strlen(TEXT(LANGFilePath));
    strcpy(P, szBitmapFile);      // add name
  }
  hbm = (HBITMAP)LoadImage(NULL, lpszName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
  if (hbm == NULL)
  {
      hbm = LoadBitmap(hInst, szBitmapRes);
  }
  return( hbm );
}
