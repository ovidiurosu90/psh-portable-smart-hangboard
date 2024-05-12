#include <MyWiFi.h>

MyWiFi::MyWiFi(const char *ssid, const char *passphrase)
{
    _WiFi.disconnect();
    _WiFi.mode(WIFI_STA); // Station Mode
    _WiFi.begin(ssid, passphrase);
}

void MyWiFi::connect()
{
    while (_WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // WiFi Connected, print IP to serial monitor
    Serial.println("\nWiFi connected");
    Serial.print("IP address: "); Serial.println(_WiFi.localIP());
}

void MyWiFi::setApiUrl(const char *apiUrl)
{
    strcpy(_apiUrl, apiUrl);
}

void MyWiFi::setApiToken(const char *apiToken)
{
    strcpy(_apiToken, apiToken);
}

char* MyWiFi::getConfig(bool calibration)
{
    char* configBuffer = new char[513];
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi is not connected, so we cannot get the config!");
        return configBuffer;
    }

    char endPoint[165];
    strcpy(endPoint, _apiUrl);
    if (endPoint[strlen(endPoint) - 1] != '/') {
        strcat(endPoint, "/");
    }
    strcat(endPoint, "psh_get_config.php");
    if (calibration) {
         strcat(endPoint, "?calibration=1");
    }

    HTTPClient http;
    http.begin(endPoint);
    http.addHeader("PSHTOKEN", _apiToken);

    int httpResponseCode = http.GET();
    if (httpResponseCode <= 0) {
        Serial.print("Invalid HTTP response code: "); Serial.println(httpResponseCode);
        http.end();
        return configBuffer;
    }

    String payload = http.getString();
    if (httpResponseCode != 200) {
        Serial.print("HTTP non-ok response code: "); Serial.println(httpResponseCode);
        Serial.print("Payload: "); Serial.println(payload);
        http.end();
        return configBuffer;
    }

    strcpy(configBuffer, payload.c_str());
    http.end();
    return configBuffer;
}

