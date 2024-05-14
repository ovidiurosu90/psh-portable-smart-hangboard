#include <MyHX711.h>

MyHX711::MyHX711(const int DATpin, const int CLKpin, const char* label)
{
    _HX711.begin(DATpin, CLKpin);
    _label = label;

    strcpy(_prefsCalName, _prefsCalPrefix);
    strcat(_prefsCalName, _label);
}

bool MyHX711::hasCalibrationData()
{
    if (!_offset || !_scale) {
        return false;
    }
    return true;
}

bool MyHX711::hasStoredCalibrationData()
{
    _prefsCalibration.begin(_prefsCalName, _PREFS_RO_MODE);
    if (!_prefsCalibration.isKey("offset")) {
        _prefsCalibration.end();
        return false;
    }
    if (!_prefsCalibration.isKey("scale")) {
        _prefsCalibration.end();
        return false;
    }
    _prefsCalibration.end();
    return true;

}

void MyHX711::clearStoredCalibrationData()
{
    _prefsCalibration.begin(_prefsCalName, _PREFS_RW_MODE);
    _prefsCalibration.clear();
    _prefsCalibration.end();
}

void MyHX711::storeCalibrationData(uint32_t offset, float scale)
{
    _prefsCalibration.begin(_prefsCalName, _PREFS_RW_MODE);
    _prefsCalibration.putULong("offset", offset);
    _prefsCalibration.putFloat("scale", scale);
    _prefsCalibration.end();

    Serial.print("The calibration data was stored in '"); Serial.print(_prefsCalName);
    Serial.println("'!");
}

void MyHX711::getStoredCalibrationData()
{
    if (!hasStoredCalibrationData()) {
        return;
    }

    _prefsCalibration.begin(_prefsCalName, _PREFS_RO_MODE);
    _offset = _prefsCalibration.getULong("offset");
    _scale = _prefsCalibration.getFloat("scale");
    _prefsCalibration.end();
}

void MyHX711::printStoredCalibrationData()
{
    if (!hasStoredCalibrationData()) {
        Serial.print("\nNo stored calibration data found in '");
        Serial.print(_prefsCalName); Serial.println("'!");
        return;
    }

    getStoredCalibrationData();

    Serial.print("\nCurrent stored calibration data in '"); Serial.print(_prefsCalName);
    Serial.println("':");
    Serial.print("offset: "); Serial.println(_offset);
    Serial.print("scale: "); Serial.println(_scale, 6);
    Serial.println();
}

void MyHX711::useCalibrationData(bool runTare)
{
    if (!hasCalibrationData()) {
        getStoredCalibrationData();
    }

    if (!hasCalibrationData()) {
        Serial.print("\n No calibration data found for '"); Serial.print(_label);
        Serial.println("', nothing to use!");
        return;
    }

    _HX711.set_offset(_offset);
    _HX711.set_scale(_scale);
    if (runTare) {
        tare(); // reset the scale to zero = 0
    }
}

void MyHX711::calibrate()
{
    printStoredCalibrationData();

    while (Serial.available()) Serial.read(); // flush Serial input
    Serial.print("Run the calibration for '"); Serial.print(_label);
    Serial.println("'? y/n");

    bool resume = false;
    while (resume == false) {
        if (Serial.available() > 0) {
            char inByte = Serial.read();
            if (inByte == 'y') {
                resume = true;
            } else if (inByte == 'n') {
                return;
            }
        }
    }


    Serial.print("\n===========\nCALIBRATION for '"); Serial.print(_label);
    Serial.println("'\n===========");
    Serial.println("Remove all weight from the loadcell");
    // flush Serial input
    while (Serial.available()) Serial.read();

    Serial.println("and press enter\n");
    while (Serial.available() == 0);

    Serial.println("Determine zero weight offset");
    tare();
    _offset = _HX711.get_offset();

    Serial.print("OFFSET: ");
    Serial.println(_offset);
    Serial.println();


    Serial.println("Place a weight on the loadcell");
    while (Serial.available()) Serial.read(); // flush Serial input

    Serial.println("Enter the weight in (whole) grams");
    Serial.println("and press enter");
    uint32_t weight = 0;
    while (Serial.peek() != '\n') {
        if (Serial.available()) {
            char ch = Serial.read();
            if (isdigit(ch)) {
                weight *= 10;
                weight = weight + (ch - '0');
            }
        }
    }
    Serial.print("WEIGHT: ");
    Serial.println(weight);
    _HX711.calibrate_scale(weight, 20);
    _scale = _HX711.get_scale();

    Serial.print("SCALE:    ");
    Serial.println(_scale, 6);

    Serial.println("\nUse the following in the setup of your project:");
    Serial.print("scale.set_offset("); Serial.print(_offset);
    Serial.print("); scale.set_scale("); Serial.print(_scale, 6);
    Serial.println(");");
    Serial.println("\n\n");

    while (Serial.available()) Serial.read(); // flush Serial input
    Serial.print("Store calibration data in the NVS of the ESP32 (name: '");
    Serial.print(_prefsCalName);
    Serial.println("')? y/n");

    resume = false;
    while (resume == false) {
        if (Serial.available() > 0) {
            char inByte = Serial.read();
            if (inByte == 'y') {
                storeCalibrationData(_offset, _scale);

                Serial.println();
                resume = true;
            } else if (inByte == 'n') {
                Serial.println("The calibration data was NOT stored!");

                Serial.println();
                resume = true;
            }
        }
    }
    useCalibrationData(false);
}

