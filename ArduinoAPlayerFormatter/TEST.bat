@ECHO OFF
CLS
ECHO   ArduinoAPlayerFormatter select sample rate
ECHO  --------------------------------------------
ECHO[
ECHO     1.16kHz
ECHO     2.24kHz
ECHO     3.32kHz
ECHO[

CHOICE /M "Select" /C 123

:: Note - list ERRORLEVELS in decreasing order
IF ERRORLEVEL 3 GOTO 32kHzSelected
IF ERRORLEVEL 2 GOTO 24kHzSelected
IF ERRORLEVEL 1 GOTO 16kHzSelected

:16kHzSelected
ECHO[
ECHO 16kHz Selected Proceeding
TIMEOUT 3
mkdir Converted16kHz
mkdir notConverted
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 16000 -ac 1 Converted16kHz/s%%i.wav)
GOTO End

:24kHzSelected
ECHO[
ECHO 24kHz Selected Proceeding
TIMEOUT 3
mkdir Converted24kHz
mkdir notConverted
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 24000 -ac 1 Converted24kHz/s%%i.wav)
GOTO End

:32kHzSelected
ECHO[
ECHO 32kHz Selected Proceeding
TIMEOUT 3
mkdir Converted32kHz
mkdir notConverted
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 32000 -ac 1 Converted32kHz/s%%i.wav)
GOTO End

:End
PAUSE