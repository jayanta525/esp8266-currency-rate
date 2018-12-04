/*
  ESP8266 Currency Rate Display
  GitHub Page: github.com/jayanta525

  OLED Example, display on I2C 128x64 OLED.
  Check other examples for LCD and OTA Update support.

  This sketch uses currency converter API, adapt the URL for changes.

  Example: "convert?q=USD_INR" -- converts 1 USD into INR
  Example: "convert?q=INR_USD" -- converts 1 INR into USD

  Please note that this free API updates currency rates every 60 minutes.
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET D5

#define button1 10                        //Activates display, GPIO pin
#define button2 9                         //Go into OTA mode, GPIO pin

const char* ssid = "xxxxxxxxx";					  //Change to your wireless network SSID
const char* password = "xxxxxxxxx";				//Password, blank for open networks

HTTPClient http;								          //Create object for HTTPClient
Adafruit_SSD1306 display(OLED_RESET);     //Create object for OLED

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

bool otamode = false;                     //this will keep track of the ota mode
unsigned long previousMillis = 0;
long interval = 10000;                    //Display auto off delay interval

void setup() {
  Serial.begin(115200);
  pinMode(button1, INPUT_PULLUP);             //as no external pullup is being used
  pinMode(button2, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize display with address
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.display();
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    display.setTextSize(1);
    drawStr(30, 10, "Rebooting...");
    display.display();
    delay(5000);
    ESP.restart();
  }
  // ArduinoOTA.setHostname("myesp8266");
  // ArduinoOTA.setPassword("admin");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((digitalRead(button1) == 0) && (otamode == false))      //toggle display
    displaycurrency();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {           //display auto_off
    previousMillis = currentMillis;
    displayoff();
  }
  if (digitalRead(button2) == 0) {                            //display ota information
    ArduinoOTA.handle();
    displayota();
    otamode = true;
    long interval = 60000;
  }
}

void displaycurrency() {
  String exchange = getexchange();
  display.clearDisplay();
  display.setTextSize(3);
  drawStr(15, 20, (char*) exchange.c_str());
  display.display();
}

void displayota() {
  display.clearDisplay();
  display.setTextSize(1);
  drawStr(40, 5, "OTA MODE");
  drawStr(40, 25, "Local IP:");
  drawStr(20, 35, (char*) WiFi.localIP().toString().c_str());
  drawStr(10, 45, "Restart ArduinoIDE");
  drawStr(25, 55, "Reset NodeMCU");
  display.display();
}

void displayoff() {
  display.clearDisplay();
  display.display();
}

void drawStr(uint8_t x, uint8_t y, char* str) {
  display.setCursor(x, y);                                       //Set x,y coordinates
  display.println(str);
}

String getexchange() {
  String payload;
  http.begin("http://free.currencyconverterapi.com/api/v5/convert?q=USD_INR&compact=y");
  int httpCode = http.GET();
  if (httpCode > 0) {							                             //Check if HTTP get is empty
    payload = http.getString();
    payload = payload.substring(18, 23);		                   //substring to remove unnecessary characters
  }
  http.end();									                                //terminate HTTP
  return payload;
}
