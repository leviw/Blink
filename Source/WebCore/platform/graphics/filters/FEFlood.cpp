/*
 * Copyright (C) 2004, 2005, 2006, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
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

#include "config.h"

#include "FEFlood.h"

#include "Filter.h"
#include "GraphicsContext.h"
#include "RenderTreeAsText.h"
#include "SkFlattenableBuffers.h"
#include "SkImageFilter.h"
#include "TextStream.h"

namespace {

class FloodImageFilter : public SkImageFilter {
public:
    FloodImageFilter(const SkColor& color)
        : SkImageFilter(0, 0)
        , m_color(color)
    {
    }
    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(FloodImageFilter)

    FloodImageFilter(SkFlattenableReadBuffer& buffer) : SkImageFilter(buffer)
    {
        m_color = buffer.readColor();
    }

    virtual void flatten(SkFlattenableWriteBuffer& buffer) const
    {
        buffer.writeColor(m_color);
    }

    virtual bool onFilterImage(Proxy* proxy, const SkBitmap& src, const SkMatrix&, SkBitmap* result, SkIPoint*)
    {
        if (!src.width() || !src.height())
            return false;

        SkAutoTUnref<SkDevice> device(proxy->createDevice(src.width(), src.height()));
        SkCanvas canvas(device.get());
        SkPaint paint;
        paint.setColor(m_color);
        canvas.drawRect(SkRect::MakeWH(src.width(), src.height()), paint);
        *result = device->accessBitmap(false);
        return true;
    }
private:
    SkColor m_color;
};

}; // unnamed namespace

namespace WebCore {

FEFlood::FEFlood(Filter* filter, const Color& floodColor, float floodOpacity)
    : FilterEffect(filter)
    , m_floodColor(floodColor)
    , m_floodOpacity(floodOpacity)
{
}

PassRefPtr<FEFlood> FEFlood::create(Filter* filter, const Color& floodColor, float floodOpacity)
{
    return adoptRef(new FEFlood(filter, floodColor, floodOpacity));
}

Color FEFlood::floodColor() const
{
    return m_floodColor;
}

bool FEFlood::setFloodColor(const Color& color)
{
    if (m_floodColor == color)
        return false;
    m_floodColor = color;
    return true;
}

float FEFlood::floodOpacity() const
{
    return m_floodOpacity;
}

bool FEFlood::setFloodOpacity(float floodOpacity)
{
    if (m_floodOpacity == floodOpacity)
        return false;
    m_floodOpacity = floodOpacity;
    return true;
}

void FEFlood::platformApplySoftware()
{
    ImageBuffer* resultImage = createImageBufferResult();
    if (!resultImage)
        return;

    Color color = colorWithOverrideAlpha(floodColor().rgb(), floodOpacity());
    resultImage->context()->fillRect(FloatRect(FloatPoint(), absolutePaintRect().size()), color, ColorSpaceDeviceRGB);
}

SkImageFilter* FEFlood::createImageFilter(SkiaImageFilterBuilder* builder)
{
    Color color = colorWithOverrideAlpha(floodColor().rgb(), floodOpacity());
    return new FloodImageFilter(color.rgb());
}

void FEFlood::dump()
{
}

TextStream& FEFlood::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feFlood";
    FilterEffect::externalRepresentation(ts);
    ts << " flood-color=\"" << floodColor().nameForRenderTreeAsText() << "\" "
       << "flood-opacity=\"" << floodOpacity() << "\"]\n";
    return ts;
}

} // namespace WebCore
