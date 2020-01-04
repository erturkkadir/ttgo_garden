#include "Free_Fonts.h"   // Include the header file attached to this sketch
#include <TFT_eSPI.h>     // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFun_SGP30_Arduino_Library.h" 

SGP30 mySensor;

#define I2C_SGP30  0x58
/*TFT_WHITE TFT_BLACK TFT_YELLOW TFT_RED TFT_GREEN TFT_BLUE*/

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

const char* ssid = "golaks";
const char* pass = "";
const char* mqtt_server = "www.syshuman.com/broker";

WiFiClient espClient;

PubSubClient client(espClient);

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
  connectToWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:
  //tft.println("www.syshuman.com");
  
  tft.setCursor(1, 20, 4); // font 4
  mySensor.measureAirQuality();
  tft.println();
  tft.print("CO2: ");
  tft.print(mySensor.CO2);
  tft.print("  TVOC :");
  tft.print(mySensor.TVOC);
  mySensor.measureRawSignals();
  tft.println();
  tft.print("H2:");
  tft.print(mySensor.H2);
  tft.print(" Eth:");
  tft.print(mySensor.ethanol);
 
  //client.publish("esp32/temperature", "1");
  //client.publish("esp32/humidity", "0");
 
  yield(); // We must yield() to stop a watchdog timeout.
  delay(1000);
  
}
