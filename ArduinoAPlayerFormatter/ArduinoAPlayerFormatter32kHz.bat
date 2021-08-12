mkdir Converted32kHz
mkdir notConverted
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 32000 -ac 1 Converted32kHz/s%%i.wav)