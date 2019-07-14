#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <SI7021.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h> 
#include "WiFi.h"

#define ANALOG_SAMPLES 10
#define SOIL_POWER_PIN D3
#define SENSOR_POWER_PIN D4
#define VOLTAGE_POWER_PIN D5
#define SOIL_DATA_PIN A0

#define LUX_DEBUG 1
#define TEST_MODE 1
#define SEND_RESULTS 0

//10 min
#define SLEEP_DURATION 10 * 60e6
#define SOIL_PAUSE_COUNT 1*6
//#define SOIL_PAUSE_COUNT 0


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
    digitalWrite(VOLTAGE_POWER_PIN, LOW);

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

int sampleAnalog(){
    int value = 0;
    for (int i=0; i<ANALOG_SAMPLES; i++) {
      value += analogRead(SOIL_DATA_PIN);
      delay(2);
    }
    return value/ANALOG_SAMPLES;
}

int readSoil(){
    digitalWrite(SENSOR_POWER_PIN, LOW);
    digitalWrite(VOLTAGE_POWER_PIN, LOW);
    delay(100);
    digitalWrite(SOIL_POWER_PIN, HIGH);//turn D7 "On"
    Serial.println("Moisture On");
    //delay(5000);
    delay(100);//wait 10 milliseconds 
    int val = sampleAnalog();//Read the SIG value form sensor 
    digitalWrite(SOIL_POWER_PIN, LOW);//turn D7 "Off"
    //delay(10);
    //digitalWrite(VOLTAGE_POWER_PIN, HIGH);   
    delay(100); 
    Serial.println("Moisture Off");
    
    return val;//send current moisture value
}

struct SoilInfo{    
    uint8_t counter;    
    uint16_t lastValue;
    uint8_t lastMTreg;
};

int readMoisture(SoilInfo& sInfo, bool& moistureCached, bool force=false){
    int moisture;
    if (force || sInfo.counter>=SOIL_PAUSE_COUNT){
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
    
    Serial.print("Moisture: ");
    Serial.print(moisture);    
    Serial.println("");

    return moisture;
}

void readVoltage(int& voltage, float& pvoltage){
    digitalWrite(SENSOR_POWER_PIN, LOW);
    digitalWrite(SOIL_POWER_PIN, LOW);
    delay(100);
    digitalWrite(VOLTAGE_POWER_PIN, HIGH);
    delay(100);
    voltage = sampleAnalog();
    pvoltage = map(voltage, 13, 240, 0, 255)/255.0f;
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" ");
    Serial.print(pvoltage*100);
    Serial.println("%");
    digitalWrite(VOLTAGE_POWER_PIN, LOW);
    delay(100);
}

void readEnv(float& temperature, int& humidity){
    digitalWrite(SENSOR_POWER_PIN, HIGH);
    delay(100);    
    temperature = sensor.getCelsiusHundredths()/100.0f;
    humidity = sensor.getHumidityPercent();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    digitalWrite(SENSOR_POWER_PIN, LOW);
    delay(100);
}

float readBrightness(SoilInfo& sInfo){
    float lux;
#if LUX_DEBUG == 1
    for (;;)
#endif
    {        
        delay(100);
        lux = lightMeter.readLightLevel();
        delay(50);
        
        if (lux > 40000.0) {
            sInfo.lastMTreg = 32;
            if (lightMeter.setMTreg(32)) {   
                delay(50);     
                lux = lightMeter.readLightLevel();
                #if LUX_DEBUG == 1        
                Serial.println(F("Setting MTReg to low value for high light environment"));
                #endif
            } else {
                #if LUX_DEBUG == 1
                Serial.println(F("Error setting MTReg to low value for high light environment"));
                #endif
            }
        } else {
            if (lux > 10.0) {
                sInfo.lastMTreg = 69;
                // typical light environment
                if (lightMeter.setMTreg(69)) {
                    delay(50);     
                    lux = lightMeter.readLightLevel(); 
                    #if LUX_DEBUG == 1
                    Serial.println(F("Setting MTReg to default value for normal light environment"));
                    #endif
                } else {
                    #if LUX_DEBUG == 1
                    Serial.println(F("Error setting MTReg to default value for normal light environment"));
                    #endif
                }
            } else {
            if (lux <= 10.0) {
                sInfo.lastMTreg = 138;
                //very low light environment
                if (lightMeter.setMTreg(138)) {
                    delay(50);
                    lux = lightMeter.readLightLevel();
                    
                    #if LUX_DEBUG == 1
                    Serial.println(F("Setting MTReg to high value for low light environment"));
                    #endif
                } else {
                    #if LUX_DEBUG == 1
                    Serial.println(F("Error setting MTReg to high value for low light environment"));
                    #endif
                }
            }
        }
        }

        Serial.print("Light: ");
        Serial.print(lux);
        Serial.println(" lx");
        delay(10);
    }
    return lux;
}

void readSoilInfo(SoilInfo& sInfo){
    if (!SPIFFS.exists("/soil.dat")){
        sInfo.counter = 0xff;
        sInfo.lastValue = 0;  
        sInfo.lastMTreg = 138;      
    } else {
        File persistent = SPIFFS.open("/soil.dat", "r");
        persistent.readBytes((char*)&sInfo, sizeof(sInfo));
        Serial.printf("Read %d: ct=%d, val=%d, mt=%d\n", sizeof(sInfo), sInfo.counter, sInfo.lastValue, sInfo.lastMTreg);
        persistent.close();
    } 

    lightMeter.setMTreg(sInfo.lastMTreg);
}

void writeSoilInfo(SoilInfo& sInfo){
    Serial.printf("Writing %d: ct=%d, val=%d, mt=%d\n", sizeof(sInfo), sInfo.counter, sInfo.lastValue, sInfo.lastMTreg);
    File persistent = SPIFFS.open("/soil.dat", "w");
    persistent.write((char*)&sInfo, sizeof(sInfo));
    persistent.close();
}

void loop() {
    Serial.println("Start Reading Data");  
    SoilInfo sInfo;
    readSoilInfo(sInfo);   

    //Temperature & Humidity 
    float temperature;
    int humidity;
    readEnv(temperature, humidity);  

    //Soil Moisture
    bool moistureCached = false;
    int moisture = readMoisture(sInfo, moistureCached, false);

    //Supply Voltage
    int voltage;
    float pvoltage;
    readVoltage(voltage, pvoltage);    

    //Ambient Brightness
    float lux = readBrightness(sInfo);

    writeSoilInfo(sInfo);

#if TEST_MODE == 1
    delay(1000);
    Serial.println("---------------------");
    return;
#endif

#if SEND_RESULTS == 1
    bool didSend = false;
    int retry = 5;
    while (!didSend && retry>0) {

        //WIFI connection
        WiFi.begin(WiFiName, WiFiPass);

        Serial.print("Connecting");
        bool failed = false;
        int tries = 30;
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

                if (doc["force"]["moisture"]) {
                    moisture = readMoisture(sInfo, moistureCached, true);
                    writeSoilInfo(sInfo);
                }

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
                didSend = true; //does this fix the issue of multiple sends at night???

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
        Serial.println("Disconnected");
        retry--;
    }
#endif

    SPIFFS.end();
    digitalWrite(VOLTAGE_POWER_PIN, LOW);
    //ESP.deepSleep(1e6); //1s
    Serial.printf("Going to Sleep %f\n", SLEEP_DURATION);
    ESP.deepSleep(SLEEP_DURATION); 
}