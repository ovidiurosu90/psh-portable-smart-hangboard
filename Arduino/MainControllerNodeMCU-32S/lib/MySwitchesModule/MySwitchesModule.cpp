#include <MySwitchesModule.h>

MySwitchesModule::MySwitchesModule(
    const uint8_t sw1Pin, const uint8_t sw2Pin, const uint8_t sw3Pin,
    const uint8_t sw4Pin, const uint8_t sw5Pin, const uint8_t sw6Pin
) {
    _sw1Pin = sw1Pin;
    _sw2Pin = sw2Pin;
    _sw3Pin = sw3Pin;
    _sw4Pin = sw4Pin;
    _sw5Pin = sw5Pin;
    _sw6Pin = sw6Pin;

    pinMode(_sw1Pin, INPUT_PULLUP);
    pinMode(_sw2Pin, INPUT_PULLUP);
    pinMode(_sw3Pin, INPUT_PULLUP);

    //NOTE The first 3 switches go through GPIO pins
    // that are connected to internal pullup resistors
    // while the last 3 switches go through GPIO pins
    // that are connected to external 10k pullup resistors
    pinMode(_sw4Pin, INPUT);
    pinMode(_sw5Pin, INPUT);
    pinMode(_sw6Pin, INPUT);
}

void MySwitchesModule::setSw1PressedCallback(void (*callback)())
{
    _sw1PressedCallback = callback;
}
void MySwitchesModule::setSw2PressedCallback(void (*callback)())
{
    _sw2PressedCallback = callback;
}
void MySwitchesModule::setSw3PressedCallback(void (*callback)())
{
    _sw3PressedCallback = callback;
}
void MySwitchesModule::setSw4ToggledCallback(void (*callback)())
{
    _sw4ToggledCallback = callback;
}
void MySwitchesModule::setSw5ToggledCallback(void (*callback)())
{
    _sw5ToggledCallback = callback;
}
void MySwitchesModule::setSw6ToggledCallback(void (*callback)())
{
    _sw6ToggledCallback = callback;
}

bool MySwitchesModule::sw4IsToggledOn()
{
    return _sw4Value == 0;
}
bool MySwitchesModule::sw5IsToggledOn()
{
    return _sw5Value == 0;
}
bool MySwitchesModule::sw6IsToggledOn()
{
    return _sw6Value == 0;
}

void MySwitchesModule::loop()
{
    int sw1Value = digitalRead(_sw1Pin);
    if (sw1Value != _sw1Value) {
        _sw1Value = sw1Value;
        Serial.print("Switch 1 value: "); Serial.println(_sw1Value);

        if (_sw1Value == 0 && _sw1PressedCallback != NULL) {
            _sw1PressedCallback();
        }
    }
    int sw2Value = digitalRead(_sw2Pin);
    if (sw2Value != _sw2Value) {
        _sw2Value = sw2Value;
        Serial.print("Switch 2 value: "); Serial.println(_sw2Value);

        if (_sw2Value == 0 && _sw2PressedCallback != NULL) {
            _sw2PressedCallback();
        }
    }
    int sw3Value = digitalRead(_sw3Pin);
    if (sw3Value != _sw3Value) {
        _sw3Value = sw3Value;
        Serial.print("Switch 3 value: "); Serial.println(_sw3Value);

        if (_sw3Value == 0 && _sw3PressedCallback != NULL) {
            _sw3PressedCallback();
        }
    }
    int sw4Value = digitalRead(_sw4Pin);
    if (sw4Value != _sw4Value) {
        _sw4Value = sw4Value;
        Serial.print("Switch 4 value: "); Serial.println(_sw4Value);

        if (_sw4ToggledCallback != NULL) {
            _sw4ToggledCallback();
        }
    }
    int sw5Value = digitalRead(_sw5Pin);
    if (sw5Value != _sw5Value) {
        _sw5Value = sw5Value;
        Serial.print("Switch 5 value: "); Serial.println(_sw5Value);

        if (_sw5ToggledCallback != NULL) {
            _sw5ToggledCallback();
        }
    }
    int sw6Value = digitalRead(_sw6Pin);
    if (sw6Value != _sw6Value) {
        _sw6Value = sw6Value;
        Serial.print("Switch 6 value: "); Serial.println(_sw6Value);

        if (_sw6ToggledCallback != NULL) {
            _sw6ToggledCallback();
        }
    }
}

