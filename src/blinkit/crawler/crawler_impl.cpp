// -------------------------------------------------
// BlinKit - BlinKit Library
// -------------------------------------------------
//   File Name: crawler_impl.cpp
// Description: CrawlerImpl Class
//      Author: Ziming Li
//     Created: 2019-03-20
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#include "crawler_impl.h"

#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoadRequest.h"

#include "app/app_impl.h"
#include "blink_impl/cookie_jar_impl.h"

#include "js/public/script_controller.h"

using namespace blink;

namespace BlinKit {

CrawlerImpl::CrawlerImpl(BkCrawlerClient &client, const std::string_view &script)
    : m_client(client), m_frame(LocalFrame::create(this, nullptr))
{
    m_frame->init();
    m_frame->script().CreateCrawlerObject(script.data(), script.length());
}

CrawlerImpl::~CrawlerImpl(void)
{
    m_frame->detach(FrameDetachType::Remove);
}

int BKAPI CrawlerImpl::AccessCrawlerMember(const char *name, Accessor accessor, void *userData)
{
    return m_frame->script().AccessCrawlerMember(name, accessor, userData);
}

int BKAPI CrawlerImpl::CallCrawler(const char *method, BkCallerContext::Callback callback, void *userData)
{
    return m_frame->script().CallCrawler(method, callback, userData);
}

int BKAPI CrawlerImpl::CallFunction(const char *name, BkCallerContext::Callback callback, void *userData)
{
    return m_frame->script().CallFunction(name, callback, userData);
}

void CrawlerImpl::dispatchDidFinishLoad(void)
{
    m_client.DocumentReady(this);
}

std::string CrawlerImpl::GetCookie(const std::string &URL) const
{
    std::string ret;
    m_client.GetCookie(URL.c_str(), BkMakeBuffer(ret).Wrap());
    if (ret.empty())
        ret = AppImpl::Get().CookieJar().GetCookie(URL);
    return ret;
}

int BKAPI CrawlerImpl::Load(const char *URL)
{
    KURL u(ParsedURLString, URL);
    if (!u.protocolIsInHTTPFamily())
        return BkError::URIError;

    FrameLoadRequest request(nullptr, ResourceRequest(u));
    m_frame->loader().load(request);
    return BkError::Success;
}

int BKAPI CrawlerImpl::RegisterCrawlerFunction(const char *name, BkFunction *functionImpl)
{
    return m_frame->script().RegisterFunction(name, functionImpl);
}

String CrawlerImpl::userAgent(void)
{
    std::string userAgent = m_frame->script().GetCrawlerProperty("userAgent");
    if (!userAgent.empty())
        return String::fromUTF8(userAgent.data(), userAgent.length());
    return FrameLoaderClientImpl::userAgent();
}

} // namespace BlinKit
