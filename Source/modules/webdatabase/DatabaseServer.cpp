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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "DatabaseServer.h"

#include "Database.h"
#include "DatabaseBackend.h"
#include "DatabaseBackendContext.h"
#include "DatabaseBackendSync.h"
#include "DatabaseSync.h"
#include "DatabaseTracker.h"
#include <wtf/UnusedParam.h>

namespace WebCore {

void DatabaseServer::initialize(const String& databasePath)
{
    UNUSED_PARAM(databasePath);
}

void DatabaseServer::setClient(DatabaseManagerClient* client)
{
    UNUSED_PARAM(client);
}

String DatabaseServer::databaseDirectoryPath() const
{
    return "";
}

void DatabaseServer::setDatabaseDirectoryPath(const String& path)
{
}

String DatabaseServer::fullPathForDatabase(SecurityOrigin* origin, const String& name, bool createIfDoesNotExist)
{
    return DatabaseTracker::tracker().fullPathForDatabase(origin, name, createIfDoesNotExist);
}

void DatabaseServer::closeDatabasesImmediately(const String& originIdentifier, const String& name)
{
    DatabaseTracker::tracker().closeDatabasesImmediately(originIdentifier, name);
}

void DatabaseServer::interruptAllDatabasesForContext(const DatabaseBackendContext* context)
{
    DatabaseTracker::tracker().interruptAllDatabasesForContext(context);
}

PassRefPtr<DatabaseBackendBase> DatabaseServer::openDatabase(RefPtr<DatabaseBackendContext>& backendContext,
    DatabaseType type, const String& name, const String& expectedVersion, const String& displayName,
    unsigned long estimatedSize, bool setVersionInNewDatabase, DatabaseError &error, String& errorMessage,
    OpenAttempt attempt)
{
    RefPtr<DatabaseBackendBase> database;
    bool success = false; // Make some older compilers happy.
    
    switch (attempt) {
    case FirstTryToOpenDatabase:
        success = DatabaseTracker::tracker().canEstablishDatabase(backendContext.get(), name, displayName, estimatedSize, error);
        break;
    case RetryOpenDatabase:
        success = DatabaseTracker::tracker().retryCanEstablishDatabase(backendContext.get(), name, displayName, estimatedSize, error);
    }

    if (success)
        database = createDatabase(backendContext, type, name, expectedVersion, displayName, estimatedSize, setVersionInNewDatabase, error, errorMessage);
    return database.release();
}

PassRefPtr<DatabaseBackendBase> DatabaseServer::createDatabase(RefPtr<DatabaseBackendContext>& backendContext,
    DatabaseType type, const String& name, const String& expectedVersion, const String& displayName,
    unsigned long estimatedSize, bool setVersionInNewDatabase, DatabaseError& error, String& errorMessage)
{
    RefPtr<DatabaseBackendBase> database;
    switch (type) {
    case DatabaseType::Async:
        database = adoptRef(new Database(backendContext, name, expectedVersion, displayName, estimatedSize));
        break;
    case DatabaseType::Sync:
        database = adoptRef(new DatabaseSync(backendContext, name, expectedVersion, displayName, estimatedSize));
    }

    if (!database->openAndVerifyVersion(setVersionInNewDatabase, error, errorMessage))
        return 0;

    DatabaseTracker::tracker().setDatabaseDetails(backendContext->securityOrigin(), name, displayName, estimatedSize);
    return database.release();
}

} // namespace WebCore
