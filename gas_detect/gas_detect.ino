#include "Free_Fonts.h"   // Include the header file attached to this sketch
#include <TFT_eSPI.h>     // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFun_SGP30_Arduino_Library.h" 

SGP30 mySensor;

#define NO2 27
#define NH3 26
#define CO  25

#define I2C_SGP30  0x58
/*TFT_WHITE TFT_BLACK TFT_YELLOW TFT_RED TFT_GREEN TFT_BLUE*/

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

const char* ssid = "golaks";
const char* pass = "";
const char* mqtt_server = "www.syshuman.com/broker";

WiFiClient espClient;

PubSubClient client(espClient);

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  float absoluteHumidity = 213.7f * (humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature);
  uint32_t scaledHumidity = (uint32_t) (1000.0f * absoluteHumidity); 
  return scaledHumidity;
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
 //     digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
 //     digitalWrite(ledPin, LOW);
    }
  }
}


void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  uint8_t i = 0;
  while(WiFi.status() != WL_CONNECTED) {   
    delay(200);
    if( (++i % 16) == 0) {
      tft.setCursor(0, 1, 2); // font 4
      tft.println("Trying to connect..."); 
    }
  }
  //tft.println(WiFi.localIP());
}

void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);  
  tft.setCursor(1, 1, 4); // font 4
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Wire.begin();
  Wire.setClock(400000);
  if (mySensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1);
  }
  mySensor.initAirQuality();
  //connectToWiFi();
  analogReadResolution(14);

  pinMode(NO2,INPUT);
  pinMode(NH3,INPUT);
  pinMode(CO,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //tft.println("www.syshuman.com");

  mySensor.setHumidity(8200);
 
  mySensor.measureAirQuality();
  tft.setCursor(0, 20, 4); // font 4
  tft.print("CO2 : ");
  tft.printf("%4d ", mySensor.CO2);
  tft.println();
  
  tft.setCursor(0, 60, 2); // font 4
  tft.print("TVOC : ");
  tft.printf("%4d", mySensor.TVOC);
  mySensor.measureRawSignals();
  tft.println();
  tft.print("H2   : ");
  tft.printf("%4d", mySensor.H2);
  
  tft.println();
  tft.print("Eth  : ");
  tft.printf("%4d", mySensor.ethanol);

  tft.println();
  int no2 = analogRead(NO2);
  int nh3 = analogRead(NH3);
  int co = analogRead(CO);
  tft.print("NO2 : "); tft.print(no2);
  tft.print(" NH3 : "); tft.print(nh3);
  tft.print(" CO  : "); tft.print(co);
  
 
  //client.publish("esp32/temperature", "1");
  //client.publish("esp32/humidity", "0");
 
  yield(); // We must yield() to stop a watchdog timeout.
  delay(1000);
  
}
