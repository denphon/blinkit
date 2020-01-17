// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: id_target_observer_registry.h
// Description: IdTargetObserverRegistry Class
//      Author: Ziming Li
//     Created: 2020-01-12
// -------------------------------------------------
// Copyright (C) 2020 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2012 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BLINKIT_BLINK_ID_TARGET_OBSERVER_REGISTRY_H
#define BLINKIT_BLINK_ID_TARGET_OBSERVER_REGISTRY_H

#pragma once

#include <unordered_set>
#include <unordered_map>
#include "base/macros.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {

class IdTargetObserver;

class IdTargetObserverRegistry final : public GarbageCollected<IdTargetObserverRegistry>
{
public:
    static std::unique_ptr<IdTargetObserverRegistry> Create(void);

    void NotifyObservers(const AtomicString &id);
private:
    IdTargetObserverRegistry(void) = default;

    void NotifyObserversInternal(const AtomicString &id);

    typedef std::unordered_set<Member<IdTargetObserver>> ObserverSet;
    typedef std::unordered_map<StringImpl *, Member<ObserverSet>> IdToObserverSetMap;
    IdToObserverSetMap m_registry;
    Member<ObserverSet> m_notifyingObserversInSet;
    DISALLOW_COPY_AND_ASSIGN(IdTargetObserverRegistry);
};

} // namespace blink

#endif // BLINKIT_BLINK_ID_TARGET_OBSERVER_REGISTRY_H
