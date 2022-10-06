#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>                     //For using I2C connection of BMP180 in order to connect it to the board
#include <Adafruit_BMP085.h>

// Update these with values suitable for your network.

const char* ssid = "WOMANETOMA";
const char* password = "<password>";
const char* mqtt_server = "192.168.2.250";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_BMP085 bmp;                  //Defining the object bmp
#define I2C_SCL 12                    //Connect SCL pin of BMP180 to GPIO12(D6) of Nodemcu
#define I2C_SDA 13                    //Connect SDA pin of BMP180 to GPIO13(D7) of Nodemcu

long lastMsg = 0;
char msg[50];
int value = 0;
float dst,bt,bp,ba;
bool bmp085_present=true;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     // client.publish("outTopic", "hello world");
      // ... and resubscribe
     // client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Wire.begin(I2C_SDA, I2C_SCL); //Initializing the I2C connection
  delay(10);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 
  if (!bmp.begin()) 
  {
      Serial.println("Could not find a valid BMP085 sensor, check wiring!");
      while (1) {}
  }
          
  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;
    float bp =  bmp.readPressure()/100; // Division by 100 makes it in millibars
    //float ba =  bmp.readAltitude();
    float bt =  bmp.readTemperature();
    //float dst = bmp.readSealevelPressure()/100;
    Serial.print("Temp : ");
    Serial.println(String(bt).c_str());
    Serial.print("luchtdruk : ");
    Serial.println(String(bp).c_str());

    client.publish("temperatuur/buiten", String(bt).c_str());
    client.publish("luchtdruk/buiten", String(bp).c_str());
    
  }
}
