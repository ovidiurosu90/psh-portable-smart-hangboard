#include <MyTrainer.h>

MyTrainer::MyTrainer(MyTFTeSPI* myTFTeSPI, MyESP32AudioI2S* myESP32AudioI2S,
    MyWiFi* myWiFi
) {
    _myTFTeSPI = myTFTeSPI;
    _myESP32AudioI2S = myESP32AudioI2S;
    _myWiFi = myWiFi;

    getConfigFromWiFi();
}

void MyTrainer::addScaleTotalMeasurement(float totalMeasurement)
{
    _scaleTotalMeasurement = totalMeasurement;
    _scaleTotalMeasurementMillis = millis();

    _myTFTeSPI->plotTotal(_scaleTotalMeasurement);
}

void MyTrainer::getConfigFromWiFi()
{
    char* configBuffer = _myWiFi->getConfig();
    Serial.println("===== configBuffer: "); Serial.println(configBuffer);

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

            if (strcmp(configKey, "totalCycles") == 0) {
                _totalCycles = atoi(configValue);
                Serial.print("Setting totalCycles = "); Serial.println(_totalCycles);
            } else if (strcmp(configKey, "activityTotalSeconds") == 0) {
                _activityTotalMillis = atoi(configValue) * 1000;
                Serial.print("Setting activityTotalMillis = ");
                Serial.println(_activityTotalMillis);
            } else if (strcmp(configKey, "restTotalSeconds") == 0) {
                _restTotalMillis = atoi(configValue) * 1000;
                Serial.print("Setting restTotalMillis = ");
                Serial.println(_restTotalMillis);
            } else if (strcmp(configKey, "scaleLowerLimitGrams") == 0) {
                _scaleLowerLimitGrams = atoi(configValue);
                Serial.print("Setting scaleLowerLimit = ");
                Serial.println(_scaleLowerLimitGrams);
            } else if (strcmp(configKey, "scaleUpperLimitGrams") == 0) {
                _scaleUpperLimitGrams = atoi(configValue);
                Serial.print("Setting scaleUpperLimit = ");
                Serial.println(_scaleUpperLimitGrams);
            } else {
                Serial.print("Unexpected configKey: "); Serial.println(configKey);
            }
        }
        isKey = !isKey;

        configItem = strtok(NULL, "=\n");
    }

    delete configBuffer;
}

void MyTrainer::startTraining()
{
    if (_trainingInProgress) {
        Serial.println("Training already in progress!");
        return;
    }

    _myESP32AudioI2S->stopSound();
    _myESP32AudioI2S->playGetReady();
    _myTFTeSPI->setAction("start");
    _myTFTeSPI->setValueColorNeutral();
    _trainingInProgress = true;
    _trainingStartMillis = millis();
    _trainingEndMillis = -1;
    _currentCycle = 0;
    _currentCycleStartMillis = _trainingStartMillis;
    _myTFTeSPI->setCycleStats(getCycleStats());
    Serial.print("==== Training START | millis: "); Serial.println(_trainingStartMillis);
}

void MyTrainer::endTraining()
{
    if (!_trainingInProgress) {
        Serial.println("Training already ended!");
        return;
    }

    _trainingInProgress = false;
    _trainingEndMillis = millis();
    _currentCycle = -1;
    _currentCycleStartMillis = -1;
    Serial.print("==== Training END | millis: "); Serial.print(_trainingEndMillis);
    Serial.print(" | millis diff: ");
    Serial.println(_trainingEndMillis - _trainingStartMillis);
    _trainingStartMillis = -1;

    _myESP32AudioI2S->stopSound();
    _myESP32AudioI2S->playEndSound();
    _myTFTeSPI->setAction("end");
    _myTFTeSPI->setCycleStats(getCycleStats());
    _myTFTeSPI->setValueColorNeutral();
}

bool MyTrainer::currentCycleIsActivity()
{
    return (_currentCycle % 2 == 1);
}

