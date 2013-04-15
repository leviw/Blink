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

#include "config.h"
#include "RenderLazyBlock.h"

#include "LayoutRepainter.h"
#include "RenderLayer.h"
#include "RenderView.h"
#include "ClientRect.h"

namespace WebCore {

RenderLazyBlock::RenderLazyBlock(Element* element)
    : RenderBlock(element)
    , m_next(0)
    , m_previous(0)
    , m_attached(false)
{
    setChildrenInline(false); // All of our children must be block-level.
}

RenderLazyBlock::~RenderLazyBlock()
{
    ASSERT(!m_attached);
    ASSERT(!m_next && !m_previous);
}

void RenderLazyBlock::willBeDestroyed()
{
    detachLazyBlock();
    RenderBlock::willBeDestroyed();
}

void RenderLazyBlock::willBeRemovedFromTree()
{
    RenderBlock::willBeRemovedFromTree();
    detachLazyBlock();
}

void RenderLazyBlock::layoutVisibleChildrenInViewport(const IntRect& viewportRect)
{
    // FIXME: Do we need to handle continuations?
    ASSERT(!isAnonymousBlockContinuation());
    // We shouldn't be in layout
    ASSERT(view() && !view()->m_layoutState);

    IntRect expandedViewportRect = viewportRect;
    expandedViewportRect.expand(4000, 4000);

    // FIXME: We probably want a RenderGeometryMap instead since we need to handle
    // rotation of the RenderLazyBlock and other transforms.
    Vector<FloatQuad> quads;
    enclosingBoxModelObject()->absoluteQuads(quads);
    LayoutRect rect = quads[0].enclosingBoundingBox();
    if (hasOverflowClip())
        rect.move(-scrolledContentOffset());

    // FIXME: Setting the empty layout state breaks absolute coordinates so we
    // need to do it after computing our rect. We should probably get a real
    // layout state instead.
    LayoutState layoutState;
    view()->m_layoutState = &layoutState;

    m_firstVisibleChildBox = m_lastVisibleChildBox = 0;
    LayoutUnit oldHeight = logicalHeight();
    LayoutUnit heightForChild = 0;
    for (RenderBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
        LayoutUnit height = cachedChildLogicalHeight(child);
        rect.setHeight(height);
        if (expandedViewportRect.intersects(enclosingIntRect(rect))) {
            if (!m_firstVisibleChildBox)
                m_firstVisibleChildBox = child;
            m_lastVisibleChildBox = child->nextSiblingBox();
            if (child->needsLayout()) {
                setLogicalHeight(heightForChild);
                setLogicalTopForChild(child, heightForChild);
                child->layout();
                // FIXME: This ia a huge hack. Updating these layers should be way easier since we know we're normal flow
                if (child->layer()) {
                    view()->disableLayoutState();
                    child->layer()->updateLayerPositionsAfterLayout(view()->layer(), RenderLayer::defaultFlags);
                    view()->enableLayoutState();
                }
                // FIXME: Repaint child
                // FIXME: support non-fixed-height children
                ASSERT(child->logicalHeight() == height);
            }
        } else if (m_firstVisibleChildBox)
            break;
        rect.setY(rect.y() + height);
        heightForChild += height;
    }
    // FIXME: if we're guessing at our children's heights, we'll need to determine
    // the proper final value
    setLogicalHeight(oldHeight);
    view()->m_layoutState = 0;
    // FIXME: We should only have to repaint the children that were laid out
    repaint();
}

bool RenderLazyBlock::hitTestContents(const HitTestRequest& request, HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    HitTestAction childHitTest = hitTestAction;
    if (hitTestAction == HitTestChildBlockBackgrounds)
        childHitTest = HitTestChildBlockBackground;
    for (RenderBox* child = lastChildBox(); child && child != m_lastVisibleChildBox; child = child->previousSiblingBox()) {
        LayoutPoint childPoint = flipForWritingModeForChild(child, accumulatedOffset);
        if (!child->hasSelfPaintingLayer() && !child->isFloating() && child->nodeAtPoint(request, result, locationInContainer, childPoint, childHitTest))
            return true;
    }
    return false;
}

void RenderLazyBlock::paintChildren(PaintInfo& paintInfo, const LayoutPoint& paintOffset, PaintInfo& paintInfoForChild, bool usePrintRect)
{
    for (RenderBox* child = m_firstVisibleChildBox; child && child != m_lastVisibleChildBox; child = child->nextSiblingBox()) {
        if (!paintChild(child, paintInfo, paintOffset, paintInfoForChild, usePrintRect))
            return;
    }
}

// FIXME: This method and detachLazyBlock are essentially identical to
// RenderQuote::attachQuote and detachQuote. We should just have a
// RenderTreeOrderedList that does this stuff internally.
void RenderLazyBlock::attachLazyBlock()
{
    ASSERT(view());
    ASSERT(!m_attached);
    ASSERT(!m_next && !m_previous);
    ASSERT(isRooted());

    if (!view()->firstLazyBlock()) {
        view()->setFirstLazyBlock(this);
        m_attached = true;
        return;
    }

    for (RenderObject* predecessor = previousInPreOrder(); predecessor; predecessor = predecessor->previousInPreOrder()) {
        if (!predecessor->isRenderLazyBlock() || !toRenderLazyBlock(predecessor)->isAttached())
            continue;
        m_previous = toRenderLazyBlock(predecessor);
        m_next = m_previous->m_next;
        m_previous->m_next = this;
        if (m_next)
            m_next->m_previous = this;
        break;
    }

    if (!m_previous) {
        m_next = view()->firstLazyBlock();
        view()->setFirstLazyBlock(this);
        if (m_next)
            m_next->m_previous = this;
    }
    m_attached = true;

    ASSERT(!m_next || m_next->m_attached);
    ASSERT(!m_next || m_next->m_previous == this);
    ASSERT(!m_previous || m_previous->m_attached);
    ASSERT(!m_previous || m_previous->m_next == this);
}

void RenderLazyBlock::detachLazyBlock()
{
    ASSERT(!m_next || m_next->m_attached);
    ASSERT(!m_previous || m_previous->m_attached);
    if (!m_attached)
        return;
    if (m_previous)
        m_previous->m_next = m_next;
    else if (view())
        view()->setFirstLazyBlock(m_next);
    if (m_next)
        m_next->m_previous = m_previous;
    m_attached = false;
    m_next = 0;
    m_previous = 0;
}

void RenderLazyBlock::layoutBlock(bool relayoutChildren, LayoutUnit pageLogicalHeight)
{
    ASSERT(needsLayout());

    // FIXME: We should adjust the style to disallow columns too.
    ASSERT(!hasColumns());

    if (!m_attached)
        attachLazyBlock();

    LayoutRepainter repainter(*this, checkForRepaintDuringLayout());

    setLogicalHeight(0);

    if (updateLogicalWidthAndColumnWidth())
        relayoutChildren = true;

    bool pageLogicalHeightChanged = false;
    bool hasSpecifiedPageLogicalHeight = false;
    checkForPaginationLogicalHeightChange(pageLogicalHeight, pageLogicalHeightChanged, hasSpecifiedPageLogicalHeight);

    RenderView* renderView = view();
    RenderStyle* styleToUse = style();
    LayoutStateMaintainer statePusher(renderView, this, locationOffset(), hasTransform() || hasReflection() || styleToUse->isFlippedBlocksWritingMode(), pageLogicalHeight, pageLogicalHeightChanged);

    // Assert we're not in a flow thread?

    // We use four values, maxTopPos, maxTopNeg, maxBottomPos, and maxBottomNeg, to track
    // our current maximal positive and negative margins.  These values are used when we
    // are collapsed with adjacent blocks, so for example, if you have block A and B
    // collapsing together, then you'd take the maximal positive margin from both A and B
    // and subtract it from the maximal negative margin from both A and B to get the
    // true collapsed margin.  This algorithm is recursive, so when we finish layout()
    // our block knows its current maximal positive/negative values.
    //
    // Start out by setting our margin values to our current margins.
    initMaxMarginValues();
    setHasMarginBeforeQuirk(styleToUse->hasMarginBeforeQuirk());
    setHasMarginAfterQuirk(styleToUse->hasMarginAfterQuirk());
    setPaginationStrut(0);

    if (relayoutChildren)
        clearChildLogicalHeightCache();

    LayoutUnit beforeEdge = borderBefore() + paddingBefore();
    LayoutUnit afterEdge = borderAfter() + paddingAfter() + scrollbarLogicalHeight();

    setLogicalHeight(beforeEdge);

    // The margin struct caches all our current margin collapsing state.  The compact struct caches state when we encounter compacts,
    MarginInfo marginInfo(this, beforeEdge, afterEdge);

    LayoutUnit height = beforeEdge;

    for (RenderBox* child = firstChildBox(); child; child = child->nextSiblingBox()) {
        // FIXME: Enable guessing about height (will need to fix layoutVisibleChildrenInViewport)
        if (child->style()->logicalHeight().isSpecified()) {
            LogicalExtentComputedValues computedValues;
            child->computeLogicalHeight(-1, height, computedValues);
            child->setLogicalHeight(computedValues.m_extent);
            cacheChildLogicalHeight(child, computedValues.m_extent);
            height += computedValues.m_extent;
            if (relayoutChildren)
                child->setNeedsLayout(true, MarkOnlyThis);
        } else {
            setLogicalHeight(height);
            setLogicalTopForChild(child, height);
            layoutBlockChild(child, marginInfo, height, height);
            height += child->logicalHeight();
            cacheChildLogicalHeight(child, child->logicalHeight());
        }
    }

    setLogicalHeight(height + afterEdge);
 
    // Calculate our new height.
    LayoutUnit oldClientAfterEdge = clientLogicalBottom();
    updateLogicalHeight();

    // Add overflow from children (unless we're multi-column, since in that case all our child overflow is clipped anyway).
    computeOverflow(oldClientAfterEdge);
    
    statePusher.pop();

    fitBorderToLinesIfNeeded();

    if (renderView->layoutState()->m_pageLogicalHeight)
        setPageLogicalOffset(renderView->layoutState()->pageLogicalOffset(this, logicalTop()));

    updateLayerTransform();

    // Update our scroll information if we're overflow:auto/scroll/hidden now that we know if
    // we overflow or not.
    updateScrollInfoAfterLayout();

    setNeedsLayout(false);
}

} // namespace WebCore
