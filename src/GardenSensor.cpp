#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <SI7021.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h> 
#include "WiFi.h"

#define SOIL_POWER_PIN D3
#define SENSOR_POWER_PIN D4
#define VOLTAGE_POWER_PIN D5
#define SOIL_DATA_PIN A0


//uint8_t
#define EEPROM_SOIL_COUNTER 0x00

//uint16_t
#define EEPROM_LAST_SOIL 0x01

BH1750 lightMeter(0x23);
SI7021 sensor;
StaticJsonDocument<2048> doc;
void setup() {
    Serial.begin(115200);
    delay(100);
    // Wait for serial to initialize.
    //while(!Serial) { }
    Serial.println("Staring up...");

    pinMode(SOIL_POWER_PIN, OUTPUT); 
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    pinMode(VOLTAGE_POWER_PIN, OUTPUT);
    
    digitalWrite(SOIL_POWER_PIN, LOW);
    digitalWrite(SENSOR_POWER_PIN, HIGH);

    Wire.begin();
    delay(50);

    // begin returns a boolean that can be used to detect setup problems.
    if (lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE)) {
        Serial.println(F("BH1750 Advanced begin"));
    } else {
        Serial.println(F("Error initialising BH1750"));
    }

    sensor.begin(SDA,SCL);

    pinMode(SOIL_DATA_PIN, INPUT);
    
    //adcAttachPin(SOIL_DATA_PIN); //ESP32-Only
    //analogReadResolution(12); //ESP32-Only
    //analogSetAttenuation(ADC_6db);
    
    
    digitalWrite(SENSOR_POWER_PIN, LOW);
    digitalWrite(VOLTAGE_POWER_PIN, HIGH);

    if (!SPIFFS.begin()){
        Serial.println("SPIFFS Initialization Error");
    } else  if (!SPIFFS.exists("/soil.dat")){
        Serial.println("Formating SPIFFS");
        SPIFFS.format();
    }

    // String str = "";
    // Serial.println("FILES:");
    // Dir dir = SPIFFS.openDir("/");
    // while (dir.next())
    // {
    //     str += dir.fileName();
    //     str += " / ";
    //     str += dir.fileSize();
    //     str += "\r\n";
    // }
    // Serial.print(str);
    // Serial.println("==>***************");
}

int readSoil(){
    digitalWrite(VOLTAGE_POWER_PIN, LOW);
    delay(100);
    digitalWrite(SOIL_POWER_PIN, HIGH);//turn D7 "On"
    Serial.println("Moisture On");
    //delay(5000);
    delay(100);//wait 10 milliseconds 
    int val = analogRead(SOIL_DATA_PIN);//Read the SIG value form sensor 
    digitalWrite(SOIL_POWER_PIN, LOW);//turn D7 "Off"
    delay(10);
    digitalWrite(VOLTAGE_POWER_PIN, HIGH);   
    delay(100); 
    Serial.println("Moisture Off");
    
    return val;//send current moisture value
}

struct SoilInfo{    
    uint8_t counter;    
    uint16_t lastValue;
    uint8_t reserved;
};

void loop() {
    delay(100);
    Serial.println("Tick");
    

    digitalWrite(SENSOR_POWER_PIN, HIGH);
    delay(200);    
    float temperature = sensor.getCelsiusHundredths()/100.0f;
    int humidity = sensor.getHumidityPercent();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    digitalWrite(SENSOR_POWER_PIN, LOW);
    delay(500);    

    SoilInfo sInfo;
    if (!SPIFFS.exists("/soil.dat")){
        sInfo.counter = 0xff;
        sInfo.lastValue = 0;        
    } else {
        File persistent = SPIFFS.open("/soil.dat", "r");
        persistent.readBytes((char*)&sInfo, sizeof(sInfo));
        Serial.printf("Read %d: ct=%d, val=%d\n", sizeof(sInfo), sInfo.counter, sInfo.lastValue);
        persistent.close();
    }
     
    int moisture;
    bool moistureCached;
    if (sInfo.counter>=12){
        Serial.println("[measured]");
        sInfo.counter = 0;
        moisture = readSoil();
        sInfo.lastValue = moisture;                
        moistureCached = false;
    } else {
       moistureCached = true;
       Serial.println("[cached]");
       sInfo.counter++;       
       moisture = sInfo.lastValue; 
    }
    
    {
        Serial.printf("Writing %d: ct=%d, val=%d\n", sizeof(sInfo), sInfo.counter, sInfo.lastValue);
        File persistent = SPIFFS.open("/soil.dat", "w");
        persistent.write((char*)&sInfo, sizeof(sInfo));
        persistent.close();
    }
    
    Serial.print("Moisture: ");
    Serial.print(moisture);    
    Serial.println("");

    int voltage = analogRead(SOIL_DATA_PIN);
    float pvoltage = map(voltage, 177, 225, 0, 255)/255.0f;
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" ");
    Serial.print(pvoltage*100);
    Serial.println("%");
    delay(500);

    float lux = lightMeter.readLightLevel(true);
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    digitalWrite(VOLTAGE_POWER_PIN, LOW);

    // delay(5000);
    // Serial.println("---------------------");
    // return;


    bool didSend = false;
    int retry = 5;
    while (!didSend && retry>0) {

        //WIFI connection
        WiFi.begin(WiFiName, WiFiPass);

        Serial.print("Connecting");
        bool failed = false;
        int tries = 20;
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            tries--;
            if (tries <= 0) {
                failed = true;
                break;
            }
        }
        if (!failed) {
            Serial.println();

            Serial.print("Connected, IP address: ");
            Serial.println(WiFi.localIP());


            HTTPClient http;  //Declare an object of class HTTPClient
            WiFiClient client;
            http.begin(client, host, 7056, "/api/001/data", false);  //Specify request destination
            int httpCode = http.GET();                             //Send the request
            failed = true;
            String payload = "";
            if (httpCode > 0) { //Check the returning code                        
                payload = http.getString();   //Get the request response payload
                Serial.println("Received:");
                Serial.println(payload);                     //Print the response payload   
                deserializeJson(doc, payload);  



                doc["data"]["temperature"] = temperature;   
                doc["data"]["humidity"] = humidity;   
                doc["data"]["voltage"] = voltage;   
                doc["data"]["brightness"] = lux;   
                doc["data"]["moisture"]["value"] = moisture;   
                doc["data"]["moisture"]["cached"] = moistureCached;   

                payload = "";
                serializeJson(doc, payload);
                
                failed = false;
            }     
            http.end();   //Close connection


            if (!failed) {
                http.begin(client, host, 7056, "/api/001/data", false);  //Specify request destination
                Serial.println("Returning:");
                Serial.println(payload);
                http.addHeader("Content-Type", "application/json");
                httpCode = http.PUT(payload);
                if (httpCode > 0) {
                    Serial.print("Success with: ");
                    Serial.println(httpCode);
                    didSend = true;
                } else {
                    Serial.print("Error with: ");
                    Serial.println(httpCode);
                }

                http.end();   //Close connection
            }
        }

        WiFi.disconnect(true);
        Serial.print("Disconnected");
        retry--;
    }

    SPIFFS.end();
    //ESP.deepSleep(1e6); //1s
    ESP.deepSleep(15 * 60e6); //15min
}