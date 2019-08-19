// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: unicode_apple.mm
// Description: Unicode Helpers
//      Author: Ziming Li
//     Created: 2019-08-13
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#include "wtf/text/Unicode.h"

namespace WTF {
namespace Unicode {

DecompositionType decompositionType(UChar32 c)
{
    assert(false); // BKTODO:
    return DecompositionNone;
}

int foldCase(UChar *result, int resultLength, const UChar *src, int srcLength, bool *error)
{
    assert(false); // BKTODO:
    return 0;
}

UChar32 foldCase(UChar32 c)
{
    return 0;
}

UChar32 mirroredChar(UChar32 c)
{
    assert(false); // BKTODO:
    return 0;
}

int toLower(UChar *result, int resultLength, const UChar *src, int srcLength, bool *error)
{
    assert(false); // BKTODO:
    return 0;
}

UChar32 toLower(UChar32 c)
{
    assert(false); // BKTODO:
    return 0;
}

UChar32 toTitleCase(UChar32 c)
{
    assert(false); // BKTODO:
    return 0;
}

int toUpper(UChar *result, int resultLength, const UChar *src, int srcLength, bool *error)
{
    assert(false); // BKTODO:
    return 0;
}

UChar32 toUpper(UChar32 c)
{
    assert(false); // BKTODO:
    return 0;
}

int umemcasecmp(const UChar *a, const UChar *b, int len)
{
    assert(false); // BKTODO:
    return 0;
}

} // namespace Unicode
} // namespace WTF