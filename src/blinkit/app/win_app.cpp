// -------------------------------------------------
// BlinKit - BlinKit Library
// -------------------------------------------------
//   File Name: win_app.cpp
// Description: WinApp Class
//      Author: Ziming Li
//     Created: 2019-03-05
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#include "win_app.h"

#include "base/strings/sys_string_conversions.h"
#include "blinkit/blink_impl/win_single_thread_task_runner.h"
#include "third_party/blink/renderer/platform/wtf/wtf.h"
#if 0 // BKTODO:
#include "base/win/resource_util.h"

#include "blink_impl/win_task_runner.h"
#ifndef BLINKIT_CRAWLER_ONLY
#   include "blink_impl/win_clipboard.h"
#   include "blink_impl/win_theme_engine.h"
#   include "view/win_view.h"
#endif
#endif // 0

namespace BlinKit {

struct BackgoundThreadData
{
    BackgoundThreadData(void) : hEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr))
    {
    }
    ~BackgoundThreadData(void)
    {
        CloseHandle(hEvent);
    }

    HANDLE hEvent;
    HANDLE hThread = nullptr;
};

WinApp::WinApp(HANDLE hBackgroundThread)
    : m_appThreadId(GetCurrentThreadId())
    , m_taskRunner(std::make_shared<WinSingleThreadTaskRunner>())
    , m_backgroundThread(hBackgroundThread)
{
    m_msgHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, nullptr, m_appThreadId);
}

WinApp::~WinApp(void)
{
    UnhookWindowsHookEx(m_msgHook);
    if (nullptr != m_backgroundThread)
        CloseHandle(m_backgroundThread);
}

DWORD WINAPI WinApp::BackgroundThread(PVOID param)
{
    BackgoundThreadData *data = reinterpret_cast<BackgoundThreadData *>(param);

    WinApp *app = new WinApp(data->hThread);
    if (nullptr == app)
    {
        ASSERT(nullptr != app);
        CloseHandle(data->hThread);
        SetEvent(data->hEvent);
        return EXIT_FAILURE;
    }

    app->Initialize(nullptr);
    IsGUIThread(TRUE);
    SetEvent(data->hEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete app;
    return msg.wParam;
}

WTF::String WinApp::DefaultLocale(void)
{
    std::string localName("en-US");

    int len = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, nullptr, 0);
    if (len > 0)
    {
        localName.resize(len - 1);
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, const_cast<PSTR>(localName.c_str()), len);
    }

    return WTF::String::FromStdUTF8(localName);
}

void WinApp::FinalizeInBackground(void)
{
    ASSERT(nullptr != m_backgroundThread);
    ASSERT(GetCurrentThreadId() != m_appThreadId);

    PostThreadMessage(m_appThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(m_backgroundThread, INFINITE);
}

WinApp& WinApp::Get(void)
{
    return static_cast<WinApp &>(AppImpl::Get());
}

std::shared_ptr<base::SingleThreadTaskRunner> WinApp::GetTaskRunner(void) const
{
    return m_taskRunner;
}

LRESULT CALLBACK WinApp::HookProc(int code, WPARAM w, LPARAM l)
{
    WinApp &app = Get();
    do {
        if (code < 0 || PM_REMOVE != w)
            break;

        LPMSG msg = reinterpret_cast<LPMSG>(l);
        app.m_taskRunner->ProcessMessage(*msg);
    } while (false);
    return CallNextHookEx(app.m_msgHook, code, w, l);
}

#if 0 // BKTODO:
#ifndef BLINKIT_CRAWLER_ONLY

blink::WebClipboard* WinApp::clipboard(void)
{
    if (!m_clipboard)
    {
        AutoLock lock(m_lock);
        if (!m_clipboard)
            m_clipboard = std::make_unique<WinClipboard>();
    }
    return m_clipboard.get();
}

BkView* BKAPI WinApp::CreateView(BkViewClient &client)
{
    return new WinView(client);
}

blink::WebData WinApp::loadResource(const char *name)
{
    void *data;
    size_t dataLength;
    if (base::GetResourceFromModule(theModule, name, "RES", &data, &dataLength))
        return blink::WebData(reinterpret_cast<const char *>(data), dataLength);

    assert(false); // Failed loading resource!
    return blink::WebData();
}

blink::WebThemeEngine* WinApp::themeEngine(void)
{
    if (!m_themeEngine)
    {
        AutoLock lock(m_lock);
        if (!m_themeEngine)
            m_themeEngine = std::make_unique<WinThemeEngine>();
    }
    return m_themeEngine.get();
}

#endif // BLINKIT_CRAWLER_ONLY
#endif // 0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AppImpl* AppImpl::CreateInstance(void)
{
    return new WinApp;
}

void AppImpl::InitializeBackgroundInstance(void)
{
    BackgoundThreadData data;
    data.hThread = CreateThread(nullptr, 0, WinApp::BackgroundThread, &data, 0, nullptr);
    WaitForSingleObject(data.hEvent, INFINITE);
}

void AppImpl::Log(const char *s)
{
    std::wstring ws = base::SysUTF8ToWide(s);
    ws.append(L"\r\n");
    ::OutputDebugStringW(ws.c_str());
}

} // namespace BlinKit
