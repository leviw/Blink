/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FEMorphology.h"

#include "SkMorphologyImageFilter.h"
#include "SkiaImageFilterBuilder.h"

namespace WebCore {

bool FEMorphology::platformApplySkia()
{
    ImageBuffer* resultImage = createImageBufferResult();
    if (!resultImage)
        return false;

    FilterEffect* in = inputEffect(0);

    IntRect drawingRegion = drawingRegionOfInputImage(in->absolutePaintRect());

    setIsAlphaImage(in->isAlphaImage());

    float radiusX = filter()->applyHorizontalScale(m_radiusX);
    float radiusY = filter()->applyVerticalScale(m_radiusY);
    
    RefPtr<Image> image = in->asImageBuffer()->copyImage(DontCopyBackingStore);

    SkPaint paint;
    GraphicsContext* dstContext = resultImage->context();
    PlatformContextSkia* platformContext = dstContext->platformContext();
    if (m_type == FEMORPHOLOGY_OPERATOR_DILATE)
        paint.setImageFilter(new SkDilateImageFilter(radiusX, radiusY))->unref();
    else if (m_type == FEMORPHOLOGY_OPERATOR_ERODE)
        paint.setImageFilter(new SkErodeImageFilter(radiusX, radiusY))->unref();

    platformContext->saveLayer(0, &paint);
    dstContext->drawImage(image.get(), ColorSpaceDeviceRGB, drawingRegion.location(), CompositeCopy);
    platformContext->restoreLayer();
    return true;
}

SkImageFilter* FEMorphology::createImageFilter(SkiaImageFilterBuilder* builder)
{
    SkAutoTUnref<SkImageFilter> input(builder->build(inputEffect(0)));
    SkScalar radiusX = SkFloatToScalar(m_radiusX);
    SkScalar radiusY = SkFloatToScalar(m_radiusY);
    if (m_type == FEMORPHOLOGY_OPERATOR_DILATE)
        return new SkDilateImageFilter(radiusX, radiusY, input);
    return new SkErodeImageFilter(radiusX, radiusY, input);
}

};
