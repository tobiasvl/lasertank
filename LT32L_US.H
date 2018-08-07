/*******************************************************
 **             LaserTank ver 4.1.1                   **
 **               By Jim Kindley                      **
 **               (c) 2001                            **
 **         The Program and Source is Public Domain   **
 *******************************************************
 **       Release version 2005 by Yves Maingoy        **
 **               ymaingoy@free.fr                    **
 *******************************************************/
#define App_Title		"LaserTank"
#define App_Version		"4.1.2"
#define FILE_VERSION    "4.1.2\0"
#define PRODUCT_VERSION "4.1.2\0"

// The name of Language File
#define LANGFilePath     "Language\\"
#define LANGFileName     "Language\\Language.dat"
#define GAME_BMP         "game.bmp"
#define MASK_BMP         "mask.bmp"
#define OPENING_BMP      "opening.bmp"
#define CONTROL_BMP      "control.bmp"


//
// ID of Dialogs
// Text values and old ID are in comments
//

// This is the Button Function ID number
#define ButID1 	  		110
#define ButID2 		  	111
#define ButID3 		  	112
#define ButID4 		  	101
#define ButID5 		  	119
#define ButID6 		  	107
#define ButID7 		  	301
#define ButID8 		  	105
#define ButID9 		  	106


// number of linesof each sections in the Language File
#define SIZE_BUTTON    9
#define SIZE_TEXT      48
#define SIZE_MMENU     49
#define SIZE_EMENU     24
#define SIZE_DIALOGS   (4+3+9+9+7+3+14+1+4+3+9+4+7+3+10+6)
#define SIZE_ABOUTMSG  14
#define SIZE_ALL       (SIZE_BUTTON+SIZE_TEXT+SIZE_MMENU+SIZE_EMENU+SIZE_ABOUTMSG+SIZE_DIALOGS)


// Start LINES in the Language File
#define START_MMENU     0
#define START_EMENU     (START_MMENU  + SIZE_MMENU )
#define START_BUTTON    (START_EMENU  + SIZE_EMENU )
#define START_TEXT      (START_BUTTON + SIZE_BUTTON)
#define START_DIALOGS   (START_TEXT   + SIZE_TEXT)
#define START_ABOUTMSG  (START_ABOUTMSG + SIZE_ABOUTMSG)

// Start LINES in the LANGText[]
#define OFFSET_TEXT      SIZE_BUTTON
#define OFFSET_DIALOGS   (OFFSET_TEXT + SIZE_TEXT)
#define OFFSET_ABOUTMSG  (OFFSET_DIALOGS + SIZE_DIALOGS)



#define ButText1 	LANGText[0]
#define ButText2 	LANGText[1]
#define ButText3 	LANGText[2]
#define ButText4 	LANGText[3]
#define ButText5 	LANGText[4]
#define ButText6 	LANGText[5]
#define ButText7 	LANGText[6]
#define ButText8 	LANGText[7]
#define ButText9 	LANGText[8]

#define txt001    LANGText[OFFSET_TEXT +  0]
#define txt002    LANGText[OFFSET_TEXT +  1]
#define txt003    empty
#define txt004    LANGText[OFFSET_TEXT +  2]
#define txt005    LANGText[OFFSET_TEXT +  3]
#define txt006    LANGText[OFFSET_TEXT +  4]
#define txt007    LANGText[OFFSET_TEXT +  5]
#define txt008    LANGText[OFFSET_TEXT +  6]
#define txt009    LANGText[OFFSET_TEXT +  7]
#define txt010    LANGText[OFFSET_TEXT +  8]
#define txt011    LANGText[OFFSET_TEXT +  9]
#define txt012    LANGText[OFFSET_TEXT + 10]
#define txt013    LANGText[OFFSET_TEXT + 11]
#define txt014    LANGText[OFFSET_TEXT + 12]
#define txt015    LANGText[OFFSET_TEXT + 13]
#define txt016    LANGText[OFFSET_TEXT + 14]
#define txt017    LANGText[OFFSET_TEXT + 15]
#define txt018    LANGText[OFFSET_TEXT + 16]
#define txt019    LANGText[OFFSET_TEXT + 17]
#define txt020    LANGText[OFFSET_TEXT + 18]
#define txt021    LANGText[OFFSET_TEXT + 19]
#define txt022    LANGText[OFFSET_TEXT + 20]
#define txt023    LANGText[OFFSET_TEXT + 21]
#define txt024    LANGText[OFFSET_TEXT + 22]
#define txt025    LANGText[OFFSET_TEXT + 23]
#define txt026    LANGText[OFFSET_TEXT + 24]
#define txt027    LANGText[OFFSET_TEXT + 25]
#define txt028    LANGText[OFFSET_TEXT + 26]
#define txt029    LANGText[OFFSET_TEXT + 27]
#define txt030    empty
#define txt031    LANGText[OFFSET_TEXT + 28]
#define txt032    LANGText[OFFSET_TEXT + 29]
#define txt033    LANGText[OFFSET_TEXT + 30]
#define txt034    LANGText[OFFSET_TEXT + 31]
#define txt035    LANGText[OFFSET_TEXT + 32]
#define txt036    LANGText[OFFSET_TEXT + 33]
#define txt037    LANGText[OFFSET_TEXT + 34]
#define txt038    LANGText[OFFSET_TEXT + 35]
#define txt039    LANGText[OFFSET_TEXT + 36]
#define txt040    LANGText[OFFSET_TEXT + 37]
#define txt041    LANGText[OFFSET_TEXT + 38]
#define txt042    LANGText[OFFSET_TEXT + 39]
#define txt043    LANGText[OFFSET_TEXT + 40]
#define txt044    LANGText[OFFSET_TEXT + 41]
#define txt045    LANGText[OFFSET_TEXT + 42]
#define REC_Title LANGText[OFFSET_TEXT + 43]

