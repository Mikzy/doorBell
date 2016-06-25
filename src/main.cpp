#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

const char* ssid = "wireless";
const char* password = "oklahoma";

String mikael = "v168A49222058499";

String getIp();

ESP8266WebServer server(80);
HTTPClient http;
boolean deeps = false;

String getVoltage() {
    if(!deeps)
      return String(analogRead(A0)*0.00343);
    else
      return "";
}

void handleRoot() {
  String cont = "voltage is: ";
  cont += getVoltage();
  server.send(200, "text/html", "" + cont);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void sendData() {
    http.begin("http://data.sparkfun.com/input/bGDKM2qRXbtmnz3l8W4n?private_key=VpE2kZ6rlmSoWKxMypjW&power=" + String(1.0));
    http.GET();
    http.end();
}

String getIp() {
    http.begin("https://wtfismyip.com/text");
    http.GET();
    String myip = http.getString();
    myip = myip.substring(0, myip.length() - 1);
    http.end();
    return myip;
}

void ringDoorBell() {
    String volt = "";
    volt += getVoltage();
    http.begin("http://ringeklokkedet/ring");
    http.GET();
    http.end();
}

void sendMessage(String device) {
    String volt = "";
    volt += getVoltage();
    http.begin("http://api.pushingbox.com/pushingbox?devid=" + device + "&ip=" + getIp() + "&power=" + volt);
    http.GET();
    http.end();
}

void setup(void){
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("starting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    sendMessage(mikael);
    sendData();
    ringDoorBell();

    server.begin();
    String hostname = "RingeKlokke";
    ArduinoOTA.setHostname((const char *)hostname.c_str());
    ArduinoOTA.begin();
}

void loop(void){
    server.handleClient();
    if(millis() > 25000) {
        Serial.println("deep sleeping");
        //ESP.deepSleep(0);
        deeps = true;
        ArduinoOTA.handle();
    }
    if(millis() > 5000 && deeps) {
      ESP.deepSleep(0);
    }
}
