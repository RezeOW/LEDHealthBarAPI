#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "WifiCreds.h"

// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;

// Pins for the LED strips
const int Pin1 = 33;
const int Pin2 = 25;
const int Pin3 = 26;
const int Pin4 = 27;

int colorIndex;
CRGB Color;

Preferences pref;

StaticJsonDocument<250> jsonDocument;

// Color matrix
DEFINE_GRADIENT_PALETTE(heatmap_gp){
  0, 0, 0, 0,        //black
  50, 153, 0, 0,     //green
  100, 153, 153, 0,  //bright yellow
  150, 0, 153, 0     //red
};

CRGBPalette16 myPal = heatmap_gp;

// Number of LEDs in each strip
const int numLeds = 5;

CRGB leds1[numLeds];
CRGB leds2[numLeds];
CRGB leds3[numLeds];
CRGB leds4[numLeds];

// Current and maximum HP values for each player
int c1, m1, c2, m2, c3, m3, c4, m4;

String p1, p2, p3, p4;

int b;

WebServer server(80);  // create a server on port 80

void setup() {
  Serial.begin(115200);

  pref.begin("ValueSafe", false);

  c1 = pref.getInt("c1", 0);
  m1 = pref.getInt("m1", 0);
  c2 = pref.getInt("c2", 0);
  m2 = pref.getInt("m2", 0);
  c3 = pref.getInt("c3", 0);
  m3 = pref.getInt("m3", 0);
  c4 = pref.getInt("c4", 0);
  m4 = pref.getInt("m4", 0);

  p1 = pref.getString("p1", "Player 1");
  p2 = pref.getString("p2", "Player 2");
  p3 = pref.getString("p3", "Player 3");
  p4 = pref.getString("p4", "Player 4");

  b  = pref.getInt("b", 150);

  // Initialize the LED strip
  FastLED.addLeds<WS2812B, Pin1>(leds1, numLeds);
  FastLED.addLeds<WS2812B, Pin2>(leds2, numLeds);
  FastLED.addLeds<WS2812B, Pin3>(leds3, numLeds);
  FastLED.addLeds<WS2812B, Pin4>(leds4, numLeds);

  CRGB correction = CRGB(255, 255, 255);  // Example correction values

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
  server.on("/", HTTP_GET, handleGet);    // register a request handling function for GET requests
  server.on("/", HTTP_POST, handlePost);  // register a request handling function for POST requests
  server.on("/Foundry", HTTP_POST, handlePostFoundry);
  server.on("/Foundry", HTTP_OPTIONS, handlePreflight);
  server.enableCORS(true);
  server.begin();  // start the server
  updateLeds();
}

void loop() {
  // Check for incoming connections
  server.handleClient();
}

void handleGet() {
  Serial.println("GET by Browser");
  // handle the GET request

  send();
  updateLeds();
}

