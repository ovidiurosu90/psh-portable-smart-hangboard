#ifndef MyTrainer_h
#define MyTrainer_h

#include "MyTFTeSPI.h"
#include "MyESP32AudioI2S.h"
#include "MyWiFi.h"

class MyTrainer
{
public:
    MyTrainer(MyTFTeSPI* myTFTeSPI, MyESP32AudioI2S* myESP32AudioI2S, MyWiFi* myWiFi);

    void addScaleTotalMeasurement(float totalMeasurement);

    void getConfigFromWiFi();

    void startTraining();
    void endTraining();
    bool currentCycleIsActivity();
    char* getCycleStats();

    void loop();


protected:

private:
    MyTFTeSPI* _myTFTeSPI;
    MyESP32AudioI2S* _myESP32AudioI2S;
    MyWiFi* _myWiFi;

    float _scaleTotalMeasurement = 0.0;
    uint32_t _scaleTotalMeasurementMillis = -1;

    bool _trainingInProgress = false;
    uint32_t _trainingStartMillis = -1;
    uint32_t _trainingEndMillis = -1;

    uint8_t  _currentCycle = -1;
    uint32_t _currentCycleStartMillis = -1;
    uint32_t _currentCycleMillis = -1;
    uint32_t _currentCycleTotalMillis = -1; // will take either activity or rest total

    uint32_t _currentScore = -1;
    uint32_t _scoreValue = -1;
    uint32_t _scoreLastPrintMillis = -1;
    const uint32_t _scoreLastPrintFrequencyMillis = 1000;

    uint32_t _outOfBoundsLastPrintMillis = -1;
    const uint32_t _outOfBoundsPrintFrequencyMillis = 1000;

    //NOTE These are default values. They are overwritten from WiFi
    uint8_t  _totalCycles          = 4; // break, round 0, break, round 1
    uint32_t _activityTotalMillis  = 10000;
    uint32_t _restTotalMillis      = 20000;
    uint32_t _scaleLowerLimitGrams = 5000;
    uint32_t _scaleUpperLimitGrams = 7500;
};

#endif

