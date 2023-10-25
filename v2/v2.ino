#include "FastLED.h"
#include "WiFiTerm.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define NUM_LEDS      33 //le plus grand est a 340
#define LED_TYPE   WS2812
#define COLOR_ORDER   GRB
#define DATA_PIN        5
//#define CLK_PIN       4
#define VOLTS           5
#define MAX_MA      10000
#define TENSION A0 //PIN A0 correspond à la tension des piles

/* Set these to your desired credentials. */
const char *ssid = "Jeb_";
const char *password = "TourtelMoustik";

ESP8266WebServer webserver(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/

//My variables
unsigned long currentMillis = 0;
unsigned long oldMillisDisco = 0; 
unsigned long oldMillisShow = 0; 
unsigned long oldMillisStatic = 0; 
unsigned long oldMillisTension = 0;
unsigned int colorMode = 0;
unsigned int staticColor = 0;
unsigned int brightness = 50;
String outputLedState = "on";
uint8_t hue = 0;
unsigned int jebSpeed = 20;

CRGBArray<NUM_LEDS> leds;

void setup() {
  Serial.begin(115200);
  
  //StartWIFI
  startup_wifi();
  //Start ArduinoOTA  
  startup_arduinoOTA();

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //SettingUP Fast led
  FastLED.setMaxPowerInVoltsAndMilliamps( VOLTS, MAX_MA);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);

  // clear all pixel data
  LEDS.setBrightness(brightness);
  FastLED.clear();  
  FastLED.show();
  term.begin(webserver);
}


void loop()
{
  //############################# Loop #####################################
  ArduinoOTA.handle();
  webserver.handleClient();
  yield(); //shitty webserver is not working without that
  term.handleClient();

  if (term.available())
  {
    term.print("Received : ");
    term.println(term.readString());
  }
  yield();
  currentMillis = millis();

  if (outputLedState == "off") {
    if ((currentMillis - oldMillisShow) > 10) {
      FastLED.clear();  // clear all pixel data
      FastLED.show();
      //Serial.println("LED OFF SHOW");
      oldMillisShow = millis();
    }
  } else {
    if (colorMode == 1 && (currentMillis - oldMillisStatic) > 500){
      if(staticColor == 0){ //Blue
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(59, 161, 227);
        }
      }else if(staticColor == 1){ //Red
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(227, 64, 59);
        } 
      }else if(staticColor == 2){ //Yellow
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(220, 227, 59);
        }
      }else if(staticColor == 3){ //Orange
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(227, 197, 59);
        }
      }else if(staticColor == 4){ //Green
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(61, 227, 59);
        }
      }else if(staticColor == 5){ //White
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(255, 255, 255);
        }
      }
      FastLED.show();
      oldMillisStatic = millis();
    } else if (colorMode == 0) {
      if ((currentMillis - oldMillisDisco) > jebSpeed){
        jeb();
        FastLED.show();
        oldMillisDisco = millis();
      }
    }
  }
  if ((currentMillis - oldMillisTension) > 200) {
    term.println("Tension piles: " + String(analogRead(TENSION)));
    oldMillisTension = millis();
  }
}

//Mes fonctions

void startup_wifi(){
  // Connect to Wi-Fi network with SSID and password
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  

  webserver.on("/", handleRoot);
  webserver.on("/brightness/", handleBrightness);
  webserver.on("/color/", handleColor);
  webserver.on("/led/", handleLed);
  webserver.on("/mode/", handleMode);
  webserver.on("/jeb/", handleJeb);
  webserver.begin();

  Serial.println("HTTP server started");
}

void startup_arduinoOTA(){
  ArduinoOTA.setHostname("Jeb");

  //Starting OTA Service
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
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
}

void jeb(){
    for(int i = 0; i < 33; i++) {
       leds[i] = CHSV(hue, 255, 255);
    }
    leds[15] = CRGB(255,90,110);
    leds[16] = CRGB(255,90,110);
    leds[18] = CRGB(255,90,110);
    leds[19] = CRGB(255,90,110);
    leds[21] = CRGB(255,90,110);
    leds[22] = CRGB(255,90,110);    
    leds[24] = CRGB(255,90,110);
    leds[25] = CRGB(255,90,110);
    hue++; // Incrementation HUE (cycle de couleur arc-en-ciel RGB)
}