#define help01    LANGText[OFFSET_TEXT + 44]
#define help02    LANGText[OFFSET_TEXT + 45]
#define help03    LANGText[OFFSET_TEXT + 46]
#define HelpFileName  LANGText[OFFSET_TEXT + 47]

// This Id is used By Load, HighScore & Global High Score ListBoxs
#define ID_LISTBOX         801

// Others IDs
#define IDM_ABOUTBOX_00    "About Laser Tank"
#define IDM_ABOUTBOX_01    "&Ok"
#define IDM_ABOUTBOX_04    "By Jim Kindley"
#define IDM_ABOUTBOX_06    "Version"
#define IDM_ABOUTBOX_02    "Title"
#define IDM_ABOUTBOX_03    "Ver Info"
#define IDM_ABOUTBOX_05    "©2002, JEK Software"
#define ID_ABOUTBOX_00     (OFFSET_DIALOGS + 0)
#define ID_ABOUTBOX_OK     (OFFSET_DIALOGS + 1)
#define ID_ABOUTBOX_04     (OFFSET_DIALOGS + 2)
#define ID_ABOUTBOX_06     (OFFSET_DIALOGS + 3)
#define ID_ABOUTBOX_02     801
#define ID_ABOUTBOX_03     802
#define ID_ABOUTBOX_05     803
#define ID_ABOUTBOX_07     804

#define IDM_DEADBOX_00     "&Undo Last Move"
#define IDM_DEADBOX_01     "YOU ARE DEAD ! ! !"
#define IDM_DEADBOX_02     "&ReStart"
#define ID_DEADBOX_UNDO    (OFFSET_DIALOGS + 4)
#define ID_DEADBOX_DEAD    (OFFSET_DIALOGS + 5)
#define ID_DEADBOX_RESTART (OFFSET_DIALOGS + 6)
/*
#define IDM_DIFFBOX_00     "Game Difficulty"
#define IDM_DIFFBOX_01     "&Ok"
#define IDM_DIFFBOX_02     "&Kids"
#define IDM_DIFFBOX_03     "&Easy"
#define IDM_DIFFBOX_04     "&Medium"
#define IDM_DIFFBOX_05     "&Hard"
#define IDM_DIFFBOX_06     "&Deadly"
#define IDM_DIFFBOX_07     "&All"
#define IDM_DIFFBOX_08     "Select"
*/
#define ID_DIFFBOX_00     (OFFSET_DIALOGS + 7)
#define ID_DIFFBOX_01     (OFFSET_DIALOGS + 8)
#define ID_DIFFBOX_02     (OFFSET_DIALOGS + 9)
#define ID_DIFFBOX_03     (OFFSET_DIALOGS + 10)
#define ID_DIFFBOX_04     (OFFSET_DIALOGS + 11)
#define ID_DIFFBOX_05     (OFFSET_DIALOGS + 12)
#define ID_DIFFBOX_06     (OFFSET_DIALOGS + 13)
#define ID_DIFFBOX_07     (OFFSET_DIALOGS + 14)
#define ID_DIFFBOX_08     (OFFSET_DIALOGS + 15)

