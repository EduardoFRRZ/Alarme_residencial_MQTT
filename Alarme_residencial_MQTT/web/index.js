{/* <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script> */ }

// Create a client instance


// connect the client
var client;
var clientes_ativos_tempo = []
var clientes_ativos = [null, null, null]

function onConnect() {
  console.log("conectado");
  // client.subscribe('unoesc/led_1', { qos: Number(2) });
  // client.subscribe('unoesc/trancar_1', { qos: Number(2) });
  // client.subscribe('unoesc/led_2', { qos: Number(2) });
  // client.subscribe('unoesc/trancar_2', { qos: Number(2) });
  // client.subscribe('unoesc/led_3', { qos: Number(2) });
  // client.subscribe('unoesc/trancar_3', { qos: Number(2) });
  // client.subscribe('unoesc/buzzer', { qos: Number(2) });
  // client.subscribe('unoesc/ativar_alarme', { qos: Number(2) });
  client.subscribe('unoesc/#', { qos: Number(2) });

}
// called when the client connects
function connect() {

  let conexao = document.getElementById('conexao');
  let broker = conexao.host.value;
  let port = conexao.port.value;

  client = new Paho.MQTT.Client(broker, Number(port), "unoesc-websocket");
  client.connect({ onSuccess: onConnect });
  client.onConnectionLost = onConnectionLost;
  client.onMessageArrived = onMessageArrived;
}

function publish(topico, payload) {
  console.log(topico, payload)
  let message = new Paho.MQTT.Message(payload);
  message.destinationName = topico;
  message.qos = Number(2);
  message.retained = true;
  client.send(message);
}

function alterar_badge(elementoId, payload, classTrue, classFalse) {
  let span = document.getElementById(elementoId)
  if (payload == 1)
    span.className = "badge badge-pill " + classTrue
  else
    span.className = "badge badge-pill " + classFalse
}

function alterar_badge_led(elementoId, payload) {
  alterar_badge(elementoId, payload, "badge-success", "badge-light")
}

function alterar_badge_sensor_alarme(elementoId, payload) {
  alterar_badge(elementoId, payload, "badge-warning", "badge-light")
}

function alterar_badge_buzzer(elementoId, payload) {
  alterar_badge(elementoId, payload, "badge-danger", "badge-primary")
}

// called when a message arrives
function onMessageArrived(message) {
  console.log(message.destinationName)

  if (message.destinationName == "unoesc/led_1")
    alterar_badge_led("spanled1", message.payloadString)

  if (message.destinationName == "unoesc/trancar_1")
    alterar_badge_led("spantranca1", message.payloadString)

  if (message.destinationName == "unoesc/led_2")
    alterar_badge_led("spanled2", message.payloadString)

  if (message.destinationName == "unoesc/trancar_2")
    alterar_badge_led("spantranca2", message.payloadString)

  if (message.destinationName == "unoesc/led_3")
    alterar_badge_led("spanled3", message.payloadString)

  if (message.destinationName == "unoesc/trancar_3")
    alterar_badge_led("spantranca3", message.payloadString)

  if (message.destinationName == "unoesc/sensor_1")
    alterar_badge_sensor_alarme("spanpresenca1", message.payloadString)

  if (message.destinationName == "unoesc/sensor_2")
    alterar_badge_sensor_alarme("spanpresenca2", message.payloadString)

  if (message.destinationName == "unoesc/sensor_3")
    alterar_badge_sensor_alarme("spanpresenca3", message.payloadString)

  if (message.destinationName == "unoesc/buzzer")
    alterar_badge_buzzer("status_buzzer", message.payloadString)

  verificarAtividade(message.destinationName)
}

function verificarAtividade(topico) {
  if (topico.includes('sensor_1'))
    clientes_ativos_tempo[0] = new Date().getSeconds()

  if (topico.includes('sensor_2'))
    clientes_ativos_tempo[1] = new Date().getSeconds()

  if (topico.includes('sensor_3'))
    clientes_ativos_tempo[2] = new Date().getSeconds()
}

function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0)
    console.log("onConnectionLost:" + responseObject.errorMessage);
}

function salvarConfigAtuais() {
  let conexao = document.getElementById('conexao');
  let host = conexao.host.value;
  let port = conexao.port.value;
  let obj = { host: host, port: port };
  localStorage.setItem('conexao', JSON.stringify(obj));
}

window.onload = () => {
  let conexaoSalva = localStorage.getItem('conexao');
  if (conexaoSalva) {
    conexaoSalva = JSON.parse(conexaoSalva);
    let conexao = document.getElementById('conexao');
    conexao.host.value = conexaoSalva.host + '';
    conexao.port.value = conexaoSalva.port + '';
  }

  // conectar automaticamente
  connect();

  setInterval(alertFunc, 4000);

}

function alertFunc() {

  for (let index = 0; index < 3; index++) {
    clientes_ativos[index] = (clientes_ativos_tempo[index] != null)
    clientes_ativos_tempo[index] == null
  }

  for (let index = 0; index < 3; index++) {
    var texto = "onn"
    if (clientes_ativos[index] == false)
      texto = "offline"


    if (index == 0)
      document.getElementById("online1").innerText = texto

    if (index == 1)
      document.getElementById("online2").innerText = texto

    if (index == 2)
      document.getElementById("online3").innerText = texto

  }




  console.log(clientes_ativos)
  console.log(clientes_ativos_tempo)
}