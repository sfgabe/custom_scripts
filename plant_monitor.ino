/* This works with nodemcu / esp8266 for reporting plant info via mqtt */

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

#define DHTPIN D3     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11     // DHT 11
#define ONE_WIRE_BUS D4 // Digital pin D4 is connected to the DS18B20
#define ANALOGPIN A0 // This is all so we can split 2 analog inputs to 1 pin
#define MOISTPIN D6
#define LIGHTPIN D7

int getMoisureInformation(int analogPin);
float getTemperatureOfSoil(DallasTemperature sensorTemperature ,DeviceAddress sensorDS18B20Address);
int delayBetweenReading(void);
int moistValue = 0;  // variable to store the value coming from moisture sensor
int lightValue = 0;  // variable to store the value coming from light sensor

// Update these with values suitable for your network.
const char* ssid = "WIFI NAME";
const char* password = "WIFI PASSWORD";
const char* mqtt_server = "mqttserver.address.local";
// char c;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// mqtt authentication info. 
#define MQTTuser "MQTT_USERNAME"
#define MQTTpsw "MQTT_PASSWORD"

DHT dht(DHTPIN, DHTTYPE); // on pin D3
int period = 120000;
unsigned long time_now = 0;
OneWire  pin(ONE_WIRE_BUS);  // on pin D4 (a 4.7K resistor is necessary)
DallasTemperature sensorTemperatureSoil(&pin);
DeviceAddress sensorDS18B20Address;

/*
   Function: getMoisureInformation
   ----------------------------
     Returns in percentege the value of Moisture

     analogPin: the value of ADC ping that is connected the sensor
     returns: Returns in percentege the value of Moisture of selected Pin
*/

int getMoisureInformation(int analogPin) {
  int rawValue = analogRead(analogPin);
  int dryValue = 950; //adjust as needed
  int wetValue = 250; //adjust as needed
  int friendlyDryValue = 0;
  int friendlyWetValue = 100;
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | ");
  int friendlyValue = map(rawValue, dryValue, wetValue, friendlyDryValue, friendlyWetValue);
  Serial.print(F("Moisture: "));
  Serial.print(friendlyValue);
  Serial.println(F("%"));
  return friendlyValue;
}

int getLightInformation(int analogPin) {
  int rawValue = analogRead(analogPin);
  int lightValue = 1; //adjust as needed
  int darkValue = 500; //adjust as needed
  int friendlyLightValue = 100;
  int friendlyDarkValue = 0;
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | ");
  int friendlyValue = map(rawValue, lightValue, darkValue, friendlyLightValue, friendlyDarkValue);
  Serial.print(F("Light: "));
  Serial.print(friendlyValue);
  Serial.println(F("%"));
  return friendlyValue;
}
/*
   Function: getTemperatureOfSoil
   ----------------------------
     Returns temperature of soil using DS18B20 sensor

     DallasTemperature sensorTemperature: the class created to comunicate to DS18B20 sensor
     DeviceAddress sensorDS18B20Address : Address of DS18B20A sensor
     returns: Returns temperature of environment(float)
*/
float getTemperatureOfSoil(DallasTemperature sensorTemperature ,DeviceAddress sensorDS18B20Address ) {
  sensorTemperature.requestTemperatures(); 
  float soilTemperature = sensorTemperature.getTempC(sensorDS18B20Address);
  Serial.print(F("Temperature of Soil: "));
  Serial.print(soilTemperature);
  Serial.println(F("°C"));
  return soilTemperature; 
}
/*
   Function: delayBetweenReading
   ----------------------------
     Returns the delay necessary between readings based on sensor details

     returns: Returns the delay in seconds 
*/

void setup() {
  Serial.begin(9600);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  dht.begin();
  sensorTemperatureSoil.begin();
  sensorTemperatureSoil.getAddress(sensorDS18B20Address, 0);
    // declare the moist and light pins as an OUTPUT:
  pinMode(MOISTPIN, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);
}

void setup_wifi()
{
  delay(100);
  
  // We start by connecting to a WiFi network
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (!mqttClient.connected()) 
  {
    connect();
  }
  
  mqttClient.loop();
  {
  {
    digitalWrite(1, getTemperatureOfSoil(sensorTemperatureSoil,sensorDS18B20Address));
    Serial.println(" Getting soil temp! ");
    mqttClient.publish("monitor-soil-temp/reading", String(getTemperatureOfSoil(sensorTemperatureSoil,sensorDS18B20Address)).c_str(), true);
  }
  
  {
   // read the value from sensor A:
    digitalWrite(MOISTPIN, HIGH); 
    delay(500);
    moistValue = analogRead(ANALOGPIN);
    Serial.println(" Getting soil moisture! ");
    Serial.println(moistValue);
    mqttClient.publish("monitor-soil-moisture/reading", String(getMoisureInformation(ANALOGPIN)).c_str(), true);
    digitalWrite(MOISTPIN, LOW);
 
    delay(500);
  
    // read the value from sensor B:
    digitalWrite(LIGHTPIN, HIGH); 
    delay(500);
    lightValue = analogRead(ANALOGPIN);
    Serial.println(" Getting light level! ");
    Serial.println(lightValue);
    mqttClient.publish("monitor-light-level/reading", String(getLightInformation(ANALOGPIN)).c_str(), true);
    digitalWrite(LIGHTPIN, LOW);
    Serial.println("----------------------------------------");
    delay(1000);
  }
  
  {
    int i; // count loops
    for (i = 0; i < 6; i++) {
      
      // Reading temperature or humidity takes about 250 milliseconds!
      // This loops because sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      delay(1000);
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);
        // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F("°C "));
      Serial.print(f);
      Serial.print(F("°F  Heat index: "));
      Serial.print(hic);
      Serial.print(F("°C "));
      Serial.print(hif);
      Serial.println(F("°F"));
      // Publish to MQTT
      mqttClient.publish("monitor-temp-h/reading", String(h).c_str(), true);
      mqttClient.publish("monitor-temp-f/reading", String(f).c_str(), true);
      mqttClient.publish("monitor-temp-c/reading", String(t).c_str(), true);
      mqttClient.publish("monitor-temp-hic/reading", String(hif).c_str(), true);
      mqttClient.publish("monitor-temp-hif/reading", String(hif).c_str(), true);
      }
    }
  // deep sleep for 15 min prevents moisture sensor rom corroding
    ESP.deepSleep(15 * 60 * 1000000);
  }
}

void connect()
{
  // Loop until we're connected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "monitor-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    // if you MQTT broker has clientID,username and password then change following line to
    if (mqttClient.connect(clientId.c_str(),MQTTuser,MQTTpsw))
    // if (mqttClient.connect(clientId.c_str()))
    {
      Serial.println("connected");
      
      // once connected to MQTT broker, subscribe to the topic
      // this is just in case you want to set it up to also receive commands from a water pump
      mqttClient.subscribe("monitor-incoming/reading");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
}