/*
#define IDM_GRAPHBOX_00    "Select Graphics Set"
#define IDM_GRAPHBOX_01    "Select One"
#define IDM_GRAPHBOX_02    "&Internal Graphics"
#define IDM_GRAPHBOX_03    "&User Graphics "
#define IDM_GRAPHBOX_04    "&Close"
#define IDM_GRAPHBOX_05    "Author :"
#define IDM_GRAPHBOX_06    "Laser Tank Graphic Set Description"
#define IDM_GRAPHBOX_07    "&View Opening Screen "
#define IDM_GRAPHBOX_08    "Change &Directory"
*/
#define ID_GRAPHBOX_00    (OFFSET_DIALOGS + 16)
#define ID_GRAPHBOX_01    (OFFSET_DIALOGS + 17)
#define ID_GRAPHBOX_02    (OFFSET_DIALOGS + 18)
#define ID_GRAPHBOX_03    (OFFSET_DIALOGS + 19)
#define ID_GRAPHBOX_04    (OFFSET_DIALOGS + 20)
#define ID_GRAPHBOX_05    (OFFSET_DIALOGS + 21)
#define ID_GRAPHBOX_06    (OFFSET_DIALOGS + 22)
#define ID_GRAPHBOX_07    801
#define ID_GRAPHBOX_08    (OFFSET_DIALOGS + 23)
#define ID_GRAPHBOX_09    (OFFSET_DIALOGS + 24)
#define ID_GRAPHBOX_10    802
#define ID_GRAPHBOX_11    803


