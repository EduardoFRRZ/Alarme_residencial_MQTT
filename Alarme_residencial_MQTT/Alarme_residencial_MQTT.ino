//Programa : Sensor de presenca com modulo PIR
//Autor : FILIPEFLOP
 
int pinoRele = 13; //Pino ligado ao rele
int acionamento; //Variavel para guardar valor do sensor
int pinoSensorPir = 15;
 
void setup(){
  
  pinMode(pinoRele, OUTPUT); //Define pino rele como saida
  pinMode(pinoSensorPir, INPUT); //Define pino sensor como entrada
  Serial.begin(9600);

}
 
void loop(){
  
  acionamento = digitalRead(pinoSensorPir); //Le o valor do sensor PIR
 
  if (acionamento == LOW) //Sem movimento, mantem rele desligado
  {
    digitalWrite(pinoRele, LOW);
    Serial.println(acionamento);
  }
    else //Caso seja detectado um movimento, aciona o rele
  {
    digitalWrite(pinoRele, HIGH);
    Serial.println(acionamento);
  }
  
}
