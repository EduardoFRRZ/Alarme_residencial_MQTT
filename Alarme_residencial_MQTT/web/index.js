{/* <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script> */ }

// Create a client instance


// connect the client
var client;

function onConnect() {
  console.log("conectado");
  client.subscribe('unoesc/led_1', { qos: Number(2) });
  client.subscribe('unoesc/trancar_1', { qos: Number(2) });
  client.subscribe('unoesc/led_2', { qos: Number(2) });
  client.subscribe('unoesc/trancar_2', { qos: Number(2) });
  client.subscribe('unoesc/led_3', { qos: Number(2) });
  client.subscribe('unoesc/trancar_3', { qos: Number(2) });
  
}
// called when the client connects
function connect() {

  let conexao = document.getElementById('conexao');
  let broker = conexao.host.value;
  let port = conexao.port.value;
  console.log({ broker: broker, port: port });

  client = new Paho.MQTT.Client(broker, Number(port), "Arduino##@!W");
  client.connect({ onSuccess: onConnect });
  client.onConnectionLost = onConnectionLost;
  client.onMessageArrived = onMessageArrived;


}

function subscribe() {
  let topic = document.getElementById('escutar').topico.value;

  var qos = Number(2);
  console.log("INFO", "Subscribing to: [Topic: ", topic, ", QoS: ", qos, "]");

  client.subscribe(topic, { qos: Number(qos) });

  adicionarTopicoHtml(topic, qos);
}

function publish(topico, payload) {
  console.log(topico, payload)
  let message = new Paho.MQTT.Message(payload);
  message.destinationName = topico;
  message.qos = Number(2);
  message.retained = true;
  client.send(message);
}


function alterar_badge(elementoId, payload) {
  let span = document.getElementById(elementoId)
  if (payload == 0)
    span.className = "badge badge-pill badge-light"
  else
    span.className = "badge badge-pill badge-success"
}
// called when a message arrives
function onMessageArrived(message) {

  if (message.destinationName == "unoesc/led_1")
    alterar_badge("spanled1", message.payloadString)

  if (message.destinationName == "unoesc/trancar_1")
    alterar_badge("spantranca1", message.payloadString)

  if (message.destinationName == "unoesc/led_2")
    alterar_badge("spanled2", message.payloadString)

  if (message.destinationName == "unoesc/trancar_2")
    alterar_badge("spantranca2", message.payloadString)

  if (message.destinationName == "unoesc/led_3")
    alterar_badge("spanled3", message.payloadString)

  if (message.destinationName == "unoesc/trancar_3")
    alterar_badge("spantranca3", message.payloadString)

}

// called when the client loses its connection
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
}