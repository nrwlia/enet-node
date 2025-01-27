import EventEmitter from "eventemitter3";
import { Host } from "../native";

export class Client extends EventEmitter {
    public host: Host;

    constructor(ip: string | undefined, port: number, maxPeers: number, channelLimit: number, useNewPacketAsClient: boolean, useNewPacketAsServer: boolean) {
        super();

        this.host = new Host(
            ip,
            port,
            maxPeers,
            channelLimit,
            useNewPacketAsClient,
            useNewPacketAsServer
        );
    }

    public on(event: "connect", listener: (netID: number) => void): this;
    public on(event: "data", listener: (netID: number, channelID: number, data: Buffer) => void): this;
    public on(event: "ready", listener: () => void): this;
    public on(event: "error", listener: (error: Error, data?: Buffer) => void): this;
    public on(event: "disconnect", listener: (netID: number) => void): this;
    public on(event: string | symbol, listener: (...args: any[]) => void): this {
        return super.on(event, listener);
    }

    public send(netID: number, channelID: number, data: Buffer) {
        this.host.send(netID, channelID, data);
        return;
    }

    public connect(ipAddress: string, port: number) {
        return this.host.connect(ipAddress, port);
    }

    public listen() {
        try {
            this.host.setEmitter(this.emit.bind(this));

            const acceptPromise = () =>
                new Promise((resolve) => setImmediate(() => resolve(this.host.service())));

            const loop = async () => {
                while (true) await acceptPromise();
            };

            loop();
            this.emit("ready");
        } catch {
            this.emit("error", new Error("Failed to initialize ENet server"));
        }
    }
}