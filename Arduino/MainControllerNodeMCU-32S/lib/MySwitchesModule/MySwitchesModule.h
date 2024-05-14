#ifndef MySwitchesModule_h
#define MySwitchesModule_h

#include <Arduino.h> // For Serial

class MySwitchesModule
{
public:
    MySwitchesModule(const uint8_t sw1Pin, const uint8_t sw2Pin, const uint8_t sw3Pin,
        const uint8_t sw4Pin, const uint8_t sw5Pin, const uint8_t sw6Pin);

    void setSw1PressedCallback(void (*callback)());
    void setSw2PressedCallback(void (*callback)());
    void setSw3PressedCallback(void (*callback)());
    void setSw4ToggledCallback(void (*callback)());
    void setSw5ToggledCallback(void (*callback)());
    void setSw6ToggledCallback(void (*callback)());

    bool sw4IsToggledOn();
    bool sw5IsToggledOn();
    bool sw6IsToggledOn();

    void loop();


protected:

private:
    uint8_t _sw1Pin;
    uint8_t _sw2Pin;
    uint8_t _sw3Pin;
    uint8_t _sw4Pin;
    uint8_t _sw5Pin;
    uint8_t _sw6Pin;

    int _sw1Value = 1; // 0 if pressed
    int _sw2Value = 1; // 0 if pressed
    int _sw3Value = 1; // 0 if pressed
    int _sw4Value = 0; // 0 if ON
    int _sw5Value = 0; // 0 if ON
    int _sw6Value = 0; // 0 if ON

    void (*_sw1PressedCallback)() = NULL;
    void (*_sw2PressedCallback)() = NULL;
    void (*_sw3PressedCallback)() = NULL;
    void (*_sw4ToggledCallback)() = NULL;
    void (*_sw5ToggledCallback)() = NULL;
    void (*_sw6ToggledCallback)() = NULL;
};

#endif

