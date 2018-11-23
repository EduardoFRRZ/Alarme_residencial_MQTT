{/* <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.min.js" type="text/javascript"></script> */ }

// Create a client instance


// connect the client
var client;

function onConnect() {
  console.log("conectado");
}
// called when the client connects
function connect() {

  let conexao = document.getElementById('conexao');
  let broker = conexao.host.value;
  let port = conexao.port.value;
  console.log({broker:broker,port:port});

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
  console.log(topico,payload)
  let message = new Paho.MQTT.Message(payload);
  message.destinationName = topico;
  message.qos = Number(2);
  message.retained = true;
  client.send(message);
}

// called when a message arrives
function onMessageArrived(message) {
  console.log("onMessageArrived:" + message.payloadString);
  subscribeAddHtml(message);
}

function unsubscribe() {
  var topic = document.getElementById("subscribeTopicInput").value;
  console.log("INFO", "Unsubscribing: [Topic: ", topic, "]");
  client.unsubscribe(topic, {
    onSuccess: unsubscribeSuccess,
    onFailure: unsubscribeFailure,
    invocationContext: { topic: topic }
  });
}

// called when the client loses its connection
function onConnectionLost(responseObject) {
  if (responseObject.errorCode !== 0)
    console.log("onConnectionLost:" + responseObject.errorMessage);
}

function unsubscribeSuccess(context) {
  console.log("INFO", "Unsubscribed. [Topic: ", context.invocationContext.topic, "]");
}

function unsubscribeFailure(context) {
  console.log("ERROR", "Failed to unsubscribe. [Topic: ", context.invocationContext.topic, ", Error: ", context.errorMessage, "]");
}

function subscribeAddHtml(mensagem) {

  document.getElementById('tabelaSubscribe').innerHTML +=
    ` <tr>
					<th scope="row">${document.getElementById('tabelaSubscribe').rows.length + 1} </th>
					<td>${mensagem.destinationName}</td>
					<td>${mensagem.payloadString}</td>
					<td>${mensagem.qos}</td>
			</tr>`;

}

function adicionarTopicoHtml(topic, qos) {
  document.getElementById('topicosInscritos').innerHTML +=
    ` <tr>
					<th scope="row">${document.getElementById('topicosInscritos').rows.length + 1} </th>
					<td>${topic}</td>
					<td>${qos}</td>
			</tr>`;
}

function salvarConfigAtuais() {
  let conexao = document.getElementById('conexao');
  let host = conexao.host.value;
  let port = conexao.port.value;
  let obj = { host: host, port: port};
  localStorage.setItem('conexao', JSON.stringify(obj));
}

window.onload = () => {
  let conexaoSalva = localStorage.getItem('conexao');
  if (conexaoSalva) {
    conexaoSalva = JSON.parse(conexaoSalva);
    let conexao = document.getElementById('conexao');
    conexao.host.value = conexaoSalva.host + '';
    conexao.port.value = conexaoSalva.port+ '';
  }

  // conectar automaticamente
  connect();
}