void MyHX711::calibrateHeadless(uint32_t knownWeightGrams, MyTFTeSPI* myTFTeSPI,
    MyESP32AudioI2S* myESP32AudioI2S, MySwitchesModule* mySwitchesModule
) {
    printStoredCalibrationData();

    Serial.print("\n===========\nCALIBRATION for '"); Serial.print(_label);
    Serial.println("'\n===========");
    char message[26];
    strcpy(message, "CALIB ");
    strcat(message, _label);
    myTFTeSPI->plotFooterLine1(message);

    Serial.println("Remove all weight from the loadcell");
    Serial.println("You have 3s\n");
    myTFTeSPI->plotFooterLine2("Remove weight, 3s...");
    delay(3000);

    Serial.println("Determine zero weight offset");
    myTFTeSPI->plotFooterLine2("Determine zero weight offset");
    tare();
    _offset = _HX711.get_offset();

    Serial.print("OFFSET: "); Serial.println(_offset);
    Serial.println("You have 2s\n");

    Serial.println();
    strcpy(message, "OFFSET ");
    char offsetValue[33];
    sprintf(offsetValue, "%u", _offset);
    strcat(message, offsetValue);
    myTFTeSPI->plotFooterLine2(message);
    delay(2000);

    Serial.print("Place the known "); Serial.print(knownWeightGrams);
    Serial.println("g weight on the loadcell");
    Serial.println("You have 5s\n");
    strcpy(message, "Place weight ");
    char knownWeightGramsValue[17];
    sprintf(knownWeightGramsValue, "%u", knownWeightGrams);
    strcat(message, knownWeightGramsValue);
    strcat(message, ", 5s...");
    myTFTeSPI->plotFooterLine2(message);
    delay(5000); // blocking
    // delay(5000 - myESP32AudioI2S->get321BeepSoundLengthMillis());
    // myESP32AudioI2S->play321Beep();
    // delay(myESP32AudioI2S->get321BeepSoundLengthMillis());

    _HX711.calibrate_scale(knownWeightGrams, 20);
    _scale = _HX711.get_scale();

    Serial.print("SCALE:    ");
    Serial.println(_scale, 6);
    char scaleValue[16];
    sprintf(scaleValue, "%.6f", _scale);
    strcpy(message, "SCALE ");
    strcat(message, scaleValue);
    myTFTeSPI->plotFooterLine2(message);

    Serial.println("\nUse the following in the setup of your project:");
    Serial.print("scale.set_offset("); Serial.print(_offset);
    Serial.print("); scale.set_scale("); Serial.print(_scale, 6);
    Serial.println(");");
    Serial.println("\n\n");
    // myESP32AudioI2S->playEndSound();

    delay(2000);
    if (mySwitchesModule->sw5IsToggledOn()) {
        Serial.print("Storing calibration data in the NVS of the ESP32 (name: '");
        Serial.print(_prefsCalName); Serial.println("') ...");
        storeCalibrationData(_offset, _scale);
    } else {
        Serial.print("Calibration data was not stored in the NVS of the ESP32 ");
        Serial.println("because SW5 is toggled off!");
    }
    useCalibrationData(false);
    myTFTeSPI->plotFooterLine2("CALIB stored in NVS");
    delay(2000);
}

void MyHX711::tare()
{
    // average 20 measurements
    _HX711.tare(20); // reset the scale to zero = 0
}

float MyHX711::getMeasurement()
{
    while (!_HX711.is_ready()) {
        continue;
    }
    float result = _HX711.get_units(1);
    if (result < _precisionGrams) {
        result = 0;
    }
    return result;
}

void MyHX711::printMeasurement()
{
    if (_HX711.is_ready()) {
        float result = _HX711.get_units(1);
        if (result < _precisionGrams) {
            result = 0;
        }
        Serial.print("Measurement for '"); Serial.print(_label); Serial.print("': ");
        Serial.print(result); Serial.println(" grams");
    }
}

