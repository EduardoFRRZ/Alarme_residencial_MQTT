//Programa : ALARME RESIDÊNCIAL MQTT
//Autor : Eduardo Ferrarezi e Willian Agostini

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Btelway_Marines";
const char* password = "agostini";
//const char* ssid = "Net Virtua 577";
//const char* password = "1000160930";

const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define PIN_SENSOR D4 //definição do pino de saída do sensor PIR HC (Sensor de presença)
#define PIN_BUZZER D1 //definição do pino de entrada do Buzzer
#define PIN_LED D2 //definição do pino de entrada do LED

const char* topico_alarme = "unoesc/alarme";
const char* topico_led = "unoesc/led";

void setup() {
  //Definir os pinos como entrada ou saída de dados
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (millis() - lastMsg > 1000) {
    lastMsg = now;
    veriricarSensorPIR();
    // snprintf (msg, 50, "hello world #%ld", value);
    client.publish("outTopic", msg);
  }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void veriricarSensorPIR() {
  int acionamento = digitalRead(PIN_SENSOR); //Le o valor do sensor PIR
  //  Serial.println("sensor PIR");
  //  Serial.println(acionamento);

  snprintf (msg, 50, "%ld", acionamento);

  client.publish(topico_alarme, msg);

  //if (acionamento == HIGH) //Sem movimento
}

void setup_wifi() {

  delay(10);
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

void acaoLed(char payload) {
  Serial.println("aqui");
  Serial.println(payload);
  if (payload == 1 || payload == '1') {
    digitalWrite(PIN_LED, HIGH);
  } else {
    digitalWrite(PIN_LED, LOW);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == String(topico_led))
    acaoLed((char)payload[0]);
}

void reconectarNosTopicos() {
  client.publish("outTopic", "hello world");
  client.subscribe("inTopic");
  client.subscribe(topico_led);
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
      reconectarNosTopicos();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
