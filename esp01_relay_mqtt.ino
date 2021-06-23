/*
 ESP8266 ESP-01 Relay board
 ESP has Tx only enabled to free pins 2 & 3 for later use.
 Tx is only really used for checking WiFi or MQTT status
 It connects to an MQTT server then:
  - announces connection with unqiue ESP8266 chip ID
  - switches relay low
  - publishes heartbeat relay state on relay01/status
  - waits for a message 0 or 1 in relay01/set/request
  - confirms success in relay01/set/response
 
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "-----";
const char* password = "-----";
const char* mqtt_server = "-----";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
int relayPin = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
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

void setRelay(char* topic, byte* payload, unsigned int length) {
  // Switch on the relay if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(relayPin, LOW);
    client.publish("relay01/set/response", "on");
  } 
   else if ((char)payload[0] == '0') {
    digitalWrite(relayPin, HIGH);
    client.publish("relay01/set/response", "off");
  }
  else {
    client.publish("relay01/response/set", "Unknown Request");
    Serial.print("Unknown MQTT Request");
}
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = String(ESP.getChipId(), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("relay01/status", "connected");
      // ... and resubscribe
      client.subscribe("relay01/set/request");
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
  pinMode(relayPin, OUTPUT);     // Initialize the relay pin as an output
  digitalWrite(relayPin, HIGH);   // force it off once at start
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY); // Removes Rx so gpio 3 available for later use
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(setRelay);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
//heartbeat 5min
  unsigned long now = millis();
  if (now - lastMsg > 300000) {
    lastMsg = now;
  
    if (digitalRead(relayPin) == LOW) {
       client.publish("relay01/status", "on");
    }
    else if (digitalRead(relayPin) == HIGH) {
       client.publish("relay01/status", "off");
    }
    else {  
       client.publish("relay01/status", "error");
    }
  }
}
