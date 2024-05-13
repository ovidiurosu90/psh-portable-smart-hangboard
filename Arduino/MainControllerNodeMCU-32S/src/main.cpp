#include <MySwitchesModule.h>
#include <MyWiFi.h>
#include <MyTFTeSPI.h>
#include <SD.h>
#include <MyESP32AudioI2S.h>
#include <MyScaleModule.h>
#include <MyTrainer.h>
#include <MyTaskRunner.h>

MySwitchesModule* mySwitchesModule;
MyWiFi* myWiFi;
MyTFTeSPI* myTFTeSPI;
MyESP32AudioI2S* myESP32AudioI2S;
MyScaleModule* myScaleModule;
MyTrainer* myTrainer;
MyTaskRunner* myTaskRunner;
bool IS_RUNNING = true;

void sw1PressedCallback()
{
    myTrainer->startTraining();
}
void sw2PressedCallback()
{
    myScaleModule->calibrateHeadless();
}
void sw3PressedCallback()
{
    myTrainer->endTraining();
}

void taskFunction0(void* pvParameters)
{
    Serial.print("TaskFunction0 running on core "); Serial.println(xPortGetCoreID());

    for(;;) {
        if (Serial.available() > 0) {
            char inByte = Serial.read();
            if (inByte == '|') {
                Serial.println("Received '|' before upload! Ending training...");
                myTrainer->endTraining();
                // myTFTeSPI->endSD();
                // IS_RUNNING = false;
            }
        }
        delay(50);
    }
}

void taskFunction1(void* pvParameters)
{
    Serial.print("TaskFunction1 running on core "); Serial.println(xPortGetCoreID());

    for(;;) {
        if (IS_RUNNING) {
            myTrainer->addScaleTotalMeasurement(myScaleModule->getTotalMeasurement());
            myTFTeSPI->loop();
        }
        delay(500);
    }
}

void setup()
{
    Serial.begin(115200);

    mySwitchesModule = new MySwitchesModule(21, 27, 13, 36, 39, 35);

    myWiFi = new MyWiFi(WIFI_SSID, WIFI_PASSPHRASE);
    myWiFi->connect();
    myWiFi->setApiUrl(API_URL);
    myWiFi->setApiToken(API_TOKEN);

    //NOTE The pins are in lib/TFT_eSPI/User_Setup.h
    myTFTeSPI = new MyTFTeSPI();
    myTFTeSPI->initSD(33);

    myESP32AudioI2S = new MyESP32AudioI2S(26, 25, 22);
    myESP32AudioI2S->setVolume(5);

    //NOTE Not all ESP-32S pins can be used as outputs
    myScaleModule = new MyScaleModule(5, 4, "_L", 16, 15, "_R");
    myScaleModule->init(myTFTeSPI, myESP32AudioI2S, myWiFi);

    myTrainer = new MyTrainer(myTFTeSPI, myESP32AudioI2S, myWiFi);

    mySwitchesModule->setSw1PressedCallback(&sw1PressedCallback);
    mySwitchesModule->setSw2PressedCallback(&sw2PressedCallback);
    mySwitchesModule->setSw3PressedCallback(&sw3PressedCallback);

    myTaskRunner = new MyTaskRunner();
    myTaskRunner->addTaskCore0(&taskFunction0);
    myTaskRunner->addTaskCore1(&taskFunction1);
}

void loop()
{
    if (!IS_RUNNING) {
        return;
    }

    myTrainer->loop();
    myESP32AudioI2S->loop();
    mySwitchesModule->loop();
}

// Audio events (optional)
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    myTFTeSPI->printStreamTitle(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
