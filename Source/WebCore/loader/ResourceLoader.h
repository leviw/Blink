/*
 * Copyright (C) 2005, 2006, 2011 Apple Inc. All rights reserved.
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

#ifndef ResourceLoader_h
#define ResourceLoader_h

#include "ResourceHandleClient.h"
#include "ResourceLoaderOptions.h"
#include "ResourceLoaderTypes.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"

#include <wtf/Forward.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class CachedResource;
class CachedResourceLoader;
class DocumentLoader;
class Frame;
class FrameLoader;
class KURL;
class ResourceBuffer;
class ResourceHandle;

class ResourceLoader : public RefCounted<ResourceLoader>, protected ResourceHandleClient {
public:
    virtual ~ResourceLoader();

    void cancel();
    void cancel(const ResourceError&);
    void cancelIfNotFinishing();

    virtual bool init(const ResourceRequest&);

    void start();

    FrameLoader* frameLoader() const;
    DocumentLoader* documentLoader() const { return m_documentLoader.get(); }
    CachedResource* cachedResource() { return m_resource; }
    const ResourceRequest& originalRequest() const { return m_originalRequest; }

    ResourceError cancelledError();
    ResourceError cannotShowURLError();
    
    virtual void setDefersLoading(bool);
    bool defersLoading() const { return m_defersLoading; }

    unsigned long identifier() const { return m_identifier; }

    virtual void releaseResources();
    const ResourceResponse& response() const;

    PassRefPtr<ResourceBuffer> resourceData();
    void clearResourceData();
    
    virtual void willSendRequest(ResourceRequest&, const ResourceResponse& redirectResponse);
    virtual void didReceiveResponse(const ResourceResponse&);
    virtual void didReceiveCachedMetadata(const char*, int) { }
    virtual void didFinishLoading(double finishTime);
    virtual void didFail(const ResourceError&);
    void didChangePriority(ResourceLoadPriority);

    virtual bool shouldUseCredentialStorage();

    // ResourceHandleClient
    virtual void willSendRequest(ResourceHandle*, ResourceRequest&, const ResourceResponse& redirectResponse) OVERRIDE;
    virtual void didSendData(ResourceHandle*, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) OVERRIDE;
    virtual void didReceiveResponse(ResourceHandle*, const ResourceResponse&) OVERRIDE;
    virtual void didReceiveData(ResourceHandle*, const char*, int, int encodedDataLength) OVERRIDE;
    virtual void didReceiveCachedMetadata(ResourceHandle*, const char* data, int length) OVERRIDE { didReceiveCachedMetadata(data, length); }
    virtual void didFinishLoading(ResourceHandle*, double finishTime) OVERRIDE;
    virtual void didFail(ResourceHandle*, const ResourceError&) OVERRIDE;
    virtual bool shouldUseCredentialStorage(ResourceHandle*) OVERRIDE { return shouldUseCredentialStorage(); }
    virtual void didDownloadData(ResourceHandle*, int) OVERRIDE;

    const KURL& url() const { return m_request.url(); } 
    ResourceHandle* handle() const { return m_handle.get(); }
    bool shouldSendResourceLoadCallbacks() const { return m_options.sendLoadCallbacks == SendCallbacks; }
    void setSendCallbackPolicy(SendCallbackPolicy sendLoadCallbacks) { m_options.sendLoadCallbacks = sendLoadCallbacks; }
    bool shouldSniffContent() const { return m_options.sniffContent == SniffContent; }

    bool reachedTerminalState() const { return m_reachedTerminalState; }

    const ResourceRequest& request() const { return m_request; }

    void setDataBufferingPolicy(DataBufferingPolicy);

    void reportMemoryUsage(MemoryObjectInfo*) const;

protected:
    ResourceLoader(Frame*, CachedResource*, ResourceLoaderOptions);

    void didFinishLoadingOnePart(double finishTime);

    bool cancelled() const { return m_cancelled; }

    void sendDataToResource(const char*, int);

    RefPtr<ResourceHandle> m_handle;
    RefPtr<Frame> m_frame;
    RefPtr<DocumentLoader> m_documentLoader;
    ResourceResponse m_response;

    void addData(const char*, int);

    ResourceRequest m_request;
    ResourceRequest m_originalRequest; // Before redirects.
    RefPtr<ResourceBuffer> m_resourceData;
    
    unsigned long m_identifier;

    bool m_loadingMultipartContent;
    bool m_reachedTerminalState;
    bool m_cancelled;
    bool m_notifiedLoadComplete;

    bool m_defersLoading;
    ResourceRequest m_deferredRequest;
    ResourceLoaderOptions m_options;

    enum ResourceLoaderState {
        Uninitialized,
        Initialized,
        Finishing
    };

    class RequestCountTracker {
    public:
        RequestCountTracker(CachedResourceLoader*, CachedResource*);
        ~RequestCountTracker();
    private:
        CachedResourceLoader* m_cachedResourceLoader;
        CachedResource* m_resource;
    };

    CachedResource* m_resource;
    ResourceLoaderState m_state;
    OwnPtr<RequestCountTracker> m_requestCountTracker;
};

inline const ResourceResponse& ResourceLoader::response() const
{
    return m_response;
}

}

#endif
