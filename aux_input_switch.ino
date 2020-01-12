/*********
 * Works with NodeMCU (https://amzn.to/2UdY5cX), and 3 relay switchs to flip 3 relays at the same time and switch between stereo AUX inputs
********/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "RollinsRogue";
const char* password = "SonnyNights";
#define INPUT1_LED  4     // the number of the INPUT1_LED pin
#define INPUT2_LED  5     // the number of the INPUT2_LED pin

ESP8266WebServer server(80);
String webSite="";
int relay1 = 12; 
int relay2 = 13; 
int relay3 = 16; 

void setup(void){
  
  webSite +="<title>AUX Input Switch</title>\n";
  webSite+="<button onclick='myFunction()'>HOME</button>";
  webSite+="<script>function myFunction() {location.reload();}</script>";
  webSite +="<h1>AUX Input Switch</h1><p>Switch #1 <a href=\"input1\"><button>INPUT1</button></a>&nbsp;<a href=\"input2\"><button>INPUT2</button></a></p>\n";

  // preparing GPIOs
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  pinMode(INPUT1_LED, OUTPUT);
  pinMode(INPUT2_LED, OUTPUT);
  digitalWrite(INPUT1_LED, HIGH); // Low setting defaults to INPUT1 with no connection
  digitalWrite(INPUT2_LED, LOW);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(INPUT1_LED, HIGH); // Low setting defaults to INPUT1 with no connection
  digitalWrite(INPUT2_LED, LOW);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
 if (mdns.begin("aux_input", WiFi.localIP())) {
   Serial.println("MDNS responder started");
  }
    server.on("/", [](){
    server.send(200, "text/html", webSite);
    });

  server.on("/input1", [](){
    server.send(200, "text/html", webSite);
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(INPUT1_LED, HIGH); // Light goes on while on INPUT1
    digitalWrite(INPUT2_LED, LOW);
  });
  server.on("/input2", [](){
    server.send(200, "text/html", webSite);
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, LOW);
    digitalWrite(INPUT2_LED, HIGH); // Light goes on while on INPUT2
    digitalWrite(INPUT1_LED, LOW);
  });
    
  server.begin();
  Serial.println("HTTP server started");
}

 
void loop(void){
  server.handleClient();
  if(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, LOW);
    digitalWrite(INPUT2_LED, LOW); // Both lights flashing means it's disconnected
    delay(1000);
    digitalWrite(INPUT1_LED, HIGH);
    delay(1000);
    digitalWrite(INPUT2_LED, HIGH);
    delay(1000);
    digitalWrite(INPUT1_LED, LOW);
    delay(1000);
    digitalWrite(INPUT2_LED, LOW);
    delay(1000);
    digitalWrite(INPUT1_LED, HIGH);
    delay(1000);
    digitalWrite(INPUT2_LED, HIGH);
    delay(1000);
    digitalWrite(INPUT1_LED, LOW);
    Serial.println("");
    Serial.print("Wifi is disconnected!");
    Serial.println("");
    Serial.print("Reconnecting");
    Serial.println("");
    digitalWrite(INPUT1_LED, HIGH);
    digitalWrite(INPUT2_LED, HIGH);
    //WiFi.begin(ssid,password);
    
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, LOW);
    digitalWrite(INPUT1_LED, LOW);
    digitalWrite(INPUT2_LED, LOW);
    delay(1000);
    digitalWrite(INPUT1_LED, HIGH); // Blink means its connected
    digitalWrite(INPUT2_LED, HIGH);
    delay(1000);
    digitalWrite(INPUT1_LED, LOW);
    digitalWrite(INPUT2_LED, LOW);
    delay(1000);
    digitalWrite(INPUT1_LED, HIGH);
    digitalWrite(INPUT2_LED, HIGH);
    delay(1000);
    digitalWrite(INPUT1_LED, LOW);
    digitalWrite(INPUT2_LED, LOW);
    delay(1000);
    digitalWrite(INPUT1_LED, HIGH);
    digitalWrite(INPUT2_LED, HIGH);
  }

} 
