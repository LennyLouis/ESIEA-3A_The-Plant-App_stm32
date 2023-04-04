#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <PubSubClient.h>

// Paramètres de connexion WiFi
const char* ssid = "iPhone de Lenny";
const char* password = "YBYh-hC0X-62eT-FT86";

// Paramètres du broker MQTT
const char* mqtt_server = "lennylouis.fr";
const char* mqtt_user = "1sihuvY7E9zKSY3CYINh";
const char* mqtt_password = "n0sks6LwqTUchlQUdoIfrKNdPSoRRrUC4r60w7rDc";
const int mqtt_port = 1883;

// Initialise le client WiFi et le client MQTT
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  // Traitement du message MQTT reçu
}

void reconnect() {
  // Boucle de reconnexion MQTT
  while (!mqttClient.connected()) {
    Serial.println("Tentative de reconnexion au broker MQTT...");
    if (mqttClient.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connecté au broker MQTT");
      mqttClient.subscribe("plant");
    } else {
      Serial.print("Échec de la reconnexion au broker MQTT, état : ");
      Serial.print(mqttClient.state());
      Serial.println(" réessayer dans 5 secondes");
      delay(5000);
    }
  }
}

void setup() {
  // Démarrage de la liaison série (debug)
  Serial.begin(115200);

  // Connexion au réseau WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au réseau WiFi...");
  }
  Serial.println("Connecté au réseau WiFi");

  // Configuration du client MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);

  // Connexion au broker MQTT
  while (!mqttClient.connected()) {
    Serial.println("Connexion au broker MQTT...");
    if (mqttClient.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connecté au broker MQTT");
      mqttClient.subscribe("plant");
    } else {
      Serial.print("Échec de la connexion au broker MQTT, état : ");
      Serial.print(mqttClient.state());
      Serial.println(" réessayer dans 5 secondes");
      delay(5000);
    }
  }
}

void loop() {
  // Vérification de la connexion MQTT
  if (!mqttClient.connected()) {
    reconnect();
  }

  // Vérification des messages reçus
  mqttClient.loop();

  // Envoyer au broker mqtt les informations reçu du STM32
  if (Serial.available()) {
      String message = Serial.readString();
      mqttClient.publish("plant", message.c_str());
  }
}