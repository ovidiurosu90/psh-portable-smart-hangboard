#ifndef MyScaleModule_h
#define MyScaleModule_h

#include <MyHX711.h>

#include "MyTFTeSPI.h"
#include "MyESP32AudioI2S.h"
#include "MyWiFi.h"
#include "MySwitchesModule.h"

class MyScaleModule
{
public:
    MyScaleModule(const int DAT1pin, const int CLK1pin, const char* label1,
                  const int DAT2pin, const int CLK2pin, const char* label2);

    void init(MyTFTeSPI* myTFTeSPI, MyESP32AudioI2S* myESP32AudioI2S, MyWiFi* myWiFi);
    void getCalibrationConfigFromWiFi();

    void calibrateHeadless(MySwitchesModule* mySwitchesModule);
    uint32_t getKnownWeightGrams();

    float getTotalMeasurement();

protected:

private:

    MyHX711* _myHX711Left;
    MyHX711* _myHX711Right;

    MyTFTeSPI* _myTFTeSPI;
    MyESP32AudioI2S* _myESP32AudioI2S;
    MyWiFi* _myWiFi;

    uint32_t _knownWeightGrams = 0;
};

#endif

