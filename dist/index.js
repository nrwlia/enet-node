"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.Client = void 0;
const eventemitter3_1 = __importDefault(require("eventemitter3"));
const native_1 = require("../native");
class Client extends eventemitter3_1.default {
    host;
    constructor(ip, port, maxPeers, channelLimit, useNewPacketAsClient, useNewPacketAsServer) {
        super();
        this.host = new native_1.Host(ip, port, maxPeers, channelLimit, useNewPacketAsClient, useNewPacketAsServer);
    }
    on(event, listener) {
        return super.on(event, listener);
    }
    send(netID, channelID, data) {
        this.host.send(netID, channelID, data);
        return;
    }
    connect(ipAddress, port) {
        return this.host.connect(ipAddress, port);
    }
    listen() {
        try {
            this.host.setEmitter(this.emit.bind(this));
            const acceptPromise = () => new Promise((resolve) => setImmediate(() => resolve(this.host.service())));
            const loop = async () => {
                while (true)
                    await acceptPromise();
            };
            loop();
            this.emit("ready");
        }
        catch {
            this.emit("error", new Error("Failed to initialize ENet server"));
        }
    }
}
exports.Client = Client;
