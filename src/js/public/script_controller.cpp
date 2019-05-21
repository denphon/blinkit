// -------------------------------------------------
// BlinKit - js Library
// -------------------------------------------------
//   File Name: script_controller.cpp
// Description: ScriptController Class
//      Author: Ziming Li
//     Created: 2019-03-07
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#include "public/script_controller.h"

#include "core/frame/LocalFrame.h"
#include "platform/weborigin/KURL.h"

#include "context/duk_context.h"

using namespace BlinKit;

namespace blink {

ScriptController::ScriptController(LocalFrame *frame) : m_frame(*frame)
{
    // Nothing
}

ScriptController::~ScriptController(void)
{
    // Nothing, just for std::unique_ptr
}

int ScriptController::CallCrawler(const char *method, BkCallerContext::Callback callback, void *userData)
{
    assert(m_frame.IsCrawlerFrame());
    if (!m_context)
        return BkError::NotFound;
    return m_context->CallCrawler(method, callback, userData);
}

int ScriptController::CallFunction(const char *name, BkCallerContext::Callback callback, void *userData)
{
    if (!m_context)
        return BkError::NotFound;
    return m_context->CallFunction(name, callback, userData);
}

bool ScriptController::canExecuteScripts(ReasonForCallingCanExecuteScripts)
{
    return true; // Scripts are always allowed in BlinKit.
}

void ScriptController::clearForClose(void)
{
    m_context.reset();
}

void ScriptController::clearScriptObjects(void)
{
    // Nothing to do, this method was for NP objects.
}

void ScriptController::clearWindowProxy(void)
{
    m_context.reset();
}

int ScriptController::CreateCrawlerObject(const char *script, size_t length)
{
    assert(m_frame.IsCrawlerFrame());
    return EnsureContext().CreateCrawlerObject(script, length);
}

void ScriptController::disableEval(const String &errorMessage)
{
    assert(false); // Not reached!
}

void ScriptController::enableEval(void)
{
    // Nothing to do, eval is always enabled in BlinKit.
}

DukContext& ScriptController::EnsureContext(void)
{
    if (!m_context)
        m_context = std::make_unique<DukContext>(m_frame);
    return *m_context;
}

bool ScriptController::executeScriptIfJavaScriptURL(const KURL &url)
{
    assert(!protocolIsJavaScript(url));
    return false;
}

void ScriptController::executeScriptInMainWorld(const ScriptSourceCode &sourceCode, AccessControlStatus accessControlStatus, double *compilationFinishTime)
{
    assert(false); // BKTODO:
}

void ScriptController::namedItemAdded(HTMLDocument *, const AtomicString &)
{
    assert(false); // BKTODO:
}

void ScriptController::namedItemRemoved(HTMLDocument *, const AtomicString &)
{
    assert(false); // BKTODO:
}

int ScriptController::RegisterFunction(const char *name, BkFunction *functionImpl)
{
    return EnsureContext().RegisterFunction(name, functionImpl);
}

void ScriptController::updateDocument(void)
{
    if (m_context)
        m_context->Reset();
}

void ScriptController::updateSecurityOrigin(SecurityOrigin *)
{
    assert(false); // BKTODO:
}

} // namespace blink
