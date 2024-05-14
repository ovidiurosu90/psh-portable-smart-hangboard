#ifndef MyWiFi_h
#define MyWiFi_h

#include "WiFi.h"
#include "HTTPClient.h"

class MyWiFi
{
public:
    MyWiFi(const char *ssid, const char *passphrase = NULL);

    void connect();

    void setApiUrl(const char *apiUrl);
    void setApiToken(const char *apiToken);
    char* getConfig(bool calibration = false);
    void schedulePostResults(uint32_t sessionUID, uint32_t scoreValue,
        uint32_t scoreHistory[]);
    bool postResults();


protected:

private:

    WiFiClass _WiFi;

    char _apiUrl[129];
    char _apiToken[257];

    bool _scheduledPostResults = false;
    uint32_t _sessionUID = -1;
    uint32_t _scoreValue = -1;
    uint32_t* _scoreHistory;
};

#endif

