// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: document_fragment.h
// Description: DocumentFragment Class
//      Author: Ziming Li
//     Created: 2019-10-19
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_FRAGMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_FRAGMENT_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/container_node.h"
#include "third_party/blink/renderer/core/dom/parser_content_policy.h"

namespace blink {

class CORE_EXPORT DocumentFragment : public ContainerNode {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static DocumentFragment* Create(Document&);

  void ParseHTML(const String&,
                 Element* context_element,
                 ParserContentPolicy = kAllowScriptingContent);

#ifndef BLINKIT_CRAWLER_ONLY
  bool CanContainRangeEndPoint() const final { return true; }
#endif
  virtual bool IsTemplateContent() const { return false; }

 protected:
  DocumentFragment(Document*, ConstructionType = kCreateContainer);
  String nodeName() const final;

 private:
  NodeType getNodeType() const final;
  Node* Clone(Document&, CloneChildrenFlag) const override;
  bool ChildTypeAllowed(NodeType) const override;

  bool IsDocumentFragment() const =
      delete;  // This will catch anyone doing an unnecessary check.
};

DEFINE_NODE_TYPE_CASTS(DocumentFragment, IsDocumentFragment());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_FRAGMENT_H_
