#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"
#include "DHT.h"
#define DHTPIN 4
int relay = 5;
int intValue;
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "189"
#define WIFI_PASSWORD "hello3214"
#define API_KEY "AIzaSyA9GyxBO7Go_e5iagskVWRkpttQMduw9cw"
#define USER_EMAIL "het@gmail.com"
#define USER_PASSWORD "hetislegend"
#define DATABASE_URL "https://adityaariot-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

String uid;
String databasePath;

String tempPath = "/temperature";
String humPath = "/humidity";
String timePath = "/timestamp";

String parentPath;
int timestamp;
FirebaseJson json;
const char *ntpServer = "pool.ntp.org";
float temperature;
float humidity;
float pressure;
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 3000;

void initWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
    Serial.println();
}

unsigned long getTime()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        // Serial.println("Failed to obtain time");
        return (0);
    }
    time(&now);
    return now;
}

void setup()
{
    Serial.begin(9600);
    pinMode(relay, OUTPUT);
    Serial.println(F("DHTxx test!"));
    initWiFi();
    configTime(0, 0, ntpServer);
    config.api_key = API_KEY;
    dht.begin();
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.database_url = DATABASE_URL;
    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
    config.max_token_generation_retry = 5;
    Firebase.begin(&config, &auth);
    Serial.println("Getting User UID");
    while ((auth.token.uid) == "")
    {
        Serial.print('.');
        delay(1000);
    }

    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);
    databasePath = "/UsersData/" + uid + "/readings";
}

void loop()
{
    delay(2000);
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();

        // Get current timestamp
        timestamp = getTime();
        Serial.print("time: ");
        Serial.println(timestamp);
        Serial.println(h);
        parentPath = databasePath + "reading";

        json.set(tempPath.c_str(), String(t));
        json.set(humPath.c_str(), String(h));
        json.set(timePath, String(timestamp));
        Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());

        if (Firebase.RTDB.getInt(&fbdo, "/UsersData/HHeFtnU2LTXWVTBsKqgjwHbJ1pz2/Relay/value"))
        {
            if (fbdo.dataType() == "int")
            {
                intValue = fbdo.intData();
                Serial.println(intValue);
                if (intValue == 1)
                {
                    digitalWrite(relay, HIGH);
                }
                else
                {
                    digitalWrite(relay, LOW);
                }
            }
        }
        else
        {
            Serial.println(fbdo.errorReason());
        }
    }
}