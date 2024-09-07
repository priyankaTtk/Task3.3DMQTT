#include <WiFiNINA.h>
#include <PubSubClient.h>

// Wi-Fi and MQTT settings
const char* ssid = "Vivo";
const char* password = "abcdefgh";
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_topic = "SIT210/wave";

WiFiClient espClient;
PubSubClient client(espClient);

// Pin definition
const int ledPin = 2;  // LED connected to Digital Pin 2

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("SubscriberClient")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  if (message == "Priyanka") {
    Serial.println("Priyanka message received, starting LED wave effect");
    waveEffect();
  }
}

void waveEffect() {
  const int waveLength = 3; // Number of steps in the wave
  const int delayTime = 100; // Delay between steps

  // Turn LED on for each step in the wave
  for (int step = 0; step < waveLength; step++) {
    for (int i = 0; i < waveLength; i++) {
      if (i == step) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
      delay(delayTime);
    }
  }

  // Turn LED off after completing one wave
  digitalWrite(ledPin, LOW);
}
