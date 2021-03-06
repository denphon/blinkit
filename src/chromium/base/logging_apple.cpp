// -------------------------------------------------
// BlinKit - base Library
// -------------------------------------------------
//   File Name: logging_apple.cpp
// Description: BkLog Implementation
//      Author: Ziming Li
//     Created: 2019-08-13
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

#include "logging.h"

#include "base/strings/stringprintf.h"
#include "base/mac/scoped_cftyperef.h"

namespace BlinKit {

void BkLog(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::string s = base::StringPrintV(format, args);
    va_end(args);

    base::ScopedCFTypeRef<CFStringRef> cf = CFStringCreateWithBytesNoCopy(nullptr,
        reinterpret_cast<const UInt8 *>(s.data()), s.length(),
        kCFStringEncodingUTF8, false, kCFAllocatorNull);
    CFShow(cf);
}

} // namespace BlinKit
