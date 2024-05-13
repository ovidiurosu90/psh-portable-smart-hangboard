#ifndef MyTFTeSPI_h
#define MyTFTeSPI_h

#include <SPI.h>
#include <TFT_eSPI.h>

#include <Arduino.h>
#include "SD.h"

#define M_SIZE 0.667 // Define meter size
#define TFT_GREY   0x5AEB
//#define TFT_ORANGE 0xFD20 /* 255, 165, 0 */

//NOTE The colors are off, and I don't know why, so we need a translation
/*
// Experiments
https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_eSPI.h#L305
TFT_RED = dark_blue
TFT_ORANGE = light_blue
TFT_GREEN = green
TFT_GOLD = light_blue

TFT_BROWN = dark_blue
TFT_YELLOW = light_blue
TFT_BLUE = red
TFT_PURPLE = purple

TFT_CYAN = yellow
TFT_MAGENTA = magenta / purple
TFT_MAROON = dark_blue
TFT_DARKGREEN = green

TFT_NAVY = red
TFT_PINK = grey
TFT_DARKCYAN = green
TFT_OLIVE = light_blue

TFT_LIGHTGREY = grey
TFT_DARKGREY = dark_grey
TFT_GREENYELLOW = light_blue
TFT_SILVER = grey

TFT_SKYBLUE = yellow_maroon
TFT_VIOLET = purple
TFT_YELLOW = light_blue
TFT_RED = dark_blue
*/
#define MYCOLOR_YELLOW TFT_CYAN
#define MYCOLOR_GREEN TFT_GREEN
#define MYCOLOR_BLUE TFT_YELLOW


class MyTFTeSPI
{
public:
    MyTFTeSPI();

    fs::SDFS initSD(uint8_t ssPin);
    void beginSD();
    void endSD();

    void plotNeedle(int value, byte ms_delay);
    void plotAnalogMeter();
    void plotCycleStats();
    void plotScore();
    void plotFooter();
    void plotFooterLine1(const char *line);
    void plotFooterLine2(const char *line);
    void plotFooterLine3(const char *line);
    void plotTotal(float total = 0.0);
    void printStreamTitle(const char *streamTitle);
    void setCountdown(uint8_t countdown);
    void setAction(const char *action);
    void setCycleStats(const char *cycleStats);

    void setValueColorNeutral();
    void setValueColorWithinBounds();
    void setValueColorUnderBound();
    void setValueColorOverBound();

    void loop();

protected:

private:
    TFT_eSPI* _tft;
    uint8_t _ssPin;

    uint32_t _updateTime = 0;

    char _valueBuffer[4] = " 0";
    uint16_t _valueColor = TFT_BLACK;

    char _cycleStatsBuffer[6] = "cycle";

    uint32_t _score = 0;
    char _scoreBuffer[6] = "score";

    uint8_t _countdownValue = -1;
    char _countdownBuffer[4] = " ";

    char _actionBuffer[6] = "idle";

    // For plotNeedle
    float _ltx = 0;    // Saved x coord of bottom of needle
    uint16_t _osx = M_SIZE * 120, _osy = M_SIZE * 120; // Saved x & y coords
    int _old_analog =  -999; // Value last displayed

};

#endif