char* MyTrainer::getCycleStats()
{
    char* cycleStatsBuffer = new char[6];
    if (_currentCycle == -1 || _currentCycle == 255) {
        strcpy(cycleStatsBuffer, "cycle");
        return cycleStatsBuffer;
    }

    char currentCycleBuffer[3];
    sprintf(currentCycleBuffer, "%d", _currentCycle + 1);
    char totalCyclesBuffer[3];
    sprintf(totalCyclesBuffer, "%d", _totalCycles);

    strcpy(cycleStatsBuffer, currentCycleBuffer);
    strcat(cycleStatsBuffer, "/");
    strcat(cycleStatsBuffer, totalCyclesBuffer);

    return cycleStatsBuffer;
}

void MyTrainer::loop()
{
    if (!_trainingInProgress) {
        return;
    }

    if (_currentCycle >= _totalCycles) {
        endTraining();
        return;
    }

    if (currentCycleIsActivity()) {
        _currentCycleTotalMillis = _activityTotalMillis;
    } else {
        _currentCycleTotalMillis = _restTotalMillis;
    }


    _currentCycleMillis = millis() - _currentCycleStartMillis;
    if (_currentCycleMillis < _currentCycleTotalMillis) {

        if (_currentCycleMillis >=
            _currentCycleTotalMillis - _myESP32AudioI2S->get321BeepSoundLengthMillis()
        ) {
            _myESP32AudioI2S->play321Beep();
            //NOTE This will also try to play at t-1s if there was an error before
        } else {
            if (!currentCycleIsActivity() && _currentCycle > 0) {
                _myESP32AudioI2S->playRestSound();
                _myTFTeSPI->setAction("rest");
            }
            if (currentCycleIsActivity()) {
                _myTFTeSPI->setAction("hang");
            }
        }

        if (currentCycleIsActivity()) {
            if (_scaleTotalMeasurement < _scaleLowerLimitGrams ||
                _scaleTotalMeasurement > _scaleUpperLimitGrams
            ) {
                if (millis() - _outOfBoundsLastPrintMillis >
                    _outOfBoundsPrintFrequencyMillis
                ) {
                    // Only print once a second
                    Serial.print("Scale measurement '");
                    Serial.print(_scaleTotalMeasurement);
                    Serial.print("' out of bounds (");
                    Serial.print(_scaleLowerLimitGrams);
                    Serial.print(", "); Serial.print(_scaleUpperLimitGrams);
                    Serial.println(")");
                    _outOfBoundsLastPrintMillis = millis();
                }

                // _myESP32AudioI2S->stopMeasurementWithinBounds();
                _myESP32AudioI2S->playMeasurementOutOfBounds();
                if (_scaleTotalMeasurement > _scaleUpperLimitGrams) {
                    _myTFTeSPI->setValueColorOverBound();
                } else if (_scaleTotalMeasurement < _scaleLowerLimitGrams) {
                    _myTFTeSPI->setValueColorUnderBound();
                }
            } else {
                // _myESP32AudioI2S->stopMeasurementOutOfBounds();
                _myESP32AudioI2S->playMeasurementWithinBounds();
                _myTFTeSPI->setValueColorWithinBounds();
            }
        }

        // Countdown in seconds
        _myTFTeSPI->setCountdown(ceil(
            (_currentCycleTotalMillis - _currentCycleMillis) / 1000));

        return;
    }

    Serial.print("= Finished cycle "); Serial.print(_currentCycle); Serial.print(" in ");
    Serial.print(_currentCycleMillis); Serial.println(" millis");

    _currentCycleStartMillis = millis();
    _currentCycle++;
    _myESP32AudioI2S->stopGetReady();
    _myESP32AudioI2S->stop321Beep();
    _myESP32AudioI2S->stopMeasurementOutOfBounds();
    _myESP32AudioI2S->stopMeasurementWithinBounds();
    _myESP32AudioI2S->stopRestSound();
    _myESP32AudioI2S->stopEndSound();
    _myTFTeSPI->setCycleStats(getCycleStats());
    _myTFTeSPI->setValueColorNeutral();
}

