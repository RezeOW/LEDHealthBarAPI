#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "WifiCreds.h" // Header with WIFI_SSID and WIFI_PASSWD Definition
#include "HTML.h" // HTML for Browser Control

// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;

// Pins for the LED strips
const int Pin1 = 33;
const int Pin2 = 25;
const int Pin3 = 26;
const int Pin4 = 27;

// Dimensions
const int numLeds = 5;
const int numStrips = 4;

Preferences pref; // Value Safe
StaticJsonDocument<250> jsonDocument; // Json for HTTP Requests
CRGB leds[numStrips][numLeds]; // Led Control

int b; //Brightness
int c[4]; // Current HP
int bc[4]; // Last Current HP for Animation Checker
int m[4]; // Max HP
String p[4]; // Player Name
String cName[] = {"c1", "c2", "c3", "c4"}; // Value Safe Keys
String mName[] = {"m1", "m2", "m3", "m4"}; // Value Safe Keys
String pName[] = {"p1", "p2", "p3", "p4"}; // Value Safe Keys

WebServer server(80);  // create a server on port 80

void setup() {
  Serial.begin(115200);

  //Value Safe
  pref.begin("ValueSafe", false);
  for(int i = 0; i < numStrips; i++){
    c[i] = pref.getInt(cName[i].c_str(), 0);
    bc[i] = pref.getInt(cName[i].c_str(), 0);
    m[i] = pref.getInt(mName[i].c_str(), 0);
    p[i] = pref.getString(pName[i].c_str(), String("Player" + (i+1)));
  }
  b  = pref.getInt("b", 150);

  // Initialize the LED strip
  FastLED.addLeds<WS2812B, Pin1>(leds[0], numLeds);
  FastLED.addLeds<WS2812B, Pin2>(leds[1], numLeds);
  FastLED.addLeds<WS2812B, Pin3>(leds[2], numLeds);
  FastLED.addLeds<WS2812B, Pin4>(leds[3], numLeds);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", HTTP_GET, handleGet);    // Browser GET
  server.on("/", HTTP_POST, handlePost);  // Browser POST
  server.on("/Foundry", HTTP_POST, handlePostFoundry); // Foundry POST
  server.on("/Foundry", HTTP_OPTIONS, handlePreflight); // Foundry CORS Preflight
  server.enableCORS(true); // CORS for Foundry needed
  server.begin();  // start the server
  updateLeds(true, false);
  Serial.println("Booting finished");
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
  updateLeds(true, true); // Update LEDs
}

//Handle Browser POST
void handlePost() {
  Serial.println("POST by Browser");

  // Parse the form data
  for(int i = 0; i < numStrips; i++){
    if(!(server.arg(cName[i]) == "")){
      bc[i] = c[i]; // Log last HP for Animation Checker
      c[i] = server.arg(cName[i]).toInt();
      pref.putInt(cName[i].c_str(), c[i]);
    }
    if(!(server.arg(mName[i]) == "")){
      m[i] = server.arg(mName[i]).toInt();
      pref.putInt(mName[i].c_str(), m[i]);
    }
    if(!(server.arg(pName[i]) == "")){
      p[i] = server.arg(pName[i]);
      pref.putString(pName[i].c_str(), p[i]);
    }
  }
  if (!(server.arg("b") == "")) {
    b = server.arg("b").toInt();
    pref.putInt("b", b);
  }

  send(); // Send back Website
  updateLeds(true, true); // Update LEDs
}

//Foundry POST
void handlePostFoundry() {
  if (server.hasArg("plain") == false) { // handle empty POST
    Serial.println("POST from Foundry EMPTY");
    server.send(400);
  } else {
    // handle the POST request from Foundry
    Serial.println("POST from Foundry");
    String body = server.arg("plain");
    deserializeJson(jsonDocument, body);

    // Parse the Data
    for(int i; i < numStrips; i++){
      if (jsonDocument[cName[i]] >= 0) {
        bc[i] = c[i]; // Log last HP for Animation Checker
        c[i] = jsonDocument[cName[i]];
        pref.putInt(cName[i].c_str(), c[i]);
        Serial.println(c[i]);
      }
      if (jsonDocument[mName[i]] >= 0) {
        m[i] = jsonDocument[mName[i]];
        pref.putInt(mName[i].c_str(), m[i]);
        Serial.println(m[i]);
      }
      if (jsonDocument[pName[i]]) {
        const char* ptemp = jsonDocument[pName[i]];
        p[i] = ptemp;
        pref.putString(pName[i].c_str(), p[i]);
        Serial.println(p[i]);
      }
    }
    updateLeds(false, false); // Update LEDs
    server.send(200);
  }
}

//Preflight for CORS
void handlePreflight() {
  server.send(200);
}


//LED Control

// Update all LEDs with crent Values
void updateLeds(bool boot, bool animation){
  FastLED.setBrightness(b); // Set Brightness
  //Loop over Led Strips
  for(int i = 0; i < numStrips; i++){
    //Check for Animations
    if( animation == true ){
      AnimationChecker(i);
      Serial.println("Animations");
    }
    //Get Modifier and Color
    int mod = modifier(c[i], m[i]);
    CRGB Color = getColor(c[i], m[i]);
    //Loop over single Leds
    for(int x = 0; x < numLeds; x++){
      if(x < mod){
        leds[i][x] = Color;
      } else {
        leds[i][x] = CRGB(0, 0, 0); // Off
      }
      //On Boot Update individual Leds
      if(boot == true){
        Serial.println("LED BOOT");
        FastLED.show();
        delay(50);
      }
    }
  }
  // Commit
  FastLED.show();
}

//Red Bar from top piercing down
void DamageAnimation(int i){
  int mod = modifier(bc[i], m[i]) - 1;
  //Loop over single Leds
  for(int x = mod; x >= 0; x--){
      leds[i][x] = CRGB(0, 153, 0); // Red
      FastLED.show();
      delay(50);
  }
}

//Green Heal effect going up
void HealAnimation(int i){
  int mod = modifier(c[i], m[i]);
  //Loop over single Leds
  for(int x = 0; x < numLeds; x++){
    if(x < mod){
      leds[i][x] = CRGB(153, 0, 0); // Green
     } else {
       leds[i][x] = CRGB(0, 0, 0); // Off
    }
      FastLED.show();
      delay(50);
  }
}


//Helper Methods

// Check how HP changed to determine Animation
void AnimationChecker(int i){
if(c[i] < bc[i]){
    DamageAnimation(i);
  } else if(c[i] > bc[i]) {
    HealAnimation(i);
  } 
}

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
  // Replace with current Values
  SiteBody.replace("$c1", String(c[0]));
  SiteBody.replace("$m1", String(m[0]));
  SiteBody.replace("$c2", String(c[1]));
  SiteBody.replace("$m2", String(m[1]));
  SiteBody.replace("$c3", String(c[2]));
  SiteBody.replace("$m3", String(m[2]));
  SiteBody.replace("$c4", String(c[3]));
  SiteBody.replace("$m4", String(m[3]));
  SiteBody.replace("$p1", String(p[0]));
  SiteBody.replace("$p2", String(p[1]));
  SiteBody.replace("$p3", String(p[2]));
  SiteBody.replace("$p4", String(p[3]));
  SiteBody.replace("$b", String(b));

  server.send(200, "text/html", SiteBody);
}
