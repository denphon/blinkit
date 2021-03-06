// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: use_counter.h
// Description: UseCounter Class
//      Author: Ziming Li
//     Created: 2019-10-30
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#ifndef BLINKIT_BLINK_USE_COUNTER_H
#define BLINKIT_BLINK_USE_COUNTER_H

#pragma once

#include "third_party/blink/renderer/core/frame/web_feature.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"

namespace blink {

class Document;

class UseCounter
{
    STATIC_ONLY(UseCounter);
public:
    static void Count(Document &, WebFeature) {}
};

} // namespace blink

#endif // BLINKIT_BLINK_USE_COUNTER_H
