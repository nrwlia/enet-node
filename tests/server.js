const { Client } = require('../dist');

const host = new Client("0.0.0.0", 17000, 16, 2, false, false);
host.on('connect', (netID) => {
    console.log(`Connected to peer with netID ${netID}`);
});
host.on('data', (netID, channel, data) => {
    console.log(`Received data: ${data}`);
    host.send(netID, channel, data);
    console.log('echoed data');
});
host.on('ready', () => {
    console.log('Server is ready');
});
host.listen();