/*
#define IDM_LOADLEV_00     "Pick Level to Load"
#define IDM_LOADLEV_01     "&Cancel"
#define IDM_LOADLEV_03     "&Filter"
#define IDM_LOADLEV_04     "File:"
#define IDM_LOADLEV_05     "- File Loading Error -"
#define IDM_LOADLEV_06     "Lev#   Name                           Author"
#define IDM_LOADLEV_07     "or Direct Level Number Entry >>"
*/
#define ID_LOADLEV_00     (OFFSET_DIALOGS + 25)
#define ID_LOADLEV_01     (OFFSET_DIALOGS + 26)
#define ID_LOADLEV_02     802
#define ID_LOADLEV_03     (OFFSET_DIALOGS + 27)
#define ID_LOADLEV_04     (OFFSET_DIALOGS + 28)
#define ID_LOADLEV_05     (OFFSET_DIALOGS + 29)
#define ID_LOADLEV_06     (OFFSET_DIALOGS + 30)
#define ID_LOADLEV_07     (OFFSET_DIALOGS + 31)
#define ID_LOADLEV_08     ID_LISTBOX
/*
#define IDM_HINTBOX_00     "Modify Hint"
#define IDM_HINTBOX_01     "&Ok"
#define IDM_HINTBOX_02     "&Cancel"
*/
#define ID_HINTBOX_00     (OFFSET_DIALOGS + 32)
#define ID_HINTBOX_01     (OFFSET_DIALOGS + 33)
#define ID_HINTBOX_02     (OFFSET_DIALOGS + 34)
#define ID_HINTBOX_03     801
/*
#define IDM_SEARCH_00      "Filter Level Data"
#define IDM_SEARCH_01      "Enter Search String :"
#define IDM_SEARCH_02      "Search By"
#define IDM_SEARCH_03      "&Title"
#define IDM_SEARCH_04      "&Author"
#define IDM_SEARCH_05      "&Cancel"
#define IDM_SEARCH_06      "&Ok"
#define IDM_SEARCH_07      "ICON1"
#define IDM_SEARCH_08      "&Only Unsolved Levels"
#define IDM_SEARCH_09      "&Filter by Difficulty"
#define IDM_SEARCH_10      "Kids"
#define IDM_SEARCH_11      "Easy"
#define IDM_SEARCH_12      "Medium"
#define IDM_SEARCH_13      "Hard"
#define IDM_SEARCH_14      "Deadly"
*/
#define ID_SEARCH_00      (OFFSET_DIALOGS + 35)
#define ID_SEARCH_01      (OFFSET_DIALOGS + 36)
#define ID_SEARCH_02      (OFFSET_DIALOGS + 37)
#define ID_SEARCH_03      (OFFSET_DIALOGS + 38)
#define ID_SEARCH_04      (OFFSET_DIALOGS + 39)
#define ID_SEARCH_05      (OFFSET_DIALOGS + 40)
#define ID_SEARCH_06      (OFFSET_DIALOGS + 41)
#define ID_SEARCH_07      801
#define ID_SEARCH_08      (OFFSET_DIALOGS + 42)
#define ID_SEARCH_09      (OFFSET_DIALOGS + 43)
#define ID_SEARCH_10      (OFFSET_DIALOGS + 44)
#define ID_SEARCH_11      (OFFSET_DIALOGS + 45)
#define ID_SEARCH_12      (OFFSET_DIALOGS + 46)
#define ID_SEARCH_13      (OFFSET_DIALOGS + 47)
#define ID_SEARCH_14      (OFFSET_DIALOGS + 48)
/*
#define IDM_RETBOX_00      "&Return to Game"
*/
#define ID_RETBOX_00      (OFFSET_DIALOGS + 49)
/*
#define IDM_WINBOX_00      "&Ok"
#define IDM_WINBOX_01      "You Won ! ! !"
#define IDM_WINBOX_02      "File :"
#define IDM_WINBOX_03      "- Error -"
*/
#define ID_WINBOX_00      (OFFSET_DIALOGS + 50)
#define ID_WINBOX_01      (OFFSET_DIALOGS + 51)
#define ID_WINBOX_02      (OFFSET_DIALOGS + 52)
#define ID_WINBOX_03      (OFFSET_DIALOGS + 53)
#define ID_WINBOX_04      801
#define ID_WINBOX_05      802
#define ID_WINBOX_06      803
/*
#define IDM_LOADTID_00     "Pick Tunnel ID #"
#define IDM_LOADTID_03     "&Ok"
#define IDM_LOADTID_04     "Enter the ID Number for this Tunnel"
*/
#define ID_LOADTID_00     (OFFSET_DIALOGS + 54)
#define ID_LOADTID_01     801
#define ID_LOADTID_02     802
#define ID_LOADTID_03     (OFFSET_DIALOGS + 55)
#define ID_LOADTID_04     (OFFSET_DIALOGS + 56)
/*
#define IDM_HIGHBOX_00     "! New High Score !"
#define IDM_HIGHBOX_01     "&Ok"
#define IDM_HIGHBOX_02     "Level Number :"
#define IDM_HIGHBOX_03     "Level Name :"
#define IDM_HIGHBOX_04     "Moves :"
#define IDM_HIGHBOX_05     "Shots :"
#define IDM_HIGHBOX_06     "0"
#define IDM_HIGHBOX_07     "0"
#define IDM_HIGHBOX_08     "0"
#define IDM_HIGHBOX_09     " "
#define IDM_HIGHBOX_10     "Initials :"
#define IDM_HIGHBOX_12     "Global HS :"
#define IDM_HIGHBOX_13     "- No Data -"
*/
#define ID_HIGHBOX_00     (OFFSET_DIALOGS + 57)
#define ID_HIGHBOX_01     (OFFSET_DIALOGS + 58)
#define ID_HIGHBOX_02     (OFFSET_DIALOGS + 59)
#define ID_HIGHBOX_03     (OFFSET_DIALOGS + 60)
#define ID_HIGHBOX_04     (OFFSET_DIALOGS + 61)
#define ID_HIGHBOX_05     (OFFSET_DIALOGS + 62)
#define ID_HIGHBOX_06     103
#define ID_HIGHBOX_07     104
#define ID_HIGHBOX_08     ID_LISTBOX
#define ID_HIGHBOX_09     102
#define ID_HIGHBOX_10     (OFFSET_DIALOGS + 63)
#define ID_HIGHBOX_12     (OFFSET_DIALOGS + 64)
#define ID_HIGHBOX_13     (OFFSET_DIALOGS + 65)
#define ID_HIGHBOX_14     105
#define ID_HIGHBOX_15     106
/*
#define IDM_HIGHLIST_00    "High Scores"
#define IDM_HIGHLIST_01    "&Ok"
#define IDM_HIGHLIST_02    "- File Loading Error -"
#define IDM_HIGHLIST_03    "Lev#  Name                          Moves  Shots  Initials"
*/
#define ID_HIGHLIST_00    (OFFSET_DIALOGS + 66)
#define ID_HIGHLIST_01    (OFFSET_DIALOGS + 67)
#define ID_HIGHLIST_02    (OFFSET_DIALOGS + 68)
#define ID_HIGHLIST_03    (OFFSET_DIALOGS + 69)
/*
#define IDM_GHIGHLIST_00   "Global High Scores"
#define IDM_GHIGHLIST_01   "&Ok"
#define IDM_GHIGHLIST_02   "Lev#  Name                             M      S   Init   M     S  Init"
#define IDM_GHIGHLIST_03   "Global"
#define IDM_GHIGHLIST_04   "Local"
#define IDM_GHIGHLIST_05   "- No Data -"
#define IDM_GHIGHLIST_06   "&Beat"
*/
#define ID_GHIGHLIST_00   (OFFSET_DIALOGS + 70)
#define ID_GHIGHLIST_01   (OFFSET_DIALOGS + 71)
#define ID_GHIGHLIST_02   (OFFSET_DIALOGS + 72)
#define ID_GHIGHLIST_03   (OFFSET_DIALOGS + 73)
#define ID_GHIGHLIST_04   (OFFSET_DIALOGS + 74)
#define ID_GHIGHLIST_05   (OFFSET_DIALOGS + 75)
#define ID_GHIGHLIST_06   (OFFSET_DIALOGS + 76)
/*
#define IDM_RECBOX_00      "Recording Game"
#define IDM_RECBOX_01      "Please Enter Your Name"
#define IDM_RECBOX_02      "&Ok"
*/
#define ID_RECBOX_00      (OFFSET_DIALOGS + 77)
#define ID_RECBOX_01      (OFFSET_DIALOGS + 78)
#define ID_RECBOX_02      (OFFSET_DIALOGS + 79)
#define ID_RECBOX_03      101
/*
#define IDM_PLAYBOX_00     "Playback Game"
#define IDM_PLAYBOX_01     "&Close"
#define IDM_PLAYBOX_02     "&Play"
#define IDM_PLAYBOX_03     "&Reset"
#define IDM_PLAYBOX_04     "&Fast"
#define IDM_PLAYBOX_05     "&Slow"
#define IDM_PLAYBOX_06     "S&tep"
#define IDM_PLAYBOX_07     "Step :"
#define IDM_PLAYBOX_08     "of"
#define IDM_PLAYBOX_09     "0"
#define IDM_PLAYBOX_10     "0"
#define IDM_PLAYBOX_11     "Speed"
*/
#define ID_PLAYBOX_00     (OFFSET_DIALOGS + 80)
#define ID_PLAYBOX_01     (OFFSET_DIALOGS + 81)
#define ID_PLAYBOX_02     (OFFSET_DIALOGS + 82)
#define ID_PLAYBOX_03     (OFFSET_DIALOGS + 83)
#define ID_PLAYBOX_04     (OFFSET_DIALOGS + 84)
#define ID_PLAYBOX_05     (OFFSET_DIALOGS + 85)
#define ID_PLAYBOX_06     (OFFSET_DIALOGS + 86)
#define ID_PLAYBOX_07     (OFFSET_DIALOGS + 87)
#define ID_PLAYBOX_08     (OFFSET_DIALOGS + 88)
#define ID_PLAYBOX_09     103
#define ID_PLAYBOX_10     104
#define ID_PLAYBOX_11     (OFFSET_DIALOGS + 89)
/*
#define IDM_PICKLEVEL_00   "Save Laser Tank Level"
#define IDM_PICKLEVEL_01   "&Ok"
#define IDM_PICKLEVEL_02   "Enter the Level Number to"
#define IDM_PICKLEVEL_03   "Save :"
#define IDM_PICKLEVEL_04   "0 - 10"
#define IDM_PICKLEVEL_05   "icon1"
#define IDM_PICKLEVEL_06   "= Add"
#define IDM_PICKLEVEL_07   "11"
#define IDM_PICKLEVEL_08   "&Clear High Score"
*/
#define ID_PICKLEVEL_00   (OFFSET_DIALOGS + 90)
#define ID_PICKLEVEL_01   (OFFSET_DIALOGS + 91)
#define ID_PICKLEVEL_02   (OFFSET_DIALOGS + 92)
#define ID_PICKLEVEL_03   (OFFSET_DIALOGS + 93)
#define ID_PICKLEVEL_04   103
#define ID_PICKLEVEL_05   101
// MGY - About dialog Bug - 2005/07/08
#define ID_PICKLEVEL_06   (OFFSET_DIALOGS + 94)
#define ID_PICKLEVEL_07   104
#define ID_PICKLEVEL_08   (OFFSET_DIALOGS + 95)
