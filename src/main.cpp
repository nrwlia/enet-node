#include <napi.h>
#include "host.hpp"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  enet_initialize();
  exports.Set(Napi::String::New(env, "Host"), Host::GetClass(env));
  return exports;
}

NODE_API_MODULE("enet-node", Init)