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

// Pin definitions
const int triggerPin = 2;
const int echoPin = 3;
const long timeout = 10000; // Timeout for pulseIn
const int distanceThreshold = 5; // Distance threshold in cm

// Flags for distance printing and message sending
bool hasSentMessage = false;

void setup() {
  Serial.begin(9600);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
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

  long duration;
  int distance;

  // Trigger the sensor
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Read the echo
  duration = pulseIn(echoPin, HIGH, timeout);

  // Calculate distance
  if (duration > 0) {
    distance = (duration / 2) / 29.1;
  } else {
    distance = 0; // If no pulse is received, set distance to 0
  }

  // Check if distance is within the threshold and greater than 0
  if (distance > 0 && distance <= distanceThreshold) {
    if (!hasSentMessage) {
      Serial.print("Distance: ");
      Serial.println(distance); // Print distance
      String message = "Priyanka";
      if (client.publish(mqtt_topic, message.c_str())) {
        Serial.println("Message sent: Priyanka");
      } else {
        Serial.println("Message failed to send");
      }
      hasSentMessage = true; // Prevent continuous message sending
    }
  } else {
    hasSentMessage = false; // Reset flag when object is not detected or distance is 0
  }

  delay(100); // Small delay to avoid rapid looping
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
    if (client.connect("PublisherClient")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Empty callback, as this device is a publisher
}