//===============================================================
// Webpage routines
//===============================================================
void handleRoot() {
  String webpage = "<!DOCTYPE html>\
  <html>\
  <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
    <link rel=\"icon\" href=\"data:,\">\
    <style>html { \
      font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\
      .wrapper li {float: left;padding: 4em;}\
      .jeb li {float: left;padding: 0.5em;}\
      .jeb{text-align:center;}\
      .wrapper{text-align:center;} ul{display:inline-block;margin:0;padding:0;list-style:none;}\
      .button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;}\
      text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\
      .button2 {background-color: #77878A;}\
      .blue {background-color: #276EE7;}\
      .red {background-color: #E73927;}\
      .orange {background-color: #E78E27;}\
      .green {background-color: #27E762;}\
      .yellow {background-color: #DFE727;}\
      .white {background-color: #E1E4CD;}\
    </style>\
  </head>\
  <body><div class=\"wrapper\"><ul>\
    <li><form action=\"/brightness/\" method=\"post\" id=\"form\">\
      <h3>Luminosite:<output name=\"LuminosityOutName\" id=\"LuminosityOut\">" + String(brightness) + "</output></h3>\
        <p><input type=\"range\" name=\"LuminosityName\" id=\"LuminosityId\" value=\"" + String(brightness) + "\" min=\"5\" max=\"255\" oninput=\"LuminosityOut.value = LuminosityId.value\"></p>\
        <p><input type=\"submit\" class=\"button\" name=\"Send\"></p>\
      </form></li>\
    <li><h1>Color Choice Host </h1>";
    if (colorMode == 1) {
      webpage += "<h2>Static Mode</h2></li>";
    } else if (colorMode == 0) {
      webpage += "<h2>Jeb Mode</h2></li>";
    }
    webpage += "<li><h3>LED - State : " + outputLedState + "</h3>\
                <form action=\"/led/\" method=\"post\">";
  if (outputLedState == "off") {
    webpage += "<p><button name=\"led\" value=\"on\" class=\"button\">ON</button></p>";
  } else {
    webpage += "<p><button name=\"led\" value=\"off\" class=\"button button2\">OFF</button></p>";
  }
  webpage += "</form></li></ul></div>";
  if (colorMode == 1) {
    webpage += "<form action=\"/mode/\" method=\"post\">\
                <p>Change color mode</p>\
                <p><button name=\"mode\" value=\"jeb\" class=\"button\">Jeb mode</button></p>\
                </form><form action=\"/color/\" method=\"post\">\
                <p><button name=\"color\" value=\"blue\" class=\"button blue\">Blue</button>\
                <button name=\"color\" value=\"red\" class=\"button red\">Red</button>\
                <button name=\"color\" value=\"yellow\" class=\"button yellow\">Yellow</button>\
                <button name=\"color\" value=\"orange\" class=\"button orange\">Orange</button>\
                <button name=\"color\" value=\"green\" class=\"button green\">Green</button>\
                <button name=\"color\" value=\"white\" class=\"button white\">White</button></p>\
                </form>";
  } else if (colorMode == 0) {
    webpage += "<div class=\"jeb\"><form action=\"/jeb/\" method=\"post\" id=\"form\">\
                  <p>Vitesse (en ms):<output name=\"SpeedOutName\" id=\"SpeedOut\">" + String(jebSpeed) + "</output></p>\
                  <ul><li style=\" \"><p><input type=\"range\" name=\"Speed\" id=\"SpeedId\" value=\"" + String(jebSpeed) + "\" min=\"5\" max=\"50\" oninput=\"SpeedOut.value = SpeedId.value\"></p></li>\
                  <li><p><input type=\"submit\" class=\"button\" name=\"Send\"></p></li></ul>\
                </form>\
                <form action=\"/mode/\" method=\"post\">\
                <p>Change color mode</p>\
                <p><button name=\"mode\" value=\"static\" class=\"button\">Static mode</button></p>\
                </form></div>";
  }
  webpage += "</body></html>";
  webserver.send(200, "text/html", webpage);
}


void handleBrightness() {
  String brightnesslider = webserver.arg("LuminosityName");

  brightness = brightnesslider.toInt();
  LEDS.setBrightness(brightness);
  
  webserver.sendHeader("Location", String("/"), true);
  webserver.send(302, "text/plain", "");
  Serial.println("Luminosite voulue:");
  Serial.println(brightnesslider);
}

void handleJeb() {
  String speedSlider = webserver.arg("Speed");

  jebSpeed = speedSlider.toInt();
  
  webserver.sendHeader("Location", String("/"), true);
  webserver.send(302, "text/plain", "");
}
void handleColor() {
  if (webserver.arg("color") == "blue") {
    staticColor = 0; //Blue
  } else if (webserver.arg("color") == "red") {
    staticColor = 1;
  } else if (webserver.arg("color") == "yellow") {
    staticColor = 2;
  } else if (webserver.arg("color") == "orange") {
    staticColor = 3;
  } else if (webserver.arg("color") == "green") {
    staticColor = 4;
  } else if (webserver.arg("color") == "white") {
    staticColor = 5;
  } else {
    String message = "Button send the argument: " + webserver.arg(0);
    webserver.send(200, "text/html", message);
    return;
  }
  webserver.sendHeader("Location", String("/"), true);
  webserver.send(302, "text/plain", "");
}

void handleMode() {
  if (webserver.arg("mode") == "static") {
    Serial.println("Changing mode to static");
    colorMode = 1;
  } else if (webserver.arg("mode") == "jeb") {
    Serial.println("Changing mode to jeb");
    colorMode = 0;
  } else {
    String message = "Button send the argument: " + webserver.arg(0);
    webserver.send(200, "text/html", message);
    return;
  }
  webserver.sendHeader("Location", String("/"), true);
  webserver.send(302, "text/plain", "");
}

void handleLed() {
  if (webserver.arg(0) == "off") {
    Serial.println("LED off");
    outputLedState = "off";
  } else if (webserver.arg(0) == "on") {
    Serial.println("LED on");
    outputLedState = "on";
  } else {
    webserver.send(200, "text/html", "Button send the argument: " + webserver.arg(0));
    return;
  }
  webserver.sendHeader("Location", String("/"), true);
  webserver.send(302, "text/plain", "");
}
