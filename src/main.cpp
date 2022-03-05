#if defined(ESP8266)
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>
#endif

#include "main.h"

AsyncWebServer server(80);
DNSServer dns;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(17, D1, NEO_RGB + NEO_KHZ800);

const char *PARAM_MESSAGE = "message";

int mode = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  // set led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  AsyncWiFiManager wifiManager(&server, &dns);
  // reset settings - for testing
  // wifiManager.resetSettings();

  // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  // keep LED on
  digitalWrite(LED_BUILTIN, LOW);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              if (mode < 4){
                mode = mode + 1;
              } else {
                mode = 0;
              }

              switch (mode)
              {
              case 0:
                // off
                strip.clear();
                break;
              case 1:
                // r
                strip.fill(strip.Color(255, 0, 0));
                break;
              case 2:
                // g
                strip.fill(strip.Color(0, 255, 0));
                break;
              case 3:
                 // b
                strip.fill(strip.Color(0, 0, 255));
                break;
              case 4:
                // blend
                strip.fill(strip.Color(127, 127, 127));
                break;
              default:
                strip.clear();
                break;
              }

              strip.show();
      
              request->send(200, "text/plain", "mode: " +  String(mode)); });

  // Send a POST request to <IP>/post with a form field message set to <message>
  // example post.
  // server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
  //           {
  //       String message;
  //       if (request->hasParam(PARAM_MESSAGE, true)) {
  //           message = request->getParam(PARAM_MESSAGE, true)->value();
  //       } else {
  //           message = "No message sent";
  //       }
  //       request->send(200, "text/plain", "Hello, POST: " + message); });

  server.onNotFound(notFound);

  server.begin();

  strip.begin();
  strip.setBrightness(70);
  strip.show();
}

void colorWipe(uint32_t c, unsigned long wait)
{
  for (uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void theaterChase(uint32_t c, uint8_t wait)
{
  for (int j = 0; j < 10; j++)
  { // do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
    {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, c); // turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3)
      {
        strip.setPixelColor(i + q, 0); // turn every third pixel off
      }
    }
  }
}

void loop()
{
  // colorWipe(strip.Color(255, 0, 0), 100); // Red
  // colorWipe(strip.Color(0, 255, 0), 100); // Green
  // colorWipe(strip.Color(0, 0, 255), 100); // Blue

  // theaterChase(strip.Color(127, 127, 127), 150); // White
}
