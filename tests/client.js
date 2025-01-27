const { Client } = require('../dist');

const client = new Client(undefined, 0, 1, 2, false, false);
client.on('connect', (netID) => {
    console.log(`Connected to peer with netID ${netID}`);
    client.send(netID, 0, Buffer.from('hello'));
});
client.on('data', (netID, channel, data) => {
    console.log(`Received data: ${data}`);
    client.send(netID, channel, data);
    console.log('echoed data');
});
client.connect("127.0.0.1", 17000);
client.listen();