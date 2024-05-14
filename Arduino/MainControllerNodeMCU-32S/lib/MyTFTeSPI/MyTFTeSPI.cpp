#include <MyTFTeSPI.h>

MyTFTeSPI::MyTFTeSPI(MySwitchesModule* mySwitchesModule)
{
    _mySwitchesModule = mySwitchesModule;

    _tft = new TFT_eSPI();
    _tft->init();
    _tft->setRotation(3);
    _tft->setFreeFont(NULL); // to select the GLCD (for size 1)
    _tft->fillScreen(TFT_BLACK);

    plotAnalogMeter();
    plotFooter();
    _updateTime = millis();
}

fs::SDFS MyTFTeSPI::initSD(uint8_t ssPin)
{
    _ssPin = ssPin;

    pinMode(_ssPin, OUTPUT);
    pinMode(TFT_CS, OUTPUT); //NOTE TFT_CS in TFT_eSPI/User_Setup.h
    digitalWrite(_ssPin, HIGH);  // SD_CS
    digitalWrite(TFT_CS, HIGH); // TFT_CS

    beginSD();
    return SD;
}


// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void MyTFTeSPI::plotNeedle(int value, byte ms_delay)
{
    _tft->setTextColor(TFT_BLACK, TFT_WHITE);
    dtostrf(value, 3, 0, _valueBuffer);

    // Draw lower left value
    // _tft->drawRightString(_valueBuffer, 33, M_SIZE*(119 - 20), 2);
    // _tft->drawRightString(" 0", 33, M_SIZE*(119 - 20), 2);

    if (value < -10) value = -10; // Limit value to emulate needle end stops
    if (value > 110) value = 110;

    // Move the needle until new value reached
    while (!(value == _old_analog)) {
        if (_old_analog < value) _old_analog++;
        else _old_analog--;

        if (ms_delay == 0) _old_analog = value; // Update immediately if delay is 0

        float sdeg = map(_old_analog, -10, 110, -150, -30); // Map value to angle
        // Calculate tip of needle coords
        float sx = cos(sdeg * 0.0174532925);
        float sy = sin(sdeg * 0.0174532925);

        // Calculate x delta of needle start (does not start at pivot point)
        float tx = tan((sdeg + 90) * 0.0174532925);

        // Erase old needle image
        _tft->drawLine(M_SIZE*(120 + 24 * _ltx) - 1, M_SIZE*(150 - 24), _osx - 1, _osy,
            TFT_WHITE);
        _tft->drawLine(M_SIZE*(120 + 24 * _ltx), M_SIZE*(150 - 24), _osx, _osy,
            TFT_WHITE);
        _tft->drawLine(M_SIZE*(120 + 24 * _ltx) + 1, M_SIZE*(150 - 24), _osx + 1, _osy,
            TFT_WHITE);

        // Re-plot text under needle
        _tft->setTextColor(_valueColor, TFT_WHITE);

        // Draw middle center value
        // _tft->drawCentreString("%RH", M_SIZE*120, M_SIZE*75, 4);
        _tft->fillRect(M_SIZE*80, M_SIZE*75, 60, 28, TFT_WHITE);
        _tft->drawCentreString(_valueBuffer, M_SIZE*114, M_SIZE*75, 4);

        // Store new needle end coords for next erase
        _ltx = tx;
        _osx = M_SIZE*(sx * 98 + 120);
        _osy = M_SIZE*(sy * 98 + 150);

        // Draw the needle in the new postion, magenta makes needle a bit bolder
        // draws 3 lines to thicken needle
        _tft->drawLine(M_SIZE*(120 + 24 * _ltx) - 1, M_SIZE*(150 - 24), _osx - 1, _osy,
            TFT_RED);
        _tft->drawLine(M_SIZE*(120 + 24 * _ltx), M_SIZE*(150 - 24), _osx, _osy,
            TFT_MAGENTA);
        _tft->drawLine(M_SIZE*(120 + 24 * _ltx) + 1, M_SIZE*(150 - 24), _osx + 1, _osy,
            TFT_RED);

        // Slow needle down slightly as it approaches new postion
        if (abs(_old_analog - value) < 10) ms_delay += ms_delay / 5;

        // Wait before next update
        delay(ms_delay);
    }
}

