#ifndef MyTaskRunner_h
#define MyTaskRunner_h

#include <Arduino.h> // For TaskHandle_t

class MyTaskRunner
{
public:
    MyTaskRunner();

    void addTaskCore0(TaskFunction_t taskFunction);
    void addTaskCore1(TaskFunction_t taskFunction);


protected:

private:
    TaskHandle_t _task1;
    TaskHandle_t _task2;
};

#endif

