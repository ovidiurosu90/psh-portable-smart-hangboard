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
        delay(200);
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

void MyWiFi::schedulePostResults(uint32_t sessionUID, uint32_t scoreValue,
    uint32_t scoreHistory[]
) {
    _scheduledPostResults = true;
    _sessionUID = sessionUID;
    _scoreValue = scoreValue;
    _scoreHistory = scoreHistory;
    Serial.print("POST Results scheduled! sessionUID: "); Serial.print(_sessionUID);
    Serial.print(", scoreValue: "); Serial.println(_scoreValue);
}

bool MyWiFi::postResults()
{
    bool result = false;
    if (!_scheduledPostResults) { // Nothing to post
        return result;
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi is not connected, so we cannot send the config!");
        return result;
    }

    char endPoint[165];
    strcpy(endPoint, _apiUrl);
    if (endPoint[strlen(endPoint) - 1] != '/') {
        strcat(endPoint, "/");
    }
    strcat(endPoint, "psh_post_results.php");

    HTTPClient http;
    http.begin(endPoint);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("PSHTOKEN", _apiToken);

    String httpRequestData = "";
    char sessionUIDBuffer[11];
    sprintf(sessionUIDBuffer, "%d", _sessionUID);
    httpRequestData += String("sessionUID=");
    httpRequestData += String(sessionUIDBuffer);

    char scoreValueBuffer[11];
    sprintf(scoreValueBuffer, "%d", _scoreValue);
    httpRequestData += String("&scoreValue=");
    httpRequestData += String(scoreValueBuffer);


    httpRequestData += String("&scoreHistory=");
    char scoreHistoryValueBuffer[11];
    for (int i = 0; _scoreHistory[i] != '\0'; i++) {
        sprintf(scoreHistoryValueBuffer, "%d", _scoreHistory[i]);
        httpRequestData += String(scoreHistoryValueBuffer);
        httpRequestData += String(",");
    }
    httpRequestData.remove(httpRequestData.length() - 1); // removing the last comma

    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode <= 0) {
        Serial.print("Invalid HTTP response code: "); Serial.println(httpResponseCode);
        http.end();
        return result;
    }

    _scheduledPostResults = false;
    String payload = http.getString();
    if (httpResponseCode != 200) {
        Serial.print("HTTP non-ok response code: "); Serial.println(httpResponseCode);
        Serial.print("Payload: "); Serial.println(payload);
        http.end();
        return result;
    }

    Serial.println("Sending Results");
    Serial.print("- request data: "); Serial.println(httpRequestData);
    Serial.print("- response payload: "); Serial.println(payload.c_str());
    result = strcmp(payload.c_str(), "1") == 0;
    http.end();
    return result;
}

