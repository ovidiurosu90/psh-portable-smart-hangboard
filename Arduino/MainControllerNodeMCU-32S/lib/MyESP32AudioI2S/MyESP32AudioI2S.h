#ifndef MyESP32AudioI2S_h
#define MyESP32AudioI2S_h

#include "Audio.h"
#include <FS.h>
#include <Preferences.h>
#include <MyTFTeSPI.h>

class MyESP32AudioI2S: public Audio
{
public:
    MyESP32AudioI2S(const uint8_t BCLKpin, const uint8_t LRCpin, const uint8_t DOUTpin);

    bool connectToHost(const char* host, const char* username = "",
                       const char* password = "");
    bool connectToFS(fs::FS fs, const char* path);

    void setVolumeIfNotInNVM(uint8_t vol);
    void decreaseVolume(MyTFTeSPI* myTFTeSPI);
    void storeVolumeInNVM(MyTFTeSPI* myTFTeSPI);
    void increaseVolume(MyTFTeSPI* myTFTeSPI);

    void stopSound();

    void playGetReady();
    void stopGetReady();

    void play10secondsCountdown();
    void stop10secondsCountdown();

    void play321Beep();
    void stop321Beep();

    void playMeasurementWithinBounds();
    void stopMeasurementWithinBounds();

    void playMeasurementOutOfBounds();
    void stopMeasurementOutOfBounds();

    void playRestSound();
    void stopRestSound();

    void playEndSound();
    void stopEndSound();

    const uint32_t get321BeepSoundLengthMillis();


protected:

private:
    Preferences _prefsVolume;
    const bool _PREFS_RW_MODE = false;
    const bool _PREFS_RO_MODE = true;
    const char* _prefsVolName = "volPrefs";

    uint8_t _volume = 8;
    char _soundPath[64];

    bool _isPlayingGetReady                = false;
    bool _isPlaying10secondsCountdown      = false;
    bool _isPlaying321Beep                 = false;
    bool _isPlayingMeasurementWithinBounds = false;
    bool _isPlayingMeasurementOutOfBounds  = false;
    bool _isPlayingRestSound               = false;
    bool _isPlayingEndSound                = false;

    const uint32_t _321BeepSoundLengthMillis = 3720;
};

#endif

