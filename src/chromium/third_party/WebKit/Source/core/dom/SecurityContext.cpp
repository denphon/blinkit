// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: SecurityContext.cpp
// Description: SecurityContext Class
//      Author: Ziming Li
//     Created: 2019-02-06
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2011 Google Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "core/dom/SecurityContext.h"

#include "core/frame/csp/ContentSecurityPolicy.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

SecurityContext::SecurityContext()
    : m_haveInitializedSecurityOrigin(false)
    , m_hostedInReservedIPRange(false)
    , m_insecureRequestsPolicy(InsecureRequestsDoNotUpgrade)
    , m_enforceStrictMixedContentChecking(false)
{
}

SecurityContext::~SecurityContext()
{
}

DEFINE_TRACE(SecurityContext)
{
    visitor->trace(m_contentSecurityPolicy);
}

void SecurityContext::setSecurityOrigin(PassRefPtr<SecurityOrigin> securityOrigin)
{
    m_securityOrigin = securityOrigin;
    m_haveInitializedSecurityOrigin = true;
}

void SecurityContext::setContentSecurityPolicy(PassRefPtrWillBeRawPtr<ContentSecurityPolicy> contentSecurityPolicy)
{
    m_contentSecurityPolicy = contentSecurityPolicy;
}

bool SecurityContext::isSecureTransitionTo(const KURL& url) const
{
    // If we haven't initialized our security origin by now, this is probably
    // a new window created via the API (i.e., that lacks an origin and lacks
    // a place to inherit the origin from).
    if (!haveInitializedSecurityOrigin())
        return true;

    RefPtr<SecurityOrigin> other = SecurityOrigin::create(url);
    return securityOrigin()->canAccess(other.get());
}

}
