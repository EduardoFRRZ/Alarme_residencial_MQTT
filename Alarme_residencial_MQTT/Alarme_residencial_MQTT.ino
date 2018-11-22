//Programa : ALARME RESIDÊNCIAL MQTT
//Autor : Eduardo Ferrarezi e Willian Agostini

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//const char* ssid = "Unoesc";
//const char* password = "";

//const char* ssid = "Btelway_Marines";
//const char* password = "agostini";

const char* ssid = "Net Virtua 577";
const char* password = "1000160930";

const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#define PIN_BUZZER D1 //definição do pino de entrada do Buzzer
#define PIN_LED D2 //definição do pino de entrada do LED
#define PIN_RELE D3 //definição do pino de entrada do Relé
#define PIN_SENSOR D4 //definição do pino de saída do sensor PIR HC (Sensor de presença)

// NodeMCU 1
const char* topico_alarme_1 = "unoesc/alarme_1";
const char* topico_buzzer_1 = "unoesc/buzzer_1";
const char* topico_led_1 = "unoesc/led_1";
const char* topico_trancar_1 = "unoesc/trancar_1";

// NodeMCU 2
const char* topico_alarme_2 = "unoesc/alarme_2";
const char* topico_buzzer_2 = "unoesc/buzzer_2";
const char* topico_led_2 = "unoesc/led_2";
const char* topico_trancar_2 = "unoesc/trancar_2";

// NodeMCU 3
const char* topico_alarme_3 = "unoesc/alarme_3";
const char* topico_buzzer_3 = "unoesc/buzzer_3";
const char* topico_led_3 = "unoesc/led_3";
const char* topico_trancar_3 = "unoesc/trancar_3";

void setup() {
  //Definir os pinos como entrada ou saída de dados
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected())
    reconnect();
    
  client.loop();

  long now = millis();
  if (millis() - lastMsg > 1000) {
    lastMsg = now;
    
    leMeuSensorPresenca();
    escutaSensoresPresenca(); // Escuta os sensores das outras NodeMCU
    
    // snprintf (msg, 50, "hello world #%ld", value);
    client.publish("outTopic", msg);
  }

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void escutaSensoresPresenca(){
  bool statusSensor_2 = (client.subscribe(topico_alarme_2));
  bool statusSensor_3 = (client.subscribe(topico_alarme_3));

  if((statusSensor_2 != false) || (statusSensor_3 != false))
    acaoBuzzer('1');
  else
    acaoBuzzer('0');
}

void leMeuSensorPresenca() {
  int dados = digitalRead(PIN_SENSOR); //Le o valor do sensor de presença
  snprintf (msg, 50, "%ld", dados);
  Serial.println(dados);
  client.publish(topico_alarme_1, msg);
  delay(100);
  //if (dados == HIGH) //Sem movimento
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
  Serial.println(payload);
  if (payload == 1 || payload == '1') {
    digitalWrite(PIN_LED, HIGH);
  } else {
    digitalWrite(PIN_LED, LOW);
  }
}

void acaoBuzzer(char payload) {
  Serial.println(payload);
  if (payload == 1 || payload == '1') {
    digitalWrite(PIN_BUZZER, HIGH);
  } else {
    digitalWrite(PIN_BUZZER, LOW);
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
  
  if (String(topic) == String(topico_led_1))
    acaoLed((char)payload[0]);

  if (String(topic) == String(topico_buzzer_1))
    acaoBuzzer((char)payload[0]);
}

void reconectarNosTopicos() {
  client.subscribe("unoesc/buzzer_1");
  client.subscribe("unoesc/led_1");
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
