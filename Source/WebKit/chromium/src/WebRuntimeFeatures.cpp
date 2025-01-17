/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "WebRuntimeFeatures.h"

#include "DatabaseManager.h"
#include "RuntimeEnabledFeatures.h"
#include "WebMediaPlayerClientImpl.h"
#include "modules/websockets/WebSocket.h"

#include <wtf/UnusedParam.h>

using namespace WebCore;

namespace WebKit {

// FIXME: Remove native validation message things when we finish implementations
// of all platforms.
static bool nativeValidationMessageEnabled = false;

void WebRuntimeFeatures::enableNativeValidationMessage(bool enable)
{
    nativeValidationMessageEnabled = enable;
}

bool WebRuntimeFeatures::isNativeValidationMessageEnabled()
{
    return nativeValidationMessageEnabled;
}

void WebRuntimeFeatures::enableDatabase(bool enable)
{
    DatabaseManager::manager().setIsAvailable(enable);
}

bool WebRuntimeFeatures::isDatabaseEnabled()
{
    return DatabaseManager::manager().isAvailable();
}

// FIXME: Remove the ability to enable this feature at runtime.
void WebRuntimeFeatures::enableLocalStorage(bool enable)
{
    RuntimeEnabledFeatures::setLocalStorageEnabled(enable);
}

// FIXME: Remove the ability to enable this feature at runtime.
bool WebRuntimeFeatures::isLocalStorageEnabled()
{
    return RuntimeEnabledFeatures::localStorageEnabled();
}

// FIXME: Remove the ability to enable this feature at runtime.
void WebRuntimeFeatures::enableSessionStorage(bool enable)
{
    RuntimeEnabledFeatures::setSessionStorageEnabled(enable);
}

// FIXME: Remove the ability to enable this feature at runtime.
bool WebRuntimeFeatures::isSessionStorageEnabled()
{
    return RuntimeEnabledFeatures::sessionStorageEnabled();
}

void WebRuntimeFeatures::enableMediaPlayer(bool enable)
{
#if ENABLE(VIDEO)
    WebMediaPlayerClientImpl::setIsEnabled(enable);
#endif
}

bool WebRuntimeFeatures::isMediaPlayerEnabled()
{
#if ENABLE(VIDEO)
    return WebMediaPlayerClientImpl::isEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableNotifications(bool enable)
{
#if ENABLE(NOTIFICATIONS) || ENABLE(LEGACY_NOTIFICATIONS)
    RuntimeEnabledFeatures::setWebkitNotificationsEnabled(enable);
#endif
}

bool WebRuntimeFeatures::isNotificationsEnabled()
{
#if ENABLE(NOTIFICATIONS) || ENABLE(LEGACY_NOTIFICATIONS)
    return RuntimeEnabledFeatures::webkitNotificationsEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableApplicationCache(bool enable)
{
    RuntimeEnabledFeatures::setApplicationCacheEnabled(enable);
}

bool WebRuntimeFeatures::isApplicationCacheEnabled()
{
    return RuntimeEnabledFeatures::applicationCacheEnabled();
}

void WebRuntimeFeatures::enableDataTransferItems(bool enable)
{
    // FIXME: This is in the process of being removed.
}

bool WebRuntimeFeatures::isDataTransferItemsEnabled()
{
#if ENABLE(DATA_TRANSFER_ITEMS)
    return RuntimeEnabledFeatures::dataTransferItemsEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableGeolocation(bool enable)
{
    RuntimeEnabledFeatures::setGeolocationEnabled(enable);
}

bool WebRuntimeFeatures::isGeolocationEnabled()
{
    return RuntimeEnabledFeatures::geolocationEnabled();
}

void WebRuntimeFeatures::enableIndexedDatabase(bool enable)
{
    RuntimeEnabledFeatures::setWebkitIndexedDBEnabled(enable);
}

bool WebRuntimeFeatures::isIndexedDatabaseEnabled()
{
    return RuntimeEnabledFeatures::webkitIndexedDBEnabled();
}

void WebRuntimeFeatures::enableWebAudio(bool enable)
{
#if ENABLE(WEB_AUDIO)
    RuntimeEnabledFeatures::setWebkitAudioContextEnabled(enable);
#endif
}

bool WebRuntimeFeatures::isWebAudioEnabled()
{
#if ENABLE(WEB_AUDIO)
    return RuntimeEnabledFeatures::webkitAudioContextEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableTouch(bool enable)
{
#if ENABLE(TOUCH_EVENTS)
    RuntimeEnabledFeatures::setTouchEnabled(enable);
#endif
}

bool WebRuntimeFeatures::isTouchEnabled()
{
#if ENABLE(TOUCH_EVENTS)
    return RuntimeEnabledFeatures::touchEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableDeviceMotion(bool enable)
{
    RuntimeEnabledFeatures::setDeviceMotionEnabled(enable);
}

bool WebRuntimeFeatures::isDeviceMotionEnabled()
{
    return RuntimeEnabledFeatures::deviceMotionEnabled();
}

void WebRuntimeFeatures::enableDeviceOrientation(bool enable)
{
    RuntimeEnabledFeatures::setDeviceOrientationEnabled(enable);
}

bool WebRuntimeFeatures::isDeviceOrientationEnabled()
{
    return RuntimeEnabledFeatures::deviceOrientationEnabled();
}

void WebRuntimeFeatures::enableSpeechInput(bool enable)
{
    RuntimeEnabledFeatures::setSpeechInputEnabled(enable);
}

bool WebRuntimeFeatures::isSpeechInputEnabled()
{
    return RuntimeEnabledFeatures::speechInputEnabled();
}

void WebRuntimeFeatures::enableScriptedSpeech(bool enable)
{
#if ENABLE(SCRIPTED_SPEECH)
    RuntimeEnabledFeatures::setScriptedSpeechEnabled(enable);
#endif
}

bool WebRuntimeFeatures::isScriptedSpeechEnabled()
{
#if ENABLE(SCRIPTED_SPEECH)
    return RuntimeEnabledFeatures::scriptedSpeechEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableXHRResponseBlob(bool enable)
{
}

bool WebRuntimeFeatures::isXHRResponseBlobEnabled()
{
    return true;
}

void WebRuntimeFeatures::enableFileSystem(bool enable)
{
    RuntimeEnabledFeatures::setFileSystemEnabled(enable);
}

bool WebRuntimeFeatures::isFileSystemEnabled()
{
    return RuntimeEnabledFeatures::fileSystemEnabled();
}

void WebRuntimeFeatures::enableJavaScriptI18NAPI(bool enable)
{
#if ENABLE(JAVASCRIPT_I18N_API)
    RuntimeEnabledFeatures::setJavaScriptI18NAPIEnabled(enable);
#endif
}

bool WebRuntimeFeatures::isJavaScriptI18NAPIEnabled()
{
#if ENABLE(JAVASCRIPT_I18N_API)
    return RuntimeEnabledFeatures::javaScriptI18NAPIEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableQuota(bool enable)
{
    RuntimeEnabledFeatures::setQuotaEnabled(enable);
}

bool WebRuntimeFeatures::isQuotaEnabled()
{
    return RuntimeEnabledFeatures::quotaEnabled();
}

void WebRuntimeFeatures::enableMediaStream(bool enable)
{
#if ENABLE(MEDIA_STREAM)
    RuntimeEnabledFeatures::setMediaStreamEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isMediaStreamEnabled()
{
#if ENABLE(MEDIA_STREAM)
    return RuntimeEnabledFeatures::mediaStreamEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enablePeerConnection(bool enable)
{
#if ENABLE(MEDIA_STREAM)
    RuntimeEnabledFeatures::setPeerConnectionEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isPeerConnectionEnabled()
{
#if ENABLE(MEDIA_STREAM)
    return RuntimeEnabledFeatures::peerConnectionEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableFullScreenAPI(bool enable)
{
    RuntimeEnabledFeatures::setWebkitFullScreenAPIEnabled(enable);
}

bool WebRuntimeFeatures::isFullScreenAPIEnabled()
{
    return RuntimeEnabledFeatures::webkitFullScreenAPIEnabled();
}

void WebRuntimeFeatures::enableMediaSource(bool enable)
{
    RuntimeEnabledFeatures::setMediaSourceEnabled(enable);
}

bool WebRuntimeFeatures::isMediaSourceEnabled()
{
    return RuntimeEnabledFeatures::mediaSourceEnabled();
}

void WebRuntimeFeatures::enableEncryptedMedia(bool enable)
{
#if ENABLE(ENCRYPTED_MEDIA)
    RuntimeEnabledFeatures::setEncryptedMediaEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isEncryptedMediaEnabled()
{
#if ENABLE(ENCRYPTED_MEDIA)
    return RuntimeEnabledFeatures::encryptedMediaEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableVideoTrack(bool enable)
{
#if ENABLE(VIDEO_TRACK)
    RuntimeEnabledFeatures::setWebkitVideoTrackEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isVideoTrackEnabled()
{
#if ENABLE(VIDEO_TRACK)
    return RuntimeEnabledFeatures::webkitVideoTrackEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableGamepad(bool enable)
{
#if ENABLE(GAMEPAD)
    RuntimeEnabledFeatures::setWebkitGetGamepadsEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isGamepadEnabled()
{
#if ENABLE(GAMEPAD)
    return RuntimeEnabledFeatures::webkitGetGamepadsEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableShadowDOM(bool enable)
{
    RuntimeEnabledFeatures::setShadowDOMEnabled(enable);
}

bool WebRuntimeFeatures::isShadowDOMEnabled()
{
    return RuntimeEnabledFeatures::shadowDOMEnabled();
}

void WebRuntimeFeatures::enableCustomDOMElements(bool enable)
{
#if ENABLE(CUSTOM_ELEMENTS)
    RuntimeEnabledFeatures::setCustomDOMElements(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isCustomDOMElementsEnabled()
{
#if ENABLE(CUSTOM_ELEMENTS)
    return RuntimeEnabledFeatures::customDOMElementsEnabled();
#else
    return false;
#endif
}


void WebRuntimeFeatures::enableStyleScoped(bool enable)
{
    RuntimeEnabledFeatures::setStyleScopedEnabled(enable);
}

bool WebRuntimeFeatures::isStyleScopedEnabled()
{
    return RuntimeEnabledFeatures::styleScopedEnabled();
}

void WebRuntimeFeatures::enableInputTypeDateTime(bool enable)
{
#if ENABLE(INPUT_TYPE_DATETIME_INCOMPLETE)
    RuntimeEnabledFeatures::setInputTypeDateTimeEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isInputTypeDateTimeEnabled()
{
#if ENABLE(INPUT_TYPE_DATETIME_INCOMPLETE)
    return RuntimeEnabledFeatures::inputTypeDateTimeEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableInputTypeWeek(bool enable)
{
    RuntimeEnabledFeatures::setInputTypeWeekEnabled(enable);
}

bool WebRuntimeFeatures::isInputTypeWeekEnabled()
{
    return RuntimeEnabledFeatures::inputTypeWeekEnabled();
}

void WebRuntimeFeatures::enableDialogElement(bool enable)
{
#if ENABLE(DIALOG_ELEMENT)
    RuntimeEnabledFeatures::setDialogElementEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isDialogElementEnabled()
{
#if ENABLE(DIALOG_ELEMENT)
    return RuntimeEnabledFeatures::dialogElementEnabled();
#else
    return false;
#endif
}

void WebRuntimeFeatures::enableExperimentalContentSecurityPolicyFeatures(bool enable)
{
    RuntimeEnabledFeatures::setExperimentalContentSecurityPolicyFeaturesEnabled(enable);
}

bool WebRuntimeFeatures::isExperimentalContentSecurityPolicyFeaturesEnabled()
{
    return RuntimeEnabledFeatures::experimentalContentSecurityPolicyFeaturesEnabled();
}

void WebRuntimeFeatures::enableSeamlessIFrames(bool enable)
{
    return RuntimeEnabledFeatures::setSeamlessIFramesEnabled(enable);
}

bool WebRuntimeFeatures::areSeamlessIFramesEnabled()
{
    return RuntimeEnabledFeatures::seamlessIFramesEnabled();
}

void WebRuntimeFeatures::enableCanvasPath(bool enable)
{
    RuntimeEnabledFeatures::setCanvasPathEnabled(enable);
}

bool WebRuntimeFeatures::isCanvasPathEnabled()
{
    return RuntimeEnabledFeatures::canvasPathEnabled();
}

void WebRuntimeFeatures::enableCSSExclusions(bool enable)
{
    RuntimeEnabledFeatures::setCSSExclusionsEnabled(enable);
}

bool WebRuntimeFeatures::isCSSExclusionsEnabled()
{
    return RuntimeEnabledFeatures::cssExclusionsEnabled();
}

void WebRuntimeFeatures::enableCSSRegions(bool enable)
{
    RuntimeEnabledFeatures::setCSSRegionsEnabled(enable);
}

bool WebRuntimeFeatures::isCSSRegionsEnabled()
{
    return RuntimeEnabledFeatures::cssRegionsEnabled();
}

void WebRuntimeFeatures::enableCSSCompositing(bool enable)
{
    RuntimeEnabledFeatures::setCSSCompositingEnabled(enable);
}

bool WebRuntimeFeatures::isCSSCompositingEnabled()
{
    return RuntimeEnabledFeatures::cssCompositingEnabled();
}

void WebRuntimeFeatures::enableFontLoadEvents(bool enable)
{
    RuntimeEnabledFeatures::setFontLoadEventsEnabled(enable);
}

bool WebRuntimeFeatures::isFontLoadEventsEnabled()
{
    return RuntimeEnabledFeatures::fontLoadEventsEnabled();
}

void WebRuntimeFeatures::enableRequestAutocomplete(bool enable)
{
    RuntimeEnabledFeatures::setRequestAutocompleteEnabled(enable);
}

bool WebRuntimeFeatures::isRequestAutocompleteEnabled()
{
    return RuntimeEnabledFeatures::requestAutocompleteEnabled();
}

void WebRuntimeFeatures::enableDoNotTrack(bool enable)
{
    RuntimeEnabledFeatures::setDoNotTrackEnabled(enable);
}

bool WebRuntimeFeatures::isDoNotTrackEnabled()
{
    return RuntimeEnabledFeatures::doNotTrackEnabled();
}

void WebRuntimeFeatures::enableWebPInAcceptHeader(bool enable)
{
#if USE(WEBP)
    RuntimeEnabledFeatures::setWebPInAcceptHeaderEnabled(enable);
#else
    UNUSED_PARAM(enable);
#endif
}

bool WebRuntimeFeatures::isWebPInAcceptHeaderEnabled()
{
#if USE(WEBP)
    return RuntimeEnabledFeatures::webPInAcceptHeaderEnabled();
#else
    return false;
#endif
}

} // namespace WebKit