void MyTFTeSPI::plotAnalogMeter()
{
    // Meter outline
    _tft->fillRect(0, 0, M_SIZE*239, M_SIZE*131, TFT_GREY);
    _tft->fillRect(1, M_SIZE*3, M_SIZE*234, M_SIZE*125, TFT_WHITE);

    _tft->setTextColor(TFT_BLACK);

    plotCycleStats();
    plotScore();


    // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
    for (int i = -50; i < 51; i += 5) {
        // Long scale tick length
        int tl = 15;

        // Coordinates of tick to draw
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
        uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*150;
        uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
        uint16_t y1 = sy * M_SIZE*100 + M_SIZE*150;

        // Coordinates of next tick for zone fill
        float sx2 = cos((i + 5 - 90) * 0.0174532925);
        float sy2 = sin((i + 5 - 90) * 0.0174532925);
        int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
        int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*150;
        int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
        int y3 = sy2 * M_SIZE*100 + M_SIZE*150;

        if (i >= -50 && i < -25) {
            _tft->fillTriangle(x0, y0, x1, y1, x2, y2, MYCOLOR_YELLOW);
            _tft->fillTriangle(x1, y1, x2, y2, x3, y3, MYCOLOR_YELLOW);
        }

        if (i >= -25 && i < 0) {
            _tft->fillTriangle(x0, y0, x1, y1, x2, y2, MYCOLOR_YELLOW);
            _tft->fillTriangle(x1, y1, x2, y2, x3, y3, MYCOLOR_YELLOW);
        }

        if (i >= 0 && i < 25) {
            _tft->fillTriangle(x0, y0, x1, y1, x2, y2, MYCOLOR_GREEN);
            _tft->fillTriangle(x1, y1, x2, y2, x3, y3, MYCOLOR_GREEN);
        }

        if (i >= 25 && i < 50) {
            _tft->fillTriangle(x0, y0, x1, y1, x2, y2, MYCOLOR_BLUE);
            _tft->fillTriangle(x1, y1, x2, y2, x3, y3, MYCOLOR_BLUE);
        }

        // Short scale tick length
        if (i % 25 != 0) tl = 8;

        // Recalculate coords incase tick lenght changed
        x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
        y0 = sy * (M_SIZE*100 + tl) + M_SIZE*150;
        x1 = sx * M_SIZE*100 + M_SIZE*120;
        y1 = sy * M_SIZE*100 + M_SIZE*150;

        // Draw tick
        _tft->drawLine(x0, y0, x1, y1, TFT_BLACK);

        // Check if labels should be drawn, with position tweaks
        if (i % 25 == 0) {
            // Calculate label positions
            x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
            y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*150;
            switch (i / 25) {
                case -2:
                    _tft->setTextColor(TFT_BLACK);
                    _tft->drawCentreString("0", x0+4, y0-4, 1);
                    break;
                case -1:
                    _tft->setTextColor(TFT_BLACK);
                    _tft->drawCentreString("25", x0+2, y0, 1);
                    break;
                case 0:
                    _tft->setTextColor(MYCOLOR_GREEN);
                    _tft->drawCentreString("50", x0, y0, 1);
                    break;
                case 1:
                    _tft->setTextColor(MYCOLOR_GREEN);
                    _tft->drawCentreString("75", x0, y0, 1);
                    break;
                case 2:
                    _tft->setTextColor(TFT_BLACK);
                    _tft->drawCentreString("100", x0-2, y0-4, 1);
                    break;
            }
            _tft->setTextColor(TFT_BLACK);
        }

        // Now draw the arc of the scale
        sx = cos((i + 5 - 90) * 0.0174532925);
        sy = sin((i + 5 - 90) * 0.0174532925);
        x0 = sx * M_SIZE*100 + M_SIZE*120;
        y0 = sy * M_SIZE*100 + M_SIZE*150;
        // Draw scale arc, don't draw the last part
        if (i < 50) _tft->drawLine(x0, y0, x1, y1, TFT_BLACK);
    }

    // Units at bottom right
    // Draw lower right value
    // _tft->drawString("MAX", M_SIZE*(3 + 230 - 40), M_SIZE*(119 - 20), 2);

    // Draw middle center value
    // _tft->drawCentreString("%RH", M_SIZE*120, M_SIZE*75, 4);
    _tft->fillRect(M_SIZE*120, M_SIZE*75, 32, 32, TFT_WHITE);
    _tft->drawCentreString(" 0", M_SIZE*120, M_SIZE*75, 4);

    _tft->drawRect(1, M_SIZE*3, M_SIZE*236, M_SIZE*126, TFT_BLACK); // Draw bezel line

    plotNeedle(0, 0); // Put meter needle at 0
}

void MyTFTeSPI::plotCycleStats()
{
    _tft->fillRect(M_SIZE*(6), M_SIZE*(6), M_SIZE*47, M_SIZE*18, TFT_WHITE);
    _tft->setTextColor(TFT_BLACK, TFT_WHITE);
    _tft->drawString(_cycleStatsBuffer, M_SIZE*(6), M_SIZE*(6), 1);
}

void MyTFTeSPI::plotScore()
{
    if (_scoreValue != -1 && _scoreValue != UINT32_MAX) {
        sprintf(_scoreBuffer, "%d", _scoreValue);
    } else {
        sprintf(_scoreBuffer, "%s", "score");
    }

    _tft->fillRect(M_SIZE*(190), M_SIZE*(6), M_SIZE*47, M_SIZE*18, TFT_WHITE);
    _tft->setTextColor(TFT_BLACK, TFT_WHITE);
    _tft->drawRightString(_scoreBuffer, M_SIZE*(236), M_SIZE*(6), 1);
}

void MyTFTeSPI::plotAction()
{
    _tft->fillRect(M_SIZE*(6), M_SIZE*(100), M_SIZE*44, M_SIZE*26, TFT_WHITE);
    _tft->setTextColor(TFT_BLACK, TFT_WHITE);
    _tft->drawCentreString(_actionBuffer, M_SIZE*(28), M_SIZE*(100), 2);
}

