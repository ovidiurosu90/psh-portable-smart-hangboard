#include <MyESP32AudioI2S.h>

MyESP32AudioI2S::MyESP32AudioI2S(const uint8_t BCLKpin, const uint8_t LRCpin,
    const uint8_t DOUTpin
) {
    setPinout(BCLKpin, LRCpin, DOUTpin);

}

// E.g. myESP32AudioI2S->connectToHost("0n-80s.radionetz.de:8000/0n-70s.mp3");
bool MyESP32AudioI2S::connectToHost(const char* host, const char* username,
    const char* password
) {
    return connecttohost(host, username, password);
}

/* E.g.
// NOTE .flac files only work with PSRAM! ESP32-S doesn't have that!
// myESP32AudioI2S->connectToFS(SD, "/PSH/10_seconds_countdown.flac");

// myESP32AudioI2S->connectToFS(SD, "/PSH/get_ready.mp3");
*/
bool MyESP32AudioI2S::connectToFS(fs::FS fs, const char* path)
{
    return connecttoFS(fs, path);
}

void MyESP32AudioI2S::stopSound()
{
    if (isRunning()) {
        Serial.println("Stopping sound...");
        strcpy(_soundPath, "");
        pauseResume();
    }

    _isPlayingGetReady                = false;
    _isPlaying10secondsCountdown      = false;
    _isPlaying321Beep                 = false;
    _isPlayingMeasurementWithinBounds = false;
    _isPlayingMeasurementOutOfBounds  = false;
    _isPlayingRestSound               = false;
    _isPlayingEndSound                = false;
}

void MyESP32AudioI2S::playGetReady()
{
    if (_isPlayingGetReady) {
        return;
    }
    _isPlayingGetReady = true;
    strcpy(_soundPath, "/PSH/get_ready.mp3");
    Serial.print("Playing GetReady. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stopGetReady()
{
    _isPlayingGetReady = false;
    stopSound();
}

void MyESP32AudioI2S::play10secondsCountdown()
{
    if (_isPlaying10secondsCountdown) {
        return;
    }
    _isPlaying10secondsCountdown = true;
    strcpy(_soundPath, "/PSH/10_seconds_countdown.mp3");
    Serial.print("Playing 10secondsCountdown. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stop10secondsCountdown()
{
    _isPlaying10secondsCountdown = false;
    stopSound();
}

void MyESP32AudioI2S::play321Beep()
{
    if (_isPlaying321Beep) {
        return;
    }
    _isPlaying321Beep = true;
    strcpy(_soundPath, "/PSH/3_2_1_beep.mp3");
    Serial.print("Playing 321Beep. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stop321Beep()
{
    _isPlaying321Beep = false;
    stopSound();
}

void MyESP32AudioI2S::playMeasurementWithinBounds()
{
    if (_isPlayingMeasurementOutOfBounds) {
        stopMeasurementOutOfBounds();
    }
    if (_isPlayingMeasurementWithinBounds) {
        return;
    }
    _isPlayingMeasurementWithinBounds = true;
    strcpy(_soundPath, "/PSH/10_seconds_ticks.mp3");
    Serial.print("Playing MeasurementWithinBounds. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stopMeasurementWithinBounds()
{
    _isPlayingMeasurementWithinBounds = false;
    stopSound();
}

void MyESP32AudioI2S::playMeasurementOutOfBounds()
{
    if (_isPlayingMeasurementWithinBounds) {
        stopMeasurementWithinBounds();
    }
    if (_isPlayingMeasurementOutOfBounds) {
        return;
    }
    _isPlayingMeasurementOutOfBounds = true;
    strcpy(_soundPath, "/PSH/error1.mp3");
    Serial.print("Playing MeasurementOutOfBounds. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stopMeasurementOutOfBounds()
{
    _isPlayingMeasurementOutOfBounds = false;
    stopSound();
}

void MyESP32AudioI2S::playRestSound()
{
    if (_isPlayingRestSound) {
        return;
    }
    _isPlayingRestSound = true;
    strcpy(_soundPath, "/PSH/rest.mp3");
    Serial.print("Playing RestSound. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stopRestSound()
{
    _isPlayingRestSound = false;
    stopSound();
}

void MyESP32AudioI2S::playEndSound()
{
    if (_isPlayingEndSound) {
        return;
    }
    _isPlayingEndSound = true;
    strcpy(_soundPath, "/PSH/beep_beep_beep.mp3");
    Serial.print("Playing EndSound. Path: "); Serial.println(_soundPath);
    connectToFS(SD, _soundPath);
}
void MyESP32AudioI2S::stopEndSound()
{
    _isPlayingEndSound = false;
    stopSound();
}

const uint32_t MyESP32AudioI2S::get321BeepSoundLengthMillis()
{
    return _321BeepSoundLengthMillis;
}