void send(){

    String Site = R"(

      <h1 style='text-align: center;'><span style='color: #ff0000;'><strong>Player HP Tracker</strong></span></h1>
      <form method='post'>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 8px;'>
      <td style='width: 40.5622%; height: 8px; text-align: center;'>Brightness</td> 
      <td style='width: 59.4377%; height: 8px; text-align: center;'><input style='text-align: center;'id='b' name='b' size='3' type='text' value='$b' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>
      
      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;' id='p1' name='p1' size='5' type='text' value='$p1' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c1' name='c1' size='3' type='text' value='$c1' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m1' name='m1' size='3' type='text' value='$m1' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;'id='p2' name='p2' size='5' type='text' value='$p2' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c2' name='c2' size='3' type='text' value='$c2' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m2' name='m2' size='3' type='text' value='$m2' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;'id='p3' name='p3' size='5' type='text' value='$p3' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c3' name='c3' size='3' type='text' value='$c3' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m3' name='m3' size='3' type='text' value='$m3' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <table style='border-collapse: collapse; width: 43.6079%; height: 64px; margin-left: auto; margin-right: auto;' border='1'>
      <tbody>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>HP</td> 
      <td style='width: 59.4377%; height: 18px; text-align: center;' colspan='2'><input style='text-align: center;'id='p4' name='p4' size='5' type='text' value='$p4' /></td>
      </tr>
      <tr style='height: 18px;'>
      <td style='width: 40.5622%; height: 18px; text-align: center;'>Current/Max</td>
      <td style='width: 29.7189%; height: 18px; text-align: center;'><input style='text-align: center;'id='c4' name='c4' size='3' type='text' value='$c4' /></td>
      <td style='width: 29.7188%; height: 18px; text-align: center;'><input style='text-align: center;'id='m4' name='m4' size='3' type='text' value='$m4' /></td>
      </tr>
      </tbody>
      </table>

      <p>&nbsp;</p>

      <p style='text-align: center;'><input style='text-align: center;'type='submit' onsubmit='return false' value='Update' /></p>

      </form>
      </body>
      </html>)";

  server.sendHeader("Content-Type", "text/html");

  Site.replace("$c1", String(c1));
  Site.replace("$m1", String(m1));
  Site.replace("$c2", String(c2));
  Site.replace("$m2", String(m2));
  Site.replace("$c3", String(c3));
  Site.replace("$m3", String(m3));
  Site.replace("$c4", String(c4));
  Site.replace("$m4", String(m4));
  Site.replace("$p1", String(p1));
  Site.replace("$p2", String(p2));
  Site.replace("$p3", String(p3));
  Site.replace("$p4", String(p4));
  Site.replace("$b", String(b));

  server.send(200, "text/html", Site);

}
void handlePost() {

  // handle the POST request
  Serial.println("POST by Browser");
  // Parse the form data
  if (!(server.arg("c1") == "")) {
    c1 = server.arg("c1").toInt();
    pref.putInt("c1", c1);
  }
  if (!(server.arg("m1") == "")) {
    m1 = server.arg("m1").toInt();
    pref.putInt("m1", m1);
  }
  if (!(server.arg("c2") == "")) {
    c2 = server.arg("c2").toInt();
    pref.putInt("c2", c2);
  }
  if (!(server.arg("m2") == "")) {
    m2 = server.arg("m2").toInt();
    pref.putInt("m2", m2);
  }
  if (!(server.arg("c3") == "")) {
    c3 = server.arg("c3").toInt();
    pref.putInt("c3", c3);
  }
  if (!(server.arg("m3") == "")) {
    m3 = server.arg("m3").toInt();
    pref.putInt("m3", m3);
  }
  if (!(server.arg("c4") == "")) {
    c4 = server.arg("c4").toInt();
    pref.putInt("c4", c4);
  }
  if (!(server.arg("m4") == "")) {
    m4 = server.arg("m4").toInt();
    pref.putInt("m4", m4);
  }
  if (!(server.arg("p1") == "")) {
    p1 = server.arg("p1");
    pref.putString("p1", p1);
  }
  if (!(server.arg("p2") == "")) {
    p2 = server.arg("p2");
    pref.putString("p2", p2);
  }
  if (!(server.arg("p3") == "")) {
    p3 = server.arg("p3");
    pref.putString("p3", p3);
  }
  if (!(server.arg("p4") == "")) {
    p4 = server.arg("p4");
    pref.putString("p4", p4);
  }
  if (!(server.arg("b") == "")) {
    b = server.arg("b").toInt();
    pref.putInt("b", b);
  }

  // Update the LED colors
  updateLeds();
  send();

}

