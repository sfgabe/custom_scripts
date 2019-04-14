/*
   IoT - Remotely controled mailbox with NodeMCU through MQTT broker
   This one works on NODEMCU
*/
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define GPIO_PIN_LEDR D1  // pinout for setup
#define GPIO_PIN_LEDY D2  // pinout for setup
#define GPIO_PIN_LEDG D7  // pinout for setup
#define ARD_PIN_SERVO 2  // note - needs to be pin number not GPIO number for servo controller
#define MQTTuser "MQTT-USERNAME"
#define MQTTpsw "MQTT-PASSWORD"
#define DHTPIN 12
#define DHTTYPE DHT11 // or DHT22

// Update these with values suitable for your network.
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "your-mqtt-server.local";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
DHT dht(DHTPIN, DHTTYPE);
int period = 120000;
unsigned long time_now = 0;

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);
  pinMode(GPIO_PIN_LEDR, OUTPUT);
  digitalWrite(GPIO_PIN_LEDR, LOW);
  pinMode(GPIO_PIN_LEDY, OUTPUT);
  digitalWrite(GPIO_PIN_LEDY, LOW);
  pinMode(GPIO_PIN_LEDG, OUTPUT);
  digitalWrite(GPIO_PIN_LEDG, LOW);
  myservo.attach(ARD_PIN_SERVO);  // attaches the servo on pin 2 to the servo object
}

void loop()
{
  if (!mqttClient.connected()) 
  {
    connect();
  }

  mqttClient.loop();
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

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Command from MQTT broker is : ");
  Serial.print(topic);
  char p = payload[0];

  // if MQTT comes a 0 turn ON red light
  if (p == '0')
  {
    digitalWrite(GPIO_PIN_LEDR, HIGH);
    Serial.println(" Turn On Red Light! ");
    mqttClient.publish("mailbox-red/reading", String("ON").c_str(), true);
  }

  // if MQTT comes a 1, turn OFF red light
  if (p == '1')
  {
    digitalWrite(GPIO_PIN_LEDR, LOW);
    Serial.println(" Turn Off Red Light! " );
    mqttClient.publish("mailbox-red/reading", String("OFF").c_str(), true);
  }
  // if MQTT comes a 2 turn ON Yellow Light
  if (p == '2')
  {
    digitalWrite(GPIO_PIN_LEDY, HIGH);
    Serial.println(" Turn On Yellow Light! ");
    mqttClient.publish("mailbox-yellow/reading", String("ON").c_str(), true);
  }

  // if MQTT comes a 3, turn OFF pin A
  if (p == '3')
  {
    digitalWrite(GPIO_PIN_LEDY, LOW);
    Serial.println(" Turn Off Yellow Light! " );
    mqttClient.publish("mailbox-yellow/reading", String("OFF").c_str(), true);
  }
  // if MQTT comes a 4 turn ON Green Light
  if (p == '4')
  {
    digitalWrite(GPIO_PIN_LEDG, HIGH);
    Serial.println(" Turn On Green Light! ");
    mqttClient.publish("mailbox-green/reading", String("ON").c_str(), true);
  }

  // if MQTT comes a 5, turn OFF Green Light
  if (p == '5')
  {
    digitalWrite(GPIO_PIN_LEDG, LOW);
    Serial.println(" Turn Off Green Light! " ); 
    mqttClient.publish("mailbox-green/reading", String("OFF").c_str(), true);
  }
  // if MQTT comes a 6 turn down flag
   if (p == '6')
  {
    for (pos = 97; pos >= 2; pos -= 1) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
 //     delay(15);                       // waits 15ms for the servo to reach the position
    }
    Serial.println(" Lower flag! ");
    mqttClient.publish("mailbox-flag/reading", String("OFF").c_str(), true);
  }
  // if MQTT comes a 7, turn up flag
  if (p == '7')
  {
    for (pos = 2; pos <= 97; pos += 1) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
 //     delay(15);                       // waits 15ms for the servo to reach the position
    }
    Serial.println(" Raise Flag! " );
    mqttClient.publish("mailbox-flag/reading", String("ON").c_str(), true);
  }
  
  // if MQTT comes a 8 publish temp data
   if (p == '8')
  {
  // publish temp info every 2 minutes
 // if(millis() > time_now + period){
 //   time_now = millis();
 //   {
      // Loop a few times
    int i; // count loops
    for (i = 0; i < 6; i++) {
      
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
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
      mqttClient.publish("mailbox-temp-h/reading", String(h).c_str(), true);
      mqttClient.publish("mailbox-temp-f/reading", String(f).c_str(), true);
      mqttClient.publish("mailbox-temp-c/reading", String(t).c_str(), true);
      mqttClient.publish("mailbox-temp-hic/reading", String(hif).c_str(), true);
      mqttClient.publish("mailbox-temp-hif/reading", String(hif).c_str(), true);
      }
 //     }
 //   }
  }
}

void connect()
{
  // Loop until we're connected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "mailbox-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    // if you MQTT broker has clientID,username and password then change following line to
    if (mqttClient.connect(clientId.c_str(),MQTTuser,MQTTpsw))
    // if (mqttClient.connect(clientId.c_str()))
    {
      Serial.println("connected");
      
      //once connected to MQTT broker, subscribe to the topic
      mqttClient.subscribe("mailbox/reading");
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
