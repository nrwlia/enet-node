import EventEmitter from "eventemitter3";
import { Host } from "../native";
export declare class Client extends EventEmitter {
    host: Host;
    constructor(ip: string | undefined, port: number, maxPeers: number, channelLimit: number, useNewPacketAsClient: boolean, useNewPacketAsServer: boolean);
    on(event: "connect", listener: (netID: number) => void): this;
    on(event: "data", listener: (netID: number, channelID: number, data: Buffer) => void): this;
    on(event: "ready", listener: () => void): this;
    on(event: "error", listener: (error: Error, data?: Buffer) => void): this;
    on(event: "disconnect", listener: (netID: number) => void): this;
    send(netID: number, channelID: number, data: Buffer): void;
    connect(ipAddress: string, port: number): boolean;
    listen(): void;
}