void handlePostFoundry() {

  if (server.hasArg("plain") == false) {



    // handle the POST request from Foundry
    Serial.println("POST EMPTY");
    server.send(400);

  } else {

    // handle the POST request from Foundry
    Serial.println("POST from Foundry");

    String body = server.arg("plain");
    deserializeJson(jsonDocument, body);
    // Parse the data
    if (jsonDocument["c1"] >= 0) {
      c1 = jsonDocument["c1"];
      pref.putInt("c1", c1);
    }
    if (jsonDocument["m1"] >= 0) {
      m1 = jsonDocument["m1"];
      pref.putInt("m1", m1);
    }
    if (jsonDocument["c2"] >= 0) {
      c2 = jsonDocument["c2"];
      pref.putInt("c2", c2);
    }
    if (jsonDocument["m2"] >= 0) {
      m2 = jsonDocument["m2"];
      pref.putInt("m2", m2);
    }
    if (jsonDocument["c3"] >= 0) {
      c3 = jsonDocument["c3"];
      pref.putInt("c3", c3);
    }
    if (jsonDocument["m3"] >= 0) {
      m3 = jsonDocument["m3"];
      pref.putInt("m3", m3);
    }
    if (jsonDocument["c4"] >= 0) {
      c4 = jsonDocument["c4"];
      pref.putInt("c4", c4);
    }
    if (jsonDocument["m4"] >= 0) {
      m4 = jsonDocument["m4"];
      pref.putInt("m4", m4);
    }

    // Update the LED colors
    updateLeds();
    server.send(200, "application/json", "{}");
  }
}

void handlePreflight() {
  server.send(200);
}

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

