/*
 * Copyright (C) 2007 Apple, Inc.  All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
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
#include "DebuggerClient.h"

#include "DebuggerDocument.h"
#include "ServerConnection.h"

#include <WebKit/IWebView.h>
#include <JavaScriptCore/JSContextRef.h>

DebuggerClient::DebuggerClient()
    : m_webViewLoaded(false)
    , m_debuggerDocument(new DebuggerDocument(new ServerConnection()))
    , m_globalContext(0)
{
}

DebuggerClient::~DebuggerClient()
{
    if (m_globalContext)
        JSGlobalContextRelease(m_globalContext);
}

// IUnknown ------------------------------
HRESULT STDMETHODCALLTYPE DebuggerClient::QueryInterface(REFIID riid, void** ppvObject)
{
    *ppvObject = 0;
    if (IsEqualGUID(riid, IID_IUnknown))
        *ppvObject = this;
    else if (IsEqualGUID(riid, IID_IWebFrameLoadDelegate))
        *ppvObject = static_cast<IWebFrameLoadDelegate*>(this);
    else if (IsEqualGUID(riid, IID_IWebUIDelegate))
        *ppvObject = static_cast<IWebUIDelegate*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE DebuggerClient::AddRef()
{
    // COM ref-counting isn't useful to us because we're in charge of the lifetime of the WebView.
    return 1;
}

ULONG STDMETHODCALLTYPE DebuggerClient::Release()
{
    // COM ref-counting isn't useful to us because we're in charge of the lifetime of the WebView.
    return 1;
}

// IWebFrameLoadDelegate ------------------------------
HRESULT STDMETHODCALLTYPE DebuggerClient::didFinishLoadForFrame(
    /* [in] */ IWebView* webView,
    /* [in] */ IWebFrame*)
{
    HRESULT ret = S_OK;

    m_webViewLoaded = true;

    COMPtr<IWebFrame> mainFrame;
    ret = webView->mainFrame(&mainFrame);
    if (FAILED(ret))
        return ret;

    if (!m_globalContext) {
        JSGlobalContextRef context = mainFrame->globalContext();
        if (!context)
            return E_FAIL;

        m_globalContext = JSGlobalContextRetain(context);
    }

    if (serverConnected())
        m_debuggerDocument->server()->setGlobalContext(m_globalContext);

    return ret;
}

HRESULT STDMETHODCALLTYPE DebuggerClient::windowScriptObjectAvailable( 
    /* [in] */ IWebView*,
    /* [in] */ JSContextRef context,
    /* [in] */ JSObjectRef windowObject)
{
    JSValueRef exception = 0;
    if (m_debuggerDocument)
        m_debuggerDocument->windowScriptObjectAvailable(context, windowObject, &exception);

    if (exception)
        return E_FAIL;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE DebuggerClient::didReceiveTitle(
    /* [in] */ IWebView*,
    /* [in] */ BSTR /* title */,
    /* [in] */ IWebFrame*)
{
    // FIXME: Set the title of Drosera's window to "[server name] - [title]"
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DebuggerClient::createWebViewWithRequest( 
        /* [in] */ IWebView*,
        /* [in] */ IWebURLRequest*,
        /* [retval][out] */ IWebView**)
{
    // FIXME: Opens the console window, this might get replaced by some Windows Fu
    return S_OK;
}


// IWebUIDelegate ------------------------------
HRESULT STDMETHODCALLTYPE DebuggerClient::runJavaScriptAlertPanelWithMessage(  // For debugging purposes
    /* [in] */ IWebView*,
    /* [in] */ BSTR message)
{
#ifndef NDEBUG
    fwprintf(stderr, L"%s\n", message ? message : L"");
#else
    (void)message;
#endif
    return S_OK;
}

bool DebuggerClient::serverConnected() const
{
    return m_debuggerDocument->server()->serverConnected();
}

void DebuggerClient::attemptToCreateServerConnection()
{
    m_debuggerDocument->server()->attemptToCreateServerConnection(m_globalContext);
}
