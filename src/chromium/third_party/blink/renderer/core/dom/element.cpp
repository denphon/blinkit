// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: element.cpp
// Description: Element Class
//      Author: Ziming Li
//     Created: 2019-10-18
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Peter Kelly (pmk@post.com)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012, 2013 Apple Inc.
 * All rights reserved.
 *           (C) 2007 Eric Seidel (eric@webkit.org)
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
 */

#include "element.h"

#include "third_party/blink/renderer/core/dom/attr.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element_data.h"
#include "third_party/blink/renderer/core/dom/element_data_cache.h"
#include "third_party/blink/renderer/core/dom/element_rare_data.h"
#include "third_party/blink/renderer/core/dom/mutation_observer_interest_group.h"
#include "third_party/blink/renderer/core/dom/text.h"
#include "third_party/blink/renderer/core/editing/serializers/serialization.h"
#include "third_party/blink/renderer/core/html_names.h"
#include "third_party/blink/renderer/core/html/parser/html_parser_idioms.h"
#include "third_party/blink/renderer/platform/bindings/gc_pool.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"
#include "third_party/blink/renderer/platform/wtf/not_found.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"

using namespace BlinKit;

namespace blink {

enum class ClassStringContent { kEmpty, kWhiteSpaceOnly, kHasClasses };

Element::Element(const QualifiedName &tagName, Document *document, ConstructionType type)
    : ContainerNode(document, type), m_tagName(tagName)
{
    // Nothing
}

Element::~Element(void)
{
#ifndef BLINKIT_CRAWLER_ONLY
    DCHECK(NeedsAttach());
#endif
}

void Element::AppendAttributeInternal(
    const QualifiedName &name, const AtomicString &value,
    SynchronizationOfLazyAttribute inSynchronizationOfLazyAttribute)
{
    if (!inSynchronizationOfLazyAttribute)
        WillModifyAttribute(name, g_null_atom, value);
    EnsureUniqueElementData().Attributes().Append(name, value);
    if (!inSynchronizationOfLazyAttribute)
        DidAddAttribute(name, value);
}

Attr* Element::AttrIfExists(const QualifiedName &name)
{
    if (AttrNodeList *attrNodeList = GetAttrNodeList())
    {
        for (const auto &attr : *attrNodeList)
        {
            if (attr->GetQualifiedName().Matches(name))
                return attr;
        }
    }
    return nullptr;
}

static inline AtomicString MakeIdForStyleResolution(const AtomicString &value, bool inQuirksMode)
{
    if (inQuirksMode)
        return value.LowerASCII();
    return value;
}

void Element::AttributeChanged(const AttributeModificationParams &params)
{
    const QualifiedName &name = params.name;
#ifndef BLINKIT_CRAWLER_ONLY
    if (ShadowRoot* parent_shadow_root =
        ShadowRootWhereNodeCanBeDistributedForV0(*this)) {
        if (ShouldInvalidateDistributionWhenAttributeChanged(
            *parent_shadow_root, name, params.new_value))
            parent_shadow_root->SetNeedsDistributionRecalc();
    }
    if (name == HTMLNames::slotAttr && params.old_value != params.new_value) {
        if (ShadowRoot* root = V1ShadowRootOfParent())
            root->DidChangeHostChildSlotName(params.old_value, params.new_value);
    }
#endif

    ParseAttribute(params);

    GetDocument().IncDOMTreeVersion();

    if (name == html_names::kIdAttr)
    {
        AtomicString oldId = GetElementData()->IdForStyleResolution();
        AtomicString newId = MakeIdForStyleResolution(params.newValue, GetDocument().InQuirksMode());
        if (newId != oldId)
        {
            GetElementData()->SetIdForStyleResolution(newId);
#ifndef BLINKIT_CRAWLER_ONLY
            GetDocument().GetStyleEngine().IdChangedForElement(old_id, new_id, *this);
#endif
        }
    }
    else if (name == html_names::kClassAttr)
    {
        ClassAttributeChanged(params.newValue);
        if (HasRareData())
        {
            ASSERT(false); // BKTODO:
#if 0
            if (GetElementRareData()->GetClassList())
            {
                GetElementRareData()->GetClassList()->DidUpdateAttributeValue(
                    params.old_value, params.new_value);
            }
#endif
        }
    }
    else if (name == html_names::kNameAttr)
    {
        SetHasName(!params.newValue.IsNull());
    }
#ifndef BLINKIT_CRAWLER_ONLY
    else if (name == HTMLNames::partAttr) {
        if (RuntimeEnabledFeatures::CSSPartPseudoElementEnabled()) {
            EnsureElementRareData().SetPart(params.new_value);
            GetDocument().GetStyleEngine().PartChangedForElement(*this);
        }
    }
    else if (name == HTMLNames::partmapAttr) {
        if (RuntimeEnabledFeatures::CSSPartPseudoElementEnabled()) {
            EnsureElementRareData().SetPartNamesMap(params.new_value);
            GetDocument().GetStyleEngine().PartmapChangedForElement(*this);
        }
    }
    else if (IsStyledElement()) {
        if (name == styleAttr) {
            StyleAttributeChanged(params.new_value, params.reason);
        }
        else if (IsPresentationAttribute(name)) {
            GetElementData()->presentation_attribute_style_is_dirty_ = true;
            SetNeedsStyleRecalc(kLocalStyleChange,
                StyleChangeReasonForTracing::FromAttribute(name));
        }
        else if (RuntimeEnabledFeatures::InvisibleDOMEnabled() &&
            name == HTMLNames::invisibleAttr &&
            params.old_value != params.new_value) {
            InvisibleAttributeChanged(params.old_value, params.new_value);
        }
    }
#endif

    InvalidateNodeListCachesInAncestors(&name, this, nullptr);

#ifndef BLINKIT_CRAWLER_ONLY
    if (params.reason == AttributeModificationReason::kDirectly &&
        name == tabindexAttr && AdjustedFocusedElementInTreeScope() == this) {
        // The attribute change may cause supportsFocus() to return false
        // for the element which had focus.
        //
        // TODO(tkent): We should avoid updating style.  We'd like to check only
        // DOM-level focusability here.
        GetDocument().UpdateStyleAndLayoutTreeForNode(this);
        if (!SupportsFocus())
            blur();
    }
#endif
}

AttributeCollection Element::Attributes(void) const
{
    if (const ElementData *elementData = GetElementData())
    {
        SynchronizeAllAttributes();
        return elementData->Attributes();
    }
    return AttributeCollection();
}

NamedNodeMap* Element::attributes(void) const
{
    ElementRareData &rareData = const_cast<Element *>(this)->EnsureElementRareData();
    if (NamedNodeMap *attributeMap = rareData.AttributeMap())
        return attributeMap;

    std::unique_ptr<NamedNodeMap> attributeMap = NamedNodeMap::Create(const_cast<Element *>(this));
    rareData.SetAttributeMap(attributeMap);
    return rareData.AttributeMap();
}

AttributeCollection Element::AttributesWithoutUpdate(void) const
{
    if (!GetElementData())
        return AttributeCollection();
    return GetElementData()->Attributes();
}

#ifndef BLINKIT_CRAWLER_ONLY
void Element::ChildrenChanged(const ChildrenChange &change)
{
    ContainerNode::ChildrenChanged(change);

    ASSERT(false); // BKTODO:
#if 0
    CheckForEmptyStyleChange(change.sibling_before_change,
        change.sibling_after_change);

    if (!change.by_parser && change.IsChildElementChange())
        CheckForSiblingStyleChanges(
            change.type == kElementRemoved ? kSiblingElementRemoved
            : kSiblingElementInserted,
            ToElement(change.sibling_changed), change.sibling_before_change,
            change.sibling_after_change);

    if (ShadowRoot* shadow_root = GetShadowRoot())
        shadow_root->SetNeedsDistributionRecalcWillBeSetNeedsAssignmentRecalc();
#endif
}
#endif

bool Element::ChildTypeAllowed(NodeType type) const
{
    switch (type)
    {
        case kElementNode:
        case kTextNode:
        case kCommentNode:
        case kProcessingInstructionNode:
        case kCdataSectionNode:
            return true;
        default:
            break;
    }
    return false;
}

template <typename CharacterType>
static inline ClassStringContent ClassStringHasClassName(const CharacterType *characters, unsigned length)
{
    ASSERT(length > 0);

    unsigned i = 0;
    do {
        if (IsNotHTMLSpace<CharacterType>(characters[i]))
            break;
        ++i;
    } while (i < length);

    if (i == length && length >= 1)
        return ClassStringContent::kWhiteSpaceOnly;

    return ClassStringContent::kHasClasses;
}

static inline ClassStringContent ClassStringHasClassName(const AtomicString &newClassString)
{
    unsigned length = newClassString.length();

    if (!length)
        return ClassStringContent::kEmpty;

    if (newClassString.Is8Bit())
        return ClassStringHasClassName(newClassString.Characters8(), length);
    return ClassStringHasClassName(newClassString.Characters16(), length);
}

void Element::ClassAttributeChanged(const AtomicString &newClassString)
{
    const ElementData *elementData = GetElementData();
    ASSERT(nullptr != elementData);

    ClassStringContent classStringContentType = ClassStringHasClassName(newClassString);
    const bool shouldFoldCase = GetDocument().InQuirksMode();
    if (classStringContentType == ClassStringContent::kHasClasses)
    {
#ifdef BLINKIT_CRAWLER_ONLY
        elementData->SetClass(newClassString, shouldFoldCase);
#else
        if (ForCrawler())
        {
            elementData->SetClass(newClassString, shouldFoldCase);
        }
        else
        {
            const SpaceSplitString oldClasses = elementData->ClassNames();
            elementData->SetClass(newClassString, shouldFoldCase);
            const SpaceSplitString& new_classes = elementData->ClassNames();
            GetDocument().GetStyleEngine().ClassChangedForElement(old_classes, new_classes, *this);
        }
#endif
    }
    else
    {
#ifndef BLINKIT_CRAWLER_ONLY
        if (!ForCrawler())
        {
            const SpaceSplitString& old_classes = elementData->ClassNames();
            GetDocument().GetStyleEngine().ClassChangedForElement(old_classes, *this);
        }
#endif
        if (classStringContentType == ClassStringContent::kWhiteSpaceOnly)
            elementData->SetClass(newClassString, shouldFoldCase);
        else
            elementData->ClearClass();
    }
}

const SpaceSplitString& Element::ClassNames(void) const
{
    ASSERT(HasClass());
    ASSERT(GetElementData());
    return GetElementData()->ClassNames();
}

void Element::CloneAttributesFrom(const Element &other)
{
    if (HasRareData())
        DetachAllAttrNodesFromElement();

    other.SynchronizeAllAttributes();
    if (!other.m_elementData)
    {
        m_elementData.reset();
        return;
    }

    const AtomicString &oldId = GetIdAttribute();
    const AtomicString &newId = other.GetIdAttribute();

    if (!oldId.IsNull() || !newId.IsNull())
        UpdateId(oldId, newId);

    const AtomicString &oldName = GetNameAttribute();
    const AtomicString &newName = other.GetNameAttribute();

    if (!oldName.IsNull() || !newName.IsNull())
        UpdateName(oldName, newName);

    // Quirks mode makes class and id not case sensitive. We can't share the
    // ElementData if the idForStyleResolution and the className need different
    // casing.
    bool ownerDocumentsHaveDifferentCaseSensitivity = false;
    if (other.HasClass() || other.HasID())
        ownerDocumentsHaveDifferentCaseSensitivity = other.GetDocument().InQuirksMode() != GetDocument().InQuirksMode();

    // If 'other' has a mutable ElementData, convert it to an immutable one so we
    // can share it between both elements.
    // We can only do this if there are no presentation attributes and sharing the
    // data won't result in different case sensitivity of class or id.
    if (other.m_elementData->IsUnique() && !ownerDocumentsHaveDifferentCaseSensitivity
        && nullptr == other.m_elementData->PresentationAttributeStyle())
    {
        const_cast<Element &>(other).m_elementData = ToUniqueElementData(other.m_elementData.get())->MakeShareableCopy();
    }

    ASSERT(GetDocument() == other.GetDocument()); // Old logic from NeedsURLResolutionForInlineStyle
    if (!other.m_elementData->IsUnique() && !ownerDocumentsHaveDifferentCaseSensitivity)
        m_elementData = other.m_elementData;
    else
        m_elementData = other.m_elementData->MakeUniqueCopy();

    for (const Attribute &attr : m_elementData->Attributes())
    {
        AttributeModificationParams params(attr.GetName(), g_null_atom, attr.Value(), AttributeModificationReason::kByCloning);
        AttributeChanged(params);
    }
}

Node* Element::Clone(Document &document, CloneChildrenFlag flag) const
{
    return flag == CloneChildrenFlag::kClone
        ? CloneWithChildren(&document)
        : CloneWithoutChildren(&document);
}

Element* Element::CloneWithChildren(Document *factory) const
{
    if (nullptr == factory)
        factory = &GetDocument();

    Element *clone = CloneWithoutAttributesAndChildren(*factory);

    clone->CloneAttributesFrom(*this);
    clone->CloneNonAttributePropertiesFrom(*this, CloneChildrenFlag::kClone);
    clone->CloneChildNodesFrom(*this);
    return clone;
}

Element* Element::CloneWithoutAttributesAndChildren(Document &factory) const
{
    ASSERT(!IsScriptElement()); // BKTODO:
    return factory.CreateElement(localName(), CreateElementFlags::ByCloneNode());
}

Element* Element::CloneWithoutChildren(Document *nullableFactory) const
{
    ASSERT(false); // BKTODO:
    return nullptr;
}

AtomicString Element::ComputeInheritedLanguage(void) const
{
    const Node *n = this;
    AtomicString value;
    // The language property is inherited, so we iterate over the parents to find
    // the first language.
    do {
        if (n->IsElementNode())
        {
            if (const ElementData *elementData = ToElement(n)->GetElementData())
            {
                AttributeCollection attributes = elementData->Attributes();
                if (const Attribute *attribute = attributes.Find(html_names::kLangAttr))
                    value = attribute->Value();
            }
        }
        else if (auto *document = DynamicTo<Document>(n))
        {
            // checking the MIME content-language
            value = document->ContentLanguage();
        }

        n = n->ParentOrShadowHostNode();
    } while (nullptr != n && value.IsNull());

    return value;
}

#ifndef BLINKIT_CRAWLER_ONLY
void Element::DefaultEventHandler(Event & event)
{
    if (RuntimeEnabledFeatures::InvisibleDOMEnabled() &&
        event.type() == EventTypeNames::activateinvisible &&
        event.target() == this) {
        removeAttribute(invisibleAttr);
        event.SetDefaultHandled();
        return;
    }
    ContainerNode::DefaultEventHandler(event);
}
#endif

void Element::DetachAllAttrNodesFromElement(void)
{
    ASSERT(false); // BKTODO:
}

void Element::DidAddAttribute(const QualifiedName &name, const AtomicString &value)
{
    if (name == html_names::kIdAttr)
        UpdateId(g_null_atom, value);
    AttributeChanged(AttributeModificationParams(name, g_null_atom, value, AttributeModificationReason::kDirectly));
    DispatchSubtreeModifiedEvent();
}

void Element::DidModifyAttribute(const QualifiedName &name, const AtomicString &oldValue, const AtomicString &newValue)
{
    ASSERT(false); // BKTODO:
}

Attr* Element::EnsureAttr(const QualifiedName &name)
{
    Attr *attrNode = AttrIfExists(name);
    if (nullptr == attrNode)
    {
        attrNode = Attr::Create(*this, name);
        GetTreeScope().AdoptIfNeeded(*attrNode);
        EnsureElementRareData().AddAttr(attrNode);
    }
    return attrNode;
}

ElementRareData& Element::EnsureElementRareData(void)
{
    return static_cast<ElementRareData &>(EnsureRareData());
}

UniqueElementData& Element::EnsureUniqueElementData(void)
{
    if (!m_elementData)
        m_elementData = UniqueElementData::Create();
    else if (!m_elementData->IsUnique())
        m_elementData = ToShareableElementData(m_elementData.get())->MakeUniqueCopy();
    return ToUniqueElementData(*m_elementData);
}

const AtomicString& Element::FastGetAttribute(const QualifiedName &name) const
{
    if (const ElementData *elementData = GetElementData())
    {
        if (const Attribute *attribute = elementData->Attributes().Find(name))
            return attribute->Value();
    }
    return g_null_atom;
}

bool Element::FastHasAttribute(const QualifiedName &name) const
{
    if (const ElementData *elementData = GetElementData())
        return elementData->Attributes().FindIndex(name) != kNotFound;
    return false;
}

void Element::FinishParsingChildren(void)
{
    SetIsFinishedParsingChildren(true);
#ifndef BLINKIT_CRAWLER_ONLY
    ASSERT(false); // BKTODO:
    CheckForEmptyStyleChange(this, this);
    CheckForSiblingStyleChanges(kFinishedParsingChildren, nullptr, lastChild(), nullptr);
#endif
    // BKTODO: Check HTML element overrides.
}

AttrNodeList* Element::GetAttrNodeList(void)
{
    return HasRareData() ? GetElementRareData()->GetAttrNodeList() : nullptr;
}

const AtomicString& Element::getAttribute(const QualifiedName &name) const
{
    if (const ElementData *elementData = GetElementData())
    {
        SynchronizeAttribute(name);
        if (const Attribute *attribute = elementData->Attributes().Find(name))
            return attribute->Value();
    }
    return g_null_atom;
}

Attr* Element::getAttributeNode(const AtomicString &name)
{
    const ElementData *elementData = GetElementData();
    if (nullptr == elementData)
        return nullptr;

    SynchronizeAttribute(name);
    const Attribute* attribute = elementData->Attributes().Find(LowercaseIfNecessary(name));
    if (nullptr == attribute)
        return nullptr;

    return EnsureAttr(attribute->GetName());
}

Attr* Element::getAttributeNodeNS(const AtomicString &namespaceURI, const AtomicString &localName)
{
    ASSERT(false); // BKTODO:
    return nullptr;
}

ElementRareData* Element::GetElementRareData(void) const
{
    ASSERT(HasRareData());
    return static_cast<ElementRareData *>(RareData());
}

const AtomicString& Element::GetIdAttribute(void) const
{
    return HasID() ? FastGetAttribute(html_names::kIdAttr) : g_null_atom;
}

const AtomicString& Element::GetNameAttribute(void) const
{
    return HasName() ? FastGetAttribute(html_names::kNameAttr) : g_null_atom;
}

BkURL Element::GetURLAttribute(const QualifiedName &name) const
{
#if DCHECK_IS_ON()
    if (const ElementData *elementData = GetElementData())
    {
        if (const Attribute *attribute = elementData->Attributes().Find(name))
            ASSERT(IsURLAttribute(*attribute));
    }
#endif
    return GetDocument().CompleteURL(StripLeadingAndTrailingHTMLSpaces(getAttribute(name)));
}

bool Element::hasAttribute(const AtomicString &name) const
{
    if (const ElementData *elementData = GetElementData())
    {
        SynchronizeAttribute(name);
        return GetElementData()->Attributes().FindIndex(LowercaseIfNecessary(name)) != kNotFound;
    }
    return false;
}

bool Element::HasClass(void) const
{
    if (const ElementData *elementData = GetElementData())
        return elementData->HasClass();
    return false;
}

bool Element::HasClassName(const AtomicString &className) const
{
    return HasClass() && ClassNames().Contains(className);
}

bool Element::HasID(void) const
{
    if (const ElementData *elementData = GetElementData())
        return elementData->HasID();
    return false;
}

const AtomicString& Element::IdForStyleResolution(void) const
{
    ASSERT(HasID());
    return GetElementData()->IdForStyleResolution();
}

String Element::innerHTML(void) const
{
    return CreateMarkup(this, kChildrenOnly);
}

Node::InsertionNotificationRequest Element::InsertedInto(ContainerNode &insertionPoint)
{
    // need to do superclass processing first so isConnected() is true
    // by the time we reach updateId
    ContainerNode::InsertedInto(insertionPoint);

    ASSERT(!HasRareData() || !GetElementRareData()->HasPseudoElements());

    if (!insertionPoint.IsInTreeScope())
        return kInsertionDone;

    if (HasRareData())
    {
        ElementRareData *rareData = GetElementRareData();
        if (rareData->IntersectionObserverData() && rareData->IntersectionObserverData()->HasObservations())
        {
            ASSERT(false); // BKTODO:
#if 0
            GetDocument().EnsureIntersectionObserverController().AddTrackedTarget(
                *this);
            if (LocalFrameView* frame_view = GetDocument().View())
                frame_view->SetIntersectionObservationState(LocalFrameView::kRequired);
#endif
        }
    }

#ifndef BLINKIT_CRAWLER_ONLY
    if (isConnected())
    {
        ASSERT(false); // BKTODO:
#if 0
        if (GetCustomElementState() == CustomElementState::kCustom)
            CustomElement::EnqueueConnectedCallback(this);
        else if (IsUpgradedV0CustomElement())
            V0CustomElement::DidAttach(this, GetDocument());
        else if (GetCustomElementState() == CustomElementState::kUndefined)
            CustomElement::TryToUpgrade(this);
#endif
    }
#endif

    TreeScope &scope = insertionPoint.GetTreeScope();
#ifdef BLINKIT_CRAWLER_ONLY
    ASSERT(GetTreeScope() == scope);
#else
    ASSERT(false); // BKTODO:
    if (GetTreeScope() != scope)
        return kInsertionDone;
#endif

    const AtomicString &idValue = GetIdAttribute();
    if (!idValue.IsNull())
        UpdateId(scope, g_null_atom, idValue);

    const AtomicString &nameValue = GetNameAttribute();
    if (!nameValue.IsNull())
        UpdateName(g_null_atom, nameValue);

    return kInsertionDone;
}

bool Element::IsInDescendantTreeOf(const Element *shadowHost) const
{
#ifdef BLINKIT_CRAWLER_ONLY
    NOTREACHED();
#else
    DCHECK(shadow_host);
    DCHECK(IsShadowHost(shadow_host));

    for (const Element* ancestor_shadow_host = OwnerShadowHost();
        ancestor_shadow_host;
        ancestor_shadow_host = ancestor_shadow_host->OwnerShadowHost()) {
        if (ancestor_shadow_host == shadow_host)
            return true;
    }
#endif
    return false;
}

const AtomicString& Element::IsValue(void) const
{
    if (HasRareData())
        return GetElementRareData()->IsValue();
    return g_null_atom;
}

AtomicString Element::LowercaseIfNecessary(const AtomicString &name) const
{
    return name.LowerASCII();
}

String Element::nodeName(void) const
{
    return m_tagName.ToString();
}

void Element::ParseAttribute(const AttributeModificationParams &params)
{
#ifndef BLINKIT_CRAWLER_ONLY
    ASSERT(false); // BKTODO:
#endif
}

void Element::ParserSetAttributes(const Vector<Attribute> &attributeVector)
{
    ASSERT(!isConnected());
    ASSERT(nullptr == parentNode());
    ASSERT(!m_elementData);

    if (!attributeVector.IsEmpty())
    {
        if (ElementDataCache *elementDataCache = GetDocument().GetElementDataCache())
            m_elementData = elementDataCache->CachedShareableElementDataWithAttributes(attributeVector);
        else
            ASSERT(false); // BKTODO: m_elementData = ShareableElementData::CreateWithAttributes(attributeVector);
    }

    ParserDidSetAttributes();

    for (const auto &attribute : attributeVector)
    {
        AttributeModificationParams params(attribute.GetName(), g_null_atom, attribute.Value(), AttributeModificationReason::kByParser);
        AttributeChanged(params);
    }
}

void Element::setAttribute(const QualifiedName &name, const AtomicString &value)
{
    ASSERT(false); // BKTODO:
}

void Element::setAttribute(const AtomicString &localName, const AtomicString &value, ExceptionState &exceptionState)
{
    if (!Document::IsValidName(localName))
    {
        exceptionState.ThrowDOMException(DOMExceptionCode::kInvalidCharacterError,
            "'" + localName + "' is not a valid attribute name.");
        return;
    }

    SynchronizeAttribute(localName);
    AtomicString caseAdjustedLocalName = LowercaseIfNecessary(localName);

    if (nullptr == GetElementData())
    {
        QualifiedName qName(g_null_atom, caseAdjustedLocalName, g_null_atom);
        SetAttributeInternal(kNotFound, qName, value, kNotInSynchronizationOfLazyAttribute);
        return;
    }

    AttributeCollection attributes = GetElementData()->Attributes();
    wtf_size_t index = attributes.FindIndex(caseAdjustedLocalName);
    const QualifiedName qName = index != kNotFound
        ? attributes[index].GetName()
        : QualifiedName(g_null_atom, caseAdjustedLocalName, g_null_atom);
    SetAttributeInternal(index, qName, value, kNotInSynchronizationOfLazyAttribute);
}

void Element::SetAttributeInternal(
    wtf_size_t index,
    const QualifiedName &name, const AtomicString &newValue,
    SynchronizationOfLazyAttribute inSynchronizationOfLazyAttribute)
{
    if (newValue.IsNull())
    {
        ASSERT(false); // BKTODO:
#if 0
        if (index != kNotFound)
            RemoveAttributeInternal(index, inSynchronizationOfLazyAttribute);
#endif
        return;
    }

    if (index == kNotFound)
    {
        AppendAttributeInternal(name, newValue, inSynchronizationOfLazyAttribute);
        return;
    }

    const Attribute &existingAttribute = GetElementData()->Attributes().at(index);
    AtomicString existingAttributeValue = existingAttribute.Value();
    QualifiedName existingAttributeName = existingAttribute.GetName();

    if (!inSynchronizationOfLazyAttribute)
        WillModifyAttribute(existingAttributeName, existingAttributeValue, newValue);
    if (newValue != existingAttributeValue)
        EnsureUniqueElementData().Attributes().at(index).SetValue(newValue);
    if (!inSynchronizationOfLazyAttribute)
        DidModifyAttribute(existingAttributeName, existingAttributeValue, newValue);
}

void Element::SetElementFlag(ElementFlags mask, bool value)
{
    if (!HasRareData() && !value)
        return;
    EnsureElementRareData().SetElementFlag(mask, value);
}

void Element::setInnerHTML(const String &html, ExceptionState &exceptionState)
{
    if (html.IsEmpty() && !HasNonInBodyInsertionMode())
    {
        setTextContent(html);
    }
    else
    {
        DocumentFragment *fragment = CreateFragmentForInnerOuterHTML(html, this, kAllowScriptingContent, "innerHTML", exceptionState);
        if (nullptr != fragment)
        {
            ContainerNode *container = this;
            ASSERT(!HasTagName(html_names::kTemplateTag)); // BKTODO:
#if 0
            if (auto* template_element = ToHTMLTemplateElementOrNull(*this))
                container = template_element->content();
#endif
            ReplaceChildrenWithFragment(container, fragment, exceptionState);
        }
    }
}

void Element::SetIsValue(const AtomicString &isValue)
{
    ASSERT(false); // BKTODO:
}

bool Element::ShouldSerializeEndTag(void) const
{
    using namespace html_names;
    // See https://www.w3.org/TR/DOM-Parsing/
    if (HasTagName(kAreaTag) || HasTagName(kBaseTag) || HasTagName(kBasefontTag)
        || HasTagName(kBgsoundTag) || HasTagName(kBrTag) || HasTagName(kColTag)
        || HasTagName(kEmbedTag) || HasTagName(kFrameTag) || HasTagName(kHrTag)
        || HasTagName(kImgTag) || HasTagName(kInputTag) || HasTagName(kKeygenTag)
        || HasTagName(kLinkTag) || HasTagName(kMetaTag) || HasTagName(kParamTag)
        || HasTagName(kSourceTag) || HasTagName(kTrackTag) || HasTagName(kWbrTag))
    {
        return false;
    }
    return true;
}

void Element::StripScriptingAttributes(Vector<Attribute> &attributeVector) const
{
    ASSERT(false); // BKTODO:
}

void Element::SynchronizeAllAttributes(void) const
{
#ifndef BLINKIT_CRAWLER_ONLY
    if (!GetElementData())
        return;
    // NOTE: AnyAttributeMatches in selector_checker.cc currently assumes that all
    // lazy attributes have a null namespace.  If that ever changes we'll need to
    // fix that code.
    if (GetElementData()->style_attribute_is_dirty_)
    {
        DCHECK(IsStyledElement());
        SynchronizeStyleAttributeInternal();
    }
#endif
}

void Element::SynchronizeAttribute(const AtomicString &localName) const
{
#ifndef BLINKIT_CRAWLER_ONLY
    // This version of synchronizeAttribute() is streamlined for the case where
    // you don't have a full QualifiedName, e.g when called from DOM API.
    if (!GetElementData())
        return;
    if (GetElementData()->style_attribute_is_dirty_ &&
        LowercaseIfNecessary(local_name) == styleAttr.LocalName()) {
        DCHECK(IsStyledElement());
        SynchronizeStyleAttributeInternal();
        return;
    }
#endif
}

void Element::SynchronizeAttribute(const QualifiedName &name) const
{
#ifndef BLINKIT_CRAWLER_ONLY
    if (!GetElementData())
        return;
    if (UNLIKELY(name == styleAttr &&
        GetElementData()->style_attribute_is_dirty_)) {
        DCHECK(IsStyledElement());
        SynchronizeStyleAttributeInternal();
        return;
    }
#endif
}

String Element::TextFromChildren(void) const
{
    Text *firstTextNode = nullptr;
    bool foundMultipleTextNodes = false;
    unsigned totalLength = 0;

    for (Node *child = firstChild(); nullptr != child; child = child->nextSibling())
    {
        if (!child->IsTextNode())
            continue;
        Text *text = ToText(child);
        if (nullptr == firstTextNode)
            firstTextNode = text;
        else
            foundMultipleTextNodes = true;
        unsigned length = text->data().length();
        if (length > std::numeric_limits<unsigned>::max() - totalLength)
            return g_empty_string;
        totalLength += length;
    }

    if (nullptr == firstTextNode)
        return g_empty_string;

    if (nullptr != firstTextNode && !foundMultipleTextNodes)
    {
        firstTextNode->Atomize();
        return firstTextNode->data();
    }

    StringBuilder content;
    content.ReserveCapacity(totalLength);
    for (Node *child = firstTextNode; nullptr != child; child = child->nextSibling())
    {
        if (!child->IsTextNode())
            continue;
        content.Append(ToText(child)->data());
    }

    ASSERT(content.length() == totalLength);
    return content.ToString();
}

void Element::UpdateId(const AtomicString &oldId, const AtomicString &newId)
{
    if (!IsInTreeScope())
        return;

    if (oldId == newId)
        return;

    UpdateId(ContainingTreeScope(), oldId, newId);
}

void Element::UpdateId(TreeScope &scope, const AtomicString &oldId, const AtomicString &newId)
{
    ASSERT(IsInTreeScope());
    ASSERT(oldId != newId);

    if (!oldId.IsEmpty())
        scope.RemoveElementById(oldId, *this);
    if (!newId.IsEmpty())
        scope.AddElementById(newId, *this);

    NamedItemType type = GetNamedItemType();
    if (NamedItemType::kNameOrId == type || NamedItemType::kNameOrIdWithName == type)
        UpdateIdNamedItemRegistration(type, oldId, newId);
}

void Element::UpdateIdNamedItemRegistration(NamedItemType type, const AtomicString &oldName, const AtomicString &newName)
{
    ASSERT(false); // BKTODO:
}

void Element::UpdateName(const AtomicString &oldName, const AtomicString &newName)
{
    if (!IsInDocumentTree())
        return;

    if (oldName == newName)
        return;

    NamedItemType type = GetNamedItemType();
    if (type != NamedItemType::kNone)
        UpdateNamedItemRegistration(type, oldName, newName);
}

void Element::UpdateNamedItemRegistration(NamedItemType type, const AtomicString &oldName, const AtomicString &newName)
{
    ASSERT(false); // BKTODO:
}

void Element::WillModifyAttribute(const QualifiedName &name, const AtomicString &oldValue, const AtomicString &newValue)
{
    if (name == html_names::kNameAttr)
        UpdateName(oldValue, newValue);

#ifndef BLINKIT_CRAWLER_ONLY
    if (GetCustomElementState() == CustomElementState::kCustom) {
        CustomElement::EnqueueAttributeChangedCallback(this, name, old_value,
            new_value);
    }

    if (old_value != new_value) {
        GetDocument().GetStyleEngine().AttributeChangedForElement(name, *this);
        if (IsUpgradedV0CustomElement()) {
            V0CustomElement::AttributeDidChange(this, name.LocalName(), old_value,
                new_value);
        }
    }
#endif

    if (MutationObserverInterestGroup *recipients = MutationObserverInterestGroup::CreateForAttributesMutation(*this, name))
        ASSERT(false); // BKTODO: recipients->EnqueueMutationRecord(MutationRecord::CreateAttributes(this, name, oldValue));
}

} // namespace blink