void updateLeds() {
  
  for (int i = 0; i < 4; i++) {
    int c, m;

    if (i == 0) {

      c = c1;
      m = m1;
      Color = getColor(c, m);
      Serial.println(p1);
      Serial.println(c);
      Serial.println(m);

      if (c >= m * (4.0 / 5.0)) {
        leds1[0] = Color;
        leds1[1] = Color;
        leds1[2] = Color;
        leds1[3] = Color;
        leds1[4] = Color;

      } else if (c == 0) {
        leds1[0] = Color;
        leds1[1] = ColorFromPalette(myPal, 0);
        leds1[2] = Color;
        leds1[3] = ColorFromPalette(myPal, 0);
        leds1[4] = Color;

      } else if (c >= m * (3.0 / 5.0)) {
        leds1[0] = Color;
        leds1[1] = Color;
        leds1[2] = Color;
        leds1[3] = Color;
        leds1[4] = ColorFromPalette(myPal, 0);


      } else if (c >= m * (2.0 / 5.0)) {
        leds1[0] = Color;
        leds1[1] = Color;
        leds1[2] = Color;
        leds1[3] = ColorFromPalette(myPal, 0);
        leds1[4] = ColorFromPalette(myPal, 0);



      } else if (c >= m * (1.0 / 5.0)) {
        leds1[0] = Color;
        leds1[1] = Color;
        leds1[2] = ColorFromPalette(myPal, 0);
        leds1[3] = ColorFromPalette(myPal, 0);
        leds1[4] = ColorFromPalette(myPal, 0);


      } else {
        leds1[0] = Color;
        leds1[1] = ColorFromPalette(myPal, 0);
        leds1[2] = ColorFromPalette(myPal, 0);
        leds1[3] = ColorFromPalette(myPal, 0);
        leds1[4] = ColorFromPalette(myPal, 0);
      }

    } else if (i == 1) {

      c = c2;
      m = m2;
      Color = getColor(c, m);
      Serial.println(p2);
      Serial.println(c);
      Serial.println(m);


      if (c >= m * (4.0 / 5.0)) {
        leds2[0] = Color;
        leds2[1] = Color;
        leds2[2] = Color;
        leds2[3] = Color;
        leds2[4] = Color;

      } else if (c == 0) {
        leds2[0] = Color;
        leds2[1] = ColorFromPalette(myPal, 0);
        leds2[2] = Color;
        leds2[3] = ColorFromPalette(myPal, 0);
        leds2[4] = Color;


      } else if (c >= m * (3.0 / 5.0)) {
        leds2[0] = Color;
        leds2[1] = Color;
        leds2[2] = Color;
        leds2[3] = Color;
        leds2[4] = ColorFromPalette(myPal, 0);


      } else if (c >= m * (2.0 / 5.0)) {
        leds2[0] = Color;
        leds2[1] = Color;
        leds2[2] = Color;
        leds2[3] = ColorFromPalette(myPal, 0);
        leds2[4] = ColorFromPalette(myPal, 0);



      } else if (c >= m * (1.0 / 5.0)) {
        leds2[0] = Color;
        leds2[1] = Color;
        leds2[2] = ColorFromPalette(myPal, 0);
        leds2[3] = ColorFromPalette(myPal, 0);
        leds2[4] = ColorFromPalette(myPal, 0);


      } else {
        leds2[0] = Color;
        leds2[1] = ColorFromPalette(myPal, 0);
        leds2[2] = ColorFromPalette(myPal, 0);
        leds2[3] = ColorFromPalette(myPal, 0);
        leds2[4] = ColorFromPalette(myPal, 0);
      }


    } else if (i == 2) {

      c = c3;
      m = m3;
      Color = getColor(c, m);
      Serial.println(p3);
      Serial.println(c);
      Serial.println(m);


      if (c >= m * (4.0 / 5.0)) {
        leds3[0] = Color;
        leds3[1] = Color;
        leds3[2] = Color;
        leds3[3] = Color;
        leds3[4] = Color;

      } else if (c == 0) {
        leds3[0] = Color;
        leds3[1] = ColorFromPalette(myPal, 0);
        leds3[2] = Color;
        leds3[3] = ColorFromPalette(myPal, 0);
        leds3[4] = Color;


      } else if (c >= m * (3.0 / 5.0)) {
        leds3[0] = Color;
        leds3[1] = Color;
        leds3[2] = Color;
        leds3[3] = Color;
        leds3[4] = ColorFromPalette(myPal, 0);


      } else if (c >= m * (2.0 / 5.0)) {
        leds3[0] = Color;
        leds3[1] = Color;
        leds3[2] = Color;
        leds3[3] = ColorFromPalette(myPal, 0);
        leds3[4] = ColorFromPalette(myPal, 0);



      } else if (c >= m * (1.0 / 5.0)) {
        leds3[0] = Color;
        leds3[1] = Color;
        leds3[2] = ColorFromPalette(myPal, 0);
        leds3[3] = ColorFromPalette(myPal, 0);
        leds3[4] = ColorFromPalette(myPal, 0);


      } else {
        leds3[0] = Color;
        leds3[1] = ColorFromPalette(myPal, 0);
        leds3[2] = ColorFromPalette(myPal, 0);
        leds3[3] = ColorFromPalette(myPal, 0);
        leds3[4] = ColorFromPalette(myPal, 0);
      }


    } else {

      c = c4;
      m = m4;
      Color = getColor(c, m);
      Serial.println(p4);
      Serial.println(c);
      Serial.println(m);


      if (c >= m * (4.0 / 5.0)) {
        leds4[0] = Color;
        leds4[1] = Color;
        leds4[2] = Color;
        leds4[3] = Color;
        leds4[4] = Color;

      } else if (c == 0 ) {
        leds4[0] = Color;
        leds4[1] = ColorFromPalette(myPal, 0);
        leds4[2] = Color;
        leds4[3] = ColorFromPalette(myPal, 0);
        leds4[4] = Color;


      } else if (c >= m * (3.0 / 5.0)) {
        leds4[0] = Color;
        leds4[1] = Color;
        leds4[2] = Color;
        leds4[3] = Color;
        leds4[4] = ColorFromPalette(myPal, 0);


      } else if (c >= m * (2.0 / 5.0)) {
        leds4[0] = Color;
        leds4[1] = Color;
        leds4[2] = Color;
        leds4[3] = ColorFromPalette(myPal, 0);
        leds4[4] = ColorFromPalette(myPal, 0);



      } else if (c >= m * (1.0 / 5.0)) {
        leds4[0] = Color;
        leds4[1] = Color;
        leds4[2] = ColorFromPalette(myPal, 0);
        leds4[3] = ColorFromPalette(myPal, 0);
        leds4[4] = ColorFromPalette(myPal, 0);


      } else {
        leds4[0] = Color;
        leds4[1] = ColorFromPalette(myPal, 0);
        leds4[2] = ColorFromPalette(myPal, 0);
        leds4[3] = ColorFromPalette(myPal, 0);
        leds4[4] = ColorFromPalette(myPal, 0);
      }
    }
  }
  
  FastLED.setBrightness(b);
  FastLED.show();
}
