#define LILYGO_WATCH_2019_WITH_TOUCH
#include <LilyGoWatch.h>
TTGOClass *ttgo;

#include <SimpleDHT.h>

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "winand";
const char* password = "wolfenberg";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://18.224.22.27:1234/sendEmail";

String response;

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

      ttgo->tft->drawString(String((int)temperature*1.8 + 32) + " *F",  5, 10);
      ttgo->tft->drawString(String(humidity) + " % H",  5, 40);

      int t = (int)temperature;
      int h = (int)humidity;
      String url = String(serverName) + "?t=" + t + "&h=" + h;
      Serial.println(url);       
      response = httpGETRequest(url.c_str());
      Serial.println(response);

    if(WiFi.status()== WL_CONNECTED){

      Serial.println(response);
      
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

}

void loop() {
  ttgo->button->loop();
}
