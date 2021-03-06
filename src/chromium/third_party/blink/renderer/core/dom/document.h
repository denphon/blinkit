// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: document.h
// Description: Document Class
//      Author: Ziming Li
//     Created: 2019-09-16
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All
 * rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved.
 * (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef BLINKIT_BLINK_DOCUMENT_H
#define BLINKIT_BLINK_DOCUMENT_H

#pragma once

#include <stack>
#include <unordered_set>
#include <vector>
#include "blinkit/common/bk_url.h"
#include "third_party/blink/renderer/core/dom/container_node.h"
#include "third_party/blink/renderer/core/dom/create_element_flags.h"
#include "third_party/blink/renderer/core/dom/document_encoding_data.h"
#include "third_party/blink/renderer/core/dom/document_lifecycle.h"
#include "third_party/blink/renderer/core/dom/document_shutdown_notifier.h"
#include "third_party/blink/renderer/core/dom/document_shutdown_observer.h"
#include "third_party/blink/renderer/core/dom/live_node_list_registry.h"
#include "third_party/blink/renderer/core/dom/mutation_observer_options.h"
#include "third_party/blink/renderer/core/dom/synchronous_mutation_notifier.h"
#include "third_party/blink/renderer/core/dom/synchronous_mutation_observer.h"
#include "third_party/blink/renderer/core/dom/tree_scope.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/casting.h"

namespace blink {

class DocumentFragment;
class DocumentInit;
class DocumentLoader;
class DocumentParser;
class DocumentType;
class ElementDataCache;
class LayoutView;
class LocalDOMWindow;
class LocalFrame;
class Location;
class NthIndexCache;
class ResourceFetcher;
class ScriptableDocumentParser;
class ScriptElementBase;
class ScriptRunner;
class SelectorQueryCache;
class Text;

enum NodeListInvalidationType : int {
    kDoNotInvalidateOnAttributeChanges = 0,
    kInvalidateOnClassAttrChange,
    kInvalidateOnIdNameAttrChange,
    kInvalidateOnNameAttrChange,
    kInvalidateOnForAttrChange,
    kInvalidateForFormControls,
    kInvalidateOnHRefAttrChange,
    kInvalidateOnAnyAttrChange,
};

const int kNumNodeListInvalidationTypes = kInvalidateOnAnyAttrChange + 1;

enum ShadowCascadeOrder {
    kShadowCascadeNone,
    kShadowCascadeV0,
    kShadowCascadeV1
};

class Document : public ContainerNode
               , public ExecutionContext
               , public TreeScope
               , public DocumentShutdownNotifier
               , public SynchronousMutationNotifier
{
public:
    ~Document(void) override;

    void Initialize(void);
    virtual void Shutdown(void);
    bool IsHTMLDocument(void) const { return true; } // Just a placeholder.

    uint64_t DomTreeVersion(void) const { return m_domTreeVersion; }
    void IncDOMTreeVersion(void)
    {
        ASSERT(m_lifecycle.StateAllowsTreeMutations());
        m_domTreeVersion = ++m_globalTreeVersion;
    }

    LocalDOMWindow* domWindow(void) const { return m_domWindow; }
    void ClearDOMWindow(void) { m_domWindow = nullptr; }
    LocalFrame* GetFrame(void) const { return m_frame; }  // can be null
    LocalFrame* ExecutingFrame(void);
    Document* ContextDocument(void) const;
    ScriptRunner* GetScriptRunner(void) { return m_scriptRunner.get(); }

    // Exports for JS
    Element* body(void) const;
    DocumentFragment* createDocumentFragment(void);
    Element* createElement(const AtomicString &name, ExceptionState &exceptionState);
    Element* documentElement(void) const { return m_documentElement.Get(); }
    using TreeScope::getElementById;
    Location* location(void) const;
    void open(Document *enteredDocument, ExceptionState &exceptionState);
    void write(const String &text, Document *enteredDocument = nullptr, ExceptionState &exceptionState = ASSERT_NO_EXCEPTION);
    void write(LocalDOMWindow *callingWindow, const std::vector<std::string> &text, ExceptionState &exceptionState);
    void writeln(const String &text, Document *enteredDocument = nullptr, ExceptionState &exceptionState = ASSERT_NO_EXCEPTION);
    void writeln(LocalDOMWindow *callingWindow, const std::vector<std::string> &text, ExceptionState &exceptionState);

    void SetDoctype(DocumentType *docType);

    // Return the document URL, or an empty URL if it's unavailable.
    // This is not an implementation of web-exposed Document.prototype.URL.
    const BlinKit::BkURL& Url(void) const final { return m_URL; }
    void SetURL(const BlinKit::BkURL &url);
    BlinKit::BkURL ValidBaseElementURL(void) const;
    // Creates URL based on passed relative url and passed base URL override.
    BlinKit::BkURL CompleteURLWithOverride(const String &url, const BlinKit::BkURL &baseUrlOverride) const;
    // Bind the url to document.url, if unavailable bind to about:blank.
    BlinKit::BkURL urlForBinding(void) const;
    // Fallback base URL.
    // https://html.spec.whatwg.org/multipage/urls-and-fetching.html#fallback-base-url
    BlinKit::BkURL FallbackBaseURL(void) const;

    const AtomicString& ContentLanguage(void) const { return m_contentLanguage; }
    void SetContentLanguage(const AtomicString &language);

    // The following implements the rule from HTML 4 for what valid names are.
    // To get this right for all the XML cases, we probably have to improve this
    // or move it and make it sensitive to the type of document.
    static bool IsValidName(const String &name);
    virtual Element* CreateElement(const AtomicString &name, CreateElementFlags flags) = 0;

    // keep track of what types of event listeners are registered, so we don't
    // dispatch events unnecessarily
    enum ListenerType {
        kDOMSubtreeModifiedListener = 1,
        kDOMNodeInsertedListener = 1 << 1,
        kDOMNodeRemovedListener = 1 << 2,
        kDOMNodeRemovedFromDocumentListener = 1 << 3,
        kDOMNodeInsertedIntoDocumentListener = 1 << 4,
        kDOMCharacterDataModifiedListener = 1 << 5,
        kAnimationEndListener = 1 << 6,
        kAnimationStartListener = 1 << 7,
        kAnimationIterationListener = 1 << 8,
        kTransitionEndListener = 1 << 9,
        kScrollListener = 1 << 10,
        kLoadListenerAtCapturePhaseOrAtStyleElement = 1 << 11
        // 4 bits remaining
    };
    bool HasListenerType(ListenerType listenerType) const { return 0 != (m_listenerTypes & listenerType); }
    void AddListenerType(ListenerType listenerType) { m_listenerTypes |= listenerType; }
    void AddListenerTypeIfNeeded(const AtomicString &eventType, EventTarget &eventTarget);
    void AddMutationEventListenerTypeIfEnabled(ListenerType listenerType);

    bool HasMutationObservers(void) const { return 0 != m_mutationObserverTypes; }
    bool HasMutationObserversOfType(MutationType type) const { return 0 != (m_mutationObserverTypes & type); }

    int NodeCount(void) const { return m_nodeCount; }
    void IncrementNodeCount(void) { ++m_nodeCount; }
    void DecrementNodeCount(void)
    {
        ASSERT(m_nodeCount > 0);
        --m_nodeCount;
    }

    ElementDataCache* GetElementDataCache(void) { return m_elementDataCache.get(); }
    SelectorQueryCache& GetSelectorQueryCache(void);

    NthIndexCache* GetNthIndexCache(void) const { return m_nthIndexCache; }
    void SetNthIndexCache(NthIndexCache *nthIndexCache)
    {
        ASSERT(nullptr == m_nthIndexCache || nullptr == nthIndexCache);
        m_nthIndexCache = nthIndexCache;
    }

    void RegisterNodeList(const LiveNodeListBase *list);

    bool MayContainV0Shadow(void) const { return m_mayContainV0Shadow; }

    bool ShouldInvalidateNodeListCaches(const QualifiedName *attrName = nullptr) const;
    void InvalidateNodeListCaches(const QualifiedName *attrName);

    DocumentLoader* Loader(void) const;

    enum CompatibilityMode { kQuirksMode, kLimitedQuirksMode, kNoQuirksMode };
    CompatibilityMode GetCompatibilityMode(void) const { return m_compatibilityMode; }
    bool InQuirksMode(void) const { return kQuirksMode == m_compatibilityMode; }
    void SetCompatibilityMode(CompatibilityMode mode);

    const WTF::TextEncoding& Encoding(void) const { return m_encodingData.Encoding(); }
    void SetEncodingData(const DocumentEncodingData &newData);

    enum DocumentReadyState { kLoading, kInteractive, kComplete };
    void SetReadyState(DocumentReadyState readyState);

    enum ParsingState { kParsing, kInDOMContentLoaded, kFinishedParsing };
    bool Parsing(void) const { return kParsing == m_parsingState; }
    bool HasFinishedParsing(void) const { return kFinishedParsing == m_parsingState; }
    void SetParsingState(ParsingState parsingState);
    void ParserInsertedHtmlElement(Element &element);
    void WillInsertBody(void);
    void FinishedParsing(void);
    void CancelParsing(void);
    void Abort(void);
    void CheckCompleted(void);

    void DispatchUnloadEvents(void);

    bool IsActive(void) const { return m_lifecycle.IsActive(); }
    bool InStyleRecalc(void) const { return m_lifecycle.GetState() == DocumentLifecycle::kInStyleRecalc; }

    enum PageDismissalType {
        kNoDismissal,
        kBeforeUnloadDismissal,
        kPageHideDismissal,
        kUnloadVisibilityChangeDismissal,
        kUnloadDismissal
    };
    PageDismissalType PageDismissalEventBeingDispatched(void) const;

    bool WasDiscarded(void) const { return m_wasDiscarded; }
    void SetWasDiscarded(bool wasDiscarded) { m_wasDiscarded = wasDiscarded; }

    enum LoadEventProgress {
        kLoadEventNotRun,
        kLoadEventInProgress,
        kLoadEventCompleted,
        kBeforeUnloadEventInProgress,
        kBeforeUnloadEventCompleted,
        kPageHideInProgress,
        kUnloadVisibilityChangeInProgress,
        kUnloadEventInProgress,
        kUnloadEventHandled
    };
    bool LoadEventFinished(void) const { return m_loadEventProgress >= kLoadEventCompleted; }
    bool LoadEventStillNeeded(void) const { return kLoadEventNotRun == m_loadEventProgress; }
    void SuppressLoadEvent(void);
    // Used to allow element that loads data without going through a FrameLoader
    // to delay the 'load' event.
    void IncrementLoadEventDelayCount(void) { ++m_loadEventDelayCount; }
    void DecrementLoadEventDelayCount(void);
    void CheckLoadEventSoon(void);
    bool IsDelayingLoadEvent(void) { return 0 != m_loadEventDelayCount; }

    DocumentParser* Parser(void) const { return m_parser.get(); }
    ScriptableDocumentParser* GetScriptableDocumentParser(void) const;
    std::shared_ptr<DocumentParser> OpenForNavigation(const AtomicString &mimeType, const AtomicString &encoding);
    std::shared_ptr<DocumentParser> ImplicitOpen(void);
    std::shared_ptr<DocumentParser> CreateParser(void);

    enum HttpRefreshType { kHttpRefreshFromHeader, kHttpRefreshFromMetaTag };
    void MaybeHandleHttpRefresh(const String &content, HttpRefreshType refreshType);

    bool IsScriptExecutionReady(void) const {
        return HaveImportsLoaded() && HaveScriptBlockingStylesheetsLoaded();
    }

    bool CanAcceptChild(const Node &newChild, const Node *next, const Node *oldChild, ExceptionState &exceptionState) const;
    // nodeChildrenWillBeRemoved is used when removing all node children at once.
    void NodeChildrenWillBeRemoved(ContainerNode &container);
    // nodeWillBeRemoved is only safe when removing one node at a time.
    void NodeWillBeRemoved(Node &n);

    bool ContainsV1ShadowTree(void) const { return ShadowCascadeOrder::kShadowCascadeV1 == m_shadowCascadeOrder; }

    bool IsInDocumentWrite(void) const { return m_writeRecursionDepth > 0; }

    void PushCurrentScript(ScriptElementBase *newCurrentScript);
    void PopCurrentScript(ScriptElementBase *script);

    // ExecutionContext overrides
    const BlinKit::BkURL& BaseURL(void) const override;
    // Creates URL based on passed relative url and this documents base URL.
    // Depending on base URL value it is possible that parent document
    // base URL will be used instead. Uses CompleteURLWithOverride internally.
    BlinKit::BkURL CompleteURL(const String &url) const final;
    ResourceFetcher* Fetcher(void) const override { return m_fetcher.get(); }
    bool CanExecuteScripts(ReasonForCallingCanExecuteScripts reason) override;
    std::shared_ptr<base::SingleThreadTaskRunner> GetTaskRunner(TaskType type) override;

    // Temporary flag for some UseCounter items. crbug.com/859391.
    enum class InDOMNodeRemovedHandlerState {
        kNone,
        kDOMNodeRemoved,
        kDOMNodeRemovedFromDocument
    };
    void SetInDOMNodeRemovedHandlerState(InDOMNodeRemovedHandlerState state) { m_inDomNodeRemovedHandlerState = state; }
    InDOMNodeRemovedHandlerState GetInDOMNodeRemovedHandlerState(void) const { return m_inDomNodeRemovedHandlerState; }
    bool InDOMNodeRemovedHandler(void) const { return m_inDomNodeRemovedHandlerState != InDOMNodeRemovedHandlerState::kNone; }
    void CountDetachingNodeAccessInDOMNodeRemovedHandler(void) { ASSERT(GetInDOMNodeRemovedHandlerState() != InDOMNodeRemovedHandlerState::kNone); } // Just a placeholder
protected:
    Document(const DocumentInit &initializer);
private:
    friend class IgnoreDestructiveWriteCountIncrementer;
    friend class ThrowOnDynamicMarkupInsertionCountIncrementer;
    friend class IgnoreOpensDuringUnloadCountIncrementer;

    static ConstructionType GetConstructionType(const DocumentInit &init);

    void DispatchDidReceiveTitle(void);
    void UpdateBaseURL(void);

    // ImplicitClose() actually does the work of closing the input stream.
    void ImplicitClose(void);
    bool ShouldComplete(void);
    // Returns |true| if both document and its owning frame are still attached.
    // Any of them could be detached during the check, e.g. by calling
    // iframe.remove() from an event handler.
    bool CheckCompletedInternal(void);
    void DetachParser(void);

    bool HaveImportsLoaded(void) const;
    bool HaveScriptBlockingStylesheetsLoaded(void) const;

    void LoadEventDelayTimerFired(TimerBase *);

    // EventTarget overrides
    void RemoveAllEventListeners(void) final;
    // Node overrides
    String nodeName(void) const final;
    NodeType getNodeType(void) const final { return kDocumentNode; }
    Node* Clone(Document &factory, CloneChildrenFlag flag) const override;
    bool ChildTypeAllowed(NodeType type) const final;
    // ContainerNode overrides
    void ChildrenChanged(const ChildrenChange &change) override;
    // ExecutionContext overrides
    bool IsDocument(void) const final { return true; }
    LocalDOMWindow* ExecutingWindow(void) const final;

    static uint64_t m_globalTreeVersion;
    uint64_t m_domTreeVersion;

    DocumentLifecycle m_lifecycle;
    Member<LocalFrame> m_frame;
    Member<LocalDOMWindow> m_domWindow;
    DocumentEncodingData m_encodingData;
    DocumentReadyState m_readyState = kComplete;
    ParsingState m_parsingState = kFinishedParsing;
    ShadowCascadeOrder m_shadowCascadeOrder = kShadowCascadeNone;

    unsigned short m_listenerTypes = 0;
    MutationObserverOptions m_mutationObserverTypes = 0;

    // https://html.spec.whatwg.org/C/dynamic-markup-insertion.html#ignore-destructive-writes-counter
    unsigned ignore_destructive_write_count_ = 0;
    // https://html.spec.whatwg.org/C/dynamic-markup-insertion.html#throw-on-dynamic-markup-insertion-counter
    unsigned throw_on_dynamic_markup_insertion_count_ = 0;
    // https://html.spec.whatwg.org/C/dynamic-markup-insertion.html#ignore-opens-during-unload-counter
    unsigned ignore_opens_during_unload_count_ = 0;

    bool m_wellFormed = false;

    String m_title;

    // Document URLs.
    BlinKit::BkURL m_URL;  // Document.URL: The URL from which this document was retrieved.
    BlinKit::BkURL m_baseURL;  // Node.baseURI: The URL to use when resolving relative URLs.
    BlinKit::BkURL m_baseURLOverride;
    BlinKit::BkURL m_baseElementURL;  // The URL set by the <base> element.

    AtomicString m_contentLanguage;

    Member<DocumentType> m_docType;
    Member<Element> m_titleElement;
    Member<Element> m_documentElement;

    CompatibilityMode m_compatibilityMode = kNoQuirksMode;
    bool m_compatibilityModeLocked = false;

    bool m_wasDiscarded = false;

    LoadEventProgress m_loadEventProgress = kLoadEventCompleted;
    int m_loadEventDelayCount = 0;
    TaskRunnerTimer<Document> m_loadEventDelayTimer;

    std::shared_ptr<ResourceFetcher> m_fetcher;
    std::shared_ptr<DocumentParser> m_parser;
    std::unique_ptr<ScriptRunner> m_scriptRunner;
#ifndef BLINKIT_CRAWLER_ONLY
    LayoutView *m_layoutView = nullptr;
    Member<Element> m_focusedElement;
#endif

    std::unique_ptr<ElementDataCache> m_elementDataCache;
    std::unique_ptr<SelectorQueryCache> m_selectorQueryCache;

    // It is safe to keep a raw, untraced pointer to this stack-allocated
    // cache object: it is set upon the cache object being allocated on
    // the stack and cleared upon leaving its allocated scope. Hence it
    // is acceptable not to trace it -- should a conservative GC occur,
    // the cache object's references will be traced by a stack walk.
    NthIndexCache *m_nthIndexCache = nullptr;

    int m_nodeCount = 0;
    std::unordered_set<const LiveNodeListBase *> m_listsInvalidatedAtDocument;
    LiveNodeListRegistry m_nodeLists;

    // Temporary flag for some UseCounter items. crbug.com/859391.
    InDOMNodeRemovedHandlerState m_inDomNodeRemovedHandlerState = InDOMNodeRemovedHandlerState::kNone;
    bool m_mayContainV0Shadow = false; // BKTODO: This may be useless for crawlers.

    TaskRunnerTimer<Document> m_elementDataCacheClearTimer;
    void ElementDataCacheClearTimerFired(TimerBase *);

    bool m_writeRecursionIsTooDeep = false;
    unsigned m_writeRecursionDepth = 0;

    std::stack<ScriptElementBase *> m_currentScriptStack;
};

DEFINE_COMPARISON_OPERATORS_WITH_REFERENCES(Document)

template <>
struct DowncastTraits<Document> {
    static bool AllowFrom(const ExecutionContext &context)
    {
        return context.IsDocument();
    }
    static bool AllowFrom(const Node &node)
    {
        return node.IsDocumentNode();
    }
};

} // namespace blink

#endif // BLINKIT_BLINK_DOCUMENT_H
