// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: HashTools.h
// Description: Hash Tools
//      Author: Ziming Li
//     Created: 2019-08-21
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2010 Andras Becsi <abecsi@inf.u-szeged.hu>, University of Szeged
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

#ifndef HashTools_h
#define HashTools_h

#include "wtf/Allocator.h"

namespace blink {

struct Property {
    DISALLOW_NEW();
    int nameOffset;
    int id;
};

struct Value {
    DISALLOW_NEW();
    int nameOffset;
    int id;
};

const Property* findProperty(const char* str, unsigned len);
const Value* findValue(const char* str, unsigned len);

} // namespace blink

#endif // HashTools_h
