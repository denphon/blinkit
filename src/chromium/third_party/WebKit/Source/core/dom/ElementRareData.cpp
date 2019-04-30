// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: ElementRareData.cpp
// Description: ElementRareData Class
//      Author: Ziming Li
//     Created: 2019-02-05
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/dom/ElementRareData.h"
#include "core/style/ComputedStyle.h"

namespace blink {

struct SameSizeAsElementRareData : NodeRareData {
#ifdef BLINKIT_CRAWLER_ONLY
    void* pointers[9];
    PersistentWillBeMember<void*> persistentMember[1];
#else
    short indices[1];
    LayoutSize sizeForResizing;
    IntSize scrollOffset;
    void* pointers[12];
    PersistentWillBeMember<void*> persistentMember[2];
#endif
};

#ifndef BLINKIT_CRAWLER_ONLY
CSSStyleDeclaration& ElementRareData::ensureInlineCSSStyleDeclaration(Element* ownerElement)
{
    if (!m_cssomWrapper)
        m_cssomWrapper = adoptPtrWillBeNoop(new InlineCSSStyleDeclaration(ownerElement));
    return *m_cssomWrapper;
}
#endif

AttrNodeList& ElementRareData::ensureAttrNodeList()
{
    if (!m_attrNodeList)
        m_attrNodeList = adoptPtrWillBeNoop(new AttrNodeList);
    return *m_attrNodeList;
}

DEFINE_TRACE_AFTER_DISPATCH(ElementRareData)
{
    visitor->trace(m_dataset);
    visitor->trace(m_classList);
#ifndef BLINKIT_CRAWLER_ONLY
    visitor->trace(m_shadow);
#endif
    visitor->trace(m_attributeMap);
#if ENABLE(OILPAN)
    visitor->trace(m_attrNodeList);
#endif
#ifndef BLINKIT_CRAWLER_ONLY
    visitor->trace(m_elementAnimations);
    visitor->trace(m_cssomWrapper);
#endif
    visitor->trace(m_customElementDefinition);
    visitor->trace(m_generatedBefore);
    visitor->trace(m_generatedAfter);
    visitor->trace(m_generatedFirstLetter);
    visitor->trace(m_backdrop);
    visitor->trace(m_intersectionObserverData);
    NodeRareData::traceAfterDispatch(visitor);
}

static_assert(sizeof(ElementRareData) == sizeof(SameSizeAsElementRareData), "ElementRareData should stay small");

} // namespace blink