void MyTFTeSPI::plotCountdown()
{
    _tft->fillRect(M_SIZE*(200), M_SIZE*(100), M_SIZE*36, M_SIZE*26, TFT_WHITE);
    _tft->setTextColor(TFT_BLACK, TFT_WHITE);
    if (_countdownValue != -1 && _countdownValue != UINT8_MAX) {
        sprintf(_countdownBuffer, "%d", _countdownValue);
    } else {
        sprintf(_countdownBuffer, " %c ", 'x');
    }
    _tft->drawCentreString(_countdownBuffer, M_SIZE*(216), M_SIZE*(100), 2);
}

void MyTFTeSPI::plotFooter()
{
    // Under the analog meter
    if(_mySwitchesModule->sw4IsToggledOn()) {
        plotFooterLine1("Press sw1 to start training");
        plotFooterLine2("Press sw3 to end training");
        plotFooterLine3("S1:Train  S2:Calib  S3:End");
    } else {
        plotFooterLine1("Press sw1 to decrease volume");
        plotFooterLine2("Press sw3 to increase volume");
        if(_mySwitchesModule->sw5IsToggledOn()) {
            plotFooterLine3("S1:Vol-   S2:Vol=  S3:Vol+");
        } else {
            plotFooterLine3("S1:Vol-   S2:x     S3:Vol+");
        }
    }
}

void MyTFTeSPI::plotFooterLine1(const char *line)
{
    _tft->fillRect(M_SIZE*1, M_SIZE*136, M_SIZE*240, M_SIZE*14, TFT_BLACK);
    _tft->setTextColor(TFT_WHITE, TFT_BLACK);
    _tft->drawString(line, M_SIZE*(1), M_SIZE*(136), 1);
}
void MyTFTeSPI::plotFooterLine2(const char *line)
{
    _tft->fillRect(M_SIZE*1, M_SIZE*154, M_SIZE*240, M_SIZE*14, TFT_BLACK);
    _tft->setTextColor(TFT_WHITE, TFT_BLACK);
    _tft->drawString(line, M_SIZE*(1), M_SIZE*(154), 1);
}
void MyTFTeSPI::plotFooterLine3(const char *line)
{
    _tft->fillRect(M_SIZE*1, M_SIZE*172, M_SIZE*240, M_SIZE*14, TFT_BLACK);
    _tft->setTextColor(TFT_WHITE, TFT_BLACK);
    _tft->drawString(line, M_SIZE*(1), M_SIZE*(172), 1);
}

void MyTFTeSPI::plotTotal(float total)
{
    if (_updateTime <= millis()) {
        _updateTime = millis() + 35; // Update meter every 35 milliseconds

        plotNeedle((int)(total / 1000), 0); // g to kg
  }
}

void MyTFTeSPI::printStreamTitle(const char *streamTitle)
{
    Serial.print("streamtitle: "); Serial.println(streamTitle);
    plotFooterLine1(streamTitle);
}

/* NOTE When uploading a new sketch, platform io is Hard resetting via RTS pin...
    When that happens, the connection to the SD card remains hanging
    And the only way to fix it is to take out and reinsert the SD card.
    We can try beginning and ending the session every time we need it,
    though that can be too slow. */
void MyTFTeSPI::beginSD()
{
    while (!SD.begin(_ssPin, _tft->getSPIinstance())) {
        // Under the analog meter
        plotFooterLine1("SD begin failed! Try reinserting it");
        delay(10);
    }

    // _tft->fillRect(M_SIZE*1, M_SIZE*136, M_SIZE*240, M_SIZE*55, TFT_WHITE);
    // _tft->fillRect(M_SIZE*1, M_SIZE*136, M_SIZE*240, M_SIZE*18, TFT_BLACK);
    plotFooter();
}

//NOTE It doesn't work
void MyTFTeSPI::endSD()
{
    SD.end();
    // (_tft->getSPIinstance()).end();
}

void MyTFTeSPI::setCountdown(uint8_t countdown)
{
    _countdownValue = countdown;
}

void MyTFTeSPI::setAction(const char *action)
{
    strcpy(_actionBuffer, action);
}

void MyTFTeSPI::setCycleStats(const char *cycleStats)
{
    strcpy(_cycleStatsBuffer, cycleStats);
}

void MyTFTeSPI::setScore(uint32_t score)
{
    _scoreValue = score;
}

void MyTFTeSPI::setValueColorNeutral()
{
    _valueColor = TFT_BLACK;
}
void MyTFTeSPI::setValueColorWithinBounds()
{
    _valueColor = MYCOLOR_GREEN;
}
void MyTFTeSPI::setValueColorUnderBound()
{
    _valueColor = MYCOLOR_YELLOW;
}
void MyTFTeSPI::setValueColorOverBound()
{
    _valueColor = MYCOLOR_BLUE;
}

void MyTFTeSPI::loop()
{
    // Draw upper left & right values
    plotCycleStats();
    plotScore();

    // Draw lower left & right values
    plotAction();
    plotCountdown();
}

