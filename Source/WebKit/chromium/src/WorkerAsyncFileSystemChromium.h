/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef WorkerAsyncFileSystemChromium_h
#define WorkerAsyncFileSystemChromium_h

#include "AsyncFileSystemChromium.h"
#include "FileSystemType.h"
#include <wtf/PassOwnPtr.h>
#include <wtf/RefPtr.h>

namespace WebKit {
class WebFileSystem;
class WebURL;
class WorkerFileSystemCallbacksBridge;
}

namespace WebCore {

class AsyncFileSystemCallbacks;
class ScriptExecutionContext;
class WorkerContext;
class WorkerLoaderProxy;

class WorkerAsyncFileSystemChromium : public AsyncFileSystemChromium {
public:
    static PassOwnPtr<AsyncFileSystem> create(ScriptExecutionContext* context, FileSystemSynchronousType synchronousType)
    {
        return adoptPtr(new WorkerAsyncFileSystemChromium(context, synchronousType));
    }

    virtual ~WorkerAsyncFileSystemChromium();

    // Runs one pending operation (to wait for completion in the sync-mode).
    virtual bool waitForOperationToComplete();

    virtual void move(const KURL& sourcePath, const KURL& destinationPath, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void copy(const KURL& sourcePath, const KURL& destinationPath, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void remove(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void removeRecursively(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void readMetadata(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void createFile(const KURL& path, bool exclusive, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void createDirectory(const KURL& path, bool exclusive, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void fileExists(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void directoryExists(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void readDirectory(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void createWriter(AsyncFileWriterClient*, const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);
    virtual void createSnapshotFileAndReadMetadata(const KURL& path, PassOwnPtr<AsyncFileSystemCallbacks>);

private:
    WorkerAsyncFileSystemChromium(ScriptExecutionContext*, FileSystemSynchronousType);

    PassRefPtr<WebKit::WorkerFileSystemCallbacksBridge> createWorkerFileSystemCallbacksBridge(PassOwnPtr<AsyncFileSystemCallbacks>);

    ScriptExecutionContext* m_scriptExecutionContext;
    WorkerLoaderProxy* m_workerLoaderProxy;
    WorkerContext* m_workerContext;
    RefPtr<WebKit::WorkerFileSystemCallbacksBridge> m_bridgeForCurrentOperation;
    String m_modeForCurrentOperation;
    FileSystemSynchronousType m_synchronousType;
};

} // namespace WebCore

#endif // WorkerAsyncFileSystemChromium_h
