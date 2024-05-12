#include <MyTaskRunner.h>

MyTaskRunner::MyTaskRunner()
{
}

void MyTaskRunner::addTaskCore0(TaskFunction_t taskFunction)
{
    xTaskCreatePinnedToCore(
        taskFunction,/* Task function. */
        "TaskCore0", /* Name of task. */
        10000,       /* Stack size of task */
        NULL,        /* Parameter of the task */
        1,           /* Priority of the task */
        &_task1,     /* Task handle to keep track of created task */
        0);          /* Pin task to core 0 */

    delay(500);
}

void MyTaskRunner::addTaskCore1(TaskFunction_t taskFunction)
{
    xTaskCreatePinnedToCore(
        taskFunction,/* Task function. */
        "TaskCore1", /* Name of task. */
        10000,       /* Stack size of task */
        NULL,        /* Parameter of the task */
        2,           /* Priority of the task */
        &_task2,     /* Task handle to keep track of created task */
        1);          /* Pin task to core 0 */

    delay(500);
}

