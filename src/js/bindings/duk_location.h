// -------------------------------------------------
// BlinKit - js Library
// -------------------------------------------------
//   File Name: duk_location.h
// Description: Bindings for Location
//      Author: Ziming Li
//     Created: 2019-06-11
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#ifndef BLINKIT_JS_DUK_LOCATION_H
#define BLINKIT_JS_DUK_LOCATION_H

#pragma once

#include "core/frame/Location.h"
#include "duk_object_impl.hpp"

namespace BlinKit {

class PrototypeManager;

class DukLocation final : public DukObjectImpl<blink::Location>
{
    friend class DukContext;
public:
    static const char ProtoName[];

    static void RegisterPrototypeForCrawler(duk_context *ctx, PrototypeManager &protos);
private:
    static duk_ret_t Finalizer(duk_context *ctx);
};

} // namespace BlinKit

#endif // BLINKIT_JS_DUK_LOCATION_H