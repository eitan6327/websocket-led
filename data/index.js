/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton()
}

let connection = "CLOSED";
function onOpen(Event) {
    console.log('Connection opened');
    connection = "OPEN";
}

function onClose(Event) {
    console.log('Connection Closed');
    connection = "CLOSED";
    setTimeout(initWebSocket, 2000);
}

function initButton() {
    document.getElementById('toggle').addEventListener('click', onToggle);
}

function onToggle(event) {
    // websocket.send('toggle');
    websocket.send(JSON.stringify({ 'action': 'toggle' }));
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket() {
    websocket = new WebSocket(gateway);
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage; // <-- add this line
    websocket.on
}

var ping_pong_times = [];
var start_time;

function onMessage(event) {
    let data = JSON.parse(event.data);
    let keys = Object.keys(data);
    if (keys.includes('status')) {
        document.getElementById('led').className = data.status;
    }
    if (keys.includes('connection')) {
        var latency = (new Date).getTime() - start_time;
        document.getElementById('ping-pong').innerHTML = latency;
    }
    
}
// Interval function that tests message latency by sending a "ping"
// message. The server then responds with a "pong" message and the
// round trip time is measured.

window.setInterval(function () {
    start_time = (new Date).getTime();
    if (connection == "OPEN")
    websocket.send(JSON.stringify({ 'connection': 'ping' }));
    else console.log('connection closed')
    // socket.emit('my_ping');
}, 1000);

// Handler for the "pong" message. When the pong is received, the
// time from the ping is stored, and the average of the last 30
// samples is average and displayed.


// socket.on('my_pong', function () {
//     var latency = (new Date).getTime() - start_time;
//     ping_pong_times.push(latency);
//     ping_pong_times = ping_pong_times.slice(-30); // keep last 30 samples
//     var sum = 0;
//     for (var i = 0; i < ping_pong_times.length; i++)
//         sum += ping_pong_times[i];
//     $('#ping-pong').text(Math.round(10 * sum / ping_pong_times.length) / 10);
// });
