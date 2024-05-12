#ifndef MyHX711_h
#define MyHX711_h

#include "HX711.h"
#include <Preferences.h>

#include "MyTFTeSPI.h"
#include "MyESP32AudioI2S.h"

class MyHX711
{
public:
    MyHX711(const int DATpin, const int CLKpin, const char* label);

    bool hasCalibrationData();
    bool hasStoredCalibrationData();
    void clearStoredCalibrationData();
    void storeCalibrationData(uint32_t offset, float scale);
    void getStoredCalibrationData();
    void printStoredCalibrationData();

    void useCalibrationData(bool runTare = true);
    void calibrate();
    void calibrateHeadless(uint32_t knownWeightGrams, MyTFTeSPI* myTFTeSPI,
        MyESP32AudioI2S* myESP32AudioI2S);
    void tare();
    float getMeasurement();
    void printMeasurement();

protected:

private:
    Preferences _prefsCalibration;
    const bool _PREFS_RW_MODE = false;
    const bool _PREFS_RO_MODE = true;
    const char* _prefsCalPrefix = "calPrefs";
    char _prefsCalName[15];

    HX711 _HX711;
    const char* _label;
    uint32_t _offset = 0;
    float _scale = 0.0;
    int _precisionGrams = 5;
};

#endif

