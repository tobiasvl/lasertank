/*******************************************************
 **             Laser Tank ver 3.2a                   **
 **               By Jim Kindley                      **
 **               (c) 2000                            **
 **         The Program and Source is Public Domain   **
 *******************************************************/



// Dialog Callback functions
LRESULT CALLBACK DiffBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DefBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK LoadBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HintBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PickBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WinBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HSBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RecordBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK PBWindow(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HSList(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GHSList(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GraphBox(HWND, UINT, WPARAM, LPARAM);

extern int FileHand, HSClear;
extern HWND PlayH,PBCountH;
