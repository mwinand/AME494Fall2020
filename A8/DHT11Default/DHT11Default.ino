#define LILYGO_WATCH_2019_WITH_TOUCH
#include <LilyGoWatch.h>
TTGOClass *ttgo;

#include <SimpleDHT.h>

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "winand";
const char* password = "wolfenberg";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://3.137.140.14:1234/sendData";

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 25
int pinDHT11 = 25;
SimpleDHT11 dht11(pinDHT11);


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void pressed()
{
    uint16_t color = random(0xFFFF);
    ttgo->tft->fillScreen(color);
    ttgo->tft->setTextColor(color, TFT_WHITE);
    ttgo->tft->drawString("User Button pressed",  5, 100, 4);

    if(WiFi.status()== WL_CONNECTED){

      byte temperature = 0;
      byte humidity = 0;
      
      int err = SimpleDHTErrSuccess;
      if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
        Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(1000);
        return;
      }
  
      Serial.print("Sample OK: ");
      Serial.print(String((float)temperature) + "* C, ");
      Serial.println(String((float)humidity) + "% H");

      int t = (int)temperature;
      int h = (int)humidity;   
      Serial.println(response);
      response = httpGETRequest(serverName + "?t=" + t + "&h=" + h);
      
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  
}


void setup() {
  Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();

    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->setTextFont(4);
    ttgo->tft->drawString("User Button released",  5, 100, 4);

    ttgo->button->setPressedHandler(pressed);
    //ttgo->button->setReleasedHandler(released);

    
      WiFi.begin(ssid, password);
      Serial.println("Connecting");
      while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");



}

void loop() {
  ttgo->button->loop();
}
