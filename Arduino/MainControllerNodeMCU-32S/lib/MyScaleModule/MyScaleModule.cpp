#include <MyScaleModule.h>

MyScaleModule::MyScaleModule(const int DAT1pin, const int CLK1pin, const char* label1,
                             const int DAT2pin, const int CLK2pin, const char* label2
) {
    Serial.println("===== Running MyScaleModule =====");

    _myHX711Left = new MyHX711(DAT1pin, CLK1pin, label1);
    // _myHX711Left->clearStoredCalibrationData();

    _myHX711Right = new MyHX711(DAT2pin, CLK2pin, label2);
    // _myHX711Right->clearStoredCalibrationData();
}

void MyScaleModule::init(MyTFTeSPI* myTFTeSPI, MyESP32AudioI2S* myESP32AudioI2S,
    MyWiFi* myWiFi
) {
    _myTFTeSPI = myTFTeSPI;
    _myESP32AudioI2S = myESP32AudioI2S;
    _myWiFi = myWiFi;

    getCalibrationConfigFromWiFi();

    if (!_myHX711Left->hasStoredCalibrationData()) {
        _myTFTeSPI->plotFooterLine1("No stored calibration 1");
    } else {
        _myHX711Left->printStoredCalibrationData();
        _myHX711Left->useCalibrationData();
    }

    if (!_myHX711Right->hasStoredCalibrationData()) {
        _myTFTeSPI->plotFooterLine2("No stored calibration 2");
    } else {
        _myHX711Right->printStoredCalibrationData();
        _myHX711Right->useCalibrationData();
    }
}

void MyScaleModule::getCalibrationConfigFromWiFi()
{
    char* configBuffer = _myWiFi->getConfig(true);
    Serial.println("===== calibrationConfigBuffer: "); Serial.println(configBuffer);

    char* configItem;
    bool isKey = true;
    char configKey[65];
    char configValue[257];

    configItem = strtok(configBuffer, "=\n");
    while (configItem != NULL) {
        // Serial.print("configItem: "); Serial.println(configItem);
        if (isKey) {
            strcpy(configKey, configItem);
        } else {
            strcpy(configValue, configItem);

            if (strcmp(configKey, "knownWeightGrams") == 0) {
                _knownWeightGrams = atoi(configValue);
                Serial.print("Setting knownWeightGrams = ");
                Serial.println(_knownWeightGrams);
            } else {
                Serial.print("Unexpected configKey: "); Serial.println(configKey);
            }
        }
        isKey = !isKey;

        configItem = strtok(NULL, "=\n");
    }

    delete configBuffer;
}

void  MyScaleModule::calibrateHeadless()
{
    if (!_knownWeightGrams) {
        Serial.println("CalibrateHeadless can't start as _knownWeightGrams is missing.");
        return;
    }

    _myHX711Left->calibrateHeadless(_knownWeightGrams/2, _myTFTeSPI, _myESP32AudioI2S);
    _myHX711Right->calibrateHeadless(_knownWeightGrams/2, _myTFTeSPI, _myESP32AudioI2S);

    _myTFTeSPI->plotFooter();
}

uint32_t MyScaleModule::getKnownWeightGrams()
{
    return _knownWeightGrams;
}

float MyScaleModule::getTotalMeasurement()
{
    // _myHX711Left->printMeasurement();
    // _myHX711Right->printMeasurement();
    float measurementLeft = _myHX711Left->getMeasurement();
    float measurementRight = _myHX711Right->getMeasurement();
    return measurementLeft + measurementRight;
}

