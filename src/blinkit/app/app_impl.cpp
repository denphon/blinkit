// -------------------------------------------------
// BlinKit - BlinKit Library
// -------------------------------------------------
//   File Name: app_impl.cpp
// Description: AppImpl Class
//      Author: Ziming Li
//     Created: 2019-03-05
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#include "app_impl.h"

#include "public/web/WebCache.h"
#include "public/web/WebKit.h"

#include "view/view_impl.h"

namespace BlinKit {

static AppImpl *theApp = nullptr;

AppImpl::AppImpl(void)
{
    assert(nullptr == theApp);
    theApp = this;

    blink::PlatformThreadId currentThreadId = ThreadImpl::CurrentThreadId();
    ThreadImpl::ApplyThreadId(currentThreadId);
    m_threads[currentThreadId] = this;
}

AppImpl::~AppImpl(void)
{
    assert(theApp == this);
    theApp = nullptr;
}

BkCrawler* BKAPI AppImpl::CreateCrawler(BkCrawlerClient &client)
{
    assert(false); // BKTODO:
    return nullptr;
}

BkView* BKAPI AppImpl::CreateView(BkViewClient &client)
{
    return ViewImpl::CreateInstance(client);
}

blink::WebThread* AppImpl::currentThread(void)
{
    AutoLock lock(m_lock);
    auto it = m_threads.find(ThreadImpl::CurrentThreadId());
    if (std::end(m_threads) != it)
        return it->second;

    assert(std::end(m_threads) != it);
    return nullptr;
}

void BKAPI AppImpl::Exit(void)
{
    blink::WebCache::clear();
    blink::shutdown();
    delete theApp;
}

AppImpl& AppImpl::Get(void)
{
    return *theApp;
}

void AppImpl::Initialize(BkAppClient *client)
{
    m_client = client;
    blink::initialize(this);
}

} // namespace BlinKit

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace BlinKit;

extern "C" BkApp* BKAPI BkGetApp(void)
{
    return theApp;
}

extern "C" BkApp* BKAPI BkInitialize(BkAppClient *client)
{
    assert(nullptr == theApp);
    if (nullptr == theApp)
    {
        AppImpl::CreateInstance();
        assert(nullptr != theApp);
        theApp->Initialize(client);
    }
    return theApp;
}