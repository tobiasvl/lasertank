@ECHO OFF


SET Version=412
SET Source="..\Files\LaserTank%Version%.exe"
SET Target="..\Files\LaserTank.exe"
SET Backup=LaserTank%Version%.exe

cls
ECHO =============================================================================
ECHO == Compress (with UPX) and Setup LaserTank
ECHO == Variables :
ECHO ==   Version = "%Version%" (Version de LaserTank)
ECHO ==   Source  = %Source%
ECHO ==   Target  = %Target%
ECHO ==   Backup  = "%Backup%"
ECHO =============================================================================
ECHO.
ECHO.
ECHO.


ECHO Compress...
IF NOT EXIST %Source% GOTO :Ready
    MOVE %Source% %Backup%

IF NOT EXIST %Target% GOTO :Compress
    DEL %Target%

:Compress
    upx.exe -9 -v -o%Target% %Backup%

:Ready
IF EXIST %Target% GOTO :TargetFound
    ECHO Error, %Target% not found.
    GOTO CleanUP

:TargetFound
IF NOT EXIST %Backup% GOTO :SetupGen
    DEL %Backup%

:SetupGen 
    ECHO SetupGen...
    Setupgen.exe

:CleanUP
    ECHO End.
    SET Target=
    SET Source=
    SET Version=
    PAUSE


