// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: event.h
// Description: Event Class
//      Author: Ziming Li
//     Created: 2019-10-20
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights
 * reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef BLINKIT_BLINK_EVENT_H
#define BLINKIT_BLINK_EVENT_H

#pragma once

#include "third_party/blink/renderer/core/dom/events/event_dispatch_result.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

namespace blink {

class EventDispatcher;
class EventPath;
class EventTarget;

class Event : public ScriptWrappable
{
public:
    enum class Bubbles {
        kYes,
        kNo,
    };
    enum class Cancelable {
        kYes,
        kNo,
    };
    enum PhaseType {
        kNone = 0,
        kCapturingPhase = 1,
        kAtTarget = 2,
        kBubblingPhase = 3
    };
    enum class ComposedMode {
        kComposed,
        kScoped,
    };

    static Event* CreateBubble(const AtomicString &type)
    {
        return new Event(type, Bubbles::kYes, Cancelable::kNo);
    }
    ~Event(void) override;

    const AtomicString& type(void) const { return m_type; }
    void SetType(const AtomicString &type) { m_type = type; }

    EventTarget* target(void) const { return m_target.Get(); }
    void SetTarget(EventTarget *target);

    EventTarget* currentTarget(void) const { return m_currentTarget.Get(); }
    void SetCurrentTarget(EventTarget *currentTarget)
    {
        m_currentTarget = currentTarget;
    }

    void SetRelatedTargetIfExists(EventTarget *relatedTarget);

    bool bubbles(void) const { return m_bubbles; }

    bool defaultPrevented(void) const { return m_defaultPrevented; }

    bool DefaultHandled(void) const { return m_defaultHandled; }

    bool isTrusted(void) const { return m_isTrusted; }
    void SetTrusted(bool value) { m_isTrusted = value; }
    
    unsigned short eventPhase(void) const { return m_eventPhase; }
    void SetEventPhase(unsigned short eventPhase) { m_eventPhase = eventPhase; }

    bool PropagationStopped(void) const { return m_propagationStopped || m_immediatePropagationStopped; }
    void SetStopPropagation(bool stopPropagation) { m_propagationStopped = stopPropagation; }
    void SetStopImmediatePropagation(bool stopImmediatePropagation) { m_immediatePropagationStopped = stopImmediatePropagation; }

    bool HasEventPath(void) { return !!m_eventPath; }
    EventPath& GetEventPath(void)
    {
        ASSERT(m_eventPath);
        return *m_eventPath;
    }
    void InitEventPath(Node &node);

    virtual DispatchEventResult DispatchEvent(EventDispatcher &dispatcher);
protected:
    Event(const AtomicString &eventType, Bubbles bubbles, Cancelable cancelable, ComposedMode composedMode, TimeTicks platformTimeStamp);
    Event(const AtomicString &eventType, Bubbles bubbles, Cancelable cancelable, ComposedMode composedMode = ComposedMode::kScoped);

    virtual void ReceivedTarget(void) {}
private:
    AtomicString m_type;
    unsigned m_bubbles : 1;
    unsigned m_cancelable : 1;
    unsigned m_composed : 1;
    unsigned m_propagationStopped : 1;
    unsigned m_immediatePropagationStopped : 1;
    unsigned m_defaultPrevented : 1;
    unsigned m_defaultHandled : 1;
    unsigned m_isTrusted : 1;
    unsigned short m_eventPhase = 0;
    Member<EventTarget> m_target, m_currentTarget;
    std::unique_ptr<EventPath> m_eventPath;
    // The monotonic platform time in seconds, for input events it is the
    // event timestamp provided by the host OS and reported in the original
    // WebInputEvent instance.
    TimeTicks m_platformTimeStamp;
};

#define DEFINE_EVENT_TYPE_CASTS(typeName)                               \
    DEFINE_TYPE_CASTS(typeName, Event, event, event->Is##typeName(),    \
        event.Is##typeName())

}  // namespace blink

#endif // BLINKIT_BLINK_EVENT_H