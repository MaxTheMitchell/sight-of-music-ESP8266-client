#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>
#include <math.h>

#ifndef STASSID
#define STASSID "YourWifiNetworkName"
#define STAPSK  "YourWifiPassword"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "sight-of-music-stage.herokuapp.com";
const int httpsPort = 80;

#define LED_PIN     5
#define NUM_LEDS    256
#define BRIGHTNESS  10
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100;

void setup() {
  delay( 3000 ); 
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  
  Serial.begin(9600);
  Serial.print("\nconnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  feedDataToLights(75);
  FastLED.show();
  delay(1000);
}

void feedDataToLights(int stringLength){
  int i = 0;
  int lightIndex = 0;
  String colorPart = "start";
  while (lightIndex < NUM_LEDS && colorPart != (String)"[]"){
    colorPart = getReq((String)"http://sight-of-music-stage.herokuapp.com/image/pixles/"+(String)sideLength(NUM_LEDS)+"/section/reversed?start="+(String)(stringLength*i)+(String)"&end="+(String)(stringLength*(i+1)));
    colorPart.trim();
    lightIndex = displayLights(colorPart,lightIndex);
    i++;
  }
}

int sideLength(int pixleAmount){
  return (int)sqrt(pixleAmount);
}


int displayLights(String colors,int lightIndex){
  int rgb[3];
  int rgbIndex = 0;
  String tmpRGB = "";
  for (int i = 0; i < colors.length();++i){
    char c = colors.charAt(i);
    if (c > '/' && c < ':'){
      tmpRGB += c;
    }
    else if (tmpRGB.length() > 0){
      if (lightIndex >= NUM_LEDS){
          break;
       }
      rgb[rgbIndex] = tmpRGB.toInt();
      tmpRGB = "";
      rgbIndex++;
      if (rgbIndex > 2){
        leds[NUM_LEDS-lightIndex-1].setRGB(rgb[0],rgb[1],rgb[2]);
        rgbIndex = 0;
        lightIndex++;
      }
    }
  }
  return lightIndex;
}

String getReq(String url){
  WiFiClient client;
  HTTPClient http;
  if (http.begin(client,url)) {
    if(http.GET() > 0){
      return http.getString();
    }
  }
  return "get req failed]";  
}
