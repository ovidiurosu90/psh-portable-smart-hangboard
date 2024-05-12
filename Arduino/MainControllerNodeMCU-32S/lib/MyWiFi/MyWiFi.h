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


protected:

private:

    WiFiClass _WiFi;

    char _apiUrl[129];
    char _apiToken[257];
};

#endif

