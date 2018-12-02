/*
  ESP8266 Currency Rate Display
  GitHub Page: github.com/jayanta525

  Basic Example, display on Serial Monitor. 
  Check other examples for OLED, LCD and OTA Update support.
  
  This sketch uses currency converter API, adapt the URL for changes.
  
  Example: "convert?q=USD_INR" -- converts 1 USD into INR
  Example: "convert?q=INR_USD" -- converts 1 INR into USD

  Please note that this free API updates currency rates every 60 minutes.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "xxxxxxxxx";					//Change to your wireless network SSID
const char* password = "xxxxxxxxx";				//Password, blank for open networks

HTTPClient http;								//Create object for HTTPClient

void setup() {
  Serial.begin(115200);							//Serial baud set to 115200
  WiFi.begin(ssid, password);					//Connect to wireless
  while (WiFi.status() != WL_CONNECTED) {		//Loop here until connection established
    delay(1000);
    Serial.print("Connecting..");
  }
}

void loop() {
  String exchange = getexchange();
  Serial.println(exchange);
  delay(30000);									//30 seconds delay
}

String getexchange() {
  String payload;
  http.begin("http://free.currencyconverterapi.com/api/v5/convert?q=USD_INR&compact=y");
  int httpCode = http.GET();
  if (httpCode > 0) {							//Check if HTTP get is empty
    payload = http.getString();
    payload = payload.substring(18, 23);		//substring to remove unnecessary characters
  }
  http.end();									//terminate HTTP
  return payload;
}
