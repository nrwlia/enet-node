#include "host.hpp"
#include <thread>

#define RET_ERR(env, msg)                                            \
    {                                                                \
        Napi::TypeError::New(env, msg).ThrowAsJavaScriptException(); \
        return Napi::Boolean::New(env, false);                       \
    }
#define CHECK_PARAM_LEN(env, len) \
    if (info.Length() < len)      \
    RET_ERR(env, "Not enough parameters")

Host::Host(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Host>(info)
{
    // parameters (address, port, peer count, channel count, using new packet, using new packet for server)
    Napi::Env env = info.Env();

    if (info.Length() < 6)
    {
        Napi::TypeError::New(env, "Not enough parameters").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Port must be a number").ThrowAsJavaScriptException();
        return;
    }
    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Peer count must be a number").ThrowAsJavaScriptException();
        return;
    }
    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Channel count must be a number").ThrowAsJavaScriptException();
        return;
    }
    if (!info[4].IsBoolean())
    {
        Napi::TypeError::New(env, "Using new packet must be a boolean").ThrowAsJavaScriptException();
        return;
    }
    if (!info[5].IsBoolean())
    {
        Napi::TypeError::New(env, "Using new packet for server must be a boolean").ThrowAsJavaScriptException();
        return;
    }

    port = info[1].As<Napi::Number>().Int32Value();
    peerCount = info[2].As<Napi::Number>().Int32Value();
    channelCount = info[3].As<Napi::Number>().Int32Value();
    usingNewPacket = info[4].As<Napi::Boolean>().Value();
    usingNewPacketForServer = info[5].As<Napi::Boolean>().Value();

    if (info[0].IsString()) // Expect host
    {
        std::string str_addr = info[0].As<Napi::String>().Utf8Value();
        address = ENetAddress();
        enet_address_set_host(&address, str_addr.c_str());
        address.port = port;

        host = enet_host_create(&address, peerCount, channelCount, 0, 0);
    }
    else if (info[0].IsUndefined()) // Expect client
        host = enet_host_create(NULL, peerCount, channelCount, 0, 0);
    if (host == NULL)
    {
        Napi::TypeError::New(env, "Failed to create ENetHost").ThrowAsJavaScriptException();
        while (true)
            ; // force crash
    }

    host->checksum = enet_crc32;
    enet_host_compress_with_range_coder(host);

    host->usingNewPacket = usingNewPacket;
    host->usingNewPacketForServer = usingNewPacketForServer;
}
Host::~Host()
{
    enet_host_destroy(host);
    delete this;
}

Napi::Value Host::Connect(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 2);
    if (!info[0].IsString())
        RET_ERR(env, "Address must be a string");
    if (!info[1].IsNumber())
        RET_ERR(env, "Port must be a number");

    std::string str_addr = info[0].As<Napi::String>().Utf8Value();
    port = info[1].As<Napi::Number>().Int32Value();

    ENetAddress addr;
    enet_address_set_host(&addr, str_addr.c_str());
    addr.port = port;

    ENetPeer *peer = enet_host_connect(host, &addr, channelCount, 0);
    if (peer == NULL)
        RET_ERR(env, "Failed to connect to peer");

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::Disconnect(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 1);
    if (!info[0].IsNumber())
        RET_ERR(env, "Peer ID must be a number");

    size_t peerID = info[0].As<Napi::Number>().Int32Value();
    ENetPeer *peer = &host->peers[peerID];

    enet_peer_disconnect(peer, 0);

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::DisconnectLater(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 1);
    if (!info[0].IsNumber())
        RET_ERR(env, "Peer ID must be a number");

    size_t peerID = info[0].As<Napi::Number>().Int32Value();
    ENetPeer *peer = &host->peers[peerID];

    enet_peer_disconnect_later(peer, 0);

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::DisconnectNow(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 1);
    if (!info[0].IsNumber())
        RET_ERR(env, "Peer ID must be a number");

    size_t peerID = info[0].As<Napi::Number>().Int32Value();
    ENetPeer *peer = &host->peers[peerID];

    enet_peer_disconnect_now(peer, 0);

    return Napi::Boolean::New(env, true);
}

Napi::Value Host::SetTimeout(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 4);
    if (!info[0].IsNumber())
        RET_ERR(env, "Peer ID must be a number");
    if (!info[1].IsNumber())
        RET_ERR(env, "Timeout limit must be a number");
    if (!info[2].IsNumber())
        RET_ERR(env, "Timeout minimum must be a number");
    if (!info[3].IsNumber())
        RET_ERR(env, "Timeout maximum must be a number");

    size_t peerID = info[0].As<Napi::Number>().Int32Value();
    ENetPeer *peer = &host->peers[peerID];

    int limit = info[1].As<Napi::Number>().Int32Value();
    int min = info[2].As<Napi::Number>().Int32Value();
    int max = info[3].As<Napi::Number>().Int32Value();

    enet_peer_timeout(peer, limit, min, max);

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::SetPingInterval(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 2);
    if (!info[0].IsNumber())
        RET_ERR(env, "Peer ID must be a number");
    if (!info[1].IsNumber())
        RET_ERR(env, "Ping interval must be a number");

    size_t peerID = info[0].As<Napi::Number>().Int32Value();
    ENetPeer *peer = &host->peers[peerID];

    int interval = info[1].As<Napi::Number>().Int32Value();
    enet_peer_ping_interval(peer, interval);

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::Send(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 3);
    if (!info[0].IsNumber())
        RET_ERR(env, "Peer ID must be a number");
    if (!info[1].IsNumber())
        RET_ERR(env, "Channel ID must be a number");
    if (!info[2].IsBuffer())
        RET_ERR(env, "Data must be a buffer");

    size_t peerID = info[0].As<Napi::Number>().Int32Value();
    ENetPeer *peer = &host->peers[peerID];

    size_t channelID = info[1].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> buffer = info[2].As<Napi::Buffer<uint8_t>>();

    ENetPacket *packet = enet_packet_create(buffer.Data(), buffer.Length(), usingNewPacket ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(peer, channelID, packet);

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::SetEmitter(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    CHECK_PARAM_LEN(env, 1);
    if (!info[0].IsFunction())
        RET_ERR(env, "Emitter must be a function");

    emit = Napi::Persistent(info[0].As<Napi::Function>());

    return Napi::Boolean::New(env, true);
}
Napi::Value Host::Service(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (emit.IsEmpty())
        RET_ERR(env, "Emitter is not set");

    Napi::Function callback = emit.Value();

    ENetEvent event;
    int ret = enet_host_service(host, &event, 0);

    if (ret > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            callback.Call({Napi::String::New(env, "connect"),
                           Napi::Number::New(env, event.peer->incomingPeerID)});
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            callback.Call({Napi::String::New(env, "data"),
                           Napi::Number::New(env, event.peer->incomingPeerID),
                           Napi::Number::New(env, event.channelID),
                           Napi::Buffer<uint8_t>::New(env, event.packet->data, event.packet->dataLength)});
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            callback.Call({Napi::String::New(env, "disconnect"),
                           Napi::Number::New(env, event.peer->incomingPeerID)});
            break;
        }
        default:
            break;
        }
    }
    else if (ret < 0)
        RET_ERR(env, "Failed to service host");

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return env.Null();
}
