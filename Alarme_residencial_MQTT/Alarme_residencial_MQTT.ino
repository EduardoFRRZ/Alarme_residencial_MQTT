//Programa : sensor RESIDÊNCIAL MQTT
//Autor : Eduardo Ferrarezi e Willian Agostini

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//const char* ssid = "Unoesc";
//const char* password = "";

const char* ssid = "Btelway_Marines";
const char* password = "agostini";

//const char* ssid = "Net Virtua 577";
//const char* password = "1000160930";

const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

bool alarme_ativado = false;

#define PIN_BUZZER D1 //definição do pino de entrada do Buzzer
#define PIN_LED D2 //definição do pino de entrada do LED
#define PIN_RELE D3 //definição do pino de entrada do Relé
#define PIN_SENSOR D4 //definição do pino de saída do sensor PIR HC (Sensor de presença)
#define NODEMCU 2

#if NODEMCU == 1
const char* topico_led = "unoesc/led_1";
const char* topico_sensor = "unoesc/sensor_1";
const char* topico_trancar = "unoesc/trancar_1";

#elif NODEMCU == 2
const char* topico_led = "unoesc/led_2";
const char* topico_sensor = "unoesc/sensor_2";
const char* topico_trancar = "unoesc/trancar_2";

#elif NODEMCU == 3 //apenas buzzer
const char* topico_led = "unoesc/led_3";
const char* topico_sensor = "unoesc/sensor_3";
const char* topico_trancar = "unoesc/trancar_3";

#endif

const char* topico_buzzer = "unoesc/buzzer";
const char* topico_ativar_alarme = "unoesc/ativar_alarme";


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
  leMeuSensorPresenca();
}

void leMeuSensorPresenca() {
  long now = millis();
  if (millis() - lastMsg > 1000) {
    lastMsg = now;
    char msg[10];
    int dado = digitalRead(PIN_SENSOR); //Le o valor do sensor de presença
    snprintf (msg, 10, "%ld", dado);
    //Serial.print("PIR ");
    //Serial.println(dado);
    client.publish(topico_sensor, msg);

    if (alarme_ativado && dado == HIGH)
      client.publish(topico_buzzer, msg);
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

  if (String(topic) == String(topico_buzzer))
    acaoBuzzer((char)payload[0]);

  if (String(topic) == String(topico_ativar_alarme))
    acaoAtivarAlarme((char)payload[0]);

  if (String(topic) == String(topico_trancar))
    acaoTrancar((char)payload[0]);

}

void reconectarNosTopicos() {
  //client.subscribe("unoesc/#");
  client.subscribe(topico_led);
  client.subscribe(topico_trancar);
  client.subscribe(topico_buzzer);
  client.subscribe(topico_ativar_alarme);
}

void acaoLed(char payload) {
  digitalWrite(PIN_LED, (payload == 1 || payload == '1') );
}

void acaoBuzzer(char payload) {
  digitalWrite(PIN_BUZZER, (payload == 1 || payload == '1') );
}

void acaoAtivarAlarme (char payload) {
  alarme_ativado = (payload == 1 || payload == '1');
  client.publish(topico_buzzer, "0");

  if (alarme_ativado)
    client.publish(topico_trancar, "1");
}

void acaoTrancar(char payload) {
  digitalWrite(PIN_RELE, (payload == 1 || payload == '1') );
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

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      reconectarNosTopicos();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
