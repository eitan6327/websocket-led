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

 function onOpen(Event) {
     console.log('Connection opened');
 }

 function onClose(Event) {
     console.log('Connection Closed');
     setTimeout(initWebSocket, 2000);
 }
 
function initButton() {
    document.getElementById('toggle').addEventListener('click', onToggle);
}

function onToggle(event) {
    // websocket.send('toggle');
    websocket.send(JSON.stringify({'action': 'toggle'}));
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
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
}

function onMessage(event) {
    // console.log(`Received a notification from ${event.origin}`);
    // console.log(event);
    // document.getElementById('led').className = event.data;

    let data = JSON.parse(event.data);
    document.getElementById('led').className = data.status;
}