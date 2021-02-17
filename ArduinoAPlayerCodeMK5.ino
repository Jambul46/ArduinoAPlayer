//idea by Jambul46<hsd and code by DarthTux.
//easy to build WAV audio file player for nano328p etc... with atmega328p.
//this player should be able to play infinite amount of songs until it fits on your sd card.
//all libraries should be available in library manager.
//Main formats: WAV files, 8-bit, 16khz Sample Rate, mono, PCM unsigned 8-bit.
//https://audio.online-convert.com/convert-to-wav
//GERBERS for pcb: https://www.dropbox.com/s/l31omz06y4ecplu/ArduinoAPlayer.zip?dl=1

#include <ezButton.h>
#define SD_ChipSelectPin 10
#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>

static const uint8_t PAUSE_PIN = 5;
static const uint8_t NEXT_PIN = 6;  //change those numbers acording to pins u want to use to control volume,pause,next,back
static const uint8_t PREV_PIN = 7;                   //hold next for volume up and back for volume down

static const unsigned LAST_TRACK_ID = 75; // Changet this number according to number of your songs. Every song must be named "s1.wav,s2.wav,s3.wav etc..."
static const uint8_t VOLUME_MAX = 5;      // I would recommend to not touch this setting this is for setting maximum volume and level 6-7 does not sound good (max:7 default:5)
static const int LONG_THRESHOLD = 1000;   // Number of milliseconds until a long press is detected
static const int REPEAT_DELAY = 350;      // Number of milliseconds to wait before automatic volume change
static const int DONE_DELAY = 500;        // Removes annoying sound when automatically changing to next track

typedef enum {
  IDLE,
  SET_NEXT,   SET_PREV,
  NEXT,       PREV,
  VOL_UP,     VOL_DOWN,
  DONE,
  PAUSE,
  RESET,
} state_t;

ezButton next(NEXT_PIN);
ezButton prev(PREV_PIN);
ezButton pause(PAUSE_PIN);
TMRpcm tmrpcm;

uint32_t timer;
uint16_t track_id = 1;
uint8_t volume = 2;
uint8_t state;

void play(uint16_t next_id)
{
  // Since next_id is unsigned it cannot be negative hence
  // an overflow will always be greater than the last track ID!
  if (1 <= next_id && next_id <= LAST_TRACK_ID)
  {
    Serial.print("Playing track ");
    Serial.println(track_id = next_id);
    String song = String("s") + String(track_id = next_id) + String(".wav");
    tmrpcm.play(song.c_str());
  }
}

void set_volume(int n)
{
  if (n > 0 && volume < VOLUME_MAX) ++volume;
  else if (n == 0 && volume) --volume;
  Serial.print("Volume set to ");
  Serial.println(volume);
  tmrpcm.setVolume(volume);
}

void toggle_pause()
{
  static bool paused;
  paused = !paused;
  Serial.println(paused ? "Paused" : "Playing...");
  tmrpcm.pause();
}

void setup()
{
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD Failed");
    return;
  }
  next.setDebounceTime(50);
  prev.setDebounceTime(50);
  pause.setDebounceTime(50);
  Serial.println("Ready...");
  tmrpcm.speakerPin = 9;
  tmrpcm.quality(1);
  tmrpcm.setVolume(3);
  tmrpcm.play("s1.wav");
}

void loop()
{
  // MUST process button states!
  next.loop();
  prev.loop();
  pause.loop();

  switch (state)
  {
    case IDLE:
      if (next.isPressed()) state = SET_NEXT;
      if (prev.isPressed()) state = SET_PREV;
      if (pause.isPressed()) state = PAUSE;
      if (!tmrpcm.isPlaying()) state = DONE;
      break;

    // Manage short/long presses for both buttons
    case SET_NEXT:
    case SET_PREV:
      // Set timer if it wasn't yet and wait for a button to be released
      if (timer == 0) timer = millis();

      // Wait till either a long press or a button release occurs
      else
      {
        // This is a long press
        if (millis() - timer >= LONG_THRESHOLD) state += 4;

        // This is a button release event (occured before the long press threshold)
        if (next.isReleased() || prev.isReleased()) state += 2;
      }
      break;

    case NEXT:
    case PREV:
      play(track_id + (state == NEXT ? 1 : -1));
      state = RESET;
      break;

    case VOL_UP:
    case VOL_DOWN:
      // Stop as soon as buttons are released
      if (next.isReleased() || prev.isReleased()) state = RESET;

      // Keep changing volume for as long as the button is held down
      else if (millis() - timer > REPEAT_DELAY)
      {
        timer = millis();
        set_volume((state == VOL_UP ? 1 : 0));
      }
      break;

    case DONE:
      // Wait a little before switching to the next track
      if (timer == 0) timer = millis();
      else if (millis() - timer > DONE_DELAY) state = NEXT;
      break;

    case PAUSE:
      toggle_pause();
    // State will be reset automatically from here, hence no break


    case RESET:
      timer = 0;
      state = IDLE;
  }
}
