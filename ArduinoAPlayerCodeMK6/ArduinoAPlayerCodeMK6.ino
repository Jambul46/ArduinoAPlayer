//Arduino Audio Player
//idea by Jambul46<hsd and code by DarthTux.
//this player should be able to play 65535 song's from one SD.
//all libraries should be available in library manager.
//Main format: WAV, 8-bit, 16/24/32khz Sample Rate, mono, PCM unsigned 8-bit.
//official page: https://github.com/Jambul46/ArduinoAPlayer
//This code was made for MK3 board. will work with MK2 but there has to be jumper from GND to D4 (if you use PCBGerberMK2 then there is predone jumper on the PCB)

#include <ezButton.h>
#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>

static const uint8_t SDCARD_PIN = 4;
static const uint8_t PAUSE_PIN = 5;
static const uint8_t NEXT_PIN = 6;        //change those numbers acording to pins u want to use to control volume,pause,next,back
static const uint8_t PREV_PIN = 7;        //hold next for volume up and back for volume down
static const uint8_t SD_ChipSelectPin = 10;


static const uint8_t VOLUME_MAX = 5;      // I would recommend to not touch this setting this is for setting maximum volume and level 6-7 does not sound good (max:7 default:5)
static const int LONG_THRESHOLD = 1000;   // Number of milliseconds until a long press is detected
static const int REPEAT_DELAY = 350;      // Number of milliseconds to wait before automatic volume change
static const int DONE_DELAY = 500;        // Removes annoying sound when automatically changing to next track

typedef enum {
  DETECT,
  IDLE,
  SET_NEXT, SET_PREV, SET_PAUSE,
  NEXT,     PREV,     PAUSE,
  VOL_UP,   VOL_DOWN, UNMOUNT,
  DONE,
  RESET,
} state_t;

ezButton next(NEXT_PIN);
ezButton prev(PREV_PIN);
ezButton pause(PAUSE_PIN);
ezButton detect(SDCARD_PIN);
TMRpcm tmrpcm;

uint16_t track_id;
uint8_t volume = 2;
static uint8_t state;

bool play(uint16_t next_id)
{
  // Return false if a track couldn't be played (ID==0 or file not found
  if (next_id < 1) return true;

  // Build track name from its ID and make sure the file exists
  char track_name[16];                // Better not use String at all
  track_name[0] = 's';                // file name prefix
  utoa(track_id = next_id, &track_name[1], 10); // append track ID, with terminating NUL
  strcat(track_name, ".wav");         // then add file extension

  // Now play the track since it exists
  Serial.print("Checking track ");
  Serial.println(track_id);
  tmrpcm.play(track_name);

  // Maybe there should be a small delay here?
  return tmrpcm.isPlaying();
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

void unmount()
{
  tmrpcm.stopPlayback();  // Stop playing
  SD.end();               // Prepare for removing the card
  track_id = 0;
  Serial.println("Stopped. Safe removal: Ok");
}

bool start()
{
  // The SD card must be properly initialized
  if (SD.begin(SD_ChipSelectPin)) return true;

  Serial.println("SD Failed. Halting.");
  return false;
}

void setup()
{
  Serial.begin(9600);

  next.setDebounceTime(50);
  prev.setDebounceTime(50);
  pause.setDebounceTime(50);

  tmrpcm.speakerPin = 9;
  tmrpcm.quality(1);
  tmrpcm.setVolume(3);

  if (digitalRead(SDCARD_PIN) == LOW && start()) state = DONE;
  if (state == DETECT) Serial.println("Ready...");
}

void loop()
{
  // MUST process button states!
  next.loop();
  prev.loop();
  pause.loop();
  detect.loop();

  static uint32_t timer;

  switch (state)
  {
    case DETECT:
      if (detect.isPressed() && start()) state = DONE;
      break;

    case IDLE:
      if (next.isPressed()) state = SET_NEXT;
      if (prev.isPressed()) state = SET_PREV;
      if (pause.isPressed()) state = SET_PAUSE;
      if (!tmrpcm.isPlaying()) state = DONE;
      break;

    // Manage short/long presses for both buttons
    case SET_NEXT:
    case SET_PREV:
    case SET_PAUSE:
      // Set timer if it wasn't yet and wait for a button to be released
      if (timer == 0) timer = millis();

      // Wait till either a long press or a button release occurs
      else
      {
        // This is a long press
        if (millis() - timer >= LONG_THRESHOLD) state += 6;

        // This is a button release event (occured before the long press threshold)
        if (next.isReleased() || prev.isReleased() || pause.isReleased()) state += 3;
      }
      break;

    case NEXT:
    case PREV:
      state = play(track_id + (state == NEXT ? 1 : -1)) ? RESET : UNMOUNT;
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

    case UNMOUNT:
      unmount();
      state = DETECT;
      break;

    case PAUSE:
      toggle_pause();
    // State will be reset automatically from here, hence no break

    case RESET:
      timer = 0;
      state = IDLE;
  }
}
