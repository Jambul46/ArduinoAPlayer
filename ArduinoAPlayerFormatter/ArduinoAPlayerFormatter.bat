@ECHO OFF
COLOR 06
MKDIR notConverted
MKDIR Converted16kHz
MKDIR Converted24kHz
MKDIR Converted32kHz
CLS
ECHO   ArduinoAPlayerFormatter select sample rate
ECHO  --------------------------------------------
ECHO[
ECHO     1. 16kHz - work's perfect, audio quality is a bit worse. (max vol:5)
ECHO     2. 24kHz - work's perfect, audio quality is good. (max vol:5)
ECHO     3. 32kHz - work's OK, audio quality is perfect. (max vol:4) vol:5 makes annoying sound's - unlistenable
ECHO[
ECHO[
ECHO[
ECHO		!  Note that all songs have to be named song1,2,3.wav  !
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
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 16000 -ac 1 Converted16kHz/s%%i.wav)
GOTO End

:24kHzSelected
ECHO[
ECHO 24kHz Selected Proceeding
TIMEOUT 3
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 24000 -ac 1 Converted24kHz/s%%i.wav)
GOTO End

:32kHzSelected
ECHO[
ECHO 32kHz Selected Proceeding
TIMEOUT 3
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 32000 -ac 1 Converted32kHz/s%%i.wav)
GOTO End

:End
PAUSE