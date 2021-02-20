mkdir Converted
mkdir notConverted
FOR /L %%i IN (1,1,255) DO (ffmpeg -i notConverted/song%%i.mp3 -acodec pcm_u8 -ar 16000 -ac 1 Converted/s%%i.wav)