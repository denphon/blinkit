// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: RuntimeEnabledFeatures.h
// Description: RuntimeEnabledFeatures Class
//      Author: Ziming Li
//     Created: 2019-01-22
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#ifndef BLINKIT_BLINK_RUNTIME_ENABLED_FEATURES_H
#define BLINKIT_BLINK_RUNTIME_ENABLED_FEATURES_H

#pragma once

#include "wtf/Allocator.h"

#define BLINKIT_ENABLE_FEATURE(name)    static bool name ## Enabled(void) { return true;  }
#define BLINKIT_DISABLE_FEATURE(name)   static bool name ## Enabled(void) { return false; }

#define BLINK_DEFINE_STABLE_FEATURE BLINKIT_ENABLE_FEATURE

/**
 * Experimental & test features are disabled by default.
 * To enable these features, change the macros below to BLINKIT_ENABLE_FEATURE.
 */
#define BLINK_DEFINE_EXPERIMENTAL_FEATURE   BLINKIT_DISABLE_FEATURE
#define BLINK_DEFINE_TEST_FEATURE           BLINKIT_DISABLE_FEATURE

class RuntimeEnabledFeatures final {
    STATIC_ONLY(RuntimeEnabledFeatures);
public:
    BLINK_DEFINE_STABLE_FEATURE(eventListenerOptions)
    BLINK_DEFINE_STABLE_FEATURE(linkPreconnect)
    BLINK_DEFINE_STABLE_FEATURE(smil)

    BLINKIT_DISABLE_FEATURE(media)
    BLINKIT_DISABLE_FEATURE(xslt)

    BLINK_DEFINE_EXPERIMENTAL_FEATURE(contextMenu)
    BLINK_DEFINE_EXPERIMENTAL_FEATURE(fractionalScrollOffsets)
    BLINK_DEFINE_EXPERIMENTAL_FEATURE(linkPreload)
    BLINK_DEFINE_EXPERIMENTAL_FEATURE(referrerPolicyAttribute)
    BLINK_DEFINE_EXPERIMENTAL_FEATURE(scrollTopLeftInterop)
    BLINK_DEFINE_EXPERIMENTAL_FEATURE(shadowDOMV1)
    BLINK_DEFINE_EXPERIMENTAL_FEATURE(suborigins)

    BLINKIT_DISABLE_FEATURE(authorShadowDOMForAnyElement)
    BLINKIT_DISABLE_FEATURE(langAttributeAwareFormControlUI)
    BLINKIT_DISABLE_FEATURE(mediaCapture)
    BLINKIT_DISABLE_FEATURE(orientationEvent)
    BLINKIT_DISABLE_FEATURE(pointerEvent)
    BLINKIT_DISABLE_FEATURE(scrollCustomization)
    BLINKIT_DISABLE_FEATURE(slimmingPaintOffsetCaching)
    BLINKIT_DISABLE_FEATURE(slimmingPaintV2)
    BLINKIT_DISABLE_FEATURE(threadedParserDataReceiver)
    BLINKIT_DISABLE_FEATURE(trustedEventsDefaultAction)
};

#endif // BLINKIT_BLINK_RUNTIME_ENABLED_FEATURES_H
