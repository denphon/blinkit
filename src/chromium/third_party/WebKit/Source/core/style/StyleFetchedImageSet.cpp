// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: StyleFetchedImageSet.cpp
// Description: StyleFetchedImageSet Class
//      Author: Ziming Li
//     Created: 2019-03-31
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/style/StyleFetchedImageSet.h"

#include "core/css/CSSImageSetValue.h"
#include "core/fetch/ImageResource.h"
#include "core/layout/LayoutObject.h"
#include "core/svg/graphics/SVGImageForContainer.h"

namespace blink {

StyleFetchedImageSet::StyleFetchedImageSet(ImageResource* image, float imageScaleFactor, CSSImageSetValue* value, const KURL& url)
    : m_bestFitImage(image)
    , m_imageScaleFactor(imageScaleFactor)
    , m_imageSetValue(value)
    , m_url(url)
{
    m_isImageResourceSet = true;
    m_bestFitImage->addClient(this);
#if ENABLE(OILPAN)
    ThreadState::current()->registerPreFinalizer(this);
#endif
}

StyleFetchedImageSet::~StyleFetchedImageSet()
{
#if !ENABLE(OILPAN)
    dispose();
#endif
}

void StyleFetchedImageSet::dispose()
{
    m_bestFitImage->removeClient(this);
}

WrappedImagePtr StyleFetchedImageSet::data() const
{
    return m_bestFitImage.get();
}

ImageResource* StyleFetchedImageSet::cachedImage() const
{
    return m_bestFitImage.get();
}

PassRefPtrWillBeRawPtr<CSSValue> StyleFetchedImageSet::cssValue() const
{
    return m_imageSetValue;
}

PassRefPtrWillBeRawPtr<CSSValue> StyleFetchedImageSet::computedCSSValue() const
{
    return m_imageSetValue->valueWithURLsMadeAbsolute();
}

bool StyleFetchedImageSet::canRender() const
{
    return m_bestFitImage->canRender();
}

bool StyleFetchedImageSet::isLoaded() const
{
    return m_bestFitImage->isLoaded();
}

bool StyleFetchedImageSet::errorOccurred() const
{
    return m_bestFitImage->errorOccurred();
}

LayoutSize StyleFetchedImageSet::imageSize(const LayoutObject* layoutObject, float multiplier) const
{
    LayoutSize scaledImageSize = m_bestFitImage->imageSize(LayoutObject::shouldRespectImageOrientation(layoutObject), multiplier);
    scaledImageSize.scale(1 / m_imageScaleFactor);
    return scaledImageSize;
}

bool StyleFetchedImageSet::imageHasRelativeWidth() const
{
    return m_bestFitImage->imageHasRelativeWidth();
}

bool StyleFetchedImageSet::imageHasRelativeHeight() const
{
    return m_bestFitImage->imageHasRelativeHeight();
}

void StyleFetchedImageSet::computeIntrinsicDimensions(const LayoutObject*, Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio)
{
    m_bestFitImage->computeIntrinsicDimensions(intrinsicWidth, intrinsicHeight, intrinsicRatio);
}

bool StyleFetchedImageSet::usesImageContainerSize() const
{
    return m_bestFitImage->usesImageContainerSize();
}

void StyleFetchedImageSet::addClient(LayoutObject* layoutObject)
{
    m_bestFitImage->addClient(layoutObject);
}

void StyleFetchedImageSet::removeClient(LayoutObject* layoutObject)
{
    m_bestFitImage->removeClient(layoutObject);
}

PassRefPtr<Image> StyleFetchedImageSet::image(const LayoutObject*, const IntSize& containerSize, float zoom) const
{
#ifdef BLINKIT_CRAWLER_ONLY
    assert(false); // BKTODO: Not reached!
    return nullptr;
#else
    if (!m_bestFitImage->image()->isSVGImage())
        return m_bestFitImage->image();

    return SVGImageForContainer::create(toSVGImage(m_bestFitImage->image()), containerSize, zoom, m_url);
#endif
}

bool StyleFetchedImageSet::knownToBeOpaque(const LayoutObject* layoutObject) const
{
    TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "PaintImage", "data", InspectorPaintImageEvent::data(layoutObject, *m_bestFitImage.get()));
    return m_bestFitImage->image()->currentFrameKnownToBeOpaque(Image::PreCacheMetadata);
}

DEFINE_TRACE(StyleFetchedImageSet)
{
    visitor->trace(m_imageSetValue);
    StyleImage::trace(visitor);
}

} // namespace blink
