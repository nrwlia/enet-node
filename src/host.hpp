#pragma once
#include <napi.h>
#include <vector>
#include <enet/enet.h>

class Host : public Napi::ObjectWrap<Host>
{
public:
    Host(const Napi::CallbackInfo &info);
    ~Host();

    Napi::Value Connect(const Napi::CallbackInfo &info);
    Napi::Value Disconnect(const Napi::CallbackInfo &info);
    Napi::Value DisconnectLater(const Napi::CallbackInfo &info);
    Napi::Value DisconnectNow(const Napi::CallbackInfo &info);

    Napi::Value SetTimeout(const Napi::CallbackInfo &info);
    Napi::Value SetPingInterval(const Napi::CallbackInfo &info);
    Napi::Value Send(const Napi::CallbackInfo &info);
    Napi::Value SetEmitter(const Napi::CallbackInfo &info);
    Napi::Value Service(const Napi::CallbackInfo &info);

    static Napi::Function GetClass(Napi::Env env)
    {
        return DefineClass(env, "Host",
                           {ObjectWrap::InstanceMethod("connect", &Host::Connect),
                            ObjectWrap::InstanceMethod("disconnect", &Host::Disconnect),
                            ObjectWrap::InstanceMethod("disconnectLater", &Host::DisconnectLater),
                            ObjectWrap::InstanceMethod("disconnectNow", &Host::DisconnectNow),
                            ObjectWrap::InstanceMethod("setTimeout", &Host::SetTimeout),
                            ObjectWrap::InstanceMethod("setPingInterval", &Host::SetPingInterval),
                            ObjectWrap::InstanceMethod("send", &Host::Send),
                            ObjectWrap::InstanceMethod("setEmitter", &Host::SetEmitter),
                            ObjectWrap::InstanceMethod("service", &Host::Service)});
    }

private:
    size_t peerCount;
    size_t channelCount;
    short port;
    bool usingNewPacket;
    bool usingNewPacketForServer;
    ENetAddress address;
    ENetHost *host;
    Napi::FunctionReference emit;
};