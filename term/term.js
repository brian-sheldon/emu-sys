
const { SerialPort } = require('serialport');

let args = process.argv.slice( 2 );

let termport = 'COM1';
let baudrate = 115200;
if ( args.length > 0 ) {
  termport = args[0];
}
if ( args.length > 1 ) {
  baudrate = args[1];
}

// --- Configuration ---
const portConfig = {
  path: termport, // Update for your system
  baudRate: baudrate,
  autoOpen: false // Better to control opening manually for reconnection
};

// Initialize serial port
let port;
function connectDevice() {
  port = new SerialPort( portConfig );
  
  port.open((err) => {
    if (err) {
      console.error('Error opening port: ', err.message);
      // Try again in 2 seconds if open fails
      return setTimeout(connectDevice, 2000);
    }
    console.log('Port opened successfully');
  });

  // Handle errors
  port.on('error', (err) => {
    console.error('SerialPort Error: ', err.message);
  });
  
  // Reconnect logic on close
  port.on('close', (err) => {
    console.log('Port closed.');
    if (err && err.disconnected) {
      console.log('Hardware disconnected. Attempting to reconnect...');
    }
    // Schedule a retry
    setTimeout( function() {
      port.open();
    }, 2000);
  });
  
}

function listen() {
  // --- Raw Input Handling ---
  // Set stdin to raw mode to send keys instantly without pressing enter
  process.stdin.setRawMode(true);
  process.stdin.resume();
  process.stdin.setEncoding('utf8');
  
  console.log(`--- Serial Terminal: ${portConfig.path} @ ${portConfig.baudRate} ---`);
  console.log('--- Press Ctrl+D to exit ---');
  
  // Pipe serial data to console
  port.pipe(process.stdout);
  
  // Pipe keyboard input to serial
  process.stdin.on('data', (key) => {
    // Exit application on Ctrl+D
    if (key === '\u0004') {
        process.exit();
    }
    // Write raw key to serial
    port.write(key);
  });
}

connectDevice();
listen();


