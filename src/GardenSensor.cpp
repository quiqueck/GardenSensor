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
#define LUX_SAMPLES 3
#define SOIL_POWER_PIN D3
#define SENSOR_POWER_PIN D4
#define VOLTAGE_POWER_PIN D5
#define SOIL_DATA_PIN A0

#define LUX_DEBUG 0
#define MULTIPLEX_DEBUG 0
#define TEST_MODE 0
#define SEND_RESULTS_TEST 1

//10 min
#define SLEEP_DURATION 10 * 60e6
#if TEST_MODE == 0
#define SEND_RESULTS 1
#define SOIL_PAUSE_COUNT 1*6
#else
#define SEND_RESULTS SEND_RESULTS_TEST
#define SOIL_PAUSE_COUNT 0
#define LOOP_PAUSE 1000 * 60
#endif
//#define SOIL_PAUSE_COUNT 0


//uint8_t
#define EEPROM_SOIL_COUNTER 0x00

//uint16_t
#define EEPROM_LAST_SOIL 0x01

BH1750 lightMeter(0x23);
SI7021 sensor;
StaticJsonDocument<2048> doc;

#define MUX_OFF -1
void multiplex(int pin=MUX_OFF){
    #if MULTIPLEX_DEBUG==1
    Serial.printf("Multiplexing Pin %d\n", pin);
    #endif

    if (pin != SENSOR_POWER_PIN){
        #if MULTIPLEX_DEBUG==1
        Serial.println("Env OFF");
        #endif
        digitalWrite(SENSOR_POWER_PIN, LOW);
    }
    if (pin != VOLTAGE_POWER_PIN) {
        #if MULTIPLEX_DEBUG==1
        Serial.println("Voltage OFF");
        #endif
        digitalWrite(VOLTAGE_POWER_PIN, LOW);
    }
    if (pin != SOIL_POWER_PIN) {
        #if MULTIPLEX_DEBUG==1
        Serial.println("Soil OFF");
        #endif
        digitalWrite(SOIL_POWER_PIN, LOW);
    }

    delay(10);

    if (pin != MUX_OFF) {
        #if MULTIPLEX_DEBUG==1
        Serial.printf("Pin %d ON\n", pin);
        #endif
        
        digitalWrite(pin, HIGH);
        delay(100);
    }
}

void setup() {
    Serial.begin(115200);
    #if TEST_MODE == 1
    delay(100);
    #endif
    // Wait for serial to initialize.
    //while(!Serial) { }
    Serial.println("Staring up...");

    pinMode(SOIL_POWER_PIN, OUTPUT); 
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    pinMode(VOLTAGE_POWER_PIN, OUTPUT);
    
    digitalWrite(SOIL_POWER_PIN, LOW);
    digitalWrite(VOLTAGE_POWER_PIN, LOW);
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
    
    
    multiplex();

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
    multiplex(SOIL_POWER_PIN);   
    Serial.println("Moisture On");

    int val = sampleAnalog();//Read the SIG value form sensor 
    
    multiplex();    
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
    multiplex(VOLTAGE_POWER_PIN); 
    Serial.println("Voltage On");

    voltage = sampleAnalog();
    pvoltage = map(voltage, 13, 240, 0, 255)/255.0f;
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.print(" ");
    Serial.print(pvoltage*100);
    Serial.println("%");
    
    multiplex();    
    Serial.println("Voltage Off");
}

void readEnv(float& temperature, int& humidity){
    multiplex(SENSOR_POWER_PIN); 
    Serial.println("Sensor On");

    temperature = sensor.getCelsiusHundredths()/100.0f;
    humidity = sensor.getHumidityPercent();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    
    multiplex();    
    Serial.println("Env Off");
}

float readLux(){
    float lux = 0;
    for (int i=0; i<LUX_SAMPLES; i++) {
        lux += lightMeter.readLightLevel();
        delay(10);
    }
    return lux / LUX_SAMPLES;
}

float readBrightness(SoilInfo& sInfo){
    float lux;
#if LUX_DEBUG == 1
    //for (;;)
#endif
    {       
        delay(10);
        lux = readLux();         
        
        if (lux > 40000.0 ) {
            if (sInfo.lastMTreg != 32){
                sInfo.lastMTreg = 32;
                if (lightMeter.setMTreg(32)) {   
                    delay(50);     
                    lux = readLux();  
                    #if LUX_DEBUG == 1        
                    Serial.println(F("Setting MTReg to low value for high light environment"));
                    #endif
                } else {
                    #if LUX_DEBUG == 1
                    Serial.println(F("Error setting MTReg to low value for high light environment"));
                    #endif
                }
            }
        } else {
            if (lux > 10.0) {
                if (sInfo.lastMTreg != 69){
                    sInfo.lastMTreg = 69;
                    // typical light environment
                    if (lightMeter.setMTreg(69)) {
                        delay(50);     
                        lux = readLux();   
                        #if LUX_DEBUG == 1
                        Serial.println(F("Setting MTReg to default value for normal light environment"));
                        #endif
                    } else {
                        #if LUX_DEBUG == 1
                        Serial.println(F("Error setting MTReg to default value for normal light environment"));
                        #endif
                    }
                }
            } else {
                if (lux <= 10.0) {
                    if (sInfo.lastMTreg != 138){
                        sInfo.lastMTreg = 138;
                        //very low light environment
                        if (lightMeter.setMTreg(138)) {
                            delay(50);     
                            lux = readLux();  
                            
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
                doc["data"]["brightness"]["value"] = lux;   
                doc["data"]["brightness"]["mt"] = sInfo.lastMTreg;   
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
    

#if TEST_MODE == 1
    Serial.printf("Pausing %f\n", LOOP_PAUSE/1000.0f);
    delay(LOOP_PAUSE);
    Serial.println("---------------------");
    return;
#endif
    Serial.printf("Going to Sleep %f\n", SLEEP_DURATION);
    ESP.deepSleep(SLEEP_DURATION); 
}