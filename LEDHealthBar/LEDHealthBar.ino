#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "WifiCreds.h"
#include "HTML.h"

// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;

// Pins for the LED strips
const int Pin1 = 33;
const int Pin2 = 25;
const int Pin3 = 26;
const int Pin4 = 27;

// Number of LEDs in each strip
const int numLeds = 5;
const int numStrips = 4;

Preferences pref;

StaticJsonDocument<250> jsonDocument;

CRGB leds[numStrips][numLeds];

// Brightness, Current HP, Max HP, Player Names
int b;
int c[4];
int m[4];
String name[4];
String cName[] = {"c1", "c2", "c3", "c4"};
String mName[] = {"m1", "m2", "m3", "m4"};
String nameName[] = {"p1", "p2", "p3", "p4"};

WebServer server(80);  // create a server on port 80

void setup() {
  Serial.begin(115200);

  //Value Safe
  pref.begin("ValueSafe", false);
  for(int i = 0; i < numStrips; i++){
    c[i] = pref.getInt(cName[i].c_str(), 0);
    m[i] = pref.getInt(mName[i].c_str(), 0);
    name[i] = pref.getString(nameName[i].c_str(), "Player");
  }
  b  = pref.getInt("b", 150);

  // Initialize the LED strip
  FastLED.addLeds<WS2812B, Pin1>(leds[0], numLeds);
  FastLED.addLeds<WS2812B, Pin2>(leds[1], numLeds);
  FastLED.addLeds<WS2812B, Pin3>(leds[2], numLeds);
  FastLED.addLeds<WS2812B, Pin4>(leds[3], numLeds);

  CRGB correction = CRGB(255, 255, 255);
  FastLED.setCorrection(correction);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", HTTP_GET, handleGet);    // Browser GET
  server.on("/", HTTP_POST, handlePost);  // Browser POST
  server.on("/Foundry", HTTP_POST, handlePostFoundry); // Foundry POST
  server.on("/Foundry", HTTP_OPTIONS, handlePreflight); // Foundry CORS Preflight
  server.enableCORS(true); // CORS for Foundry needed
  server.begin();  // start the server
  updateLeds();
}

void loop() {
  // Check for incoming connections
  server.handleClient();
}


//Handler

//Handle Browser GET
void handleGet() {
  Serial.println("GET by Browser");
  send(); // Send back Website
  updateLeds(); // Update LEDs
}

//Handle Browser POST
void handlePost() {
  Serial.println("POST by Browser");

  // Parse the form data
  for(int i = 0; i < numStrips; i++){
    if(!(server.arg(cName[i]) == "")){
      c[i] = server.arg(cName[i]).toInt();
      pref.putInt(cName[i].c_str(), c[i]);
    }
    if(!(server.arg(mName[i]) == "")){
      m[i] = server.arg(mName[i]).toInt();
      pref.putInt(mName[i].c_str(), m[i]);
    }
    if(!(server.arg(nameName[i]) == "")){
      name[i] = server.arg(nameName[i]);
      pref.putString(nameName[i].c_str(), name[i]);
    }
  }
  if (!(server.arg("b") == "")) {
    b = server.arg("b").toInt();
    pref.putInt("b", b);
  }

  send(); // Send back Website
  updateLeds(); // Update LEDs
}

//Foundry POST
void handlePostFoundry() {
  if (server.hasArg("plain") == false) {

    // handle empty POST
    Serial.println("POST EMPTY");
    server.send(400);

  } else {
    // handle the POST request from Foundry
    Serial.println("POST from Foundry");
    String body = server.arg("plain");
    deserializeJson(jsonDocument, body);

    // Parse the 
    for(int i; i < numStrips; i++){
      if (jsonDocument[cName[i]] >= 0) {
        c[i] = jsonDocument[cName[i]];
        pref.putInt(cName[i].c_str(), c[i]);
      }
      if (jsonDocument[mName[i]] >= 0) {
        m[i] = jsonDocument[mName[i]];
        pref.putInt(mName[i].c_str(), m[i]);
      }
    }
    updateLeds(); // Update LEDs
    server.send(200);
  }
}

//Preflight for CORS
void handlePreflight() {
  server.send(200);
}


//LED Control

// Update all LEDs with crent Values
void updateLeds(){

  //Loop over Led Strips
  for(int i = 0; i < numStrips; i++){
    int mod = modifier(c[i], m[i]);
    CRGB Color = getColor(c[i], m[i]);
    //Loop over single Leds
    for(int x = 0; x < numLeds; x++){
      if(x < mod){
        leds[i][x] = Color;
      } else {
        leds[i][x] = CRGB(0, 0, 0);
      }
    }
  }
  //Set Brighness and Update
  FastLED.setBrightness(b);
  FastLED.show();
}


//Helper Methods

//Decide on LED Color
CRGB getColor(int c, int m) {

  // Map the current value to a color index
  CRGB heatindex;
  int colorIndex = map(c, 0, m, 0, 100);

  if (colorIndex <= 30) {
    heatindex = CRGB(0, 153, 0);
  } else if (colorIndex <= 70) {
    heatindex = CRGB(153, 153, 0);
  } else {
    heatindex = CRGB(153, 0, 0);
  }

  return heatindex;
}

//Decide how many Leds are on
int modifier(int c, int m){
  int mod;
  if (c >= m * (4.0 / 5.0)) {
    mod = numLeds;
  } else if (c == 0) {
    mod = numLeds - 5;
  } else if (c >= m * (3.0 / 5.0)) {
    mod = numLeds - 1;
  } else if (c >= m * (2.0 / 5.0)) {
    mod = numLeds - 2;
  } else if (c >= m * (1.0 / 5.0)) {
    mod = numLeds - 3;
  } else {
    mod = numLeds - 4;
  }
  return mod;
}

//Send new Website back
void send(){

  String SiteBody = SITE_HTML;

  server.sendHeader("Content-Type", "text/html");

  SiteBody.replace("$c1", String(c[0]));
  SiteBody.replace("$m1", String(m[0]));
  SiteBody.replace("$c2", String(c[1]));
  SiteBody.replace("$m2", String(m[1]));
  SiteBody.replace("$c3", String(c[2]));
  SiteBody.replace("$m3", String(m[2]));
  SiteBody.replace("$c4", String(c[3]));
  SiteBody.replace("$m4", String(m[3]));
  SiteBody.replace("$p1", String(name[0]));
  SiteBody.replace("$p2", String(name[1]));
  SiteBody.replace("$p3", String(name[2]));
  SiteBody.replace("$p4", String(name[3]));
  SiteBody.replace("$b", String(b));

  server.send(200, "text/html", SiteBody);

}