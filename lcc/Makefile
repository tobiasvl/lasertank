# Wedit Makefile for project LTank32_406i
SRCDIR=d:\lasertank
CFLAGS=-IC:\Lcc\Include  -O 
CC=$(LCCROOT)\bin\lcc.exe
LINKER=$(LCCROOT)\bin\lcclnk.exe
OBJS=\
	ltank2.obj \
	ltank_d.obj \
	ltank.res \
	ltank.obj \
	lt_sfx.obj \
	language.obj

LIBS=OLE32.LIB SHELL32.LIB WINMM.LIB
EXE=d:\lasertank\setups\files\lasertank.exe

$(EXE):	$(OBJS) Makefile
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\setups\files\lasertank.exe $(OBJS) $(LIBS)

# Build LTANK2.C
LTANK2_C=\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\stddef.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\win.h\
	C:\Lcc\Include\limits.h\
	C:\Lcc\Include\stdarg.h\
	C:\Lcc\Include\ctype.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\windowsx.h\
	C:\Lcc\Include\commdlg.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\mmsystem.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdio.h\
	C:\Lcc\Include\_syslist.h\
	$(SRCDIR)\ltank.h\
	C:\Lcc\Include\windows.h\
	$(SRCDIR)\lt32l_us.h\
	$(SRCDIR)\ltank_d.h\
	$(SRCDIR)\lt_sfx.h\

ltank2.obj: $(LTANK2_C) $(SRCDIR)\ltank2.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ltank2.c

# Build LTANK_D.C
LTANK_D_C=\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\stddef.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\win.h\
	C:\Lcc\Include\limits.h\
	C:\Lcc\Include\stdarg.h\
	C:\Lcc\Include\ctype.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\stdio.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\objbase.h\
	C:\Lcc\Include\rpc.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\rpcdce.h\
	C:\Lcc\Include\rpcdcep.h\
	C:\Lcc\Include\rpcnsi.h\
	C:\Lcc\Include\rpcnterr.h\
	C:\Lcc\Include\excpt.h\
	C:\Lcc\Include\winerror.h\
	C:\Lcc\Include\rpcasync.h\
	C:\Lcc\Include\rpcndr.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\rpcnsip.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\wtypes.h\
	C:\Lcc\Include\unknwn.h\
	C:\Lcc\Include\wtypes.h\
	C:\Lcc\Include\objidl.h\
	C:\Lcc\Include\unknwn.h\
	C:\Lcc\Include\cguid.h\
	C:\Lcc\Include\urlmon.h\
	C:\Lcc\Include\rpc.h\
	C:\Lcc\Include\rpcndr.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\objidl.h\
	C:\Lcc\Include\oleidl.h\
	C:\Lcc\Include\rpcbase.h\
	C:\Lcc\Include\servprov.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\msxml.h\
	C:\Lcc\Include\oaidl.h\
	C:\Lcc\Include\propidl.h\
	C:\Lcc\Include\oaidl.h\
	C:\Lcc\Include\oleauto.h\
	C:\Lcc\Include\shlobj.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\prsht.h\
	C:\Lcc\Include\commctrl.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\shlguid.h\
	C:\Lcc\Include\pshpack1.h\
	C:\Lcc\Include\shtypes.h\
	C:\Lcc\Include\rpc.h\
	C:\Lcc\Include\rpcndr.h\
	C:\Lcc\Include\wtypes.h\
	C:\Lcc\Include\pshpack1.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack1.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack1.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\shobjidl.h\
	C:\Lcc\Include\rpc.h\
	C:\Lcc\Include\rpcndr.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\objidl.h\
	C:\Lcc\Include\oleidl.h\
	C:\Lcc\Include\rpcbase.h\
	C:\Lcc\Include\oaidl.h\
	C:\Lcc\Include\shtypes.h\
	C:\Lcc\Include\rpc.h\
	C:\Lcc\Include\rpcndr.h\
	C:\Lcc\Include\servprov.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\comcat.h\
	C:\Lcc\Include\rpc.h\
	C:\Lcc\Include\rpcndr.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\ole2.h\
	C:\Lcc\Include\unknwn.h\
	C:\Lcc\Include\propidl.h\
	C:\Lcc\Include\prsht.h\
	C:\Lcc\Include\msxml.h\
	C:\Lcc\Include\prsht.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\commctrl.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\wininet.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\pshpack4.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack1.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\pshpack8.h\
	C:\Lcc\Include\poppack.h\
	C:\Lcc\Include\poppack.h\
	$(SRCDIR)\ltank.h\
	C:\Lcc\Include\windows.h\
	$(SRCDIR)\lt32l_us.h\
	$(SRCDIR)\ltank_d.h\

ltank_d.obj: $(LTANK_D_C) $(SRCDIR)\ltank_d.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ltank_d.c

# Build ltank.res
LTANK_RC=\
	$(SRCDIR)\lt32l_us.h\
	$(SRCDIR)\lt32l_us.inc\

ltank.res:	$(LTANK_RC) d:\lasertank\ltank.rc
	$(LCCROOT)\bin\lrc -Id:\lasertank -IC:\Lcc\Include  d:\lasertank\ltank.rc

# Build LTANK.C
LTANK_C=\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\stddef.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\win.h\
	C:\Lcc\Include\limits.h\
	C:\Lcc\Include\stdarg.h\
	C:\Lcc\Include\ctype.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\stdio.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\mmsystem.h\
	C:\Lcc\Include\windows.h\
	$(SRCDIR)\ltank.h\
	C:\Lcc\Include\windows.h\
	$(SRCDIR)\lt32l_us.h\
	$(SRCDIR)\ltank_d.h\
	$(SRCDIR)\lt_sfx.h\

ltank.obj: $(LTANK_C) $(SRCDIR)\ltank.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\ltank.c

# Build LT_SFX.C
LT_SFX_C=\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\stddef.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\win.h\
	C:\Lcc\Include\limits.h\
	C:\Lcc\Include\stdarg.h\
	C:\Lcc\Include\ctype.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\mmsystem.h\
	C:\Lcc\Include\windows.h\
	$(SRCDIR)\lt_sfx.h\

lt_sfx.obj: $(LT_SFX_C) $(SRCDIR)\lt_sfx.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\lt_sfx.c

# Build LANGUAGE.C
LANGUAGE_C=\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\stddef.h\
	C:\Lcc\Include\_syslist.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\win.h\
	C:\Lcc\Include\limits.h\
	C:\Lcc\Include\stdarg.h\
	C:\Lcc\Include\ctype.h\
	C:\Lcc\Include\basetsd.h\
	C:\Lcc\Include\windowsx.h\
	C:\Lcc\Include\commdlg.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\string.h\
	C:\Lcc\Include\mmsystem.h\
	C:\Lcc\Include\windows.h\
	C:\Lcc\Include\stdlib.h\
	C:\Lcc\Include\stdio.h\
	C:\Lcc\Include\_syslist.h\
	$(SRCDIR)\ltank.h\
	C:\Lcc\Include\windows.h\
	$(SRCDIR)\lt32l_us.h\

language.obj: $(LANGUAGE_C) $(SRCDIR)\language.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\language.c

link:
	$(LINKER)  -s -subsystem windows -o $(SRCDIR)\setups\files\lasertank.exe $(OBJS) $(LIBS)

clean:
	del $(OBJS) d:\lasertank\setups\files\lasertank.exe
