/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaControlsChromium_h
#define MediaControlsChromium_h

#if ENABLE(VIDEO)

#include "MediaControls.h"

namespace WebCore {

class MediaControlsChromium : public MediaControls {
public:
    // Called from port-specific parent create function to create custom controls.
    static PassRefPtr<MediaControlsChromium> createControls(Document*);

    virtual void setMediaController(MediaControllerInterface*) OVERRIDE;

    virtual void reset() OVERRIDE;

    virtual void playbackStarted() OVERRIDE;

    void changedMute() OVERRIDE;

    virtual void updateCurrentTimeDisplay() OVERRIDE;

#if ENABLE(VIDEO_TRACK)
    void createTextTrackDisplay() OVERRIDE;

    virtual void insertTextTrackContainer(PassRefPtr<MediaControlTextTrackContainerElement>);
#endif

protected:
    explicit MediaControlsChromium(Document*);

    bool initializeControls(Document*);

private:
    MediaControlTimeRemainingDisplayElement* m_durationDisplay;
    MediaControlPanelEnclosureElement* m_enclosure;
};

}

#endif

#endif
