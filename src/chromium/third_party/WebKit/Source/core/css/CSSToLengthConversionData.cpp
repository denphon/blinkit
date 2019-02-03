/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/css/CSSToLengthConversionData.h"

#include "core/css/CSSHelper.h"
#include "core/layout/LayoutView.h"
#include "core/style/ComputedStyle.h"

namespace blink {

CSSToLengthConversionData::FontSizes::FontSizes(float em, float rem, const Font* font)
    : m_em(em)
    , m_rem(rem)
    , m_font(font)
{
    // FIXME: Improve RAII of StyleResolverState to use const Font&.
    ASSERT(m_font);
}

CSSToLengthConversionData::FontSizes::FontSizes(const ComputedStyle* style, const ComputedStyle* rootStyle)
    : FontSizes(style->computedFontSize(), rootStyle ? rootStyle->computedFontSize() : 1.0f, &style->font())
{
}

float CSSToLengthConversionData::FontSizes::ex() const
{
    ASSERT(m_font);
    // FIXME: We have a bug right now where the zoom will be applied twice to EX units.
    // We really need to compute EX using fontMetrics for the original specifiedSize and not use
    // our actual constructed layoutObject font.
    if (!m_font->fontMetrics().hasXHeight())
        return m_em / 2.0f;
    return m_font->fontMetrics().xHeight();
}

float CSSToLengthConversionData::FontSizes::ch() const
{
    ASSERT(m_font);
    return m_font->fontMetrics().zeroWidth();
}

CSSToLengthConversionData::ViewportSize::ViewportSize(const LayoutView* layoutView)
    : m_width(layoutView ? layoutView->layoutViewportWidth() : 0)
    , m_height(layoutView ? layoutView->layoutViewportHeight() : 0)
{
}

CSSToLengthConversionData::CSSToLengthConversionData(const ComputedStyle* style, const FontSizes& fontSizes, const ViewportSize& viewportSize, float zoom)
    : m_style(style)
    , m_fontSizes(fontSizes)
    , m_viewportSize(viewportSize)
    , m_zoom(clampTo<float>(zoom, std::numeric_limits<float>::denorm_min()))
{
    ASSERT(m_style);
}

CSSToLengthConversionData::CSSToLengthConversionData(const ComputedStyle* style, const ComputedStyle* rootStyle, const LayoutView* layoutView, float zoom)
    : CSSToLengthConversionData(style, FontSizes(style, rootStyle), ViewportSize(layoutView), zoom)
{
}

double CSSToLengthConversionData::viewportWidthPercent() const
{
    m_style->setHasViewportUnits();
    return m_viewportSize.width() / 100;
}
double CSSToLengthConversionData::viewportHeightPercent() const
{
    m_style->setHasViewportUnits();
    return m_viewportSize.height() / 100;
}
double CSSToLengthConversionData::viewportMinPercent() const
{
    m_style->setHasViewportUnits();
    return std::min(m_viewportSize.width(), m_viewportSize.height()) / 100;
}
double CSSToLengthConversionData::viewportMaxPercent() const
{
    m_style->setHasViewportUnits();
    return std::max(m_viewportSize.width(), m_viewportSize.height()) / 100;
}

float CSSToLengthConversionData::remFontSize() const
{
    m_style->setHasRemUnits();
    return m_fontSizes.rem();
}

double CSSToLengthConversionData::zoomedComputedPixels(double value, CSSPrimitiveValue::UnitType type) const
{
    // The logic in this function is duplicated in MediaValues::computeLength()
    // because MediaValues::computeLength() needs nearly identical logic, but we haven't found a way to make
    // zoomedComputedPixels() more generic (to solve both cases) without hurting performance.
    switch (type) {
    case CSSPrimitiveValue::UnitType::Pixels:
    case CSSPrimitiveValue::UnitType::UserUnits:
        return value * zoom();

    case CSSPrimitiveValue::UnitType::Centimeters:
        return value * cssPixelsPerCentimeter * zoom();

    case CSSPrimitiveValue::UnitType::Millimeters:
        return value * cssPixelsPerMillimeter * zoom();

    case CSSPrimitiveValue::UnitType::Inches:
        return value * cssPixelsPerInch * zoom();

    case CSSPrimitiveValue::UnitType::Points:
        return value * cssPixelsPerPoint * zoom();

    case CSSPrimitiveValue::UnitType::Picas:
        return value * cssPixelsPerPica * zoom();

    case CSSPrimitiveValue::UnitType::ViewportWidth:
        return value * viewportWidthPercent() * zoom();

    case CSSPrimitiveValue::UnitType::ViewportHeight:
        return value * viewportHeightPercent() * zoom();

    case CSSPrimitiveValue::UnitType::ViewportMin:
        return value * viewportMinPercent() * zoom();

    case CSSPrimitiveValue::UnitType::ViewportMax:
        return value * viewportMaxPercent() * zoom();

    // We do not apply the zoom factor when we are computing the value of the font-size property. The zooming
    // for font sizes is much more complicated, since we have to worry about enforcing the minimum font size preference
    // as well as enforcing the implicit "smart minimum."
    case CSSPrimitiveValue::UnitType::Ems:
    case CSSPrimitiveValue::UnitType::QuirkyEms:
        return value * emFontSize();

    case CSSPrimitiveValue::UnitType::Exs:
        return value * exFontSize();

    case CSSPrimitiveValue::UnitType::Rems:
        return value * remFontSize();

    case CSSPrimitiveValue::UnitType::Chs:
        return value * chFontSize();

    default:
        ASSERT_NOT_REACHED();
        return 0;
    }
}

} // namespace blink
