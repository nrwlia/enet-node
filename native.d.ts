export declare class Host {
  constructor(ipAddress: string | undefined, port: number, peerLimit: number, channelLimit: number, usingNewPacket: boolean, usingNewPacketForServer: boolean)
  connect(ipAddress: string, port: number): boolean
  disconnect(peerId: number): boolean
  disconnectLater(peerId: number): boolean
  disconnectNow(peerId: number): boolean

  setTimeout(peerId: number, limit: number, min: number, max: number): boolean
  setPingInterval(peerId: number, pingInterval: number): boolean
  send(peerId: number, channelId: number, data: Buffer): boolean
  setEmitter(emitter: (...args: any[]) => any): boolean
  service(): void
}